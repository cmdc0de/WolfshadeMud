//////////////////////////////////////////////////////////////
//Wolfshade MUD server
//Copyright (C) 1999 Demetrius and John Comes
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////

#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"

#include <sstream>
#include "world.h"
#include <stdlib.h>
#include <ctype.h>
#include "log.h"

extern CGlobalVariableManagement GVM;
void StackObjects(const CList<CObject *> &Objs, const CCharacter *pLooker, CString &strLook, bool bInInventory);
CString CRoom::m_RoomColors[TOTAL_ROOM_TYPES];
CString CRoom::m_strBlock("\x1B[40m ");
bool CRoom::m_bInit = false;
CRoom::sAffectInfo CRoom::m_AffectInfo[TOTAL_ROOM_AFFECTS];

//room constants
const short CRoom::TINY = 0;
const short CRoom::SMALL = 1;
const short CRoom::MEDIUM = 2;
const short CRoom::LARGE = 3;
const short CRoom::HUGE = 4;
const short CRoom::GIAGANTIC = 5;
const short CRoom::ENDLESS = 6;
//ROOMTYPES //used for colors
const short CRoom::TYPE_INSIDE = 0;
const short CRoom::TYPE_FIELD = 1;
const short CRoom::TYPE_FOREST = 2;
const short CRoom::TYPE_HILLS = 3;
const short CRoom::TYPE_MOUNTAINS = 4;
const short CRoom::TYPE_WATERENTER = 5;
const short CRoom::TYPE_WATERNOENTER = 6;
const short CRoom::TYPE_UNDERWATER = 7;
const short CRoom::TYPE_UNUSED = 8;
const short CRoom::TYPE_DESERT = 9;
const short CRoom::TYPE_UNDERGROUND = 10;
const short CRoom::TYPE_SHOP = 11;
const short CRoom::TYPE_ROAD = 12;
const short CRoom::TYPE_SNOWMOUNTAINS = 13;
const short CRoom::TYPE_TUNDRA = 14;
const short CRoom::TYPE_DARKFOREST = 15;
const short CRoom::TYPE_SWAMP = 16;
const short CRoom::TYPE_BANK = 17;

//Door Types
const int CRoom::sDoor::UNRESTRICTED_DOOR = (1 << 0);
const int CRoom::sDoor::DOOR_OPEN = (1 << 1);
const int CRoom::sDoor::DOOR_CLOSED = (1 << 2);
const int CRoom::sDoor::DOOR_PICKPROFF = (1 << 3);
const int CRoom::sDoor::DOOR_SECRET = (1 << 4);
const int CRoom::sDoor::DOOR_LOCKED = (1 << 5);
const int CRoom::sDoor::DOOR_WALL = (1 << 6);

//Room Flags*/
const int CRoom::DARK = (1 << 0);
const int CRoom::NOMOB = (1 << 1);
const int CRoom::INDOORS = (1 << 2);
const int CRoom::PEACEFULL = (1 << 3);
const int CRoom::NOSOUND = (1 << 4);
const int CRoom::NOTRACK = (1 << 5);
const int CRoom::NOMAGIC = (1 << 6);
const int CRoom::NO_SCAVENGE = (1 << 7);
const int CRoom::GODROOM = (1 << 8);
const int CRoom::FASTHEAL = (1 << 9);
const int CRoom::TUNNEL = (1 << 10); //TODO
const int CRoom::INN = (1 << 11);
const int CRoom::FLYING = (1 << 12);
const int CRoom::PERM_DARK = (1 << 13);
const int CRoom::PERM_LIGHT = (1 << 14);
const int CRoom::NO_CAMP = (1 << 15);


const int CRoom::ROOM_DARK = 0;
const int CRoom::ROOM_LIT = 1;
const int CRoom::ROOM_BRIGHT = 2;
const int CRoom::ROOM_TWILIGHT = 3;

const int CRoom::VIRTUAL_DOOR_NORTH = (1 << 0);
const int CRoom::VIRTUAL_DOOR_SOUTH = (1 << 1);
const int CRoom::VIRTUAL_DOOR_EAST = (1 << 2);
const int CRoom::VIRTUAL_DOOR_WEST = (1 << 3);

const short CRoom::ROOM_AFFECT_LIGHT = 0;
const short CRoom::ROOM_AFFECT_DARK = 1;
const short CRoom::ROOM_AFFECT_NOMOB = 2;
const short CRoom::ROOM_AFFECT_PEACEFUL = 3;
const short CRoom::ROOM_AFFECT_NOTRACK = 4;
const short CRoom::ROOM_AFFECT_NO_SOUND = 5;
const short CRoom::ROOM_AFFECT_GOD_ROOM = 6;
const short CRoom::ROOM_AFFECT_FAST_HEAL = 7;
const short CRoom::ROOM_AFFECT_TUNNEL = 8;
const short CRoom::ROOM_AFFECT_INN = 9;
const short CRoom::ROOM_AFFECT_NOMAGIC = 10;
const short CRoom::ROOM_AFFECT_NO_SCAVENGE = 11;
const short CRoom::ROOM_AFFECT_FLYING = 12;
const short CRoom::ROOM_AFFECT_INDOORS = 13;
const short CRoom::ROOM_AFFECT_PERM_DARK = 14;
const short CRoom::ROOM_AFFECT_PERM_LIGHT = 15;
const short CRoom::ROOM_AFFECT_NO_CAMP = 16;

//TOTAL_ROOM_AFFECTS defined in room.h

const short CRoom::m_MovementCost[TOTAL_ROOM_TYPES] = {
   1, //[INSIDE] 
   2, //[FIELD] 
   2, //[FOREST]
   3, //[HILLS]
   4, //[MOUNTAINS]
   3, //[WATERENTER]
   0, //[WATERNOENTER] 
   5, //[UNDERWATER]
   1, //[FLYING]
   2, //[DESERT]
   3, //[UNDERGROUND]
   6, //[SNOWMOUNTAINS] 
   1, //[SHOP]
   1, //[ROAD] 
   6, //[TUNDRA]
   3, //[DARKFOREST]
   4, //[SWAMP]
};

const char *CRoom::m_strRoomType[] = {
   "inside",
   "field",
   "forest",
   "hills",
   "mountains",
   "water enter",
   "water no enter",
   "underwater",
   "flying",
   "desert",
   "underground",
   "snow mountains",
   "shop",
   "road",
   "tundra",
   "dark forest",
   "swamp"
};

const char *CRoom::m_strRoomSize[] = {
   "tiny",
   "small",
   "medium",
   "large",
   "huge",
   "giagantic",
   "endless"
};

const char *CRoom::m_strRoomBrightness[] = {
   "dark",
   "lit",
   "bright",
   "twilight"
};

const char *CRoom::strDir[] = {
   "north",
   "east",
   "south",
   "west",
   "up",
   "down"
};

/////////////////////////
//	init statics

void CRoom::InitStatics() {
   if (m_bInit)
	return;
   m_bInit = true;
   m_RoomColors[TYPE_INSIDE] = "&g.&n";
   m_RoomColors[TYPE_FIELD] = "&k &i&n";
   m_RoomColors[TYPE_FOREST] = "&k &i&n";
   m_RoomColors[TYPE_HILLS] = "&y^&n";
   m_RoomColors[TYPE_MOUNTAINS] = "&yM&n";
   m_RoomColors[TYPE_WATERENTER] = "&u &i&n";
   m_RoomColors[TYPE_WATERNOENTER] = "&u &i&n";
   m_RoomColors[TYPE_UNDERWATER] = "&u &i&n";
   m_RoomColors[TYPE_UNUSED] = "&h_&n";
   m_RoomColors[TYPE_DESERT] = "&p &i&n";
   m_RoomColors[TYPE_UNDERGROUND] = "&f &i&n";
   m_RoomColors[TYPE_SNOWMOUNTAINS] = "&WM&n";
   m_RoomColors[TYPE_SHOP] = "&Us&n";
   m_RoomColors[TYPE_ROAD] = "&L+&n";
   m_RoomColors[TYPE_TUNDRA] = "&h &i&n";
   m_RoomColors[TYPE_DARKFOREST] = "&g*&n";
   m_RoomColors[TYPE_SWAMP] = "&i &n";
   m_RoomColors[TYPE_BANK] = "&Ub&n";
   for (int i = 0; i < TOTAL_ROOM_TYPES; i++) {
	m_RoomColors[i].Colorize();
   }

#define ROOM_NULL (void (CRoom::*)(short,bool)) NULL
   m_AffectInfo[ROOM_AFFECT_LIGHT] = sAffectInfo("LIGHT", ROOM_NULL, true);
   m_AffectInfo[ROOM_AFFECT_DARK] = sAffectInfo("DARK", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_NOMOB] = sAffectInfo("NO_MOB", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_PEACEFUL] = sAffectInfo("PEACEFUL", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_NOTRACK] = sAffectInfo("NO_TRACK", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_NO_SOUND] = sAffectInfo("NO_SOUND", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_GOD_ROOM] = sAffectInfo("GOD_ROOM", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_FAST_HEAL] = sAffectInfo("FAST_HEAL", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_TUNNEL] = sAffectInfo("TUNNEL", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_INN] = sAffectInfo("INN", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_NOMAGIC] = sAffectInfo("NO_MAGIC", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_NO_SCAVENGE] = sAffectInfo("NO_SCAVENGE", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_FLYING] = sAffectInfo("FLYING", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_INDOORS] = sAffectInfo("INDOORS", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_PERM_DARK] = sAffectInfo("PERM_DARK", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_PERM_LIGHT] = sAffectInfo("PERM_LIGHT", ROOM_NULL, false);
   m_AffectInfo[ROOM_AFFECT_NO_CAMP] = sAffectInfo("NO_CAMP", ROOM_NULL, false);
}

///////////////////////////////
//	Adds affect to room

bool CRoom::AddAffect(short nAffect, int nPulses, int nValue) {
#ifdef _DEBUG
   if (nAffect > TOTAL_ROOM_AFFECTS)
	assert(0);
#endif
   if (nPulses != sAffect::PERM_AFFECT &&
	   nPulses != sAffect::MANUAL_AFFECT) {
	ErrorLog << "CODE ERROR: " << "Room affects do not support timed affects only PERM_AFFECTS and MANUAL_AFFECTS"
		<< " affect # is " << nAffect << endl;
	return false;
   }
   bool bHasAffect = m_Affects[nAffect].HasAffect();
   if (!bHasAffect) {
	m_Affects[nAffect].Set(nPulses, nValue);
	if (m_AffectInfo[nAffect].m_pAffectFnc != NULL) {
	   (this->*m_AffectInfo[nAffect].m_pAffectFnc)(nAffect, false);
	}
	return true;
   } else if (!m_Affects[nAffect].IsPermAffect()) {
	int nOldValue = m_Affects[nAffect].m_nValue;
	if (m_AffectInfo[nAffect].m_bAddValue) {
	   //remove affects for this affect
	   if (m_AffectInfo[nAffect].m_pAffectFnc != NULL) {
		(this->*m_AffectInfo[nAffect].m_pAffectFnc)(nAffect, true);
	   }
	   //reset affect struct
	   m_Affects[nAffect].Remove();
	   //reset with new affects
	   //for room affect npulses willbe manual_affect
	   m_Affects[nAffect].Set(nPulses, nOldValue + nValue);
	   //reset char affects
	   if (m_AffectInfo[nAffect].m_pAffectFnc != NULL) {
		(this->*m_AffectInfo[nAffect].m_pAffectFnc)(nAffect, false);
	   }
	   return true;
	}
   }
   return false;
}

void CRoom::ReduceAffect(short nAffect, int nValue) {
   if (!m_Affects[nAffect].IsPermAffect()
	   && m_Affects[nAffect].HasAffect()) {
	//rooms don't have timed affects so
	int nOldValue = m_Affects[nAffect].m_nValue;
	if ((nOldValue - nValue) <= 0) {
	   RemoveAffect(nAffect);
	} else {
	   //remove affects for this affect
	   if (m_AffectInfo[nAffect].m_pAffectFnc != NULL) {
		(this->*m_AffectInfo[nAffect].m_pAffectFnc)(nAffect, true);
	   }
	   //reset affect struct
	   m_Affects[nAffect].Remove();
	   //reset with new affects
	   //for room affect npulses willbe manual_affect
	   m_Affects[nAffect].Set(sAffect::MANUAL_AFFECT, nOldValue - nValue);
	   //reset char affects
	   if (m_AffectInfo[nAffect].m_pAffectFnc != NULL) {
		(this->*m_AffectInfo[nAffect].m_pAffectFnc)(nAffect, false);
	   }
	}
   }
}

//////////////////////////////
//	A Little recursion to find a character

short CRoom::LocateChar(CCharacter *pCh, CLinkList<CRoom> &UsedLL, short nDistance) {
   //add to used list
   UsedLL.Add(this);
   //add nDistnace!=0 for speed reasons
   for (int i = 0; i < NUM_OF_DOORS && nDistance != 0; i++) {
	//makes sphere
	for (int j = 0; j < nDistance; j++) {
	   if (this->DoesContain(pCh)) {
		return i;
	   } else {
		if (this->m_Doors[i] != NULL &&
			this->m_Doors[i]->m_pToRoom != NULL &&
			!UsedLL.DoesContain(this->m_Doors[i]->m_pToRoom)) {
		   //if we find them in the next room we want to return the direction
		   //this recursion is on and so on.
		   //if we just return the recursion call we would be returnin the direction
		   //from the room we are checking in NOT the own we are currently in
		   if (m_Doors[i]->m_pToRoom->LocateChar(pCh, UsedLL, nDistance - 1) != NOWHERE) {
			return i;
		   }
		}
	   }
	}
   }
   return NOWHERE;
}


////////////////////////////////////
//non statics
//Must initialize direction to NULL
//should prolly init all variables
//m_Affects nulls itself out

CRoom::CRoom() {
   InitStatics();
   m_ExtraDesc.SetDeleteData();
   for (int i = 0; i < NUM_OF_DOORS; i++)
	m_Doors[i] = NULL;
   m_nRoomSize = 0;
   m_nRoomType = 0;
   m_lVnum = -1;
   m_nZone = -1;
   m_bZoneRoom = false;
   // initalize X and y pos when we know Virtual rooms from Real rooms
   // Real rooms will still have -1
   // could also be done by if Room # < TotalVirtualrooms
   m_nXpos = m_nYpos = -1;
}

CRoom::~CRoom() {
   for (int i = 0; i < NUM_OF_DOORS; i++)
	delete m_Doors[i];
}

/////////////////////////
//      Add - character overload
//      check for light source and add light affects if room is not
//      perm dark and to LL

void CRoom::Add(CCharacter *pCh) {
   if (pCh->IsAffectedBy(CCharacter::AFFECT_LIGHT)) {
	AddAffect(ROOM_AFFECT_LIGHT, sAffect::MANUAL_AFFECT, pCh->GetAffectValue(CCharacter::AFFECT_LIGHT));
   }
   m_CharactersInRoom.Add(pCh);
}

////////////////////////////
//	Adds an object to a room

void CRoom::Add(CObject *pObj, bool bAddToEnd) {
   if (pObj->IsAffectBit(CObject::ITEM_NEWBIE)) {
	pObj->Set(NULL, NULL);
	//next pulse obj will be deleted
	pObj->SetDelete();
   } else {
	m_ObjInRoom.Add(pObj, bAddToEnd);
	pObj->Set(NULL, this);
   }
}

/////////////////////////
//      Remove character overload
//      check for light source and add light affects
//

void CRoom::Remove(CCharacter *pCh) {
   ReduceAffect(ROOM_AFFECT_LIGHT, pCh->GetAffectValue(CCharacter::AFFECT_LIGHT));
   m_CharactersInRoom.Remove(pCh);
}

bool CRoom::IsPCInRoom() {
   POSITION pos = m_CharactersInRoom.GetStartPosition();
   while (pos) {
	if (!(m_CharactersInRoom.GetNext(pos)->IsNPC())) {
	   return true;
	}
   }
   return false;
}

//reads in a room from a file stream and places data
//into this object

void CRoom::ReadRoom(CAscii &Infile) {
   CString temp(MAX_FILE_STRING_SIZE);
   char flag[33] = {'\0'};
   static CString ENDFILE = "ENDFILE";

   Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
   if (temp == ENDFILE) {
	if (Infile.rdstate() != std::ios_base::eofbit) {
	   Infile.clear(std::ios_base::eofbit);
	}
	//if we are at end of file throw out
	CError Err(NONERROR, "ENDFILE Marker hit!");
	throw &Err;
   }
   std::string s(temp.ptr(), MAX_FILE_STRING_SIZE);
   std::istringstream one(s);
   one >> m_lVnum;
   if (!one.good()) {
	//no Vnum don't read in this room...just skip it
	CError Err(NON_CRITICAL_ERROR, "Error in reading room vnum");
	throw &Err;
   }

   Infile.ReadFileString(m_strRoomName, '~');

   Infile.ReadFileString(m_strDescription, '~');

   Infile.Numeric("%d %s %b %b", &m_nZone, flag, &m_nRoomType, &m_nRoomSize);
   //set the room affects
   SetRoomLoadAffects(Infile.AsciiConvert(flag));

   static CString ENDROOM("ENDROOM");
   static CString DOOR("DOOR");
   static CString EXTRA("EXTRA");
   Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
   while (temp != ENDROOM) {
	if (temp == DOOR) {
	   int door;
	   Infile.Numeric("%d", &door); //door = direction
	   if (door < 0 || door >= NUM_OF_DOORS) {
		ErrorLog << "In Room #: " << m_lVnum << " door number is out of range...skipping door" << endl;
		Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
	   } else if (m_Doors[door]) {
		ErrorLog << "In Room #: " << m_lVnum
			<< " the same door shows up twice in a room ignoring second." << endl;
	   } else {
		m_Doors[door] = new sDoor();
		Infile.ReadFileString(m_Doors[door]->KeyWord, '~');
		Infile.Numeric("%s %l %l", flag, &m_Doors[door]->VnumKey, &m_Doors[door]->ToRoom);
		m_Doors[door]->DoorType = Infile.AsciiConvert(flag);
		if (m_Doors[door]->DoorType == 0) {
		   ErrorLog << "Room vnum: " << m_lVnum << " door #" << door << " has a 0 door type...changing to 1 so it is an unrestricted door type...no such thing as 0 door type." << endl;
		}
	   }
	} else if (temp == EXTRA) {
	   sExtraDesc *current;
	   m_ExtraDesc.Add((current = new sExtraDesc()));
	   Infile.ReadFileString(current->KeyWord, '~');
	   Infile.ReadFileString(current->Description, '~');
	} else {
	   ErrorLog << "Extra lines after ENDROOM marker in room #: " << m_lVnum << " line is: " << temp << endl;
	}
	Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
   }
}

//looks for a character in this room

CCharacter * CRoom::FindCharacterByAlias(CString &Com, short nNum, CCharacter *pLooker) {
   //are we trying to find ourself?
   if (CCharacterAttributes::IsSelf(Com)) {
	if (m_CharactersInRoom.DoesContain(pLooker)) {
	   return pLooker;
	}
	return NULL;
   }

   CCharacter *ch;
   POSITION pos = m_CharactersInRoom.GetStartPosition();
   int nCount = 0;

   while (pos) {
	ch = m_CharactersInRoom.GetNext(pos);
	if (pLooker != ch
		&& pLooker->CanSeeChar(ch)
		&& (ch->GetAlias().Find(Com) != -1)) {
	   if (++nCount == nNum) {
		return ch;
	   }
	}
   }
   return NULL;
}

void CRoom::BuildRoomStat(CString &strRoom) {
   static const char *strFormat =
	   "Zone = %d\r\n"
	   "ZoneRoom = %d\r\n"
	   "m_nXpos = %d\r\n"
	   "m_nYpos = %d\r\n"
	   "Vnum = %u\r\n"
	   "m_nRoomType = %s\r\n"
	   "m_nRoomSize = %s\r\n"
	   "Light level = %s\r\n"
	   "Objects in room = %d\r\n"
	   "chars in room = %d\r\n"
	   "Description = %s\r\n"
	   "Room Affects = %s\r\n";
   CString format, strAffects;
   int i;
   for (i = 0; i < TOTAL_ROOM_AFFECTS; i++) {
	if (IsAffectedBy(i)) {
	   strAffects.Format("%s %s",
		   strAffects.cptr(),
		   m_AffectInfo[i].m_strAffectName.cptr());
	}
   }
   strRoom.Format(strFormat, m_nZone, m_bZoneRoom,
	   m_nXpos, m_nYpos, m_lVnum, m_strRoomType[m_nRoomType],
	   m_strRoomSize[m_nRoomSize],
	   m_strRoomBrightness[GetRoomBrightness()],
	   m_CharactersInRoom.GetCount(),
	   m_ObjInRoom.GetCount(),
	   m_strDescription.cptr(),
	   strAffects.cptr());
   if (m_Doors[NORTH]) {
	format.Format("NORTH = %u\r\n", m_Doors[NORTH]->ToRoom);
	strRoom += format;
   }
   if (m_Doors[SOUTH]) {
	format.Format("SOUTH = %u\r\n", m_Doors[SOUTH]->ToRoom);
	strRoom += format;
   }
   if (m_Doors[EAST]) {
	format.Format("EAST = %u\r\n", m_Doors[EAST]->ToRoom);
	strRoom += format;
   }
   if (m_Doors[WEST]) {
	format.Format("WEST = %u\r\n", m_Doors[WEST]->ToRoom);
	strRoom += format;
   }
}

////////////////////////////////////////////////
//	SendToRoom Overloaded
//	The CString must have at least 1 formatting character
//	(%s) that will be replace with either the characters
//	name or race depending on which side of the race war
//	they are on.
//  This will also work for 2 characters but if you pass
//	2 you must have 2 (%s)'s.
//  written by: Demetrius Comes
//////////////////////////////////////////////

void CRoom::SendToRoom(CString strToRoom, const CCharacter *ch, const CCharacter *pch2) {
   POSITION pos = m_CharactersInRoom.GetStartPosition();
   CCharacter *pch;
   CString strToChar;
   while (pos) {
	pch = m_CharactersInRoom.GetNext(pos);
	short nVision = pch->CanSeeInRoom(this);
	if (pch != ch && pch != pch2 && !pch->InPosition(POS_SLEEPING)) {
	   //are there really 2
	   if (ch && pch2) {
		//make sure pch can see at least one of them
		strToChar.Format(strToRoom.cptr(), ch->GetRaceOrName(pch).cptr(), pch2->GetRaceOrName(pch).cptr());
		pch->SendToChar(strToChar);
	   } else if (ch) {
		strToChar.Format(strToRoom.cptr(), ch->GetRaceOrName(pch).cptr());
		pch->SendToChar(strToChar);
	   } else {
		ErrorLog << "Both characters NULL in SendToRoom!\r\n" << endl;
	   }
	}
   }
}

CObject * CRoom::FindObjectInRoom(CString strObj, short nNum, const CCharacter *pLooker) {
   POSITION pos = m_ObjInRoom.GetStartPosition();
   short nCount = 0;
   CObject *pObj;
   while (pos) {
	pObj = m_ObjInRoom.GetNext(pos);
	if ((pObj->GetAlias().Find(strObj) != -1)
		&& pLooker->CanSee(pObj)) {
	   if (++nCount == nNum) {
		return pObj;
	   }
	}
   }
   return NULL;
}

CRoom::CRoom(CRoom * pRoom) {
   m_ExtraDesc.SetDeleteData();
   assert(pRoom);
   m_strDescription = pRoom->m_strDescription;
   m_strRoomName = pRoom->m_strRoomName;
   m_nRoomSize = pRoom->m_nRoomSize;
   m_nRoomType = pRoom->m_nRoomType;
   m_nZone = pRoom->m_nZone;
   m_bZoneRoom = pRoom->m_bZoneRoom;
   m_lVnum = pRoom->m_lVnum;
   m_nXpos = pRoom->m_nXpos;
   m_nYpos = pRoom->m_nYpos;
   int i;
   for (i = 0; i < TOTAL_ROOM_AFFECTS; i++) {
	m_Affects[i] = pRoom->m_Affects[i];
   }
   for (i = 0; i < NUM_OF_DOORS; i++) {
	if (pRoom->m_Doors[i]) {
	   m_Doors[i] = new sDoor(pRoom->m_Doors[i]);
	} else {
	   m_Doors[i] = NULL;
	}
   }
   POSITION pos = pRoom->m_ExtraDesc.GetStartPosition();
   sExtraDesc *pDesc;
   while (pos) {
	pDesc = pRoom->m_ExtraDesc.GetNext(pos);
	m_ExtraDesc.Add(new sExtraDesc(pDesc));
   }
}

/////////////////////////////
//	Finds an object in a room by Vnum
//	written by: Demetrius Comes 
/////////////////////////////

CObject * CRoom::FindObjectInRoom(long lVnum) {
   POSITION pos = m_ObjInRoom.GetStartPosition();
   CObject *pObj;
   while (pos) {
	pObj = m_ObjInRoom.GetNext(pos);
	if (pObj->GetVnum() == lVnum) {
	   return pObj;
	}
   }
   return NULL;
}

///////////////////////////////////
//	LookObjects to Stack up
//	CCharacter passed in for invis objects etc
// to check to see if they can see obj
/////////////////////////////////

void CRoom::LookObjects(CString & strLook, CCharacter * pCh) {
   StackObjects(m_ObjInRoom, pCh, strLook, false);
}

/////////////////////////////
//	Builds Looks string for characters in a room
//	pCh is so we don't look at ourself but
//	is inited to NULL so you see everyone in the room
//	including yourself if you don't pass it in.
////////////////////////////

void CRoom::LookCharacters(CString & strLook, CCharacter * pCh) {
   CCharacter *pCurrentPerson;
   POSITION posChar = m_CharactersInRoom.GetStartPosition();
   CString strInfo, strTemp;
   while (posChar) {
	pCurrentPerson = m_CharactersInRoom.GetNext(posChar);
	if (pCurrentPerson != pCh
		&& pCh->CanSeeChar(pCurrentPerson)
		&& pCh->CanSeeInRoom(pCurrentPerson->GetRoom()) == CCharacter::SEES_ALL) {
	   strInfo.Empty();
	   //if they are fighting and in the same room as person
	   //they are fighting
	   if (pCurrentPerson->IsFighting()
		   && pCurrentPerson->IsInRoom(pCurrentPerson->IsFighting())) {
		if (pCurrentPerson->IsFighting() == pCh) {
		   strInfo += " fighting YOU!";
		} else {
		   strTemp.Format(" fighting %s.", pCurrentPerson->IsFighting()->GetRaceOrName(pCh).cptr());
		   strInfo += strTemp;
		}
	   }//else there opponite fleed and fighting positon
		//has not been updated yet!
	   else if (pCurrentPerson->IsFighting()) {
		strInfo += " fighting someone that has already left.";
	   } else if (!pCurrentPerson->IsNPC()) {
		strInfo += ".";
	   }
	   if (pCurrentPerson->IsAffectedBy(CCharacter::AFFECT_INVISIBLE)) {
		strInfo += " (&Linv&n)";
	   }
	   if (pCurrentPerson->IsAffectedBy(CCharacter::AFFECT_FLYING)) {
		strInfo += " (&Wflying&n)";
	   }
	   if (pCurrentPerson->IsAffectedBy(CCharacter::AFFECT_CASTING)) {
		strInfo += " (&mcasting&n) ";
	   }
	   if (pCurrentPerson->IsNPC()) {
		if (pCurrentPerson->InPosition(pCurrentPerson->GetDefaultPos()) && !pCurrentPerson->IsFighting()) {
		   strLook.Format("%s%s%s\r\n", strLook.cptr(),
			   pCurrentPerson->GetDefaultPosDesc().cptr(),
			   strInfo.cptr());
		} else {
		   strLook.Format("%s%s %s%s\r\n", strLook.cptr(),
			   pCurrentPerson->GetName().cptr(),
			   pCurrentPerson->GetPositionNames(false),
			   strInfo.cptr());
		}
	   } else if (pCurrentPerson->IsAffectedBy(CCharacter::AFFECT_FUGUE_PLANE)) {
		strLook.Format("%s&WSpirit of the %s&W, %s, floats here.&n\r\n", strLook.cptr(),
			GVM.GetColorRaceName(pCurrentPerson->GetRace()),
			pCurrentPerson->GetName().cptr());
	   } else {
		if (pCh->CanSeeName(pCurrentPerson)) {
		   strLook.Format("%s%s (%s) [%s] %s %s%s\r\n", strLook.cptr(),
			   pCurrentPerson->GetRaceOrName(pCh).cptr(),
			   GVM.GetColorRaceName(pCurrentPerson->GetRace()),
			   CCharacterAttributes::GetSizeName(pCurrentPerson->GetSize()),
			   pCurrentPerson->GetTitle().cptr(),
			   pCurrentPerson->GetPositionNames(false),
			   strInfo.cptr());
		} else {

		   strLook.Format("%s%s [%s] %s %s%s\r\n", strLook.cptr(),
			   pCurrentPerson->GetRaceOrName(pCh).cptr(),
			   CCharacterAttributes::GetSizeName(pCurrentPerson->GetSize()),
			   pCurrentPerson->GetTitle().cptr(),
			   pCurrentPerson->GetPositionNames(false),
			   strInfo.cptr());
		}
	   }
	} else if (pCurrentPerson != pCh
		&& pCh->CanSeeChar(pCurrentPerson)
		&& pCh->CanSeeInRoom(pCurrentPerson->GetRoom()) == CCharacter::SEES_RED_DOTS) {
	   strLook.Format("%sYou see a &Rred shape.&n\r\n",
		   strLook.cptr());
	}
   }
}

bool CRoom::IsInRoom(CCharacter * pCh) {
   return m_CharactersInRoom.DoesContain(pCh);
}

///////////////////////////////////
//	SendToRoom Overloaded
//	This overload send the message to
//	everyone in the room.  The message
//	must not contain any formating characters (%s,%d etc)
//	written by: Demetrius Comes
//	8-7-98
///////////////////////////////////

void CRoom::SendToRoom(CString strToRoom) {
   POSITION pos = m_CharactersInRoom.GetStartPosition();
   CCharacter *pch;
   while (pos) {
	pch = m_CharactersInRoom.GetNext(pos);
	if (!pch->InPosition(POS_SLEEPING)) {
	   pch->SendToChar(strToRoom);
	}
   }
}

///////////////////////////////////////
//	SendToRoom Overloaded
//	This send to room must have a strFormat with 2 formating characters
//	(%s).  The first get the character race or name and the second 
//	the second string
//	written by: Demetrius Comes
////////////////////////////////////

void CRoom::SendToRoom(CString strFormat, const CCharacter * pCh, CString strForRoom) {
#ifdef _DEBUG
   if (strFormat.Count('%') != 2)
	assert(0);
#endif
   POSITION pos = m_CharactersInRoom.GetStartPosition();
   CCharacter *pch;
   CString strToChar, str;
   while (pos) {
	pch = m_CharactersInRoom.GetNext(pos);
	if (pch != pCh && !pch->InPosition(POS_SLEEPING)) {
	   str = pCh->GetRaceOrName(pch);
	   strToChar.Format(strFormat.cptr(), str.cptr(), strForRoom.cptr());
	   pch->SendToChar(strToChar);
	}
   }
}
////////////////////////////////////
//	SendToRoom Overloaded
//	String format must have 4 %s's
//	Two will get characters race or name
//	and two will get string passed in
//	written by: Demetrius Comes
//////////////////////////////////

void CRoom::SendToRoom(CString strFormat, const CCharacter * pCh, const CCharacter * pCh2, CString strOne, CString strTwo) {
#ifdef _DEBUG
   if (strFormat.Count('%') != 4)
	assert(0);
#endif
   POSITION pos = m_CharactersInRoom.GetStartPosition();
   CCharacter *pch;
   CString strToChar;
   while (pos) {
	pch = m_CharactersInRoom.GetNext(pos);
	if (pch != pCh && pch != pCh2 && !pch->InPosition(POS_SLEEPING)) {
	   strToChar.Format(strFormat.cptr(),
		   pCh->GetRaceOrName(pch).cptr(),
		   pCh2->GetRaceOrName(pch).cptr(),
		   strOne.cptr(),
		   strTwo.cptr());
	   pch->SendToChar(strToChar);
	}
   }
}

/////////////////////////////////////////
//	Another overload of SendToRoom
//	the format string must have 3 %s
//	1 and last get character name or race
////////////////////////////////////////

void CRoom::SendToRoom(CString strFormat, const CCharacter * pCh, CString strOne, const CCharacter * pCh2) {
#ifdef _DEBUG
   if (strFormat.Count('%') != 3)
	assert(0);
#endif
   POSITION pos = m_CharactersInRoom.GetStartPosition();
   CCharacter *pch;
   CString strToChar;
   while (pos) {
	pch = m_CharactersInRoom.GetNext(pos);
	if (pch != pCh && pch != pCh2 && !pch->InPosition(POS_SLEEPING)) {
	   strToChar.Format(strFormat.cptr(),
		   pCh->GetRaceOrName(pch).cptr(),
		   strOne.cptr(),
		   pCh2->GetRaceOrName(pch).cptr());
	   pch->SendToChar(strToChar);
	}
   }
}

/////////////////////////
//	Object Overload SendTo room

void CRoom::SendToRoom(CString strFormat, CCharacter *pCh, CObject *pObj) {
#ifdef _DEBUG
   if (strFormat.Count('%') != 2)
	assert(0);
#endif
   POSITION pos = m_CharactersInRoom.GetStartPosition();
   CCharacter *pch;
   CString strToChar;
   while (pos) {
	pch = m_CharactersInRoom.GetNext(pos);
	if (pch != pCh && !pch->InPosition(POS_SLEEPING)) {
	   strToChar.Format(strFormat.cptr(),
		   pCh->GetRaceOrName(pch).cptr(),
		   pObj->GetObjName(pch));
	   pch->SendToChar(strToChar);
	}
   }
}

///////////////////////////////
//	Sends Death Cry to Nieghboring rooms
///////////////////////////////

void CRoom::DeathCry(const CCharacter * pCh) {
   CString strToChar("%s is dead! R.I.P.\r\n&rYour spine tingles as you hear the death cry of %s.&n\r\n");
   SendToRoom(strToChar, pCh, pCh);
   strToChar = "&rThe stench of &Ldeath &rpermeates the air as you hear a death cry nearby.&n\r\n";
   for (int i = 0; i < NUM_OF_DOORS; i++) {
	if (m_Doors[i]) {
	   m_Doors[i]->m_pToRoom->SendToRoom(strToChar);
	}
   }
}

/////////////////////////
//	DoList
//	We are not in a shop
//	so set string that way
/////////////////////////

void CRoom::DoList(CString & strList) {
   strList = "You're not in a shop!\r\n";
}

//Does a look at the room

CString CRoom::DoRoomLook(CCharacter *pLooker) {
   CString strToChar;
   const short nVision = pLooker->CanSeeInRoom(pLooker->GetRoom()); //null mean just check vision with in room
   if (nVision == CCharacter::SEES_ALL) {
	//if you see all you can see room description, exits, maps, etc
	//if not you have a chance to see characters and that's it!
	if (!pLooker->IsNPC() && pLooker->Prefers(CCharacterAttributes::PREFERS_MAP_INSIDE)
		&& IsRealRoom()) {
	   strToChar += GetMap(pLooker);
	} else if (!pLooker->IsNPC() && pLooker->Prefers(CCharacterAttributes::PREFERS_MAP_OUTSIDE)
		&& !IsRealRoom()) {
	   strToChar += GetMap(pLooker);
	}
	if (pLooker->Prefers(CCharacterAttributes::PREFERS_BRIEF) ||
		pLooker->IsNPC()) {
	   strToChar.Format("%s\r\n\r\n%s\r\n%s", strToChar.cptr(),
		   m_strRoomName.cptr(), GetExitString(pLooker).cptr());
	} else {
	   strToChar.Format("%s\r\n\r\n%s\r\n%s\r\n%s", strToChar.cptr(),
		   m_strRoomName.cptr(),
		   m_strDescription.cptr(),
		   GetExitString(pLooker).cptr());
	}
	m_CashInRoom.RoomString(strToChar);
	LookObjects(strToChar, pLooker);
	LookCharacters(strToChar, pLooker);
   }//end sees_all
   else if (nVision == CCharacter::SEES_RED_DOTS) {
	strToChar.Format("%s\r\n%s\r\n",
		m_strRoomName.cptr(),
		GetExitString(pLooker).cptr());
	LookCharacters(strToChar, pLooker);
   } else //see nothing
   {
	if (pLooker->IsAffectedBy(CCharacter::AFFECT_BLINDNESS)) {
	   strToChar = "You can't see a damn thing you're blind!\r\n";
	} else {
	   if (pLooker->HasInnate(INNATE_DAY_SIGHT)) {
		strToChar = "&bIt's too dark in here for you to see anything.&n\r\n";
	   } else {
		strToChar = "&wYour eyes sting because it soo bright!&n\r\n";
	   }
	}
   }
   return strToChar;
}

////////////////////////////////////////////////
//	static Scan
//	static because it needs structures in many rooms
//	not just one.
//

void CRoom::Scan(CCharacter * pCh, CString & str) {
   static const char *strScan[] = {"close by", "a short distance", "a distance"};
   CRoom *pRoom;
   CCharacter *pTarget;
   for (int i = 0; i < NUM_OF_DOORS; i++) {
	//set the room to 1 room away in the direction i
	pRoom = (pCh->GetRoom()->m_Doors[i] != NULL && pCh->GetRoom()->m_Doors[i]->IsOpen()) ? pCh->GetRoom()->m_Doors[i]->m_pToRoom : NULL;
	for (int j = 0; j < 3 && pRoom != NULL; j++) {
	   //loop through all characters to get scan wording
	   POSITION pos = pRoom->m_CharactersInRoom.GetStartPosition();
	   while (pos) {
		pTarget = pRoom->m_CharactersInRoom.GetNext(pos);
		if (pCh->CanSeeChar(pTarget)) {
		   int nVision = pCh->CanSeeInRoom(pTarget->GetRoom());
		   if (nVision == CCharacter::SEES_ALL) {
			str.Format("%sYou see %s %s %s\r\n",
				str.cptr(),
				pTarget->GetRaceOrName(pCh).cptr(),
				strScan[j],
				strDir[i]);
		   } else if (nVision == CCharacter::SEES_RED_DOTS) {
			str.Format("%sYou see a figure of &Rheat&n %s %s\r\n",
				str.cptr(), strScan[j], strDir[i]);
		   }
		}
	   }
	   //go another room in direction 'i'
	   pRoom = (pRoom->m_Doors[i] != NULL && pRoom->m_Doors[i]->IsOpen()) ? pRoom->m_Doors[i]->m_pToRoom : NULL;
	}
   }
}

//////////////////////////////////////////
//	DoShout
//	Recursive!  

void CRoom::DoShout(CString &strShout, CLinkList<CRoom> &UsedRooms, CCharacter *pCh, int nDistance) {
   //send to the room your in
   SendToRoom(strShout, pCh);
   //add to used list
   UsedRooms.Add(this);
   //add nDistnace!=0 for speed reasons
   for (int i = 0; i < NUM_OF_DOORS && nDistance != 0; i++) {
	//makes sphere
	for (int j = 0; j < nDistance; j++) {
	   if (this->m_Doors[i] != NULL &&
		   this->m_Doors[i]->m_pToRoom != NULL &&
		   !UsedRooms.DoesContain(this->m_Doors[i]->m_pToRoom)) {
		this->m_Doors[i]->m_pToRoom->DoShout(strShout, UsedRooms, pCh, nDistance - 1);
	   }
	}
   }
}

////////////////////////////////////////////
//	Gets most valuable object in room
//	written by: Demetrius Comes
//	4-25-99

CObject *CRoom::GetMostValuableObject() {
   CObject *pObj;
   CObject *pObjVal = NULL;
   POSITION pos = m_ObjInRoom.GetStartPosition();
   while (pos) {
	pObj = m_ObjInRoom.GetNext(pos);
	if (pObjVal == NULL ||
		(pObjVal->GetCost() < pObj->GetCost())) {
	   if (!pObj->IsAffectBit(CObject::ITEM_NOT_TAKEABLE)) {
		pObjVal = pObj;
	   }
	}
   }
   return pObjVal;
}

/////////////////////////////////
//	Gets a random object from a room
//	written by Demetrius Comes
//	4-25-99

CObject *CRoom::GetRandomObject() {
   CObject *pObj = NULL;
   if (!m_ObjInRoom.IsEmpty()) {
	int nObj = DIE(m_ObjInRoom.GetCount());
	int nCount = 0;
	POSITION pos = m_ObjInRoom.GetStartPosition();
	while (pos) {
	   pObj = m_ObjInRoom.GetNext(pos);
	   if (nCount++ == nObj) {
		if (pObj->IsAffectBit(CObject::ITEM_NOT_TAKEABLE)) {
		   return NULL;
		} else {
		   return pObj;
		}
	   }
	}
   }
   return NULL;
}

////////////////////////////////////////////
//	Gets the exits of a room;

void CRoom::GetExits(CString & str) {
   int i;
   for (i = 0; i < NUM_OF_DOORS; i++) {
	if (m_Doors[i] != NULL) {
	   str.Format("%s%s\t%s\r\n",
		   str.cptr(), strDir[i],
		   m_Doors[i]->m_pToRoom->m_strRoomName.cptr());
	}
   }
}

////////////////////////////////////
//	A room is bright if the we have a ROOM_BRIGHT
//	Or the room is outside and it is the part of the day
//	Or room is ROOM_LIT and is day time.

int CRoom::GetRoomBrightness() const {
   if (IsAffectedBy(ROOM_AFFECT_PERM_DARK)
	   && IsAffectedBy(ROOM_AFFECT_PERM_LIGHT)) {
	return CRoom::ROOM_TWILIGHT;
   }
   if (IsAffectedBy(ROOM_AFFECT_PERM_DARK)) {
	return CRoom::ROOM_DARK;
   } else if (IsAffectedBy(ROOM_AFFECT_PERM_LIGHT)) {
	return CRoom::ROOM_BRIGHT;
   } else if (m_nRoomType == TYPE_INSIDE ||
	   m_nRoomType == TYPE_UNDERWATER ||
	   m_nRoomType == TYPE_DARKFOREST ||
	   IsAffectedBy(ROOM_AFFECT_INDOORS) ||
	   IsAffectedBy(ROOM_AFFECT_DARK)) {
	if (m_Affects[ROOM_AFFECT_LIGHT].m_nValue > 0) {
	   //take room size into account
	   short nPower = m_nRoomSize / m_Affects[ROOM_AFFECT_LIGHT].m_nValue;
	   if (nPower >= 2) {
		return ROOM_LIT;
	   } else {
		return ROOM_BRIGHT;
	   }
	} else {
	   return ROOM_DARK;
	}
   } else {
	if (CMudTime::IsBrightPartOfDay())
	   return ROOM_BRIGHT;
	else {
	   if (m_Affects[ROOM_AFFECT_LIGHT].m_nValue > 0) {
		int nPower = m_nRoomSize / m_Affects[ROOM_AFFECT_LIGHT].m_nValue;
		if (nPower >= 2 && CMudTime::IsDayTime()) {
		   return ROOM_BRIGHT;
		} else if (nPower < 2) {
		   return ROOM_BRIGHT;
		} else {
		   return ROOM_LIT;
		}
	   } else {
		if (CMudTime::IsDayTime())
		   return ROOM_LIT;
		else
		   return ROOM_DARK;
	   }
	}
   }
}

//////////////////////////////////////
//	GetMovementCost()
//	return: the movement points to move into a room
//	set by static const array m_MovementCost

short CRoom::GetMovementCost() {
   //TODO weather to affect movement
#ifdef _DEBUG
   if (m_nRoomType >= TOTAL_ROOM_TYPES || m_nRoomType < 0) {
	ErrorLog << "Room type for room: " << m_lVnum << " is out of range, currently set to: " << m_nRoomType << endl;
	return 1;
   }
#endif
   short nCost = m_MovementCost[m_nRoomType];
   return nCost;
}

//////////////////////////////////////
//	CanEnter
//	returns: bool if player can enter room or not
//	ex.  room type = WATERENTRY
//	then they can't enter unless they are flying
//	or waternoentry they can't enter even if flying
//////////////////////////////////////

CRoom * CRoom::CanEnter(CCharacter *pCh, CString &strDir, CString &strReverse) {
   CRoom::sDoor *pDoor;
   switch (StringToDirection(strDir)) {
	case NORTH:
	   strReverse = "south";
	   pDoor = m_Doors[NORTH];
	   break;
	case SOUTH:
	   pDoor = m_Doors[SOUTH];
	   strReverse = "north";
	   break;
	case WEST:
	   pDoor = m_Doors[WEST];
	   strReverse = "east";
	   break;
	case EAST:
	   pDoor = m_Doors[EAST];
	   strReverse = "west";
	   break;
	case UP:
	   pDoor = m_Doors[UP];
	   strReverse = "below";
	   break;
	case DOWN:
	   pDoor = m_Doors[DOWN];
	   strReverse = "above";
	   break;
	default:
	   pDoor = NULL;
	   break;
   }
   CRoom *pToRoom = NULL;

   if (!pDoor) {
	pCh->SendToChar("You can't go that way!\r\n");
   }//if there is a door and we can walk through it
   else if (pDoor && pDoor->CanWalkThrough()) {
	//get the room we are trying to walk into
	pToRoom = pDoor->m_pToRoom;
	//Check the next Rooms room types and insure 
	//we can walk in there
	switch (pToRoom->m_nRoomType) {
	   case TYPE_WATERENTER:
		if (!pCh->IsAffectedBy(CCharacter::AFFECT_FLYING)) {
		   pCh->SendToChar("The water is a bit too cold to swim in.\r\n");
		   return NULL;
		}
		break;
	   case TYPE_WATERNOENTER:
		pCh->SendToChar("The Water looks a bit too cold.\r\n");
		return NULL;
		break;
	}
	//check room affects
	if (pToRoom && pToRoom->IsAffectedBy(ROOM_AFFECT_GOD_ROOM)
		&& !pCh->IsGod()) {
	   pCh->SendToChar("You are not godly enough to enter that room.\r\n");
	   return NULL;
	}
	if (pToRoom && pToRoom->IsAffectedBy(ROOM_AFFECT_FLYING)
		&& !pCh->IsAffectedBy(CCharacter::AFFECT_FLYING)) {
	   pCh->SendToChar("Better flap your wings faster.\r\n");
	   return NULL;
	}
   } else if (!pDoor->CanWalkThrough()) {
	if (pDoor->IsWall()) {
	   pCh->SendToChar("How do you plan to walk through the wall?\r\n");
	   SendToRoom("%s walks right into a wall.\r\n", pCh);
	} else {
	   pCh->SendToChar("The door is shut!\r\n");
	}
   }
   return pToRoom;
};

//////////////////////////////
//	Turn the bit vectors we loaded
//	into room affects

void CRoom::SetRoomLoadAffects(int nAffects) {
#define BITCHECK(a,b) (a&b)==b
   if (nAffects == 0) {
	return;
   }
   if (BITCHECK(nAffects, DARK)) {
	AddAffect(ROOM_AFFECT_DARK, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, NOMOB)) {
	AddAffect(ROOM_AFFECT_NOMOB, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, INDOORS)) {
	AddAffect(ROOM_AFFECT_INDOORS, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, PEACEFULL)) {
	AddAffect(ROOM_AFFECT_PEACEFUL, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, NOSOUND)) {
	AddAffect(ROOM_AFFECT_NO_SOUND, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, NOTRACK)) {
	AddAffect(ROOM_AFFECT_NOTRACK, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, NOMAGIC)) {
	AddAffect(ROOM_AFFECT_NOMAGIC, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, NO_SCAVENGE)) {
	AddAffect(ROOM_AFFECT_NO_SCAVENGE, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, GODROOM)) {
	AddAffect(ROOM_AFFECT_GOD_ROOM, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, FASTHEAL)) {
	AddAffect(ROOM_AFFECT_FAST_HEAL, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, TUNNEL)) {
	AddAffect(ROOM_AFFECT_TUNNEL, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, INN)) {
	AddAffect(ROOM_AFFECT_INN, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, PERM_DARK)) {
	AddAffect(ROOM_AFFECT_PERM_DARK, sAffect::PERM_AFFECT);
   }
   if (BITCHECK(nAffects, PERM_LIGHT)) {
	AddAffect(ROOM_AFFECT_PERM_LIGHT, sAffect::PERM_AFFECT);
   }
}

//////////////////////////////
//	Makes exit string for looks

CString CRoom::GetExitString(CCharacter *pLooker) {
   static const char * strExitsWording[] = {
	"-N", "-E", "-S", "-W", "-U", "-D"
   };
   CString strExits;
   if (pLooker->Prefers(CCharacterAttributes::PREFERS_AUTO_EXIT)
	   && !pLooker->IsNPC()) {
	strExits = "&wExits: &g";
	bool bDoorFound = false;
	for (int i = 0; i < NUM_OF_DOORS; i++) {
	   if (m_Doors[i] && !m_Doors[i]->IsSecret()) {
		bDoorFound = true;
		strExits.Format("%s%s%s",
			strExits.cptr(), strExitsWording[i],
			(m_Doors[i]->CanWalkThrough() ? m_Doors[i]->IsUnrestricted() ? " " : "| " : "# "));
	   }
	}
	if (!bDoorFound) {
	   strExits += "&RNONE!&n";
	}
	strExits += "&n\r\n";
   }
   return strExits;
}

/////////////////////////
//	change string to a direction

short CRoom::StringToDirection(CString &strDir) {
   switch (strDir[0]) {
	case 'n': case 'N':
	   strDir = "north";
	   return NORTH;
	   break;
	case 's': case 'S':
	   strDir = "south";
	   return SOUTH;
	   break;
	case 'w': case 'W':
	   strDir = "west";
	   return WEST;
	   break;
	case 'e': case 'E':
	   strDir = "east";
	   return EAST;
	   break;
	case 'u': case 'U':
	   strDir = "up";
	   return UP;
	   break;
	case 'd': case 'D':
	   strDir = "down";
	   return DOWN;
	   break;
	default:
	   strDir = "nowhere";
	   return NOWHERE;
	   break;
   }
}

//////////////////////////////
//	Opens a door if it exists

void CRoom::OpenDoor(short nDir, const CCharacter *pCh, bool bOpening) {
   if (nDir == NOWHERE) {
	pCh->SendToChar("Which door?\r\n");
   } else {
	sDoor *pDoor = m_Doors[nDir];
	if (pDoor == NULL) {
	   pCh->SendToChar("There is no door there!\r\n");
	} else if (bOpening) {
	   if (pDoor->IsOpen()) {
		pCh->SendToChar("It's already open!\r\n");
	   } else if (!pDoor->CanBeOpened()) {
		if (pDoor->IsWall()) {
		   pCh->SendToChar("How are you going to get passed the wall?\r\n");
		} else {
		   pCh->SendToChar("You can't seem to open it.\r\n");
		}
	   } else {
		pDoor->Open();
		pCh->SendToChar("You open the door.\r\n");
		pCh->GetRoom()->SendToRoom("%s opens the door.\r\n", pCh);
		pDoor->m_pToRoom->SendToRoom("The door opens from the other side!\r\n");
		//open the door on the other size too
		short nRD = CWorld::ReverseDirection(nDir);
		if (pDoor->m_pToRoom->m_Doors[nRD] != NULL) {
		   pDoor->m_pToRoom->m_Doors[nRD]->Open();
		} else {
		   ErrorLog << "Error in open door. Reverse door is null." << endl;
		}
	   }
	} else {
	   if (pDoor->IsClosed()) {
		pCh->SendToChar("It's already closed!\r\n");
	   } else if (!pDoor->CanBeClosed()) {
		pCh->SendToChar("Hrm maybe you just don't know how?\r\n");
	   } else {
		pDoor->Close();
		pCh->SendToChar("You close the door.\r\n");
		SendToRoom("%s closes the door.\r\n", pCh);
		pDoor->m_pToRoom->SendToRoom("The door is closed from the other side!\r\n");
		//close the door on the other size too
		short nRD = CWorld::ReverseDirection(nDir);
		if (pDoor->m_pToRoom->m_Doors[nRD] != NULL) {
		   pDoor->m_pToRoom->m_Doors[nRD]->Close();
		} else {
		   ErrorLog << "Error in close door. Reverse door is null." << endl;
		}
	   }
	}
   }
}

///////////////////////////////////
//	CanCampInRoom
//	virtual 
//	if room type is indoors or has either room affect
//	NO_CAMP or INDOORS then they can't!

bool CRoom::CanCampInRoom() {
   return(m_nRoomType != TYPE_INSIDE && !IsAffectedBy(ROOM_AFFECT_NO_CAMP) && !IsAffectedBy(ROOM_AFFECT_INDOORS));
}

CString CRoom::GetMap(const CCharacter *pLooker) {
   CString strMap;
   if (IsRealRoom()) {
	strMap.Format("\r\n\t&i &n%s&i &n\r\n\t%s%s%s\r\n\t&i &n%s&i &n",
		(m_Doors[NORTH] ? m_RoomColors[(m_Doors[NORTH]->m_pToRoom->m_nRoomType)].cptr() : m_strBlock.cptr()),
		(m_Doors[WEST] ? m_RoomColors[(m_Doors[WEST]->m_pToRoom->m_nRoomType)].cptr() : m_strBlock.cptr()),
		"&WO&n",
		(m_Doors[EAST] ? m_RoomColors[(m_Doors[EAST]->m_pToRoom->m_nRoomType)].cptr() : m_strBlock.cptr()),
		(m_Doors[SOUTH] ? m_RoomColors[(m_Doors[SOUTH]->m_pToRoom->m_nRoomType)].cptr() : m_strBlock.cptr()));
   } else {
	//This could be sped up
	//TODO Calcualte how far they can see
	short nStartX = (m_nXpos - 3) >= 0 ? m_nXpos - 3 : 0;
	short nStartY = (m_nYpos - 2) >= 0 ? m_nYpos - 2 : 0;
	short nEndX = (m_nXpos + 3) >= VIRTUALWIDTH ? VIRTUALWIDTH - 1 : m_nXpos + 3;
	short nEndY = (m_nYpos + 2) >= VIRTUALHEIGHT ? VIRTUALHEIGHT - 1 : m_nYpos + 2;
	const CRoom *pRoom;
	for (int i = nStartY; i <= nEndY; i++) {
	   strMap += "\r\n\t";
	   for (int j = nStartX; j <= nEndX; j++) {
		pRoom = CWorld::m_Map[j][i];
		if (i == m_nYpos && j == m_nXpos) {
		   strMap += "&WO&n";
		} else if (pRoom->m_bZoneRoom) {
		   strMap += "&h&B^&n";
		} else if (pRoom->IsShop()) {
		   strMap += "&G&Us&n";
		} else if (pRoom->ContainsShip()) {
		   strMap += "&WS&n";
		} else {
		   if (pRoom->m_CharactersInRoom.IsEmpty()) {
			strMap += m_RoomColors[pRoom->m_nRoomType];
		   } else {
			if (pLooker->CanSeeChar(pRoom->m_CharactersInRoom.GetFirst())) {
			   if (pRoom->m_CharactersInRoom.GetFirst()->IsNPC()) {
				strMap += "&R&FM&n";
			   } else {
				strMap += "&F&WP&n";
			   }
			} else {
			   strMap += m_RoomColors[pRoom->m_nRoomType];
			}
		   }
		}
	   }
	}
   }
   return strMap;
}

////////////////////////////////
//	Does room has a ship in it

bool CRoom::ContainsShip() const {
   POSITION pos = m_ObjInRoom.GetStartPosition();
   while (pos) {
	if (m_ObjInRoom.GetNext(pos)->IsType(ITEM_TYPE_TRANSPORT))
	   return true;
   }
   return false;
}

///////////////////////////////////
// Add an affect to each character

void CRoom::AddAffectToCharacters(short nAffect, int nPulses, int nValue, bool bSendMsg) {
   POSITION pos = m_CharactersInRoom.GetStartPosition();
   while (pos) {
	m_CharactersInRoom.GetNext(pos)->AddAffect(nAffect, nPulses, nValue, bSendMsg);
   }
}

//////////////////////////////////
//	Adds the affect to each object in the room

void CRoom::AddAffectToObjects(short nAffect, int nPulses, int nValue) {
   POSITION pos = m_ObjInRoom.GetStartPosition();
   while (pos) {
	m_ObjInRoom.GetNext(pos)->AddAffect(nAffect, nPulses, nValue);
   }
}

//////////////////////////////////
//	Check All doors in the room and 
//	return the door that matches or return NOWHERE

short CRoom::IsKeyWordForDoor(CString str, CString strDir) {
   short nDir = this->StringToDirection(strDir);
   if (nDir != NOWHERE
	   && m_Doors[nDir] != NULL
	   && m_Doors[nDir]->KeyWord.Find(str)) {
	return nDir;
   }
   return NOWHERE;
}

///////////////////////////////////
//	Purge
//	deletes all objects in a room.

void CRoom::PurgeObjects(const CCharacter *pCh) {
   POSITION pos = this->m_ObjInRoom.GetStartPosition();
   CList <CObject *> ObjToDelete;
   CObject *pObj;
   while (pos) {
	pObj = m_ObjInRoom.GetNext(pos);
	if (!pObj->IsAffectedBy(CObject::ITEM_NOT_TAKEABLE)) {
	   pObj->SetDelete();
	   ObjToDelete.Add(pObj);
	}
   }
   pos = ObjToDelete.GetStartPosition();
   while (pos) {
	this->Remove(ObjToDelete.GetNext(pos));
   }
   SendToRoom("%s purges the room of objects.\r\n", pCh);
   pCh->SendToChar("You purge the objects in the room.\r\n");
}

//////////////////////////////////
//	Sets a Wall in the direction given

void CRoom::Wall(short nDir, bool bRemoving) {
   assert(nDir > 0 || nDir < NUM_OF_DOORS);
   if (m_Doors[nDir] != NULL) {
	if (bRemoving) {
	   m_Doors[nDir]->RemoveWall();
	} else {
	   m_Doors[nDir]->SetWall();
	}
   }
}

///////////////////////////////////////
//	for search type 
//	check to see if any character in the room is affectby nAffect
//	if so pass back the first occurance 

CCharacter * CRoom::IsAnyCharacterAffectedBy(short nAffect) {
   POSITION pos = this->m_CharactersInRoom.GetStartPosition();
   CCharacter *pCh;
   while (pos) {
	pCh = this->m_CharactersInRoom.GetNext(pos);
	if (pCh->IsAffectedBy(nAffect)) {
	   return pCh;
	}
   }
   return NULL;
}

///////////////////////////////////
//	For search
//	if we find a secret door remove secret and
//	return a true

bool CRoom::OpenSecretDoor() {
   for (int i = 0; i < NUM_OF_DOORS; i++) {
	if (this->m_Doors[i] &&
		this->m_Doors[i]->IsSecret()) {
	   this->m_Doors[i]->RemoveDoorType(sDoor::DOOR_SECRET);
	   return true;
	}
   }
   return false;
}

////////////////////////////////
//	search
//	Checks to see if any object in the room is affectedby
//	naffect if so passes it back

CObject * CRoom::IsObjectAffectedBy(short nAffect) {
   POSITION pos = this->m_ObjInRoom.GetStartPosition();
   CObject *pObj;
   while (pos) {
	pObj = this->m_ObjInRoom.GetNext(pos);
	if (pObj->IsAffectedBy(nAffect)) {
	   return pObj;
	}
   }
   return NULL;
}

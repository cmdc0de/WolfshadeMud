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
/*
 *
 *  World Implementation
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include <stdlib.h>
#include <ctype.h>
#include "world.h"
#include "shop.room.h"
#include "bank.room.h"

CMap<long, CRoom *> CWorld::m_WorldMap;
CString CWorld::m_strMap;
const CRoom * CWorld::m_Map[VIRTUALWIDTH][VIRTUALHEIGHT];
const CString CWorld::DEFAULT_REALINDEX = "realroomindex";

CWorld::CWorld(const CObjectManager *pObjManager, const CMobManager *pMobManager, CString strIndex) {
   REALINDEX = REALPREFIX;
   REALINDEX += strIndex;
   try {
	BootWorld();
	BootShops(pObjManager, pMobManager);
	ConnectRooms();
	BuildMap();
   } catch (CError *Er) {
	ErrorLog << Er->GetMessage() << endl;
	CError Err(Er->GetError(), "Caught in CWorld");
	throw &Err;
   }
}

CWorld::~CWorld() {
   //delete rooms in World map
   POSITION pos = m_WorldMap.GetStartingPosition();
   CRoom *pRoom;
   while (pos) {
	pRoom = m_WorldMap.GetNext(pos);
	delete pRoom;
   }
}

////////////////////////////////
//	BootWorld
//	Ok virtual rooms will start with the Vnum MIN_VIRTUAL_VNUM
//	and be added to from there.  All real rooms must have a Vnum
//	of lower than MIN_VIRTUAL_VNUM
//	written by: Demetrius Comes
/////////////////////////////////

void CWorld::BootWorld() {
   try {
	ReadInVirtualRooms();
	ReadRealRooms();
	MudLog << "size of a room is : " << int(sizeof(CRoom)) << endl;
   } catch (CError *Er) {
	ErrorLog << Er->GetMessage() << endl;
	if (Er->GetError() == CRITICAL_ERROR) {
	   CError Err(Er->GetError(), "catch in boot world");
	   throw &Err;
	}
   }
}

/////////////////////////////////
//  ReadInVirtualRooms
//  Open the virtual prototypes file
//  Read in the room, add it to the hash table
//  Then open virtual world file and 
//  add rooms to m_worldmap per the virtual world file
//	written by: Demetrius Comes
//////////////////////////////////

void CWorld::ReadInVirtualRooms() {
   CMap<long, CRoom *> VirtualPrototypes;
   int hyphen;
   CString Error, temp, RoomInfo(MAX_FILE_STRING_SIZE);

   CAscii Virtualtypes(VIRTURALINDEX, std::ios_base::in);
   if (!Virtualtypes) {
	Error.Format("File won't open: %s", VIRTURALINDEX);
	CError Err(CRITICAL_ERROR, Error.cptr());
	throw &Err;
   }
   MudLog << "booting prototype rooms" << endl;
   while (Virtualtypes.good()) {
	try {
	   CRoom *pCheck = NULL, RoomTmp;
	   RoomTmp.ReadRoom(Virtualtypes);
	   if (!VirtualPrototypes.Lookup(RoomTmp.m_lVnum, pCheck)) {
		VirtualPrototypes.Add(RoomTmp.m_lVnum, new CRoom(&RoomTmp));
	   } else {
		ErrorLog << "Duplicate room vnums#: " << RoomTmp.m_lVnum << endl;
	   }
	} catch (CError *Er) {
	   if (Er->GetError() == NON_CRITICAL_ERROR) {
		if (!Virtualtypes.ReadTill("ENDROOM")) {
		   ErrorLog << "Went bad in read till function!" << endl;
		   throw Er;
		} else {
		   ErrorLog << Er->GetMessage() << endl;
		}
	   } else if (Er->GetError() == CRITICAL_ERROR) {
		ErrorLog << Er->GetMessage() << endl;
		throw Er;
	   } else if (Er->GetError() == NONERROR) {
		//do nothing
	   }
	}
   }
   Virtualtypes.close();

   MudLog << VirtualPrototypes.GetCount() << " virtual rooms prototypes read." << endl;

   CAscii VirtualWorld(VIRTUALWORLD, std::ios_base::in);
   if (!VirtualWorld) {
	Error.Format("File won't open: %s", VIRTUALWORLD);
	CError Err(CRITICAL_ERROR, Error.cptr());
	throw &Err;
   }
   MudLog << "creating virutal world" << endl;
   CRoom *pNewRoom, *pRoom;
   for (int y = 0; y < VIRTUALHEIGHT; y++) {
	for (int x = 0; x < VIRTUALWIDTH; x++) {
	   pRoom = NULL;
	   VirtualWorld >> RoomInfo;
	   hyphen = RoomInfo.Find('-');
	   //before - is prototypes number to use
	   temp = RoomInfo(0, (hyphen - 1));
	   if (VirtualPrototypes.Lookup(temp.MakeLong(), pRoom)) {
		pNewRoom = new CRoom(pRoom);
		pNewRoom->m_lVnum = ((y * VIRTUALWIDTH) + x) + MIN_VIRTUAL_VNUM;
		pNewRoom->m_nXpos = x;
		pNewRoom->m_nYpos = y;
		pNewRoom->m_bZoneRoom = false;
		//get the prototype room
		m_WorldMap.Add(pNewRoom->m_lVnum, pNewRoom);
		temp = RoomInfo((hyphen + 1), RoomInfo.GetLength());
		SetVirtualDoors(pNewRoom, VirtualWorld.AsciiConvert(temp.ptr()));
	   } else {
		ErrorLog << "Virtual world is trying to use a prototypes that is not defined"
			<< "\r\nOffending line is " << temp << " at X = " << x << " and Y=" << y << endl;
		assert(0);
	   }
	}
   }
   //delete memory allocated for prototypes
   POSITION pos = VirtualPrototypes.GetStartingPosition();
   while (pos) {
	pRoom = VirtualPrototypes.GetNext(pos);
	delete pRoom;
   }

   Virtualtypes.close();
   MudLog << m_WorldMap.GetCount() << " virtual rooms made." << endl;
}
//if direction has a null there is no door present
//void CWorld::SetVirtualDoors(unsigned int Roomnum, int DoorCode)

void CWorld::SetVirtualDoors(CRoom *pRoom, int DoorCode) {
   if (DoorCode == 0)
	return;

   assert(pRoom);
   if ((DoorCode & CRoom::VIRTUAL_DOOR_NORTH) == CRoom::VIRTUAL_DOOR_NORTH) {
	pRoom->m_Doors[NORTH] = new CRoom::sDoor();
	pRoom->m_Doors[NORTH]->DoorType = 1;
	if (pRoom->m_nYpos == 0)
	   pRoom->m_Doors[NORTH]->ToRoom =
		((VIRTUALHEIGHT - 1) * VIRTUALWIDTH) + pRoom->m_nXpos;
	else
	   pRoom->m_Doors[NORTH]->ToRoom = pRoom->m_lVnum - VIRTUALWIDTH;
   }
   if ((DoorCode & CRoom::VIRTUAL_DOOR_SOUTH) == CRoom::VIRTUAL_DOOR_SOUTH) {
	pRoom->m_Doors[SOUTH] = new CRoom::sDoor();
	pRoom->m_Doors[SOUTH]->DoorType = 1;
	if (pRoom->m_nYpos == (VIRTUALHEIGHT - 1))
	   pRoom->m_Doors[SOUTH]->ToRoom = pRoom->m_nXpos + MIN_VIRTUAL_VNUM;
	else
	   pRoom->m_Doors[SOUTH]->ToRoom = pRoom->m_lVnum + VIRTUALWIDTH;
   }
   if ((DoorCode & CRoom::VIRTUAL_DOOR_EAST) == CRoom::VIRTUAL_DOOR_EAST) {
	pRoom->m_Doors[EAST] = new CRoom::sDoor();
	pRoom->m_Doors[EAST]->DoorType = 1;
	if (pRoom->m_nXpos == (VIRTUALWIDTH - 1))
	   pRoom->m_Doors[EAST]->ToRoom = pRoom->m_lVnum - (VIRTUALWIDTH - 1);
	else
	   pRoom->m_Doors[EAST]->ToRoom = pRoom->m_lVnum + 1;
   }
   if ((DoorCode & CRoom::VIRTUAL_DOOR_WEST) == CRoom::VIRTUAL_DOOR_WEST) {
	pRoom->m_Doors[WEST] = new CRoom::sDoor();
	pRoom->m_Doors[WEST]->DoorType = 1;
	if (pRoom->m_nXpos == 0)
	   pRoom->m_Doors[WEST]->ToRoom = VIRTUALWIDTH - 1;
	else
	   pRoom->m_Doors[WEST]->ToRoom = pRoom->m_lVnum - 1;
   }
   return;
}

//////////////////////////////////
//	ReadRealRooms
//	Open index file...Get one line at a time
//	these lines are file names.  Open those
//	files names and read rooms and adding them to
//	the m_WorldMap hash table
//	VNum will be += TOTAL_VIRTUAL_ROOMS
//	Door->ToRoom variable will be modified as well if
//	being connected to another real room.
//	written by: Demetrius comes
/////////////////////////////////

void CWorld::ReadRealRooms() {
   CString Error, strFile;
   char temp[MAX_FILE_LENGTH];
   long nRoomCount = m_WorldMap.GetCount();

   std::ifstream Index(REALINDEX.cptr(), std::ios_base::in);
   if (!Index) {
	Error.Format("File won't open: %s", REALINDEX.cptr());
	CError Err(CRITICAL_ERROR, Error.cptr());
	throw &Err;
   }
   MudLog << "booting real rooms" << endl;
   while (Index.good()) {
	strFile = REALPREFIX;
	Index >> temp;
	strFile += temp;
	CAscii CurrentFile(strFile, std::ios_base::in);
	if (!CurrentFile) {
	   Error.Format("File would not open! %s", &temp[0]);
	   CError Err(CRITICAL_ERROR, Error.cptr());
	   throw &Err;
	}
	MudLog << "reading file " << strFile << endl;
	while (CurrentFile.good()) {
	   try {
		CRoom RoomTmp, *pCheck = NULL;
		RoomTmp.ReadRoom(CurrentFile);
		if (!m_WorldMap.Lookup(RoomTmp.m_lVnum, pCheck)) {
		   if (RoomTmp.IsType(CRoom::TYPE_BANK)) {
			m_WorldMap.Add(RoomTmp.m_lVnum, new CBankRoom(&RoomTmp));
		   } else {
			m_WorldMap.Add(RoomTmp.m_lVnum, new CRoom(&RoomTmp));
		   }
		} else {
		   ErrorLog << "Duplicate room vnums#: " << RoomTmp.m_lVnum << endl;
		}
	   } catch (CError *Er) {
		if (Er->GetError() == CRITICAL_ERROR) {
		   throw Er;
		} else if (Er->GetError() == NON_CRITICAL_ERROR) {
		   if (!CurrentFile.ReadTill("ENDROOM")) {
			ErrorLog << "Went bad in read till" << endl;
			throw Er;
		   } else {
			ErrorLog << Er->GetMessage() << endl;
		   }
		} else if (Er->GetError() == NONERROR) {
		   //perr->DisplayError();
		}
	   }
	}
	CurrentFile.close();
   }
   Index.close();
   MudLog << m_WorldMap.GetCount() - nRoomCount << " real rooms read." << endl;
   MudLog << m_WorldMap.GetCount() << " total rooms in world." << endl;
}

////////////////////////////////
//	ConnectRooms
//	Walk the m_WorldMap like you would walk a LL
//	Run though the doors in the room.  Each door
//	that do a look up on the room it's sopose to connect to
//	set the CRoom ptr in the CDoor class associated with 
//	the door we are on to the room we find from the lookk up
//	written by: Demetrius Comes
///////////////////////////////

void CWorld::ConnectRooms() {
   MudLog << "Setting Room pointers for faster movement =)" << endl;

   POSITION pos = m_WorldMap.GetStartingPosition();
   CRoom *pRoom, *pToRoom;
   long lToRoom, lInRoom;
   while (pos) {
	//get room pointer
	pRoom = m_WorldMap.GetNext(pos);
	assert(pRoom);
	lInRoom = pRoom->m_lVnum;
	//run all doors in room
	for (int i = 0; i < NUM_OF_DOORS; i++) {
	   //if we have a door
	   if (pRoom->m_Doors[i]) {
		lToRoom = pRoom->m_Doors[i]->ToRoom;
		//Look up the room, pRoom is connected to
		if (m_WorldMap.Lookup(lToRoom, pToRoom)) {
		   pRoom->m_Doors[i]->m_pToRoom = pToRoom;
		   if (!pToRoom->IsRealRoom() && pRoom->IsRealRoom()) {
			short nRD = ReverseDirection(i);
			//we know pToRoom is a virtual room so we take what room the
			//reverse exit is pointing to and make it a zone room
			//this eliminated the problem of having two zone rooms marked
			//if a zone has two entrances.
			CRoom *pZone = pToRoom->m_Doors[nRD] != NULL ? CWorld::FindRoom(pToRoom->m_Doors[nRD]->ToRoom) : NULL;
			if (pZone != NULL) {
			   pZone->m_bZoneRoom = true;
			}
			//after we have mark the room we sue to walk into as zone
			//we delete the door and make the connection from real to
			//virtual room
			MudLog << "creating zone connection at room vnum: " << pToRoom->m_lVnum << endl;
			delete pToRoom->m_Doors[nRD];
			pToRoom->m_Doors[nRD] = NULL;
			pToRoom->m_Doors[nRD] = new CRoom::sDoor();
			pToRoom->m_Doors[nRD]->m_pToRoom = pRoom;
			pToRoom->m_Doors[nRD]->ToRoom = lInRoom;
			//set door type to be unrestricted
			pToRoom->m_Doors[nRD]->DoorType = CRoom::sDoor::UNRESTRICTED_DOOR;
		   }
		}		   //the room we are trying to connect todon't exist
		else {
		   ErrorLog << "Door #" << i << " in room vnum# "
			   << pRoom->m_lVnum
			   << " has no room to go to\r\n Missing Vnum is "
			   << pRoom->m_Doors[i]->ToRoom;
		   ErrorLog << "\r\nEliminating door\r\n" << endl;
		   delete pRoom->m_Doors[i];
		   pRoom->m_Doors[i] = NULL;
		}
	   }
	}
   }
   MudLog << "Checking for bad doors..." << endl;
   pos = m_WorldMap.GetStartingPosition();
   while (pos) {
	pRoom = m_WorldMap.GetNext(pos);
	for (int i = 0; i < NUM_OF_DOORS; i++) {
	   if (pRoom->m_Doors[i] != NULL) {
		if (pRoom->m_Doors[i]->m_pToRoom == NULL) {
		   ErrorLog << "Room Vnum : " << pRoom->GetVnum() << "Door direction: " << CRoom::strDir[i] << " There is a door but is not connected to a room." << endl;
		}
		if (pRoom->m_Doors[i]->m_pToRoom == pRoom) {
		   ErrorLog << "Room Vnum: " << pRoom->GetVnum() << "Door direction: " << CRoom::strDir[i] << " is connected to itself!" << endl;
		}
	   }
	}
   }
   MudLog << "Done checking bad doors." << endl;
   return;
}

///////////////////////
//	static function
//	get get room pointer
//////////////////////

CRoom * CWorld::FindRoom(long nRoomNum) {
   CRoom *pRoom;
   if (m_WorldMap.Lookup(nRoomNum, pRoom)) {
	return pRoom;
   }
   return NULL;
}

//////////////////////////
//	Reverse Direction
//	Gives the opposite direction

short int CWorld::ReverseDirection(short nDir) {
   switch (nDir) {
	case NORTH:
	   return SOUTH;
	   break;
	case EAST:
	   return WEST;
	   break;
	case WEST:
	   return EAST;
	   break;
	case SOUTH:
	   return NORTH;
	   break;
	case UP:
	   return DOWN;
	   break;
	case DOWN:
	   return UP;
	   break;
	default:
	   ErrorLog << "Bad direction to ReverseDirection\r\n" << endl;
	   break;
   }
   return -1;
}

//////////////////////////////
//	Boots shops
/////////////////////////////

void CWorld::BootShops(const CObjectManager *pObjManager, const CMobManager *pMobManager) {
   CString Error, temp, temp1;
   int nCount = 0;

   std::ifstream Index(SHOPINDEX, std::ios_base::in);
   if (!Index) {
	Error.Format("File won't open: %s", SHOPINDEX);
	ErrorLog << Error << endl;
	return;
   }
   MudLog << "booting shops" << endl;
   while (Index.good()) {
	temp = SHOPPREFIX;
	Index >> temp1;
	temp += temp1;
	CAscii CurrentFile(temp, std::ios_base::in);
	if (!CurrentFile) {
	   Error.Format("File would not open! %s", temp.cptr());
	   CError Err(NON_CRITICAL_ERROR, Error.cptr());
	   throw &Err;
	}
	MudLog << "Reading shop file: " << temp << endl;
	while (CurrentFile.good()) {
	   try {
		CRoom *pCheck = NULL;
		CShopRoom ShopTmp, *pShop;
		ShopTmp.ReadShop(CurrentFile, pObjManager);
		if (!m_WorldMap.Lookup(ShopTmp.m_lVnum, pCheck)) {
		   ErrorLog << "no room to put shop in vnum#: " << ShopTmp.m_lVnum << endl;
		} else {
		   nCount++;
		   m_WorldMap.RemoveKey(pCheck->m_lVnum);
		   pShop = new CShopRoom(pCheck, ShopTmp);
		   m_WorldMap.Add(ShopTmp.m_lVnum, pShop);
		   delete pCheck;
		   //load the Shopkeeper in the room
		   if (ShopTmp.m_lShopKeeperVnum != -1) {
			pMobManager->Create(ShopTmp.m_lShopKeeperVnum, pShop);
		   }
		}
	   } catch (CError *Er) {
		if (Er->GetError() == CRITICAL_ERROR) {
		   throw Er;
		} else if (Er->GetError() == NON_CRITICAL_ERROR) {
		   if (!CurrentFile.ReadTill("ENDSHOP")) {
			MudLog << "Went bad in read till" << endl;
			throw Er;
		   } else {
			ErrorLog << Er->GetMessage() << endl;
		   }
		} else if (Er->GetError() == NONERROR) {
		   //perr->DisplayError();
		}
	   }
	}
	CurrentFile.close();
   }
   MudLog << nCount << " shops loaded" << endl;
   MudLog << "size of a shop room is " << int(sizeof(CShopRoom)) << endl;
   Index.close();
}

//////////////////////////
//	BuildMap
//	Put pointers to virtual rooms in 
//	array for map
//

void CWorld::BuildMap() {
   MudLog << "Making virtual map." << endl;
   CString strMap;

   for (int i = 0; i < VIRTUALHEIGHT; i++) {
	for (int j = 0; j < VIRTUALWIDTH; j++) {
	   m_Map[j][i] = FindRoom((i * VIRTUALWIDTH + j) + MIN_VIRTUAL_VNUM);
	   //			strMap+= (m_Map[j][i]->ZoneRoom ? "\x1B[1;37m@\x1B[0;0m" : 
	   //				CRoom::m_RoomColors[m_Map[j][i]->m_nRoomType]);
	}
	//		strMap+="\r\n";
   }
   //	m_strMap=strMap;
   //	*/
   //	m_strMap = "This function has been disabled the world is too big for the screen anyway!\r\n";
}

//////////////////////////////
//	Does Zone have an PC is it

bool CWorld::IsZoneEmpty(int nZone) {
   POSITION pos = m_WorldMap.GetStartingPosition();
   CRoom *pRoom;
   while (pos) {
	pRoom = m_WorldMap.GetNext(pos);
	if (pRoom->IsZone(nZone) &&
		pRoom->IsPCInRoom()) {
	   return false;
	}
   }
   return true;
}

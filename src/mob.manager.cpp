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
/**
 *
 *
 * MobManager
 *
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "mob.manager.h"
#include "global.functions.h"
//#include "wall.h"

#define MOBPREFIX "mobs/"

extern CGlobalVariableManagement GVM;
CCharacter * CalcClassPtr(CMobPrototype *mob, int room);
const CString CMobManager::DEFAULT_MOBINDEX = "mobindex";

//statics
CMap<CString, bool> CMobManager::m_MobNames; //take the defaults

bool CMobManager::IsMobName(CString strName) {
   bool bDummy;
   if (m_MobNames.Lookup(strName, bDummy)) {
	return true;
   }
   return false;
}

///////////////////////////////////////
// non statics

CMobManager::CMobManager(CString strIndex) {
   MOBINDEX = MOBPREFIX;
   MOBINDEX += strIndex;
   try {
	BootMobs();
	MobNames();
   } catch (CError *pError) {
	ErrorLog << pError->GetMessage() << endl;
	if (pError->GetError() == CRITICAL_ERROR) {
	   CError Err(pError->GetError(), "Error caught in CMobManager..rethrown\n");
	   throw &Err;
	}
   }
}

void CMobManager::BootMobs() {
   CString File1;
   char temp[MAX_FILE_LENGTH];
   CString Error;

   CAscii Index(MOBINDEX, std::ios_base::in);
   if (!Index) {
	Error.Format("File would not open: %s", MOBINDEX.cptr());
	CError Err(CRITICAL_ERROR, Error.cptr());
	throw &Err;
   }
   MudLog << "booting mob prototypes" << endl;
   while (Index.good()) {
	File1 = MOBPREFIX;
	Index >> temp;
	File1 += temp;
	MudLog << "Reading file: " << File1 << endl;
	CAscii CurrentFile(File1.cptr(), std::ios_base::in);
	if (!CurrentFile) {
	   Error.Format("File would not open: %s", File1.cptr());
	   CError Err(NON_CRITICAL_ERROR, Error.cptr());
	   throw &Err;
	}
	while (CurrentFile.good()) {
	   try {
		CMobPrototype *pCheck, MobReader;
		MobReader.ReadMob(CurrentFile);
		if (!m_MobMap.Lookup(MobReader.Vnum, pCheck)) {
		   m_MobMap.Add(MobReader.Vnum, new CMobPrototype(&MobReader));
		} else {
		   ErrorLog << "Duplicate mob vnums: " << MobReader.Vnum << endl;
		}
	   } catch (CError *Er) {
		if (Er->GetError() == CRITICAL_ERROR) {
		   throw Er;
		} else if (Er->GetError() == NON_CRITICAL_ERROR) {
		   if (!CurrentFile.ReadTill("ENDMOB")) {
			ErrorLog << "Went bad in read till" << endl;
			throw Er;
		   } else {
			ErrorLog << Er->GetMessage() << endl;
		   }
		} else if (Er->GetError() == NONERROR) {
		   //	perr->DisplayError();
		}
	   }
	}
	CurrentFile.close();
   }
   Index.close();
   MudLog << m_MobMap.GetCount() << " mobs read in." << endl;
}

/////////////////////////////
//	Take all mob names/alias (this names have color excapes in them
//	and put them in a hash table so we can insure
//	we don't have a player with the name of a mob

void CMobManager::MobNames() {
   POSITION pos = m_MobMap.GetStartingPosition();
   CMobPrototype *pMob;
   CString strName, strAlias;
   int nCount;
   while (pos) {
	nCount = 0;
	pMob = m_MobMap.GetNext(pos);
	strAlias = pMob->GetAlias();
	bool bDummy;
	while (!(strName = strAlias.GetWordAfter(nCount)).IsEmpty()) {
	   if (!m_MobNames.Lookup(strName, bDummy)) {
		m_MobNames.Add(strName, true);
	   }
	   nCount++;
	}
   }
   MudLog << m_MobNames.GetCount() << " mob names and aliases loaded" << endl;
}

CMobManager::~CMobManager() {
   POSITION pos = m_MobMap.GetStartingPosition();
   CMobPrototype *pMobProto;
   while (pos) {
	pMobProto = m_MobMap.GetNext(pos);
	delete pMobProto;
   }
}

/////////////////////////////////////////
//	Create (Overloaded)
//	This create creates a mob places the
//	mob in the proper room...it's give the room Vnum
//	NOT real Num like the other create.
//	Adds mob to global LL and room LL
///	written by: Demetrius Comes
////////////////////////////////////////

CCharacter * CMobManager::Create(mob_vnum MobVnum, CRoom *pRoom, int nMax) {
   CCharacter *pNewMob = NULL;
   CMobPrototype *pMobProto;
   if (m_MobMap.Lookup(MobVnum, pMobProto) && pRoom) {
	if (pMobProto->NumInGame < nMax) {
	   pNewMob = CalcClassPtr(pMobProto, pRoom);
	   for (int i = 0; i < pMobProto->GetLevel(); i++) {
		pNewMob->AdvanceLevel(true, false);
	   }
	   GVM.Add(pNewMob);
	   pRoom->Add(pNewMob);
	   pMobProto->NumInGame++;
	   return pNewMob;
	}
	return NULL;
   } else if (pRoom) {
	ErrorLog << "Mob could not be found in prototypes Vnum is: " << MobVnum << endl;
   } else {
	ErrorLog << "Room " << pRoom->GetVnum() << " could not be found to put mob: " << MobVnum << " in" << endl;
   }
   return NULL;
}

/////////////////////////////////////////
//	GetVnums
//	Runs though all mob prototypes building
//	a string of all mobs with strAlias in their
//	alias
//	written by: Demetrius Comes
//////////////////////////////////////////

void CMobManager::GetVnums(CString & strToChar, CString strAlias) {
   POSITION pos = m_MobMap.GetStartingPosition();
   CMobPrototype *pMobProto;
   while (pos) {
	pMobProto = m_MobMap.GetNext(pos);
	if (pMobProto->GetAlias().Find(strAlias) != -1) {
	   strToChar.Format("%s%-40s %d\r\n", strToChar.cptr(),
		   pMobProto->GetName().cptr(), pMobProto->Vnum);
	}
   }
}

////////////////////////////////////////
//	Overloaded create
//	creates a mob in room given without
//	checking for max!  so gods can create
//	more than max
//	Add mob to global LL and Room LL
//	written by:  Demetrius Comes
//	4/12/98
///////////////////////////////////////////

CCharacter * CMobManager::Create(mob_vnum MobVnum, CRoom *pRoom) const {
   CCharacter *pNewMob = NULL;
   CMobPrototype *pMobProto;
   if (m_MobMap.Lookup(MobVnum, pMobProto) && pRoom) {
	pNewMob = CalcClassPtr(pMobProto, pRoom);
	for (int i = 0; i < pMobProto->GetLevel(); i++) {
	   pNewMob->AdvanceLevel(true, false);
	}
	GVM.Add(pNewMob);
	pRoom->Add(pNewMob);
	//Increase the Number in game
	pMobProto->NumInGame++;
	return pNewMob;
   } else if (pRoom) {
	ErrorLog << "Mob could not be found in prototypes Vnum is: " << MobVnum << endl;
   } else {
	ErrorLog << "Room could not be found to put mob in" << endl;
   }
   return NULL;
}

////////////////////////////////////////
//	Overloaded create
//	creates a mob in room given without
//	checking for max!  so gods can create
//	more than max and can set number of hp
//	Add mob to global LL and Room LL
//	written by:  John Comes (Copied)
//	12/24/98
///////////////////////////////////////////

CCharacter * CMobManager::Create(mob_vnum MobVnum, CRoom *pRoom, int nHp, int nLevel) const {
   CCharacter *pNewMob = NULL;
   CMobPrototype *pMobProto;
   if (m_MobMap.Lookup(MobVnum, pMobProto) && pRoom) {
	pNewMob = CalcClassPtr(pMobProto, pRoom);
	for (int i = 0; i < nLevel; i++) {
	   pNewMob->AdvanceLevel(true, false);
	}
	GVM.Add(pNewMob);
	pRoom->Add(pNewMob);
	//Increase the Number in game
	pMobProto->NumInGame++;
	pNewMob->SetHits(nHp);
	return pNewMob;
   } else if (pRoom) {
	ErrorLog << "Mob could not be found in prototypes Vnum is: " << MobVnum << endl;
   } else {
	ErrorLog << "Room could not be found to put mob in" << endl;
   }
   return NULL;
}

/////////////////////////////////////////////
//	Vstat
//	Builds a CString of information about mob
//	Find mob, build string with information
//	written by: Demetrius Comes
//	4/27/98
////////////////////////////////////////////

void CMobManager::VStat(CString & ToChar, mob_vnum lVnum) {
   CMobPrototype *pMobProto;
   if (!m_MobMap.Lookup(lVnum, pMobProto)) {
	ToChar = "There is no mob with that vnum\r\n";
	return;
   }
   pMobProto->VStat(ToChar);
}

//////////////////////////
//	Creates an undead mob
//	
//	written by: John Comes 1-7-99
////////////////////////////

CCharacter * CMobManager::CreateUndead(const sUndeadInfo *pUndeadInfo, CRoom *pRoom) {
   sUndeadInfo CheckLevelInfo = (*pUndeadInfo);
   CheckLevelInfo.m_nLevel = (CheckLevelInfo.m_nUndeadType == VNUM_UNDEAD_SKELETON && CheckLevelInfo.m_nLevel > 15) ? 15 : CheckLevelInfo.m_nLevel;
   CheckLevelInfo.m_nLevel = (CheckLevelInfo.m_nUndeadType == VNUM_UNDEAD_ZOMBIE && CheckLevelInfo.m_nLevel > 25) ? 25 : CheckLevelInfo.m_nLevel;
   CheckLevelInfo.m_nLevel = (CheckLevelInfo.m_nUndeadType == VNUM_UNDEAD_SPECTRE && CheckLevelInfo.m_nLevel > 30) ? 30 : CheckLevelInfo.m_nLevel;
   CheckLevelInfo.m_nLevel = (CheckLevelInfo.m_nUndeadType == VNUM_UNDEAD_GHOST && CheckLevelInfo.m_nLevel > 35) ? 35 : CheckLevelInfo.m_nLevel;
   CheckLevelInfo.m_nLevel = (CheckLevelInfo.m_nUndeadType == VNUM_UNDEAD_GHOUL && CheckLevelInfo.m_nLevel > 40) ? 40 : CheckLevelInfo.m_nLevel;
   CheckLevelInfo.m_nLevel = (CheckLevelInfo.m_nUndeadType == VNUM_UNDEAD_WIGHT && CheckLevelInfo.m_nLevel > 40) ? 40 : CheckLevelInfo.m_nLevel;

   CMobPrototype NewMob(&CheckLevelInfo);
   CCharacter *pNewMob = CalcClassPtr(&NewMob, pRoom);

   for (int i = 0; i < NewMob.GetLevel(); i++) {
	pNewMob->AdvanceLevel(true, false, true);
   }
   GVM.Add(pNewMob);
   pNewMob->GetRoom()->Add(pNewMob);
   return pNewMob;
}

//////////////////////////
//	Creates a Wall
//	
//	written by: John Comes 1-13-99
////////////////////////////
#include "npc.classes.h"
#include "wall.h"

CCharacter * CMobManager::CreateWall(const sWallInfo *pWallInfo, CRoom *pRoom) {
   CMobPrototype NewMob(pWallInfo);
   sWallInfo OtherSide = (*pWallInfo);
   OtherSide.m_nDirection = CWorld::ReverseDirection(pWallInfo->m_nDirection);
   CMobPrototype OtherSideMob(&OtherSide);
   //walls add themselfs to the room
   CCharacter *pNewMob = new CWall<CNPCWarrior>(pWallInfo, NewMob, OtherSideMob, pRoom);
   GVM.Add(pNewMob);
   for (int i = 0; i < NewMob.GetLevel(); i++) {
	pNewMob->AdvanceLevel(true, false);
   }
   return pNewMob;
}

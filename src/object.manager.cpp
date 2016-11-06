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
 * CObjectManager
 *
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "object.manager.h"
#include "object.save.h"
#include "light.h"
#include "transport.h"
#include "transportroom.h"
#include "armor.h"
#include "portal.h"
#include "object.misc.h"
#define OBJPREFIX "objs/"

CMap<int, CObjectManager::sSpecialProc *> CObjectManager::m_SpecialProcs;
bool CObjectManager::m_bInit = false;
CLinkList<CObject> CObjectManager::ObjectsInMud;
CList<CCorpse *> CObjectManager::CorpsesInMud;
const CString CObjectManager::DEFAULT_OBJINDEX = "objectindex";
const char *CObjectManager::CORPSE_FILE = "players/corpses.wolfshade";

///////////////////////////
//	statics
////////////////////////////

/////////////////////////////
//	Save Corpses
////////////////////////////

void CObjectManager::SaveCorpses() {
   std::ofstream CorpseFile(CORPSE_FILE, std::ios_base::binary | std::ios_base::out);
   if (!CorpseFile) {
	ErrorLog << "Error opening corpse file!\r\n" << endl;
   } else {
	POSITION pos = CorpsesInMud.GetStartPosition();
	CObject *pObj;
	while (pos) {
	   pObj = CorpsesInMud.GetNext(pos);
	   //the save also saves all contents
	   pObj->Save(CorpseFile, sObjectSave::IN_ROOM, sObjectSave::IN_ROOM);
	}
	CorpseFile.close();
   }
}

/////////////////////////////////////
//	Called once to load all saved corpes into 
//	proper rooms

void CObjectManager::LoadCorpses() {
   std::ifstream CorpseFile(CORPSE_FILE, std::ios_base::binary | std::ios_base::in);
   if (!CorpseFile) {
	MudLog << "No corpses to load..." << endl;
   } else {
	CorpseFile.seekg(0, std::ios_base::end);
	long lSize = CorpseFile.tellg();
	if (lSize % (sizeof(sObjectSave))) {
	   ErrorLog << "Corpse file is corrupt..." << endl;
	} else if (lSize == 0) {
	   MudLog << "No corpses to load..." << endl;
	} else {
	   int nCount = 0;
	   CorpseFile.seekg(0, std::ios_base::beg);
	   sObjectSave ObjSave;
	   CObject *pCorpse;
	   CRoom *pPutInRoom;
	   while (CorpseFile.good() && lSize != CorpseFile.tellg()) {
		nCount++;
		CorpseFile.read((char *) &ObjSave, sizeof(sObjectSave));
		pPutInRoom = CWorld::FindRoom(ObjSave.GetSaveRoom());
		if (pPutInRoom == NULL) {
		   ErrorLog << pCorpse->GetObjName() << " can be loaded into room#: " << ObjSave.GetSaveRoom() << endl;
		} else {
		   pCorpse = new CCorpse(ObjSave, pPutInRoom);
		   //add to game LL
		   //must use false for add so we don't save the
		   //save file we are reading
		   //add also adds the corpses to corpses in mud ll
		   Add(pCorpse, false);
		   pCorpse->LoadContents(CorpseFile, ObjSave.GetContentsCount());
		   //add to room ll
		   pPutInRoom->Add(pCorpse);
		}
	   }
	   MudLog << nCount << " corpses loaded." << endl;
	}
   }
   CorpseFile.close();
}

///////////////////////////
//	Adds objects to "global LL" (acutally static)
//  and if corpse adds it to corpse ll.
//  bool bCanSaveCorpses added to insure than LoadContents
//	when game is booting doesn't happen to over write the
//	file we are trying to load corpses in from...
//	normally a bad idea
//  we could have just not save corpses in this function
//	but then we'd have to always check every object to see
//	if it's a corpse and save it before we delete it...
//	so this seemed the better of the two solutions

void CObjectManager::Add(CObject *pObj, bool bCanSaveCorpses) {
   ObjectsInMud.Add(pObj);
   if (pObj->IsType(ITEM_TYPE_CORPSE) &&
	   ((CCorpse *) pObj)->WasPc()) {
	CorpsesInMud.Add((CCorpse *&) pObj);
	if (bCanSaveCorpses) {
	   SaveCorpses();
	}
   }
}

//////////////////////////
//	public overload

void CObjectManager::Remove(CObject *pObj) {
   if (pObj->IsType(ITEM_TYPE_CORPSE) &&
	   ((CCorpse *) pObj)->WasPc()) {
	CorpsesInMud.Remove((CCorpse *&) pObj);
	SaveCorpses();
   }
   ObjectsInMud.Remove(pObj);
}

//////////////////////////
//	private overload

void CObjectManager::Remove(CObject *pObj, POSITION pos) {
   if (pObj->IsType(ITEM_TYPE_CORPSE) &&
	   ((CCorpse *) pObj)->WasPc()) {
	CorpsesInMud.Remove((CCorpse *&) pObj);
	SaveCorpses();
   }
   //must remove from objectsinmud after we remove from corpses in mud
   //because objects in mud deletes it's data
   ObjectsInMud.Remove(pObj, pos);
}

CObjectManager::CObjectManager(CString strIndex) {
   InitStatics();
   OBJINDEX = OBJPREFIX;
   OBJINDEX += strIndex;
   try {
	BootObjects();
   } catch (CError *pError) {
	ErrorLog << pError->GetMessage() << endl;
	if (pError->GetError() == CRITICAL_ERROR) {
	   CError Err(pError->GetError(), "Error caught in CObjectManager..rethrown\n");
	   throw &Err;
	}
   }
}

void CObjectManager::BootObjects() {
   CString File1;
   char temp[MAX_FILE_LENGTH];
   CString Error;

   CAscii Index(OBJINDEX, std::ios_base::in);
   if (!Index) {
	Error.Format("File would not open:  %s", OBJINDEX.cptr());
	CError Err(CRITICAL_ERROR, Error.cptr());
	throw &Err;
   }
   MudLog << "booting object prototypes" << endl;
   while (Index.good()) {
	File1 = OBJPREFIX;
	Index >> temp;
	File1 += temp;
	MudLog << "booting object file: " << File1 << endl;
	CAscii CurrentFile(File1, std::ios_base::in);
	if (!CurrentFile) {
	   Error.Format("File would not open: %s", File1.cptr());
	   CError Err(CRITICAL_ERROR, Error.cptr());
	   throw &Err;
	}
	while (CurrentFile.good()) {
	   try {
		CObjectPrototype *pCheck, ObjectReader;
		ObjectReader.ReadObj(CurrentFile);
		if (!m_ObjectPrototypeMap.Lookup(ObjectReader.Vnum, pCheck)) {
		   m_ObjectPrototypeMap.Add(ObjectReader.Vnum, new CObjectPrototype(&ObjectReader));
		} else {
		   ErrorLog << "Two objects have the save vnum: " << ObjectReader.Vnum << endl;
		}
	   } catch (CError *Er) {
		if (Er->GetError() == CRITICAL_ERROR) {
		   throw Er;
		} else if (Er->GetError() == NON_CRITICAL_ERROR) {
		   if (!CurrentFile.ReadTill("ENDOBJ")) {
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
   MudLog << m_ObjectPrototypeMap.GetCount() << " object prototypes read in." << endl;
}

CObjectManager::~CObjectManager() {
   CObjectPrototype *pObjProto;
   POSITION pos = m_ObjectPrototypeMap.GetStartingPosition();
   while (pos) {
	pObjProto = m_ObjectPrototypeMap.GetNext(pos);
	delete pObjProto;
   }
   pos = m_SpecialProcs.GetStartingPosition();
   while (pos) {
	delete m_SpecialProcs.GetNext(pos);
   }
   SaveCorpses();
}

/////////////////////////////////////////////
// Creates an object from a vnum
//	Add the obj to the global LL then adds it
//	to the CCharacter's Inventory.
//	written by: Demetrius Comes
//	pch can be NULL then we just return the new object
//	and let the calling function worry about putting on character
//	or whatever
////////////////////////////////////////////	

CObject * CObjectManager::Create(obj_vnum nObjVnum, CCharacter *pCh) {
   CObjectPrototype *pProto;

   if (!m_ObjectPrototypeMap.Lookup(nObjVnum, pProto)) {
	ErrorLog << "Trying to create an object we don't have a prototype for Object Vnum = " << nObjVnum << endl;
	return NULL;
   }

   CObject *NewObj = NULL;

   NewObj = CalObjectPointer(pProto, pCh, NULL);

   if (NewObj) {
	if (NewObj->IsType(ITEM_TYPE_TRANSPORT)) {
	   pCh->GetRoom()->Add(NewObj, false);
	} else if (pCh != NULL && !pCh->Add(NewObj)) {
	   ErrorLog << "Obj Vnum#: " << NewObj->GetVnum()
		   << " can't be added to character: " << pCh->GetName() << endl;
	   delete NewObj;
	   NewObj = NULL;
	}
   } else {
	ErrorLog << nObjVnum << " could not be created." << endl;
   }
   if (NewObj) {
	CObjectManager::Add(NewObj);
	//Increase the number in the game
	pProto->NumberInGame++;
	DoSpecialProc(NewObj);
   }
   return NewObj;
}

////////////////////////////////////
//  GetVnums
//	Builds a string of Objects Names and VNums
//	written by: Demetrius Comes
////////////////////////////////////

void CObjectManager::GetVnums(CString &strToChar, CString strAlias) {
   POSITION pos = m_ObjectPrototypeMap.GetStartingPosition();
   CObjectPrototype *pProto;
   while (pos) {
	pProto = m_ObjectPrototypeMap.GetNext(pos);
	if (pProto->ObjAlias.Find(strAlias) != -1) {
	   strToChar.Format("%s%s %d\r\n", strToChar.cptr(),
		   pProto->ObjName.cptr(), pProto->Vnum);
	}
   }
}

////////////////////////////////////////////
//	Create
//	Overloaded to create an object 
//	and add it to a room's ObjInRoom Link List
//	written by: Demetrius Comes
////////////////////////////////////////////

CObject * CObjectManager::Create(obj_vnum nObjVnum, CRoom *pRealRoom, short nMax) {
   CObjectPrototype *pProto;
   if (!m_ObjectPrototypeMap.Lookup(nObjVnum, pProto)) {
	ErrorLog << "Trying to create an object we don't have a prototype for ObjectVnum = " << nObjVnum << endl;
	return NULL;
   }

   //CRoom * pRealRoom = CWorld::FindRoom(nRoomVnum);

   if (!pRealRoom) {
	ErrorLog << "Trying to place an object in a non-existant room!\r\n" << endl;
	return NULL;
   }

   if (pProto->NumberInGame < nMax || nMax == -1) {
	CObject *NewObj = CalObjectPointer(pProto, NULL, pRealRoom);

	if (NewObj) {
	   CObjectManager::Add(NewObj);
	   pRealRoom->Add(NewObj);
	   //Increase the number in the game
	   pProto->NumberInGame++;
	   DoSpecialProc(NewObj);
	   return NewObj;
	} else {
	   ErrorLog << nObjVnum << " could not be created." << endl;
	}
   }
   return NULL;
}

void CObjectManager::VStat(CString & ToChar, int nVnum) {
   CObjectPrototype *pProto;
   if (!m_ObjectPrototypeMap.Lookup(nVnum, pProto)) {
	ToChar = "There is no object with that vnum\r\n";
	return;
   }
   pProto->VStat(ToChar);
}

void CObjectManager::CreatePortal(CRoom *pTargetRoom, CRoom *pCasterRoom, short nAffect, int nPulses) {
   CObjectPrototype pProto(nAffect, pTargetRoom, nPulses);
   CObjectPrototype pProto1(nAffect, pCasterRoom, nPulses);

   CObject *pNewObj1 = NULL, *pNewObj2 = NULL;
   pNewObj1 = CalObjectPointer(&pProto1, NULL, pTargetRoom);
   pNewObj2 = CalObjectPointer(&pProto, NULL, pCasterRoom);
   if (pNewObj1 != NULL && pNewObj2 != NULL) {
	CObjectManager::Add(pNewObj1);
	CObjectManager::Add(pNewObj2);
	pTargetRoom->Add(pNewObj1);
	pCasterRoom->Add(pNewObj2);
   } else {
	ErrorLog << "Portal of type " << nAffect << "could not be created." << endl;
   }
}

///////////////////////////
//	Create Fireball
//	Creates a Delayed Blast Fireball.
//	John Comes: 2-11-99

void CObjectManager::CreateFireball(CRoom *pTargetRoom, int nPulses, int nDam) {
   CObjectPrototype pProto(CObject::OBJ_AFFECT_DELAYED_BLAST_FIREBALL, nDam);
   CObject *pNewObj = NULL;
   CObjectManager::Add(pNewObj = new CObject(&pProto, NULL, pTargetRoom));
   pTargetRoom->Add(pNewObj);
   pNewObj->AddAffect(pNewObj->OBJ_AFFECT_DELAYED_BLAST_FIREBALL, nPulses, nDam);
}



/////////////////////////////////////////
//	Initialize m_SpecialProcs
//	this is a map of object vnum to the
//	special proc function to be called 
//	when object is created.
//
//  This should be called run once not init statics
//	Because LoadCorpses in NOT static

void CObjectManager::InitStatics() {
   if (m_bInit) {
	return;
   }
   m_bInit = true;
   ObjectsInMud.SetDeleteData();
   m_SpecialProcs.Add(2, new sSpecialProc(&CObjectManager::AddGeneralProc));
   m_SpecialProcs.Add(3, new sSpecialProc(&CObjectManager::AddGeneralProc));
   m_SpecialProcs.Add(4, new sSpecialProc(&CObjectManager::AddGeneralProc));
   m_SpecialProcs.Add(5, new sSpecialProc(&CObjectManager::AddGeneralProc));
   m_SpecialProcs.Add(6, new sSpecialProc(&CObjectManager::AddGeneralProc));
   m_SpecialProcs.Add(26, new sSpecialProc(&CObjectManager::AddGeneralProc));
}

void CObjectManager::AddGeneralProc(CObject * pObj) {
   switch (pObj->GetVnum()) {
	case 2:
	   pObj->AddAffect(CObject::OBJ_AFFECT_CAST_MAGIC_MISSILE, sAffect::PERM_AFFECT);
	   break;
	case 3:
	   pObj->AddAffect(CObject::OBJ_AFFECT_HASTE, sAffect::PERM_AFFECT);
	   break;
	case 4:
	   pObj->AddAffect(CObject::OBJ_AFFECT_DEATHS_MATRIARCH, sAffect::PERM_AFFECT);
	   pObj->AddAffect(CObject::OBJ_AFFECT_HASTE, sAffect::PERM_AFFECT);
	   pObj->AddAffect(CObject::OBJ_AFFECT_HIDE, sAffect::PERM_AFFECT);
	   pObj->AddAffect(CObject::OBJ_AFFECT_COLD_SHIELD, sAffect::PERM_AFFECT);
	   pObj->AddAffect(CObject::OBJ_AFFECT_DETECT_INVIS, sAffect::PERM_AFFECT);
	   break;
	case 5:
	   pObj->AddAffect(CObject::OBJ_AFFECT_DIVINE_FATE, sAffect::PERM_AFFECT);
	   pObj->AddAffect(CObject::OBJ_AFFECT_HASTE, sAffect::PERM_AFFECT);
	   pObj->AddAffect(CObject::OBJ_AFFECT_DETECT_INVIS, sAffect::PERM_AFFECT);
	   pObj->AddAffect(CObject::OBJ_AFFECT_AIR_SHIELD, sAffect::PERM_AFFECT);
	   break;
	case 6:
	   pObj->AddAffect(CObject::OBJ_AFFECT_SHADOW_ARMOR, sAffect::PERM_AFFECT);
	   break;
	case 26:
	   pObj->AddAffect(CObject::OBJ_AFFECT_BOX, sAffect::PERM_AFFECT);
	   break;
	default:
	   ErrorLog << "default in CObjectManager::AddGerneralProc() " << endl;
	   break;
   }
}

////////////////////////////////////
//	Calls the special procs

void CObjectManager::DoSpecialProc(CObject * pObj) {
   sSpecialProc *pSproc;
   if (m_SpecialProcs.Lookup(pObj->GetVnum(), pSproc)) {
	(this->*(pSproc->m_pFncSpecialPrc))(pObj);
   }
}

////////////////////////////////
//	ReduceObjectAffects
//	Calls FadeAffects for each object in the 
//	objectsinmud LL
//	written by: Demetrius Comes
//////////////////////////////

void CObjectManager::ReduceObjectAffects(long lToReduce) {
   POSITION pos = ObjectsInMud.GetStartPosition();
   CObject *pObj;
   while (pos) {
	pObj = ObjectsInMud.GetNext(pos);
	pObj->FadeAffects(lToReduce);
	if (pObj->ShouldBeDeleted()) {
	   //calls the private version of REmove
	   //which will remove the corpse if need be
	   Remove(pObj, pos);
	}
   }
}

CObject * CObjectManager::CalObjectPointer(CObjectPrototype *obj, CCharacter *PersonCarriedby, CRoom *pRoom) {
   CObject *pObj;
   switch (obj->ObjType) {
	case ITEM_TYPE_CORPSE:
	   pObj = NULL;
	   break;
	case ITEM_TYPE_CONTAINER:
	   pObj = (new CContainer(obj, PersonCarriedby, pRoom));
	   break;
	case ITEM_TYPE_LIGHT:
	   pObj = (new CLight(obj, PersonCarriedby, pRoom));
	   break;
	case ITEM_TYPE_ARMOR:
	   pObj = (new CArmor(obj, PersonCarriedby, pRoom));
	   break;
	case ITEM_TYPE_PORTAL:
	   pObj = (new CPortal(obj, pRoom));
	   break;
	case ITEM_TYPE_POTION:
	   pObj = (new CPotion(obj, PersonCarriedby, pRoom));
	   break;
	case ITEM_TYPE_SCROLL: case ITEM_TYPE_STAFF:
	case ITEM_TYPE_FOUNTAIN:case ITEM_TYPE_WEAPON:
	case ITEM_TYPE_SPELL_OBJ:
	   pObj = (new CObject(obj, PersonCarriedby, pRoom));
	   break;
	case ITEM_TYPE_TRANSPORT:
	   pObj = (new CTransport(obj, pRoom));
	   break;
	default:
	   ErrorLog << "Default in calobjectpointer!" << endl;
	   pObj = NULL;
   }
   if (pObj != NULL && pObj->IsValidObject()) {
	return pObj;
   } else {
	return NULL;
   }
}

////////////////////////////////
//  give a saved object pass back the object pointer
//	each object must pass the validity check

CObject * CObjectManager::CalObjectPointer(const sObjectSave & ObjSave, CCharacter *pChar, CRoom *pRoom) {
   CObject *pObj;
   switch (ObjSave.m_nObjType) {
	case ITEM_TYPE_CORPSE:
	   pObj = (new CCorpse(ObjSave, pRoom));
	   break;
	case ITEM_TYPE_CONTAINER:
	   pObj = (new CContainer(ObjSave, pChar, pRoom));
	   break;
	case ITEM_TYPE_LIGHT:
	   pObj = (new CLight(ObjSave, pChar, pRoom));
	   break;
	case ITEM_TYPE_ARMOR:
	   pObj = (new CArmor(ObjSave, pChar, pRoom));
	   break;
	case ITEM_TYPE_POTION:
	   pObj = (new CPotion(ObjSave, pChar, pRoom));
	   break;
	case ITEM_TYPE_SCROLL: case ITEM_TYPE_STAFF:
	case ITEM_TYPE_FOUNTAIN:case ITEM_TYPE_WEAPON:
	case ITEM_TYPE_SPELL_OBJ:
	   pObj = (new CObject(ObjSave, pChar, pRoom));
	   break;
	case ITEM_TYPE_PORTAL:
	   ErrorLog << "A Saved Portal is trying to be created." << endl;
	   pObj = NULL;
	   break;
	default:
	   ErrorLog << "Default in calobjectpointer!" << endl;
	   pObj = NULL;
   }
   if (pObj != NULL && pObj->IsValidObject()) {
	return pObj;
   } else {
	return NULL;
   }
}

/////////////////////////////////////////
//	Locate Obj..
//	hunt the static LL of objects in the mud
//	for each object matching the str passed in
//	tell who it is carriedby, or what room it is in
//	or if it is in some kind of container
/////////////////////////////////////////

CString CObjectManager::LocateObject(CString strAlias, const CCharacter *pCh) {
   POSITION pos = ObjectsInMud.GetStartPosition();
   CObject *pObj;
   CString str;
   while (pos) {
	pObj = ObjectsInMud.GetNext(pos);
	if (pCh->CanSee(pObj) && pObj->ByAlias(strAlias)) {
	   if (pObj->GetRoom()) {
		str.Format("%s%s is in room vnum: %d\r\n",
			str.cptr(),
			pObj->GetObjName(pCh),
			pObj->GetRoom()->GetVnum());
	   } else if (pObj->m_pCarriedby != NULL) {
		if (pObj->IsAffectedBy(CObject::OBJ_AFFECT_CURRENTLY_WORN)) {
		   str.Format("%s%s is currently worn by %s\r\n",
			   str.cptr(),
			   pObj->GetObjName(pCh),
			   pObj->m_pCarriedby->GetName().cptr());
		} else {
		   str.Format("%s%s is currently carried by %s\r\n",
			   str.cptr(),
			   pObj->GetObjName(pCh),
			   pObj->m_pCarriedby->GetName().cptr());
		}
	   } else {
		str.Format("%s%s is in some sort on container.\r\n",
			str.cptr(),
			pObj->GetObjName(pCh));
	   }
	}
   }
   return str;
}

///////////////////////////////////
//	Gets object name from prototype given a vnum
//	used for shops

CString CObjectManager::GetObjName(obj_vnum lObjVnum) const {
   const CObjectPrototype *pObj = operator[](lObjVnum);
   if (pObj == NULL) {
	ErrorLog << "GetObjName was given an illegal object vnum" << endl;
	return "";
   } else {
	return CString(pObj->ObjName);
   }
}

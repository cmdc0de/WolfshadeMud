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
*	CZoneManagement
*
*
*/

#include "stdinclude.h"
#pragma hdrstop
#include "linklist.h"
#include "zone.manager.h"

#define ZONEPREFIX "zones/"

const CString CZoneManager::DEFAULT_ZONEINDEX("zoneindex");
const short CZoneManager::ZONE_NORMAL_RESET=0;
const short CZoneManager::ZONE_NEVER_RESET=1;
const short CZoneManager::ZONE_ON_EMPTY_RESET=2;
const char *strResetType[] = 
{
	"Normal",
	"Never",
	"If Empty"
};

extern CGlobalVariableManagement GVM;

CZoneManager::CZoneManager(CString strIndex)
{
	ZONEINDEX=ZONEPREFIX;
	ZONEINDEX+=strIndex;
	try
	{
		BootZones();
	}
	catch(CError *pError)
	{
		ErrorLog << pError->GetMessage() << endl;
		if(pError->GetError()==CRITICAL_ERROR)
		{
			CError Err(pError->GetError(),"Error caught in CZoneManagement..rethrown\n");
			throw &Err;
		}
	}
}

CZoneManager::~CZoneManager()
{
	POSITION pos = m_ZoneMap.GetStartingPosition();
	CZone *pZone;
	while(pos)
	{
		pZone = m_ZoneMap.GetNext(pos);
		delete pZone;
	}
}

void CZoneManager::BootZones()
{
	int count = 0; 
	//CAscii InFile(ZONEINDEX,std::ios::in|ios::nocreate);
	CAscii InFile(ZONEINDEX,std::ios::in);
	CString FileName;
	char temp[MAX_FILE_LENGTH];

	if(!InFile)
	{
		CError Err(CRITICAL_ERROR,"Error zoneindex not opening!");
		throw &Err;
	}
	
	CString strColorizedZoneName;
	while(InFile.good())
	{
		FileName = ZONEPREFIX;
		InFile >> temp;
		FileName+=temp;
		MudLog << "Reading zone file: " << FileName << endl;
		try
		{
			CZone *pCheck,ZoneReader;
			ZoneReader.ReadZone(FileName);
			if(!m_ZoneMap.Lookup(ZoneReader.ZoneNum,pCheck))
			{
				strColorizedZoneName = ZoneReader.ZoneName;
				strColorizedZoneName.Colorize(false);
				MudLog << strColorizedZoneName << " read." << endl;
				m_ZoneMap.Add(ZoneReader.ZoneNum,new CZone(&ZoneReader));
			}
			else
			{
				ErrorLog << "Duplicate Zone numbers: " << ZoneReader.ZoneNum << endl;
			}
		}
		catch(...)
		{
			CString Error;
			Error.Format("Problem in Loading Zones. Bad File is %s",
				(char *)FileName);
			CError Err(CRITICAL_ERROR,Error);
			throw &Err;
		}
	}
	if(!InFile.eof())
	{
		ErrorLog << "EOF not reached for ZoneIndex File";
	}
	InFile.close();
	MudLog << m_ZoneMap.GetCount() << " zones read." << endl;
}

///////////////////////////////////////
//	ResetZones
//	Called to reset check if a zone needs
//	reset and does the resetting
//	written by: Demetrius Comes
/////////////////////////////////////////
void CZoneManager::ResetZones(bool bResetAll, CObjectManager &ObjManager,CMobManager &MobManager)
{
	POSITION pos = m_ZoneMap.GetStartingPosition();
	CZone *pZone;
	while(pos)
	{
		pZone = m_ZoneMap.GetNext(pos);
		//subtract a minute off the times
		//all timers work by the minute.
		if(bResetAll || ((--pZone->Timer <= 0)
			&& (!pZone->ResetModeIs(ZONE_NEVER_RESET))))
		{
			if((!pZone->ResetModeIs(ZONE_ON_EMPTY_RESET))
				|| CWorld::IsZoneEmpty(pZone->GetZoneNum()))
			{
				CString str;
				MudLog << "resetting zone " << pZone->ZoneName << endl(str);
				GVM.SendToGods(str,CCharacterAttributes::PREFERS_GOD_LOG_ZONES,NULL);
				ResetDoors(pZone);
				ResetMobs(pZone,MobManager,ObjManager,bResetAll);
				ResetRoomObjects(pZone,ObjManager);
				RemoveObjectsFromRoom(pZone,ObjManager);
				ResetRoomCash(pZone,bResetAll);
			}
			pZone->ResetTimer();
		}
	}
}

/////////////////////////////////////////
//	ResetDoors
//	Resets the doors to the way to zone file is set up
//	written by: Demetrius Comes
///////////////////////////////////////////
void CZoneManager::ResetDoors(CZone *pZone)
{
	CZone::sDoorFlag *currentDoor;
	currentDoor = NULL;
	POSITION pos = pZone->Doors.GetStartPosition();
	CRoom *pRoom;
	while(pos)
	{
		currentDoor = pZone->Doors.GetNext(pos);
		if((pRoom = CWorld::FindRoom(currentDoor->RoomVnum)))
		{
			if(!pRoom->m_Doors[currentDoor->Dir])
			{
				ErrorLog << "Attempting to set a door that doesn't exist in Room " 
					<< currentDoor->Dir << endl;
			}
			else
			{
				pRoom->m_Doors[currentDoor->Dir]->DoorType = 
					currentDoor->State;
			}
		}
		else
		{
			ErrorLog << "Attempting to reset a door in a room that doesn't exist.  Room # used: " << currentDoor->RoomVnum << endl;
		}
	}
}

//////////////////////////////////////////////////////
//	ResetMobs
//	Cycle though MobCommands in zone set up, for each mob
//	check to see if we need to load objects to the mob
//	The MobManager::Create adds Mob to the Room
//	written by: Demetrius Comes
///////////////////////////////////////////////////////
void CZoneManager::ResetMobs(CZone *pZone,CMobManager &MobManager,CObjectManager &ObjectManager,bool bResetAll)
{
	CZone::sMobCommands *currentMob = NULL;
	POSITION pos = pZone->MobCommands.GetStartPosition();
	CCharacter *NewMob;
	CZone::sMobCommands::sMobObj *currentMobObj = NULL;
	POSITION posMobObj;
	CObject *NewObj;
	while(pos)
	{
		currentMob = pZone->MobCommands.GetNext(pos);
		//create adds mob to room as well
		if((NewMob = MobManager.Create(currentMob->MobVNum,
			CWorld::FindRoom(currentMob->RoomVNum),currentMob->Max)))
		{
			//Add EQ to NewMob
			posMobObj = currentMob->MobObjs.GetStartPosition();
			while(posMobObj)
			{
				currentMobObj = currentMob->MobObjs.GetNext(posMobObj);
				//2 = 2:mob get object only and wear it when he does
				//3:mob has gets object only once and put it in inventory when he does
				if((currentMobObj->EqPos!=2 && currentMobObj->EqPos!=3)
					|| bResetAll)
				{
					if((NewObj = ObjectManager.Create(currentMobObj->ObjVnum,NewMob)))
					{
						//Object manager adds object to mob's inventory!
						if(currentMobObj->EqPos != 1
							&& currentMobObj->EqPos!=3) //1 means keep in inventory
						{
							NewMob->Wear(NewObj);
						}
					}
				}
			}
		}
	}
}

///////////////////////////
//	RemoveObjectsFromRoom
//	Remove object from rooms that shouldn't be there.
//	written by: Demetrius Comes
///////////////////////////
void CZoneManager::RemoveObjectsFromRoom(CZone *pZone,CObjectManager &ObjManager)
{
	CZone::sObjRemove *ObjTobeRemoved = NULL;
	POSITION pos = pZone->ObjsToRemove.GetStartPosition();
	CObject *temp;
	CRoom *pRoom;
	while(pos)
	{
		ObjTobeRemoved = pZone->ObjsToRemove.GetNext(pos);
		if((pRoom = CWorld::FindRoom(ObjTobeRemoved->RoomVNum)))
		{
			while((temp = pRoom->FindObjectInRoom(ObjTobeRemoved->ObjVNum)))
			{
				pRoom->Remove(temp);
				CObjectManager::Remove(temp);
			}
		}
	}
}

//////////////////////
//	ResetRoomObjects
//	Cycle link list and create object in room
/////////////////////
void CZoneManager::ResetRoomObjects(CZone *pZone,CObjectManager &ObjManagement)
{
	CZone::sRoomObj *ObjTobe = NULL;
	POSITION pos = pZone->RoomObjs.GetStartPosition();
	while(pos)
	{
		ObjTobe = pZone->RoomObjs.GetNext(pos);
		ObjManagement.Create(ObjTobe->ObjVNum,CWorld::FindRoom(ObjTobe->RoomVNum),ObjTobe->Max);
	}
}

////////////////////
//	Resets cash in rooms
////////////////////
void CZoneManager::ResetRoomCash(CZone * pZone, bool bResetAll)
{
	POSITION pos = pZone->RoomCash.GetStartPosition();
	CZone::sRoomCash *pRoomCash = NULL;
	CRoom *pRoom;
	while(pos)
	{
		pRoomCash = pZone->RoomCash.GetNext(pos);
		if(bResetAll || pRoomCash->m_bLoadEveryTime)
		{
			pRoom = CWorld::FindRoom(pRoomCash->m_lRoomVnum);
			if(pRoom)
			{
				pRoom->AddCash(pRoomCash->m_lBaseAmt);
			}
		}
	}
}

/////////////////////////////////////////////
//	GetZoneInformation
//	Get all the information we can about all the zones
//	
CString CZoneManager::GetZoneInformation()
{
	CString strInfo("Zone#     Zone Name                           Time      Left      Reset-Mode\r\n");
	POSITION pos = m_ZoneMap.GetStartingPosition();
	CZone *pZone;
	while(pos)
	{
		pZone = m_ZoneMap.GetNext(pos);
		strInfo.Format("%s%-10d%-40s%-5d%-5d%s\r\n",
			(char *)strInfo,
			pZone->ZoneNum,
			(char *)pZone->ZoneName,
			pZone->LifeSpan,
			pZone->Timer,
			strResetType[pZone->ResetMode]);
	}
	return strInfo;
}

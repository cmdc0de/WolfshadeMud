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
 *  CZone Implemetation
 *
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include <stdlib.h>
#include <ctype.h>
#include "zone.h"
#include "log.h"

CZone::CZone() {
   MobCommands.SetDeleteData();
   ObjsToRemove.SetDeleteData();
   RoomObjs.SetDeleteData();
   Doors.SetDeleteData();
   RoomCash.SetDeleteData();
   LifeSpan = -1;
}

void CZone::ReadZone(CString FileName) {
   char CommandType, Throwaway[MAX_FILE_STRING_SIZE];
   sMobCommands *MobCom = NULL;
   sMobCommands::sMobObj *ObjForMob = NULL;
   sObjRemove *RemovalObj = NULL;
   sRoomObj *ObjToPutInRoom = NULL;
   sDoorFlag *DoorFlags = NULL;
   sRoomCash *pRoomCash = NULL;
   char flag[33];

   //	CAscii CurrentFile(FileName,ios::in|ios::nocreate);
   CAscii CurrentFile(FileName, std::ios::in);

   CurrentFile.Numeric("%b", &ZoneNum);
   CurrentFile.ReadFileString(ZoneName); //delim preset to ~
   CurrentFile.Numeric("%b %b", &LifeSpan, &ResetMode);
   while (CurrentFile.good()) {
	CurrentFile >> CommandType;
	switch (tolower(CommandType)) {
	   case 'c':
		RoomCash.Add((pRoomCash = new sRoomCash()));
		CurrentFile.Numeric("%l %l %d", &pRoomCash->m_lBaseAmt,
			&pRoomCash->m_lRoomVnum, &pRoomCash->m_bLoadEveryTime);
		break;
	   case 'm':
		MobCommands.Add((MobCom = new sMobCommands()));
		CurrentFile.Numeric("%d %b %l", &MobCom->MobVNum,
			&MobCom->Max, &MobCom->RoomVNum);
		break;
	   case 'o':
		RoomObjs.Add((ObjToPutInRoom = new sRoomObj()));
		CurrentFile.Numeric("%d %b %l", &ObjToPutInRoom->ObjVNum,
			&ObjToPutInRoom->Max, &ObjToPutInRoom->RoomVNum);
		break;
	   case 'g':
		MobCom = MobCommands.GetLast();
		MobCom->MobObjs.Add((ObjForMob = new sMobCommands::sMobObj()));
		CurrentFile.Numeric("%d %d", &ObjForMob->ObjVnum,
			&ObjForMob->EqPos);
		break;
	   case 'p':
		ErrorLog << "Not written yet! put obj in obj" << endl;
		break;
	   case 'd':
		Doors.Add((DoorFlags = new sDoorFlag()));
		CurrentFile.Numeric("%l %d %s", &DoorFlags->RoomVnum,
			&DoorFlags->Dir, flag);
		DoorFlags->State = CurrentFile.AsciiConvert(flag);
		break;
	   case 'r':
		ObjsToRemove.Add((RemovalObj = new sObjRemove()));
		CurrentFile.Numeric("%l %d", &RemovalObj->RoomVNum, &RemovalObj->ObjVNum);
		break;
	   case ';':
		break;
	   default:
		ErrorLog << "We are reading something but not a command " << FileName << endl;
		CurrentFile.getline(Throwaway, MAX_FILE_STRING_SIZE);
		break;
	}
   }
   CurrentFile.close();
}

CZone::~CZone() {

}

//////////////////////////
//	Overloaded constructor for readin
//////////////////////////

CZone::CZone(CZone * pZone) {
   MobCommands.SetDeleteData();
   ObjsToRemove.SetDeleteData();
   RoomObjs.SetDeleteData();
   Doors.SetDeleteData();
   RoomCash.SetDeleteData();

   LifeSpan = pZone->LifeSpan;
   ResetMode = pZone->ResetMode;
   Timer = pZone->LifeSpan;
   ZoneName = pZone->ZoneName;
   ZoneNum = pZone->ZoneNum;
   POSITION pos = pZone->Doors.GetStartPosition();
   while (pos) {
	Doors.Add(new sDoorFlag(pZone->Doors.GetNext(pos)));
   }
   pos = pZone->MobCommands.GetStartPosition();
   while (pos) {
	MobCommands.Add(new sMobCommands(pZone->MobCommands.GetNext(pos)));
   }
   pos = pZone->ObjsToRemove.GetStartPosition();
   while (pos) {
	ObjsToRemove.Add(new sObjRemove(pZone->ObjsToRemove.GetNext(pos)));
   }
   pos = pZone->RoomObjs.GetStartPosition();
   while (pos) {
	RoomObjs.Add(new sRoomObj(pZone->RoomObjs.GetNext(pos)));
   }
   pos = pZone->RoomCash.GetStartPosition();
   while (pos) {
	RoomCash.Add(new sRoomCash(pZone->RoomCash.GetNext(pos)));
   }
}

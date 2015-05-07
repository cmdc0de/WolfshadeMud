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
*  Helper classes for CZone
*
*
*
*/

#ifndef _ZONEHELPERS_INC_
#define _ZONEHELPERS_INC_

//Zone command to remove an object from a room
struct sObjRemove
{
public:
	long RoomVNum;
	int ObjVNum;
	sObjRemove(){RoomVNum = -1;ObjVNum = -1;}
	sObjRemove(sObjRemove *pObj)
	{RoomVNum = pObj->RoomVNum;ObjVNum = pObj->ObjVNum;}
};

//storage for mob commands from zone files
struct sMobCommands
{
public:
	struct sMobObj
	{
	public:
		long ObjVnum;
		short EqPos;
		sMobObj() {ObjVnum = -1;EqPos = -1;}
		sMobObj(sMobObj *pObj)
		{ObjVnum = pObj->ObjVnum;EqPos = pObj->EqPos;}
	};
public:
	mob_vnum MobVNum;
	short int Max;
	long RoomVNum;
	CLinkList<sMobObj> MobObjs;
	sMobCommands()
	{MobObjs.SetDeleteData();MobVNum = -1;Max = -1;RoomVNum = -1;}
	sMobCommands(sMobCommands *pMob)
	{
		MobObjs.SetDeleteData();
		MobVNum = pMob->MobVNum;
		Max = pMob->Max;
		RoomVNum = pMob->RoomVNum;
		POSITION pos = pMob->MobObjs.GetStartPosition();
		while(pos)
		{
			MobObjs.Add(new sMobObj(pMob->MobObjs.GetNext(pos)));
		}
	}
};

//zone command storage for loading a object into a room
struct sRoomObj
{
public:
	int ObjVNum;
	short int Max;
	long RoomVNum;
	sRoomObj(){ObjVNum = -1;Max = -1;RoomVNum = -1;}
	sRoomObj(sRoomObj *pObj)
	{ObjVNum = pObj->ObjVNum;Max = pObj->Max;RoomVNum = pObj->RoomVNum;}
};

//holds the door states as stated in the zone file
struct sDoorFlag
{
public:
	unsigned int State;  //bit flag
	long RoomVnum; 
	short int Dir; //direction
	sDoorFlag(){State = 0;RoomVnum = -1;}
	sDoorFlag(sDoorFlag *pDoor)
	{State = pDoor->State;RoomVnum = pDoor->RoomVnum;}
};
#endif
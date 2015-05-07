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
*  Zone class
*
*/

#ifndef _ZONE_H_
#define _ZONE_H_

#include <iostream>
#include <sstream>

class CZone
{
	friend class CZoneManager;
protected:
	struct sRoomCash
	{
	public:
		long m_lRoomVnum, m_lBaseAmt;
		bool m_bLoadEveryTime;
	public:
		sRoomCash() {m_lRoomVnum = m_lBaseAmt = -1;m_bLoadEveryTime = false;}
		sRoomCash(sRoomCash *pRC) 
		{m_lRoomVnum = pRC->m_lRoomVnum;m_lBaseAmt = pRC->m_lBaseAmt;m_bLoadEveryTime = pRC->m_bLoadEveryTime;}
	};
protected:
	short ZoneNum, 
		ResetMode,
		LifeSpan;

	CString ZoneName;

	int Timer;

	#include "zone.helpers.inc"
	CLinkList<sMobCommands> MobCommands;
	CLinkList<sObjRemove> ObjsToRemove;
	CLinkList<sRoomObj> RoomObjs;
	CLinkList<sDoorFlag> Doors;
	CLinkList<sRoomCash> RoomCash;

protected:
	CZone(CZone *pZone);
	void ResetTimer() {Timer = LifeSpan;}
	void ReadZone(CString FileName);
	bool ResetModeIs(short nMode) {return ResetMode==nMode;}
	short GetZoneNum() {return ZoneNum;}
	
public:
	CZone();
	~CZone();
};

#endif

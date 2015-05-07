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
*		Manages the Zone table and Zone Resets
*
*
*/

#ifndef _ZONE_MANAGEMENT_H_
#define _ZONE_MANAGEMENT_H_
#include "zone.h"
#include "object.manager.h"
#include "mob.manager.h"

#define TOTAL_ZONE_RESET_TYPES 3

class CZoneManager
{
private:
	static const short ZONE_NORMAL_RESET;
	static const short ZONE_NEVER_RESET;
	static const short ZONE_ON_EMPTY_RESET;
	static const char *strResetTypes[];
private:
	void ResetRoomCash(CZone *pZone,bool bResetAll);
	void ResetRoomObjects(CZone *pZone,CObjectManager &ObjManager);
	void RemoveObjectsFromRoom(CZone *pZone,CObjectManager &ObjManager);
	void ResetMobs(CZone *pZone,CMobManager &MobManager,CObjectManager &ObjectManager,bool bResetAll);
	void ResetDoors(CZone *pZone);
	void BootZones();
	CMap<int,CZone *> m_ZoneMap;
	CString ZONEINDEX;
public:
	void ResetZones(bool bResetAll, CObjectManager &ObjManager,CMobManager &MobManager);
	CString GetZoneInformation();
	~CZoneManager();
	CZoneManager(CString strIndex);
public: //static 
	static const CString DEFAULT_ZONEINDEX;
public: 
	inline const char *GetResetName(short nType);
};

inline const char* CZoneManager::GetResetName(short nType)
{
#ifdef _DEBUG
	if(nType<0 || nType>=TOTAL_ZONE_RESET_TYPES)
	{
		return "Type out of range";
	}
#endif
	return strResetTypes[nType];
}
#endif

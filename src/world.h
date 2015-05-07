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
*  World Boot
*	The class boots the rooms and manages the management classes
*	for zones, objects, and mobs
*/
#ifndef _WORLD_H_
#define _WORLD_H_

#include "zone.manager.h"
#include "object.manager.h"
#include "mob.manager.h"
#include "hash.h"
#include "room.h"

#define VIRTUALWIDTH 100
#define VIRTUALHEIGHT 100
#define TOTAL_VIRTUAL_ROOMS (VIRTUALWIDTH*VIRTUALHEIGHT)
//all virtual rooms will have a vnum between
//MIN_VIRTUAL_VNUM to MIN_VIRTUAL_VUN + TOTAL_VIRTUAL_ROOMS
//And therefore all real rooms have to have Vnum's of less than
//MIN_VIRTUAL_VNUM!!!!!!!!!!!!!!!!!!!!!!
#define MIN_VIRTUAL_VNUM 1000000
#define MAX_VIRTUAL_VNUM (MIN_VIRTUAL_MIN+TOTAL_VIRTUAL_ROOMS)
#define VIRTURALINDEX "virtualrooms/prototypes"
#define VIRTUALWORLD "virtualrooms/vworld"
#define REALPREFIX "realrooms/"
#define SHOPINDEX "shops/shopindex"
#define SHOPPREFIX "shops/"

class CRoom;

class CWorld
{
private:
	static void BuildMap();
	void BootShops(const CObjectManager *pObjManager,const CMobManager *pMobManager);
	void ConnectRooms();
	void ReadRealRooms();
	void SetVirtualDoors(CRoom *pRoom, int DoorCode);
	void ReadInVirtualRooms();
	void BootWorld();
	static CString m_strMap;
	static CMap<long,CRoom *> m_WorldMap;
	CString REALINDEX;
public:
	static short ReverseDirection(short nDir);
	static bool IsZoneEmpty(int nZone);
	static const CString &GetMap() {return m_strMap;}
	static const CRoom *m_Map[VIRTUALWIDTH][VIRTUALHEIGHT];
	static CRoom * FindRoom(long nRoomNum);
	static const CString DEFAULT_REALINDEX;
	//Static hash table of rooms in the world
	~CWorld();
	CWorld(const CObjectManager *pObjManager,const CMobManager *pMobManager,CString strIndex);
};
#endif



///////////////////////////////////////////////////////////////
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
*  CMobManager
*
*/

#ifndef _MOB_MANAGER_H_
#define _MOB_MANAGER_H_

#include "mob.prototype.h"
#include "error.h"
#include "log.h"

class CMobManager
{
private:
	void BootMobs();
	CMap<mob_vnum,CMobPrototype *> m_MobMap;
	CString MOBINDEX;
private:
	static CMap<CString,bool> m_MobNames;
private:
	void MobNames();
public:
	static bool IsMobName(CString strName);
	static const CString DEFAULT_MOBINDEX;
public:
	CCharacter * CreateUndead(const sUndeadInfo *pUndeadInfo, CRoom *pRoom);
	CCharacter * CreateWall(const sWallInfo *pWallInfo, CRoom *pRoom);
	void VStat(CString &ToChar,mob_vnum lVnum);
	CCharacter * Create(mob_vnum MobVnum,CRoom *pRoom) const;
	void GetVnums(CString &strToChar,CString strAlias);
	CCharacter * Create(mob_vnum MobVnum,CRoom *pRoom, int nMax);
	CCharacter * Create(mob_vnum MobVnum,CRoom *pRoom,int nHp,int nLevel) const;
	virtual ~CMobManager();
	CMobManager(CString strIndex);
	inline const CMobPrototype *operator[](mob_vnum lMob);
	inline void ReduceNumberInGame(mob_vnum Vnum);
};

inline void CMobManager::ReduceNumberInGame(mob_vnum Vnum)
{
	CMobPrototype *pMob;
	if(m_MobMap.Lookup(Vnum,pMob))
	{
		pMob->NumInGame--;
	}
	//there are mobs that don't have prototypes...eg mirror images
}

inline const CMobPrototype *CMobManager::operator[](mob_vnum lMob)
{
	CMobPrototype *pMobProto;
	if(m_MobMap.Lookup(lMob,pMobProto))
	{
		return pMobProto;
	}
	return NULL;
}
#endif
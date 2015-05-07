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
*
*  CPortal header
*
* - 
*/

#ifndef _PORTAL_H_
#define _PORTAL_H_

#include "object.h"

//m_nVal0 = to room vnum
//m_nVal1 = time to live
class CPortal : public CObject 
{
	//protected constructor so only friend class can create =)
	friend class CObjectManager;
protected:
	CRoom *m_pToRoom;
protected:
	CPortal(CObjectPrototype *obj, CRoom *pInRoom)
		: CObject(obj,NULL,pInRoom)
	{
		m_pToRoom = NULL;
		m_pToRoom = CWorld::FindRoom(m_nVal0);
		if(GetRoom()==NULL)
		{
			ErrorLog << "Portal ERROR: vnum: " << GetVnum() << " has a null to room POINTER!" << endl;
		}
		m_nAffBit |= ITEM_NOT_TAKEABLE;
		this->m_nObjType = ITEM_TYPE_PORTAL;
	}
public:
	CRoom *GetToRoom() {return m_pToRoom;}
public:
	virtual void FadeAffects(long lTimeToRemove);
	virtual CRoom *Add(CCharacter *pCh);
	virtual void Save(std::ofstream &s, int nWornOn, short nPos) {return;} //portals can't be saved
	virtual ~CPortal();
public:
	static short WORMHOLE;
	static short MOONWELL;
	static short GUKS_MAGIC_PORTAL;
};
#endif
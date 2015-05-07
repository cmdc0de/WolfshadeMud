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

#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

class CTransportRoom; //forward def

class CTransport : public CObject
{
	friend class CObjectManager;
private:
	static int TRANSPORT_MOVEMENT_RATE;
private:
	int m_nWaitTime;
protected:
	CLinkList<CTransportRoom> m_RoomsInTransport; //these rooms are not in the CWorld::Map
protected:
	virtual void FadeAffects(long lTimeToRemove);
	virtual void SendToTransport(CString str);
	virtual CRoom *Add(CCharacter *pCh);
	CTransport(CObjectPrototype *pObj,CRoom *pRoom);
public:
	virtual ~CTransport();
public:
	void DoMove(const CCharacter *pCh, CString strDir);
};
#endif
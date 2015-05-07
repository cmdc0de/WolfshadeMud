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

#include "stdinclude.h"
#pragma hdrstop

#include "transport.h"
#include "transportroom.h"

/*
v0 = speed factor (how many moves allowed in one mud hour 1-6)
	v1 = damage it can with stand
	v2 = room vnum of starting room
	v3 = crew vnum
	v4 = how many crew members
*/

#define TRANS_SPEED_FACTOR m_nVal0
#define TRANS_HIT_POINTS m_nVal1
#define TRANS_FIRST_ROOM m_nVal2
#define TRANS_CREW_VNUM m_nVal3
#define TRANS_CREW_NUM m_nVal4

//statics
int CTransport::TRANSPORT_MOVEMENT_RATE = CMudTime::PULSES_PER_MUD_HOUR;

//Non statics

//constructor
//	here we set up default vaules for variables and build
//the rooms in the transport.  The rooms in a transport
//are not accectable by the CWorld::m_WorldMap
//	the room vnums stored in the files are used as prototypes
//	to create transport rooms
CTransport::CTransport(CObjectPrototype *pObj,CRoom *pRoom)
	:CObject(pObj,NULL,pRoom)
{
	m_nWaitTime = 0;
	//because the world::m_WorldMap can't delete this memory the transport will
	m_RoomsInTransport.SetDeleteData();
	CRoom *pRoomType = CWorld::FindRoom(TRANS_FIRST_ROOM);
	if(pRoomType)
	{
		CTransportRoom *pTransRoom = new CTransportRoom(this,pRoomType);
		m_RoomsInTransport.Add(pTransRoom);
		register int i;
		for(i=0;i<NUM_OF_DOORS;i++)
		{
			delete pTransRoom->m_Doors[i];
			pTransRoom->m_Doors[i] = NULL;
		}
		//see if it is a two room ship
		//can you say HACK?
		if((pRoomType = CWorld::FindRoom(TRANS_FIRST_ROOM+1))!=NULL)
		{
			CTransportRoom *pTransRoom2 = new CTransportRoom(this,pRoomType);
			m_RoomsInTransport.Add(pTransRoom2);
			for(i=0;i<NUM_OF_DOORS;i++)
			{
				delete pTransRoom2->m_Doors[i];
				pTransRoom2->m_Doors[i] = NULL;
			}
			pTransRoom->m_Doors[NORTH] = new CTransportRoom::sDoor();
			pTransRoom->m_Doors[NORTH]->m_pToRoom = pTransRoom2;
			pTransRoom->m_Doors[NORTH]->ToRoom = pTransRoom2->GetVnum();
			pTransRoom->m_Doors[NORTH]->DoorType = CTransportRoom::sDoor::DOOR_CLOSED;
			pTransRoom2->m_Doors[SOUTH] = new CTransportRoom::sDoor();
			pTransRoom2->m_Doors[SOUTH]->m_pToRoom = pTransRoom;
			pTransRoom2->m_Doors[SOUTH]->ToRoom = pTransRoom->GetVnum();
			pTransRoom2->m_Doors[SOUTH]->DoorType = CTransportRoom::sDoor::DOOR_CLOSED;
		}
		else
		{
			ErrorLog << GetObjName() << " has no second room for this transport." << endl;
		}
	}
	else
	{
		ErrorLog << "First Room in transport could not be found...trans port will be deleted" << endl;
		delete pRoomType;
	}
}

CTransport::~CTransport()
{

}

////////////////////////
//	Moves the transport
///////////////////////
void CTransport::DoMove(const CCharacter *pCh, CString strDir)
{
	short nDir = GetRoom()->StringToDirection(strDir);
	if(nDir!=NOWHERE)
	{
		if(m_nWaitTime<=0)
		{
			CRoom *pToRoom = GetRoom()->GetRoomToThe(nDir);
			if(pToRoom!=NULL 
				&& GetRoom()->IsWaterRoom() 
				|| (!GetRoom()->IsWaterRoom() 
				&& pToRoom->IsWaterRoom()))
			{
				m_nWaitTime = TRANSPORT_MOVEMENT_RATE/TRANS_SPEED_FACTOR;
				GetRoom()->Remove(this);
				pToRoom->Add(this);
				CString str;
				if(!GetRoom()->IsWaterRoom())
				{
					str.Format("The ship sails %s, and docks.\r\n",
						(char *)strDir);
				}
				else
				{
					str.Format("The ship sails %s.\r\n",
						(char *)strDir);
				}
				POSITION pos = m_RoomsInTransport.GetStartPosition();
				while(pos)
				{
					m_RoomsInTransport.GetNext(pos)->SendMap(pCh,str);
				}
			}
			else
			{
				pCh->SendToChar("You can't sail in that direction!\r\n");
			}
		}
		else
		{
			pCh->SendToChar("Your ship doesn't move that fast!\r\n");
		}
	}
	else
	{
		pCh->SendToChar("You can't go that way!\r\n");
	}
}

///////////////////////////
//	Sends a string to a transport
void CTransport::SendToTransport(CString str)
{
	POSITION pos = m_RoomsInTransport.GetStartPosition();
	while(pos)
	{
		m_RoomsInTransport.GetNext(pos)->SendToRoom(str);
	}
}

CRoom *CTransport::Add(CCharacter *pCh)
{
	CRoom *pRoom = m_RoomsInTransport.GetFirst();
	pCh->SendToChar("You come aboard.\r\n");
	pRoom->SendToRoom("%s come aboard.\r\n",pCh);
	return pRoom;
}

////////////////////////////////
//	Reduce wait time for ship to move
//	then call CObject::FadeAffects
void CTransport::FadeAffects(long lTimeToRemove)
{
	if(m_nWaitTime>0)
	{
		m_nWaitTime-=(int)lTimeToRemove;
		if(m_nWaitTime<0)
		{
			m_nWaitTime = 0;
		}
	}
	CObject::FadeAffects(lTimeToRemove);
}

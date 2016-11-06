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
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "portal.h"

short CPortal::WORMHOLE = -1;
short CPortal::MOONWELL = -2;
short CPortal::GUKS_MAGIC_PORTAL = -3;

//m_nVal0 = to room vnum
//m_nVal1 = time to live

CPortal::~CPortal() {
   if (!CGame::IsShuttingDown()) {
	if (GetVnum() == WORMHOLE) {
	   this->GetRoom()->SendToRoom("&LThe wormhole disappears.&n\r\n");
	} else if (GetVnum() == MOONWELL) {
	   this->GetRoom()->SendToRoom("&GThe ground closes up and the &WMoonwell&G disappears.&n\r\n");
	} else if (GetVnum() == GUKS_MAGIC_PORTAL) {
	   this->GetRoom()->SendToRoom("&WThe magic portal dissapates.&n\r\n");
	} else {
	   ErrorLog << "Illegal vnum for portal." << endl;
	}
   }
}

/////////////////////////////////////
///	

void CPortal::FadeAffects(long lTimeToRemove) {
   if (m_nVal1>-1) //-1 is perm
   {
	m_nVal1 -= lTimeToRemove;
	if (m_nVal1 <= 0) {
	   m_nState = STATE_DELETE;
	}
   }
   CObject::FadeAffects(lTimeToRemove);
}

CRoom *CPortal::Add(CCharacter *pCh) {
   CRoom *pRoom = CWorld::FindRoom(m_nVal0);
   if (pRoom == NULL) {
	pCh->SendToChar("The portal does not see stable enough\r\n");
	return NULL;
   } else {
	CString str;
	str.Format("You enter the %s.\r\n", (char *) GetObjName());
	pCh->SendToChar(str);
	pCh->GetRoom()->SendToRoom("%s enters the %s.\r\n", pCh, (char *) GetObjName());
	pRoom->SendToRoom("%s emerges from %s.\r\n", pCh, (char *) GetObjName());
	return pRoom;
   }
}
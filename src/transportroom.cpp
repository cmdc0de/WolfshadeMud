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
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "transport.h"
#include "transportroom.h"

CTransportRoom::CTransportRoom(CTransport *pTrans, CRoom *pRoom)
: CRoom(pRoom) {
   assert(pTrans);
   m_pTrans = pTrans;
   m_lVnum = TRANSPORT_ROOM_VNUM;
}

CTransportRoom::~CTransportRoom() {

}

////////////////////////
//	Over Ridden doRoomlook
//	so we see land around transport

CString CTransportRoom::DoRoomLook(CCharacter *pLooker) {
   CString strToChar;
   //can they see in the room the transport is in?
   //if they can see red dots we'll give them the area map
   if (pLooker->CanSeeInRoom(GetTransport()->GetRoom()) >= CCharacter::SEES_RED_DOTS
	   && !pLooker->IsNPC()
	   && pLooker->Prefers(CCharacterAttributes::PREFERS_MAP_OUTSIDE)
	   && !GetTransport()->GetRoom()->IsRealRoom()) {
	strToChar += GetTransport()->GetRoom()->GetMap(pLooker);
   }
   const short nVision = pLooker->CanSeeInRoom(pLooker->GetRoom());
   if (nVision == CCharacter::SEES_ALL) {
	if (pLooker->Prefers(CCharacterAttributes::PREFERS_BRIEF)) {
	   strToChar.Format("%s\r\n\r\n%s\r\n%s", strToChar.cptr(),
		   m_strRoomName.cptr(), GetExitString(pLooker).cptr());
	} else {
	   strToChar.Format("%s\r\n\r\n%s\r\n%s\r\n%s", strToChar.cptr(),
		   m_strRoomName.cptr(),
		   m_strDescription.cptr(),
		   GetExitString(pLooker).cptr());
	}
	m_CashInRoom.RoomString(strToChar);
	LookObjects(strToChar, pLooker);
	LookCharacters(strToChar, pLooker);
   }//end sees_all
   else if (nVision == CCharacter::SEES_RED_DOTS) {
	strToChar.Format("%s\r\n%s\r\n",
		m_strRoomName.cptr(),
		GetExitString(pLooker).cptr());
	LookCharacters(strToChar, pLooker);
   } else //see nothing
   {
	if (pLooker->IsAffectedBy(CCharacter::AFFECT_BLINDNESS)) {
	   strToChar = "You can't see a damn thing you're blind!\r\n";
	} else {
	   strToChar = "You can't see a damn thing!\r\n";
	}
   }
   return strToChar;
}

//////////////////////////
//	Sends Maps to players that want it
//	while moving

void CTransportRoom::SendMap(const CCharacter *pCommander, CString &str) {
   POSITION pos = m_CharactersInRoom.GetStartPosition();
   CCharacter *pLooker;
   short nVision;
   while (pos) {
	pLooker = m_CharactersInRoom.GetNext(pos);
	nVision = pLooker->CanSeeInRoom(m_pTrans->GetRoom());
	if (!pLooker->IsNPC()
		&& pLooker->Prefers(CCharacterAttributes::PREFERS_MAP_OUTSIDE)
		&& nVision == CCharacter::SEES_ALL) {
	   if (!GetTransport()->GetRoom()->IsRealRoom()) {
		str.Format("%s\r\n%s\r\n",
			str.cptr(), GetTransport()->GetRoom()->GetMap(pLooker).cptr());
	   }
	}
	pLooker->SendToChar(str);
	if (pLooker != pCommander) {
	   pLooker->SendPrompt();
	}
   }
}

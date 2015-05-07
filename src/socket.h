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
//////////////////////////////////////////////////////////////
/*
*  Header File for CSocket
*	Socket handling
*		Handals all socket manipulates moves commands from sockets
*		to descriptor command buffer.  
*		Manages charIntermediate
*/


#ifndef _SOCKET_H
#define _SOCKET_H

#ifndef _WOLFSHADE_WINDOWS_
#include <netinet/in.h> //INADDR.ANY header
#endif

#include "constants.h"
#include "linklist.h"
#include "char.intermediate.h"

class CSocket
{
private:
	//variables
	struct linger ld;	
	bool bNoNameServer;
protected:
	static UINT MAX_PLAYERS;
	static UINT RESERVED_CONNECTIONS;
public:
	static const SOCKET DEFAULT_PORT;

#ifdef _WOLFSHADE_WINDOWS_
	WSADATA m_WsaData;
#endif
	SOCKET ListenSocket,m_nPort;
	fd_set input_set, output_set, exception_set;
	struct sockaddr_in sa;
	short m_MaxPlayers;
	CLinkList<sPlayerDescriptor> Descriptors;
	CLinkList<CCharIntermediate> NotCharsYet;
	CLinkList<CCharIntermediate> CharQ;

	//functions
	int NonBlock(SOCKET s);
	void BootFileStrings();
	void NewDescriptor();
	void SignalTrap();
public:
	void AddCharFromDeath(CCharacter *pChar);
	void KillSocket(sPlayerDescriptor *pDesc);
	CLinkList<CCharIntermediate> &GetCharQ() {return CharQ;}
	void DoNotCharsYet();
	void GetCommandsFromSockets();
	void KickOutExceptionSet();
	~CSocket();
	bool NewConnections();
	CSocket(CString strOpts);
};
#endif

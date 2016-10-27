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
 *  CSocket implementation file
 *
 *
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#ifdef _WOLFSHADE_WINDOWS_
typedef int socklen_t;
#else
#include <sys/socket.h>  //socket functions
#include <sys/resource.h> //timeval stuct declaration
#include <errno.h> //for the type error_t
#include <fcntl.h>  //function header to nonblock a socket
#include <unistd.h> //int close()
#define closesocket close
#include <netdb.h> //hostent declaration
#include <signal.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#endif

#include "socket.h"
#include <string.h>
#include <signal.h>
#include "log.h"

extern CGlobalVariableManagement GVM;

UINT CSocket::MAX_PLAYERS = 300;
UINT CSocket::RESERVED_CONNECTIONS = 4;
const SOCKET CSocket::DEFAULT_PORT = 1234;

CSocket::CSocket(CString strOpts) {
   bNoNameServer = (strOpts.Find("-n") == -1);
   m_nPort = DEFAULT_PORT;
   int nPos = strOpts.Find("-p");
   if (nPos != -1) {
	CString strPort = strOpts.Right(nPos + 1).GetWordAfter(0);
	m_nPort = strPort.MakeInt();
	if (m_nPort == -1) {
	   ErrorLog << "Your port number is incorrect... using default" << endl;
	   m_nPort = DEFAULT_PORT;
	}
   }
   Descriptors.SetDeleteData(); //delete decriptors
   NotCharsYet.SetDeleteData(); //delete ccharintermediate that are not characters yet
   CharQ.SetDeleteData(); //remove Ccharintermediate when they leave Q
   CString Error;
   m_MaxPlayers = MAX_PLAYERS - RESERVED_CONNECTIONS;

#ifdef _WOLFSHADE_WINDOWS_
   //start winsock
   if (WSAStartup(MAKEWORD(2, 0), &m_WsaData) != 0) {
	if (WSAStartup(MAKEWORD(1, 1), &m_WsaData) != 0) {
	   CError Err(CRITICAL_ERROR, "Winsock not started correctly!\n");
	   throw &Err;
	} else {
	   MudLog << "Winsock started using 1.1" << endl;
	   //this doesn't work in winsock 2 or greater
	   //anyone know what the call is?
	   m_MaxPlayers = m_WsaData.iMaxSockets - RESERVED_CONNECTIONS;
	}
   } else {
	MudLog << "Winsock started using 2.0" << endl;
   }
#elif HAVE_GETRLIMIT
   rlimit lim;
   if (getrlimit(RLIMIT_NOFILE, &lim) < 0) {
	ErrorLog << "Can't get rlimit for number of files using MAX_PLAYERS: " << MAX_PLAYERS << endl;
   } else {
	m_MaxPlayers = lim.rlim_cur - RESERVED_CONNECTIONS;
	MudLog << "Current system limit for players is: " << lim.rlim_max << endl;
   }
#endif
   MudLog << "Max Players set to " << m_MaxPlayers << endl;
   //make listening socket
   if ((ListenSocket = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
#ifdef _WOLFSHADE_WINDOWS_
	Error.Format("%s %d", "Error opening network socket ", WSAGetLastError());
#else
	Error.Format("%s %d", "Error opening network socket ", errno);
#endif
	CError Err(CRITICAL_ERROR, Error.cptr());
	throw &Err;
   }

#if defined(SO_SNDBUF)
   int opt = MAX_COMMAND_BUFFER;
   if (setsockopt(ListenSocket, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)) < 0) {
	CError Err(CRITICAL_ERROR, "Error setting SO_SNDBUF");
	throw &Err;
   }
#endif

#if defined (SO_REUSEADDR)
   opt = 1;
   if (setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
	CError Err(CRITICAL_ERROR, "Error setting SO_REUSEADDR\n");
	throw &Err;
   }
#endif

#if defined (SO_LINGER)
   ld.l_onoff = 0;
   ld.l_linger = 0;
   if (setsockopt(ListenSocket, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld)) < 0) {
	ErrorLog << "Informational: SO_LINGER won't set" << endl;
   }
#endif

   sa.sin_family = AF_INET;
   sa.sin_port = htons(m_nPort);
   sa.sin_addr.s_addr = htonl(INADDR_ANY);

   if (bind(ListenSocket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
#ifdef _WOLFSHADE_WINDOWS_
	Error.Format("%s %d %s", "Error binding network socket: ",
		WSAGetLastError(), "closing socket");
#else
	Error.Format("%s %d %s", "Error binding network socket: ",
		errno, "closing socket");
#endif
	closesocket(ListenSocket);
	CError Err(CRITICAL_ERROR, Error.cptr());
	throw &Err;
   }
   MudLog << "Wolfshade mud is running on port: " << m_nPort << endl;
   if (NonBlock(ListenSocket) != 0) {
#ifdef _WOLFSHADE_WINDOWS_
	Error.Format("%s %d %s", "Error in blocking Network socket: ",
		WSAGetLastError(), "\nclosing socket!");
#else
	Error.Format("%s %d %s", "Error in blocking network socket: ",
		errno, "\r\nclosing socket!");
#endif
	closesocket(ListenSocket);
	CError Err(CRITICAL_ERROR, Error.cptr());
	throw &Err;
   }
   if (listen(ListenSocket, 5) != 0) {
#ifdef _WOLFSHADE_WINDOWS_
	Error.Format("%s %d %s", "Error in making network socket listening sock: ",
		WSAGetLastError(), "\nclosing socket!");
#else
	Error.Format("%s %d %s", "Error in making network socket listening socket: ",
		errno, "\r\nclosing socket!");
#endif
	closesocket(ListenSocket);
	CError Err(CRITICAL_ERROR, Error.cptr());
	throw &Err;
   }
   //trap all signals
   SignalTrap();
}

int CSocket::NonBlock(SOCKET s) {
   int nRetVal;
#ifdef _WOLFSHADE_WINDOWS_
   unsigned long val;
   val = 1;
   nRetVal = ioctlsocket(s, FIONBIO, &val);
#else
#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

   int flags;
   flags = fcntl(s, F_GETFL, 0);
   flags |= O_NONBLOCK;
   if ((nRetVal = fcntl(s, F_SETFL, flags)) < 0) {
	CString str;
	str.Format("Could not non block socket %d", (int) s);
	CError Err(CRITICAL_ERROR, str.cptr());
	throw &Err;
   }
#endif
   return nRetVal;
}

//checks for new connects

bool CSocket::NewConnections() {
   struct timeval NullTime;
   NullTime.tv_sec = 0;
   NullTime.tv_usec = 0;
   sPlayerDescriptor *currentDesc;
   POSITION pos = Descriptors.GetStartPosition();
   //zero input set
   FD_ZERO(&input_set);
   //put listensocket into input set
   FD_SET(ListenSocket, &input_set);
   if (pos == NULL) {
	//poll with no wait
	if (select(ListenSocket + 1, &input_set, (fd_set *) 0, (fd_set *) 0, NULL) < 0) {
	   ErrorLog << "select < 0\n" << endl;
	} else {
	   MudLog << "New connection waking up!\r\n";
	}
   }
   FD_ZERO(&input_set);
   FD_ZERO(&output_set);
   FD_ZERO(&exception_set);
   FD_SET(ListenSocket, &input_set);
   while (pos) {
	currentDesc = Descriptors.GetNext(pos);
	FD_SET(currentDesc->m_ConnectedSocket, &input_set);
	FD_SET(currentDesc->m_ConnectedSocket, &output_set);
	FD_SET(currentDesc->m_ConnectedSocket, &exception_set);
   }
   //Circle sleeps here before next pulse
   if (select(ListenSocket + 1, &input_set, &output_set,
	   &exception_set, &NullTime) < 0) {
	//ErrorLog << "Select poll";
	//NO NEW CONNECTIONS
	return false;
   }
   /* If there are new connections waiting, accept them. */
   if (FD_ISSET(ListenSocket, &input_set)) {
	NewDescriptor();
   }
   /* kick out the freaky folks in the exception set */
   KickOutExceptionSet();
   return true;
}

//handles a new connection
void CSocket::NewDescriptor() {
   SOCKET desc;
   struct sockaddr_in peer;
   struct hostent *from;
   static const CString buf("Welcome to Wolfshade MUD! Ascii Tile Screen? [y]: ");
   static const CString strDeny("Sorry too many connections right now.  Try back later.\r\n");

   socklen_t i = sizeof(peer);
   if ((desc = accept(ListenSocket, (struct sockaddr *) &peer, &i)) == INVALID_SOCKET) {
	ErrorLog << "error on accept!\n";
	return;
   }
   NonBlock(desc);
   if (Descriptors.GetCount() > m_MaxPlayers) {
	send(desc, strDeny.cptr(), strDeny.GetLength(), 0);
   } else {
	sPlayerDescriptor *newbieDesc = NULL;
	//add new socket to descriptor link list
	Descriptors.Add((newbieDesc = new sPlayerDescriptor(desc)));
	CCharIntermediate *newbieChar = NULL;
	//use new descriptor to make character intermediate
	//add this intermediate to link list
	NotCharsYet.Add((newbieChar = new CCharIntermediate(newbieDesc)));
	//send first screen
	newbieChar->SendToChar(buf);
	//set the socket number
	MudLog << "The desc is: " << desc << endl;
	//We'll want to get log in time here!
	if (bNoNameServer || !(from = gethostbyaddr((char *) &peer.sin_addr, sizeof(peer.sin_addr), AF_INET))) {
	   newbieDesc->Host = inet_ntoa(peer.sin_addr);
	} else {
	   newbieDesc->Host = from->h_name;
	}
	CString strGod;
	MudLog << newbieDesc->Host << " and there are " << Descriptors.GetCount() << " connected sockets" << endl(strGod);
	GVM.SendToGods(strGod, CCharacterAttributes::PREFERS_GOD_CONNECTION_LOG, NULL);
   }
}

CSocket::~CSocket() {
   //MUST CLOSE ALL SOCKETS OR CLIENTS WILL HANG!
   POSITION pos = Descriptors.GetStartPosition();
   sPlayerDescriptor *pDesc;
   while (pos) {
	pDesc = Descriptors.GetNext(pos);
	closesocket(pDesc->m_ConnectedSocket);
   }
   closesocket(ListenSocket);
#ifdef _WOLFSHADE_WINDOWS_
   WSACleanup();
#endif
}

void CSocket::KickOutExceptionSet() {
   sPlayerDescriptor *current = NULL;
   POSITION pos = Descriptors.GetStartPosition();
   while (pos) {
	current = Descriptors.GetNext(pos);
	if (FD_ISSET(current->m_ConnectedSocket, &exception_set)) {
	   ErrorLog << "Exception found: " << current->m_ConnectedSocket << endl;
	   FD_CLR(current->m_ConnectedSocket, &input_set);
	   FD_CLR(current->m_ConnectedSocket, &output_set);
	   closesocket(current->m_ConnectedSocket);
	}
   }
}

void CSocket::GetCommandsFromSockets() {
   CString NewCommands;
   sPlayerDescriptor *currentDesc;
   currentDesc = NULL;
   int nBytesRead;
   POSITION pos = Descriptors.GetStartPosition();
   //char Com[MAX_COMMAND_BUFFER] = {'\0'};
   CString Com(MAX_COMMAND_BUFFER);
   while (pos) {
	currentDesc = Descriptors.GetNext(pos);
	//get socket info!
	if ((nBytesRead = recv(currentDesc->m_ConnectedSocket, Com.ptr(), MAX_COMMAND_BUFFER - 1, 0)) < 0) {
#ifdef _WOLFSHADE_WINDOWS_
	   if (WSAGetLastError() != WSAEWOULDBLOCK)
#else
#ifdef EWOULDBLOCK
	   if (errno == EWOULDBLOCK)
		errno = EAGAIN;
#endif /* EWOULDBLOCK */
	   if (errno != EAGAIN)
#endif //woflshade_windows
	   {
		ErrorLog << "Socket " << currentDesc->m_ConnectedSocket << " bad read" << endl;
		currentDesc->ChangeState(STATE_LINK_DEAD);
	   }
	} else if (nBytesRead == 0) {
	   ErrorLog << "End of file read from socket...connect closed by peer. Socket #: " << currentDesc->m_ConnectedSocket << endl;
	   currentDesc->ChangeState(STATE_LINK_DEAD);
	} else {
	   NewCommands = Com.Left(nBytesRead);
	   NewCommands.RemoveFunkyChars(); //not correct!
	   //can't do this b/c clients that send 
	   //each keystoke will lose all spaces!
	   //NewCommands.SkipSpaces();
	   if ((NewCommands.GetLength() + currentDesc->CommandBuf.GetLength()) >
		   MAX_COMMAND_BUFFER - 1) {
		ErrorLog << "Socket " << currentDesc->m_ConnectedSocket << " had too much data!" << endl;
		currentDesc->CommandBuf += NewCommands.Left(MAX_COMMAND_BUFFER - currentDesc->CommandBuf.GetLength());
	   } else {
		currentDesc->CommandBuf += NewCommands;
	   }
	}
   }
}

void CSocket::DoNotCharsYet() {
   CCharIntermediate *current = NULL;
   POSITION pos = NotCharsYet.GetStartPosition();

   while (pos) {
	current = NotCharsYet.GetNext(pos);

	if (current->IsPlaying()) {
	   //have to new memory because the charinterdiate
	   //will be deleted when removed from NotCharsYet LL
	   CharQ.Add(new CCharIntermediate(current));
	   current->SendToChar("Enjoy...\r\n\r\n");
	   NotCharsYet.Remove(current, pos);
	} else if (current->IsLinkDead()) {
	   KillSocket(current->m_pDesc);
	   NotCharsYet.Remove(current, pos);
	} else if (current->HasCommand()) {
	   current->ExecuteState();
	   if (current->ShouldKillIntermediate()) {
		NotCharsYet.Remove(current, pos);
	   }
	}
   }
}

void MySignal(int x) {
   MudLog << "Signal caught: mud shutting down." << endl;
   CGame::ShutDownWolfshade();
}

void CSocket::SignalTrap() {
   /* just to be on the safe side: */
   //void (*signal(SIGHUP, void (__cdecl *func) ( int sig [, int subcode ] )) ) ( int sig );
   signal(SIGABRT, MySignal);
   signal(SIGTERM, MySignal);
   signal(SIGINT, MySignal);
   signal(SIGSEGV, SIG_IGN);
   signal(SIGILL, SIG_IGN);
}

////////////////////////////
//	KillSocket
//	reduce sockets connected count
//	close socket
//	remove removes the sPlayerDescriptor
//	Descriptor is set to DeleteAllData()
//	written by: Demetrius Comes
/////////////////////////////////

void CSocket::KillSocket(sPlayerDescriptor *pDesc) {
   MudLog << "Losing Socket " << pDesc->m_ConnectedSocket << " is leaving." << endl;
   closesocket(pDesc->m_ConnectedSocket);
   Descriptors.Remove(pDesc);
}

///////////////////////////////////////
//	AddCharFromDeath
//	Someone just died (sniff sniff) or rented
//	so we add them back to the NotCharYet LL
//	and set state to Menu
//	written by: Demetrius Comes
//	5/22/98
///////////////////////////////////////

void CSocket::AddCharFromDeath(CCharacter * pCh) {
   CCharIntermediate *pChar = new CCharIntermediate(pCh);
   pChar->m_pDesc->ChangeState(STATE_MENU);
   pChar->SendMenu();
   NotCharsYet.Add(pChar);
}

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
* Game Class - The Main Class
	This class manages the games.  
	The heartbeat function is the game loop.
*
*/

#ifndef _GAME_H_
#define _GAME_H_

#include "world.h"
#include "socket.h"
#include "interpetor.h"

class CGame
{
	friend class CInterp;
	friend void CCharacter::DoOtherState();
	friend void MySignal(int x);
	//fcns
private:
	void DoCharacterCommands(const int Pulse);
	void GetCharactersFromQ(CLinkList<CCharIntermediate> &NewChars);
	//member variables
private:
	CMobManager *m_pMobManager;
	CZoneManager *m_pZoneManager;
	CObjectManager *m_pObjectManager;
	CInterp *m_pInterpetor;
	CWorld *m_pWorld;
	CSocket *m_pConnections;
	//static private fcn
private:
	void DoNiceShutdown();
	static bool m_bShutDown;
	static CList<CCharacter *> m_FightLL;
	//static protected
protected:
	static void ShutDownWolfshade() {m_bShutDown = true;}
	//static member variables
public:
	static bool IsShuttingDown(){return m_bShutDown;}
	static void AddFighter(CCharacter *pFighter);
public:
	void Heartbeat(CMudTime &MudTime);
	CGame(CString strOpts);
	~CGame();
protected:
	void StopAllFromFighting(CCharacter *pCh);
	void Autosave(CCharacter *pSaveChar,bool bSendMsg);
	void KillChar(CCharacter *pDeadChar);
	void DoViolence();
};
#endif

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
*  CDescriptor 
*  Holds connection information!
*
*/

#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_

#include <sys/types.h>

#ifndef _WOLFSHADE_WINDOWS_
#include <sys/socket.h>
typedef int SOCKET;
#endif

#include "constants.h"
#include "wolfshade.string.h"

#define STATE_TITLE_QUESTION 0
#define STATE_GET_NAME 1
#define STATE_CONFIRM_NEW_PLAYER_NAME 2
#define STATE_GET_PASSWORD 3
#define STATE_CONFIRM_NEW_PASSWORD 4
#define STATE_PICK_RACE 5
#define STATE_PICK_CLASS 6
#define STATE_PICK_SEX 7
#define STATE_ROLL_CHAR 8
#define STATE_BONUS 9
#define STATE_MOTD 10
#define STATE_MENU 11
#define STATE_GET_SAVED_PASSWORD 12
#define STATE_KEEP_CHAR 13
#define STATE_CONFIRM_DELETE 14
#define STATE_CHANGE_PASSWORD 15
#define STATE_CHANGE_PASSWORD_CONFIRM 16



#define STATE_PLAYING 20

#define STATE_ENTER_DESCRIPTION 25

#define STATE_PAGING 30
//none charintermediate handled states
#define STATE_CONFIRM_SHUTDOWN 50
#define STATE_KILL_INTERMEDIATE 51
#define STATE_FROZEN 52
#define STATE_RENT 90
#define STATE_LINK_DEAD 98

/////////////////////////////////
//Stores Commands
//
struct sDescriptor
{
protected:
	unsigned short int m_nConnectedState;
public:
	CString CommandBuf;
public:
	bool IsLinkDead() {return m_nConnectedState==STATE_LINK_DEAD;}
	bool ShouldKillIntermediate() {return m_nConnectedState==STATE_KILL_INTERMEDIATE;}
	bool IsPlaying() {return m_nConnectedState==STATE_PLAYING;}
	bool IsWritingDescription() {return m_nConnectedState==STATE_ENTER_DESCRIPTION;}
	bool ShouldRent() {return m_nConnectedState==STATE_RENT;}
	void ChangeState(unsigned short int nState) {m_nConnectedState=nState;}
	unsigned short int GetState() {return m_nConnectedState;}
	sDescriptor(unsigned short int nState) {m_nConnectedState = nState;}
};

//////////////////////////
//stores all info for players
struct sPlayerDescriptor : public sDescriptor
{
public:
	//what is your socket number
	SOCKET m_ConnectedSocket;	
	CString Host;
	sPlayerDescriptor(SOCKET desc, unsigned short int nState = STATE_TITLE_QUESTION) 
		: sDescriptor(nState) {m_ConnectedSocket = desc;}
};
#endif
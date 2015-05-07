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
*
*
*  CCharIntermediate
*
*  Class will be used as a go between or messaging class
*  between csocket to cgame.
*/

#ifndef _CHARINTERMEDIATE_H_
#define _CHARINTERMEDIATE_H_

#include "descriptor.h"
#include "money.h"
#include "character.attributes.h"

class CSocket;

//For a new character we will have this class Set up add variables 
//properly but advance level will be done in CCharacter class!

class CCharIntermediate : public CCharacterAttributes
{
private: //non static
	void InitStatics();
	void BootFileStrings();
	void SendToSocket(const CString &str) const;
private:
	static void MakeRaceList();
	static void ReadNotAllowedNames();
	static const char *m_strMenu;
	static CMap<CString,bool> NamesNotAllowed;
	static bool m_bInited;
	static const bool Matrix[TOTAL_RACES][TOTAL_CLASSES];
	static const char *m_strBonus;
	static CString m_strRaceList;
	static const int VOID_TIME;
	static CString TitleScreen;
	static CString AnsiTitleScreen;
	void ChangeState(unsigned short int nState) {m_pDesc->ChangeState(nState);}
protected:
	short m_nBonus;
	int m_nTimeSinceLastCmd; //used for cutting connection
public:
	~CCharIntermediate();
	bool IsPlaying(){return m_pDesc->IsPlaying();}
	bool IsLinkDead() {return m_pDesc->IsLinkDead();}
	bool ShouldKillIntermediate() {return m_pDesc->ShouldKillIntermediate();}
	void SendMenu() {SendToChar(m_strMenu);}
	short GetClass() {return m_pAttributes->GetClass();}
public:
	void StateConfirmPasswordChange();
	void StateChangePassword();
	void StateKeepChar();
	void StateBonus();
	CCharIntermediate(CCharacter *pCh);
	void StateGetSavedPassword();
	//All functions to move through states get a pointer to CSocket class
	//so that we can access it's private members we are a friend.
	void StateMenu();
	void StateMOTD();
	void StateRollChar();
	void StatePickSex();
	void StatePickRace();
	void StatePickClass();
	void StateConfirmPassword();
	void StateGetPassWord();
	void StateConfirmName();
	void StateGetName();
	void ExecuteState();
	void StateTitle();
	void ConfirmDeletion();
	bool HasCommand();
	
	//don't delete descriptor pointer!
	sPlayerDescriptor *m_pDesc;
	CCharIntermediate(sPlayerDescriptor *desc);
	CCharIntermediate(CCharIntermediate *ch);
	void SendToChar(const char *strSend);
	void SendToChar(CString strSend) const;
protected:
	bool IsLegalName();
	void SendClassList(short nRace);
};
#endif
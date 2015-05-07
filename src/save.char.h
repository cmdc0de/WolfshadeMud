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
// SaveChar.h: interface for the CSaveChar class.
//	this is the structure that represents the character in storage
//	make all contents must be structs or native types...just not
//	classes otherwise you risk over writing the v_ptr for the class!
//	get the feeling I had to figure that out on my own?
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAVECHAR_H__0E457141_F1BD_11D1_81AA_00600834E9F3__INCLUDED_)
#define AFX_SAVECHAR_H__0E457141_F1BD_11D1_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MAX_SPELLS_FOR_ANY_CLASS 200
#include "affect.h"

class CCharacterAttributes;

struct sSaveChar  
{
public:
	//holds just base information
	struct sSavedAttributes
	{
	public:
		time_t m_dtBirth;
		long m_lSecondsPlayed;
		short int m_pnStats[MAX_NUMBER_OF_STATS];
		short int m_nSize,
			m_nSex, 
			m_nChClass, 
			m_nRace, 
			m_nHeight,
			m_nWeight;
	public:
		sSavedAttributes();
		sSavedAttributes &operator=(CAttributes *Att);
	};
private:
	short m_nState;
public:
	char m_strName[MAX_NAME_SIZE];
	char m_strPassword [MAX_PASSWORD];
	char m_strDescription [MAX_STR];
	char m_strLongDescription [MAX_LONG_STR];
	char m_strPoofIn[MAX_STR];
	char m_strPoofOut [MAX_STR];
	char m_strTitle[MAX_STR];
	char m_strGuildName[MAX_STR]; //guild name

	room_vnum m_lLastRentRoom,
		m_lPlayerSetHomeTown;	//so player can set hometown

	float m_fFrags;

	int	m_nCurrentHpts,
		m_nMaxHpts,
		m_nFuguePlaneTime,
		m_nPosition,
		m_nSpellSphere,
		m_nSkillSphere,
		m_nPreference;
	short int m_nManaPts,
		m_nMaxManaPts,
		m_nMovePts,
		m_nMaxMovePts,
		m_nLevel,
		m_nAlignment,
		m_nWorshipedGod,
		m_nByScreenSize,
		m_nPracticeSessions,
		m_nWimpy;

	skill_type m_Languages[TOTAL_LANGUAGES];
	skill_type m_Skills[MAX_MUD_SKILLS];
	skill_type m_Spells[MAX_SPELLS_FOR_ANY_CLASS];
	sAffect m_Affects[TOTAL_AFFECTS];

	int m_nExperience;

	sMoney m_CashOnHand,
		m_BankCash;

	sSavedAttributes m_SavedAttributes;

	int m_nExtra1, //do not remove or reorder these they are just extra so we don't have to do p-wipe if needed
		m_nExtra2,
		m_nExtra3;
public:
	static const short STATE_DELETED;
	static const short STATE_ALIVE;
public:
	sSaveChar();
	sSaveChar(CCharacterAttributes *pDeadChar,short nSaveState = sSaveChar::STATE_ALIVE);
	bool ShouldBeDeleted() {return m_nState == STATE_DELETED;}
	~sSaveChar();

};

#endif // !defined(AFX_SAVECHAR_H__0E457141_F1BD_11D1_81AA_00600834E9F3__INCLUDED_)


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
// SaveChar.cpp: implementation of the CSaveChar class.
//
//////////////////////////////////////////////////////////////////////

#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "save.char.h"

//////////////////////////////////////////////////////////////
// statics
/////////////////////////////////////////////////////////////
const short sSaveChar::STATE_DELETED = 0;
const short sSaveChar::STATE_ALIVE = 1;

///////////////////////////////////////////////////////////////
//  Nested classes
//////////////////////////////////////////////////////////////

sSaveChar::sSavedAttributes::sSavedAttributes() {
   memset(m_pnStats, 0, sizeof(short)*MAX_NUMBER_OF_STATS);
   m_dtBirth = 0;
   m_lSecondsPlayed = 0;
   m_nChClass = m_nHeight = m_nRace = m_nSex = m_nSize = m_nWeight = 0;
}

sSaveChar::sSavedAttributes & sSaveChar::sSavedAttributes::operator=(CAttributes *pAtt) {
   //update the amount of time played before we save
   pAtt->UpdateTime();
   memcpy(m_pnStats, pAtt->BaseStats, sizeof(stat_type) * MAX_NUMBER_OF_STATS);
   m_dtBirth = pAtt->m_dtBirth;
   m_lSecondsPlayed = pAtt->m_SecondsPlayed;
   m_nChClass = pAtt->BaseChClass;
   m_nHeight = pAtt->BaseHeight;
   m_nRace = pAtt->BaseRace;
   m_nSex = pAtt->BaseSex;
   m_nSize = pAtt->BaseSize;
   m_nWeight = pAtt->BaseWeight;
   return *this;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

sSaveChar::sSaveChar(CCharacterAttributes *pCharSave, short nSaveState) {
   memset(m_strName, '\0', sizeof(char)*MAX_NAME_SIZE);
   memset(m_strPassword, '\0', sizeof(char)*MAX_PASSWORD);
   memset(m_strDescription, '\0', sizeof(char)*MAX_STR);
   memset(m_strLongDescription, '\0', sizeof(char)*MAX_LONG_STR);
   memset(m_strPoofIn, '\0', sizeof(char)*MAX_STR);
   memset(m_strPoofOut, '\0', sizeof(char)*MAX_STR);
   memset(m_strTitle, '\0', sizeof(char)*MAX_STR);
   memset(m_strGuildName, '\0', sizeof(char)*MAX_STR);

   m_nState = nSaveState;
   m_SavedAttributes = pCharSave->m_pAttributes;
   m_nPracticeSessions = pCharSave->m_nPracticeSessions;
   m_nByScreenSize = pCharSave->m_nByScreenSize;
   m_nWimpy = pCharSave->m_nWimpy;
   m_nPreference = pCharSave->m_nPreference;
   m_lLastRentRoom = pCharSave->m_lLastRentRoom;
   m_lPlayerSetHomeTown = pCharSave->m_lPlayerSetHomeTown;
   m_fFrags = pCharSave->m_fFrags;
   m_nWorshipedGod = pCharSave->m_nWorshipedGod;
   m_nFuguePlaneTime = pCharSave->m_nFuguePlaneTime;
   m_nManaPts = pCharSave->m_nManaPts;
   m_nCurrentHpts = pCharSave->m_nCurrentHpts;
   m_nExperience = pCharSave->m_nExperience;
   m_nMaxHpts = pCharSave->m_nMaxHpts;
   m_nMaxManaPts = pCharSave->m_nMaxManaPts;
   m_nMaxMovePts = pCharSave->m_nMaxMovePts;
   m_nMovePts = pCharSave->m_nMovePts;
   m_nPosition = POS_STANDING;
   m_nLevel = pCharSave->m_nLevel;
   m_nAlignment = pCharSave->m_nAlignment;
   m_nSpellSphere = pCharSave->m_nSpellSphere;
   m_nSkillSphere = pCharSave->m_nSkillSphere;
   strncpy(m_strLongDescription, pCharSave->m_strLongDescription.ptr(), (pCharSave->m_strLongDescription.GetLength() > MAX_LONG_STR ? MAX_LONG_STR : pCharSave->m_strLongDescription.GetLength()));
   m_strLongDescription[MAX_LONG_STR - 1] = '\0';
   strncpy(m_strDescription, pCharSave->m_strDescription.ptr(), (pCharSave->m_strDescription.GetLength() > MAX_STR ? MAX_STR : pCharSave->m_strDescription.GetLength()));
   m_strDescription[MAX_STR - 1] = '\0';
   strncpy(m_strName, pCharSave->m_strName.ptr(), (pCharSave->m_strName.GetLength() > MAX_NAME_SIZE ? MAX_NAME_SIZE : pCharSave->m_strName.GetLength()));
   m_strName[MAX_NAME_SIZE - 1] = '\0';
   strncpy(m_strPassword, pCharSave->m_strPassword.ptr(), (pCharSave->m_strPassword.GetLength() > MAX_PASSWORD ? MAX_PASSWORD : pCharSave->m_strPassword.GetLength()));
   m_strPassword[MAX_PASSWORD - 1] = '\0';
   strncpy(m_strPoofIn, pCharSave->m_strPoofIn.ptr(), (pCharSave->m_strPoofIn.GetLength() > MAX_STR ? MAX_STR : pCharSave->m_strPoofIn.GetLength()));
   m_strPoofIn[MAX_STR - 1] = '\0';
   strncpy(m_strPoofOut, pCharSave->m_strPoofOut.ptr(), (pCharSave->m_strPoofOut.GetLength() > MAX_STR ? MAX_STR : pCharSave->m_strPoofOut.GetLength()));
   m_strPoofOut[MAX_STR - 1] = '\0';
   strncpy(m_strTitle, pCharSave->m_strTitle.ptr(), (pCharSave->m_strTitle.GetLength() > MAX_STR ? MAX_STR : pCharSave->m_strTitle.GetLength()));
   m_strTitle[MAX_STR - 1] = '\0';
   strncpy(m_strGuildName, pCharSave->m_strGuildName.ptr(), (pCharSave->m_strGuildName.GetLength() > MAX_STR ? MAX_STR : pCharSave->m_strGuildName.GetLength()));
   m_strGuildName[MAX_STR - 1] = '\0';
   m_CashOnHand = pCharSave->m_CashOnHand;
   if (pCharSave->m_pBankCash != NULL) {
	m_BankCash = *(pCharSave->m_pBankCash);
   } else {
	ErrorLog << m_strName << " has a NULL m_pBankCash" << "not saving bank cash" << endl;
   }
   int i;
   memset(m_Skills, 0, (sizeof(skill_type) * MAX_MUD_SKILLS));
   memset(m_Spells, 0, (sizeof(skill_type) * MAX_SPELLS_FOR_ANY_CLASS));
   memcpy(m_Skills, pCharSave->m_Skills, (sizeof(skill_type) * MAX_MUD_SKILLS));
   memcpy(m_Spells, pCharSave->m_Spells, (sizeof(skill_type) * MAX_SPELLS_FOR_ANY_CLASS));
   memcpy(m_Languages, pCharSave->m_Languages, sizeof(skill_type) * TOTAL_LANGUAGES);
   for (i = 0; i < TOTAL_AFFECTS; i++) {
	m_Affects[i] = pCharSave->m_Affects[i];
   }
}

sSaveChar::~sSaveChar() {

}

sSaveChar::sSaveChar() {
   m_nState = STATE_ALIVE;
   m_nExtra1 = 0;
   m_nExtra2 = 0;
   m_nExtra3 = 0;
   m_fFrags = (float) 0;
   memset(m_strName, '\0', sizeof(char)*MAX_NAME_SIZE);
   memset(m_strPassword, '\0', sizeof(char) *MAX_PASSWORD);
   memset(m_strDescription, '\0', sizeof(char) *MAX_STR);
   memset(m_strLongDescription, '\0', sizeof(char)*MAX_LONG_STR);
   memset(m_strPoofIn, '\0', sizeof(char) *MAX_STR);
   memset(m_strPoofOut, '\0', sizeof(char) *MAX_STR);
   memset(m_strTitle, '\0', sizeof(char) *MAX_STR);
   memset(m_strGuildName, '\0', sizeof(char) *MAX_STR);
   memset(m_Languages, '\0', sizeof(skill_type) * TOTAL_LANGUAGES);
}

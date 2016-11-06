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
 *  CAttributes implementation
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"

//put these in global class
#define WSTR "Strength"
#define WINT "Intelligence"
#define WWIS "Wisdom"
#define WCON "Constitution"
#define WAGI "Agility"
#define WDEX "Dexterity"
#define WCHA "Charisma"
#define WPOW "Power"
#define WLUCK "Luck"
#define GOOD "good"
#define BAD "bad"
#define FAIR "fair"
#define ROLL_BONUS 25

extern CRandom GenNum;
extern CGlobalRaceReferences cGRR;

const char *CAttributes::m_strLayout = "%20s: %s\t%20s: %s\r\n%20s: %s\t%20s: %s\r\n%20s: %s\t%20s: %s\r\n%20s: %s\t%20s: %s\r\n%20s: %s\r\n";

const short CAttributes::SAVE_BREATH = 0;
const short CAttributes::SAVE_PARA = 1;
const short CAttributes::SAVE_SPELL = 2;
const short CAttributes::SAVE_POISON = 3;


//only used by mobs.

CAttributes::CAttributes(const CMobPrototype &mob) {
    for (int i = 0; i < MAX_NUMBER_OF_STATS; i++) {
        AffectedStats[i] = BaseStats[i] = RANGE((mob.GetStat(i) + DIE((mob.GetLevel() + 2) >> 1)), MAX_STAT, 0);
    }
    AffectedSex = BaseSex = mob.GetSex();
    SetClass(mob.GetClassType());
    SetRace(mob.GetRace());
    time_t t;
    m_dtBirth = m_dtLastSave = time(&t);
    m_SecondsPlayed = 0;
}


/////////////////////////////////////
// RollNewStats
//	Rolls new stats
// written by:Demetrius Comes
////////////////////////////////////

void CAttributes::RollNewStats(CString &strMessage) {
    unsigned int rolls[5];
    //starts at 1 so we don't roll stradd
    for (int i = 1; i < MAX_NUMBER_OF_STATS; i++) {
        for (int j = 0; j < 4; j++) {
            rolls[j] = GenNum.Roll(25);
        }
        BaseStats[i] = ROLL_BONUS + rolls[0] + rolls[1] + rolls[2] + rolls[3] -
                MIN(rolls[0], MIN(rolls[1], MIN(rolls[2], rolls[3])));
        AffectedStats[i] = BaseStats[i];
    }
    if (BaseChClass == CLASS_WARRIOR && BaseStats[STR] == 100)
        BaseStats[STRADD] = AffectedStats[STRADD] = ROLL(25, 4);
    strMessage += "\r\n\r\n*Character Creation:\r\nYou current stats are:\r\n\r\n";
    GetStatMessage(strMessage);
    strMessage += "\r\n\r\n---------------------------------------------------------------------------\r\n";
    strMessage += "\r\nYou can keep these stats or roll again.\r\nYou will have 3 bonus rolls to add to any stat.\r\n";
    strMessage += "\r\nRoll again? (y) ";
    return;
}

///////////////////////////////////////
// SetRace
//	Sets race pointer
//	This function sets all variables
//	use changeRace to change affected race
//	written by: Demetrius Comes
///////////////////////////////////////

void CAttributes::SetRace(short int nRace) {
    BaseRace = AffectedRace = nRace;
    m_pRaceReference = cGRR.GetRaceReference(nRace);
    AffectedSize = BaseSize = m_pRaceReference->GetSize();
    BaseHeight = AffectedHeight = m_pRaceReference->CalcHeight();
    BaseWeight = AffectedWeight = m_pRaceReference->CalcWeight();
    for (int i = 0; i < TOTAL_SAVES; i++) {
        m_pnSaves[i] = m_pRaceReference->GetSaveAdjust(i);
    }
}

//////////////////////////
// ChangeRace
//	Changes affected race only
//	written by: Demetrius Comes
///////////////////////////////

void CAttributes::ChangeRace(short int nRace) {
    AffectedRace = nRace;
    m_pRaceReference = cGRR.GetRaceReference(nRace);
    AffectedSize = m_pRaceReference->GetSize();
    for (int i = 0; i < TOTAL_SAVES; i++) {
        m_pnSaves[i] = m_pRaceReference->GetSaveAdjust(i);
    }
}

/////////////////////////////
//	ReturnRace
//	returns character to original race
//	written by: Demetrius Comes
////////////////////////////

void CAttributes::ReturnRace() {
    AffectedRace = BaseRace;
    m_pRaceReference = cGRR.GetRaceReference(BaseRace);
    AffectedSize = BaseSize;
    AffectedHeight = BaseHeight;
    AffectedWeight = BaseWeight;
}

////////////////////////////
//	Builds the stat message
// for rolls and bonuses
///////////////////////////

void CAttributes::GetStatMessage(CString & strMessage) {
    strMessage.Format((strMessage += m_strLayout).cptr(), WSTR, GetStatLevel(BaseStats[STR]), WINT, GetStatLevel(BaseStats[INT]),
            WAGI, GetStatLevel(BaseStats[AGI]), WWIS, GetStatLevel(BaseStats[WIS]), WDEX, GetStatLevel(BaseStats[DEX]), WPOW, GetStatLevel(BaseStats[POW]),
            WCON, GetStatLevel(BaseStats[CON]), WLUCK, GetStatLevel(BaseStats[LUCK]), WCHA, GetStatLevel(BaseStats[CHA]));
}

char * CAttributes::GetStatLevel(short int Number) {
    if (Number <= 50) return (char *) BAD;
    if (Number >= 75) return (char *) GOOD;
    else return (char *) FAIR;
}


//////////////////////////
//	Put Saves into words
/////////////////////////

void CAttributes::SaveAttribute(CString & str) {
    str.Format("Breath [%d] Para [%d] Spell [%d] Poison [%d]",
            m_pnSaves[SAVE_BREATH], m_pnSaves[SAVE_PARA],
            m_pnSaves[SAVE_SPELL], m_pnSaves[SAVE_POISON]);
}

///////////////////////////
//	Make Saving Throw, Pass in save you want checked. returns true or false
//	John Comes 2-16-99
///////////////////////////

bool CAttributes::MakeSavingThrow(short nSavingThrow) {
    return ((DIE(200) - 99) < m_pnSaves[nSavingThrow]);
}

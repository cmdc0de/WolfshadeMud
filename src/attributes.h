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
 *  CAttributes
 *	Contains affected and base stats
 *	and a pointer to the global race reference class
 */

#ifndef _ATTRIBUTES_H_
#define _ATTRIBUTES_H_

#include "global.race.reference.h"
#include <time.h>
#include "constants.h"
#include "save.char.h"
#define GetConHitBonus() GetStartingHitPoints()

class CAttributes {
    friend class CCharacter;
    friend class CCharIntermediate;
    friend void CObject::WearAffects(bool bRemoving);
    friend sSaveChar::sSavedAttributes &sSaveChar::sSavedAttributes::operator=(CAttributes *pAtt);
private:
    static const char *m_strLayout;
private:
    const CRacialReference *m_pRaceReference;
    time_t m_dtBirth, m_dtLastSave;
    long m_SecondsPlayed;
    stat_type BaseStats[MAX_NUMBER_OF_STATS],
    AffectedStats[MAX_NUMBER_OF_STATS];
    short int m_pnSaves[TOTAL_SAVES];
    short int BaseSize,
    BaseSex,
    BaseChClass,
    BaseRace,
    BaseHeight,
    BaseWeight,
    AffectedHeight,
    AffectedWeight,
    AffectedSex,
    AffectedSize,
    AffectedChClass,
    AffectedRace;
protected:
    inline void Reset();
public:
    static const short SAVE_BREATH;
    static const short SAVE_PARA;
    static const short SAVE_SPELL;
    static const short SAVE_POISON;
public:
    void ReturnRace();
    void ChangeRace(short int nRace);
    void SetRace(short int nRace);
    void RollNewStats(CString &strMessage);
public:
    inline void TimePlayed(CString &str);
    inline short CalcAge();
    inline void UpdateTime();
    inline CAttributes();
    inline CAttributes(const CAttributes *pAttributes);
    CAttributes(const CMobPrototype &mob);
    inline CAttributes(const sSaveChar::sSavedAttributes &saved);
    CAttributes(const CAttributes &copy); //disabled will get complier error 
public:
    void SaveAttribute(CString &str);
    void GetStatMessage(CString &strMessage);
    char * GetStatLevel(short int Number);
    //call racialset raceskills and classtype skills

    short int GetSize() {
        return AffectedSize;
    }

    short int GetSex() {
        return AffectedSex;
    }

    short int GetRace() {
        return AffectedRace;
    }

    short int GetClass() {
        return AffectedChClass;
    }

    bool IsRace(short nRace) {
        return m_pRaceReference->IsRace(nRace);
    }

    short int GetManaAdjustInt() {
        return m_pRaceReference->GetIntManaAdjust(AffectedStats[INT]);
    }

    short int GetManaAdjustWis() {
        return m_pRaceReference->GetWisManaAdjust(AffectedStats[WIS]);
    }

    short int GetManaAdjustPow() {
        return m_pRaceReference->GetPowManaAdjust(AffectedStats[POW]);
    }

    short int GetStartingMana() {
        return m_pRaceReference->GetStartingMana(AffectedStats);
    }

    short int GetStartingMoves() {
        return m_pRaceReference->GetStartingMoves(AffectedStats);
    }

    long GetHomeTown() {
        return m_pRaceReference->GetHomeTown();
    }

    short int GetStartingHitPoints() {
        return m_pRaceReference->GetConHitPointBonus(AffectedStats[CON] / 10);
    }

    short int GetStartingAlignment() {
        return m_pRaceReference->GetStartingAlignment();
    }

    void SetClass(short int nClass) {
        BaseChClass = AffectedChClass = nClass;
    }

    void SetSex(short int nSex) {
        BaseSex = AffectedSex = nSex;
    }

    short GetACAdjust() {
        return m_pRaceReference->GetACAdjust(AffectedStats[AGI]);
    }

    short GetIntPrepTimeAdjust() {
        return m_pRaceReference->GetIntPrepTimeAdjust(AffectedStats[INT]);
    }

    short GetWisPrepTimeAdjust() {
        return m_pRaceReference->GetWisPrepTimeAdjust(AffectedStats[WIS]);
    }

    void AdjustDex(short nAdj) {
        AffectedStats[DEX] = RANGE(AffectedStats[DEX] + nAdj, 120, 0);
    }

    void AdjustStr(short nAdj) {
        AffectedStats[STR] = RANGE(AffectedStats[STR] + nAdj, 120, 0);
    }

    void AdjustAgi(short nAdj) {
        AffectedStats[AGI] = RANGE(AffectedStats[AGI] + nAdj, 120, 0);
    }

    short GetShoutDistance() {
        return AffectedStats[CON] / 5;
    }

    short GetDamageBonus() {
        return (m_pRaceReference->m_sStrength[(AffectedStats[STR] / 10)].m_nDamage + AffectedStats[STRADD] / 20);
    }

    short GetToHitBonus() {
        return (m_pRaceReference->m_sDexterity[(AffectedStats[DEX] / 10)].m_nToHit);
    }

    skill_type DodgeBonus() {
        return ((skill_type) (m_pRaceReference->m_sAgility[(AffectedStats[AGI] / 10)].m_nDodge));
    }

    skill_type GetParryBonus() {
        return ((skill_type) (m_pRaceReference->m_sDexterity[(AffectedStats[DEX] / 10)].m_nParry));
    }

    skill_type GetRiposteBonus() {
        return ((skill_type) (m_pRaceReference->m_sDexterity[(AffectedStats[DEX] / 10)].m_nRiposte));
    }

    bool TooOld() {
        return (CalcAge() >= m_pRaceReference->m_sInnateRace.m_nMaxAge);
    }

    int GetMaxWeight() {
        return m_pRaceReference->m_sStrength[(AffectedStats[STR] / 10)].m_nWeightCarried;
    }

    short GetWisdom() {
        return AffectedStats[WIS];
    }

    short GetInt() {
        return AffectedStats[INT];
    }
    bool MakeSavingThrow(short nSavingThrow);

    stat_type GetStat(short nStat) {
        return AffectedStats[nStat];
    }

    short GetHeight() {
        return AffectedHeight;
    }

    short GetWeight() {
        return AffectedWeight;
    }
};

/////////////////////////
//	resets affected stuff to be same as base

inline void CAttributes::Reset() {
    memcpy(AffectedStats, BaseStats, sizeof (stat_type) * MAX_NUMBER_OF_STATS);
    this->AffectedChClass = this->BaseChClass;
    this->AffectedHeight = this->BaseHeight;
    this->AffectedRace = this->BaseRace;
    this->AffectedSex = this->BaseSex;
    this->AffectedSize = this->BaseSize;
    this->AffectedWeight = this->BaseWeight;
    this->ChangeRace(BaseRace);
}

///////////////////////
//	Makes a string of time played

inline void CAttributes::TimePlayed(CString &str) {
    UpdateTime();
    str.Format("%s %d Days %d Hours %d Minutes %d Seconds",
            str.ptr(),
            (m_SecondsPlayed / (60 * 60 * 24)), //days
            ((m_SecondsPlayed % (60 * 60 * 24)) / (60 * 60)), //hours
            (((m_SecondsPlayed % (60 * 60 * 24)) % (60 * 60)) / 60),
            (((m_SecondsPlayed % (60 * 60 * 24)) % (60 * 60)) % 60));
}


//////////////////////////
//	Calculates players age in mud years
//

inline short CAttributes::CalcAge() {
    time_t t;
    return (short) ((time(&t) - m_dtBirth) / CMudTime::REAL_SECS_PER_MUD_YEAR) + m_pRaceReference->m_sInnateRace.m_nMinAge;
}

///////////////////////////
//	Updates the 
//	amount of time played
//	only should be called from save function
//	writen by: Demetrius Comes

inline void CAttributes::UpdateTime() {
    time_t t;
    time(&t);
    m_SecondsPlayed += (long) difftime(t, m_dtLastSave);
    m_dtLastSave = t;
}

//only characters use this constructor as of 4/2/98
//when ccharintermediate is used to make a new ccharacter
//didn't use Set functions like above so that it can be used
//for saved characters in the future...and they might still have
//an effect that keep affected different than base!

inline CAttributes::CAttributes(const CAttributes *pAttributes) {
    for (int i = 0; i < MAX_NUMBER_OF_STATS; i++) {
        BaseStats[i] = pAttributes->BaseStats[i];
        AffectedStats[i] = pAttributes->AffectedStats[i];
    }
    BaseSex = pAttributes->BaseSex;
    AffectedSex = pAttributes->AffectedSex;
    BaseSize = pAttributes->BaseSize;
    AffectedSize = pAttributes->AffectedSize;
    BaseChClass = pAttributes->BaseChClass;
    AffectedChClass = pAttributes->AffectedChClass;
    BaseRace = pAttributes->BaseRace;
    AffectedHeight = pAttributes->AffectedHeight;
    BaseHeight = pAttributes->BaseHeight;
    AffectedWeight = pAttributes->AffectedWeight;
    BaseWeight = pAttributes->BaseWeight;
    //we set the base race
    //then we call change race incase they logged out when race
    //changed...this also sets the m_pRaceReference
    memset(m_pnSaves, 0, (sizeof (short int)*TOTAL_SAVES));
    ChangeRace(pAttributes->AffectedRace);
    m_dtBirth = pAttributes->m_dtBirth;
    time_t t;
    m_dtLastSave = time(&t);
    m_SecondsPlayed = pAttributes->m_SecondsPlayed;
}

/////////////////////////////////////////////
//	Constructor for new character
//	from a saved character
//  we only save the Base values then when we
//  load the character we Set everything to base
//  stats then recaluce all the affects on the character

inline CAttributes::CAttributes(const sSaveChar::sSavedAttributes &Saved) {
    for (int i = 0; i < MAX_NUMBER_OF_STATS; i++) {
        BaseStats[i] = AffectedStats[i] = Saved.m_pnStats[i];
    }
    SetRace(Saved.m_nRace);
    SetSex(Saved.m_nSex);
    SetClass(Saved.m_nChClass);
    BaseSize = AffectedSize = Saved.m_nSize;
    BaseHeight = AffectedHeight = Saved.m_nHeight;
    BaseWeight = AffectedWeight = Saved.m_nWeight;
    m_dtBirth = Saved.m_dtBirth;
    time_t t;
    m_dtLastSave = time(&t);
    m_SecondsPlayed = Saved.m_lSecondsPlayed;
}

inline CAttributes::CAttributes() {
    for (int i = 0; i < MAX_NUMBER_OF_STATS; i++)
        BaseStats[i] = AffectedStats[i] = 0;
    BaseSex = AffectedSex = NONE;
    BaseSize = AffectedSize = NO_SIZE;
    BaseChClass = AffectedChClass = -1;
    BaseRace = AffectedRace = -1;
    m_pRaceReference = NULL;
    AffectedHeight = BaseHeight = -1;
    AffectedWeight = BaseWeight = -1;
    time_t t;
    m_dtBirth = m_dtLastSave = time(&t);
    m_SecondsPlayed = 0;
    memset(m_pnSaves, 0, (sizeof (short int)*TOTAL_SAVES));
}
#endif

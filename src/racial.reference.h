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
*  structs for ablities
*
*
*/

#ifndef _RACIALREFERENCE_H_
#define _RACIALREFERENCE_H_

#include "constants.h"

#define INNATE_ULTRAVISION (1<<0)
#define INNATE_INFRAVISION (1<<1)
#define INNATE_MAGIC_RESISTANCE (1<<2)
#define INNATE_FLY (1<<3)
#define INNATE_STRENGTH (1<<4)
#define INNATE_LARGE (1<<5)
#define INNATE_DAY_SIGHT (1<<6)
#define INNATE_SOCIAL_STEAL (1<<7)
#define INNATE_SUMMON_HORDE (1<<8)
#define INNATE_BITE (1<<9)
#define INNATE_REGENERATE (1<<10)
#define INNATE_FAERIE_FIRE (1<<11)
#define INNATE_INVIS (1<<12)
//0-120 ablitites so
#define MAX_ABLITIES_RANGE 13
#define BONUS_STARTING_HIT_POINTS 10

//temp variable
#define GEN_START_ROOM 1000013
#define GEN_EVIL_START_ROOM 902
#define GEN_GOOD_START_ROOM 901
#define GEN_NEUTRAL_START_ROOM 905
#define ILLITHID_START_ROOM 904
#define THRI_KREEN_START_ROOM 903
#define OGRE_START_ROOM 907
#define GNOLL_START_ROOM 906


class CEqInfo; //forward def
class CCharacter; //forward def
class CObject;

class CRacialReference
{
	friend class CAttributes;
protected:
	//all structs have to be protected or public. private will not allow derived classes to instaniate
	struct sInnateRace
	{
		int m_nInnate_ablities;
		short int m_nMinHeight, 
			m_nMaxHeight, 
			m_nMinWeight, 
			m_nMaxWeight,
			m_nMinAge,
			m_nMaxAge,
			m_nStartingAlignment,
			m_pnSaves[TOTAL_SAVES],
			m_nRaceWar;
		bool Has(int nAblities) const {return (m_nInnate_ablities & nAblities)==nAblities;}
		sInnateRace() 
		{
			m_nInnate_ablities=0;
			m_nMinHeight=m_nMaxHeight=m_nMinWeight=m_nMaxWeight=0;
			m_nMinAge=m_nMaxAge=m_nStartingAlignment=0;
			m_nRaceWar = 0;
			for(register int i=0;i<TOTAL_SAVES;i++)
			{
				m_pnSaves[i]=0;
			}
		}
	};
	struct sStrength
	{
		short m_nDamage, 
			m_nWeightCarried,
			m_nDoorBash;
		sStrength(){;}
		sStrength(const sStrength &copy); //disabled
		sStrength(short Dam,short Weight,short db)
		{m_nDamage = Dam;m_nWeightCarried = Weight;m_nDoorBash = db;}
		sStrength &operator=(const sStrength &r)
		{m_nDamage=r.m_nDamage;m_nWeightCarried=r.m_nWeightCarried;m_nDoorBash=r.m_nDoorBash;return *this;}
	};
	struct sDexterity
	{
		short m_nParry,
			m_nRiposte,
			m_nRogueAbilities,
			m_nToHit;
		sDexterity(){;}
		sDexterity(const sDexterity &copy); //disabled
		sDexterity(short nParry, short nRiposte, short nRogue, short nHit)
		{m_nParry=nParry;m_nRiposte=nRiposte;m_nRogueAbilities=nRogue;m_nToHit=nHit;}
		sDexterity &operator=(const sDexterity &r)
		{m_nParry=r.m_nParry;m_nRiposte=r.m_nRiposte;m_nRogueAbilities=r.m_nRogueAbilities;m_nToHit=r.m_nToHit;return *this;}
	};
	struct sAgility
	{
		short m_nACAdjustment,
			m_nTumbling,
			m_nDodge,
			m_nKick,
			m_nPunch;
		sAgility(){;}
		sAgility(const sAgility &copy); //disabled
		sAgility(short nAC, short nTumbling, short nDodge, short nKick, short nPunch)
		{m_nACAdjustment=nAC;m_nTumbling=nTumbling;m_nDodge=nDodge;m_nKick=nKick;m_nPunch=nPunch;}
		sAgility &operator=(const sAgility &r)
		{m_nACAdjustment=r.m_nACAdjustment;m_nTumbling=r.m_nTumbling;m_nDodge=r.m_nDodge;m_nKick=r.m_nKick;m_nPunch=r.m_nPunch;return *this;}
	};
	struct sConstitution
	{
		short m_nHitPointAdjust,
			m_nSavingThrowAdjust,
			m_nConSkills;
		sConstitution(){;}
		sConstitution(const sConstitution &copy); //disabled will get a complier error if you use now b/c no implementation
		sConstitution(short nHit,short nSave,short nCon)
		{m_nHitPointAdjust=nHit;m_nSavingThrowAdjust=nSave;m_nConSkills=nCon;}
		sConstitution &operator=(const sConstitution &r)
		{m_nHitPointAdjust=r.m_nHitPointAdjust;m_nSavingThrowAdjust=r.m_nSavingThrowAdjust;m_nConSkills=r.m_nConSkills;return *this;}
	};
	struct sPower
	{
		short m_nSpellEffect,
			m_nManaAdjust;
		sPower(){;}
		sPower(const sPower &copy); //disabled will get a complier error if you use now b/c no implementation
		sPower(short nSpellEffect,short nMana)
		{m_nSpellEffect=nSpellEffect;m_nManaAdjust=nMana;}
		sPower &operator=(const sPower &r)
		{m_nSpellEffect=r.m_nSpellEffect;m_nManaAdjust=r.m_nManaAdjust;return *this;}
	};
	struct sWisdom
	{
		short m_nSkillUpChance,
			m_nSpellTimeAdjust,
			m_nManaAdjust;
		sWisdom(){;}
		sWisdom(const sWisdom &copy); //disabled will get a complier error if you use now b/c no implementation
		sWisdom(short nSkillUpChance,short nSpellTimeAdjust,short nManaAdjust)
		{m_nSkillUpChance=nSkillUpChance;m_nSpellTimeAdjust=nSpellTimeAdjust;m_nManaAdjust=nManaAdjust;}
		sWisdom &operator=(const sWisdom &r)
		{m_nSkillUpChance=r.m_nSkillUpChance;m_nSpellTimeAdjust=r.m_nSpellTimeAdjust;m_nManaAdjust=r.m_nManaAdjust;return *this;}
	};
	struct sIntellegence
	{
		short m_nAwarness,
			m_nSpellTimeAdjust,
			m_nManaAdjust;
		sIntellegence(){;}
		sIntellegence(const sIntellegence &copy); //disabled will get a complier error if you use now b/c no implementation
		sIntellegence(short nAwarness,short nSpellTime,short nMana)
		{m_nAwarness=nAwarness;m_nSpellTimeAdjust=nSpellTime;m_nManaAdjust=nMana;}
		sIntellegence &operator=(const sIntellegence &r)
		{m_nAwarness=r.m_nAwarness;m_nSpellTimeAdjust=r.m_nSpellTimeAdjust;m_nManaAdjust=r.m_nManaAdjust;return *this;}
	};
	struct sCharisma
	{
		short m_nCharm,
			m_nBuySell,
			m_nAggresiveness;
		sCharisma(){;} //used to create array
		sCharisma(const sCharisma &copy); //disabled will get a complier error if you use now b/c no implementation
		sCharisma(short nCharm,short nBuySell,short nAggress) //to build right of =
		{m_nCharm = nCharm;m_nBuySell = nBuySell;m_nAggresiveness = nAggress;}
		sCharisma &operator=(const sCharisma &r) //used to set up values
		{m_nCharm = r.m_nCharm;m_nBuySell = r.m_nBuySell;m_nAggresiveness=r.m_nAggresiveness;return *this;}
	};
	//variables
protected:
	int m_nWearPositions[MAX_EQ_POS];
	short int m_nSize,m_nTotalWeaponPositions;
	long m_lStartRoom;
	sStrength m_sStrength[MAX_ABLITIES_RANGE];
	sDexterity m_sDexterity[MAX_ABLITIES_RANGE];
	sAgility m_sAgility[MAX_ABLITIES_RANGE];
	sConstitution m_sConstitution[MAX_ABLITIES_RANGE];
	sPower m_sPower[MAX_ABLITIES_RANGE];
	sWisdom m_sWisdom[MAX_ABLITIES_RANGE];
	sIntellegence m_sIntellegence[MAX_ABLITIES_RANGE];
	sCharisma m_sCharisma[MAX_ABLITIES_RANGE];
	sInnateRace m_sInnateRace;
	//static
protected:
	static const char *m_strEqPosNames[];
protected:
	virtual void PopulateInnateString(CString &strInnates);
	void CalcTotalWeaponPositions();
	virtual short GetEqPosIndex(UINT nWearBit) const;
	//static
public:
	static const short RACE_EVIL;
	static const short RACE_GOODIES;
	static const short RACE_NEUTRAL;
	static const short RACE_ALIEN;
	static const short RACE_NO_HASSLE;
public:
	CRacialReference();
	CRacialReference(const CRacialReference &copy); //broken copy constructor
	virtual CString GetInnates() const =0;
	virtual void SetStrength()=0;
	virtual void SetDexterity()=0;
	virtual void SetAgility()=0;
	virtual void SetConstitution()=0;
	virtual void SetPower()=0;
	virtual void SetWisdom()=0;
	virtual void SetIntellegence()=0;
	virtual void SetCharisma()=0;
	virtual void SetInnateRace()=0;
	virtual short int GetStartingMoves(const short int *pSkills) const;
	virtual short int GetStartingMana(const short int *pStats) const;
	virtual bool PutEqOn(CCharacter *pCh,CObject *pObj,bool bSendMsgToRoom = true) const;
	void SetAll();
	virtual ~CRacialReference();
public:
	void DoInnate(CCharacter *pCh, CString strSubCom) const;
	const short int *GetSaveArray() const {return m_sInnateRace.m_pnSaves;}
	short int GetSaveAdjust(short int nSave) const {return m_sInnateRace.m_pnSaves[nSave];}
	UINT GetWearBit(short nPos) const {return m_nWearPositions[nPos];}
	short GetMaxWeapons() const{return m_nTotalWeaponPositions;}
	short GetWeaponPosition(short nPos) const;
	const char *GetEqPosName(UINT nWearBit) const {return m_strEqPosNames[GetEqPosIndex(nWearBit)];}
	short GetIntManaAdjust(short nInt) const {return m_sIntellegence[nInt/10].m_nManaAdjust;}
	short GetWisManaAdjust(short nWis) const {return m_sWisdom[nWis/10].m_nManaAdjust;}
	short GetPowManaAdjust(short nPow) const {return m_sPower[nPow/10].m_nManaAdjust;}
	short GetIntPrepTimeAdjust(short nInt) const {return m_sIntellegence[nInt/10].m_nSpellTimeAdjust;}
	short GetWisPrepTimeAdjust(short nWis) const {return m_sWisdom[nWis/10].m_nSpellTimeAdjust;}
	short GetStartingAlignment() const {return m_sInnateRace.m_nStartingAlignment;}
	long GetHomeTown() const {return m_lStartRoom;}
	short GetConHitPointBonus(short nCon) const {return m_sConstitution[nCon].m_nHitPointAdjust + BONUS_STARTING_HIT_POINTS;}
	short GetACAdjust(short nAbil) const {return (m_sAgility[nAbil/10].m_nACAdjustment)*10;}
	short GetSize() const {return m_nSize;}
	bool HasInnate(int nInnate) const {return m_sInnateRace.Has(nInnate);}
	bool HasRegen() const {return m_sInnateRace.Has(INNATE_REGENERATE);}
	bool IsRace(short nRaceWar) const {return m_sInnateRace.m_nRaceWar==nRaceWar;}
	short CalcHeight() const;
	short CalcWeight() const;
};
#endif


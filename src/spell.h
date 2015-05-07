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
// Spell.h: interface for the CSpell class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SPELL_H_
#define _SPELL_H_

#include "wolfshade.string.h"
#include "constants.h"

//Affect Area
#define AREA_NONE 0
#define AREA_SELF (1<<0)
#define AREA_TARGET (1<<1)
#define AREA_ROOM (1<<2)
//offensive or defensive or ...hrm what else
#define SPELL_OFFENSIVE (1<<10)
#define SPELL_DEFENSIVE (1<<11)
#define SPELL_DOES_DAM (1<<12) //used for ai

#define NOT_PREPARED_PENALITY 4

template<class CasterClass>
class CSpell  
{
protected:
	//function ptr to function in spell cast to run when spell is casted
	void (CasterClass::*m_pSpellFnc)(const CInterp *pInterp,const CSpell<CasterClass> *pSpell);
	CString m_strName,
		m_strColorName;
	int m_nSpellSphere,
		m_nAffectArea; //self, target, room etc
	short m_nAffect, //if spell only produces one affect use this to set it
		m_nCastingTime, //sec
		m_nDice, //dice type to roll for damage
		m_nRolls, //number of times to roll that dice
		m_nAddDmg, //number added to dmg
		m_nPrepTime, //time it takes to prep the spell
		m_nManaUsed, //mana needed to prep the spell
		m_nComponents, //number of spell components
		m_nMinLevel, //min level to cast this spell
		m_nSpellID; //ID of spell so we know where to check in the Spell array for ability of casting this spell
	long *m_pComponents; //Array for vnums for spell components

public:
	bool IsOffensive() const {return (SPELL_OFFENSIVE & m_nAffectArea) ? true : false;}
	bool IsDesfensive() const {return (SPELL_DEFENSIVE & m_nAffectArea) ? true : false;}
	bool IsSpellDamaging() const {return (SPELL_DOES_DAM & m_nAffectArea) ? true : false;}
	//for is of sphere the bit must match exactly
	bool IsOfSphere(int nSphere) const {return ((m_nSpellSphere & nSphere)==nSphere) ? true : false;}
	short GetUndeadType() const {return m_nAddDmg;}
	short GetWallType() const {return m_nAddDmg;}
	short GetFirstElementalVnum() const {return m_nDice;}
	short GetSecondElementalVnum() const {return m_nRolls;}
	short GetElementalCreatedVnum() const {return m_nAddDmg;}
	short GetMinLevel() const {return m_nMinLevel;}
	int GetSphere() const {return m_nSpellSphere;}
	short GetMana(bool bMemed=true) const{return bMemed ? m_nManaUsed : (m_nManaUsed*NOT_PREPARED_PENALITY);}
	short GetSpellID() const {return m_nSpellID;}
	long GetPrepTime() const {return m_nPrepTime;}
	long GetLagTime() const {return (m_nCastingTime * CMudTime::PULSES_PER_CASTING_ROUND);}
	short GetSpellAffect() const {return m_nAffect;}
	CString GetSpellName() const {return m_strName;}
	CString GetColorizedName() const {return m_strColorName;}
	int GetAffectedArea() const {return m_nAffectArea;}
	bool IsAffectedArea(int nArea) const {return (nArea & m_nAffectArea) ? true : false;}
	//damage is roll times pct effecticy in spell + the plus damage
	inline int GetDamage(short nPct) const;
	inline void Cast(const CInterp *pInterp, CasterClass *pCh) const;
	//constructor	
	CSpell(CString strName, CString strColorName, short nSpellID, short nAffect, int nAffectArea,
	short nCastingTime, short nRolls, short nDice, short nAddDmg,
	short nPrepTime,short nManaUsed,short nMinLevel,
	void (CasterClass::*SpellFnc)(const CInterp *pInterp,const CSpell<CasterClass> *pSpell),
		int nSpellSphere = 0)
		{
			m_nSpellSphere = nSpellSphere;
			m_strColorName = strColorName;
			m_nSpellID = nSpellID;
			m_strName = strName;
			m_nAffect = nAffect;
			m_nAffectArea = nAffectArea;
			m_nCastingTime = nCastingTime;
			m_nDice = nDice;
			m_nRolls = nRolls;
			m_nAddDmg = nAddDmg;
			m_nPrepTime = nPrepTime;
			m_nManaUsed = nManaUsed;
			m_nMinLevel = nMinLevel;
			m_pComponents = NULL;
			m_nComponents = 0;
			m_pSpellFnc = SpellFnc;
		}
	CSpell(const CSpell &copy); //break copy contructor
	virtual ~CSpell();
};

template<class CasterClass>
inline void CSpell<CasterClass>::Cast(const CInterp *pInterp, CasterClass *pCh) const
{
	(pCh->*m_pSpellFnc)(pInterp,this);
}

template<class CasterClass>
inline int CSpell<CasterClass>::GetDamage(short nPct) const 
{
	int nDam = ROLL(m_nDice,m_nRolls);
	return nDam + ((nDam*nPct)/100) + m_nAddDmg;
}

template<class CasterClass>
CSpell<CasterClass>::~CSpell()
{
	delete [] m_pComponents;
}

//////////////////////////////////////////////////////////////////////////
//	Spell Utlity classes!
//////////////////////////////////////////////////////////////////////////
///


/////////////////////////
//CPreparing
//	used while a spell is being preped!
////////////////////////
template<class CasterClass>
class CPreparing
{
private:
	const CSpell<CasterClass> *m_pSpell;
	long m_lTimeLeft, m_lOriginalTime;
public:
	CPreparing(const CSpell<CasterClass> *pSpell,long lTimeLeft)
	{m_pSpell = pSpell;m_lTimeLeft = lTimeLeft;m_lOriginalTime = lTimeLeft;}
	CPreparing(const CSpell<CasterClass> *pSpell,long lTimeLeft, long lOriginalTime)
	{m_pSpell = pSpell;m_lTimeLeft = lTimeLeft;m_lOriginalTime = lOriginalTime;}
	operator long() {return m_lTimeLeft;}
	void ResetTime() {m_lTimeLeft = m_lOriginalTime;}
	long GetSecondsLeft() {return m_lTimeLeft/CMudTime::PULSES_PER_REAL_SECOND;}
	CPreparing<CasterClass> &operator--() {m_lTimeLeft--;return *this;}
	const CSpell<CasterClass> *GetSpell() const {return m_pSpell;}
	bool TimeToCheckMed() {return m_lTimeLeft == (m_lOriginalTime>>1);}
	long GetOriginalTime() const {return m_lOriginalTime;}
	long GetPulsesLeft() const {return m_lTimeLeft;}
};
//////////////////////////////
//	CPrepared
//	used when spell is prepared!
////////////////////////////
template<class CasterClass>
class CPrepared
{
private:
	const CSpell<CasterClass> *m_pSpell;
	short m_nPrepared;
public:
	CPrepared(const CPreparing<CasterClass> *pPrep)
	{m_pSpell = pPrep->GetSpell();m_nPrepared = 1;}
	//used for loading saved spells
	CPrepared(const CSpell<CasterClass> *pSpell,short nPrepared)
	{m_pSpell=pSpell;m_nPrepared = nPrepared;}
	CPrepared<CasterClass> &operator++() {m_nPrepared++;return *this;}
	operator short() {return m_nPrepared;}
	void Reduce() {m_nPrepared--;}
	const CSpell<CasterClass> *GetSpell() {return m_pSpell;}
};
////////////////////////////////
//	Holding class for the spell the caster is currently
//	casting!
//////////////////////////////
template<class CasterClass>
class CCurrentSpell
{
public:
	const CSpell<CasterClass> *m_pSpell;
	CCharacter *m_pTarget;
	CString m_strTarget;
	CCurrentSpell() {m_pSpell = NULL;m_pTarget=NULL;}
	void Empty() {m_pSpell = NULL;m_pTarget=NULL;m_strTarget.Empty();}
};
#endif
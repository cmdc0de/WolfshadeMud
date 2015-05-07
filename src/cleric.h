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
// Cleric.h: interface for the CCleric class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Cleric_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)
#define AFX_Cleric_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "hash.h"
#include "spell.h"

class CCleric : public CCaster<CCleric>
{
protected:
	static const UINT GENERAL;
//static members
protected:
	static CProficiency m_Proficiency;
	static CString m_strProficiency;
	//stores spells
	static bool StaticsInitialized;
	static CSkillInfo m_SkillInfo[MAX_MUD_SKILLS];
	static CString m_strSpellList;
	static CMap<CString,CSpell<CCleric> *> m_SpellTable;
	const static short int m_pThaco[];
	static void InitStatics();
	static const int m_Experience[LVL_IMP+1];
	static void InitSkillInfo();
//none static members
protected:
	//do nothing on mob construction
	CCleric(CMobPrototype &mob,CRoom *pPutInRoom);
protected:
	virtual CProficiency &GetProficiency(){return m_Proficiency;}
	virtual const CMap<CString,CSpell<CCleric> *> *GetSpellTable() const {return &m_SpellTable;}
	virtual short GetMaxHitPointsPerLevel();
	virtual short GetManaAdjust();
	virtual const CSkillInfo *GetSkillInfo(short nSkill);
	virtual int GetExpGap() {return m_Experience[GetLevel()+1] - m_Experience[GetLevel()];}
	virtual int GetExpInLevel() {return m_nExperience - m_Experience[GetLevel()];}
	virtual void SendProficiency();
	virtual void SendSpellList();
	virtual int GetExp(short nLvl);
	virtual const CSpell<CCleric> *GetSpellInfo(CString strSpell);
	virtual int CalcSpellDam(const CSpell<CCleric> *pSpell);
	virtual short GetPctSpellUp(const CSpell <CCleric> *pSpell);
protected:
	void HealSpell(const CInterp *pInterp,const CSpell<CCleric> *pSpell);
	void BlessCurse(const CInterp *pInterp,const CSpell<CCleric> *pSpell);
	void SpellAffectAdd(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void DestroyUndead(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void TurnUndead(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void DispelMagic(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void MovementGainSpell(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void MovementLossSpell(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void HolyWord(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void DivineIntervention(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void GuksMantleSpell(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void DuthsWarmth(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void WordOfRecall(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void ContinualLight(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void Resurrection(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
	void AnimateDead(const CInterp *pInterp, const CSpell<CCleric> *pSpell);
public:
	static void CleanNewedStaticMemory();
	virtual short GetThaco();
	CCleric(CCharIntermediate *ch);
	virtual ~CCleric();
public:
	static const int CLERIC_FLAMESTRIKE;
	static const int CLERIC_FULLHEAL;
	static const int CLERIC_MASSHEAL;
	static const int CLERIC_HEAL;
	static const int CLERIC_CURELIGHTWOUNDS;
	static const int CLERIC_CURESERIOUSWOUNDS;
	static const int CLERIC_CURECRITICALWOUNDS;
	static const int CLERIC_CAUSELIGHTWOUNDS;
	static const int CLERIC_CAUSESERIOUSWOUNDS;
	static const int CLERIC_CAUSECRITICALWOUNDS;
	static const int CLERIC_FULLHARM;
	static const int CLERIC_HARM;
	static const int CLERIC_BLESS;
	static const int CLERIC_CURSE;
	static const int CLERIC_ARKANS_BATTLE_CRY;
	static const int CLERIC_DESTROY_UNDEAD;
	static const int CLERIC_TURN_UNDEAD;
	static const int CLERIC_DISPEL_MAGIC;
	static const int CLERIC_VIGORIZE_LIGHT;
	static const int CLERIC_VIGORIZE_SERIOUS;
	static const int CLERIC_VIGORIZE_CRITICAL;
	static const int CLERIC_FULL_VIGORIZE;
	static const int CLERIC_MASS_VIGORIZE;
	static const int CLERIC_FATIGUE_LIGHT;
	static const int CLERIC_FATIGUE_SERIOUS;
	static const int CLERIC_FATIGUE_CRITICAL;
	static const int CLERIC_FULL_FATIGUE;
	static const int CLERIC_MASS_FATIGUE;
	static const int CLERIC_SPIRIT_ARMOR;
	static const int CLERIC_DIVINE_ARMOR;
	static const int CLERIC_HOLY_WORD;
	static const int CLERIC_UNHOLY_WORD;
	static const int CLERIC_DIVINE_INTERVENTION;
	static const int CLERIC_GUKS_MANTLE;
	static const int CLERIC_DUTHS_WARMTH;
	static const int CLERIC_WORD_OF_RECALL;
	static const int CLERIC_CONTINUAL_LIGHT;
	static const int CLERIC_RESURRECTION;
	static const int CLERIC_ANIMATE_DEAD;
	static const int CLERIC_BLINDNESS;
};
#endif // !defined(AFX_Cleric_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)

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
// Empath.h: interface for the CEmpath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Empath_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)
#define AFX_Empath_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "hash.h"
#include "spell.h"

class CEmpath : virtual public CCharacter  
{
protected:
	static const UINT GENERAL;
//static members
protected:
	static CProficiency m_Proficiency;
	static CString m_strProficiency;
	static CSkillInfo m_SkillInfo[MAX_MUD_SKILLS];
	static bool StaticsInitialized;
	static CString m_strSpellList;
	//stores spells
	static CMap<CString,const CSpell<CEmpath> *> m_SpellTable;
	const static short int m_pThaco[];
	static void InitStatics();
	static const int m_Experience[LVL_IMP+1];
	static void InitSkillInfo();
//none static members
protected:
	int CalcSpellDam(const CSpell<CEmpath> * pSpell);
	//struct stores all information about the spell being casted
	CCurrentSpell<CEmpath> m_CurrentSpell;
	//do nothing on mob construction
	CEmpath(CMobPrototype &mob,CRoom *pPutInRoom);
	//
	bool m_bHasSpelledUp;
protected:
	virtual void BuildPracticePriceSpells(const CCharacter *pToBeTaught,CString &str) const;
	virtual bool SetSpell(const CCharacter *pCh,CString strSpell, skill_type nValue);
	virtual CProficiency &GetProficiency(){return m_Proficiency;}
	virtual void SendProficiency();
	virtual short GetMaxHitPointsPerLevel();
	virtual void DoWill(const CInterp *pInterp);
	virtual void DoPractice(CString strToPractice, const CCharacter *pTeacher);
	virtual short GetManaAdjust();
	virtual void DoSpells();
	virtual bool CastingTargetInRoom();
	virtual const CSkillInfo* GetSkillInfo(short nSkill);
	virtual int GetExpGap() {return m_Experience[GetLevel()+1] - m_Experience[GetLevel()];}
	virtual int GetExpInLevel() {return m_nExperience - m_Experience[GetLevel()];}

protected:
	virtual void SendSpellList();
	virtual int GetExp(short nLvl);
	void OffensiveSpell(const CInterp *pInterp,const CSpell<CEmpath> *pSpell);
	void HealSpell(const CInterp *pInterp,const CSpell<CEmpath> *pSpell);
	void SpellAffectAdd(const CInterp *pInterp,const CSpell<CEmpath> *pSpell);
	bool LostConcentration();
	void SpellUp(const CSpell<CEmpath> *pSpell);
	short GetPctSpellUp(const CSpell <CEmpath> * pSpell);
public:
	virtual short GetThaco();
	static void CleanNewedStaticMemory();
	virtual bool HasCommand();
	CEmpath(CCharIntermediate *ch);
	virtual ~CEmpath();
};
#endif // !defined(AFX_Empath_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)

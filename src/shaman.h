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
// Shaman.h: interface for the CShaman class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Shaman_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)
#define AFX_Shaman_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "hash.h"
#include "spell.h"

class CShaman : public CCaster<CShaman>
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
	static CMap<CString,CSpell<CShaman> *> m_SpellTable;
	const static short int m_pThaco[];
	static void InitStatics();
	static const int m_Experience[LVL_IMP+1];
	static void InitSkillInfo();
//none static members
protected:
	//do nothing on mob construction
	CShaman(CMobPrototype &mob,CRoom *pPutInRoom);
protected:
	virtual CProficiency &GetProficiency(){return m_Proficiency;}
	virtual const CMap<CString,CSpell<CShaman> *> *GetSpellTable() const {return &m_SpellTable;}
	virtual short GetMaxHitPointsPerLevel();
	virtual short GetManaAdjust();
	virtual const CSkillInfo *GetSkillInfo(short nSkill);
	virtual int GetExpGap() {return m_Experience[GetLevel()+1] - m_Experience[GetLevel()];}
	virtual int GetExpInLevel() {return m_nExperience - m_Experience[GetLevel()];}
	virtual void SendProficiency();
	virtual void SendSpellList();
	virtual int GetExp(short nLvl);
	virtual const CSpell<CShaman> *GetSpellInfo(CString strSpell);
	virtual int CalcSpellDam(const CSpell<CShaman> *pSpell);
	virtual short GetPctSpellUp(const CSpell <CShaman> *pSpell);
protected:
	void SpellAffectAdd(const CInterp *pInterp, const CSpell<CShaman> *pSpell);
	void MovementGainSpell(const CInterp *pInterp, const CSpell<CShaman> *pSpell);
	void MovementLossSpell(const CInterp *pInterp, const CSpell<CShaman> *pSpell);
	void HealSpell(const CInterp *pInterp, const CSpell<CShaman> *pSpell);
public:
	static void CleanNewedStaticMemory();
	virtual short GetThaco();
	CShaman(CCharIntermediate *ch);
	virtual ~CShaman();
};
#endif // !defined(AFX_Shaman_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)

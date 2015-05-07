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
// AntiPaladin.h: interface for the CAntiPaladin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AntiPaladin_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)
#define AFX_AntiPaladin_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "hash.h"
#include "spell.h"

class CAntiPaladin : public CCaster<CAntiPaladin>
{
protected:
	static const UINT GENERAL;
//static members
protected:
	static CProficiency m_Proficiency;
	static CString m_strProficiency;
	static bool StaticsInitialized;
	static CSkillInfo m_SkillInfo[MAX_MUD_SKILLS];
	static CString m_strSpellList;
	//stores spells
	static CMap<CString,CSpell<CAntiPaladin> *> m_SpellTable;
	//stores thing the AntiPaladin can minor create
	static CMap<CString,long> m_CanMinorCreate;
	const static short int m_pThaco[];
	static void InitStatics();
	static void InitSkillInfo();
	static const int m_Experience[LVL_IMP+1];
	static const short LVLS_PER_2H_WIELD_BONUS;
//none static members
protected:
	//do nothing on mob construction
	CAntiPaladin(CMobPrototype &mob,CRoom *pPutInRoom);
protected:
	virtual CProficiency &GetProficiency(){return m_Proficiency;}
	virtual const CMap<CString,CSpell<CAntiPaladin> *> *GetSpellTable() const{return &m_SpellTable;}
	virtual void SendProficiency();
	virtual int GetExp(short nLvl);
	virtual short GetMaxHitPointsPerLevel();
	virtual void SendSpellList();
	virtual const CSkillInfo *GetSkillInfo(short nSkill);
	virtual int GetExpGap() {return m_Experience[GetLevel()+1] - m_Experience[GetLevel()];}
	virtual int GetExpInLevel() {return m_nExperience - m_Experience[GetLevel()];}
	virtual const CSpell<CAntiPaladin> *GetSpellInfo(CString strSpell);
	virtual int CalcSpellDam(const CSpell<CAntiPaladin> * pSpell);
	virtual short GetPctSpellUp(const CSpell <CAntiPaladin> *pSpell);
	virtual void DoIdentify();
	virtual void DoHitAll(const CInterp *pInterp);
	virtual void DoBash(const CInterp *pInterp, CCharacter *pTarget);
	virtual void DoCast(const CInterp *pInterp, bool bForceCast);
protected:
	void SpellAffectAdd(const CInterp *pInterp,const CSpell<CAntiPaladin> *pSpell);
	void Curse(const CInterp *pInterp,const CSpell<CAntiPaladin> *pSpell);
	void HolyWord(const CInterp *pInterp,const CSpell<CAntiPaladin> *pSpell);
	void TurnUndead(const CInterp *pInterp,const CSpell<CAntiPaladin> *pSpell);
public:
	virtual short GetThaco();
	static void CleanNewedStaticMemory();
	CAntiPaladin(CCharIntermediate *ch);
	virtual ~CAntiPaladin();
private:
	virtual void ResetCharVars();
};
#endif // !defined(AFX_AntiPaladin_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)

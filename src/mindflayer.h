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
// Mindflayer.h: interface for the CMindflayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Mindflayer_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)
#define AFX_Mindflayer_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "hash.h"
#include "spell.h"

class CMindflayer : public CThinker<CMindflayer>  
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
	static CMap<CString,CSpell<CMindflayer> *> m_SpellTable;
	const static short int m_pThaco[];
	static void InitStatics();
	static const int m_Experience[LVL_IMP+1];
	static void InitSkillInfo();
	static const int LVL_CAN_BE_DOMINATED;
	static const int MAX_CAN_BE_DOMINATED;
//none static members
protected:
	//do nothing on mob construction
	CMindflayer(CMobPrototype &mob,CRoom *pPutInRoom);
protected:
	virtual short GetPctSpellUp(const CSpell<CMindflayer> *pSpell) const;
	virtual CProficiency &GetProficiency(){return m_Proficiency;}
	virtual void SendProficiency();
	virtual short GetMaxHitPointsPerLevel();
	virtual short GetManaAdjust();
	virtual const CSkillInfo* GetSkillInfo(short nSkill);
	virtual const CMap<CString,CSpell<CMindflayer> *> *GetSpellTable() const {return &m_SpellTable;}
	virtual const CSpell<CMindflayer> *GetSpellInfo(CString strSpell) const;
	virtual int GetExpGap() {return m_Experience[GetLevel()+1] - m_Experience[GetLevel()];}
	virtual int GetExpInLevel() {return m_nExperience - m_Experience[GetLevel()];}

protected:
	virtual void SendSpellList();
	virtual int GetExp(short nLvl);
	void Ruse(const CInterp *pInterp,const CSpell<CMindflayer> *pSpell);
	void Confusion(const CInterp *pInterp,const CSpell<CMindflayer> *pSpell);
	void SelfSacrifice(const CInterp *pInterp,const CSpell<CMindflayer> *pSpell);
	void Wormhole(const CInterp * pInterp, const CSpell<CMindflayer> * pSpell);
	void PsionicBlast(const CInterp * pInterp, const CSpell<CMindflayer> * pSpell);
	void Shards(const CInterp * pInterp, const CSpell<CMindflayer> * pSpell);
	void Domination(const CInterp * pInterp, const CSpell<CMindflayer> * pSpell);
	void SpellAffectAdd(const CInterp * pInterp, const CSpell<CMindflayer> * pSpell);
public:
	virtual short GetThaco();
	static void CleanNewedStaticMemory();
	CMindflayer(CCharIntermediate *ch);
	virtual ~CMindflayer();
};
#endif // !defined(AFX_Mindflayer_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)

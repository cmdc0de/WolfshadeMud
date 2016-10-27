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
// NPC.h: interface for the CNPC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPC_H__5A4A7241_FCBA_11D1_81AA_00600834E9F3__INCLUDED_)
#define AFX_NPC_H__5A4A7241_FCBA_11D1_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "character.h"
#include "list.h"
#include "money.h"	// Added by ClassView

class CNPC : virtual public CCharacter
{
protected:
	static const short SMART_SCAVENGER_PCT;
	static const short SCAVENGER_PCT;
	static const short MAX_PCT_SCAVENGE;
	static const short MAX_MOB_SKILL;
	static const int PULSES_PER_DO_MOB_MOVEMENT;
	static const int COLISEUM_TIME;
	static short PCT_MOB_MOVEMENT;
	static CString MOHR_TO_ORC;
	static CString MOHR_TO_GNOLL;
protected:
	CCharacter *m_pSwitched;
	CString m_strDefaultPosDesc;
	UINT m_nDefaultPos;
	long m_nVNum;
	int m_nExtraExp;
	short int m_nThaco;
	CList<long> m_Memory;
protected:
	bool CheckForAggroAndGuarding(const CInterp *pInterp);
	bool DoMemoryAI();
	bool IsAggro(CCharacter *pCh);
	void DoColiseumAI();
	void DoColiseumFightingAI();
	void DoWCMilitiaAI();
	void DoSwampMohrAI();
	void DoSwampMohrBattleAI();
	void DoKipuAI();
	short int CalcACMobWouldHit();
	CNPC(CMobPrototype &mob, CRoom *pPutInRoom);
	void MoveMob();
protected:
	virtual void DoBreakMastersHold();
	virtual int GetExtraExp() {return m_nExtraExp;}
	virtual bool IsGoodEnoughToTeach (const CCharacter *pToBeTaught, short nSkill, bool bSkill) const;
	virtual void Track();
	virtual void DoHelpMaster(const CInterp *pInterp);
	virtual sMoney GetTrainingCosts(const CCharacter *pToBeTrained, short nSkill, bool bIsSkill) const;
	virtual skill_type CanTeach(const CCharacter *pCh, short nSkill, bool bIsSkill) const;
	virtual void DoAttributes();
	virtual void DoTog();
	virtual void DoReturn();
	virtual void SetSwitched(CCharacter *pCh) {m_pSwitched = pCh;}
	virtual short int CalcDamage(short nPos,CCharacter *pDefender);
	virtual bool IsTeacher() const {return Prefers(TEACHER);}
	virtual bool IsSwitchedTo() {return (m_pSwitched!=NULL);}
	virtual void DoAIAttack(const CInterp *pInterp, CCharacter *pTarget) {this->DoKill(pInterp,pTarget);}
	virtual void InitializeCharacter();
public:
	static const int DO_AI;
	static short SWAMP_MOHR_STATUS;
public:
	long GetVnum() {return m_nVNum;}
public:
	virtual void CleanUp();
	virtual bool CheckVoid() {m_lVoidTime=0;return false;}
	virtual int AdvanceLevel(bool IncreaseLevel,bool bSendMsg = true,bool bIsFollower = false);
	virtual void RemoveFromMemory(CCharacter *pCh);
	virtual void DoAI(const CInterp *pInterp);
	virtual UINT GetDefaultPos() {return m_nDefaultPos;}
	virtual CString GetDefaultPosDesc();
	virtual bool IsNPC() const;
	virtual void SendToChar(const char * strMsg,bool bColorize=true) const;
	virtual void SendToChar(CString strMsg,bool bColorize=true) const;
	virtual void AddMemory(CCharacter *pFighter);
	virtual void SendCharPage(CString str) {SendToChar(str);} //no paging for npc's
	virtual ~CNPC();
public:
	virtual bool IsGod() const {return false;} //npc's can't be gods
public: //static
	static const int SENTINEL;
	static const int SCAVENGER;
	static const int AWARE;
	static const int AGGRESSIVE;
	static const int STAYZONE;
	static const int FLEEING;
	static const int AGGROEVIL;
	static const int AGGROGOOD;
	static const int AGGRONEUTRAL;
	static const int MEMORY;
	static const int AGGROEVILRACE;
	static const int AGGROGOODRACE;
	static const int AGGRONEUTRALRACE;
	static const int AGGROALIENRACE;
	static const int NOBASH;
	static const int TRACK;
	static const int TEACHER;
	static const int GUARD_GOOD;
	static const int GUARD_EVIL;
	static const int GUARD_NEUTRAL;
	static const int GUARD_NPC;
	static const int GUARD_CHARISMA;
	static const int GUARD_RACE;
	static const int GUARD_GOOD_RACE;
	static const int GUARD_EVIL_RACE;
	static const int GUARD_NEUTRAL_RACE;
	static const int GUARD_ALIEN_RACE;
	static const int PERM_DI;

	static const int AGGRO_MOB;
	static const int GUARD;
};

#endif // !defined(AFX_NPC_H__5A4A7241_FCBA_11D1_81AA_00600834E9F3__INCLUDED_)

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
* Class Rogue
*	Holds all rogue only code
*
*
*/

#ifndef _Rogue_H_
#define _Rogue_H_

class CRogue : virtual public CCharacter
{
protected:
	static const int GENERAL;
	static const int ROGUE;
	static const int ENTERTAINER;
	static const int THIEF;
	static const int BRIGAND;
	static const int BARD;
	static const int ACROBAT;
	static const int CUTPURSE;
	static const int SWINDLER;
	static const int ASSASSIN;
	static const int NINJA;
	static const int MERCENARY;
	static const int SCOUT;
protected:
	static CSkillInfo m_SkillInfo[MAX_MUD_SKILLS];
	static void InitSkillInfo();
	static bool StaticsInitialized;
	static const int m_Experience[LVL_IMP+1];
	static CProficiency m_Proficiency;
	static CString m_strProficiency;
	const static short int m_pThaco[];
protected:
	CRogue(CMobPrototype &mob,CRoom *pPutInRoom);
protected:
	void DoBackStab(const CInterp *pInterp, CCharacter *pTarget);
	void DoThroatSlit(const CInterp *pInterp, CCharacter *pTarget);
	void DoSneak(const CInterp *pInterp);
	void DoHide();
	void DoCircle(const CInterp *pInterp);
	void DoBeg();
	void DoCharm();
protected:
	virtual CProficiency &GetProficiency(){return m_Proficiency;}
	virtual void SendProficiency();
	virtual const CSkillInfo* GetSkillInfo(short nSkill);
	virtual short GetMaxHitPointsPerLevel();
	virtual int GetExp(short nLvl);
	virtual int GetExpGap() {return (m_Experience[GetLevel()+1] - m_Experience[GetLevel()]);}
	virtual int GetExpInLevel() {return m_nExperience - m_Experience[GetLevel()];}
	virtual void DoAppraise();
	virtual void DoIntimidate(const CInterp *pInterp);
	virtual void DoDirtToss(CCharacter *pTarget);
	virtual void DoCharacterLook(CCharacter *ch);
public:
	CRogue(CCharIntermediate *ch);
	virtual short GetThaco();
	virtual ~CRogue();
};

#endif

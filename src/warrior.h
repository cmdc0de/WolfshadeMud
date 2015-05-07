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
* Class Warrior
*	Holds all warrior only code
*
*
*/

#ifndef _WARRIOR_H_
#define _WARRIOR_H_

class CWarrior : virtual public CCharacter
{
protected:
	static const int GENERAL;
	static const int WARRIOR;
	static const int MARTIAL_ARTIST;
	static const int SWORDSMAN;
	static const int FIGHTER;
	static const int SAMURAI;
	static const int MONK;
	static const int WEAPONS_MASTER;
	static const int SWASHBUCKLER;
	static const int KNIGHT;
	static const int THUG;

protected:
	static CSkillInfo m_SkillInfo[MAX_MUD_SKILLS];
	static void InitSkillInfo();
	static bool StaticsInitialized;
	static const int m_Experience[LVL_IMP+1];
	static CProficiency m_Proficiency;
	static CString m_strProficiency;
	const static short int m_pThaco[];
	CWarrior(CMobPrototype &mob,CRoom *pPutInRoom);
protected:
	virtual CProficiency &GetProficiency(){return m_Proficiency;}
	virtual void SendProficiency();
	virtual int GetExp(short nLvl);
	virtual const CSkillInfo *GetSkillInfo(short nSkill);
	virtual short GetMaxHitPointsPerLevel();
	virtual int GetExpGap() {return m_Experience[GetLevel()+1] - m_Experience[GetLevel()];}
	virtual int GetExpInLevel() {return m_nExperience - m_Experience[GetLevel()];}
	virtual void DoChant();
	virtual void DoBerserk();
	virtual void DoDragonPunch(const CInterp *pInterp, CCharacter *pTarget);
	virtual void DoIdentify();
	virtual void DoQuiveringPalm(const CInterp *pInterp, CCharacter *pTarget);
	virtual void DoAppraise();
	virtual void DoIntimidate(const CInterp *pInterp);
	virtual void DoHitAll(const CInterp *pInterp);
	virtual void DoDisarm(const CInterp *pInterp, CCharacter *pTarget);
public:
	CWarrior(CCharIntermediate *ch);
	virtual short GetThaco();
	virtual ~CWarrior();
};

#endif


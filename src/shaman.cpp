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
// Shaman.cpp: implementation of the CShaman class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "shaman.h"
//////////////////////////////
//	Statics
////////////////////////////

extern CRandom GenNum;

extern CGlobalVariableManagement GVM;

bool CShaman::StaticsInitialized=false;
CSkillInfo CShaman::m_SkillInfo[MAX_MUD_SKILLS];
CString CShaman::m_strSpellList;
const UINT CShaman::GENERAL = (1<<0);
CProficiency CShaman::m_Proficiency;
CString CShaman::m_strProficiency("You class has the following proficiencies:\r\n");

const short int CShaman::m_pThaco[] = 
{
	/*1,02,03,04,05,06,07,08,09,10*/
	100,98,96,94,92,90,88,86,84,82,
	 80,78,76,74,72,70,68,66,64,62,
	 60,58,56,54,52,50,48,46,44,42,
	 40,38,36,34,32,30,28,26,24,22,
	 20,18,16,14,12,10,8,6,4,2 /*51-60*/
};

const int CShaman::m_Experience[LVL_IMP+1] = 
{
	/*0*/   0,		/*1*/   1,		/*2*/   250,	/*3*/   750,
	/*4*/   1500,	/*5*/   2500,	/*6*/   3750,	/*7*/   5250,
	/*8*/   7000,	/*9*/   9000,	/*10*/  11250,	/*11*/  13750,
	/*12*/  19250,	/*13*/  25250,	/*14*/  31750,	/*15*/  38750,
	/*16*/  46250,	/*17*/  54250,	/*18*/  62750,	/*19*/  71750,
	/*20*/  81250,	/*21*/  91250,	/*22*/  112250,	/*23*/  134250,
	/*24*/  157250,	/*25*/  181250,	/*26*/  206250,	/*27*/  232250,
	/*28*/  259250,	/*29*/  287250,	/*30*/  316250,	/*31*/  376250,
	/*32*/  438250,	/*33*/  502250,	/*34*/  568250,	/*35*/  636250,
	/*36*/  706250,	/*37*/  778250,	/*38*/  852250,	/*39*/  928250,
	/*40*/  1006250,/*41*/  1166250,/*42*/  1330250,/*43*/  1498250,
	/*44*/  1670250,/*45*/  1846250,/*46*/  2026250,/*47*/  2210250,
	/*48*/  2398250,/*49*/  2590250,/*50*/  2786250,/*51*/  2986250,
	/*52*/  3394250,/*53*/  3810250,/*54*/  4234250,/*55*/  4666250,
	/*56*/  5106250,/*57*/  5554250,/*58*/  6010250,/*59*/  6474250,
	/*60*/  6946250

};
 
/*
CShamanSpell(CString strName,UINT nAffects, UINT affectedarea,
		short nCastingTime,short nDice, short nRolls,
		short nPrepTime,short nManaUsed, short nMinLevel, 
		function,UINT nSpellSphere)
*/

//CMemoryAllocator<CMap<CString,char *,CShamanSpell>::CNode> CMap<CString,char *,CShamanSpell>::CNode::m_Memory(1,10,sizeof(CMap<CString,char *,CShamanSpell>::CNode));
CMap<CString,CSpell<CShaman> *> CShaman::m_SpellTable((26*MIN_SPELL_LETTERS),MIN_SPELL_LETTERS);

////////////////////////////
//Spell id's for Shamans
//	How spell id's work...each class defines a spell id for each spell
//	this number is then used to reference the ability to cast that spell
//	in m_Spells[].
#define SHAMAN_SPELLS 0


//////////////////////////////

///////////////////////////////////////
//	InitSpellTable
//	Initalizes the spells and the things
//	this type of caster can cast and minor
//	create.
//	written by: Demetrius Comes
////////////////////////////////////
void CShaman::InitStatics()
{
	if(StaticsInitialized)
	{
		return;
	}
	StaticsInitialized = true;
	InitSkillInfo();

	m_SpellTable.Add("Flamestrike",new CSpell<CShaman>("Flamestrike","Flamestrike",SHAMAN_SPELLS,
		SPELL_NO_AFFECT,AREA_TARGET,3,10,90,100,10,9,41,&CShaman::OffensiveSpell,0));
	
	


	POSITION pos = m_SpellTable.GetStartingPosition();
	bool bReturn = false;
	CSpell<CShaman> *pCurrentSpell;
	while(pos)
	{
		pCurrentSpell = m_SpellTable.GetNext(pos);
		m_strSpellList.Format("%s  (%-2d) %-25s",
			(char *)m_strSpellList,
			pCurrentSpell->GetMinLevel(),
			(char *)pCurrentSpell->GetColorizedName());
		if(bReturn)
		{
			bReturn = false;
			m_strSpellList+="\r\n";
		}
		else
		{
			bReturn = true;
		}
	}
	m_strSpellList+="\r\n";
	//if we do it once's we won't have to do it for every
	//time whe send it to character
	m_strSpellList.Colorize();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShaman::CShaman(CCharIntermediate *ch) : CCaster<CShaman>(ch),CCharacter(ch)
{
	InitStatics();
	//can't put in CCharacter construct because
	//it will be called before us
	if(IsNewbie())
	{
		InitSkills();
		m_nSkillSphere |= GENERAL;
	}
}

CShaman::CShaman(CMobPrototype &mob,CRoom *pPutInRoom)
		: CCaster<CShaman>(mob,pPutInRoom),CCharacter(mob,pPutInRoom)
{
	InitStatics();
	//everyone gets general
	m_nSkillSphere |= GENERAL;
}

CShaman::~CShaman()
{
}

/////////////////////////////
//	Sets up the skills a Shaman can learn
//	
void CShaman::InitSkillInfo()
{											/*max,min,pct*/

	
// Base Skills
	m_SkillInfo[SKILL_BODYSLAM]=CSkillInfo(GENERAL,80,1,10);
	m_SkillInfo[SKILL_BANDAGE]=CSkillInfo(GENERAL,100,1,150);
	m_SkillInfo[SKILL_BASH]=CSkillInfo(GENERAL,25,0,10);
	m_SkillInfo[SKILL_DODGE]=CSkillInfo(GENERAL,100,1,50);
	m_SkillInfo[SKILL_DUAL_WIELD]=CSkillInfo(GENERAL,75,0,10);
	m_SkillInfo[SKILL_OFFENSE]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_FIRST_AID]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_MOUNT]=CSkillInfo(GENERAL,100,1,10);
	m_SkillInfo[SKILL_BACKSTAB]=CSkillInfo(GENERAL,25,0,10);
	m_SkillInfo[SKILL_TUNDRA_MAGIC]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_DESERT_MAGIC]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_MOUNTAIN_MAGIC]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_FOREST_MAGIC]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_SWAMP_MAGIC]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_FORAGE]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_RESCUE]=CSkillInfo(GENERAL,75,1,100);
	m_SkillInfo[SKILL_SWITCH]=CSkillInfo(GENERAL,100,1,10);
	m_SkillInfo[SKILL_TRIP]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_KICK]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_CLERIC_MAGIC]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_SHAMAN_MAGIC]=CSkillInfo(GENERAL,75,1,10);
	m_SkillInfo[SKILL_BLIND_FIGHTING]=CSkillInfo(GENERAL,75,1,100);

	m_SkillInfo[SKILL_THROW_WEAPON] = CSkillInfo(GENERAL,50 ,0 ,10);
    m_SkillInfo[SKILL_PRAY] = CSkillInfo(GENERAL,100,1,100);
    m_SkillInfo[SKILL_MEDITATE] = CSkillInfo(GENERAL,100,1,100);
    m_SkillInfo[SKILL_QUICK_CHANT] = CSkillInfo(GENERAL,100,1,100);	
	
	//profeciencies
	m_Proficiency.Add("General",0,0,0,GENERAL,0);
	m_Proficiency.GetProficiencyNames(m_strProficiency);
	m_strProficiency.Format("%s\r\n%-30s\t%-20s\r\n",
		(char *)m_strProficiency,"Skill Name","Proficiency Name");
	register int i;
	CString str;
	for(i=0;i<MAX_MUD_SKILLS;i++)
	{
		if(m_SkillInfo[i].CanLearn())
		{
			str.Format("%s%-30s\t%-20s\r\n",
				(char *)str,
				(char *)FindSkill(i),
				(char *)m_Proficiency.GetSingleProficiencyName(m_SkillInfo[i].GetSet()));
		}
	}
	m_strProficiency+=str;
}

const CSkillInfo* CShaman::GetSkillInfo(short nSkill)
{
	return &m_SkillInfo[nSkill];
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////
short CShaman::GetThaco()
{
	return m_pThaco[m_nLevel];
}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////
short CShaman::GetMaxHitPointsPerLevel()
{
	return 8;
}

//////////////////////////////////
//	HealSpell
//	this function runs for all Shaman Heal spells
//	do checks to see if we can cast in this room etc.
//	next check to see if spell is room affect
//	or target
//	modified from CShaman::Offensive by John Comes
void CShaman::HealSpell(const CInterp * pInterp, const CSpell<CShaman> * pSpell)
{
	CCharacter *pTarget;
	if(pSpell->GetAffectedArea() & AREA_ROOM)
	{
		//get first character in room
		POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
		while(pos)
		{
			pTarget = m_pInRoom->GetNextCharacter(pos);
			pTarget->TakeDamage(this,-1*CalcSpellDam(pSpell),false);
			m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget);
		}
	}
	//we are casting at self or someone else not a room
	else
	{
		pTarget = m_CurrentSpell.m_pTarget;
		if(!this->IsInRoom(pTarget))
		{
			SendToChar("Who?\r\n");
			return;
		}
		pTarget->TakeDamage(this,-1*CalcSpellDam(pSpell),false);
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget);
	}
}

/////////////////////////////
//	Not really needed for real OS
//	but we'll be friendly to 95/98 =)
//////////////////////////////
void CShaman::CleanNewedStaticMemory()
{
	POSITION pos = m_SpellTable.GetStartingPosition();
	while(pos)
	{
		delete m_SpellTable.GetNext(pos);
	}
}

///////////////////////////
//	GetManaAdjust Overriden
//	returns wisdom adjustment
//
short CShaman::GetManaAdjust()
{
	return m_pAttributes->GetManaAdjustWis();
}
////////////////////////////
//	Calc spell damage
//	we have not fully defined how we will calculate
//	damange for Shamans yet
int CShaman::CalcSpellDam(const CSpell<CShaman> * pSpell)
{
	return pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]);
}

//////////////////////
//	Returns Exp for the level 
// passed in
//////////////////////
int CShaman::GetExp(short nLvl)
{
	return m_Experience[nLvl];
}

///////////////////////
//	Sends Spell list to char
//
void CShaman::SendSpellList()
{
	SendToChar(m_strSpellList);
}

////////////////////////
//SendPRoficiency layout
void CShaman::SendProficiency()
{
	CString strProf;
	strProf.Format("Current Proficiency Bonus: %d\r\n%s",
		m_Proficiency.CalcBonus(m_nSkillSphere),
		(const char *)m_strProficiency);
	SendCharPage(strProf);
}

const CSpell<CShaman> *CShaman::GetSpellInfo(CString strSpell)
{
	CSpell<CShaman> *pSpell;
	if(m_SpellTable.Lookup(strSpell,pSpell))
	{
		return pSpell;
	}
	return NULL;
}

////////////////////////////////
//  SpellAffectAdd for General Spells (no reference affects)
//  Sets Timing for General Spells that add affects
//	John Comes 12-28-98
///////////////////////////////
void CShaman::SpellAffectAdd(const CInterp * pInterp, const CSpell<CShaman> * pSpell)
{
	int nDam = CalcSpellDam(pSpell);
	POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
	switch(pSpell->GetSpellID())
	{
	default:
		//default does not use SpellMsgs
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),0);
		break;
	}
}



//////////////////////////////////
//	MovementSpell
//	This function takes care of all the gain of movement spells
//	This does all the vigorize spells
//	John Comes 3-12-99
void CShaman::MovementGainSpell(const CInterp * pInterp, const CSpell<CShaman> * pSpell)
{
	short nDam = CalcSpellDam(pSpell);
	if(pSpell->GetAffectedArea() & AREA_ROOM)
	{
		CCharacter *pTarget;
		//get first character in room
		POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
		while(pos)
		{
			pTarget = m_pInRoom->GetNextCharacter(pos);
			pTarget->AdjustCurrentMoves(CalcSpellDam(pSpell));
			m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget);
		}
	}
	//we are casting at self or someone else not a room
	else
	{
		m_CurrentSpell.m_pTarget->AdjustCurrentMoves(CalcSpellDam(pSpell));
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,m_CurrentSpell.m_pTarget);
	}
}


//////////////////////////////////
//	MovementSpell
//	This function takes care of all the loss of movement spells
//	This does all the fatigue spells
//	John Comes 3-12-99
void CShaman::MovementLossSpell(const CInterp * pInterp, const CSpell<CShaman> * pSpell)
{
	short nDam = CalcSpellDam(pSpell);
	if(pSpell->GetAffectedArea() & AREA_ROOM)
	{
		CCharacter *pTarget;
		//get first character in room
		POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
		while(pos)
		{
			pTarget = m_pInRoom->GetNextCharacter(pos);
			if(pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL))
			{
				pTarget->AdjustCurrentMoves(-CalcSpellDam(pSpell));
				m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget);
			}
			else
			{
				m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,pTarget);
			}
			AddFighters(pTarget,false);
		}
	}
	//we are casting at self or someone else not a room
	else
	{
		if(m_CurrentSpell.m_pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL))
		{
			m_CurrentSpell.m_pTarget->AdjustCurrentMoves(-CalcSpellDam(pSpell));
			m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,m_CurrentSpell.m_pTarget);
		}
		else
		{
			m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,m_CurrentSpell.m_pTarget);
		}
		AddFighters(m_CurrentSpell.m_pTarget,false);
	}
}

////////////////////////////////
//	Send back what pct for spell ups
short CShaman::GetPctSpellUp(const CSpell <CShaman> * pSpell)
{
	return m_pAttributes->GetWisdom()/5;
}

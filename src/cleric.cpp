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
// Cleric.cpp: implementation of the CCleric class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "cleric.h"
//////////////////////////////
//	Statics
////////////////////////////

extern CRandom GenNum;

extern CGlobalVariableManagement GVM;

bool CCleric::StaticsInitialized=false;
CSkillInfo CCleric::m_SkillInfo[MAX_MUD_SKILLS];
CString CCleric::m_strSpellList;
const UINT CCleric::GENERAL = (1<<0);
CProficiency CCleric::m_Proficiency;
CString CCleric::m_strProficiency("You class has the following proficiencies:\r\n");

const short int CCleric::m_pThaco[] = 
{
	/*1,02,03,04,05,06,07,08,09,10*/
	100,98,96,94,92,90,88,86,84,82,
	 80,78,76,74,72,70,68,66,64,62,
	 60,58,56,54,52,50,48,46,44,42,
	 40,38,36,34,32,30,28,26,24,22,
	 20,18,16,14,12,10,8,6,4,2 /*51-60*/
};

const int CCleric::m_Experience[LVL_IMP+1] = 
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
CClericSpell(CString strName,UINT nAffects, UINT affectedarea,
		short nCastingTime,short nDice, short nRolls,
		short nPrepTime,short nManaUsed, short nMinLevel, 
		function,UINT nSpellSphere)
*/

//CMemoryAllocator<CMap<CString,char *,CClericSpell>::CNode> CMap<CString,char *,CClericSpell>::CNode::m_Memory(1,10,sizeof(CMap<CString,char *,CClericSpell>::CNode));
CMap<CString,CSpell<CCleric> *> CCleric::m_SpellTable((26*MIN_SPELL_LETTERS),MIN_SPELL_LETTERS);

////////////////////////////
//Spell id's for Clerics
//	How spell id's work...each class defines a spell id for each spell
//	this number is then used to reference the ability to cast that spell
//	in m_Spells[].
const int CCleric::CLERIC_FLAMESTRIKE=0;
const int CCleric::CLERIC_FULLHEAL=1;
const int CCleric::CLERIC_MASSHEAL=2;
const int CCleric::CLERIC_HEAL=3;
const int CCleric::CLERIC_CURELIGHTWOUNDS=4;
const int CCleric::CLERIC_CURESERIOUSWOUNDS=5;
const int CCleric::CLERIC_CURECRITICALWOUNDS=6;
const int CCleric::CLERIC_CAUSELIGHTWOUNDS=7;
const int CCleric::CLERIC_CAUSESERIOUSWOUNDS=8;
const int CCleric::CLERIC_CAUSECRITICALWOUNDS=9;
const int CCleric::CLERIC_FULLHARM=10;
const int CCleric::CLERIC_HARM=11;
const int CCleric::CLERIC_BLESS=12;
const int CCleric::CLERIC_CURSE=13;
const int CCleric::CLERIC_ARKANS_BATTLE_CRY=14;
const int CCleric::CLERIC_DESTROY_UNDEAD=15;
const int CCleric::CLERIC_TURN_UNDEAD=16;
const int CCleric::CLERIC_DISPEL_MAGIC=17;
const int CCleric::CLERIC_VIGORIZE_LIGHT=18;
const int CCleric::CLERIC_VIGORIZE_SERIOUS=19;
const int CCleric::CLERIC_VIGORIZE_CRITICAL=20;
const int CCleric::CLERIC_FULL_VIGORIZE=21;
const int CCleric::CLERIC_MASS_VIGORIZE=22;
const int CCleric::CLERIC_FATIGUE_LIGHT=23;
const int CCleric::CLERIC_FATIGUE_SERIOUS=24;
const int CCleric::CLERIC_FATIGUE_CRITICAL=25;
const int CCleric::CLERIC_FULL_FATIGUE=26;
const int CCleric::CLERIC_MASS_FATIGUE=27;
const int CCleric::CLERIC_SPIRIT_ARMOR=28;
const int CCleric::CLERIC_DIVINE_ARMOR=29;
const int CCleric::CLERIC_HOLY_WORD=30;
const int CCleric::CLERIC_UNHOLY_WORD=31;
const int CCleric::CLERIC_DIVINE_INTERVENTION=32;
const int CCleric::CLERIC_GUKS_MANTLE=33;
const int CCleric::CLERIC_DUTHS_WARMTH=34;
const int CCleric::CLERIC_WORD_OF_RECALL=35;
const int CCleric::CLERIC_CONTINUAL_LIGHT=36;
const int CCleric::CLERIC_RESURRECTION=37;
const int CCleric::CLERIC_ANIMATE_DEAD=38;
const int CCleric::CLERIC_BLINDNESS=39;

//////////////////////////////

///////////////////////////////////////
//	InitSpellTable
//	Initalizes the spells and the things
//	this type of caster can cast and minor
//	create.
//	written by: Demetrius Comes
////////////////////////////////////
void CCleric::InitStatics()
{
	if(StaticsInitialized)
	{
		return;
	}
	StaticsInitialized = true;
	InitSkillInfo();

	m_SpellTable.Add("Flamestrike",new CSpell<CCleric>("Flamestrike","Flamestrike",CLERIC_FLAMESTRIKE,
		SPELL_NO_AFFECT,AREA_TARGET,3,5,90,100,10,9,41,&CCleric::OffensiveSpell,0));
	m_SpellTable.Add("Full Heal",new CSpell<CCleric>("Full Heal","Full Heal",CLERIC_FULLHEAL,
		SPELL_NO_AFFECT,AREA_TARGET,3,6,50,100,20,7,31,&CCleric::HealSpell,0));
	m_SpellTable.Add("Mass Heal",new CSpell<CCleric>("Mass Heal","Mass Heal",CLERIC_MASSHEAL,
		SPELL_NO_AFFECT,AREA_ROOM,3,4,50,100,15,8,36,&CCleric::HealSpell,0));
	m_SpellTable.Add("Heal",new CSpell<CCleric>("Heal","Heal",CLERIC_HEAL,SPELL_NO_AFFECT,AREA_TARGET,
		2,2,50,100,23,5,21,&CCleric::HealSpell,0));
	m_SpellTable.Add("Cure Light Wounds",new CSpell<CCleric>("Cure Light Wounds","Cure Light Wounds",CLERIC_CURELIGHTWOUNDS,
		SPELL_NO_AFFECT,AREA_TARGET,2,2,4,2,28,1,1,&CCleric::HealSpell,0));
	m_SpellTable.Add("Cure Serious Wounds",new CSpell<CCleric>("Cure Serious Wounds","Cure Serious Wounds",CLERIC_CURESERIOUSWOUNDS,
		SPELL_NO_AFFECT,AREA_TARGET,2,2,4,10,27,2,6,&CCleric::HealSpell,0));
	m_SpellTable.Add("Cure Critical Wounds",new CSpell<CCleric>("Cure Critical Wounds","Cure Critical Wounds",CLERIC_CURECRITICALWOUNDS,
		SPELL_NO_AFFECT,AREA_TARGET,2,4,4,10,25,3,11,&CCleric::HealSpell,0));
	m_SpellTable.Add("Cause Light Wounds",new CSpell<CCleric>("Cause Light Wounds","Cause Light Wounds", CLERIC_CAUSELIGHTWOUNDS,
		SPELL_NO_AFFECT,AREA_TARGET,2,2,4,2,28,1,1,&CCleric::OffensiveSpell,0));
	m_SpellTable.Add("Cause Serious Wounds",new CSpell<CCleric>("Cause Serious Wounds","Cause Serious Wounds",CLERIC_CAUSESERIOUSWOUNDS,
		SPELL_NO_AFFECT,AREA_TARGET,2,2,4,10,27,2,6,&CCleric::OffensiveSpell,0));
	m_SpellTable.Add("Cause Critical Wounds",new CSpell<CCleric>("Cause Critical Wounds","Cause Critical Wounds",CLERIC_CAUSECRITICALWOUNDS,
		SPELL_NO_AFFECT,AREA_TARGET,2,4,4,10,25,3,11,&CCleric::OffensiveSpell,0));
	m_SpellTable.Add("Full Harm",new CSpell<CCleric>("Full Harm","Full Harm",CLERIC_FULLHARM,
		SPELL_NO_AFFECT,AREA_TARGET,3,6,50,100,20,7,31,&CCleric::OffensiveSpell,0));
	m_SpellTable.Add("Harm",new CSpell<CCleric>("Harm","Harm",CLERIC_HARM,SPELL_NO_AFFECT,AREA_TARGET,
		2,2,50,100,23,5,21,&CCleric::OffensiveSpell,0));
	m_SpellTable.Add("Bless",new CSpell<CCleric>("Bless","Bless",CLERIC_BLESS,AFFECT_BLESS,AREA_TARGET,
		2,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR<<1),28,2,6,&CCleric::BlessCurse,0));
	m_SpellTable.Add("Curse",new CSpell<CCleric>("Curse","Curse",CLERIC_CURSE,AFFECT_CURSE,AREA_TARGET,
		2,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR<<1),28,2,6,&CCleric::BlessCurse,0));
	m_SpellTable.Add("Arkans Battle Cry",new CSpell<CCleric>("Arkans Battle Cry","Arkans Battle Cry",CLERIC_ARKANS_BATTLE_CRY,AFFECT_ARKANS_BATTLE_CRY,AREA_ROOM,
		2,5,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR<<1),10,9,41,&CCleric::SpellAffectAdd,0));
	//blindness dice = pulses_per_mud_hour*18/1000 rolls  = 1 so max time is so calcdam max is 1.8 MUD_HOURS if 100 skill
	m_SpellTable.Add("Blindness",new CSpell<CCleric>("Blindness","Blindness",CLERIC_BLINDNESS,AFFECT_BLINDNESS,AREA_TARGET,
		2,(CMudTime::PULSES_PER_MUD_HOUR),1,(CMudTime::PULSES_PER_BATTLE_ROUND>>1),23,5,21,&CCleric::SpellAffectAdd,0));
	m_SpellTable.Add("Destroy Undead",new CSpell<CCleric>("Destroy Undead","Destroy Undead",CLERIC_DESTROY_UNDEAD,SPELL_NO_AFFECT,AREA_TARGET,
		2,7,100,200,23,5,21,&CCleric::DestroyUndead,0));
	m_SpellTable.Add("Turn Undead",new CSpell<CCleric>("Turn Undead","Turn Undead",CLERIC_TURN_UNDEAD,SPELL_NO_AFFECT,AREA_TARGET,
		2,7,100,200,25,3,11,&CCleric::TurnUndead,0));
	m_SpellTable.Add("Dispel Magic",new CSpell<CCleric>("Dispel Magic","Dispel Magic",CLERIC_DISPEL_MAGIC,SPELL_NO_AFFECT,AREA_TARGET,
		2,1,1,1,23,4,16,&CCleric::DispelMagic,0));
	m_SpellTable.Add("Vigorize Light",new CSpell<CCleric>("Vigorize Light","Vigorize Light",CLERIC_VIGORIZE_LIGHT,SPELL_NO_AFFECT,AREA_TARGET,
		3,2,4,0,27,2,6,&CCleric::MovementGainSpell,0));
	m_SpellTable.Add("Fatigue Light",new CSpell<CCleric>("Fatigue Light","Fatigue Light",CLERIC_FATIGUE_LIGHT,SPELL_NO_AFFECT,AREA_TARGET,
		3,2,4,0,27,2,6,&CCleric::MovementLossSpell,0));
	m_SpellTable.Add("Fatigue Serious",new CSpell<CCleric>("Fatigue Serious","Fatigue Serious",CLERIC_FATIGUE_SERIOUS,SPELL_NO_AFFECT,AREA_TARGET,
		4,4,5,10,25,3,11,&CCleric::MovementLossSpell,0));
	m_SpellTable.Add("Vigorize Serious",new CSpell<CCleric>("Vigorize Serious","Vigorize Serious",CLERIC_VIGORIZE_SERIOUS,SPELL_NO_AFFECT,AREA_TARGET,
		4,4,5,10,25,3,11,&CCleric::MovementGainSpell,0));
	m_SpellTable.Add("Fatigue Critical",new CSpell<CCleric>("Fatigue Critical","Fatigue Critical",CLERIC_FATIGUE_CRITICAL,SPELL_NO_AFFECT,AREA_TARGET,
		4,6,5,20,23,4,16,&CCleric::MovementLossSpell,0));
	m_SpellTable.Add("Vigorize Critical",new CSpell<CCleric>("Vigorize Critical","Vigorize Critical",CLERIC_VIGORIZE_CRITICAL,SPELL_NO_AFFECT,AREA_TARGET,
		4,6,5,20,23,4,16,&CCleric::MovementGainSpell,0));
	m_SpellTable.Add("Full Fatigue",new CSpell<CCleric>("Full Fatigue","Full Fatigue",CLERIC_FULL_FATIGUE,SPELL_NO_AFFECT,AREA_TARGET,
		6,3,25,50,15,7,31,&CCleric::MovementLossSpell,0));
	m_SpellTable.Add("Full Vigorize",new CSpell<CCleric>("Full Vigorize","Full Vigorize",CLERIC_FULL_VIGORIZE,SPELL_NO_AFFECT,AREA_TARGET,
		6,3,25,50,18,6,26,&CCleric::MovementGainSpell,0));
	m_SpellTable.Add("Mass Vigorize",new CSpell<CCleric>("Mass Vigorize","Mass Vigorize",CLERIC_MASS_VIGORIZE,SPELL_NO_AFFECT,AREA_ROOM,
		6,3,25,50,12,8,36,&CCleric::MovementGainSpell,0));
	m_SpellTable.Add("Mass Fatigue",new CSpell<CCleric>("Mass Fatigue","Mass Fatigue",CLERIC_MASS_VIGORIZE,SPELL_NO_AFFECT,AREA_ROOM,
		6,3,25,50,10,9,41,&CCleric::MovementLossSpell,0));
	m_SpellTable.Add("Spirit Armor",new CSpell<CCleric>("Spirit Armor","Spirit Armor",CLERIC_SPIRIT_ARMOR,AFFECT_SPIRIT_ARMOR,AREA_TARGET,
		3,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR<<1),28,1,1,&CCleric::SpellAffectAdd,0));
	m_SpellTable.Add("Divine Armor",new CSpell<CCleric>("Divine Armor","Divine Armor",CLERIC_DIVINE_ARMOR,AFFECT_DIVINE_ARMOR,AREA_TARGET,
		4,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR<<1),18,6,26,&CCleric::SpellAffectAdd,0));
	m_SpellTable.Add("Holy Word",new CSpell<CCleric>("Holy Word","Holy Word",CLERIC_HOLY_WORD,
		SPELL_NO_AFFECT,AREA_ROOM,3,5,100,100,10,9,41,&CCleric::HolyWord,0));
	m_SpellTable.Add("Unholy Word",new CSpell<CCleric>("Unholy Word","Unholy Word",CLERIC_UNHOLY_WORD,
		SPELL_NO_AFFECT,AREA_ROOM,3,5,100,100,10,9,41,&CCleric::HolyWord,0));
	m_SpellTable.Add("Divine Intervention",new CSpell<CCleric>("Divine Intervention","Divine Intervention",CLERIC_DIVINE_INTERVENTION,
		SPELL_NO_AFFECT,AREA_ROOM,6,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR<<1),8,10,46,&CCleric::DivineIntervention,0));
	m_SpellTable.Add("Guks Mantle",new CSpell<CCleric>("Guks Mantle","Guks Mantle",CLERIC_GUKS_MANTLE,
		SPELL_NO_AFFECT,AREA_ROOM,6,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR<<1),8,10,46,&CCleric::GuksMantleSpell,0));
	m_SpellTable.Add("Duths Warmth",new CSpell<CCleric>("Duths Warmth","Duths Warmth",CLERIC_DUTHS_WARMTH,
		SPELL_NO_AFFECT,AREA_ROOM,4,6,50,100,8,10,46,&CCleric::DuthsWarmth,0));
	m_SpellTable.Add("Word of Recall",new CSpell<CCleric>("Word of Recall","Word of Recall",CLERIC_WORD_OF_RECALL,
		SPELL_NO_AFFECT,0,1,0,0,0,10,9,41,&CCleric::WordOfRecall,0));
	m_SpellTable.Add("Continual light", new CSpell<CCleric>("Continual Light", "Continual Light", CLERIC_CONTINUAL_LIGHT,
		SPELL_NO_AFFECT,AREA_NONE,6,0,0,0,15,7,31,&CCleric::ContinualLight,0));
	m_SpellTable.Add("Resurrection", new CSpell<CCleric>("Resurrection","Resurrection",CLERIC_RESURRECTION,
		SPELL_NO_AFFECT,AREA_NONE,10,0,0,0,10,10,46,&CCleric::Resurrection,0));
	m_SpellTable.Add("Animate Dead",new CSpell<CCleric>("Animate Dead","Animate Dead",CLERIC_ANIMATE_DEAD,SPELL_NO_AFFECT,AREA_NONE,
		6,0,0,0,25,4,16,&CCleric::AnimateDead,0));


	POSITION pos = m_SpellTable.GetStartingPosition();
	bool bReturn = false;
	CSpell<CCleric> *pCurrentSpell;
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

CCleric::CCleric(CCharIntermediate *ch) : CCaster<CCleric>(ch),CCharacter(ch)
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

CCleric::CCleric(CMobPrototype &mob,CRoom *pPutInRoom)
		: CCaster<CCleric>(mob,pPutInRoom),CCharacter(mob,pPutInRoom)
{
	InitStatics();
	//everyone gets general
	m_nSkillSphere |= GENERAL;
}

CCleric::~CCleric()
{
}

/////////////////////////////
//	Sets up the skills a cleric can learn
//	
void CCleric::InitSkillInfo()
{											/*max,min,pct*/
// Base Skills
	m_SkillInfo[SKILL_TRACK]=CSkillInfo(GENERAL,50,0,20);
	m_SkillInfo[SKILL_BODYSLAM]=CSkillInfo(GENERAL,80,1,10);
	m_SkillInfo[SKILL_BANDAGE]=CSkillInfo(GENERAL,100,1,50);
	m_SkillInfo[SKILL_BASH]=CSkillInfo(GENERAL,25,0,10);
	m_SkillInfo[SKILL_DODGE]=CSkillInfo(GENERAL,100,1,10);
	m_SkillInfo[SKILL_DUAL_WIELD]=CSkillInfo(GENERAL,75,0,10);
	m_SkillInfo[SKILL_OFFENSE]=CSkillInfo(GENERAL,100,1,10);
	m_SkillInfo[SKILL_FIRST_AID]=CSkillInfo(GENERAL,100,1,10);
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
	m_SkillInfo[SKILL_BLIND_FIGHTING]=CSkillInfo(GENERAL,75,1,20);

	m_SkillInfo[SKILL_THROW_WEAPON] = CSkillInfo(GENERAL,50 ,0 ,10);
    m_SkillInfo[SKILL_PRAY] = CSkillInfo(GENERAL,100,0,50);
    m_SkillInfo[SKILL_MEDITATE] = CSkillInfo(GENERAL,100,1,50);
    m_SkillInfo[SKILL_QUICK_CHANT] = CSkillInfo(GENERAL,100,1,50);
	
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

const CSkillInfo * CCleric::GetSkillInfo(short nSkill)
{
	return &m_SkillInfo[nSkill];
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////
short CCleric::GetThaco()
{
	return m_pThaco[m_nLevel];
}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////
short CCleric::GetMaxHitPointsPerLevel()
{
	return 8;
}

//////////////////////////////////
//	HealSpell
//	this function runs for all Cleric Heal spells
//	do checks to see if we can cast in this room etc.
//	next check to see if spell is room affect
//	or target
//	modified from CCleric::Offensive by John Comes
//  TODO MOVE THIS TO CCASTER!
void CCleric::HealSpell(const CInterp * pInterp, const CSpell<CCleric> * pSpell)
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
		if(pSpell->GetSpellID()==CLERIC_FULLHEAL)
		{
			pTarget->RemoveAffect(AFFECT_BLINDNESS);
		}
	}
}

/////////////////////////////
//	Not really needed for real OS
//	but we'll be friendly to 95/98 =)
//////////////////////////////
void CCleric::CleanNewedStaticMemory()
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
short CCleric::GetManaAdjust()
{
	return m_pAttributes->GetManaAdjustWis();
}
////////////////////////////
//	Calc spell damage
//	we have not fully defined how we will calculate
//	damange for clerics yet
int CCleric::CalcSpellDam(const CSpell<CCleric> * pSpell)
{
	return pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]);
}

//////////////////////
//	Returns Exp for the level 
// passed in
//////////////////////
int CCleric::GetExp(short nLvl)
{
	return m_Experience[nLvl];
}

///////////////////////
//	Sends Spell list to char
//
void CCleric::SendSpellList()
{
	SendToChar(m_strSpellList);
}

////////////////////////
//SendPRoficiency layout
void CCleric::SendProficiency()
{
	CString strProf;
	strProf.Format("Current Proficiency Bonus: %d\r\n%s",
		m_Proficiency.CalcBonus(m_nSkillSphere),
		(const char *)m_strProficiency);
	SendCharPage(strProf);
}

const CSpell<CCleric> *CCleric::GetSpellInfo(CString strSpell)
{
	CSpell<CCleric> *pSpell;
	if(m_SpellTable.Lookup(strSpell,pSpell))
	{
		return pSpell;
	}
	return NULL;
}

//////////////////////
//	Bless and Curse Spell
void CCleric::BlessCurse(const CInterp * pInterp, const CSpell<CCleric> *pSpell)
{
	short nDam = GetLevel() / 25 + 1;
	if(pSpell->GetSpellAffect() == AFFECT_CURSE)
	{
		AddFighters(m_CurrentSpell.m_pTarget,false);
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),nDam);
	}
	else
	{
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),nDam);
	}
}

void CCleric::ContinualLight(const CInterp *pInterp, const CSpell<CCleric> *pSpell)
{
	//store command string
	CString strTmp(CurrentCommand);
	//put target in command string
	CurrentCommand = m_CurrentSpell.m_strTarget;
	bool bDummy;
	CObject *pObj = this->GetTarget(0,true,bDummy);
	if(pObj==NULL)
	{
		SendToChar("What do you wish to continual light?\r\n");
	}
	else
	{
		//add affect to object
		pObj->AddAffect(CObject::OBJ_AFFECT_CONTINUAL_LIGHT,sAffect::PERM_AFFECT,(m_Spells[CLERIC_CONTINUAL_LIGHT]>=50 ? 2 : 1));
		CString strToRoom;
		strToRoom.Format("%s glows brightly.\r\n",
			pObj->GetObjName());
		GetRoom()->SendToRoom(strToRoom);
	}
	//put command string back
	CurrentCommand = strTmp;
}

////////////////////////////////
//  SpellAffectAdd for General Spells (no reference affects)
//  Sets Timing for General Spells that add affects
//	John Comes 12-28-98
///////////////////////////////
void CCleric::SpellAffectAdd(const CInterp * pInterp, const CSpell<CCleric> * pSpell)
{
	int nDam = CalcSpellDam(pSpell);
	POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
	switch(pSpell->GetSpellID())
	{
	case CLERIC_ARKANS_BATTLE_CRY:
		CCharacter *pTarget;
		while(pos)
		{
			pTarget = m_pInRoom->GetNextCharacter(pos);
			pTarget->AddAffect(pSpell->GetSpellAffect(),nDam,m_Spells[pSpell->GetSpellID()]/20 + 1);
		}
		break;
	case CLERIC_SPIRIT_ARMOR:																	//At least -10 to AC
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),m_Spells[pSpell->GetSpellID()]/6 + 10);
		break;
	case CLERIC_DIVINE_ARMOR:																	//At least -20 to AC
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),m_Spells[pSpell->GetSpellID()]/4 + 20);
		break;
			
	default:
		//default does not use SpellMsgs
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),0);
		break;
	}
}

////////////////////////////////
//	Send back what pct for spell ups
short CCleric::GetPctSpellUp(const CSpell <CCleric> * pSpell)
{
	return m_pAttributes->GetWisdom()/5;
}


//////////////////////////////////////////////
//	Destroy Undead
//  Will do massive damage but only to undead
//	John Comes   3-8-99
//////////////////////////////////////////////
void CCleric::DestroyUndead(const CInterp * pInterp, const CSpell<CCleric> * pSpell)
{
	if(m_CurrentSpell.m_pTarget->GetRace() == RACE_UNDEAD || 
		m_CurrentSpell.m_pTarget->GetRace() == RACE_LICH)
	{
		m_CurrentSpell.m_pTarget->TakeDamage(this,CalcSpellDam(pSpell),false);
		if(m_CurrentSpell.m_pTarget->IsDead())
		{
			m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_KILLED,this,m_CurrentSpell.m_pTarget);
		}
		else
		{
			m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,m_CurrentSpell.m_pTarget);
			AddFighters(m_CurrentSpell.m_pTarget,false);
		}
	}
	else 
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,m_CurrentSpell.m_pTarget);
		if(m_CurrentSpell.m_pTarget != this)
		{
			AddFighters(m_CurrentSpell.m_pTarget,false);
		}
	}
}

//////////////////////////////////////////////
//	Turn Undead
//  Will cause undead, and only undead to flee
//	John Comes   3-8-99
//////////////////////////////////////////////
void CCleric::TurnUndead(const CInterp * pInterp, const CSpell<CCleric> * pSpell)
{
	if(m_CurrentSpell.m_pTarget->GetRace() == RACE_UNDEAD || 
		m_CurrentSpell.m_pTarget->GetRace() == RACE_LICH)
	{
		if(DIE(10) > (m_CurrentSpell.m_pTarget->GetLevel() - GetLevel())
			&& m_CurrentSpell.m_pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL))
		{
			m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,m_CurrentSpell.m_pTarget);
			AddFighters(m_CurrentSpell.m_pTarget,false);
		}
		else
		{
			m_CurrentSpell.m_pTarget->AddCommand((CString)"flee");
			m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,m_CurrentSpell.m_pTarget);
		}
	}
	else 
	{
			m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_KILLED,this,m_CurrentSpell.m_pTarget);
	}
}



//////////////////////////
//	Dispel Magic
//	If failed save or has consent all affects
//	Will be removed
//	John Comes	12-28-98
///////////////////////////
void CCleric::DispelMagic(const CInterp *pInterp, const CSpell<CCleric> *pSpell)
{
	if(m_CurrentSpell.m_pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL)
		|| m_CurrentSpell.m_pTarget->GetConsented() == this || m_CurrentSpell.m_pTarget == this) 
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,m_CurrentSpell.m_pTarget);		
		m_CurrentSpell.m_pTarget->RemoveAllAffects(true);
		return;
	}
	else
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,m_CurrentSpell.m_pTarget);	
	}
}



//////////////////////////////////
//	MovementSpell
//	This function takes care of all the gain of movement spells
//	This does all the vigorize spells
//	John Comes 3-12-99
void CCleric::MovementGainSpell(const CInterp * pInterp, const CSpell<CCleric> * pSpell)
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
void CCleric::MovementLossSpell(const CInterp * pInterp, const CSpell<CCleric> * pSpell)
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

//////////////////////////////////
//	Holy / Unholy Word
//	
//	
//	John Comes 3-13-99
void CCleric::HolyWord(const CInterp * pInterp, const CSpell<CCleric> * pSpell)
{
	short nDam = CalcSpellDam(pSpell);
	CCharacter *pTarget;
	//get first character in room
	POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
	while(pos)
	{
		pTarget = m_pInRoom->GetNextCharacter(pos);
		//check if I can hit the person based on their alignment.
		if((pSpell->GetSpellID() == CLERIC_HOLY_WORD && pTarget->GetAlignment() < 0) ||
			(pSpell->GetSpellID() == CLERIC_UNHOLY_WORD && pTarget->GetAlignment() > 0))
		{
			pTarget->TakeDamage(this,CalcSpellDam(pSpell),false);
			if(pTarget->IsDead())
			{
				m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_KILLED,this,pTarget);
				pInterp->KillChar(pTarget);
			}
			else
			{
				m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget);
				AddFighters(pTarget,false);
			}
		}
		else
		{
			m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,pTarget);
		}
		
	}
}


//////////////////////////////////
//	Divine Intervention
//	
//	John Comes 3-13-99
void CCleric::DivineIntervention(const CInterp * pInterp, const CSpell<CCleric> * pSpell)
{	/*Need to check to see if god is on!!*/
	short nDam = CalcSpellDam(pSpell);
	CCharacter *pTarget;
	//get first character in room
	POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
	while(pos)
	{
		pTarget = m_pInRoom->GetNextCharacter(pos);
		pTarget->AddAffect(CCharacter::AFFECT_INVISIBILITY,nDam,0);		
		pTarget->AddAffect(CCharacter::AFFECT_DI,nDam,0);
		pTarget->AddAffect(CCharacter::AFFECT_FAERIE_FLIGHT,nDam,0);
	}
}

///////////////////////
//	Guks Mantle
//	Calls GuksMantle in CGroup
//	John Comes 3-14-99
////////////////////////////////
void CCleric::GuksMantleSpell(const CInterp *pInterp, const CSpell<CCleric> *pSpell)
{
	m_Group.GuksMantle(this,CalcSpellDam(pSpell),20);
}

///////////////////////
//	Duth's Warmth
//	Calls DuthsWarmth in CGroup
//	John Comes 3-15-99
////////////////////////////////
void CCleric::DuthsWarmth(const CInterp *pInterp, const CSpell<CCleric> *pSpell)
{
	m_Group.DuthsWarmth(this,-CalcSpellDam(pSpell));
}


/////////////////////////
//	Word of Recall
//
//	Recalls caster to last rent room
//	John Comes 4-2-99
/////////////////////////
void CCleric::WordOfRecall(const CInterp *pInterp,const CSpell<CCleric> *pSpell)
{
	m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,this); 
	//change the room the caster is in
	GetRoom()->Remove(this);
	CRoom *pRealRoom = CWorld::FindRoom(GetLastRentRoom());
	this->m_pInRoom = pRealRoom;
	m_pInRoom->Add(this);
	m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_KILLED,this,this);
	//SKILL_KILLED is the message for the completion of this spell
	CString strToChar;
	SendToChar(GetRoom()->DoRoomLook(this));
	LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * 2);
}

///////////////////////////
//	Ressurection
//	bring them back from the dead!
void CCleric::Resurrection(const CInterp *pInterp, const CSpell<CCleric> *pSpell)
{
	CString strHold(CurrentCommand);
	CurrentCommand = m_CurrentSpell.m_strTarget;
	bool bAll;
	CObject *pObj = GetTarget(0,false,bAll);
	CurrentCommand = strHold;

	if(pObj==NULL)
	{
		SendToChar("Ressurect what?\r\n");
	}
	else if(!pObj->IsType(ITEM_TYPE_CORPSE))
	{
		SendToChar("You can only resurrect corpses!\r\n");
	}
	else
	{
		if(((CCorpse *)pObj)->WasPc())
		{
			CCharacter *pCh = GVM.FindCharacter(pObj->GetObjName(),this);
			if(pCh==this)
			{
				SendToChar("hrm can't resurrect yourself!\r\n");
			}
			else if(pCh==NULL)
			{
				SendToChar("That Character must be logged on!\r\n");
			}
			else if(pCh->GetConsented()!=this)
			{
				SendToChar("You must have consent first.\r\n");
			}
			else
			{
				//fail 3 times and you fail
				if((DIE(100)>m_Spells[CLERIC_RESURRECTION] &&
					DIE(100)>m_Spells[CLERIC_RESURRECTION] &&
					DIE(100)>m_Spells[CLERIC_RESURRECTION]))
				{
					m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,pCh);
				}
				else
				{
					//sends message to room that target is in
					m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_KILLED,pCh,NULL);
					m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pCh);
					//remomove all affects and eq
					pCh->RemoveAllAffects(false);
					pCh->PutAllEqInRoom();
					//remove them from room they are in
					pCh->GetRoom()->Remove(pCh);
					//change current room
					pCh->m_pInRoom = GetRoom();
					//put in new room
					pCh->GetRoom()->Add(pCh);
					pCh->m_nCurrentHpts = 1;
					pCh->m_nManaPts = 1;
					pCh->m_nMovePts = 1;
					//get % exp they get back with a possible 5% bonus from luck
					int nAdj = m_Spells[CLERIC_RESURRECTION]/5<1 ? 1 : m_Spells[CLERIC_RESURRECTION]/5;
					if(pCh->StatCheck(LUCK))
					{
						nAdj+=5;
					}
					nAdj = nAdj>EXP_LOST_IN_DEATH ? EXP_LOST_IN_DEATH : nAdj;
					pCh->m_nExperience += ((pCh->GetExp(pCh->GetLevel()) - pCh->GetExp((pCh->GetLevel())-1)) * (nAdj))/100;
					if(pCh->EnoughExpForNextLevel())
					{
						pCh->AdvanceLevel(true,true);
					}
					((CCorpse *)pObj)->Loot(pCh,true); //true means it will force add all to inventory and not send any messages
					//this also save char
				}
				//set corpse for removal
				pObj->SetDelete();
			}
		}
		else
		{
			SendToChar("You can only resurrect PC's!\r\n");
		}
	}
}

////////////////////////
//	Animate Dead
//	Animates corpse to a skeleton or zombie; copied from CMage::AnimateDead
//	John Comes: 12-29-98
//////////////////////////
void CCleric::AnimateDead(const CInterp *pInterp, const CSpell<CCleric> *pSpell)
{
	int nDot,nNum = 1;
	CString strTarget(m_CurrentSpell.m_strTarget);
	if((nDot = strTarget.Find('.'))!=-1)
	{
		nNum = strTarget.Left(nDot).MakeInt();
		if(nNum==-1)
		{
			SendToChar("Animate how many?\r\n");
			return;
		}
		strTarget = strTarget.Right(nDot);
	}

	if(strTarget.IsEmpty())
	{
		SendToChar("Animate What??\r\n");
	}
	else
	{
		CCorpse *pCon;
		if((pCon = (CCorpse *)m_pInRoom->FindObjectInRoom(strTarget,nNum,this))
			&& pCon->IsType(ITEM_TYPE_CORPSE))
		{
			//calculate level limit 
			int nLevelLimit = m_Spells[pSpell->GetSpellID()]/10;
			nLevelLimit = nLevelLimit<3 ? 3 : nLevelLimit;
			if(pSpell->GetMinLevel() - pCon->GetCorpseLevel() > nLevelLimit)
			{
				SendToChar("The Corpse is not powerful enough for you to raise.\r\n");
			}
			else
			{
				sUndeadInfo UndeadInfo;
				UndeadInfo.m_nLevel = pCon->GetCorpseLevel();
				UndeadInfo.m_nUndeadType = pSpell->GetUndeadType();;
				//If this is Animate dead type will be 0, so make a random Skeleton or Zombie
				if(UndeadInfo.m_nUndeadType==0)
				{
					if(DIE(100) < m_Spells[pSpell->GetSpellID()])
					{
						UndeadInfo.m_nUndeadType = VNUM_UNDEAD_ZOMBIE;
					}
					else
					{
						UndeadInfo.m_nUndeadType = VNUM_UNDEAD_SKELETON;
					}
				}
				
				CCharacter *pMob = pInterp->CreateUndead(&UndeadInfo,GetRoom());
				if(pMob!=NULL)
				{
					GetRoom()->SendToRoom("&L%s&L breathes &rlife&L in to a corpse...&n\r\n",this);
					pMob->AddCommand((CString) "emote looks to the sky and says, 'I'm alive!'\r\n");
					if(this->CurrentlyMasterOf()<((GetLevel()/20)+1))
					{
						pMob->SetMaster(this,m_Spells[pSpell->GetSpellID()]*CMudTime::PULSES_PER_MUD_HOUR);
					}
					else
					{
						CString str;
						str.Format("&r%s resists your magical hold.\r\nYou do not control %s.&n\r\n",
							(char *)pMob->GetName(),(char *)pMob->GetName());
						SendToChar(str);
					}
					CObjectManager::Remove(pCon);
				}
				else
				{
					SendToChar("You should never see this bug in create undead!\r\n");
					AddCommand("bug Bad Undead Type in Cleric class.");
				}
			}
		}
		else if(!pCon)
		{
			SendToChar("Animate What??\r\n");
		}
		else
		{
			SendToChar("You can only animate corpses.\r\n");
		}
	}
}

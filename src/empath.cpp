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
// Empath.cpp: implementation of the CEmpath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "empath.h"
//////////////////////////////
//	Statics
////////////////////////////

extern CRandom GenNum;

extern CGlobalVariableManagement GVM;

CSkillInfo CEmpath::m_SkillInfo[MAX_MUD_SKILLS];
bool CEmpath::StaticsInitialized = false;
CString CEmpath::m_strSpellList;
const UINT CEmpath::GENERAL = (1<<0);
CProficiency CEmpath::m_Proficiency;
CString CEmpath::m_strProficiency("You class has the following proficiencies:\r\n");

const short int CEmpath::m_pThaco[] = 
{
	/*1,02,03,04,05,06,07,08,09,10*/
	100,98,97,95,94,92,91,89,88,86,
	 85,83,82,80,79,77,76,75,73,71,
	 70,68,67,65,64,62,61,59,58,56,
	 55,53,51,49,48,46,45,43,42,40,
	 39,37,36,35,33,32,30,29,27,26,
	 25,23,22,20,19,17,16,14,13,11 /*51-60*/
};

const int CEmpath::m_Experience[LVL_IMP+1] = 
{
	/*0*/	0,		/*1*/	1,		/*2*/   300,	/*3*/   900,
	/*4*/	1800,	/*5*/	3000,	/*6*/   4500,	/*7*/   6300,
	/*8*/	8400,	/*9*/	10800,	/*10*/  13500,	/*11*/  16500,
	/*12*/  23100,	/*13*/  30300,	/*14*/  38100,	/*15*/  46500,
	/*16*/  55500,	/*17*/  65100,	/*18*/  75300,	/*19*/  86100,
	/*20*/  97500,	/*21*/  109500,	/*22*/  134700,	/*23*/  161100,
	/*24*/  188700,	/*25*/  217500,	/*26*/  247500,	/*27*/  278700,
	/*28*/  311100,	/*29*/  344700,	/*30*/  379500,	/*31*/  451500,
	/*32*/  525900,	/*33*/  602700,	/*34*/  681900,	/*35*/  763500,
	/*36*/  847500,	/*37*/  933900,	/*38*/  1022700,/*39*/  1113900,
	/*40*/  1207500,/*41*/  1399500,/*42*/  1596300,/*43*/  1797900,
	/*44*/  2004300,/*45*/  2215500,/*46*/  2431500,/*47*/  2652300,
	/*48*/  2877900,/*49*/  3108300,/*50*/  3343500,/*51*/  3583500,
	/*52*/  4073100,/*53*/  4572300,/*54*/  5081100,/*55*/  5599500,
	/*56*/  6127500,/*57*/  6665100,/*58*/  7212300,/*59*/  7769100,
	/*60*/  8335500

};

CMap<CString,const CSpell<CEmpath> *> CEmpath::m_SpellTable((26*MIN_SPELL_LETTERS),MIN_SPELL_LETTERS);

////////////////////////////
//Spell id's for Empaths
//	How spell id's work...each class defines a spell id for each spell
//	this number is then used to reference the ability to cast that spell
//	in m_Spells[].
#define EMPATH_SPIRIT_CRUSH 0
#define EMPATH_EGO_WHIP 1
#define EMPATH_BATTLE_READY 2
#define EMPATH_FEEL_PAIN 3
#define EMPATH_FEEL_WELL 4
#define EMPATH_INSTILL_STRENGTH 5
#define EMPATH_INSTILL_DEXTERITY 6
#define EMPATH_INSTILL_AGILITY 7
#define EMPATH_SENSE_PRESENCE 8
#define EMPATH_BODY_RISE 9



//////////////////////////////

///////////////////////////////////////
//	InitSpellTable
//	Initalizes the spells and the things
//	this type of caster can cast and minor
//	create.
//	written by: Demetrius Comes
////////////////////////////////////
void CEmpath::InitStatics()
{
	if(StaticsInitialized)
	{
		return;
	}
	StaticsInitialized = true;
	InitSkillInfo();
//Spells
	m_SpellTable.Add("Spirit Crush",new CSpell<CEmpath>("Spirit Crush","&BSpirit Crush&n",EMPATH_SPIRIT_CRUSH,SPELL_NO_AFFECT,AREA_TARGET| SPELL_OFFENSIVE,
		4,5,100,100,0,7,31,&CEmpath::OffensiveSpell,0));
	m_SpellTable.Add("Ego Whip",new CSpell<CEmpath>("Ego Whip","&BEgo Whip&n",EMPATH_EGO_WHIP,SPELL_NO_AFFECT,AREA_TARGET| SPELL_OFFENSIVE,
		3,5,20,50,0,5,21,&CEmpath::OffensiveSpell,0));
	m_SpellTable.Add("Battle Ready",new CSpell<CEmpath>("Battle Ready","&BBattle Ready&n",EMPATH_BATTLE_READY,CCharacter::AFFECT_BATTLE_READY,AREA_TARGET,
		3,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),0,1,1,&CEmpath::SpellAffectAdd,0));
	m_SpellTable.Add("Feel Pain",new CSpell<CEmpath>("Feel Pain","&BFeel Pain&n",EMPATH_FEEL_PAIN,SPELL_NO_AFFECT,AREA_TARGET| SPELL_OFFENSIVE,
		3,1,8,4,0,1,1,&CEmpath::OffensiveSpell,0));
	m_SpellTable.Add("Feel Well",new CSpell<CEmpath>("Feel Well","&BFeel Well&n",EMPATH_FEEL_WELL,SPELL_NO_AFFECT,AREA_TARGET,
		3,1,8,4,0,1,1,&CEmpath::HealSpell,0));
	m_SpellTable.Add("Instill Strength",new CSpell<CEmpath>("Instill Strength","&BInstill Strength&n",EMPATH_INSTILL_STRENGTH,CCharacter::AFFECT_STRENGTH,AREA_TARGET,
		5,CMudTime::PULSES_PER_MUD_HOUR,30,(CMudTime::PULSES_PER_MUD_HOUR>>1),0,2,6,&CEmpath::SpellAffectAdd,0));
	m_SpellTable.Add("Instill Dexterity",new CSpell<CEmpath>("Instill Dexterity","&BInstill Dexterity&n",EMPATH_INSTILL_DEXTERITY,CCharacter::AFFECT_DEXTERITY,AREA_TARGET,
		5,CMudTime::PULSES_PER_MUD_HOUR,30,(CMudTime::PULSES_PER_MUD_HOUR>>1),0,2,6,&CEmpath::SpellAffectAdd,0));
	m_SpellTable.Add("Instill Agility",new CSpell<CEmpath>("Instill Agility","&BInstill Agility&n",EMPATH_INSTILL_AGILITY,CCharacter::AFFECT_AGILITY,AREA_TARGET,
		5,CMudTime::PULSES_PER_MUD_HOUR,30,(CMudTime::PULSES_PER_MUD_HOUR>>1),0,2,6,&CEmpath::SpellAffectAdd,0));
	m_SpellTable.Add("Sense Presence",new CSpell<CEmpath>("Sense Presence","&BSense Presence&n",EMPATH_SENSE_PRESENCE,CCharacter::AFFECT_DI,AREA_TARGET,
		5,CMudTime::PULSES_PER_MUD_HOUR,30,(CMudTime::PULSES_PER_MUD_HOUR>>1),0,9,41,&CEmpath::SpellAffectAdd,0));
	m_SpellTable.Add("Body Rise",new CSpell<CEmpath>("Body Rise","&BBody Rise&n",EMPATH_BODY_RISE,CCharacter::AFFECT_FLIGHT,AREA_TARGET,
		5,CMudTime::PULSES_PER_MUD_HOUR,30,(CMudTime::PULSES_PER_MUD_HOUR>>1),0,9,41,&CEmpath::SpellAffectAdd,0));

	POSITION pos = m_SpellTable.GetStartingPosition();
	bool bReturn = false;
	const CSpell<CEmpath> *pCurrentSpell;
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

CEmpath::CEmpath(CCharIntermediate *ch) : CCharacter(ch)
{
	InitStatics();
	//can't put in CCharacter construct because
	//it will be called before us
	if(IsNewbie())
	{
		InitSkills();
		m_nSkillSphere |= GENERAL;
	}
	m_bHasSpelledUp=false;
}

CEmpath::CEmpath(CMobPrototype &mob,CRoom *pPutInRoom)
		: CCharacter(mob,pPutInRoom)
{
	InitStatics();
	//everyone gets general
	m_nSkillSphere |= GENERAL;
	m_bHasSpelledUp=false;
}

CEmpath::~CEmpath()
{
}

/////////////////////////////
//	Sets up the skills a warrior can learn
//	
void CEmpath::InitSkillInfo()
{											/*max,min,pct*/
// Base Skills
	m_SkillInfo[SKILL_TRACK]=CSkillInfo(GENERAL,50,0,20);
	m_SkillInfo[SKILL_BODYSLAM]=CSkillInfo(GENERAL,50,1,10);
	m_SkillInfo[SKILL_BANDAGE]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_BASH]=CSkillInfo(GENERAL,25,0,10);
	m_SkillInfo[SKILL_DODGE]=CSkillInfo(GENERAL,100,1,10);
	m_SkillInfo[SKILL_DUAL_WIELD]=CSkillInfo(GENERAL,75,0,10);
	m_SkillInfo[SKILL_OFFENSE]=CSkillInfo(GENERAL,100,1,10);
	m_SkillInfo[SKILL_FIRST_AID]=CSkillInfo(GENERAL,100,1,10);
	m_SkillInfo[SKILL_MOUNT]=CSkillInfo(GENERAL,100,1,10);
	m_SkillInfo[SKILL_BACKSTAB]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_TUNDRA_MAGIC]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_DESERT_MAGIC]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_MOUNTAIN_MAGIC]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_FOREST_MAGIC]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_SWAMP_MAGIC]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_FORAGE]=CSkillInfo(GENERAL,100,1,100);
	m_SkillInfo[SKILL_RESCUE]=CSkillInfo(GENERAL,75,1,10);
	m_SkillInfo[SKILL_SWITCH]=CSkillInfo(GENERAL,100,1,10);
	m_SkillInfo[SKILL_TRIP]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_KICK]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_CLERIC_MAGIC]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_SHAMAN_MAGIC]=CSkillInfo(GENERAL,75,0,10);
	m_SkillInfo[SKILL_BLIND_FIGHTING]=CSkillInfo(GENERAL,50,1,10);
    
	m_SkillInfo[SKILL_THROW_WEAPON]=CSkillInfo(GENERAL,50,0,10);
	m_SkillInfo[SKILL_MEDITATE]=CSkillInfo(GENERAL,100,1,50);
	m_SkillInfo[SKILL_QUICK_CHANT]=CSkillInfo(GENERAL,100,1,50);

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

const CSkillInfo* CEmpath::GetSkillInfo(short nSkill)
{
	return &m_SkillInfo[nSkill];
}

//////////////////////////////////
//	gives back the pct for a spell up
short CEmpath::GetPctSpellUp(const CSpell <CEmpath> * pSpell)
{
	return m_pAttributes->GetStat(POW)/5;
}

void CEmpath::SpellUp(const CSpell<CEmpath> *pSpell)
{
	if((!m_bHasSpelledUp) 
		&& m_Spells[pSpell->GetSpellID()]<=MAX_SKILL_LEVEL
		&& (this->GetPctSpellUp(pSpell) >= DIE(101)) 
		&& IsAffectedBy(CCharacter::AFFECT_CASTING))
	{
		m_bHasSpelledUp = true;
		m_Spells[pSpell->GetSpellID()]+=DIE(this->GetLearingBonus());
		CString strToChar;
		strToChar.Format("&cYou feel more comfortable casting %s&n\r\n",
			(char *)pSpell->GetColorizedName());
		SendToChar(strToChar);
	}
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////
short CEmpath::GetThaco()
{
	return m_pThaco[m_nLevel];
}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////
short CEmpath::GetMaxHitPointsPerLevel()
{
	return 4;
}

/////////////////////////////////////////
//	DoThink
//	Find tick ' marks
//	Get spell word and see if in hash table
//	if not return 
//	if it is..dereference function pointer in
//	CSpell class and run function assocated with
//	this spell
//	written by: Demetrius Comes
////////////////////////////////////
void CEmpath::DoWill(const CInterp *pInterp)
{
	CString CastMsg;
	static const char *pTick = "'";
	//if they aren't casting check all information and insure
	//they can cast the spell
	if(!IsAffectedBy(CCharacter::AFFECT_CASTING))
	{
		int first = CurrentCommand.Find(pTick[0]);
		int second = CurrentCommand.Find(pTick[0],2);

		if(first == -1 || second == -1)
		{
			SendToChar("Spells must be inclosed in single quotes!\r\n");
			return;
		}

		CString strSpell(CurrentCommand(first+1,second-1));

		const CSpell<CEmpath> *pSpell = NULL;

		if(!m_SpellTable.Lookup(strSpell,pSpell))
		{
			SendToChar("Think what?\r\n");
			return;
		}
		//take do they have enough to cast the spell?
		if(m_nManaPts-(pSpell->GetMana()) < 0)
		{
			SendToChar("You need more mana!\r\n");
			return;
		}
		if(!m_Spells[pSpell->GetSpellID()] || pSpell->GetMinLevel()>this->GetLevel())
		{
			SendToChar("You do not have knowledge of that spell.\r\n");
			return;
		}
		CString strTarget(CurrentCommand.Right(second+1));
		strTarget.SkipSpaces();
		CCharacter *pTarget=NULL;

		if((pSpell->GetAffectedArea() & AREA_TARGET))
		{
			CString strTmp(CurrentCommand);
			CurrentCommand = strTarget;
			pTarget = GetTarget(0,true);
			CurrentCommand=strTmp;
			if(!pTarget)
			{
				SendToChar("At Who?\r\n");
				return;
			}
		}
		m_bHasSpelledUp = false;
		if((pSpell->GetAffectedArea() & AREA_SELF)) pTarget = this;
		//now we know we can cast it so set casting bit
		//and set up all information in current spell struct
		m_lLagTime = pSpell->GetLagTime();
		AddAffect(CCharacter::AFFECT_CASTING,sAffect::MANUAL_AFFECT);
		m_CurrentSpell.m_pSpell = pSpell;
		m_CurrentSpell.m_strTarget = strTarget;
		m_CurrentSpell.m_pTarget = pTarget;
		if(m_CurrentSpell.m_pSpell->IsOffensive())
		{
			if(IsAffectedBy(CCharacter::AFFECT_INVISIBLE))
			{
				RemoveAffect(CCharacter::AFFECT_INVISIBLE);
			}
			if((m_CurrentSpell.m_pSpell->GetAffectedArea() & AREA_TARGET) 
				&& m_CurrentSpell.m_pTarget!=NULL)
			{
				if(this->m_Master.IsMasterOf(m_CurrentSpell.m_pTarget))
				{
					this->m_Master.Remove(m_CurrentSpell.m_pTarget);
				}
				if(this->m_Follow.HasFollower(m_CurrentSpell.m_pTarget))
				{
					this->m_Follow.Remove(m_CurrentSpell.m_pTarget);
				}
			}
		}
		CastMsg = "&WYou start to manipulate the area with your will....&n\r\n";
		SendToChar(CastMsg);
	}
	//they already were casting so we know casting time is up
	//other wise we wouldn't be in here.
	else
	{
		//remove casting 
		RemoveAffect(CCharacter::AFFECT_CASTING);
		//cast spell
		if(this->CastingTargetInRoom())
		{
			SendToChar("&WYour wishes form into reality with your last push of will power...&n\r\n");
			m_CurrentSpell.m_pSpell->Cast(pInterp,this);
			m_nManaPts-=m_CurrentSpell.m_pSpell->GetMana();
		}
		else
		{
			SendToChar("At who?\r\n");
		}
		m_CurrentSpell.Empty();
		
	}
}

//////////////////////////////////
//	OffensiveSpell
//	this function runs for all Empath offensive spells
//	do checks to see if we can cast in this room etc.
//	next check to see if spell is room affect
//	or target
//	written by: Demetrius Comes
void CEmpath::OffensiveSpell(const CInterp * pInterp, const CSpell<CEmpath> * pSpell)
{
	if(m_pInRoom->IsAffectedBy(CRoom::ROOM_AFFECT_PEACEFUL))
	{
		SendToChar("This place is far to peacefull.\r\n");
		return;
	}
	CCharacter *pTarget;
	if(pSpell->GetAffectedArea() & AREA_ROOM)
	{
		//get first character in room
		POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
		while(pos)
		{
			pTarget = m_pInRoom->GetNextCharacter(pos);
			//make sure it's not me
			//later will need to protect group as well
			if(pTarget!=this && !m_Group.IsInGroup(pTarget))
			{
				pTarget->ThinkerDefense(CalcSpellDam(pSpell),this);
				if(pTarget->IsDead())
				{
					m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_KILLED,this,pTarget);
					pInterp->KillChar(pTarget);
				}
				else
				{
					m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget);
					//if target isn't make it fight
					AddFighters(pTarget,false);
				}
			}
		}
	}
	//we are casting at self or someone else not a room
	else
	{
		pTarget = m_CurrentSpell.m_pTarget;
		if(!CastingTargetInRoom())
		{
			SendToChar("Who?\r\n");
		}
		else
		{
			pTarget->ThinkerDefense(CalcSpellDam(pSpell),this);
			if(pTarget->IsDead())
			{
				m_MsgManager.SpellMsg(m_CurrentSpell.m_pSpell->GetSpellName(),SKILL_KILLED,this,pTarget);
				pInterp->KillChar(pTarget);
			}
			else
			{
				m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget);
				AddFighters(pTarget,false);
			}
		}
	}
}

///////////////////////
//	Take our Target and see if it's 
//	still in the room
bool CEmpath::CastingTargetInRoom()
{
	if(m_CurrentSpell.m_pTarget)
	{
		return (m_CurrentSpell.m_pTarget->IsDead() ? false : this->IsInRoom(m_CurrentSpell.m_pTarget));
	}
	else if(m_CurrentSpell.m_pSpell->GetAffectedArea() & AREA_ROOM ||
		m_CurrentSpell.m_pSpell->GetAffectedArea() == 0)
	{
		return true;
	}
	ErrorLog << "casting bit set when it shouldn't be!\r\n" << endl;
	return false;
}

///////////////////////////////
//	Overridden HasCommand from CCharacter class
//////////////////////////////
bool CEmpath::HasCommand()
{
	CString strToChar;
	if(IsAffectedBy(CCharacter::AFFECT_CASTING))
	{
		if(!this->CastingTargetInRoom())
		{
			//don't return here so we get the next command
			RemoveAffect(CCharacter::AFFECT_CASTING);
			SendToChar("&WYou Target seems to have left the room.&n\r\n&RYou stop your spell!&n\r\n");
			GetRoom()->SendToRoom("&R%s's face turns pale!&n\r\n",this);
			m_lLagTime=0;
		}
		else if(!InPosition(POS_STANDING))
		{
			SendToChar("&WYou can't do this while not standing!&n\r\n&RYou stop your spell!&n\r\n");
			GetRoom()->SendToRoom("&R%s's face turns pale!&n\r\n",this);
			RemoveAffect(CCharacter::AFFECT_CASTING);
		}
		else if((m_lLagTime-1) && !((m_lLagTime-1)%(CMudTime::PULSES_PER_CASTING_ROUND * 2)))
		{
			SpellUp(m_CurrentSpell.m_pSpell);
			if(this->LostConcentration())
			{
				RemoveAffect(CCharacter::AFFECT_CASTING);
				SendToChar("You lose your concentration.\r\n");
				m_pInRoom->SendToRoom("%s face blushes.\r\n",this);
				m_lLagTime=0;
			}	
		}
	}
	//run base has command
	return CCharacter::HasCommand();
}

////////////////////////////////////
//	DoPractice()
//	Check to see if we are practicing a spell
//	if not call CCharacters dopractice to check
//	for a skill!
//	written by: Demetrius comes
//	7/31/98
//////////////////////////////////
void CEmpath::DoPractice(CString strToPractice, const CCharacter *pTeacher)
{
	const CSpell<CEmpath> *pSpell;
	CString strToChar;
	if(m_SpellTable.Lookup(strToPractice,pSpell))
	{
		skill_type nPctLearn;
		if(this->GetLevel()<pSpell->GetMinLevel())
		{
			SendToChar("You need more levels first!\r\n");			
		}
		else if((nPctLearn = pTeacher->CanTeach(this,pSpell->GetSpellID(),false))>0)
		{
			if(m_Spells[pSpell->GetSpellID()]==0)
			{
				sMoney Cost;
				if(CanAffordTraining(Cost,pSpell->GetSpellID(),pTeacher,false))
				{
					strToChar.Format("You learn the spell %s.\r\n",(char *)pSpell->GetColorizedName());
					m_Spells[pSpell->GetSpellID()] = (skill_type) DIE(nPctLearn);
					m_nSpellSphere |= pSpell->GetSphere();
					
					if(!Cost.IsEmpty())
					{
						strToChar.Format("%s%s\r\n",
							(char *) strToChar,
							(char *) Cost.GetMoneyString());
					}
					SendToChar(strToChar);
				}
				else
				{
					SendToChar("You can't seem to afford the training.\r\n");
				}
			}
			else
			{
				SendToChar("You already know that spell.\r\n");
			}
		}
	}
	else
	{
		CCharacter::DoPractice(strToPractice,pTeacher);
	}
}

//////////////////////////////////
//	Spells
//	Send character the spells they know
//	as well as how good they are at that spell
/////////////////////////////////
void CEmpath::DoSpells()
{
	const CSpell<CEmpath> *pSpell;
	POSITION pos = m_SpellTable.GetStartingPosition();
	CString strToChar;

	strToChar+="Spells Known:\r\n";
	while(pos)
	{
		pSpell = m_SpellTable.GetNext(pos);
		if(m_Spells[pSpell->GetSpellID()])
		{
			strToChar.Format("%s%-30s\t%s\r\n",(char *) strToChar,
				(char *)pSpell->GetColorizedName(),this->SkillWording(m_Spells[pSpell->GetSpellID()]));
		}
	}
	SendToChar(strToChar);
}

/////////////////////////////
//	Not really needed for real OS
//	but we'll be friendly to 95/98 =)
//////////////////////////////
void CEmpath::CleanNewedStaticMemory()
{
	POSITION pos = m_SpellTable.GetStartingPosition();
	while(pos)
	{
		delete m_SpellTable.GetNext(pos);
	}
}

///////////////////////////
//	GetManaAdjust overridden
//	returns POW adjustment
short CEmpath::GetManaAdjust()
{
	return m_pAttributes->GetManaAdjustPow();
}

////////////////////////////
//	Calc spell damage
//	we have not fully defined how we will calculate
//	damange for psi's yet
int CEmpath::CalcSpellDam(const CSpell<CEmpath> * pSpell)
{
	return pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]);
}

///////////////////////////
//	LostConcentration()
//	Checks to see if we lose concentration
//	on the spell we are casting.
bool CEmpath::LostConcentration()
{
	if(m_nLevel < (m_CurrentSpell.m_pSpell->GetMinLevel()+5))
	{
		//Do int check and see if concen was lost.
		return !StatCheck(WIS);
	}
	return false;
}

///////////////////////////
//	returns exp of level passed in
///////////////////////////
int CEmpath::GetExp(short nLvl)
{
	return m_Experience[nLvl];
}



//////////////////////////
//	SendSpellList to char
void CEmpath::SendSpellList()
{
	SendToChar(m_strSpellList);
}

////////////////////////
//SendPRoficiency layout
void CEmpath::SendProficiency()
{
	CString strProf;
	strProf.Format("Current Proficiency Bonus: %d\r\n%s",
		m_Proficiency.CalcBonus(m_nSkillSphere),
		(const char *)m_strProficiency);
	SendCharPage(strProf);
}

//////////////////////////////////
//	HealSpell
//	this function runs for all Empath Heal spells
//	do checks to see if we can cast in this room etc.
//	next check to see if spell is room affect
//	or target
//	Copied from CCleric::HealSpell by John Comes 3-15-99
void CEmpath::HealSpell(const CInterp * pInterp, const CSpell<CEmpath> * pSpell)
{
	CCharacter *pTarget;
	if(pSpell->GetAffectedArea() & AREA_ROOM)
	{
		//get first character in room
		POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
		while(pos)
		{
			pTarget = m_pInRoom->GetNextCharacter(pos);
			//make sure it's not me
			//later will need to protect group as well
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

////////////////////////////////
//  SpellAffectAdd for General Spells (no reference affects)
//  Sets Timing for General Spells that add affects
//	John Comes 3-15-99
///////////////////////////////
void CEmpath::SpellAffectAdd(const CInterp * pInterp, const CSpell<CEmpath> * pSpell)
{
	switch(pSpell->GetSpellID())
	{
	case EMPATH_BATTLE_READY:
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),1);
		break;
	case EMPATH_INSTILL_DEXTERITY: case EMPATH_INSTILL_STRENGTH:
	case EMPATH_INSTILL_AGILITY:
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),m_Spells[pSpell->GetSpellID()]/20+1);
		break;
	default:
		//default does not use SpellMsgs
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),0);
		break;
	}
}

///////////////////////////////
//	Over ridden for casters
//	Allows a god to the effecticy of a spell
///////////////////////////////
bool CEmpath::SetSpell(const CCharacter *pCh,CString strSpell, skill_type nValue)
{
	const CSpell<CEmpath> *pSpell;
	if(m_SpellTable.Lookup(strSpell,pSpell))
	{
		m_Spells[pSpell->GetSpellID()] = nValue>MAX_SKILL_LEVEL ? MAX_SKILL_LEVEL : nValue;
		CString strToChar;
		strToChar.Format("%s makes you feel more comfortable casting %s.\r\n",
			(char *)pCh->GetName(),(char *)pSpell->GetColorizedName());
		SendToChar(strToChar);
		strToChar.Format("You increase %s's skill in casting %s to %d.\r\n",
			(char *)GetName(),
			(char *)pSpell->GetColorizedName(),
			nValue);
		pCh->SendToChar(strToChar);
		return true;
	}
	return false;
}

void CEmpath::BuildPracticePriceSpells(const CCharacter *pToBeTaught,CString &str) const
{
	if(this->IsTeacher())
	{
		sMoney Cost;
		str.Format("%sI can teach you the following spells:\r\n",(char *)str);
		POSITION pos = m_SpellTable.GetStartingPosition();
		const CSpell<CEmpath> *pSpell;
		while(pos)
		{
			pSpell = m_SpellTable.GetNext(pos);
			if(this->IsGoodEnoughToTeach(pToBeTaught,pSpell->GetSpellID(),false))
			{
				Cost = GetTrainingCosts(pToBeTaught,pSpell->GetSpellID(),false);
				str.Format("%s%-30s\t%s\r\n",
					(char *)str,
					(char *)pSpell->GetSpellName(),
					(char *)Cost.GetMoneyString());
			}
		}
	}
	else
	{
		str = "My skills are to value-able for me to teach anyone.\r\n";
	}
}

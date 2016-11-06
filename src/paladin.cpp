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
// Paladin.cpp: implementation of the CPaladin class.
//
//////////////////////////////////////////////////////////////////////
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "paladin.h"
//////////////////////////////
//	Statics
////////////////////////////

#define MAX_RELOCATE_LAG 6

extern CGlobalVariableManagement GVM;

bool CPaladin::StaticsInitialized = false;
CSkillInfo CPaladin::m_SkillInfo[MAX_MUD_SKILLS];
CString CPaladin::m_strSpellList;
const UINT CPaladin::GENERAL = (1 << 0);
CProficiency CPaladin::m_Proficiency;
CString CPaladin::m_strProficiency("You class has the following proficiencies:\r\n");
short CPaladin::LVLS_PER_2H_WIELD_BONUS = 12;

const int CPaladin::m_Experience[LVL_IMP + 1] ={
   /*0*/ 0, /*1*/ 1, /*2*/ 200, /*3*/ 600,
   /*4*/ 1200, /*5*/ 2000, /*6*/ 3000, /*7*/ 4200,
   /*8*/ 5600, /*9*/ 7200, /*10*/ 9000, /*11*/ 11000,
   /*12*/ 15400, /*13*/ 20200, /*14*/ 25400, /*15*/ 31000,
   /*16*/ 37000, /*17*/ 43400, /*18*/ 50200, /*19*/ 57400,
   /*20*/ 65000, /*21*/ 73000, /*22*/ 89800, /*23*/ 107400,
   /*24*/ 125800, /*25*/ 145000, /*26*/ 165000, /*27*/ 185800,
   /*28*/ 207400, /*29*/ 229800, /*30*/ 253000, /*31*/ 301000,
   /*32*/ 350600, /*33*/ 401800, /*34*/ 454600, /*35*/ 509000,
   /*36*/ 565000, /*37*/ 622600, /*38*/ 681800, /*39*/ 742600,
   /*40*/ 805000, /*41*/ 933000, /*42*/ 1064200, /*43*/ 1198600,
   /*44*/ 1336200, /*45*/ 1477000, /*46*/ 1621000, /*47*/ 1768200,
   /*48*/ 1918600, /*49*/ 2072200, /*50*/ 2229000, /*51*/ 2389000,
   /*52*/ 2715400, /*53*/ 3048200, /*54*/ 3387400, /*55*/ 3733000,
   /*56*/ 4085000, /*57*/ 4443400, /*58*/ 4808200, /*59*/ 5179400,
   /*60*/ 5557000

};

const short int CPaladin::m_pThaco[] ={
   /*1,02,03,04,05,06,07,08,09,10*/
   100, 98, 96, 94, 92, 90, 88, 86, 84, 82,
   80, 78, 76, 74, 72, 70, 68, 66, 64, 62,
   60, 58, 56, 54, 52, 50, 45, 40, 35, 30,
   25, 20, 15, 10, 5, 0, -2, -4, -6, -8,
   -10, -12, -14, -16, -18, -20, -22, -24, -26, -28,
   -50, -50, -50, -50, -50, -50, -50, -50, -50, -50 /*51-60*/
};

//CMemoryAllocator<CMap<CString,char *,CPaladinSpell>::CNode> CMap<CString,char *,CPaladinSpell>::CNode::m_Memory(1,10,sizeof(CMap<CString,char *,CPaladinSpell>::CNode));
CMap<CString, CSpell<CPaladin> *> CPaladin::m_SpellTable((26 * MIN_SPELL_LETTERS), MIN_SPELL_LETTERS);
CMap<CString, long> CPaladin::m_CanMinorCreate;

////////////////////////////
//Spell id's for Paladins
//	How spell id's work...each class defines a spell id for each spell
//	this number is then used to reference the ability to cast that spell
//	in m_Spells[].
#define PALADIN_NUMBER_OF_SPELLS 20
#define PALADIN_BLESS 0
#define PALADIN_SPIRIT_ARMOR 1
#define PALADIN_CURE_SERIOUS_WOUNDS 2
#define PALADIN_CURE_LIGHT_WOUNDS 3
#define PALADIN_TURN_UNDEAD 4
#define PALADIN_DIVINE_ARMOR 5
#define PALADIN_HEAL 6
#define PALADIN_DESTROY_UNDEAD 7
#define PALADIN_FULL_HEAL 8
#define PALADIN_HOLY_WORD 9
#define PALADIN_ARKANS_BATTLE_CRY 10
#define PALADIN_BLINDNESS 11

//////////////////////////////

///////////////////////////////////////
//	InitSpellTable
//	Initalizes the spells and the things
//	this type of caster can cast and minor
//	create.
//	written by: Demetrius Comes
////////////////////////////////////

void CPaladin::InitStatics() {
   if (StaticsInitialized) {
	return;
   }
   StaticsInitialized = true;
   InitSkillInfo();
   //SPELLS
   m_SpellTable.Add("Bless", new CSpell<CPaladin>("Bless", "&WBless&n", PALADIN_BLESS, AFFECT_BLESS, AREA_TARGET,
	   4, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR << 1), 28, 2, 6, &CPaladin::Bless, 0));
   m_SpellTable.Add("Arkans Battle Cry", new CSpell<CPaladin>("Arkans Battle Cry", "&WArkans Battle Cry&n", PALADIN_ARKANS_BATTLE_CRY, AFFECT_ARKANS_BATTLE_CRY, AREA_ROOM,
	   4, 5, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR << 1), 10, 9, 41, &CPaladin::SpellAffectAdd, 0));
   m_SpellTable.Add("Spirit Armor", new CSpell<CPaladin>("Spirit Armor", "&WSpirit Armor&n", PALADIN_SPIRIT_ARMOR, AFFECT_SPIRIT_ARMOR, AREA_TARGET,
	   6, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR << 1), 25, 2, 6, &CPaladin::SpellAffectAdd, 0));
   m_SpellTable.Add("Divine Armor", new CSpell<CPaladin>("Divine Armor", "&WDivine Armor&n", PALADIN_DIVINE_ARMOR, AFFECT_DIVINE_ARMOR, AREA_TARGET,
	   8, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR << 1), 18, 6, 26, &CPaladin::SpellAffectAdd, 0));
   m_SpellTable.Add("Cure Light Wounds", new CSpell<CPaladin>("Cure Light Wounds", "&WCure Light Wounds&n", PALADIN_CURE_LIGHT_WOUNDS,
	   SPELL_NO_AFFECT, AREA_TARGET, 4, 2, 4, 2, 25, 3, 11, &CPaladin::HealSpell, 0));
   m_SpellTable.Add("Cure Serious Wounds", new CSpell<CPaladin>("Cure Serious Wounds", "&WCure Serious Wounds&n", PALADIN_CURE_SERIOUS_WOUNDS,
	   SPELL_NO_AFFECT, AREA_TARGET, 4, 2, 4, 10, 23, 4, 16, &CPaladin::HealSpell, 0));
   m_SpellTable.Add("Turn Undead", new CSpell<CPaladin>("Turn Undead", "&WTurn Undead&n", PALADIN_TURN_UNDEAD, SPELL_NO_AFFECT, AREA_TARGET,
	   4, 7, 100, 200, 21, 5, 21, &CPaladin::TurnUndead, 0));
   m_SpellTable.Add("Full Heal", new CSpell<CPaladin>("Full Heal", "&WFull Heal&n", PALADIN_FULL_HEAL,
	   SPELL_NO_AFFECT, AREA_TARGET, 6, 6, 50, 100, 10, 9, 41, &CPaladin::HealSpell, 0));
   m_SpellTable.Add("Heal", new CSpell<CPaladin>("Heal", "&WHeal&n", PALADIN_HEAL, SPELL_NO_AFFECT, AREA_TARGET,
	   4, 2, 50, 100, 18, 7, 31, &CPaladin::HealSpell, 0));
   //clerics is pulses_per_mud we'll use *6/10 so can never be as good
   m_SpellTable.Add("Blindness", new CSpell<CPaladin>("Blindness", "Blindness", PALADIN_BLINDNESS, AFFECT_BLINDNESS, AREA_TARGET,
	   2, (CMudTime::PULSES_PER_MUD_HOUR * 6 / 10), 1, 0, 15, 8, 36, &CPaladin::SpellAffectAdd, 0));
   m_SpellTable.Add("Holy Word", new CSpell<CPaladin>("Holy Word", "&WHoly Word&n", PALADIN_HOLY_WORD,
	   SPELL_NO_AFFECT, AREA_ROOM, 3, 5, 100, 100, 8, 10, 46, &CPaladin::HolyWord, 0));

   POSITION pos = m_SpellTable.GetStartingPosition();
   bool bReturn = false;
   CSpell<CPaladin> *pCurrentSpell;
   while (pos) {
	pCurrentSpell = m_SpellTable.GetNext(pos);
	m_strSpellList.Format("%s  (%-2d) %-25s",
		m_strSpellList.cptr(),
		pCurrentSpell->GetMinLevel(),
		pCurrentSpell->GetColorizedName().cptr());
	if (bReturn) {
	   bReturn = false;
	   m_strSpellList += "\r\n";
	} else {
	   bReturn = true;
	}
   }
   m_strSpellList += "\r\n";
   //if we do it once's we won't have to do it for every
   //time whe send it to character
   m_strSpellList.Colorize();
}

/////////////////////////////
//	Sets up the skills a Paladin can learn
//	

void CPaladin::InitSkillInfo() { /*max,min,pct*/
   // Base Skills
   m_SkillInfo[SKILL_TRACK] = CSkillInfo(GENERAL, 75, 0, 30);
   m_SkillInfo[SKILL_BODYSLAM] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_BANDAGE] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_BASH] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_DODGE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_DOUBLE_ATTACK] = CSkillInfo(GENERAL, 100, 5, 10);
   m_SkillInfo[SKILL_DUAL_WIELD] = CSkillInfo(GENERAL, 100, 0, 10);
   m_SkillInfo[SKILL_OFFENSE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_FIRST_AID] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_MOUNT] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_BACKSTAB] = CSkillInfo(GENERAL, 75, 0, 10);
   m_SkillInfo[SKILL_TUNDRA_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_DESERT_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_MOUNTAIN_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_FOREST_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_SWAMP_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_FORAGE] = CSkillInfo(GENERAL, 100, 0, 100);
   m_SkillInfo[SKILL_RESCUE] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_SWITCH] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_TRIP] = CSkillInfo(GENERAL, 50, 1, 10);
   m_SkillInfo[SKILL_KICK] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_CLERIC_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_SHAMAN_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_BLIND_FIGHTING] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_THROW_WEAPON] = CSkillInfo(GENERAL, 75, 0, 50);
   m_SkillInfo[SKILL_PARRY] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_BATTLE_READINESS] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_QUICK_STRIKE] = CSkillInfo(GENERAL, 100, 5, 10);
   m_SkillInfo[SKILL_WEAPON_KNOWLEDGE] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_RIPOSTE] = CSkillInfo(GENERAL, 100, 0, 20);
   m_SkillInfo[SKILL_CALLED_SHOT] = CSkillInfo(GENERAL, 100, 0, 20);
   m_SkillInfo[SKILL_WEAPON_REPAIR] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_HONE_WEAPON] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_HIT_ALL] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_MOUNTED_COMBAT] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_ARMOR_REPAIR] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_KNIGHT_CODE] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_MEDITATE] = CSkillInfo(GENERAL, 100, 1, 20);
   m_SkillInfo[SKILL_HEROISM] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_PRAY] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_SUMMON_MOUNT] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_QUICK_CHANT] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_LAY_HANDS] = CSkillInfo(GENERAL, 100, 5, 200);


   //profeciencies
   m_Proficiency.Add("General", 0, 0, 0, GENERAL, 0);
   m_Proficiency.GetProficiencyNames(m_strProficiency);
   m_strProficiency.Format("%s\r\n%-30s\t%-20s\r\n",
	   m_strProficiency.cptr(), "Skill Name", "Proficiency Name");
   int i;
   CString str;
   for (i = 0; i < MAX_MUD_SKILLS; i++) {
	if (m_SkillInfo[i].CanLearn()) {
	   str.Format("%s%-30s\t%-20s\r\n", str.cptr(),
		   FindSkill(i).cptr(),
		   m_Proficiency.GetSingleProficiencyName(m_SkillInfo[i].GetSet()).cptr());
	}
   }
   m_strProficiency += str;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaladin::CPaladin(CCharIntermediate *ch) : CCaster<CPaladin>(ch), CCharacter(ch) {
   InitStatics();
   //can't put in CCharacter construct because
   //it will be called before us
   if (IsNewbie()) {
	InitSkills();
	m_nSkillSphere |= GENERAL;
	m_nAlignment = 1000; //alignment for paladins must start good
   }
}

CPaladin::CPaladin(CMobPrototype &mob, CRoom *pPutInRoom)
: CCaster<CPaladin>(mob, pPutInRoom), CCharacter(mob, pPutInRoom) {
   InitStatics();
   //everyone gets general
   m_nSkillSphere |= GENERAL;
}

CPaladin::~CPaladin() {
}

///////////////////////
//	GetSkillInfo
//
///////////////////////

const CSkillInfo* CPaladin::GetSkillInfo(short nSkill) {
   return &m_SkillInfo[nSkill];
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////

short CPaladin::GetThaco() {
   return m_pThaco[m_nLevel];
}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////

short CPaladin::GetMaxHitPointsPerLevel() {
   return 10;
}

/////////////////////////////
//	Not really needed for real OS
//	but we'll be friendly to 95/98 =)
//////////////////////////////

void CPaladin::CleanNewedStaticMemory() {
   POSITION pos = m_SpellTable.GetStartingPosition();
   while (pos) {
	delete m_SpellTable.GetNext(pos);
   }
}

////////////////////
//	Calculates spell Damage.
///////////////////

int CPaladin::CalcSpellDam(const CSpell<CPaladin> * pSpell) {
   return pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]);
}

///////////////////////
//	GetExp
//	returns exp for level
///////////////////////

int CPaladin::GetExp(short nLvl) {
   return m_Experience[nLvl];
}

////////////////////////
//	Sends spell list to character
//

void CPaladin::SendSpellList() {
   SendToChar(m_strSpellList);
}


////////////////////////
//SendPRoficiency layout

void CPaladin::SendProficiency() {
   CString strProf;
   strProf.Format("Current Proficiency Bonus: %d\r\n%s",
	   m_Proficiency.CalcBonus(m_nSkillSphere),
	   m_strProficiency.cptr());
   SendCharPage(strProf);
}

//////////////////////////////////
//	gives back the pct for a spell up

short CPaladin::GetPctSpellUp(const CSpell <CPaladin> * pSpell) {
   return m_pAttributes->GetInt() / 10;
}

const CSpell<CPaladin> *CPaladin::GetSpellInfo(CString strSpell) {
   CSpell<CPaladin> *pSpell;
   if (m_SpellTable.Lookup(strSpell, pSpell)) {
	return pSpell;
   }
   return NULL;
}

//////////////////////
//	Curse Spell

void CPaladin::Bless(const CInterp * pInterp, const CSpell<CPaladin> *pSpell) {
   short nDam = GetLevel() / 25 + 1;
   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), nDam);
   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
}


////////////////////////////////
//  SpellAffectAdd for General Spells (no reference affects)
//  Sets Timing for General Spells that add affects
//	John Comes 3-30-98
///////////////////////////////

void CPaladin::SpellAffectAdd(const CInterp * pInterp, const CSpell<CPaladin> * pSpell) {
   int nDam = CalcSpellDam(pSpell);
   POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
   switch (pSpell->GetSpellID()) {
	case PALADIN_ARKANS_BATTLE_CRY:
	   CCharacter *pTarget;
	   while (pos) {
		pTarget = m_pInRoom->GetNextCharacter(pos);
		pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, m_Spells[pSpell->GetSpellID()] / 20 + 1);
	   }
	   break;
	case PALADIN_SPIRIT_ARMOR: //At least -10 to AC
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), m_Spells[pSpell->GetSpellID()] / 6 + 10);
	   break;
	case PALADIN_DIVINE_ARMOR: //At least -20 to AC
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), m_Spells[pSpell->GetSpellID()] / 4 + 20);
	   break;
	default:
	   //default does not use SpellMsgs
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), 0);
	   break;
   }
}


//////////////////////////////////////////////
//	Turn Undead
//  Will cause undead, and only undead to flee
//	John Comes   3-30-99
//////////////////////////////////////////////

void CPaladin::TurnUndead(const CInterp * pInterp, const CSpell<CPaladin> * pSpell) {
   if (m_CurrentSpell.m_pTarget->GetRace() == RACE_UNDEAD ||
	   m_CurrentSpell.m_pTarget->GetRace() == RACE_LICH) {
	if (DIE(10) > (m_CurrentSpell.m_pTarget->GetLevel() - GetLevel())
		&& m_CurrentSpell.m_pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL)) {
	   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_DIDNT_WORK, this, m_CurrentSpell.m_pTarget);
	   AddFighters(m_CurrentSpell.m_pTarget, false);
	} else {
	   m_CurrentSpell.m_pTarget->AddCommand((CString) "flee");
	   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
	}
   } else {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_KILLED, this, m_CurrentSpell.m_pTarget);
   }
}


//////////////////////////////////
//	Holy / Unholy Word
//	
//	
//	John Comes 3-13-99

void CPaladin::HolyWord(const CInterp * pInterp, const CSpell<CPaladin> * pSpell) {
   short nDam = CalcSpellDam(pSpell);
   CCharacter *pTarget;
   //get first character in room
   POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
   while (pos) {
	pTarget = m_pInRoom->GetNextCharacter(pos);
	//check if I can hit the person based on their alignment.
	if (pTarget->GetAlignment() > 0) {
	   pTarget->TakeDamage(this, CalcSpellDam(pSpell), false);
	   if (pTarget->IsDead()) {
		m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_KILLED, this, pTarget);
		pInterp->KillChar(pTarget);
	   } else {
		m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, pTarget);
	   }
	   AddFighters(pTarget, false);
	} else {
	   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_DIDNT_WORK, this, pTarget);
	}

   }
}

//////////////////////
//	Identify
//
//	John Comes 2/9/1999

void CPaladin::DoIdentify() {
   CString strItem(CurrentCommand.GetWordAfter(1));
   CObject *pObj;
   CString strToChar;
   int nDot, nNum = 1;

   if ((nDot = strItem.Find('.')) != -1) {
	nNum = strItem.Left(nDot).MakeInt();
	strItem = strItem.Right(nDot);
   }
   if ((pObj = FindInInventory(strItem, nNum)) && pObj->IsWeapon()) {
	if (GetSkill(SKILL_WEAPON_KNOWLEDGE) * 50 > pObj->GetCost()) {
	   SendToChar(pObj->GetIdentifyString());
	} else {
	   SendToChar("You can't seem to gleam any information off this.\r\n");
	}
	SkillUp(SKILL_WEAPON_KNOWLEDGE);
   } else {
	SendToChar("You only have knowledge of weapons.\r\n");
   }
   LagChar(CMudTime::PULSES_PER_REAL_SECOND * 4);
}

/////////////////////////
//	Hit All
//
//	John Comes 3-1-99

void CPaladin::DoHitAll(const CInterp *pInterp) {
   if (!GetSkill(SKILL_HIT_ALL)) {
	SendToChar("Learn how first!\r\n");
	return;
   }
   CString strAll = CurrentCommand.GetWordAfter(1);
   if (!strAll.Compare("all") && !IsFighting()) {
	SendToChar("You have to be fighting to drop the 'all' command.\r\n");
	return;
   } else {
	CCharacter *pTarget;
	short nSkill = GetSkill(SKILL_HIT_ALL) + m_Proficiency.CalcBonus(m_nSkillSphere) * 10;
	POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
	short nLag = 5;
	for (int i = 0; i < MAX_EQ_POS; i++) { //look for my weapon
	   if (!m_Equipment[i].IsEmpty() && m_Equipment[i]->IsWeapon()) {
		while (pos) {//Get first character in the room
		   pTarget = GetRoom()->GetNextCharacter(pos);
		   //Don't hit myself!
		   if (pTarget != this) { //Do I get the skill check to hit the char?
			if (DIE(100) <= nSkill) { //If I didn't put "all" and the target is fighting me, hit him or
			   //if I put all, hit it reguardless because we all ready made sure you were
			   //put all and were not fighting above
			   if ((!strAll.Compare("all") && pTarget->IsFighting() == this) || strAll.Compare("all")) {
				m_MsgManager.SkillMsg(this, this, SKILL_HIT_ALL, SKILL_WORKED);
				pTarget->Defense(CalcACWouldHit() - m_Proficiency.CalcBonus(m_nSkillSphere) * 10, i, this);
				nLag++; //Add lag for every char I hit
				if (pTarget->IsDead()) {//if target is dead, kill it!
				   pInterp->KillChar(pTarget);
				} else {//if target is not dead add it to fight LL.
				   AddFighters(pTarget);
				}
			   }

			} else {//if I miss a check stop hitting all and kick out.
			   m_MsgManager.SkillMsg(this, this, SKILL_HIT_ALL, SKILL_DIDNT_WORK);
			   SkillUp(SKILL_HIT_ALL);
			   i = MAX_EQ_POS;
			   pos = NULL;
			}
		   }
		}
	   }
	}
	LagChar(nLag * CMudTime::PULSES_PER_BATTLE_ROUND);
   }
}


//////////////////////////////////////////////
//	Destroy Undead
//  Will do massive damage but only to undead
//	John Comes   3-8-99
//////////////////////////////////////////////

void CPaladin::DestroyUndead(const CInterp * pInterp, const CSpell<CPaladin> * pSpell) {
   if (m_CurrentSpell.m_pTarget->GetRace() == RACE_UNDEAD ||
	   m_CurrentSpell.m_pTarget->GetRace() == RACE_LICH) {
	m_CurrentSpell.m_pTarget->TakeDamage(this, CalcSpellDam(pSpell), false);
	if (m_CurrentSpell.m_pTarget->IsDead()) {
	   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_KILLED, this, m_CurrentSpell.m_pTarget);
	} else {
	   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
	   AddFighters(m_CurrentSpell.m_pTarget, false);
	}
   } else {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_DIDNT_WORK, this, m_CurrentSpell.m_pTarget);
	if (m_CurrentSpell.m_pTarget != this) {
	   AddFighters(m_CurrentSpell.m_pTarget, false);
	}
   }
}



//////////////////////////////////
//	HealSpell
//	this function runs for all Cleric Heal spells
//	do checks to see if we can cast in this room etc.
//	next check to see if spell is room affect
//	or target
//	modified from CCleric::Offensive by John Comes

void CPaladin::HealSpell(const CInterp * pInterp, const CSpell<CPaladin> * pSpell) {
   CCharacter *pTarget;
   if (pSpell->GetAffectedArea() & AREA_ROOM) {
	//get first character in room
	POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
	while (pos) {
	   pTarget = m_pInRoom->GetNextCharacter(pos);
	   pTarget->TakeDamage(this, -1 * CalcSpellDam(pSpell), false);
	   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, pTarget);
	}
   }	//we are casting at self or someone else not a room
   else {
	pTarget = m_CurrentSpell.m_pTarget;
	if (!this->IsInRoom(pTarget)) {
	   SendToChar("Who?\r\n");
	   return;
	}
	pTarget->TakeDamage(this, -1 * CalcSpellDam(pSpell), false);
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, pTarget);
	if (pSpell->GetSpellID() == PALADIN_FULL_HEAL) {
	   pTarget->RemoveAffect(AFFECT_BLINDNESS);
	}
   }
}

/////////////////////////
//	Lay Hands
//	Paladin Lay Hands, will heal person
//	John Comes 3-31-99

void CPaladin::DoLayHands() {
   CCharacter *pTarget = GetTarget();
   if (!IsGood()) {
	SendToChar("Do you think your abilities still work even after you stray from the good side?\r\n");
   } else if (IsAffectedBy(AFFECT_CANT_LAY_HANDS)) {
	SendToChar("You are not rejuvenated enough to lay your hands.\r\n");
   } else if (!pTarget) {
	SendToChar("Lay your hands on who?\r\n");
   } else {
	pTarget->TakeDamage(this, (GetSkill(SKILL_LAY_HANDS)*-10), false);
	m_MsgManager.SkillMsg(this, pTarget, SKILL_LAY_HANDS, SKILL_WORKED);
	SkillUp(SKILL_LAY_HANDS);
	LagChar(4 * CMudTime::PULSES_PER_BATTLE_ROUND);
	AddAffect(CCharacter::AFFECT_CANT_LAY_HANDS, (CMudTime::PULSES_PER_MUD_DAY / 2), 0);
   }
}


////////////////////////////
//  Bash
//  Virtual from CCharacter Bash, no minuses for 2H Weapons
//  J Comes 7/28/99
////////////////////////////

void CPaladin::DoBash(const CInterp *pInterp, CCharacter *pTarget) {
   if (!GetSkill(SKILL_BASH)) {
	SendToChar("Learn how first!\r\n");
	return;
   }
   bool bHasShield = false;
   bool bHas2H = false;
   for (int i = 0; i < MAX_EQ_POS; i++) {
	if (m_Equipment[i].GetWearBit() & WEAR_POS_SHIELD && !m_Equipment[i].IsEmpty()) {
	   bHasShield = true;
	} else if (m_Equipment[i].GetWearBit() & WEAR_POS_WIELD_TWO_HANDED && !m_Equipment[i].IsEmpty()) {
	   bHas2H = true;
	}
   }
   if (pTarget) {
	CString strToChar, strToRoom;
	short int Adjust = 0;
	if (!pTarget->InPosition(POS_STANDING)) {
	   Adjust = DIE(25) + 25; // add up to 50% chance to miss if target is not standing.
	}
	//if you're not wearing a shield and not a anti/paladin wielding a 2h weapon you have a bad chance
	if (!bHas2H && !bHasShield) {
	   SendToChar("Bashing without a shield is tough, but you try anyway...\r\n");
	   Adjust += DIE(25) + 50;
	}
	if ((short int) DIE(100) + Adjust <= GetSkill(SKILL_BASH)) {
	   pTarget->TakeDamage(this, GetSkill(SKILL_BASH) / 5, true);
	   if (pTarget->IsDead()) {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_BASH, SKILL_WORKED);
		pInterp->KillChar(pTarget);
	   } else {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_BASH, SKILL_WORKED);
		pTarget->UpDatePosition(POS_SITTING, false);
		pTarget->LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * (DIE(3) + 1));
	   }
	} else {
	   m_MsgManager.SkillMsg(this, pTarget, SKILL_BASH, SKILL_DIDNT_WORK);
	   UpDatePosition(POS_SITTING, false);
	   SkillUp(SKILL_BASH);
	}
	AddFighters(pTarget);
	LagChar(CMudTime::PULSES_PER_REAL_SECOND * 3);
   } else {
	SendToChar("Bash who?\r\n");
   }
}

//////////////////////////////////////////////
//	overloaded DoCast
//	Paladin can't cast if alignment is better less than good

void CPaladin::DoCast(const CInterp *pInterp, bool bForceCast) {
   if (IsGood()) {
	CCaster<CPaladin>::DoCast(pInterp, bForceCast);
   } else {
	SendToChar("You think you get your abilities after turning from the good alignment?\r\n");
   }
}

void CPaladin::ResetCharVars() {
   CCaster<CPaladin>::ResetCharVars();
   short nPos;
   for (int i = 0; i < GetMaxWeaponPositions(); i++) {
	nPos = GetWeaponPosition(i + 1);
	if (!m_Equipment[nPos].IsEmpty()
		&& m_Equipment[nPos].IsWorn(WEAR_POS_WIELD_TWO_HANDED)
		&& m_Equipment[nPos].GetObject()->IsWeapon()) {
	   m_nToHit += GetLevel() / LVLS_PER_2H_WIELD_BONUS;
	   m_nDamRoll += GetLevel() / LVLS_PER_2H_WIELD_BONUS;
	}
   }
}

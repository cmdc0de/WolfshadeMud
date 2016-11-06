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
// Ranger.cpp: implementation of the CRanger class.
//
//////////////////////////////////////////////////////////////////////
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "ranger.h"
//////////////////////////////
//	Statics
////////////////////////////

#define MAX_RELOCATE_LAG 6

extern CGlobalVariableManagement GVM;

bool CRanger::StaticsInitialized = false;
CSkillInfo CRanger::m_SkillInfo[MAX_MUD_SKILLS];
CString CRanger::m_strSpellList;
const UINT CRanger::GENERAL = (1 << 0);
CProficiency CRanger::m_Proficiency;
CString CRanger::m_strProficiency("You class has the following proficiencies:\r\n");

const int CRanger::m_Experience[LVL_IMP + 1] = {
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

const short int CRanger::m_pThaco[] = {
   /*1,02,03,04,05,06,07,08,09,10*/
   100, 98, 96, 94, 92, 90, 88, 86, 84, 82,
   80, 78, 76, 74, 72, 70, 68, 66, 64, 62,
   60, 58, 56, 54, 52, 50, 45, 40, 35, 30,
   25, 20, 15, 10, 5, 0, -2, -4, -6, -8,
   -10, -12, -14, -16, -18, -20, -22, -24, -26, -28,
   -50, -50, -50, -50, -50, -50, -50, -50, -50, -50 /*51-60*/
};

//CMemoryAllocator<CMap<CString,char *,CRangerSpell>::CNode> CMap<CString,char *,CRangerSpell>::CNode::m_Memory(1,10,sizeof(CMap<CString,char *,CRangerSpell>::CNode));
CMap<CString, CSpell<CRanger> *> CRanger::m_SpellTable((26 * MIN_SPELL_LETTERS), MIN_SPELL_LETTERS);
CMap<CString, long> CRanger::m_CanMinorCreate;

////////////////////////////
//Spell id's for Rangers
//	How spell id's work...each class defines a spell id for each spell
//	this number is then used to reference the ability to cast that spell
//	in m_Spells[].
#define RANGER_NUMBER_OF_SPELLS 20
#define RANGER_MAGIC_MISSILE 0
#define RANGER_ARMOR 1
#define RANGER_DISPEL_MAGIC 3
#define RANGER_LIGHTNING_BOLT 4
#define RANGER_HASTE 5
#define RANGER_FAERIE_FIRE 6
#define RANGER_BARKSKIN 7
#define RANGER_FIRE_SHIELD 8
#define RANGER_CAMOUFLAGE 9
#define RANGER_FAERIE_FLIGHT 10
#define RANGER_BLOODLUST 11
#define RANGER_BLESS 12




//////////////////////////////

///////////////////////////////////////
//	InitSpellTable
//	Initalizes the spells and the things
//	this type of caster can cast and minor
//	create.
//	written by: Demetrius Comes
////////////////////////////////////

void CRanger::InitStatics() {
   if (StaticsInitialized) {
	return;
   }
   StaticsInitialized = true;
   InitSkillInfo();
   //SPELLS
   m_SpellTable.Add("Magic Missile", new CSpell<CRanger>("Magic Missile", "&GMagic Missile&n", RANGER_MAGIC_MISSILE, SPELL_NO_AFFECT, AREA_TARGET,
	   1, 2, 4, 0, 28, 1, 1, &CRanger::MagicMissile, 0));
   m_SpellTable.Add("Armor", new CSpell<CRanger>("Armor", "&GArmor&n", RANGER_ARMOR, SPELL_NO_AFFECT, AREA_TARGET,
	   2, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 28, 1, 1, &CRanger::SpellAffectAdd, 0));
   m_SpellTable.Add("Haste", new CSpell<CRanger>("Haste", "&GHaste&n", RANGER_HASTE, CCharacter::AFFECT_HASTE, AREA_TARGET,
	   4, 4, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 18, 6, 26, &CRanger::SpellAffectAdd, 0));
   m_SpellTable.Add("Dispel Magic", new CSpell<CRanger>("Dispel Magic", "&GDispel Magic&n", RANGER_DISPEL_MAGIC, SPELL_NO_AFFECT, AREA_TARGET,
	   3, 4, 20, 20, 23, 4, 16, &CRanger::DispelMagic, 0));
   m_SpellTable.Add("Barkskin", new CSpell<CRanger>("Barkskin", "&GBarkskin&n", RANGER_BARKSKIN, CCharacter::AFFECT_BARKSKIN, AREA_TARGET,
	   4, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 15, 7, 31, &CRanger::SpellAffectAdd, 0));
   m_SpellTable.Add("Lightning Bolt", new CSpell<CRanger>("Lightning Bolt", "&GLightning Bolt&n", RANGER_LIGHTNING_BOLT, SPELL_NO_AFFECT, AREA_TARGET,
	   3, 10, 10, 20, 20, 5, 21, &CRanger::OffensiveSpell, 0));
   m_SpellTable.Add("Fire Shield", new CSpell<CRanger>("Fire Shield", "&GFire Shield&n", RANGER_FIRE_SHIELD, CCharacter::AFFECT_FIRE_SHIELD, AREA_SELF,
	   4, 10, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 15, 7, 31, &CRanger::SpellAffectAdd, 0));
   m_SpellTable.Add("Faerie Flight", new CSpell<CRanger>("Faerie Flight", "&GFaerie Flight&n", RANGER_FAERIE_FLIGHT, CCharacter::AFFECT_FAERIE_FLIGHT, AREA_SELF,
	   4, 60, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 10, 9, 41, &CRanger::Flight, 0));
   m_SpellTable.Add("Camouflage", new CSpell<CRanger>("Camouflage", "&GCamouflage&n", RANGER_CAMOUFLAGE, CCharacter::AFFECT_CAMOUFLAGE, AREA_SELF,
	   4, 60, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 8, 10, 46, &CRanger::Camouflage, 0));
   m_SpellTable.Add("Bloodlust", new CSpell<CRanger>("Bloodlust", "&GBloodlust&n", RANGER_BLOODLUST, CCharacter::AFFECT_BLOODLUST, AREA_SELF,
	   4, 4, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 5, 11, 51, &CRanger::SpellAffectAdd, 0));
   m_SpellTable.Add("Bless", new CSpell<CRanger>("Bless", "&GBless&n", RANGER_BLESS, CCharacter::AFFECT_BLESS, AREA_TARGET,
	   4, 10, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 23, 3, 11, &CRanger::Bless, 0));
   m_SpellTable.Add("Faerie Fire", new CSpell<CRanger>("Faerie Fire", "&GFaerie Fire&n", RANGER_FAERIE_FIRE, CCharacter::AFFECT_FAERIE_FIRE, AREA_TARGET,
	   4, 10, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 13, 8, 36, &CRanger::SpellAffectAdd, 0));





   POSITION pos = m_SpellTable.GetStartingPosition();
   bool bReturn = false;
   CSpell<CRanger> *pCurrentSpell;
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
//	Sets up the skills a Ranger can learn
//	

void CRanger::InitSkillInfo() { /*max,min,pct*/
   // Base Skills
   m_SkillInfo[SKILL_BODYSLAM] = CSkillInfo(GENERAL, 100, 1, 20);
   m_SkillInfo[SKILL_BANDAGE] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_BASH] = CSkillInfo(GENERAL, 100, 1, 20);
   m_SkillInfo[SKILL_DODGE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_DUAL_WIELD] = CSkillInfo(GENERAL, 100, 5, 10);
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
   m_SkillInfo[SKILL_KICK] = CSkillInfo(GENERAL, 100, 1, 20);
   m_SkillInfo[SKILL_CLERIC_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_SHAMAN_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_BLIND_FIGHTING] = CSkillInfo(GENERAL, 100, 1, 100);

   m_SkillInfo[SKILL_THROW_WEAPON] = CSkillInfo(GENERAL, 75, 0, 50);
   m_SkillInfo[SKILL_PARRY] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_BATTLE_READINESS] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_QUICK_STRIKE] = CSkillInfo(GENERAL, 100, 5, 10);
   m_SkillInfo[SKILL_WEAPON_KNOWLEDGE] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_RIPOSTE] = CSkillInfo(GENERAL, 100, 0, 10);
   m_SkillInfo[SKILL_CALLED_SHOT] = CSkillInfo(GENERAL, 100, 0, 20);
   m_SkillInfo[SKILL_WEAPON_REPAIR] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_HONE_WEAPON] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_HIT_ALL] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_MOUNTED_COMBAT] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_ARMOR_REPAIR] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_AWARENESS] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_MEDITATE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_PREPARE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_QUICK_CHANT] = CSkillInfo(GENERAL, 100, 1, 20);
   m_SkillInfo[SKILL_SUMMON_MOUNT] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_TRACK] = CSkillInfo(GENERAL, 100, 5, 190);
   m_SkillInfo[SKILL_TRAP] = CSkillInfo(GENERAL, 100, 0, 50);
   m_SkillInfo[SKILL_DOUBLE_ATTACK] = CSkillInfo(GENERAL, 100, 5, 10);
   m_SkillInfo[SKILL_WEAPON_FLURRY] = CSkillInfo(GENERAL, 100, 0, 20);

   //profeciencies
   m_Proficiency.Add("General", 0, 0, 0, GENERAL, 0);
   m_Proficiency.GetProficiencyNames(m_strProficiency);
   m_strProficiency.Format("%s\r\n%-30s\t%-20s\r\n",
	   m_strProficiency.cptr(), "Skill Name", "Proficiency Name");
   int i;
   CString str;
   for (i = 0; i < MAX_MUD_SKILLS; i++) {
	if (m_SkillInfo[i].CanLearn()) {
	   str.Format("%s%-30s\t%-20s\r\n",
		   str.cptr(),
		   FindSkill(i).cptr(),
		   m_Proficiency.GetSingleProficiencyName(m_SkillInfo[i].GetSet()).cptr());
	}
   }
   m_strProficiency += str;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRanger::CRanger(CCharIntermediate *ch) : CCaster<CRanger>(ch), CCharacter(ch) {
   InitStatics();
   //can't put in CCharacter construct because
   //it will be called before us
   if (IsNewbie()) {
	InitSkills();
	m_nSkillSphere |= GENERAL;
	m_nAlignment = 1000;
   }
}

CRanger::CRanger(CMobPrototype &mob, CRoom *pPutInRoom)
: CCaster<CRanger>(mob, pPutInRoom), CCharacter(mob, pPutInRoom) {
   InitStatics();
   //everyone gets general
   m_nSkillSphere |= GENERAL;
}

CRanger::~CRanger() {
}

///////////////////////
//	GetSkillInfo
//
///////////////////////

const CSkillInfo* CRanger::GetSkillInfo(short nSkill) {
   return &m_SkillInfo[nSkill];
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////

short CRanger::GetThaco() {
   return m_pThaco[m_nLevel];
}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////

short CRanger::GetMaxHitPointsPerLevel() {
   return 10;
}

/////////////////////////////
//	Not really needed for real OS
//	but we'll be friendly to 95/98 =)
//////////////////////////////

void CRanger::CleanNewedStaticMemory() {
   POSITION pos = m_SpellTable.GetStartingPosition();
   while (pos) {
	delete m_SpellTable.GetNext(pos);
   }
}

////////////////////
//	Calculates spell Damage.
///////////////////

int CRanger::CalcSpellDam(const CSpell<CRanger> * pSpell) {
   return pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]);
}

///////////////////////
//	GetExp
//	returns exp for level
///////////////////////

int CRanger::GetExp(short nLvl) {
   return m_Experience[nLvl];
}

////////////////////////
//	Sends spell list to character
//

void CRanger::SendSpellList() {
   SendToChar(m_strSpellList);
}


////////////////////////
//SendPRoficiency layout

void CRanger::SendProficiency() {
   CString strProf;
   strProf.Format("Current Proficiency Bonus: %d\r\n%s",
	   m_Proficiency.CalcBonus(m_nSkillSphere),
	   m_strProficiency.cptr());
   SendCharPage(strProf);
}

//////////////////////
//	Magic Missile
//

void CRanger::MagicMissile(const CInterp * pInterp, const CSpell<CRanger> *pSpell) {
   short nMissiles = 5;
   if (GetLevel() <= 16) {
	nMissiles = GetLevel() / 4 + 1;
   }
   for (int i = 0; i < nMissiles; i++) {
	OffensiveSpell(pInterp, pSpell);
   }
}


//////////////////////////
//	Dispel Magic
//	If failed save or has consent all affects
//	Will be removed
//	John Comes	12-28-98
///////////////////////////

void CRanger::DispelMagic(const CInterp *pInterp, const CSpell<CRanger> *pSpell) {
   if ((short int) DIE(150) <= CalcSpellDam(pSpell) || m_CurrentSpell.m_pTarget->GetConsented() == this || m_CurrentSpell.m_pTarget == this) //SAVING THROW NEEDED
   {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
	m_CurrentSpell.m_pTarget->RemoveAllAffects(true);
	return;
   } else {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_DIDNT_WORK, this, m_CurrentSpell.m_pTarget);
   }
}



////////////////////////////////
//  SpellAffectAdd for General Spells (no reference affects)
//  Sets Timing for General Spells that add affects
//	John Comes 12-28-98
///////////////////////////////

void CRanger::SpellAffectAdd(const CInterp * pInterp, const CSpell<CRanger> * pSpell) {
   switch (pSpell->GetSpellID()) {
	case RANGER_ARMOR:
	   m_CurrentSpell.m_pTarget->AddAffect(CCharacter::AFFECT_ARMOR, CalcSpellDam(pSpell), 10);
	   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
	   break;
	case RANGER_FAERIE_FIRE:
	   m_CurrentSpell.m_pTarget->AddAffect(CCharacter::AFFECT_FAERIE_FIRE, CalcSpellDam(pSpell), -(m_Spells[pSpell->GetSpellID()] / 5));
	   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
	   AddFighters(m_CurrentSpell.m_pTarget, false);
	   break;
	case RANGER_BARKSKIN:
	   if (m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), 20)) {
		m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
	   }
	   break;
	default:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), 0);
	   break;
   }
}


////////////////////////////////
//  Flight
//  Sets Timing for Flight Spells
//	John Comes 1-19-99
///////////////////////////////

void CRanger::Flight(const CInterp * pInterp, const CSpell<CRanger> * pSpell) {
   //affect,pulses,value
   short nDam = CalcSpellDam(pSpell);
   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, 0);
   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
}


////////////////////////////////
//  Camouflage
//  Sets Timing for Camouflage (Invis)
//	John Comes 12-21-98
///////////////////////////////

void CRanger::Camouflage(const CInterp * pInterp, const CSpell<CRanger> * pSpell) {
   short nDam = CalcSpellDam(pSpell);
   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, 0);
   m_CurrentSpell.m_pTarget->AddAffect(CCharacter::AFFECT_INVISIBLE, nDam, 0);
   m_CurrentSpell.m_pTarget->SendToChar("&gYou &Gblend &yinto &gthe &Gsurroundings....&n\r\n");
   GetRoom()->SendToRoom("&g%s &Gblends &yinto &gthe &Gsurroundings...&n\r\n", m_CurrentSpell.m_pTarget);
}


//////////////////////
//	Bless and Curse Spell

void CRanger::Bless(const CInterp * pInterp, const CSpell<CRanger> *pSpell) {
   short nDam = GetLevel() / 25 + 1;
   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), nDam);
   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
}


//////////////////////
//	Identify
//
//	John Comes 2/9/1999

void CRanger::DoIdentify() {
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

const CSpell<CRanger> *CRanger::GetSpellInfo(CString strSpell) {
   CSpell<CRanger> *pSpell;
   if (m_SpellTable.Lookup(strSpell, pSpell)) {
	return pSpell;
   }
   return NULL;
}

//////////////////////////////////
//	gives back the pct for a spell up

short CRanger::GetPctSpellUp(const CSpell <CRanger> * pSpell) {
   return m_pAttributes->GetInt() / 10;
}

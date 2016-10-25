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
//////////////////////////////////////////////////////////////
// Druid.cpp: implementation of the CDruid class.
//
//////////////////////////////////////////////////////////////////////
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "druid.h"
#include "portal.h"
//////////////////////////////
//	Statics
////////////////////////////

#define MAX_RELOCATE_LAG 6

extern CGlobalVariableManagement GVM;

bool CDruid::StaticsInitialized = false;
CSkillInfo CDruid::m_SkillInfo[MAX_MUD_SKILLS];
CString CDruid::m_strSpellList;
const UINT CDruid::GENERAL = (1 << 0);
CProficiency CDruid::m_Proficiency;
CString CDruid::m_strProficiency("You class has the following proficiencies:\r\n");

const int CDruid::m_Experience[LVL_IMP + 1] ={
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

const short int CDruid::m_pThaco[] ={
  /*1,02,03,04,05,06,07,08,09,10*/
  100, 98, 96, 94, 92, 90, 88, 86, 84, 82,
  80, 78, 76, 74, 72, 70, 68, 66, 64, 62,
  60, 58, 56, 54, 52, 50, 45, 40, 35, 30,
  25, 20, 15, 10, 5, 0, -2, -4, -6, -8,
  -10, -12, -14, -16, -18, -20, -22, -24, -26, -28,
  -50, -50, -50, -50, -50, -50, -50, -50, -50, -50 /*51-60*/
};

//CMemoryAllocator<CMap<CString,char *,CDruidSpell>::CNode> CMap<CString,char *,CDruidSpell>::CNode::m_Memory(1,10,sizeof(CMap<CString,char *,CDruidSpell>::CNode));
CMap<CString, CSpell<CDruid> *> CDruid::m_SpellTable((26 * MIN_SPELL_LETTERS), MIN_SPELL_LETTERS);
CMap<CString, long> CDruid::m_CanMinorCreate;

////////////////////////////
//Spell id's for Druids
//	How spell id's work...each class defines a spell id for each spell
//	this number is then used to reference the ability to cast that spell
//	in m_Spells[].
#define DRUID_NUMBER_OF_SPELLS 20
#define DRUID_SPIRIT_ARMOR 0
#define DRUID_FIND_FAMILIAR 1
#define DRUID_DISPEL_MAGIC 2
#define DRUID_SUMMON_BEAST 3
#define DRUID_LIGHTNING_BOLT 4
#define DRUID_WALL_OF_BRAMBLES 5
#define DRUID_POLYMORPH_OTHER 6
#define DRUID_HEAL 7
#define DRUID_HARM 8
#define DRUID_FORESTAL_ARMOR 9
#define DRUID_CHAIN_LIGHTNING 10
#define DRUID_MOONWELL 11
#define DRUID_CREEPING_DOOM 12

//////////////////////////////

///////////////////////////////////////
//	InitSpellTable
//	Initalizes the spells and the things
//	this type of caster can cast and minor
//	create.
//	written by: Demetrius Comes
////////////////////////////////////

void CDruid::InitStatics() {
  if (StaticsInitialized) {
	return;
  }
  StaticsInitialized = true;
  InitSkillInfo();
  //SPELLS
  m_SpellTable.Add("Moonwell", new CSpell<CDruid>("Moonwell", "&gMoonwell&n", DRUID_MOONWELL, SPELL_NO_AFFECT, 0,
	  8, 3, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 10, 46, &CDruid::Moonwell, 0));
  m_SpellTable.Add("Lightning bolt", new CSpell<CDruid>("Lightning bolt", "&gLightning bolt&n", DRUID_LIGHTNING_BOLT,
	  SPELL_NO_AFFECT, AREA_TARGET, 3, 5, 10, 20, 23, 5, 21, &CDruid::OffensiveSpell, 0));
  m_SpellTable.Add("Find Familiar", new CSpell<CDruid>("Find Familiar", "&gFind Familiar&n", DRUID_FIND_FAMILIAR, SPELL_NO_AFFECT, AREA_NONE,
	  6, 24, CMudTime::PULSES_PER_MUD_HOUR, CMudTime::PULSES_PER_MUD_HOUR, 28, 1, 1, &CDruid::FindFamiliar, 0));
  m_SpellTable.Add("Wall of Brambles", new CSpell<CDruid>("Wall of Brambles", "&gWall of Brambles&n", DRUID_WALL_OF_BRAMBLES, SPELL_NO_AFFECT, 0,
	  8, 0, 0, VNUM_WALL_OF_BRAMBLES, 18, 6, 26, &CDruid::Wall, 0));
  m_SpellTable.Add("Dispel Magic", new CSpell<CDruid>("Dispel Magic", "&gDispel Magic&n", DRUID_DISPEL_MAGIC, SPELL_NO_AFFECT, AREA_TARGET,
	  4, 8, 10, 20, 23, 3, 11, &CDruid::DispelMagic, 0));
  m_SpellTable.Add("Forestal Armor", new CSpell<CDruid>("Forestal Armor", "&gForestal Armor&n", DRUID_FORESTAL_ARMOR, CCharacter::AFFECT_FORESTAL_ARMOR, AREA_SELF,
	  6, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 13, 9, 41, &CDruid::SpellAffectAdd, 0));
  m_SpellTable.Add("Creeping Doom", new CSpell<CDruid>("Creeping Doom", "&gCreeping Doom&n", DRUID_CREEPING_DOOM, SPELL_NO_AFFECT, 0,
	  6, 4, 5, 16, 8, 10, 46, &CDruid::CreepingDoom, 0));
  m_SpellTable.Add("Harm", new CSpell<CDruid>("Harm", "&gHarm&n", DRUID_HARM, SPELL_NO_AFFECT, AREA_TARGET,
	  2, 2, 50, 100, 13, 8, 36, &CDruid::OffensiveSpell, 0));
  m_SpellTable.Add("Heal", new CSpell<CDruid>("Heal", "&gHeal&n", DRUID_HEAL, SPELL_NO_AFFECT, AREA_TARGET,
	  2, 2, 50, 100, 13, 8, 36, &CDruid::HealSpell, 0));
  m_SpellTable.Add("Spirit Armor", new CSpell<CDruid>("Spirit Armor", "&gSpirit Armor&n", DRUID_SPIRIT_ARMOR, AFFECT_SPIRIT_ARMOR, AREA_TARGET,
	  3, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR << 1), 28, 1, 1, &CDruid::SpellAffectAdd, 0));




  POSITION pos = m_SpellTable.GetStartingPosition();
  bool bReturn = false;
  CSpell<CDruid> *pCurrentSpell;
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
//	Sets up the skills a Druid can learn
//	

void CDruid::InitSkillInfo() { /*max,min,pct*/
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
  m_SkillInfo[SKILL_BLIND_FIGHTING] = CSkillInfo(GENERAL, 50, 1, 30);

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
  m_SkillInfo[SKILL_TRACK] = CSkillInfo(GENERAL, 100, 1, 50);
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
		  str.cptr(), FindSkill(i).cptr(),
		  m_Proficiency.GetSingleProficiencyName(m_SkillInfo[i].GetSet()).cptr());
	}
  }
  m_strProficiency += str;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDruid::CDruid(CCharIntermediate *ch) : CCaster<CDruid>(ch), CCharacter(ch) {
  InitStatics();
  //can't put in CCharacter construct because
  //it will be called before us
  if (IsNewbie()) {
	InitSkills();
	m_nSkillSphere |= GENERAL;
  }
}

CDruid::CDruid(CMobPrototype &mob, CRoom *pPutInRoom)
: CCaster<CDruid>(mob, pPutInRoom), CCharacter(mob, pPutInRoom) {
  InitStatics();
  //everyone gets general
  m_nSkillSphere |= GENERAL;
}

CDruid::~CDruid() {
}

///////////////////////
//	GetSkillInfo
//
///////////////////////

const CSkillInfo* CDruid::GetSkillInfo(short nSkill) {
  return &m_SkillInfo[nSkill];
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////

short CDruid::GetThaco() {
  return m_pThaco[m_nLevel];
}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////

short CDruid::GetMaxHitPointsPerLevel() {
  return 10;
}

/////////////////////////////
//	Not really needed for real OS
//	but we'll be friendly to 95/98 =)
//////////////////////////////

void CDruid::CleanNewedStaticMemory() {
  POSITION pos = m_SpellTable.GetStartingPosition();
  while (pos) {
	delete m_SpellTable.GetNext(pos);
  }
}

////////////////////
//	Calculates spell Damage.
///////////////////

int CDruid::CalcSpellDam(const CSpell<CDruid> * pSpell) {
  return pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]);
}

///////////////////////
//	GetExp
//	returns exp for level
///////////////////////

int CDruid::GetExp(short nLvl) {
  return m_Experience[nLvl];
}

////////////////////////
//	Sends spell list to character
//

void CDruid::SendSpellList() {
  SendToChar(m_strSpellList);
}


////////////////////////
//SendPRoficiency layout

void CDruid::SendProficiency() {
  CString strProf;
  strProf.Format("Current Proficiency Bonus: %d\r\n%s",
	  m_Proficiency.CalcBonus(m_nSkillSphere), m_strProficiency.cptr());
  SendCharPage(strProf);
}


////////////////////////////////
//  SpellAffectAdd for General Spells (no reference affects)
//  Sets Timing for General Spells that add affects
//	John Comes 12-28-98
///////////////////////////////

void CDruid::SpellAffectAdd(const CInterp * pInterp, const CSpell<CDruid> * pSpell) {
  switch (pSpell->GetSpellID()) {
	case DRUID_FORESTAL_ARMOR:
	  m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), 20);
	  break;
	case DRUID_SPIRIT_ARMOR: //At least -10 to AC
	  m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), m_Spells[pSpell->GetSpellID()] / 6 + 10);
	  break;
	default:
	  m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), 0);
	  break;
  }
}

//////////////////////////////////
//	gives back the pct for a spell up

short CDruid::GetPctSpellUp(const CSpell <CDruid> * pSpell) {
  return m_pAttributes->GetInt() / 5;
}

const CSpell<CDruid> *CDruid::GetSpellInfo(CString strSpell) {
  CSpell<CDruid> *pSpell;
  if (m_SpellTable.Lookup(strSpell, pSpell)) {
	return pSpell;
  }
  return 0;
}


/////////////////////////
//	Moonwell
//
//	Portal Spell
//	John Comes 4-1-99
/////////////////////////

void CDruid::Moonwell(const CInterp *pInterp, const CSpell<CDruid> *pSpell) {
  CCharacter *pTarget;
  pTarget = GVM.FindCharacter(m_CurrentSpell.m_strTarget, this);

  if (pTarget == NULL
	  || pTarget->IsNPC()
	  || !CanSeeChar(pTarget)
	  || CanSeeInRoom(pTarget->GetRoom()) != CCharacter::SEES_ALL
	  || pTarget->IsGod()) {
	SendToChar("You can't moonwell to that person\r\n");
	return;
  } else {
	pInterp->CreatePortal(pTarget->GetRoom(), this->GetRoom(), CPortal::MOONWELL, CalcSpellDam(pSpell));
	pTarget->GetRoom()->SendToRoom("&GThe ground before you bubbles up revealing a &WMoonwell&G!&n\r\n");
	GetRoom()->SendToRoom("&GThe ground before you bubbles up revealing a &WMoonwell&G!&n\r\n");
  }
}


////////////////////////////////////
//	Find Familiar
//	Creates an animal from a random roll
//	written by: John Comes 10-21-98
////////////////////////////////////

void CDruid::FindFamiliar(const CInterp *pInterp, const CSpell<CDruid> *pSpell) {
  //vnum for familiars...
  mob_vnum Familiar = DIE(8) + 9; //mobs 10-17 in 1.mob
  CCharacter *pMob = pInterp->CreateMob(Familiar, GetRoom());
  if (pMob != NULL) {
	m_pInRoom->SendToRoom("%s calls to the wild for a familiar.\r\n", this);
	if (this->CurrentlyMasterOf()<((GetLevel() / 20) + 1)) {
	  pMob->SetMaster(this, CalcSpellDam(pSpell));
	} else {
	  CString str;
	  str.Format("&r%s resists your magical hold.\r\nYou do not control %s.&n\r\n",
		  pMob->GetName().cptr(), pMob->GetName().cptr());
	  SendToChar(str);
	}
  } else {
	SendToChar("No friends will find you today!\r\n");
	ErrorLog << "Missing mob prototype for FindFamiliar" << endl;
  }
}



//////////////////////////////////
//	Wall of Stone
//	Leaves a Perm-Para'd mob in the room blocking an enterance
//	NEED TO FIX FOR DIFFERENT MOBS FOR DIRECTIONS AND ADD THE PERM PARA
//	11-26-98 John Comes
//////////////////////////////////

void CDruid::Wall(const CInterp *pInterp, const CSpell<CDruid> *pSpell) {
  short nDir = CRoom::StringToDirection(m_CurrentSpell.m_strTarget);
  if (nDir == UP || nDir == DOWN) {
	SendToChar("You must use north, east, west or south.\r\n");
  } else {
	sWallInfo WallInfo;
	WallInfo.m_nDirection = nDir;
	WallInfo.m_nLevel = GetLevel();
	WallInfo.m_nWallType = pSpell->GetWallType();
	WallInfo.m_nTimeToLive = m_Spells[pSpell->GetSpellID()] / 7;
	CCharacter *pMob = pInterp->CreateWall(&WallInfo, GetRoom());
	CString strToChar;
	CString strToRoom = "%s";
	strToChar.Format("You create %s to the %s!\r\n", pMob->GetName().cptr(), m_CurrentSpell.m_strTarget.cptr());
	SendToChar(strToChar);
	strToChar.Format(" creates %s to the %s!\r\n", pMob->GetName().cptr(), m_CurrentSpell.m_strTarget.cptr());
	strToRoom += strToChar;
	GetRoom()->SendToRoom(strToRoom, this);
  }
}



//////////////////////////
//	Dispel Magic
//	If failed save or has consent all affects
//	Will be removed
//	John Comes	12-28-98
///////////////////////////

void CDruid::DispelMagic(const CInterp *pInterp, const CSpell<CDruid> *pSpell) {
  if (m_CurrentSpell.m_pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL)
	  || m_CurrentSpell.m_pTarget->GetConsented() == this || m_CurrentSpell.m_pTarget == this) {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
	m_CurrentSpell.m_pTarget->RemoveAllAffects(true);
	return;
  } else {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_DIDNT_WORK, this, m_CurrentSpell.m_pTarget);
  }
}


///////////////////////
//	Creeping Doom
//	Modified Summon Swarm
//	John Comes 4-2-99
//////////////////////

void CDruid::CreepingDoom(const CInterp * pInterp, const CSpell<CDruid> * pSpell) {
  if (m_pInRoom->IsAffectedBy(CRoom::ROOM_AFFECT_PEACEFUL)) {
	SendToChar("This place is far to peacefull.\r\n");
	return;
  }
  SendToChar("You conjure forth a &GSwarm&n of animals!\r\n");
  GetRoom()->SendToRoom("%s conjures forth a &GSwarm&n of animals!\r\n", this);
  long lSwarm = 13; // Mob Vnum 13 is spider
  short iMaxMobs;
  iMaxMobs = CalcSpellDam(pSpell);
  CCharacter *pTarget;
  POSITION pos;
  CString str;
  int i;
  for (i = 0; i < iMaxMobs; i++) {
	pos = m_pInRoom->GetFirstCharacterInRoom();
	while (pos && (i < iMaxMobs)) {
	  pTarget = m_pInRoom->GetNextCharacter(pos);
	  //make sure it's not me
	  if (pTarget != this && !m_Group.IsInGroup(pTarget) && !m_Master.IsMasterOf(pTarget)) {
		CCharacter *pMob = pInterp->CreateMob(lSwarm, GetRoom());
		GetRoom()->SendToRoom("%s charges in and attacks %s!\r\n", pMob, pTarget);
		AddFighters(pMob, false);
		if (this->CurrentlyMasterOf()<((GetLevel() / 10) + 1)) {
		  pMob->SetMaster(this, CMudTime::PULSES_PER_MUD_HOUR, false);
		} else {
		  str.Format("&r%s resists your magical hold.\r\nYou do not control %s.&n\r\n",
			  pMob->GetName().cptr(), pMob->GetName().cptr());
		  SendToChar(str);
		}
		i++;
	  }
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

void CDruid::HealSpell(const CInterp * pInterp, const CSpell<CDruid> * pSpell) {
  CCharacter *pTarget;
  pTarget = m_CurrentSpell.m_pTarget;
  pTarget->TakeDamage(this, -1 * CalcSpellDam(pSpell), false);
  m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, pTarget);
}

/////////////////////////
//	Hit All
//
//	John Comes 3-1-99

void CDruid::DoHitAll(const CInterp *pInterp) {
  if (!GetSkill(SKILL_HIT_ALL)) {
	SendToChar("Learn how first!\r\n");
	return;
  }
  CString strAll = CurrentCommand.GetWordAfter(1);
  if (!strAll.Compare("all") && !IsFighting()) {
	SendToChar("You have to be fighting to drop the 'all' command.\r\n");
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
				  AddFighters(pTarget, false);
				}
			  }

			} else {//if I miss a check stop hitting all and kick out.
			  m_MsgManager.SkillMsg(this, this, SKILL_HIT_ALL, SKILL_DIDNT_WORK);
			  SkillUp(SKILL_HIT_ALL);
			  i = MAX_EQ_POS;
			  pos = 0;
			}
		  }
		}
	  }
	}
	LagChar((nLag * CMudTime::PULSES_PER_BATTLE_ROUND) - (m_Proficiency.CalcBonus(m_nSkillSphere)) * CMudTime::PULSES_PER_BATTLE_ROUND);
  }
}

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
// Psionicist.cpp: implementation of the CPsionicist class.
//
//////////////////////////////////////////////////////////////////////
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"

#include "psionicist.h"
#include "portal.h"
//////////////////////////////
//	Statics
////////////////////////////

extern CRandom GenNum;

extern CGlobalVariableManagement GVM;

CSkillInfo CPsionicist::m_SkillInfo[MAX_MUD_SKILLS];
bool CPsionicist::StaticsInitialized = false;
CString CPsionicist::m_strSpellList;
const UINT CPsionicist::GENERAL = (1 << 0);
CProficiency CPsionicist::m_Proficiency;
CString CPsionicist::m_strProficiency("You class has the following proficiencies:\r\n");

const short int CPsionicist::m_pThaco[] = {
   /*1,02,03,04,05,06,07,08,09,10*/
   100, 98, 97, 95, 94, 92, 91, 89, 88, 86,
   85, 83, 82, 80, 79, 77, 76, 75, 73, 71,
   70, 68, 67, 65, 64, 62, 61, 59, 58, 56,
   55, 53, 51, 49, 48, 46, 45, 43, 42, 40,
   39, 37, 36, 35, 33, 32, 30, 29, 27, 26,
   25, 23, 22, 20, 19, 17, 16, 14, 13, 11 /*51-60*/
};

const int CPsionicist::m_Experience[LVL_IMP + 1] = {
   /*0*/ 0, /*1*/ 1, /*2*/ 300, /*3*/ 900,
   /*4*/ 1800, /*5*/ 3000, /*6*/ 4500, /*7*/ 6300,
   /*8*/ 8400, /*9*/ 10800, /*10*/ 13500, /*11*/ 16500,
   /*12*/ 23100, /*13*/ 30300, /*14*/ 38100, /*15*/ 46500,
   /*16*/ 55500, /*17*/ 65100, /*18*/ 75300, /*19*/ 86100,
   /*20*/ 97500, /*21*/ 109500, /*22*/ 134700, /*23*/ 161100,
   /*24*/ 188700, /*25*/ 217500, /*26*/ 247500, /*27*/ 278700,
   /*28*/ 311100, /*29*/ 344700, /*30*/ 379500, /*31*/ 451500,
   /*32*/ 525900, /*33*/ 602700, /*34*/ 681900, /*35*/ 763500,
   /*36*/ 847500, /*37*/ 933900, /*38*/ 1022700, /*39*/ 1113900,
   /*40*/ 1207500, /*41*/ 1399500, /*42*/ 1596300, /*43*/ 1797900,
   /*44*/ 2004300, /*45*/ 2215500, /*46*/ 2431500, /*47*/ 2652300,
   /*48*/ 2877900, /*49*/ 3108300, /*50*/ 3343500, /*51*/ 3583500,
   /*52*/ 4073100, /*53*/ 4572300, /*54*/ 5081100, /*55*/ 5599500,
   /*56*/ 6127500, /*57*/ 6665100, /*58*/ 7212300, /*59*/ 7769100,
   /*60*/ 8335500

};

CMap<CString, CSpell<CPsionicist> *> CPsionicist::m_SpellTable((26 * MIN_SPELL_LETTERS), MIN_SPELL_LETTERS);

////////////////////////////
//Spell id's for Psionicists
//	How spell id's work...each class defines a spell id for each spell
//	this number is then used to reference the ability to cast that spell
//	in m_Spells[].
#define PSIONICIST_DEATH_FIELD 0
#define PSIONICIST_DETONATE 1
#define PSIONICIST_PLANE_OF_PAIN 2
#define PSIONICIST_THOUGHT_WHIP 3
#define PSIONICIST_MOLECULAR_AGITATION 4
#define PSIONICIST_MENTAL_ANGUISH 5
#define PSIONICIST_LIFE_DRAIN 6
#define PSIONICIST_MENTAL_ABUSE 7
#define PSIONICIST_RUSE 8
#define PSIONICIST_CONFUSION 9
#define PSIONICIST_SELF_SACRIFICE 10
#define PSIONICIST_BIOFEEDBACK 11
#define PSIONICIST_WORMHOLE 12
//#define PSIONICIST_HALLUCINATION 13
#define PSIONICIST_PSIONIC_BLAST 14
#define PSIONICIST_INERTIAL_BARRIER 15
#define PSIONICIST_TRANSPARENT_FORM 16
#define PSIONICIST_AIR_WALK 17
#define PSIONICIST_CLEAR_SIGHT 18
#define PSIONICIST_BATTLE_READY 19
#define PSIONICIST_PHYSICAL_ENHANCEMENT 20
#define PSIONICIST_PSIONIC_LEECH 21

//////////////////////////////

///////////////////////////////////////
//	InitSpellTable
//	Initalizes the spells and the things
//	this type of caster can cast and minor
//	create.
//	written by: Demetrius Comes
////////////////////////////////////

void CPsionicist::InitStatics() {
   if (StaticsInitialized) {
	return;
   }
   StaticsInitialized = true;
   InitSkillInfo();

   m_SpellTable.Add("Death Field", new CSpell<CPsionicist>("Death Field", "&LDeath Field&n", PSIONICIST_DEATH_FIELD, SPELL_NO_AFFECT, AREA_ROOM | SPELL_OFFENSIVE,
	   3, 10, 100, 200, 0, 10, 46, &CPsionicist::OffensiveSpell, 0));
   m_SpellTable.Add("Detonate", new CSpell<CPsionicist>("Detonate", "&LDetonate&n", PSIONICIST_DETONATE, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   2, 10, 100, 100, 0, 7, 31, &CPsionicist::OffensiveSpell, 0));
   m_SpellTable.Add("Plane of pain", new CSpell<CPsionicist>("Plane of pain", "&LPlane of Pain&n", PSIONICIST_PLANE_OF_PAIN, SPELL_NO_AFFECT, AREA_ROOM | SPELL_OFFENSIVE,
	   2, 4, 100, 100, 0, 8, 36, &CPsionicist::OffensiveSpell, 0));
   m_SpellTable.Add("Thought Whip", new CSpell<CPsionicist>("Thought Whip", "&LThought Whip&n", PSIONICIST_THOUGHT_WHIP, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   1, 2, 6, 0, 0, 1, 1, &CPsionicist::OffensiveSpell, 0));
   m_SpellTable.Add("Molecular Agitation", new CSpell<CPsionicist>("Molecular Agitation", "&LMolecular Agitation&n", PSIONICIST_MOLECULAR_AGITATION, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   2, 5, 5, 5, 0, 2, 6, &CPsionicist::OffensiveSpell, 0));
   m_SpellTable.Add("Mental Anguish", new CSpell<CPsionicist>("Mental Anguish", "&LMental Anguish&n", PSIONICIST_MENTAL_ANGUISH, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   2, 5, 10, 10, 0, 3, 11, &CPsionicist::OffensiveSpell, 0));
   m_SpellTable.Add("Life Drain", new CSpell<CPsionicist>("Life Drain", "&LLife Drain&n", PSIONICIST_LIFE_DRAIN, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   2, 4, 20, 20, 0, 4, 16, &CPsionicist::OffensiveSpell, 0));
   m_SpellTable.Add("Mental Abuse", new CSpell<CPsionicist>("Mental Abuse", "&LMental Abuse&n", PSIONICIST_MENTAL_ABUSE, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   2, 5, 20, 100, 0, 5, 21, &CPsionicist::OffensiveSpell, 0));
   m_SpellTable.Add("Ruse", new CSpell<CPsionicist>("Ruse", "&LRuse&n", PSIONICIST_RUSE, SPELL_NO_AFFECT, AREA_NONE,
	   1, 0, 0, 0, 0, 1, 1, &CPsionicist::Ruse, 0));
   m_SpellTable.Add("Confusion", new CSpell<CPsionicist>("Confusion", "&LConfusion&n", PSIONICIST_CONFUSION, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   1, 100, 1, 0, 0, 6, 26, &CPsionicist::Confusion, 0));
   m_SpellTable.Add("Self Sacrifice", new CSpell<CPsionicist>("Self Sacrifice", "&LSelf Sacrifice&n", PSIONICIST_SELF_SACRIFICE, SPELL_NO_AFFECT, AREA_NONE,
	   1, 10, 5, 5, 0, 3, 11, &CPsionicist::SelfSacrifice, 0));
   m_SpellTable.Add("Biofeedback", new CSpell<CPsionicist>("Biofeedback", "&LBiofeedback&n", PSIONICIST_BIOFEEDBACK, CCharacter::AFFECT_BIOFEEDBACK, AREA_SELF,
	   4, 2, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 5, 21, &CPsionicist::SpellAffectAdd, 0));
   m_SpellTable.Add("Wormhole", new CSpell<CPsionicist>("Wormhole", "&LWormhole&n", PSIONICIST_WORMHOLE, SPELL_NO_AFFECT, AREA_NONE,
	   8, 3, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 10, 46, &CPsionicist::Wormhole, 0));
   m_SpellTable.Add("Psionic Blast", new CSpell<CPsionicist>("Psionic Blast", "&LPsionic Blast&n", PSIONICIST_PSIONIC_BLAST, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   1, 10, 7, 0, 0, 2, 6, &CPsionicist::PsionicBlast, 0));
   m_SpellTable.Add("Inertial Barrier", new CSpell<CPsionicist>("Inertial Barrier", "&LInertial Barrier&n", PSIONICIST_INERTIAL_BARRIER, CCharacter::AFFECT_INERTIAL_BARRIER, AREA_SELF,
	   5, 1, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 8, 36, &CPsionicist::SpellAffectAdd, 0));
   m_SpellTable.Add("Transparent Form", new CSpell<CPsionicist>("Transparent Form", "&LTransparent Form&n", PSIONICIST_TRANSPARENT_FORM, CCharacter::AFFECT_TRANSPARENT_FORM, AREA_TARGET,
	   5, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 7, 31, &CPsionicist::SpellAffectAdd, 0));
   m_SpellTable.Add("Air Walk", new CSpell<CPsionicist>("Air Walk", "&LAir Walk&n", PSIONICIST_AIR_WALK, CCharacter::AFFECT_AIR_WALK, AREA_TARGET,
	   5, 60, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 7, 31, &CPsionicist::SpellAffectAdd, 0));
   m_SpellTable.Add("Clear Sight", new CSpell<CPsionicist>("Clear Sight", "&LClear Sight&n", PSIONICIST_CLEAR_SIGHT, CCharacter::AFFECT_CLEAR_SIGHT, AREA_TARGET,
	   5, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 9, 41, &CPsionicist::SpellAffectAdd, 0));
   m_SpellTable.Add("Battle Ready", new CSpell<CPsionicist>("Battle Ready", "&LBattle Ready&n", PSIONICIST_BATTLE_READY, CCharacter::AFFECT_BATTLE_READY, AREA_SELF,
	   3, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 1, 1, &CPsionicist::SpellAffectAdd, 0));
   m_SpellTable.Add("Physical Enhancement", new CSpell<CPsionicist>("Physical Enhancement", "&LPhysical Enhancement&n", PSIONICIST_PHYSICAL_ENHANCEMENT, CCharacter::AFFECT_PHYSICAL_ENHANCEMENT, AREA_TARGET,
	   3, 40, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 4, 16, &CPsionicist::SpellAffectAdd, 0));
   m_SpellTable.Add("Psionic Leech", new CSpell<CPsionicist>("Psionic Leech", "&LPsionic Leech&n", PSIONICIST_PSIONIC_LEECH, AFFECT_PSIONIC_LEECH, AREA_SELF,
	   1, 10, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 6, 26, &CPsionicist::SpellAffectAdd, 0));

   POSITION pos = m_SpellTable.GetStartingPosition();
   bool bReturn = false;
   CSpell<CPsionicist> *pCurrentSpell;
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPsionicist::CPsionicist(CCharIntermediate *ch)
: CThinker<CPsionicist>(ch), CCharacter(ch) {
   InitStatics();
   //can't put in CCharacter construct because
   //it will be called before us
   if (IsNewbie()) {
	InitSkills();
	m_nSkillSphere |= GENERAL;
   }
}

CPsionicist::CPsionicist(CMobPrototype &mob, CRoom *pPutInRoom)
: CThinker<CPsionicist>(mob, pPutInRoom), CCharacter(mob, pPutInRoom) {
   InitStatics();
   //everyone gets general
   m_nSkillSphere |= GENERAL;
}

CPsionicist::~CPsionicist() {
}

/////////////////////////////
//	Sets up the skills a warrior can learn
//	

void CPsionicist::InitSkillInfo() { /*max,min,pct*/

   // Base Skills
   m_SkillInfo[SKILL_TRACK] = CSkillInfo(GENERAL, 50, 0, 10);
   m_SkillInfo[SKILL_BODYSLAM] = CSkillInfo(GENERAL, 50, 1, 10);
   m_SkillInfo[SKILL_BANDAGE] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_BASH] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_DODGE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_DUAL_WIELD] = CSkillInfo(GENERAL, 75, 0, 10);
   m_SkillInfo[SKILL_OFFENSE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_FIRST_AID] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_MOUNT] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_BACKSTAB] = CSkillInfo(GENERAL, 50, 0, 10);
   m_SkillInfo[SKILL_TUNDRA_MAGIC] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_DESERT_MAGIC] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_MOUNTAIN_MAGIC] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_FOREST_MAGIC] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_SWAMP_MAGIC] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_FORAGE] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_RESCUE] = CSkillInfo(GENERAL, 75, 1, 10);
   m_SkillInfo[SKILL_SWITCH] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_TRIP] = CSkillInfo(GENERAL, 50, 0, 10);
   m_SkillInfo[SKILL_KICK] = CSkillInfo(GENERAL, 50, 0, 10);
   m_SkillInfo[SKILL_CLERIC_MAGIC] = CSkillInfo(GENERAL, 50, 0, 10);
   m_SkillInfo[SKILL_SHAMAN_MAGIC] = CSkillInfo(GENERAL, 75, 0, 10);
   m_SkillInfo[SKILL_BLIND_FIGHTING] = CSkillInfo(GENERAL, 50, 1, 10);

   m_SkillInfo[SKILL_THROW_WEAPON] = CSkillInfo(GENERAL, 50, 0, 10);
   m_SkillInfo[SKILL_MEDITATE] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_QUICK_CHANT] = CSkillInfo(GENERAL, 100, 1, 50);

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

const CSkillInfo* CPsionicist::GetSkillInfo(short nSkill) {
   return &m_SkillInfo[nSkill];
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////

short CPsionicist::GetThaco() {
   return m_pThaco[m_nLevel];
}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////

short CPsionicist::GetMaxHitPointsPerLevel() {
   return 4;
}

/////////////////////////////
//	Not really needed for real OS
//	but we'll be friendly to 95/98 =)
//////////////////////////////

void CPsionicist::CleanNewedStaticMemory() {
   POSITION pos = m_SpellTable.GetStartingPosition();
   while (pos) {
	delete m_SpellTable.GetNext(pos);
   }
}

///////////////////////////
//	GetManaAdjust overridden
//	returns POW adjustment

short CPsionicist::GetManaAdjust() {
   return m_pAttributes->GetManaAdjustPow();
}

///////////////////////////
//	returns exp of level passed in
///////////////////////////

int CPsionicist::GetExp(short nLvl) {
   return m_Experience[nLvl];
}


///////////////////////////
//	Ruse -> Same as Cantrip
//
//	John Comes 12-08-98
///////////////////////////

void CPsionicist::Ruse(const CInterp *pInterp, const CSpell<CPsionicist> *pSpell) {
   CString strToRoom(CurrentCommand.GetWordAfter(2, true));
   strToRoom.Format("%s \r\n", strToRoom.cptr());
   m_pInRoom->SendToRoom(strToRoom);
}

//////////////////////////
//	Confusion 
//	If failed save, target will stop casting
//
//	John Comes	12-08-98
///////////////////////////

void CPsionicist::Confusion(const CInterp *pInterp, const CSpell<CPsionicist> *pSpell) {
   if (!m_CurrentSpell.m_pTarget->IsAffectedBy(CCharacter::AFFECT_CASTING)) {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_DIDNT_WORK, this, m_CurrentSpell.m_pTarget);
	return;
   }
   if ((short int) DIE(150) <= CalcSpellDam(pSpell)) {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
	m_CurrentSpell.m_pTarget->RemoveAffect(CCharacter::AFFECT_CASTING);
	return;
   } else {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_DIDNT_WORK, this, m_CurrentSpell.m_pTarget);
   }
}

//////////////////////////
//	Self Sacrifice
//	Makes mana out of HP
//
//	John Comes	12-08-98
///////////////////////////

void CPsionicist::SelfSacrifice(const CInterp *pInterp, const CSpell<CPsionicist> *pSpell) {
   int nDam = CalcSpellDam(pSpell);
   if (nDam > m_nCurrentHpts) {
	nDam = m_nCurrentHpts;
   }
   TakeDamage(this, nDam, false, false);
   m_nManaPts += nDam + m_CurrentSpell.m_pSpell->GetMana(false);
   if (m_nManaPts > m_nMaxManaPts) {
	m_nManaPts = m_nMaxManaPts + m_CurrentSpell.m_pSpell->GetMana(false);
   }
   return;
}


/////////////////////////
//	Wormhole
//
//	Portal Spell
//	John Comes 12-9-98
/////////////////////////

void CPsionicist::Wormhole(const CInterp *pInterp, const CSpell<CPsionicist> *pSpell) {
   CCharacter *pTarget = GVM.FindCharacter(m_CurrentSpell.m_strTarget, this);

   if (pTarget == NULL
	   || pTarget->IsNPC()
	   || !CanSeeChar(pTarget)
	   || CanSeeInRoom(pTarget->GetRoom()) != CCharacter::SEES_ALL
	   || pTarget->IsGod()) {
	SendToChar("You can't wormhole to that person!\r\n");
	return;
   } else {
	pInterp->CreatePortal(pTarget->GetRoom(), m_pInRoom, CPortal::WORMHOLE, CalcSpellDam(pSpell));
	pTarget->GetRoom()->SendToRoom("&LThe space before your starts to tear, revealing a wormhole!&n\r\n");
	GetRoom()->SendToRoom("&LThe space before your starts to tear, revealing a wormhole!&n\r\n");
   }
}

//////////////////////////////////////
//	Psionic Blast
//	Possiblity of Stunning the Target
//	written by: John Comes 12-9-98
//////////////////////////////////////

void CPsionicist::PsionicBlast(const CInterp *pInterp, const CSpell<CPsionicist> *pSpell) {
   if (!m_pAttributes->MakeSavingThrow(CAttributes::SAVE_SPELL)) //arbitrary number untill saves are put in
   {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
	m_CurrentSpell.m_pTarget->UpDatePosition(POS_STUNNED, true);
	return;
   } else {
	m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_DIDNT_WORK, this, m_CurrentSpell.m_pTarget);
	AddFighters(m_CurrentSpell.m_pTarget, false);
   }
}

////////////////////////////////
//  SpellAffectAdd
//  Sets Timing for just spells that add an affect
//	John Comes 11/10/99
///////////////////////////////

void CPsionicist::SpellAffectAdd(const CInterp * pInterp, const CSpell<CPsionicist> * pSpell) {
   short nDam = CalcSpellDam(pSpell);
   switch (pSpell->GetSpellID()) {
	case PSIONICIST_TRANSPARENT_FORM:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, 0);
	   break;
	case PSIONICIST_AIR_WALK:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, 0);
	   break;
	case PSIONICIST_CLEAR_SIGHT:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, 0);
	   break;
	case PSIONICIST_BATTLE_READY: case PSIONICIST_PHYSICAL_ENHANCEMENT:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, m_Spells[pSpell->GetSpellID()] / 25);
	   break;
	case PSIONICIST_INERTIAL_BARRIER:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), ((m_Spells[pSpell->GetSpellID()]*20) + m_nLevel));
	   break;
	case PSIONICIST_PSIONIC_LEECH:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), ((m_Spells[pSpell->GetSpellID()]*100) + m_nLevel));
	   break;
	default:
	   //default does not use SpellMsgs
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), 0);
	   break;
   }
}




//////////////////////////
//	SendSpellList to char

void CPsionicist::SendSpellList() {
   SendToChar(m_strSpellList);
}

////////////////////////
//SendPRoficiency layout

void CPsionicist::SendProficiency() {
   CString strProf;
   strProf.Format("Current Proficiency Bonus: %d\r\n%s",
	   m_Proficiency.CalcBonus(m_nSkillSphere),
	   m_strProficiency.cptr());
   SendCharPage(strProf);
}

/////////////////////////////////////
//	returns spell info for CThinker class

const CSpell<CPsionicist> *CPsionicist::GetSpellInfo(CString strSpell) const {
   CSpell<CPsionicist> *pSpell;
   if (!m_SpellTable.Lookup(strSpell, pSpell)) {
	return NULL;
   }
   return pSpell;
}

short CPsionicist::GetPctSpellUp(const CSpell<CPsionicist> *pSpell) const {
   return m_pAttributes->GetStat(POW) / 5;
}

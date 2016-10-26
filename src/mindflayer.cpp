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
// Mindflayer.cpp: implementation of the CMindflayer class.
//
//////////////////////////////////////////////////////////////////////
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "mindflayer.h"
#include "portal.h"
//////////////////////////////
//	Statics
////////////////////////////

extern CRandom GenNum;

extern CGlobalVariableManagement GVM;

CSkillInfo CMindflayer::m_SkillInfo[MAX_MUD_SKILLS];
bool CMindflayer::StaticsInitialized = false;
CString CMindflayer::m_strSpellList;
const UINT CMindflayer::GENERAL = (1 << 0);
CProficiency CMindflayer::m_Proficiency;
CString CMindflayer::m_strProficiency("You class has the following proficiencies:\r\n");
const int CMindflayer::LVL_CAN_BE_DOMINATED = 30;
const int CMindflayer::MAX_CAN_BE_DOMINATED = 1;

const short int CMindflayer::m_pThaco[] = {
   /*1,02,03,04,05,06,07,08,09,10*/
   100, 98, 97, 95, 94, 92, 91, 89, 88, 86,
   85, 83, 82, 80, 79, 77, 76, 75, 73, 71,
   70, 68, 67, 65, 64, 62, 61, 59, 58, 56,
   55, 53, 51, 49, 48, 46, 45, 43, 42, 40,
   39, 37, 36, 35, 33, 32, 30, 29, 27, 26,
   25, 23, 22, 20, 19, 17, 16, 14, 13, 11 /*51-60*/
};

const int CMindflayer::m_Experience[LVL_IMP + 1] = {
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

CMap<CString, CSpell<CMindflayer> *> CMindflayer::m_SpellTable((26 * MIN_SPELL_LETTERS), MIN_SPELL_LETTERS);

////////////////////////////
//Spell id's for CMindflayer
//	How spell id's work...each class defines a spell id for each spell
//	this number is then used to reference the ability to cast that spell
//	in m_Spells[].
#define MINDFLAYER_DEATH_FIELD 0
#define MINDFLAYER_DETONATE 1
#define MINDFLAYER_PLANE_OF_PAIN 2
#define MINDFLAYER_THOUGHT_WHIP 3
#define MINDFLAYER_MOLECULAR_AGITATION 4
#define MINDFLAYER_MENTAL_ANGUISH 5
#define MINDFLAYER_LIFE_DRAIN 6
#define MINDFLAYER_MENTAL_ABUSE 7
#define MINDFLAYER_RUSE 8
#define MINDFLAYER_CONFUSION 9
#define MINDFLAYER_SELF_SACRIFICE 10
#define MINDFLAYER_BIOFEEDBACK 11
#define MINDFLAYER_WORMHOLE 12
#define MINDFLAYER_HALLUCINATION 13
#define MINDFLAYER_PSIONIC_BLAST 14
#define MINDFLAYER_INERTIAL_BARRIER 15
#define MINDFLAYER_TRANSPARENT_FORM 16
#define MINDFLAYER_AIR_WALK 17
#define MINDFLAYER_CLEAR_SIGHT 18
#define MINDFLAYER_BATTLE_READY 19
#define MINDFLAYER_PHYSICAL_ENHANCEMENT 20
#define MINDFLAYER_SHARDS 21
#define MINDFLAYER_MIND_RIP 22
#define MINDFLAYER_DOMINATION 23
#define MINDFLAYER_PSIONIC_LEECH 24

//////////////////////////////

///////////////////////////////////////
//	InitSpellTable
//	Initalizes the spells and the things
//	this type of caster can cast and minor
//	create.
//	written by: Demetrius Comes
////////////////////////////////////

void CMindflayer::InitStatics() {
   if (StaticsInitialized) {
	return;
   }
   StaticsInitialized = true;
   InitSkillInfo();

   m_SpellTable.Add("Death Field", new CSpell<CMindflayer>("Death Field", "&MDeath Field&n", MINDFLAYER_DEATH_FIELD, SPELL_NO_AFFECT, AREA_ROOM | SPELL_OFFENSIVE,
	   3, 10, 100, 100, 0, 10, 46, &CMindflayer::OffensiveSpell, 0));
   m_SpellTable.Add("Detonate", new CSpell<CMindflayer>("Detonate", "&MDetonate&n", MINDFLAYER_DETONATE, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   2, 10, 100, 100, 0, 7, 31, &CMindflayer::OffensiveSpell, 0));
   m_SpellTable.Add("Plane of pain", new CSpell<CMindflayer>("Plane of pain", "&MPlane of Pain&n", MINDFLAYER_PLANE_OF_PAIN, SPELL_NO_AFFECT, AREA_ROOM | SPELL_OFFENSIVE,
	   3, 4, 50, 100, 0, 8, 36, &CMindflayer::OffensiveSpell, 0));
   m_SpellTable.Add("Thought Whip", new CSpell<CMindflayer>("Thought Whip", "&MThought Whip&n", MINDFLAYER_THOUGHT_WHIP, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   1, 2, 6, 0, 0, 1, 1, &CMindflayer::OffensiveSpell, 0));
   m_SpellTable.Add("Molecular Agitation", new CSpell<CMindflayer>("Molecular Agitation", "&MMolecular Agitation&n", MINDFLAYER_MOLECULAR_AGITATION, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   2, 5, 5, 5, 0, 2, 6, &CMindflayer::OffensiveSpell, 0));
   m_SpellTable.Add("Mental Anguish", new CSpell<CMindflayer>("Mental Anguish", "&MMental Anguish&n", MINDFLAYER_MENTAL_ANGUISH, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   3, 5, 10, 10, 0, 3, 11, &CMindflayer::OffensiveSpell, 0));
   m_SpellTable.Add("Life Drain", new CSpell<CMindflayer>("Life Drain", "&MLife Drain&n", MINDFLAYER_LIFE_DRAIN, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   3, 4, 20, 20, 0, 4, 16, &CMindflayer::OffensiveSpell, 0));
   m_SpellTable.Add("Mental Abuse", new CSpell<CMindflayer>("Mental Abuse", "&MMental Abuse&n", MINDFLAYER_MENTAL_ABUSE, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   4, 5, 20, 100, 0, 5, 21, &CMindflayer::OffensiveSpell, 0));
   m_SpellTable.Add("Ruse", new CSpell<CMindflayer>("Ruse", "&MRuse&n", MINDFLAYER_RUSE, SPELL_NO_AFFECT, 0,
	   1, 0, 0, 0, 0, 1, 1, &CMindflayer::Ruse, 0));
   m_SpellTable.Add("Confusion", new CSpell<CMindflayer>("Confusion", "&MConfusion&n", MINDFLAYER_CONFUSION, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   1, 100, 1, 0, 0, 6, 26, &CMindflayer::Confusion, 0));
   m_SpellTable.Add("Self Sacrifice", new CSpell<CMindflayer>("Self Sacrifice", "&MSelf Sacrifice&n", MINDFLAYER_SELF_SACRIFICE, SPELL_NO_AFFECT, 0,
	   1, 10, 5, 5, 0, 3, 11, &CMindflayer::SelfSacrifice, 0));
   m_SpellTable.Add("Biofeedback", new CSpell<CMindflayer>("Biofeedback", "&MBiofeedback&n", MINDFLAYER_BIOFEEDBACK, CCharacter::AFFECT_BIOFEEDBACK, AREA_SELF,
	   4, 2, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 5, 21, &CMindflayer::SpellAffectAdd, 0));
   m_SpellTable.Add("Wormhole", new CSpell<CMindflayer>("Wormhole", "&MWormhole&n", MINDFLAYER_WORMHOLE, SPELL_NO_AFFECT, 0,
	   8, 5, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_MIN * 25), 0, 10, 46, &CMindflayer::Wormhole, 0));
   m_SpellTable.Add("Psionic Blast", new CSpell<CMindflayer>("Psionic Blast", "&MPsionic Blast&n", MINDFLAYER_PSIONIC_BLAST, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   1, 10, 7, 0, 0, 2, 6, &CMindflayer::PsionicBlast, 0));
   m_SpellTable.Add("Inertial Barrier", new CSpell<CMindflayer>("Inertial Barrier", "&MInertial Barrier&n", MINDFLAYER_INERTIAL_BARRIER, CCharacter::AFFECT_INERTIAL_BARRIER, AREA_SELF,
	   5, 1, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 8, 36, &CMindflayer::SpellAffectAdd, 0));
   m_SpellTable.Add("Transparent Form", new CSpell<CMindflayer>("Transparent Form", "&MTransparent Form&n", MINDFLAYER_TRANSPARENT_FORM, CCharacter::AFFECT_TRANSPARENT_FORM, AREA_TARGET,
	   5, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 7, 31, &CMindflayer::SpellAffectAdd, 0));
   m_SpellTable.Add("Air Walk", new CSpell<CMindflayer>("Air Walk", "&MAir Walk&n", MINDFLAYER_AIR_WALK, CCharacter::AFFECT_AIR_WALK, AREA_TARGET,
	   5, 60, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 7, 31, &CMindflayer::SpellAffectAdd, 0));
   m_SpellTable.Add("Clear Sight", new CSpell<CMindflayer>("Clear Sight", "&MClear Sight&n", MINDFLAYER_CLEAR_SIGHT, CCharacter::AFFECT_CLEAR_SIGHT, AREA_TARGET,
	   5, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 9, 41, &CMindflayer::SpellAffectAdd, 0));
   m_SpellTable.Add("Battle Ready", new CSpell<CMindflayer>("Battle Ready", "&MBattle Ready&n", MINDFLAYER_BATTLE_READY, CCharacter::AFFECT_BATTLE_READY, AREA_TARGET,
	   3, 30, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 1, 1, &CMindflayer::SpellAffectAdd, 0));
   m_SpellTable.Add("Physical Enhancement", new CSpell<CMindflayer>("Physical Enhancement", "&MPhysical Enhancement&n", MINDFLAYER_PHYSICAL_ENHANCEMENT, CCharacter::AFFECT_PHYSICAL_ENHANCEMENT, AREA_TARGET,
	   3, 40, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 4, 16, &CMindflayer::SpellAffectAdd, 0));
   m_SpellTable.Add("Shards", new CSpell<CMindflayer>("Shards", "&MShards&n", MINDFLAYER_SHARDS, CCharacter::AFFECT_BERSERK, AREA_TARGET | SPELL_OFFENSIVE,
	   3, 1, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 10, 46, &CMindflayer::Shards, 0));
   m_SpellTable.Add("Mind Rip", new CSpell<CMindflayer>("Mind Rip", "&MMind Rip&n", MINDFLAYER_MIND_RIP, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   3, 12, 100, 100, 0, 9, 41, &CMindflayer::OffensiveSpell, 0));
   m_SpellTable.Add("Domination", new CSpell<CMindflayer>("Dominiation", "&MDomination&n", MINDFLAYER_DOMINATION, SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE,
	   6, 15, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 3, 11, &CMindflayer::Domination, 0));
   m_SpellTable.Add("Psionic Leech", new CSpell<CMindflayer>("Psionic Leech", "&LPsionic Leech&n", MINDFLAYER_PSIONIC_LEECH, AFFECT_PSIONIC_LEECH, AREA_SELF,
	   1, 10, CMudTime::PULSES_PER_MUD_HOUR, (CMudTime::PULSES_PER_MUD_HOUR >> 1), 0, 6, 26, &CMindflayer::SpellAffectAdd, 0));

   POSITION pos = m_SpellTable.GetStartingPosition();
   bool bReturn = false;
   CSpell<CMindflayer> *pCurrentSpell;
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

CMindflayer::CMindflayer(CCharIntermediate *ch) : CThinker<CMindflayer>(ch), CCharacter(ch) {
   InitStatics();
   //can't put in CCharacter construct because
   //it will be called before us
   if (IsNewbie()) {
	InitSkills();
	m_nSkillSphere |= GENERAL;
   }
}

CMindflayer::CMindflayer(CMobPrototype &mob, CRoom *pPutInRoom)
: CThinker<CMindflayer>(mob, pPutInRoom), CCharacter(mob, pPutInRoom) {
   InitStatics();
   //everyone gets general
   m_nSkillSphere |= GENERAL;
}

CMindflayer::~CMindflayer() {
}

/////////////////////////////
//	Sets up the skills a warrior can learn
//	

void CMindflayer::InitSkillInfo() { /*max,min,pct*/

   // Base Skills
   m_SkillInfo[SKILL_TRACK] = CSkillInfo(GENERAL, 50, 0, 2);
   m_SkillInfo[SKILL_BODYSLAM] = CSkillInfo(GENERAL, 50, 1, 1);
   m_SkillInfo[SKILL_BANDAGE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_BASH] = CSkillInfo(GENERAL, 25, 0, 1);
   m_SkillInfo[SKILL_DODGE] = CSkillInfo(GENERAL, 100, 1, 1);
   m_SkillInfo[SKILL_DUAL_WIELD] = CSkillInfo(GENERAL, 75, 0, 1);
   m_SkillInfo[SKILL_OFFENSE] = CSkillInfo(GENERAL, 100, 1, 1);
   m_SkillInfo[SKILL_FIRST_AID] = CSkillInfo(GENERAL, 100, 1, 1);
   m_SkillInfo[SKILL_MOUNT] = CSkillInfo(GENERAL, 100, 1, 1);
   m_SkillInfo[SKILL_BACKSTAB] = CSkillInfo(GENERAL, 50, 0, 1);
   m_SkillInfo[SKILL_TUNDRA_MAGIC] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_DESERT_MAGIC] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_MOUNTAIN_MAGIC] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_FOREST_MAGIC] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_SWAMP_MAGIC] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_FORAGE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_RESCUE] = CSkillInfo(GENERAL, 75, 1, 1);
   m_SkillInfo[SKILL_SWITCH] = CSkillInfo(GENERAL, 100, 1, 1);
   m_SkillInfo[SKILL_TRIP] = CSkillInfo(GENERAL, 50, 0, 1);
   m_SkillInfo[SKILL_KICK] = CSkillInfo(GENERAL, 50, 0, 1);
   m_SkillInfo[SKILL_CLERIC_MAGIC] = CSkillInfo(GENERAL, 50, 0, 1);
   m_SkillInfo[SKILL_SHAMAN_MAGIC] = CSkillInfo(GENERAL, 75, 0, 1);
   m_SkillInfo[SKILL_BLIND_FIGHTING] = CSkillInfo(GENERAL, 50, 1, 10);

   m_SkillInfo[SKILL_THROW_WEAPON] = CSkillInfo(GENERAL, 50, 0, 1);
   m_SkillInfo[SKILL_MEDITATE] = CSkillInfo(GENERAL, 100, 1, 5);
   m_SkillInfo[SKILL_QUICK_CHANT] = CSkillInfo(GENERAL, 100, 0, 5);

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

const CSkillInfo* CMindflayer::GetSkillInfo(short nSkill) {
   return &m_SkillInfo[nSkill];
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////

short CMindflayer::GetThaco() {
   return m_pThaco[m_nLevel];
}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////

short CMindflayer::GetMaxHitPointsPerLevel() {
   return 4;
}
/////////////////////////////
//	Not really needed for real OS
//	but we'll be friendly to 95/98 =)
//////////////////////////////

void CMindflayer::CleanNewedStaticMemory() {
   POSITION pos = m_SpellTable.GetStartingPosition();
   while (pos) {
	delete m_SpellTable.GetNext(pos);
   }
}

///////////////////////////
//	GetManaAdjust overridden
//	returns POW adjustment

short CMindflayer::GetManaAdjust() {
   return m_pAttributes->GetManaAdjustPow();
}

///////////////////////////
//	returns exp of level passed in
///////////////////////////

int CMindflayer::GetExp(short nLvl) {
   return m_Experience[nLvl];
}


///////////////////////////
//	Ruse -> Same as Cantrip
//
//	John Comes 12-08-98
///////////////////////////

void CMindflayer::Ruse(const CInterp *pInterp, const CSpell<CMindflayer> *pSpell) {
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

void CMindflayer::Confusion(const CInterp *pInterp, const CSpell<CMindflayer> *pSpell) {
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

void CMindflayer::SelfSacrifice(const CInterp *pInterp, const CSpell<CMindflayer> *pSpell) {
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


////////////////////////////////
//  SpellAffectAdd
//  Sets Timing for just spells that add an affect
//	John Comes 11/10/99
///////////////////////////////

void CMindflayer::SpellAffectAdd(const CInterp * pInterp, const CSpell<CMindflayer> * pSpell) {
   short nDam = CalcSpellDam(pSpell);
   switch (pSpell->GetSpellID()) {
	case MINDFLAYER_TRANSPARENT_FORM:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, 0);
	   break;
	case MINDFLAYER_AIR_WALK:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, 0);
	   break;
	case MINDFLAYER_CLEAR_SIGHT:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, 0);
	   break;
	case MINDFLAYER_BATTLE_READY: case MINDFLAYER_PHYSICAL_ENHANCEMENT:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), nDam, (m_Spells[pSpell->GetSpellID()] / 15) + 1);
	   break;
	case MINDFLAYER_INERTIAL_BARRIER:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), ((m_Spells[pSpell->GetSpellID()]*100) + m_nLevel));
	   break;
	case MINDFLAYER_PSIONIC_LEECH:
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), ((m_Spells[pSpell->GetSpellID()]*100) + m_nLevel));
	   break;
	default:
	   //default does not use SpellMsgs
	   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), 0);
	   break;
   }
}

/////////////////////////
//	Wormhole
//
//	Portal Spell
//	John Comes 12-9-98
/////////////////////////

void CMindflayer::Wormhole(const CInterp *pInterp, const CSpell<CMindflayer> *pSpell) {
   //can't worm to mobs
   CCharacter *pTarget;
   pTarget = GVM.FindCharacter(m_CurrentSpell.m_strTarget, this);

   if (pTarget == NULL
	   || pTarget->IsNPC()
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

void CMindflayer::PsionicBlast(const CInterp *pInterp, const CSpell<CMindflayer> *pSpell) {
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



///////////////////////////////
// Shards
// Sets Timing for Shards, (Berserk)
// Mark Henry 2-20-1999
///////////////////////////////

void CMindflayer::Shards(const CInterp * pInterp, const CSpell<CMindflayer> * pSpell) {
   m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(), CalcSpellDam(pSpell), 0);
   m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, m_CurrentSpell.m_pTarget);
}


//////////////////////////
//	SendSpellList to char

void CMindflayer::SendSpellList() {
   SendToChar(m_strSpellList);
}

///////////////////////
//	Domination
//	Mind Flayer spell to make a mob yours
//
//	
//	John Comes 12-3-99
///////////////////////

void CMindflayer::Domination(const CInterp * pInterp, const CSpell<CMindflayer> *pSpell) {
   //has to be level <= 30 (from spell init) or under your level
   if (m_CurrentSpell.m_pTarget->GetLevel() < LVL_CAN_BE_DOMINATED && m_CurrentSpell.m_pTarget->GetLevel() < GetLevel()
	   && (m_CurrentSpell.m_pTarget->IsNPC() || !m_CurrentSpell.m_pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL))) {
	if (this->CurrentlyMasterOf() < MAX_CAN_BE_DOMINATED) {
	   m_CurrentSpell.m_pTarget->SetMaster(this, this->CalcSpellDam(pSpell));
	} else {
	   CString str;
	   str.Format("%r%s resists your magical hold.\r\nYou do not control %s.&n\r\n",
		   m_CurrentSpell.m_pTarget->GetName().cptr(), m_CurrentSpell.m_pTarget->GetName().cptr());
	   SendToChar(str);
	}
   } else {
	this->SendToChar("You can't seem to get control of such a powerful mind!\r\n");
	AddFighters(m_CurrentSpell.m_pTarget, false);
   }
}

////////////////////////
//SendPRoficiency layout

void CMindflayer::SendProficiency() {
   CString strProf;
   strProf.Format("Current Proficiency Bonus: %d\r\n%s",
	   m_Proficiency.CalcBonus(m_nSkillSphere),
	   m_strProficiency.cptr());
   SendCharPage(strProf);
}

const CSpell<CMindflayer> *CMindflayer::GetSpellInfo(CString strSpell) const {
   CSpell<CMindflayer> *pSpell;
   if (!m_SpellTable.Lookup(strSpell, pSpell)) {
	return NULL;
   }
   return pSpell;
}

short CMindflayer::GetPctSpellUp(const CSpell<CMindflayer> *pSpell) const {
   return m_pAttributes->GetStat(POW) / 5;
}
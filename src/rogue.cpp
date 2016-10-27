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
 *  Rogue implementation
 *
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "rogue.h"

CSkillInfo CRogue::m_SkillInfo[MAX_MUD_SKILLS];
bool CRogue::StaticsInitialized = false;
const int CRogue::GENERAL = (1 << 0);
const int CRogue::ROGUE = (1 << 1);
const int CRogue::ENTERTAINER = (1 << 2);
const int CRogue::THIEF = (1 << 3);
const int CRogue::BRIGAND = (1 << 4);
const int CRogue::BARD = (1 << 5);
const int CRogue::ACROBAT = (1 << 6);
const int CRogue::CUTPURSE = (1 << 7);
const int CRogue::SWINDLER = (1 << 8);
const int CRogue::ASSASSIN = (1 << 9);
const int CRogue::NINJA = (1 << 10);
const int CRogue::MERCENARY = (1 << 11);
const int CRogue::SCOUT = (1 << 12);
CProficiency CRogue::m_Proficiency;
CString CRogue::m_strProficiency("You class has the following proficiencies:\r\n");

const short int CRogue::m_pThaco[] ={
   /*1,02,03,04,05,06,07,08,09,10*/
   100, 98, 96, 94, 92, 90, 88, 86, 84, 82,
   80, 78, 76, 74, 72, 70, 68, 66, 64, 62,
   60, 58, 56, 54, 52, 50, 47, 43, 40, 37,
   34, 30, 25, 20, 15, 10, 5, 0, 0, 0,
   -2, -4, -6, -8, -10, -12, -14, -16, -18, -20,
   -50, -50, -50, -50, -50, -50, -50, -50, -50, -50 /*51-60*/
};

const int CRogue::m_Experience[LVL_IMP + 1] ={
   /*0*/ 0, /*1*/ 1, /*2*/ 175, /*3*/ 525,
   /*4*/ 1050, /*5*/ 1750, /*6*/ 2625, /*7*/ 3675,
   /*8*/ 4900, /*9*/ 6300, /*10*/ 7875, /*11*/ 9625,
   /*12*/ 13475, /*13*/ 17675, /*14*/ 22225, /*15*/ 27125,
   /*16*/ 32375, /*17*/ 37975, /*18*/ 43925, /*19*/ 50225,
   /*20*/ 56875, /*21*/ 63875, /*22*/ 78575, /*23*/ 93975,
   /*24*/ 110075, /*25*/ 126875, /*26*/ 144375, /*27*/ 162575,
   /*28*/ 181475, /*29*/ 201075, /*30*/ 221375, /*31*/ 263375,
   /*32*/ 306775, /*33*/ 351575, /*34*/ 397775, /*35*/ 445375,
   /*36*/ 494375, /*37*/ 544775, /*38*/ 596575, /*39*/ 649775,
   /*40*/ 704375, /*41*/ 816375, /*42*/ 931175, /*43*/ 1048775,
   /*44*/ 1169175, /*45*/ 1292375, /*46*/ 1418375, /*47*/ 1547175,
   /*48*/ 1678775, /*49*/ 1813175, /*50*/ 1950375, /*51*/ 2090375,
   /*52*/ 2375975, /*53*/ 2667175, /*54*/ 2963975, /*55*/ 3266375,
   /*56*/ 3574375, /*57*/ 3887975, /*58*/ 4207175, /*59*/ 4531975,
   /*60*/ 4862375
};

/////////////////////////////
//	Sets up the skills a warrior can learn
//	

void CRogue::InitSkillInfo() { /*max,min,pct*/
   if (StaticsInitialized) {
	return;
   }
   StaticsInitialized = true;

   // Base Skills
   m_SkillInfo[SKILL_BODYSLAM] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_BANDAGE] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_BASH] = CSkillInfo(GENERAL, 75, 0, 50);
   m_SkillInfo[SKILL_DODGE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_DUAL_WIELD] = CSkillInfo(GENERAL, 90, 1, 10);
   m_SkillInfo[SKILL_OFFENSE] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_FIRST_AID] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_MOUNT] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_BACKSTAB] = CSkillInfo(GENERAL, 100, 10, 30);
   m_SkillInfo[SKILL_TUNDRA_MAGIC] = CSkillInfo(GENERAL, 75, 0, 10);
   m_SkillInfo[SKILL_DESERT_MAGIC] = CSkillInfo(GENERAL, 75, 0, 10);
   m_SkillInfo[SKILL_MOUNTAIN_MAGIC] = CSkillInfo(GENERAL, 75, 0, 10);
   m_SkillInfo[SKILL_FOREST_MAGIC] = CSkillInfo(GENERAL, 75, 0, 10);
   m_SkillInfo[SKILL_SWAMP_MAGIC] = CSkillInfo(GENERAL, 75, 0, 10);
   m_SkillInfo[SKILL_FORAGE] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_RESCUE] = CSkillInfo(GENERAL, 100, 1, 20);
   m_SkillInfo[SKILL_SWITCH] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_TRIP] = CSkillInfo(GENERAL, 100, 1, 30);
   m_SkillInfo[SKILL_KICK] = CSkillInfo(GENERAL, 100, 1, 30);
   m_SkillInfo[SKILL_CLERIC_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_SHAMAN_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_BLIND_FIGHTING] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_HIDE] = CSkillInfo(GENERAL, 99, 1, 50);
   m_SkillInfo[SKILL_SNEAK] = CSkillInfo(GENERAL, 99, 1, 50);

   m_SkillInfo[SKILL_THROW_WEAPON] = CSkillInfo(GENERAL, 100, 10, 10);
   m_SkillInfo[SKILL_RANGED_WEAPONS] = CSkillInfo(GENERAL, 100, 0, 20);
   m_SkillInfo[SKILL_DOUBLE_BACKSTAB] = CSkillInfo(ROGUE, 100, 5, 30);
   m_SkillInfo[SKILL_DIRT_TOSS] = CSkillInfo(ROGUE, 100, 1, 50);
   m_SkillInfo[SKILL_LOOTING] = CSkillInfo(ROGUE, 100, 1, 150);
   m_SkillInfo[SKILL_CIRCLE] = CSkillInfo(ROGUE, 100, 1, 10);
   m_SkillInfo[SKILL_SING] = CSkillInfo(ENTERTAINER, 100, 0, 10);
   m_SkillInfo[SKILL_DANCE] = CSkillInfo(ENTERTAINER, 100, 0, 10);
   m_SkillInfo[SKILL_INSTRUMENT_PLAYING] = CSkillInfo(ENTERTAINER, 100, 0, 10);
   m_SkillInfo[SKILL_SONG_HEALING] = CSkillInfo(BARD, 100, 0, 10);
   m_SkillInfo[SKILL_SONG_CHARMING] = CSkillInfo(BARD, 100, 0, 10);
   m_SkillInfo[SKILL_SONG_HURTING] = CSkillInfo(BARD, 100, 0, 10);
   m_SkillInfo[SKILL_SONG_HEROISM] = CSkillInfo(BARD, 100, 0, 10);
   m_SkillInfo[SKILL_SONG_REVELATION] = CSkillInfo(BARD, 100, 0, 10);
   m_SkillInfo[SKILL_SONG_FLIGHT] = CSkillInfo(BARD, 100, 0, 10);
   m_SkillInfo[SKILL_JUGGLING] = CSkillInfo(ACROBAT, 100, 0, 30);
   m_SkillInfo[SKILL_TUMBLING] = CSkillInfo(ACROBAT, 100, 0, 30);
   m_SkillInfo[SKILL_JUMPING] = CSkillInfo(ACROBAT, 100, 0, 30);
   m_SkillInfo[SKILL_CLIMB_WALLS] = CSkillInfo(ACROBAT, 100, 0, 30);
   m_SkillInfo[SKILL_DISGUISE] = CSkillInfo(THIEF, 100, 0, 20);
   m_SkillInfo[SKILL_PICK_LOCKS] = CSkillInfo(THIEF, 100, 0, 20);
   m_SkillInfo[SKILL_STEAL] = CSkillInfo(CUTPURSE, 100, 0, 20);
   m_SkillInfo[SKILL_BEG] = CSkillInfo(CUTPURSE, 100, 0, 20);
   m_SkillInfo[SKILL_FORGERY] = CSkillInfo(CUTPURSE, 100, 0, 50);
   m_SkillInfo[SKILL_APPRAISE] = CSkillInfo(CUTPURSE, 100, 0, 30);
   m_SkillInfo[SKILL_GAMBLE] = CSkillInfo(CUTPURSE, 100, 0, 30);
   m_SkillInfo[SKILL_CHARM] = CSkillInfo(SWINDLER, 100, 0, 10);
   m_SkillInfo[SKILL_SUBTERFUGE] = CSkillInfo(SWINDLER, 100, 0, 20);
   m_SkillInfo[SKILL_INTIMIDATE] = CSkillInfo(SWINDLER, 100, 0, 20);
   m_SkillInfo[SKILL_BLITHER] = CSkillInfo(SWINDLER, 100, 0, 20);
   m_SkillInfo[SKILL_BRIBE] = CSkillInfo(SWINDLER, 100, 0, 20);
   m_SkillInfo[SKILL_TRACK] = CSkillInfo(BRIGAND, 100, 0, 90);
   m_SkillInfo[SKILL_SHADOW] = CSkillInfo(BRIGAND, 100, 0, 20);
   m_SkillInfo[SKILL_KNIFE_TURN] = CSkillInfo(ASSASSIN, 100, 0, 20);
   m_SkillInfo[SKILL_APPLY_POISON] = CSkillInfo(ASSASSIN, 100, 0, 20);
   m_SkillInfo[SKILL_THROAT_SLIT] = CSkillInfo(ASSASSIN, 100, 0, 20);
   m_SkillInfo[SKILL_CAMOUFLAGE] = CSkillInfo(ASSASSIN, 100, 0, 20);
   m_SkillInfo[SKILL_INSTANT_KILL] = CSkillInfo(ASSASSIN, 100, 0, 10);
   m_SkillInfo[SKILL_BARE_HAND_FIGHTING] = CSkillInfo(NINJA, 100, 0, 20);
   m_SkillInfo[SKILL_CHANT] = CSkillInfo(NINJA, 100, 0, 20);
   m_SkillInfo[SKILL_QUICK_STRIKE] = CSkillInfo(NINJA, 100, 0, 10);
   m_SkillInfo[SKILL_DOUBLE_ATTACK] = CSkillInfo(NINJA, 100, 0, 20);
   m_SkillInfo[SKILL_HEAD_BUTT] = CSkillInfo(MERCENARY, 100, 0, 20);
   m_SkillInfo[SKILL_CAPTURE] = CSkillInfo(MERCENARY, 100, 0, 20);
   m_SkillInfo[SKILL_RIPOSTE] = CSkillInfo(MERCENARY, 100, 0, 20);
   m_SkillInfo[SKILL_PARRY] = CSkillInfo(MERCENARY, 100, 0, 20);
   m_SkillInfo[SKILL_TRAP] = CSkillInfo(SCOUT, 100, 0, 20);
   m_SkillInfo[SKILL_OBSERVATION] = CSkillInfo(SCOUT, 100, 0, 20);
   m_SkillInfo[SKILL_FAR_SIGHT] = CSkillInfo(SCOUT, 100, 0, 20);
   m_SkillInfo[SKILL_LISTEN] = CSkillInfo(SCOUT, 100, 0, 20);
   m_SkillInfo[SKILL_DIVERSION] = CSkillInfo(SCOUT, 100, 0, 20);


   //Add(CString strName, short nTier, short nBonus, short nPenatly, UINT nValue, UINT nParentValue)
   m_Proficiency.Add("General", 0, 0, 0, GENERAL, 0);
   m_Proficiency.Add("Rogue", 1, 1, 1, ROGUE, GENERAL);
   m_Proficiency.Add("Entertainer", 2, 1, 1, ENTERTAINER, ROGUE);
   m_Proficiency.Add("Thief", 2, 1, 1, THIEF, ROGUE);
   m_Proficiency.Add("Brigand", 2, 1, 1, BRIGAND, ROGUE);
   m_Proficiency.Add("Bard", 3, 1, 1, BARD, ENTERTAINER);
   m_Proficiency.Add("Acrobat", 3, 1, 1, ACROBAT, ENTERTAINER);
   m_Proficiency.Add("Cutpurse", 3, 1, 1, CUTPURSE, THIEF);
   m_Proficiency.Add("Swindler", 3, 1, 1, SWINDLER, THIEF);
   m_Proficiency.Add("Assassin", 3, 1, 1, ASSASSIN, BRIGAND);
   m_Proficiency.Add("Ninja", 3, 1, 1, NINJA, BRIGAND);
   m_Proficiency.Add("Mercenary", 3, 1, 1, MERCENARY, BRIGAND);
   m_Proficiency.Add("Scout", 3, 1, 1, SCOUT, BRIGAND);

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

CRogue::~CRogue() {

}

CRogue::CRogue(CCharIntermediate *ch) : CCharacter(ch) {
   InitSkillInfo();
   //can't put in CCharacter construct because
   //it will be called before us
   if (IsNewbie()) {
	InitSkills();
	m_nSkillSphere |= ROGUE | GENERAL;
   }
}

CRogue::CRogue(CMobPrototype &mob, CRoom *pPutInRoom)
: CCharacter(mob, pPutInRoom) {
   InitSkillInfo();
   //everyone gets general
   m_nSkillSphere |= GENERAL | ROGUE;
}

const CSkillInfo* CRogue::GetSkillInfo(short nSkill) {
   return &m_SkillInfo[nSkill];
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////

short CRogue::GetThaco() {
   return m_pThaco[m_nLevel];
}

short CRogue::GetMaxHitPointsPerLevel() {
   return 8;
}

///////////////////////////////////////////
// Backstab
// Written by John Comes
// 8/9/98
///////////////////////////////////////////

void CRogue::DoBackStab(const CInterp *pInterp, CCharacter *pTarget) {
   if (pTarget == NULL) {
	SendToChar("Backstab who?\r\n");
   } else if (pTarget == this) {
	SendToChar("How do you plan to get behind yourself?\r\n");
   } else {
	CString strToChar, strToRoom, strWeaponName;
	int StabDmg, nDieRoll = 100, nSkill;
	bool bIsDouble = false;
	bool bHadPiercer = false;
	bool bGotTurn;
	for (int i = 0; i < MAX_EQ_POS; i++) {
	   if (!m_Equipment[i].IsEmpty() && m_Equipment[i]->IsWeapon()
		   && m_Equipment[i]->GetMessageType() == PIERCE) {
		bHadPiercer = true;
		//it's harder if target is fighting.
		nSkill = GetSkill((bIsDouble ? SKILL_DOUBLE_BACKSTAB : SKILL_BACKSTAB));
		if (pTarget->IsFighting()) {
		   nSkill -= DIE(20) + 10;
		}
		strWeaponName = m_Equipment[i].GetObject()->GetObjName();
		if (DIE(nDieRoll) <= nSkill) {
		   if (pTarget->MakeSkillSave(SKILL_BACKSTAB, 0)) {
			m_MsgManager.SkillSaveMsg(pTarget, this, "backstab");
			SkillUp(bIsDouble ? SKILL_DOUBLE_BACKSTAB : SKILL_BACKSTAB);
			pTarget->Defense(CalcACWouldHit(), i, this);
			if (pTarget->IsDead()) {
			   pInterp->KillChar(pTarget);
			   i = MAX_EQ_POS;
			}
		   } else {
			StabDmg = ((GetSkill((bIsDouble ? SKILL_DOUBLE_BACKSTAB : SKILL_BACKSTAB)) / 25) + 1) * CalcDamage(i, pTarget);
			bGotTurn = false;
			//Check to see if I get a knife turn
			if (GetSkill(SKILL_KNIFE_TURN)) {
			   if (DIE(131) <= GetSkill(SKILL_KNIFE_TURN) + m_Proficiency.CalcBonus(m_nSkillSphere) * 10) { //if I turn, StabDamage is * 1.5.
				StabDmg = StabDmg * 150 / 100;
				;
				bGotTurn = true;
			   } else {
				SkillUp(SKILL_KNIFE_TURN);
			   }
			}
			pTarget->TakeDamage(this, StabDmg, true);
			if (!pTarget->IsDead()) {
			   bIsDouble = true;
			   //Send Stab message
			   strToChar.Format("%s's body racks with &Rpain&n from a well placed %s in %s's back.\r\n", pTarget->GetRaceOrName(this).cptr(), strWeaponName.cptr(), pTarget->GetPronoun(pTarget->GetSex()));
			   SendToChar(strToChar);
			   strToChar.Format("From out of nowhere %s places %s in YOUR back!\r\n", GetRaceOrName(pTarget).cptr(), strWeaponName.cptr());
			   pTarget->SendToChar(strToChar);
			   strToRoom = "%s's body racks with &Rpain&n from as %s places ";
			   strToChar.Format("%s in %s back.\r\n", strWeaponName.cptr(), pTarget->GetPronoun(pTarget->GetSex()));
			   GetRoom()->SendToRoom(strToRoom += strToChar, pTarget, this);
			   if (bGotTurn) {
				m_MsgManager.SkillMsg(this, pTarget, SKILL_KNIFE_TURN, SKILL_WORKED);
			   }
			   //Check for instant kill
			   if (DIE(20000 - m_Proficiency.CalcBonus(m_nSkillSphere) * 30) <= GetSkill(SKILL_INSTANT_KILL)
				   && ((pTarget->GetCurrentHpts()*100) / pTarget->GetMaxHits()) > 90) {
				strToChar.Format("You &RINSTANTLY KILL&n %s!\r\n", pTarget->GetRaceOrName(this).cptr());
				SendToChar(strToChar);
				strToChar.Format("You are &RINSTANTLY KILLED&n by %s!\r\n", GetRaceOrName(pTarget).cptr());
				pTarget->SendToChar(strToChar);
				strToRoom = "%s &RINSTANTLY KILLS&n %s!\r\n";
				GetRoom()->SendToRoom(strToRoom, this, pTarget);
				pInterp->KillChar(pTarget);
				i = MAX_EQ_POS;
				LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * 3);
				return;
			   } else {
				SkillUp(SKILL_INSTANT_KILL);
			   }
			} else {
			   strToChar.Format("%s is suddenly very silent as you place %s in %s back.\r\n", pTarget->GetRaceOrName(this).cptr(), strWeaponName.cptr(), pTarget->GetPronoun(pTarget->GetSex()));
			   SendToChar(strToChar);
			   strToChar.Format("From out of nowhere %s places %s in YOUR back, it is the last thing you ever feel.\r\n", GetRaceOrName(pTarget).cptr(), strWeaponName.cptr());
			   pTarget->SendToChar(strToChar);
			   strToRoom = "%s makes a strange sound as %s places ";
			   strToChar.Format("%s in thier back, making a strange sound, a lot of blood and a corpse.\r\n", strWeaponName.cptr());
			   GetRoom()->SendToRoom(strToRoom += strToChar, pTarget, this);
			   pInterp->KillChar(pTarget);
			   i = MAX_EQ_POS;
			   LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * 2);
			   return;
			}
		   }
		} else {
		   SkillUp(bIsDouble ? SKILL_DOUBLE_BACKSTAB : SKILL_BACKSTAB);
		   pTarget->Defense(CalcACWouldHit(), i, this);
		   if (pTarget->IsDead()) {
			pInterp->KillChar(pTarget);
			i = MAX_EQ_POS;
		   }
		}
	   }
	}
	if (!bHadPiercer) {
	   SendToChar("Try a piercing weapon next time fool.\r\n");
	} else {
	   AddFighters(pTarget);
	}
	LagChar(CMudTime::PULSES_PER_BATTLE_ROUND);
   }
}

//
// ThroatSlit
// Written by John Comes
// 10/7/98

void CRogue::DoThroatSlit(const CInterp *pInterp, CCharacter *pTarget) {
   CString strToChar, strToRoom;
   if (!IsAffectedBy(CCharacter::AFFECT_HIDING)) {
	SendToChar("You have to hide first, they'll notice you too easy otherwise!\r\n");
	return;
   }
   if (!pTarget) {
	SendToChar("Slice who's throat?\r\n");
	return;
   }

   int StabDmg;
   bool bHadWeapon = false;
   for (int i = 0; i < MAX_EQ_POS; i++) {
	if (!m_Equipment[i].IsEmpty() && m_Equipment[i]->IsWeapon()
		&& (m_Equipment[i]->GetMessageType() == PIERCE || m_Equipment[i]->GetMessageType() == SLASH)) {
	   bHadWeapon = true;
	   if ((short int) DIE(100 - m_Proficiency.CalcBonus(m_nSkillSphere) * 10) <= GetSkill(SKILL_THROAT_SLIT)) {
		StabDmg = GetSkill(SKILL_THROAT_SLIT) / 10 * CalcDamage(i, pTarget);
		pTarget->TakeDamage(this, StabDmg, true);
		if (!pTarget->IsDead()) {
		   m_MsgManager.SkillMsg(this, pTarget, SKILL_THROAT_SLIT, SKILL_WORKED);
		   if ((short int) DIE(18000) <= GetSkill(SKILL_INSTANT_KILL)) {
			strToChar.Format("You &RINSTANTLY KILL&n %s!\r\n", pTarget->GetRaceOrName(this).cptr());
			SendToChar(strToChar);
			strToChar.Format("You are &RINSTANTLY KILLED&n by %s!\r\n", GetRaceOrName(pTarget).cptr());
			pTarget->SendToChar(strToChar);
			strToRoom = "%s &RINSTANTLY KILLS&n %s!\r\n";
			GetRoom()->SendToRoom(strToRoom, this, pTarget);
			pInterp->KillChar(pTarget);
			LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * 4);
			RemoveAffect(CCharacter::AFFECT_HIDING);
			return;
		   } else {
			SkillUp(SKILL_INSTANT_KILL);
			AddFighters(pTarget);
		   }
		} else {
		   m_MsgManager.SkillMsg(this, pTarget, SKILL_THROAT_SLIT, SKILL_KILLED);
		   pInterp->KillChar(pTarget);
		   RemoveAffect(CCharacter::AFFECT_HIDING);
		   return;
		}
	   } else {
		SkillUp(SKILL_THROAT_SLIT);
		m_MsgManager.SkillMsg(this, pTarget, SKILL_THROAT_SLIT, SKILL_DIDNT_WORK);
		Attack(true);
		AddFighters(pTarget);
	   }
	   LagChar(CMudTime::PULSES_PER_REAL_SECOND * 4);
	   RemoveAffect(CCharacter::AFFECT_HIDING);
	   return;
	}
   }
   if (!bHadWeapon) {
	strToChar = "Wield a real weapon stupid.\r\n";
	SendToChar(strToChar);
	return;
   }
   LagChar(CMudTime::PULSES_PER_REAL_SECOND * 4);
}

////
////  Beg
////	By John Comes
////

void CRogue::DoBeg() {
   CCharacter *pTarget = GetTarget();
   if (!pTarget) {
	SendToChar("Beg who?\r\n");
	return;
   }
   CString strBegMsg;
   if (!pTarget->IsNPC()) {
	strBegMsg = "Only used on NPC atm.\r\n";
	SendToChar(strBegMsg);
	return;
   }
   strBegMsg = "%s looks poor and wretched begging %s.\r\n";
   GetRoom()->SendToRoom(strBegMsg, this, pTarget);
   CString strCommand;
   if (DIE(100) <= (GetSkill(SKILL_BEG) + m_Proficiency.CalcBonus(m_nSkillSphere) * 10)) {
	strCommand.Format("give coins %s", this->GetName().cptr());
	pTarget->AddCommand(strCommand);
	strBegMsg.Format("%s says 'Here you go your poor soul.'\r\n", pTarget->GetName().cptr());
	SendToChar(strBegMsg);
   }
   else {
	SkillUp(SKILL_BEG);
	if (DIE(100) <= 10) {
	   strBegMsg.Format("%s exclaims 'Worthless fool! I will rid this area of your filth!\r\n'", pTarget->GetName().cptr());
	   GetRoom()->SendToRoom(strBegMsg);
	   strCommand.Format("kill %s", this->GetName().cptr());
	   pTarget->AddCommand(strCommand);
	} else {
	   strBegMsg.Format("%s says 'Beg someone else, fool!'\r\n", pTarget->GetName().cptr());
	   GetRoom()->SendToRoom(strBegMsg);
	}
   }
   LagChar(CMudTime::PULSES_PER_REAL_SECOND * 3);
}

////
////  Charm
////	By John Comes
////

void CRogue::DoCharm() {
   CCharacter *pTarget = GetTarget();
   if (!pTarget || pTarget == this) {
	SendToChar("Charm who?\r\n");
	return;
   }
   CString strConMsg, strToChar;
   if (!pTarget->IsNPC()) {
	strConMsg = "Only used on NPC atm.\r\n";
	SendToChar(strConMsg);
	return;
   }
   strConMsg = "%s turns on the charm while talking to %s.\r\n";
   GetRoom()->SendToRoom(strConMsg, this, pTarget);
   short int Con;
   Con = DIE(100);
   if (Con <= (GetSkill(SKILL_CHARM) - (pTarget->GetLevel() - GetLevel()) + m_Proficiency.CalcBonus(m_nSkillSphere) * 10)) {
	pTarget->SetMaster(this, GetSkill(SKILL_CHARM) * CMudTime::PULSES_PER_MUD_HOUR);
   } else if (Con < 100 - GetSkill(SKILL_CHARM)) {
	strConMsg = "%s says 'You miserable swindler, &RDIE!&n\r\n";
	GetRoom()->SendToRoom(strConMsg, pTarget);
	strToChar.Format("kill %s", this->GetName().cptr());
	pTarget->AddCommand(strToChar);
   } else {

	strConMsg = "%s just ignores %s.\r\n";
	GetRoom()->SendToRoom(strConMsg, pTarget, this);
	strConMsg.Format("%s just ignores you.\r\n", pTarget->GetRaceOrName(this).cptr());
	SendToChar(strConMsg);
   }
   SkillUp(SKILL_CHARM);
   LagChar(CMudTime::PULSES_PER_REAL_SECOND * 4);
}

//
// Circle
// Written by John Comes
// 9/18/98

void CRogue::DoCircle(const CInterp *pInterp) {
   if (IsFighting() == NULL) {
	SendToChar("You must be fighting to circle!\r\n");
	return;
   }
   CCharacter *pTarget = IsFighting();
   if (IsFighting()->IsFighting() == this) {
	SendToChar("You can't circle while you're the tank, dummy!\r\n");
	return;
   }
   if (DIE(100) <= GetSkill(SKILL_CIRCLE)) {
	for (int i = 0; i < MAX_EQ_POS; i++) {
	   if (!m_Equipment[i].IsEmpty() && m_Equipment[i]->IsWeapon()
		   && m_Equipment[i]->GetMessageType() == PIERCE) {
		DoBackStab(pInterp, this->IsFighting());
	   } else if (!m_Equipment[i].IsEmpty() && m_Equipment[i]->IsWeapon()) {
		Attack(true);
		this->LagChar(CMudTime::PULSES_PER_BATTLE_ROUND);
	   }
	   i = MAX_EQ_POS;
	}
   } else {
	SkillUp(SKILL_CIRCLE);
	Attack(true);
	this->LagChar(CMudTime::PULSES_PER_BATTLE_ROUND);
   }
}



//
// Sneak
// Written by John Comes
// 10/1/98

void CRogue::DoSneak(const CInterp *pInterp) {
   CString strTarget(CurrentCommand.GetWordAfter(1));
   CString strToChar;
   if (strTarget.Compare("off")) {
	strToChar = "You stop trying to &Lsneak&n.\r\n";
	SendToChar(strToChar);
	RemoveAffect(CCharacter::AFFECT_SNEAKING);
	return;
   }

   strToChar = "You try to walk &csilently&n.\r\n";
   SendToChar(strToChar);
   if (DIE(100) <= GetSkill(SKILL_SNEAK)) {
	AddAffect(CCharacter::AFFECT_SNEAKING, sAffect::MANUAL_AFFECT);
   } else {
	SkillUp(SKILL_SNEAK);
	LagChar(CMudTime::PULSES_PER_REAL_SECOND);
   }
}

//
// Hide
// Written by John Comes
// 10/1/98

void CRogue::DoHide() {
   CString strObj(CurrentCommand.GetWordAfter(1));
   if (strObj.IsEmpty()) {
	CString strToChar;
	strToChar = "You try to &Lhide&n yourself.\r\n";
	SendToChar(strToChar);
	if (DIE(100) <= GetSkill(SKILL_HIDE)) {
	   SendToChar("You are hidden.\r\n");
	   AddAffect(CCharacter::AFFECT_HIDING, sAffect::MANUAL_AFFECT);
	}
	SkillUp(SKILL_HIDE);
	LagChar(CMudTime::PULSES_PER_REAL_SECOND * 3);
   } else {
	CCharacter::DoHide();
   }
}


////////////////////
//	getexp
//	returns exp for level passed in
////////////////////

int CRogue::GetExp(short nLvl) {
   return m_Experience[nLvl];
}

void CRogue::SendProficiency() {
   CString strProf;
   strProf.Format("Current Proficiency Bonus: %d\r\n%s",
	   m_Proficiency.CalcBonus(m_nSkillSphere),
	   m_strProficiency.cptr());
   SendCharPage(strProf);
}

//////////////////////
//	Appraise
//
//	John Comes 3/1/1999

void CRogue::DoAppraise() {
   if (!GetSkill(SKILL_APPRAISE)) {
	SendToChar("Learn how first!\r\n");
	return;
   }
   CString strItem(CurrentCommand.GetWordAfter(1));
   CObject *pObj;
   int nDot, nNum = 1;
   if ((nDot = strItem.Find('.')) != -1) {
	nNum = strItem.Left(nDot).MakeInt();
	strItem = strItem.Right(nDot);
   }
   if ((pObj = FindInInventory(strItem, nNum))) {
	CString strToChar;
	if (GetSkill(SKILL_APPRAISE) * 50 > pObj->GetCost()) {
	   strToChar.Format("This is worth about %d &ycopper&n pieces.\r\n", pObj->GetCost());
	   SendToChar(strToChar);
	} else {
	   SendToChar("You can't seem to gleam any information off this.\r\n");
	}
	SkillUp(SKILL_APPRAISE);
   } else {
	SendToChar("What item?\r\n");
   }
   LagChar(CMudTime::PULSES_PER_REAL_SECOND * 4);
}

/////////////////////////
//	Intimidate
//
//
//	John Comes 3-1-99
////////////////////////////////

void CRogue::DoIntimidate(const CInterp *pInterp) {
   if (!GetSkill(SKILL_INTIMIDATE)) {
	SendToChar("Learn how first!\r\n");
	return;
   }
   CCharacter *pTarget = GetTarget();
   if (pTarget) {
	CString strToChar;
	short nSkill = (GetSkill(SKILL_INTIMIDATE) + m_Proficiency.CalcBonus(m_nSkillSphere));
	short nDie = DIE(100);
	//If target is already intimidated you can't intimidate him more
	if (pTarget->IsAffectedBy(AFFECT_INTIMIDATE)) {
	   m_MsgManager.SkillMsg(this, pTarget, SKILL_INTIMIDATE, SKILL_KILLED);
	} else { //Check for intimidation, take into accout level diff
	   if (nDie <= (nSkill - (pTarget->GetLevel() - GetLevel()))) {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_INTIMIDATE, SKILL_WORKED);
		pTarget->AddAffect(CCharacter::AFFECT_INTIMIDATE, (nSkill * CMudTime::PULSES_PER_REAL_SECOND), nSkill / 10);
	   } else {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_INTIMIDATE, SKILL_DIDNT_WORK);
		SkillUp(SKILL_INTIMIDATE);
		//small chance for mob to attack you if you fail.
		if (!pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL) && pTarget != this) {
		   pTarget->GetRoom()->SendToRoom("%s says, 'Ha! I'll show you all about intimidation, fool!'\r\n", pTarget);
		   AddFighters(pTarget);
		}
	   }
	}
	LagChar(CMudTime::PULSES_PER_REAL_SECOND * 2);
   } else {
	SendToChar("Intimidate Who?\r\n");
   }
}

/////////////////////////
//	Dirt Toss
//	Can make person blind for a time
//	John Comes 3/4/99

void CRogue::DoDirtToss(CCharacter *pTarget) {
   if (pTarget) {
	//message tosses dirt
	int nBonus = m_Proficiency.CalcBonus(m_nSkillSphere);
	m_MsgManager.SkillMsg(this, pTarget, SKILL_DIRT_TOSS, SKILL_WORKED);
	if (DIE(100) <= GetSkill(SKILL_DIRT_TOSS) && !pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL)) {//if unsuccesful save and skill check, add blindness to target
	   //
	   int nTime = (nBonus * GetSkill(SKILL_DIRT_TOSS) / 100);
	   nTime += GetSkill(SKILL_DIRT_TOSS)+(CMudTime::PULSES_PER_BATTLE_ROUND / 2);
	   if (nTime > (CMudTime::PULSES_PER_BATTLE_ROUND / 2)) {
		pTarget->AddAffect(CCharacter::AFFECT_BLINDNESS, nTime, 0);
	   } else {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_DIRT_TOSS, SKILL_DIDNT_WORK);
	   }
	} else {
	   SkillUp(SKILL_DIRT_TOSS);
	}
	LagChar((4 - nBonus) * CMudTime::PULSES_PER_BATTLE_ROUND);
	AddFighters(pTarget);
   } else {
	SendToChar("Throw dirt in who's eyes?\r\n");
   }
}

///////////////////////////
//	Allow rogues to peek into inventory

void CRogue::DoCharacterLook(CCharacter *ch) {
   CCharacter::DoCharacterLook(ch);
   //if they are not a scout give them 10% chance
   if (!IsGod() && DIE(100) <= GetSkill(SKILL_OBSERVATION) + 10) {
	CString strInventory;
	strInventory.Format("You peek into %s inventory.\r\n",
		CCharacterAttributes::GetPronoun(ch->GetSex()));
	ch->GetInventoryNames(strInventory, (1 + GetSkill(SKILL_OBSERVATION)));
	SendToChar(strInventory);
   }
}

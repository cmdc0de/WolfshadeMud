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
 *  Warrior implementation
 *
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "warrior.h"

extern CRandom GenNum;

CSkillInfo CWarrior::m_SkillInfo[MAX_MUD_SKILLS];
bool CWarrior::StaticsInitialized = false;
const int CWarrior::GENERAL = (1 << 0);
const int CWarrior::WARRIOR = (1 << 1);
const int CWarrior::MARTIAL_ARTIST = (1 << 2);
const int CWarrior::SWORDSMAN = (1 << 3);
const int CWarrior::FIGHTER = (1 << 4);
const int CWarrior::SAMURAI = (1 << 5);
const int CWarrior::MONK = (1 << 6);
const int CWarrior::WEAPONS_MASTER = (1 << 7);
const int CWarrior::SWASHBUCKLER = (1 << 8);
const int CWarrior::KNIGHT = (1 << 9);
const int CWarrior::THUG = (1 << 10);
CProficiency CWarrior::m_Proficiency;
CString CWarrior::m_strProficiency("You class has the following proficiencies:\r\n");

const short int CWarrior::m_pThaco[] ={
   /*1,02,03,04,05,06,07,08,09,10*/
   100, 98, 96, 94, 92, 90, 88, 86, 84, 82,
   80, 78, 76, 74, 72, 70, 68, 66, 64, 62,
   60, 58, 56, 54, 52, 50, 45, 40, 35, 30,
   25, 20, 15, 10, 5, 0, -2, -4, -6, -8,
   -10, -12, -14, -16, -18, -20, -22, -24, -26, -28,
   -50, -50, -50, -50, -50, -50, -50, -50, -50, -50 /*51-60*/
};

const int CWarrior::m_Experience[LVL_IMP + 1] ={

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

CWarrior::~CWarrior() {

}

CWarrior::CWarrior(CMobPrototype &mob, CRoom *pPutInRoom)
: CCharacter(mob, pPutInRoom) {
   if (!StaticsInitialized) {
	InitSkillInfo();
   }
   //everyone gets general
   m_nSkillSphere |= GENERAL | WARRIOR;
}

CWarrior::CWarrior(CCharIntermediate *ch) : CCharacter(ch) {
   if (!StaticsInitialized) {
	InitSkillInfo();
   }
   //can't put in CCharacter construct because
   //it will be called before us
   if (IsNewbie()) {
	InitSkills();
	m_nSkillSphere |= WARRIOR | GENERAL;
   }
}

const CSkillInfo* CWarrior::GetSkillInfo(short nSkill) {
   return &m_SkillInfo[nSkill];
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////

short CWarrior::GetThaco() {
   return m_pThaco[m_nLevel];
}

short CWarrior::GetMaxHitPointsPerLevel() {
   return 12;
}

//////////////////////
//	GetExp
//	returns exp for level passed in
/////////////////////

int CWarrior::GetExp(short nLvl) {
   return m_Experience[nLvl];
}

/////////////////////////////
//	Sets up the skills a warrior can learn
//	

void CWarrior::InitSkillInfo() { /*max,min,pct*/
   StaticsInitialized = true;

   //	Base Skills
   m_SkillInfo[SKILL_TRACK] = CSkillInfo(GENERAL, 100, 0, 150);
   m_SkillInfo[SKILL_BODYSLAM] = CSkillInfo(GENERAL, 100, 1, 20);
   m_SkillInfo[SKILL_BANDAGE] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_BASH] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_DODGE] = CSkillInfo(GENERAL, 100, 1, 18);
   m_SkillInfo[SKILL_DUAL_WIELD] = CSkillInfo(GENERAL, 80, 1, 30);
   m_SkillInfo[SKILL_OFFENSE] = CSkillInfo(GENERAL, 100, 1, 20);
   m_SkillInfo[SKILL_FIRST_AID] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_MOUNT] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_BACKSTAB] = CSkillInfo(GENERAL, 75, 0, 10);
   m_SkillInfo[SKILL_TUNDRA_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_DESERT_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_MOUNTAIN_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_FOREST_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_SWAMP_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_FORAGE] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_RESCUE] = CSkillInfo(GENERAL, 100, 1, 40);
   m_SkillInfo[SKILL_SWITCH] = CSkillInfo(GENERAL, 100, 1, 40);
   m_SkillInfo[SKILL_TRIP] = CSkillInfo(GENERAL, 100, 1, 10);
   m_SkillInfo[SKILL_KICK] = CSkillInfo(GENERAL, 100, 1, 50);
   m_SkillInfo[SKILL_CLERIC_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_SHAMAN_MAGIC] = CSkillInfo(GENERAL, 25, 0, 10);
   m_SkillInfo[SKILL_BLIND_FIGHTING] = CSkillInfo(GENERAL, 100, 1, 100);
   m_SkillInfo[SKILL_BARE_HAND_FIGHTING] = CSkillInfo(GENERAL, 100, 5, 20);

   m_SkillInfo[SKILL_DOUBLE_ATTACK] = CSkillInfo(GENERAL, 100, 1, 20);
   m_SkillInfo[SKILL_BERSERK] = CSkillInfo(WARRIOR, 100, 1, 30);
   m_SkillInfo[SKILL_THROW_WEAPON] = CSkillInfo(WARRIOR, 90, 1, 20);
   m_SkillInfo[SKILL_PARRY] = CSkillInfo(WARRIOR, 100, 1, 20);
   m_SkillInfo[SKILL_CHANT] = CSkillInfo(MARTIAL_ARTIST, 100, 0, 50);
   m_SkillInfo[SKILL_BATTLE_READINESS] = CSkillInfo(MARTIAL_ARTIST, 100, 0, 30);
   m_SkillInfo[SKILL_QUICK_STRIKE] = CSkillInfo(MARTIAL_ARTIST, 100, 0, 30);
   m_SkillInfo[SKILL_CHANT_DEFENSE] = CSkillInfo(SAMURAI, 100, 0, 50);
   m_SkillInfo[SKILL_CHANT_HONOR] = CSkillInfo(SAMURAI, 100, 0, 50);
   m_SkillInfo[SKILL_WEAPON_FLURRY] = CSkillInfo(SAMURAI, 100, 0, 30);
   m_SkillInfo[SKILL_DRAGON_PUNCH] = CSkillInfo(MONK, 100, 0, 50);
   m_SkillInfo[SKILL_CHANT_REGENERATION] = CSkillInfo(MONK, 100, 0, 50);
   m_SkillInfo[SKILL_QUIVERING_PALM] = CSkillInfo(MONK, 100, 0, 50);
   m_SkillInfo[SKILL_WEAPON_KNOWLEDGE] = CSkillInfo(SWORDSMAN, 100, 0, 50);
   m_SkillInfo[SKILL_RIPOSTE] = CSkillInfo(SWORDSMAN, 100, 0, 50);
   m_SkillInfo[SKILL_CALLED_SHOT] = CSkillInfo(SWORDSMAN, 100, 0, 50);
   m_SkillInfo[SKILL_APPRAISE] = CSkillInfo(WEAPONS_MASTER, 100, 0, 50);
   m_SkillInfo[SKILL_WEAPON_REPAIR] = CSkillInfo(WEAPONS_MASTER, 100, 0, 50);
   m_SkillInfo[SKILL_HONE_WEAPON] = CSkillInfo(WEAPONS_MASTER, 100, 0, 50);
   m_SkillInfo[SKILL_DISARM] = CSkillInfo(SWASHBUCKLER, 100, 0, 50);
   m_SkillInfo[SKILL_GAMBLE] = CSkillInfo(SWASHBUCKLER, 100, 0, 50);
   m_SkillInfo[SKILL_SHIELD_RUSH] = CSkillInfo(FIGHTER, 100, 0, 50);
   m_SkillInfo[SKILL_HIT_ALL] = CSkillInfo(FIGHTER, 100, 0, 50);
   m_SkillInfo[SKILL_MOUNTED_COMBAT] = CSkillInfo(KNIGHT, 100, 0, 50);
   m_SkillInfo[SKILL_ARMOR_REPAIR] = CSkillInfo(KNIGHT, 100, 0, 20);
   m_SkillInfo[SKILL_KNIGHT_CODE] = CSkillInfo(KNIGHT, 100, 0, 50);
   m_SkillInfo[SKILL_HEROISM] = CSkillInfo(KNIGHT, 100, 0, 50);
   m_SkillInfo[SKILL_HEAD_BUTT] = CSkillInfo(THUG, 75, 0, 50);
   m_SkillInfo[SKILL_INTIMIDATE] = CSkillInfo(THUG, 100, 0, 50);
   m_SkillInfo[SKILL_WRESTLING] = CSkillInfo(THUG, 100, 0, 50);
   m_SkillInfo[SKILL_BEAR_HUG] = CSkillInfo(THUG, 100, 0, 50);
   m_SkillInfo[SKILL_HEAD_BUTT] = CSkillInfo(THUG, 100, 0, 30);

   //Add(CString strName, short nTier, short nBonus, short nPenatly, UINT nValue, UINT nParentValue)
   m_Proficiency.Add("General", 0, 0, 0, GENERAL, 0);
   m_Proficiency.Add("Warrior", 1, 1, 1, WARRIOR, GENERAL);
   m_Proficiency.Add("Maritial Artist", 2, 1, 1, MARTIAL_ARTIST, WARRIOR);
   m_Proficiency.Add("Swordsman", 2, 1, 1, SWORDSMAN, WARRIOR);
   m_Proficiency.Add("Fighter", 2, 1, 1, FIGHTER, WARRIOR);
   m_Proficiency.Add("Samurai", 3, 1, 1, SAMURAI, MARTIAL_ARTIST);
   m_Proficiency.Add("Monk", 3, 1, 1, MONK, MARTIAL_ARTIST);
   m_Proficiency.Add("Weapons master", 3, 1, 1, WEAPONS_MASTER, SWORDSMAN);
   m_Proficiency.Add("Swashbuckler", 3, 1, 1, SWASHBUCKLER, SWORDSMAN);
   m_Proficiency.Add("Knight", 3, 1, 1, KNIGHT, FIGHTER);
   m_Proficiency.Add("Thug", 3, 1, 1, THUG, FIGHTER);

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

void CWarrior::SendProficiency() {
   CString strProf;
   strProf.Format("Current Proficiency Bonus: %d\r\n%s",
	   m_Proficiency.CalcBonus(m_nSkillSphere),
	   m_strProficiency.cptr());
   SendCharPage(strProf);
}

/////////////////////////
//	Chant
//	Takes word after and adds chant affect accordingly
//
//
//	John Comes 1-28-99
////////////////////////////////

void CWarrior::DoChant() {
   if (!GetSkill(SKILL_CHANT)) {
	SendToChar("Learn how first!\r\n");
	return;
   }
   CString strChantType;
   strChantType = CurrentCommand.GetWordAfter(1, false);
   if ((short int) DIE(100) <= (GetSkill(SKILL_CHANT) - m_Proficiency.CalcBonus(m_nSkillSphere))) {
	if (strChantType.Compare("regeneration")) {
	   if (GetSkill(SKILL_CHANT_REGENERATION) > 0) {
		AddAffect(CCharacter::AFFECT_CHANT_REGENERATION, (GetSkill(SKILL_CHANT_REGENERATION) - m_Proficiency.CalcBonus(m_nSkillSphere)) * CMudTime::PULSES_PER_MUD_HOUR, 0);
		SkillUp(SKILL_CHANT_REGENERATION);
	   }
	} else if (strChantType.Compare("defense")) {
	   if (GetSkill(SKILL_CHANT_DEFENSE) > 0) {
		AddAffect(CCharacter::AFFECT_CHANT_DEFENSE, (GetSkill(SKILL_CHANT_DEFENSE) - m_Proficiency.CalcBonus(m_nSkillSphere)) * CMudTime::PULSES_PER_MUD_HOUR, -GetSkill(SKILL_CHANT_DEFENSE) / 5);
		SkillUp(SKILL_CHANT_DEFENSE);
	   }
	} else if (strChantType.Compare("honor")) {
	   if (GetSkill(SKILL_CHANT_HONOR) > 0) {
		AddAffect(CCharacter::AFFECT_CHANT_HONOR, DIE(GetSkill(SKILL_CHANT_HONOR)) * CMudTime::PULSES_PER_MUD_HOUR, GetSkill(SKILL_CHANT_HONOR) / 20);
		if (GetSkill(SKILL_CHANT_HONOR) > 80 && m_Proficiency.CalcBonus(m_nSkillSphere) > 0) {
		   AddAffect(CCharacter::AFFECT_HASTE, sAffect::MANUAL_AFFECT, 0);
		}
		SkillUp(SKILL_CHANT_HONOR);
	   }
	} else {
	   SendToChar("You don't know what you're chanting, but you chant anyway.\r\n");
	}
	SendToChar("You start chanting in a low voice.\r\n");
	GetRoom()->SendToRoom("%s starts chanting in a low voice.\r\n", this);
   } else {
	SendToChar("Your throat goes dry and you forget the chant.\r\n");

   }
   SkillUp(SKILL_CHANT);
   LagChar(CMudTime::PULSES_PER_REAL_SECOND * 3);
   return;
}

/////////////////////////
//	Berserk
//	Takes word after and adds chant affect accordingly
//
//
//	John Comes 2-16-99
////////////////////////////////

void CWarrior::DoBerserk() {
   if (!GetSkill(SKILL_BERSERK)) {
	SendToChar("Learn how first!\r\n");
	return;
   }
   CString strChantType;
   short nSkill = (GetSkill(SKILL_BERSERK) - m_Proficiency.CalcBonus(m_nSkillSphere));
   if (IsAffectedBy(AFFECT_BERSERK)) {
	if ((short int) DIE(100) <= nSkill) {
	   RemoveAffect(AFFECT_BERSERK);
	} else {
	   m_MsgManager.SkillMsg(this, this, SKILL_BERSERK, SKILL_KILLED);
	   SkillUp(SKILL_BERSERK);
	}
   } else {
	if ((short int) DIE(100) <= nSkill) {
	   this->AddAffect(CCharacter::AFFECT_BERSERK, (nSkill * CMudTime::PULSES_PER_REAL_SECOND), nSkill);
	   if (nSkill > 100) {
		this->AddAffect(CCharacter::AFFECT_HASTE, nSkill * CMudTime::PULSES_PER_REAL_SECOND, 0);
	   }
	} else {
	   m_MsgManager.SkillMsg(this, this, SKILL_BERSERK, SKILL_DIDNT_WORK);
	   SkillUp(SKILL_BERSERK);
	}
   }
   LagChar(CMudTime::PULSES_PER_REAL_SECOND * 2);
   return;
}

/////////////////////////
//	DragonPunch
//
//	John Comes 2-17-99

void CWarrior::DoDragonPunch(const CInterp *pInterp, CCharacter *pTarget) {
   if (!GetSkill(SKILL_DRAGON_PUNCH)) {
	SendToChar("Learn how first!\r\n");
   } else if (!pTarget) {
	SendToChar("Punch Who?\r\n");
   } else {
	short nSkill = GetSkill(SKILL_DRAGON_PUNCH) + m_Proficiency.CalcBonus(m_nSkillSphere) * 10;
	short nDie = DIE(100);
	CString strToChar;
	for (int i = 0; i < MAX_EQ_POS; i++) {
	   if (!m_Equipment[i].IsEmpty() && m_Equipment[i]->IsWeapon()) {
		SendToChar("You can't be wielding a weapon to punch!\r\n");
		return;
	   }
	}
	if (nDie <= nSkill) {
	   pTarget->TakeDamage(this, DIE(2 * nSkill) + GetSkill(SKILL_DRAGON_PUNCH) *
		   (m_Proficiency.CalcBonus(m_nSkillSphere) >= 0 ? m_Proficiency.CalcBonus(m_nSkillSphere) : 0), true);
	   if (pTarget->IsDead()) {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_DRAGON_PUNCH, SKILL_KILLED);
		pInterp->KillChar(pTarget);
	   } else {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_DRAGON_PUNCH, SKILL_WORKED);
		AddFighters(pTarget);
	   }
	} else {
	   m_MsgManager.SkillMsg(this, pTarget, SKILL_DRAGON_PUNCH, SKILL_DIDNT_WORK);
	   SkillUp(SKILL_DRAGON_PUNCH);
	   AddFighters(pTarget);
	}
	LagChar((5 * CMudTime::PULSES_PER_BATTLE_ROUND) - (m_Proficiency.CalcBonus(m_nSkillSphere)) * CMudTime::PULSES_PER_BATTLE_ROUND);
   }
}

//////////////////////
//	Identify
//
//	John Comes 2/9/1999

void CWarrior::DoIdentify() {
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
//	Quivering Palm
//
//	John Comes 2-28-99

void CWarrior::DoQuiveringPalm(const CInterp *pInterp, CCharacter *pTarget) {
   if (!GetSkill(SKILL_QUIVERING_PALM)) {
	SendToChar("Learn how first!\r\n");
	return;
   }
   if (!pTarget) {
	SendToChar("Palm Who?\r\n");
	return;
   }
   short nSkill = GetSkill(SKILL_QUIVERING_PALM) + m_Proficiency.CalcBonus(m_nSkillSphere) * 10;
   short nDie = DIE(100);
   CString strToChar;
   for (int i = 0; i < MAX_EQ_POS; i++) {
	if (!m_Equipment[i].IsEmpty() && m_Equipment[i]->IsWeapon()) {
	   SendToChar("You can't be wielding a weapon use your palms!\r\n");
	   return;
	}
   }
   if (nDie <= nSkill) {
	pTarget->TakeDamage(this, DIE(2 * nSkill) + GetSkill(SKILL_QUIVERING_PALM) *
		(m_Proficiency.CalcBonus(m_nSkillSphere) >= 0 ? m_Proficiency.CalcBonus(m_nSkillSphere) : 0), true);
	if (pTarget->IsDead()) {
	   m_MsgManager.SkillMsg(this, pTarget, SKILL_QUIVERING_PALM, SKILL_KILLED);
	} else {
	   m_MsgManager.SkillMsg(this, pTarget, SKILL_QUIVERING_PALM, SKILL_WORKED);
	}
   } else {
	m_MsgManager.SkillMsg(this, pTarget, SKILL_QUIVERING_PALM, SKILL_DIDNT_WORK);
	SkillUp(SKILL_QUIVERING_PALM);
   }
   AddFighters(pTarget);
   LagChar((5 * CMudTime::PULSES_PER_BATTLE_ROUND) - (m_Proficiency.CalcBonus(m_nSkillSphere)) * CMudTime::PULSES_PER_BATTLE_ROUND);
}

//////////////////////
//	Appraise
//
//	John Comes 3/1/1999

void CWarrior::DoAppraise() {
   if (!GetSkill(SKILL_APPRAISE)) {
	SendToChar("Learn how first!\r\n");
	return;
   }
   CString strItem(CurrentCommand.GetWordAfter(1));
   CObject *pObj;
   CString strToChar;
   int nDot, nNum = 1;
   if ((nDot = strItem.Find('.')) != -1) {
	nNum = strItem.Left(nDot).MakeInt();
	strItem = strItem.Right(nDot);
   }
   if ((pObj = FindInInventory(strItem, nNum))) {
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

void CWarrior::DoIntimidate(const CInterp *pInterp) {
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
//	Hit All
//
//	John Comes 3-1-99

void CWarrior::DoHitAll(const CInterp *pInterp) {
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
	LagChar((nLag * CMudTime::PULSES_PER_BATTLE_ROUND) - (m_Proficiency.CalcBonus(m_nSkillSphere)) * CMudTime::PULSES_PER_BATTLE_ROUND);
   }
}

///////////////////////////////////////
//	Disarm
//	If pass, weapon falls to ground
//	if fail nothing happens
//  if fail bad, you get hit
//  John Comes 3-3-99
///////////////////////////////////////

void CWarrior::DoDisarm(const CInterp *pInterp, CCharacter *pTarget) {
   if (!GetSkill(SKILL_DISARM)) {
	SendToChar("Learn how first!\r\n");
   } else if (!IsFighting()) {
	SendToChar("You have to be fighting someone to disarm them!\r\n");
   } else if (!pTarget) {
	SendToChar("Disarm who?\r\n");
   } else {
	//Get weapon you want to disarm
	CString strObj(CurrentCommand.GetWordAfter(2));
	CObject *pObj;
	bool bHadWeapon;
	short nSkill = GetSkill(SKILL_DISARM) + m_Proficiency.CalcBonus(m_nSkillSphere) * 10;
	short nDie = DIE(100);
	CString strMsg;
	for (int i = 0; i < MAX_EQ_POS; i++) { //Get target's weapons
	   if (!pTarget->GetEq(i).IsEmpty() && pTarget->GetEq(i)->IsWeapon()) { //Is weapon the one I want?
		if (pTarget->GetEq(i).GetObject()->ByAlias(strObj)) {
		   pObj = pTarget->GetEq(i).GetObject();
		   //if I pass a skill check and target passes a  dex check I get the disarm
		   if (nDie <= nSkill && !pTarget->StatCheck(DEX)) {
			m_pInRoom->Add(pObj);
			pTarget->RemoveEquipment(i, true);
			m_MsgManager.SkillMsg(this, pTarget, SKILL_DISARM, SKILL_WORKED);
			strMsg.Format("%s falls to the ground!\r\n", (char *) pObj->GetObjName());
			GetRoom()->SendToRoom(strMsg);
		   }//if I miss the disarm horribly target gets a free shot
		   else if (nDie - nSkill > 20) {
			m_MsgManager.SkillMsg(this, pTarget, SKILL_DISARM, SKILL_KILLED);
			Defense(CalcACWouldHit() - m_Proficiency.CalcBonus(m_nSkillSphere) * 10, i, pTarget);
			if (IsDead()) { //If I'm dead, kill me
			   pInterp->KillChar(this);
			}
			SkillUp(SKILL_DISARM);
		   } else {
			m_MsgManager.SkillMsg(this, pTarget, SKILL_DISARM, SKILL_DIDNT_WORK);
			SkillUp(SKILL_DISARM);
		   }
		   bHadWeapon = true;
		}
	   }
	   if (!bHadWeapon) {
		SendToChar("They aren't wielding that!\r\n");
	   }
	}
	LagChar((5 * CMudTime::PULSES_PER_BATTLE_ROUND) - (m_Proficiency.CalcBonus(m_nSkillSphere)) * CMudTime::PULSES_PER_BATTLE_ROUND);
   }
}


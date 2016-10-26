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
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "npc.classes.h"

/////////////////////////////
/// static
////////////////////////////
CMobAIManager<CNPCMage> CNPCMage::m_MobAI(&CNPCMage::DoGeneralAI);
bool CNPCMage::m_bInit = false;
const short CNPCMage::MAX_GREY_MAGE_FOLLOWERS = 6;
const short CNPCMage::MAX_GREY_MAGE_MAJOR_ELEMENTALS = 2;
const short CNPCMage::MIN_MANA_FOR_DEFENSIVE = 40; //%
const int CNPCMage::DO_DEFENSIVE_AI = CMudTime::PULSES_PER_REAL_SECOND * 5;
CList<const CSpell<CMage> *> CNPCMage::m_SpellAI[LVL_IMP + 1];

CNPCMage::CNPCMage(CMobPrototype &mob, CRoom *pPutInRoom)
: CMage(mob, pPutInRoom), CNPC(mob, pPutInRoom), CCharacter(mob, pPutInRoom) {
   DoInit();
   //can't use mob the this->GetLevel because it will always be 0
   //we don't advance level until after npc is created
   //because pure virtuals are called from advance level
   int nSkill;
   for (int i = 0; i < MAX_MUD_SKILLS; i++) {
	if (m_SkillInfo[i].CanLearn() &&
		HasSkillSet(m_SkillInfo[i].GetSet())) {
	   //if they have this skill set give them
	   //an automatic bonus of 1/4 of there level
	   m_Skills[i] = MIN(MAX_MOB_SKILL, (mob.GetLevel() + DIE((mob.GetLevel() >> 1) + 1)+(m_Proficiency.CalcBonus(m_nSkillSphere)*10)));
	}
   }
   POSITION pos = m_SpellTable.GetStartingPosition();
   CSpell<CMage> *pSpell;
   while (pos) {
	pSpell = m_SpellTable.GetNext(pos);
	if (IsOfSphere(pSpell->GetSphere()) &&
		mob.GetLevel() >= pSpell->GetMinLevel()) {
	   nSkill = DIE(mob.GetLevel() + 1) + mob.GetLevel();
	   m_Spells[pSpell->GetSpellID()] = MIN(MAX_MOB_SKILL, nSkill);
	}
   }
}

CNPCMage::~CNPCMage() {

}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////

short CNPCMage::GetMaxHitPointsPerLevel() {
   short nLevel = GetLevel() + 1;
   //6 is Base 
   short nHits = (2^((nLevel + 1) / 10) * nLevel * 6);
   return nHits;
}

////////////////////////////////////////
//	Handles the general AI for all mages
//  Always run CNPC DoAI if we don't do anything
//	special for the mages...why?  So we still do
//	scavenging, etc

void CNPCMage::DoGeneralAI(const CInterp *pInterp) {
   //if our command buffer is empty we aren't lagged
   //and we aren't casting
   if (CommandBufEmpty() && !IsLagged()
	   && !IsAffectedBy(CCharacter::AFFECT_CASTING)) {
	if (IsFighting()) {
	   //we don't can't a general offensive spell
	   //run CNPC DOAI
	   if (!InPosition(POS_STANDING) || !GeneralOffense()) {
		CNPC::DoAI(pInterp);
	   }
	} else {
	   //if we don't run a defensive spell
	   //so NPC ai stuff
	   //decision made 1-1-2000 to not do defensive AI
	   //if mob is a pet
	   if (!InPosition(POS_STANDING)
		   || HasMaster()
		   || !GeneralDefense()) {
		CNPC::DoAI(pInterp);
	   }
	}
   }
}

/////////////////////////////////
//	Init Statics

void CNPCMage::DoInit() {
   if (m_bInit) {
	return;
   }
   m_bInit = true;
   m_MobAI.RegisterAIFnc(24001, &CNPCMage::DoGreyAI);

   //Set up Spell AI by Level
   //this simply puts the offensive spells in one
   //are by min level needed to cast.
   POSITION pos = m_SpellTable.GetStartingPosition();
   const CSpell<CMage> *pSpell;
   while (pos) {
	pSpell = m_SpellTable.GetNext(pos);
	if (pSpell->IsOffensive()) {
	   m_SpellAI[pSpell->GetMinLevel()].Add(pSpell);
	}
   }
}

////////////////////////////////////////
//	AI for grey mages
//	return of false runs general function
//	true does not!
//	The grey mage will always be surrounded by
//  MAX_GREY_MAGE_FOLLOWERS and at least 
//  MAX_GREY_MAGE_MAJOR_ELEMENTALS will be major elementals
//	If we are not fighting get these followers
//	if we are fighting we run generalAI
//	if we are not fighting and we have all our followers
//	we run GeneralAI

bool CNPCMage::DoGreyAI(const CInterp *pInterp) {
   //struct used for what minor elements create what major ones

   struct ElementalAI {
	const char * strGreaterElemental;
	mob_vnum one, two;
   };
   //stucture used to test if we have two minors we can
   //turn into a major

   struct sLogic {
	bool bOne,
	bTwo;

	sLogic() {
	   bOne = bTwo = false;
	}
   };
   //casting strings for minor elementals
   static const char *strElementals[8] ={
	"cast 'Conjure Water Elemental'",
	"cast 'Conjure blood Elemental'",
	"cast 'Conjure Earth Elemental'",
	"cast 'Conjure Grave Elemental'",
	"cast 'Conjure Air Elemental'",
	"cast 'Conjure Fog Elemental'",
	"cast 'Conjure Fire Elemental'",
	"cast 'Conjure Pyre Elemental'"
   };
   //the implementation of ElementalAI struct
   static struct ElementalAI GreyAI[12] ={
	{"cast 'Create Ice Elemental' air water", VNUM_AIR_ELEMENTAL, VNUM_WATER_ELEMENTAL},
	{"cast 'Create Slime Elemental' fog blood", VNUM_FOG_ELEMENTAL, VNUM_BLOOD_ELEMENTAL},
	{"cast 'Create Rock Elemental' air earth", VNUM_AIR_ELEMENTAL, VNUM_EARTH_ELEMENTAL},
	{"cast 'Create Tomb Elemental' fog grave", VNUM_FOG_ELEMENTAL, VNUM_GRAVE_ELEMENTAL},
	{"cast 'Create Coal Elemental' earth fire", VNUM_EARTH_ELEMENTAL, VNUM_FIRE_ELEMENTAL},
	{"cast 'Create Ash Elemental' grave pyre", VNUM_GRAVE_ELEMENTAL, VNUM_PYRE_ELEMENTAL},
	{"cast 'Create Steam Elemental' water fire", VNUM_WATER_ELEMENTAL, VNUM_FIRE_ELEMENTAL},
	{"cast 'Create Plasma Elemental' blood pyre", VNUM_BLOOD_ELEMENTAL, VNUM_PYRE_ELEMENTAL},
	{"cast 'Create Mist Elemental' air fire", VNUM_AIR_ELEMENTAL, VNUM_FIRE_ELEMENTAL},
	{"cast 'Create Smoke Elemental' fog pyre", VNUM_FOG_ELEMENTAL, VNUM_PYRE_ELEMENTAL},
	{"cast 'Create Mud Elemental' earth water", VNUM_EARTH_ELEMENTAL, VNUM_WATER_ELEMENTAL},
	{"cast 'Create Gore Elemental' grave blood", VNUM_GRAVE_ELEMENTAL, VNUM_BLOOD_ELEMENTAL}
   };

   if (CommandBufEmpty() && !IsLagged() &&
	   (!(CMudTime::GetCurrentPulse() % DO_DEFENSIVE_AI)) &&
	   !IsAffectedBy(CCharacter::AFFECT_CASTING)) {
	if (IsFighting()) {
	   return false; //run general
	} else {
	   //if they don't have detect invis how are they a grey mage?
	   if (m_Spells[CMage::MAGE_DETECT_INVISIBILITY] == 0) {
		ErrorLog << "The grey mage " << m_strName << " doesn't know how to cast DI?!!!!!" << endl;
		return false;
	   }//in case we get an air or fog we can see them
	   else if (!IsAffectedBy(CCharacter::AFFECT_DETECT_INVISIBILITY)) {
		AddCommand(CString("cast 'detect invisibility' me"));
	   } else {
		//look though the mobs we are masters of
		POSITION pos = m_Master.m_MasterOf.GetStartPosition();
		CCharacter *pCh;
		short nMajorElementals = 0;
		short nElementalToCast = -1;
		sLogic CastLogic[12];
		while (pos) {
		   pCh = m_Master.m_MasterOf.GetNext(pos);
		   if (IsGreaterElemental(pCh)) {
			nMajorElementals++;
		   } else {
			int i;
			for (i = 0; i < 12 && nElementalToCast == -1; i++) {
			   if (GreyAI[i].one == pCh->GetVnum()) {
				CastLogic[i].bOne = true;
			   }
			   if (GreyAI[i].two == pCh->GetVnum()) {
				CastLogic[i].bTwo = true;
			   }
			   if (CastLogic[i].bOne == true &&
				   CastLogic[i].bTwo == true) {
				nElementalToCast = i;
			   }
			}
		   }
		}
		//make a major?
		if (nMajorElementals < MAX_GREY_MAGE_MAJOR_ELEMENTALS
			&& nElementalToCast != -1) {
		   AddCommand(CString(GreyAI[nElementalToCast].strGreaterElemental));
		   return true;
		}//make a minor?
		else if (m_Follow.m_Followers.GetCount() < MAX_GREY_MAGE_FOLLOWERS) {
		   AddCommand(CString(strElementals[(DIE(8) - 1)]));
		   return true;
		} else {
		   return false; //run generalAI
		}
	   }
	}
   }
   return false;
}

//////////////////////////////////////
//	Overridden DoAI for NPC mages
/////////////////////////////////////

void CNPCMage::DoAI(const CInterp * pInterp) {
   m_MobAI.RunMobAI(pInterp, m_nVNum, this);
}

////////////////////////////////
//	IsGreaterElemental
//	

bool CNPCMage::IsGreaterElemental(CCharacter *pCh) {
   mob_vnum vnum = pCh->GetVnum();
   if (vnum == VNUM_ICE_ELEMENTAL || vnum == VNUM_SLIME_ELEMENTAL ||
	   vnum == VNUM_ROCK_ELEMENTAL || vnum == VNUM_TOMB_ELEMENTAL ||
	   vnum == VNUM_COAL_ELEMENTAL || vnum == VNUM_ASH_ELEMENTAL ||
	   vnum == VNUM_STEAM_ELEMENTAL || vnum == VNUM_PLASMA_ELEMENTAL ||
	   vnum == VNUM_MIST_ELEMENTAL || vnum == VNUM_SMOKE_ELEMENTAL ||
	   vnum == VNUM_MUD_ELEMENTAL || vnum == VNUM_GORE_ELEMENTAL) {
	return true;
   }
   return false;
}

////////////////////////////////////
//	Generally casts the most power
//	offensive spell the mage can
//	return true means they will cast
//	other wise they won't

bool CNPCMage::GeneralOffense() {
   short nLevel = GetLevel() > LVL_IMP ? LVL_IMP : GetLevel();
   //find the highest level spell we can cast
   const CSpell<CMage> *pSpell, *pMaxSpell = NULL;
   POSITION pos;
   //now lets find the most damaging spell
   //Check for random not damaging spell ie blind farie fire etc
   bool bUseNonDam = DIE(1000) < 10; //1% chance
   do {
	if (!m_SpellAI[nLevel].IsEmpty()) {
	   pos = m_SpellAI[nLevel].GetStartPosition();
	   while (pos) {
		pSpell = m_SpellAI[nLevel].GetNext(pos);
		//for now no mob will cast area affect spells
		//at least in the general offensive
		//we don't want them casting areas in a room full of
		//elite guards =) while fighting a level 1 PC
		if (m_Spells[pSpell->GetSpellID()] > 0 &&
			(!(pSpell->IsAffectedArea(AREA_ROOM)))) {
		   if (bUseNonDam && !pSpell->IsSpellDamaging()) {
			pMaxSpell = pSpell;
			break;
		   } else if (pSpell->IsSpellDamaging()) {
			if (pMaxSpell == NULL) {
			   pMaxSpell = pSpell;
			} else if (pMaxSpell->GetDamage(m_Spells[pSpell->GetSpellID()]) < pSpell->GetDamage(m_Spells[pSpell->GetSpellID()])) {
			   pMaxSpell = pSpell;
			}
		   }
		}
	   }
	}
	nLevel--;
   } while (nLevel > 0 && pMaxSpell == NULL);
   if (pMaxSpell != NULL && (pMaxSpell->GetMana() <= this->m_nManaPts)) {
	CString str;
	str.Format("cast '%s'", pMaxSpell->GetSpellName().cptr());
	AddCommand(str);
	return true;
   } else {
	return false; //we have nothing to cast!
   }
}


///////////////////////////////
//	Generally casts the most defensive
//	spell the mage can
//	return true means they will cast
//  other wise they won't and CNPC::DOAI will be run

bool CNPCMage::GeneralDefense() {
   bool bRetValue = false;
   if (!(CMudTime::GetCurrentPulse() % DO_DEFENSIVE_AI)
	   && ((m_nManaPts * 100) / m_nMaxManaPts > MIN_MANA_FOR_DEFENSIVE))
	//if we don't check for mana not to get below MIN_MANA_FOR_DEFENSIVE
	//we could be all the way to 0 then a battle start and we have no
	//mana for offense
   {
	if (m_Spells[CMage::MAGE_GUKS_GLOWING_GLOBES] > 0 &&
		!IsAffectedBy(CCharacter::AFFECT_GUKS_GLOWING_GLOBES)) {
	   AddCommand(CString("cast 'guks glowing globes' me"));
	   bRetValue = true;
	} else if (m_Spells[CMage::MAGE_STONE_SKIN] > 0 &&
		!IsAffectedBy(CCharacter::AFFECT_STONE_SKIN)) {
	   AddCommand(CString("cast 'stone skin' me"));
	   bRetValue = true;
	} else if (m_Spells[CMage::MAGE_DETECT_INVISIBILITY] &&
		!IsAffectedBy(CCharacter::AFFECT_DETECT_INVISIBILITY)) {
	   AddCommand(CString("cast 'detect invisibility' me"));
	   bRetValue = true;
	} else if (m_Spells[CMage::MAGE_INVISIBILITY] > 0 &&
		!IsAffectedBy(CCharacter::AFFECT_INVISIBILITY)) {
	   AddCommand(CString("cast 'invisibility' me"));
	   bRetValue = true;
	} else if (m_Spells[CMage::MAGE_FIRE_SHIELD] > 0 &&
		!IsAffectedBy(CCharacter::AFFECT_FIRE_SHIELD)) {
	   AddCommand(CString("cast 'fire shield' me"));
	   bRetValue = true;
	} else if (m_Spells[CMage::MAGE_COLD_SHIELD] &&
		!IsAffectedBy(CCharacter::AFFECT_COLD_SHIELD)) {
	   AddCommand(CString("cast 'cold shield' me"));
	   bRetValue = true;
	} else if (m_Spells[CMage::MAGE_HASTE] > 0 &&
		!IsAffectedBy(CCharacter::AFFECT_HASTE)) {
	   AddCommand(CString("cast 'haste' me"));
	   bRetValue = true;
	}
   }
   return bRetValue;
}

/////////////////////////////////////////
//	DoCast
//	Find tick ' marks
//	Get spell word and see if in hash table
//	if not return 
//	if it is..dereference function pointer in
//	CSpell class and run function assocated with
//	this spell
//	written by: Demetrius Comes
////////////////////////////////////

void CNPCMage::DoCast(const CInterp *pInterp, bool bForceCast) {
   CString strToRoom = "%s ";
   CString strTemp;
   static const char *pTick = "'";
   //if they aren't casting check all information and insure
   //they can cast the spell
   if (!IsAffectedBy(CCharacter::AFFECT_CASTING)) {
	int first = CurrentCommand.Find(pTick[0]);
	int second = CurrentCommand.Find(pTick[0], 2);

	if (first == -1 || second == -1) {
	   SendToChar("Spells must be inclosed in single quotes!\r\n");
	   if (!IsSwitchedTo()) {
		ErrorLog << m_strName << " trying to cast but spell is not inclosed in single quotes.  "
			<< CurrentCommand << endl;
	   }
	   return;
	}

	CString strSpell(CurrentCommand(first + 1, second - 1));

	CSpell<CMage> *pSpell;

	if (!m_SpellTable.Lookup(strSpell, pSpell)) {
	   SendToChar("Cast what?\r\n");
	   return;
	}
	if (!m_Spells[pSpell->GetSpellID()] || pSpell->GetMinLevel()>this->GetLevel()) {
	   SendToChar("You don't know how to cast that spell!\r\n");
	   if (!IsSwitchedTo()) {
		ErrorLog << m_strName << " is trying to cast " << pSpell->GetSpellName()
			<< " but doesn't know how." << endl;
	   }
	   return;
	}
	//NPC's can't cast followers if they have a master!
	if (m_Master.HasMaster()) {
	   if (pSpell->GetSpellID() == MAGE_CONJURE_AIR_ELEMENTAL ||
		   pSpell->GetSpellID() == MAGE_CONJURE_EARTH_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CONJURE_WATER_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CONJURE_FIRE_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_ICE_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_TOMB_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CONJURE_FOG_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CONJURE_GRAVE_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CONJURE_BLOOD_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CONJURE_PYRE_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_COAL_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_ASH_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_STEAM_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_PLASMA_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_MIST_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_SMOKE_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_SLIME_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_ROCK_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_MUD_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_GORE_ELEMENTAL
		   || pSpell->GetSpellID() == MAGE_CREATE_SPECTRE
		   || pSpell->GetSpellID() == MAGE_CREATE_GHOST
		   || pSpell->GetSpellID() == MAGE_CREATE_GHOUL
		   || pSpell->GetSpellID() == MAGE_CREATE_WIGHT
		   || pSpell->GetSpellID() == MAGE_CREATE_WRAITH
		   || pSpell->GetSpellID() == MAGE_ANIMATE_DEAD
		   || pSpell->GetSpellID() == MAGE_BECOME_LICH) {
		return;
	   }
	}
	//does the mob have enough mana?
	if (m_nManaPts - (pSpell->GetMana()) < 0) {
	   SendToChar("You need more mana!\r\n");
	   return;
	}
	//get everything after the ticks and save it to
	//be added to the currentSpell structure
	CString strAfterTicks(CurrentCommand.Right(second + 1));
	//get everything after the ticks and see if we need
	//to pluck off the a '.'
	CString strTarget(CurrentCommand.Right(second + 1));
	strAfterTicks.SkipSpaces();
	strTarget.SkipSpaces();
	CCharacter *pTarget = NULL;

	if ((pSpell->GetAffectedArea() & AREA_TARGET)) {
	   CString strTmp(CurrentCommand);
	   CurrentCommand = strTarget;
	   pTarget = GetTarget(0, true);
	   CurrentCommand = strTmp;
	   if (!pTarget) {
		strTmp.Format("At who?\r\nTrying to cast %s\r\n",
			pSpell->GetSpellName().cptr());
		SendToChar(strTmp);
		return;
	   }
	} else if ((pSpell->GetAffectedArea() & AREA_SELF)) {
	   pTarget = this;
	}
	//know we know we can cast it so set casting bit
	//and set up all information in current spell struct
	AddAffect(CCharacter::AFFECT_CASTING, sAffect::MANUAL_AFFECT);
	if (DIE(100) <= GetSkill(SKILL_QUICK_CHANT)) {
	   m_lLagTime = pSpell->GetLagTime() >> 1;
	} else {
	   m_lLagTime = pSpell->GetLagTime();
	}
	m_CurrentSpell.m_pSpell = pSpell;
	m_CurrentSpell.m_strTarget = strAfterTicks;
	m_CurrentSpell.m_pTarget = pTarget;
	SendToChar(CString("&WYou start chanting...&n\r\n"));
	GetRoom()->SendToRoom(CString("%s &Wstarts casting a spell...&n \r\n"), this);
	m_bHasSpelledUp = false; //set this so we can check for spell ups
   }	//they already were casting so we know casting time is up
	//other wise we wouldn't be in here.
   else {
	SendToChar(CString("&WYou complete your spell...&n\r\n"));
	GetRoom()->SendToRoom(CString("%s &Wcompletes the spell...&n \r\n"), this);
	strTemp.Format("utters the words '%s'\r\n", m_CurrentSpell.m_pSpell->GetSpellName().cptr());
	strToRoom += strTemp;
	GetRoom()->SendToRoom(strToRoom, this);
	//remove casting Affect
	RemoveAffect(CCharacter::AFFECT_CASTING);

	//cast spell
	m_CurrentSpell.m_pSpell->Cast(pInterp, this);
	//take mana from mob
	m_nManaPts -= m_CurrentSpell.m_pSpell->GetMana();
	//empty spell struct
	m_CurrentSpell.Empty();
   }
}

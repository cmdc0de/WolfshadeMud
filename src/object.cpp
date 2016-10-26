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
 * CObject implementation
 *
 * written 9/26/97
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "object.save.h"

extern CGlobalVariableManagement GVM;

const short CObject::STATE_GOOD = 1;
const short CObject::STATE_DELETE = 0;
const short CObject::OBJ_AFFECT_CURRENTLY_WORN = 0;
const short CObject::OBJ_AFFECT_MIRAGE = 1;
const short CObject::OBJ_AFFECT_CAST_MAGIC_MISSILE = 5;
const short CObject::OBJ_AFFECT_HASTE = 6;
//this makes the person wearing the eq hidden
const short CObject::OBJ_AFFECT_HIDE = 7;
const short CObject::OBJ_AFFECT_COLD_SHIELD = 8;
const short CObject::OBJ_AFFECT_DETECT_INVIS = 9;
const short CObject::OBJ_AFFECT_DEATHS_MATRIARCH = 10;
const short CObject::OBJ_AFFECT_DIVINE_FATE = 11;
const short CObject::OBJ_AFFECT_AIR_SHIELD = 12;
const short CObject::OBJ_AFFECT_STONE_SKIN = 13;
const short CObject::OBJ_AFFECT_SHADOW_ARMOR = 14;

const short CObject::OBJ_AFFECT_DELAYED_BLAST_FIREBALL = 16;
const short CObject::OBJ_AFFECT_HONE_WEAPON = 17;
const short CObject::OBJ_AFFECT_MISSED_HONE = 18;
const short CObject::OBJ_AFFECT_BOX = 19;
const short CObject::OBJ_AFFECT_INVISIBLE = 20;
//This is when the object itself is hidden
const short CObject::OBJ_AFFECT_HIDDEN = 21;
const short CObject::OBJ_AFFECT_CONTINUAL_LIGHT = 23;

const short CObject::CAST_PCT_MAGIC_MISSILE = 25;
const short CObject::DEATHS_MATRIARCH_PCT = 20;
const short CObject::DIVINE_FATE_PCT = 25;
const short CObject::SHADOW_ARMOR_TIME = CMudTime::PULSES_PER_MUD_HOUR * 2;

//bit vectors
const int CObject::ITEM_NOT_TAKEABLE = (1 << 0); //a
const int CObject::ITEM_NEWBIE = (1 << 1); //b
const int CObject::ITEM_ANTI_GOOD = (1 << 2); //c
const int CObject::ITEM_ANTI_NEUTRAL = (1 << 3); //d
const int CObject::ITEM_ANTI_EVIL = (1 << 4); //e
const int CObject::ITEM_ANTI_ANTIPALADIN = (1 << 5); //f
const int CObject::ITEM_ANTI_CLERIC = (1 << 6); //g
const int CObject::ITEM_ANTI_DEFILER = (1 << 7); //h
const int CObject::ITEM_ANTI_DRUID = (1 << 8); //i
const int CObject::ITEM_ANTI_EMPATH = (1 << 9); //j
const int CObject::ITEM_ANTI_MAGE = (1 << 10); //k
const int CObject::ITEM_ANTI_MINDFLAYER = (1 << 11); //l
const int CObject::ITEM_ANTI_PALADIN = (1 << 12); //m
const int CObject::ITEM_ANTI_PSIONICIST = (1 << 13); //n
const int CObject::ITEM_ANTI_RANGER = (1 << 14); //o
const int CObject::ITEM_ANTI_ROGUE = (1 << 15); //p
const int CObject::ITEM_ANTI_SHAMAN = (1 << 16); //q
const int CObject::ITEM_ANTI_WARRIOR = (1 << 17); //r
const int CObject::ITEM_ANTI_GOOD_RACE = (1 << 18); //s
const int CObject::ITEM_ANTI_NEUTRAL_RACE = (1 << 19); //t
const int CObject::ITEM_ANTI_EVIL_RACE = (1 << 20); //u
const int CObject::ITEM_ANTI_ALIEN_RACE = (1 << 21);

CObject::sAffectInfo CObject::m_AffectInfo[TOTAL_OBJ_AFFECTS];
bool CObject::m_bInit = false;
extern CRandom GenNum;

CObject::CObject(CObjectPrototype *obj, CCharacter *PersonCarriedby, CRoom *pInRoom) {
   InitStatics();
   m_nState = STATE_GOOD;
   ExtraDesc.SetDeleteData();
   ObjAffects.SetDeleteData();
   m_pCarriedby = PersonCarriedby;
   m_pInRoom = pInRoom;
   m_nVnum = obj->Vnum;
   m_nAffBit = obj->AffBit;
   m_nCost = obj->Cost;
   m_strObjAlias = obj->ObjAlias;
   m_strObjDesc = obj->ObjDesc;
   m_strObjName = obj->ObjName;
   m_strObjSittingDesc = obj->ObjSittingDesc;
   m_nObjType = obj->ObjType;
   m_nVal0 = obj->Val0;
   m_nVal1 = obj->Val1;
   m_nVal2 = obj->Val2;
   m_nVal3 = obj->Val3;
   m_nVal4 = obj->Val4;
   m_nWearBit = obj->WearBit;
   m_nWeight = obj->Weight;
   POSITION pos = obj->ObjExtraDesc.GetStartPosition();
   while (pos) {
	ExtraDesc.Add(new CObjectDesc(obj->ObjExtraDesc.GetNext(pos)));
   }
   pos = obj->ObjAffects.GetStartPosition();
   while (pos) {
	ObjAffects.Add(new sObjectWearAffects(obj->ObjAffects.GetNext(pos)));
   }
}

CObject::~CObject() {
   if (!CGame::IsShuttingDown()) {
	if (IsAffectBit(ITEM_NEWBIE) && GetRoom()) {
	   CString str;
	   str.Format("%s crumbles to dust.\r\n", GetObjName());
	   GetRoom()->SendToRoom(str);
	}
	if (GetRoom()) {
	   GetRoom()->Remove(this);
	}
   }
}

////////////////////////
//	ByAlias
//	Given a string
//	try to match that string to an alias
//	 of the object
//	written by: demetrius Comes
//////////////////////////////

bool CObject::ByAlias(CString & strItem) {
   CString strAlias;
   int i = 0;
   while (!(strAlias = m_strObjAlias.GetWordAfter(i)).IsEmpty()) {
	if (strItem.Compare(strAlias)) {
	   return true;
	}
	i++;
   }
   return false;
}

////////////////////////////////
//	Makes the rest of the corpse
//////////////////////////

CObject::CObject(CCharacter * pCh) {
   InitStatics();
   m_nState = STATE_GOOD;
   ExtraDesc.SetDeleteData();
   ObjAffects.SetDeleteData();
   m_nVnum = -1;
   m_nAffBit = ITEM_NOT_TAKEABLE;
   m_nCost = 0;
   m_nObjType = ITEM_TYPE_CORPSE;
   m_nVal0 = m_nVal1 = m_nVal2 = m_nVal3 = m_nVal4 = -1;
   m_nWearBit = 0;
   m_nWeight = 0;
   m_pCarriedby = NULL;
   m_pInRoom = pCh->GetRoom();
   CString str("corpse ");
   str += pCh->GetName();
   m_strObjAlias = str;
   if (pCh->IsNPC()) {
	str.Format("The corpse of %s lies here cold and bloody.", pCh->GetName().cptr());
   } else {
	str.Format("The corpse of a fallen %s lies here cold and bloody.", GVM.GetRaceName(pCh->GetRace()));
   }
   m_strObjSittingDesc = str;
   str.Format("The corpse of %s lies here cold and bloody.", pCh->GetName().cptr());
   m_strObjDesc = str;
   m_strObjName = pCh->GetName();
}


////////////////////////////
//	Builds a string for the
//	Examine command
//	written by: Demetrius Comes
///////////////////////////

void CObject::Examine(const CCharacter *pLooker, CString & strExamine) {
   strExamine.Format("%s%s\r\n",
	   strExamine.cptr(), m_strObjDesc.cptr());

   POSITION pos = ExtraDesc.GetStartPosition();
   while (pos) {
	strExamine.Format("%s%s\r\n", strExamine.cptr(),
		ExtraDesc.GetNext(pos)->ExtraDesc.cptr());
   }
}

////////////////////////////////////
// Caffectinfo

CObject::sAffectInfo::sAffectInfo(
	CString strName, CString strAffectRate,
	CString strAffectRateRoom,
	CString strBattleRound,
	CString strBattleRoom, CString strEnd,
	CString strEndRoom,
	void (CObject::*fncRound)(short),
	void (CObject::*fncBattle)(short),
	void (CObject::*fncEnd)(short),
	int nRate) {
   m_strName = strName;
   m_strAffectRate = strAffectRate;
   m_strAffectRateRoom = strAffectRateRoom;
   m_strBattleRound = strBattleRound;
   m_strBattleRoom = strBattleRoom;
   m_strEnd = strEnd;
   m_strEndRoom = strEndRoom;
   m_pFncBattleRound = fncBattle;
   m_pFncEnd = fncEnd;
   m_pFncRound = fncRound;
   m_nRate = nRate;
}


////////////////////////////////////
//CAffectedby

void CObject::InitStatics() {
   if (m_bInit)
	return;
   m_bInit = true;

   /*
   CAffectInfo(
	   CString strName, CString AffectRate,
	   CString AffectRateRoom, CString strBattleRound,
	   CString strBattleRoom, CString strEnd, 
	   CString strEndRoom, 
	   void (CObject::*fncRound)(int),
	   void (CObject::*fncBattle)(int),
	   void (CObject::*fncEnd)(int), 
	   bool bAccum, int nRate)
    */
#define OBJNULL (void (CObject::*)(short)) NULL
   m_AffectInfo[OBJ_AFFECT_MIRAGE] = sAffectInfo("mirage", "", "", "",
	   "", "", "", OBJNULL, OBJNULL, &CObject::FadeType, 0);
   m_AffectInfo[OBJ_AFFECT_CAST_MAGIC_MISSILE] = sAffectInfo("Magic missile", "", "",
	   "", "", "", "", OBJNULL, &CObject::CastMagicMissile, OBJNULL, 0);
   m_AffectInfo[OBJ_AFFECT_DEATHS_MATRIARCH] = sAffectInfo("Deaths Matriarch", "", "",
	   "", "", "", "", OBJNULL, &CObject::DeathsMatriarch, OBJNULL, 0);
   m_AffectInfo[OBJ_AFFECT_DIVINE_FATE] = sAffectInfo("Divine Fate", "&WDivine Fate does NOT like to be held by impure hands!&n\r\n",
	   "&WDivine Fate does NOT like to be held by impure hands!&n\r\n",
	   "&WDivine Fate &wflashes with &Wholy&w ra&Wdian&wce!&n\r\n", "&WDivine Fate &wflashes with &Wholy&w ra&Wdian&wce!&n\r\n",
	   "", "", OBJNULL, &CObject::DivineFate, OBJNULL, 1); //JOHN THIS MAKES NO SENSE TO HAVE WORDING FOR fncbeging, and fnc end but then don't call a function...boggle!
   m_AffectInfo[OBJ_AFFECT_DELAYED_BLAST_FIREBALL] = sAffectInfo("Delayed Blast Fireball", "", "",
	   "", "", "", "", OBJNULL, OBJNULL, &CObject::DelayedBlastFireball, 0);
   m_AffectInfo[OBJ_AFFECT_HONE_WEAPON] = sAffectInfo("Hone Weapon", "", "",
	   "", "", "", "", OBJNULL, OBJNULL, OBJNULL, 0);
   m_AffectInfo[OBJ_AFFECT_BOX] = sAffectInfo("Little Black Box", "", "",
	   "", "", "", "", OBJNULL, OBJNULL, OBJNULL, 0);
   m_AffectInfo[OBJ_AFFECT_INVISIBLE] = sAffectInfo("Invisible", "", "",
	   "", "", "", "", OBJNULL, OBJNULL, OBJNULL, 0);
   m_AffectInfo[OBJ_AFFECT_CONTINUAL_LIGHT] = sAffectInfo("continual light", "", "", "", "", "", "", OBJNULL, OBJNULL, OBJNULL, 0);
}

bool CObject::AddAffect(short nAffect, int nPulses, int nValue) {
#ifdef _DEBUG
   if (nAffect > TOTAL_OBJ_AFFECTS)
	assert(0);
#endif
   if (!m_Affects[nAffect].HasAffect()) {
	m_Affects[nAffect].Set(nPulses, nValue);
	return true;
   }
   return false;
}

///////////////////////////
//	Call for all characters in hasCommand()
//	to reduce all affects by 1 pulse
//

void CObject::FadeAffects(long lTimeToRemove) {
   for (int i = 0; i < TOTAL_OBJ_AFFECTS; i++) {
	if (m_Affects[i].HasAffect()) {
	   if (!m_Affects[i].IsPermAffect() && !m_Affects[i].IsManual()) {
		if ((m_Affects[i].m_nTime -= lTimeToRemove) <= 0) {
		   RemoveAffect(i);
		} else if (m_Affects[i].IsAffectTime(m_AffectInfo[i].m_nRate)) {
		   if (!m_AffectInfo[i].m_strAffectRate.IsEmpty() &&
			   m_pCarriedby != NULL) {
			m_pCarriedby->SendToChar(m_AffectInfo[i].m_strAffectRate);
		   }
		   if (!m_AffectInfo[i].m_strAffectRateRoom.IsEmpty()) {
			m_pCarriedby == NULL ? m_pInRoom->SendToRoom(m_AffectInfo[i].m_strAffectRateRoom) : m_pCarriedby->GetRoom()->SendToRoom(m_AffectInfo[i].m_strAffectRateRoom, m_pCarriedby);
		   }
		   if (m_AffectInfo[i].m_pFncRound) {
			(this->*m_AffectInfo[i].m_pFncRound)(i);
		   }
		}
	   }
	}
   }
}

//////////////////////////
//	Removes an affect
//	Sends an ending message to the owner
//	calls ending function if one.
//

void CObject::RemoveAffect(short nAffect) {
   //where does the CAffectby end function get called?
   if (m_Affects[nAffect].IsPermAffect()) {
	return;
   }
   if (!m_AffectInfo[nAffect].m_strEnd.IsEmpty() &&
	   m_pCarriedby) {
	m_pCarriedby->SendToChar(m_AffectInfo[nAffect].m_strEnd);
   }
   if (!m_AffectInfo[nAffect].m_strEndRoom.IsEmpty()) {
	if (m_pCarriedby) {
	   m_pCarriedby->GetRoom()->SendToRoom(m_AffectInfo[nAffect].m_strEndRoom, m_pCarriedby);
	} else if (m_pInRoom) {
	   m_pInRoom->SendToRoom(m_AffectInfo[nAffect].m_strEndRoom);
	} else {
	   assert(0);
	}
   }
   if (m_AffectInfo[nAffect].m_pFncEnd) {
	(this->*m_AffectInfo[nAffect].m_pFncEnd)(nAffect);
   }
   //the remove function my rely on values in m_nvalue
   m_Affects[nAffect].Remove();
   if (m_nState == STATE_DELETE) {
	if (m_pCarriedby != NULL) {
	   m_pCarriedby->RemoveObjFromChar(this);
	} else if (m_pInRoom != NULL) {
	   m_pInRoom->Remove(this);
	} else {
	   ErrorLog << "Object being removed is not in a room or on a character!" << endl;
	}
   }
}

//////////////////////////
//	Removes all affects
//	Sends an ending message to the owner
//	calls ending function if one.
//

void CObject::RemoveAllAffects() {
   for (int i = 0; i < TOTAL_OBJ_AFFECTS; i++) {
	RemoveAffect(i);
   }
}

void CObject::FadeType(short nAffect) {
   //TODO do we still need this?
   switch (nAffect) {
	case OBJ_AFFECT_MIRAGE:
	   SetDelete();
	   break;
	default:
	   ErrorLog << "Error out in CObject::FadeType" << endl;
	   break;
   }
}

void CObject::BlackBox(bool bRemoving, bool bSendMsg) {
   //Send Message when removing or equipping
   if (bSendMsg) {
	m_pCarriedby->GetRoom()->SendToRoom("&LThe little black box &Wtwists, &Rswirls &Land &Btransforms&L in %s's hands!&n\r\n", m_pCarriedby);
	m_pCarriedby->SendToChar("&LThe little black box &Wtwists, &Rswirls &Land &Btransforms&L in YOUR hand!&n\r\n");
   }
   if (bRemoving) { //when removed put back the way it belongs
	m_strObjAlias = "little black box";
	m_strObjDesc = "&La &rlittle &Lblack &rbox&n";
	m_strObjName = "&La &rlittle &Lblack &rbox&n";
	m_strObjSittingDesc = "&La &rlittle &Lblack &rbox&L lies here.&n";
	switch (m_pCarriedby->GetClass()) {
	   case CLASS_MAGE: case CLASS_PSIONICIST: case CLASS_EMPATH: case CLASS_MINDFLAYER:
		m_pCarriedby->RemoveAffect(CCharacter::AFFECT_DI, bSendMsg);
		break;
	   case CLASS_CLERIC: case CLASS_SHAMAN:
		m_pCarriedby->RemoveAffect(CCharacter::AFFECT_STONE_SKIN, bSendMsg);
		break;
	   case CLASS_ROGUE:
		m_pCarriedby->RemoveAffect(CCharacter::AFFECT_HASTE, bSendMsg);
		break;
	   case CLASS_WARRIOR:
		m_pCarriedby->RemoveAffect(CCharacter::AFFECT_BLOODLUST, bSendMsg);
		break;
	   case CLASS_DEFILER: case CLASS_RANGER: case CLASS_PALADIN: case CLASS_ANTI_PALADIN:
	   case CLASS_DRUID:
		m_pCarriedby->RemoveAffect(CCharacter::AFFECT_AIR_SHIELD, bSendMsg);
		break;
	}
   }//when I wield it change for the class
   else {

	switch (m_pCarriedby->GetClass()) {
	   case CLASS_MAGE: case CLASS_PSIONICIST: case CLASS_EMPATH: case CLASS_MINDFLAYER:
		m_strObjAlias = "sleek blue staff";
		m_strObjDesc = "&bA &Bsleek &bblue &Wglowing &Bstaff&n";
		m_strObjName = "&bA &Bsleek &bblue &Wglowing &Bstaff&n";
		m_nVal4 = BLUDGEON;
		m_pCarriedby->AddAffect(CCharacter::AFFECT_DI, sAffect::MANUAL_AFFECT, 0, bSendMsg);
		break;
	   case CLASS_CLERIC: case CLASS_SHAMAN:
		m_strObjAlias = "glimmering gold hammer";
		m_strObjDesc = "&La &bg&Bli&wm&Wme&wr&Bin&bg &Ygold &Whammer&n";
		m_strObjName = "&La &bg&Bli&wm&Wme&wr&Bin&bg &Ygold &Whammer&n";
		m_nVal4 = CRUSH;
		m_pCarriedby->AddAffect(CCharacter::AFFECT_STONE_SKIN, sAffect::MANUAL_AFFECT, 1000, bSendMsg);
		break;
	   case CLASS_ROGUE:
		m_strObjAlias = "pulsating blood red dagger";
		m_strObjDesc = "&La &wpulsating &rblood &Rred &Ldagger&n";
		m_strObjName = "&La &wpulsating &rblood &Rred &Ldagger&n";
		m_nVal4 = PIERCE;
		m_pCarriedby->AddAffect(CCharacter::AFFECT_HASTE, sAffect::MANUAL_AFFECT, 0, bSendMsg);
		break;
	   case CLASS_WARRIOR:
		m_strObjAlias = "wickedly edged axe";
		m_strObjDesc = "&La wickedly &wedged &raxe&n";
		m_strObjName = "&La wickedly &wedged &raxe&n";
		m_nVal4 = SLASH;
		m_pCarriedby->AddAffect(CCharacter::AFFECT_BLOODLUST, sAffect::MANUAL_AFFECT, 0, bSendMsg);
		break;
	   case CLASS_DEFILER: case CLASS_RANGER: case CLASS_PALADIN: case CLASS_ANTI_PALADIN:
	   case CLASS_DRUID:
		m_strObjAlias = "diamond tipped katana";
		m_strObjDesc = "&wa &Wdiamond &wtipped &Wkatana&n";
		m_strObjName = "&wa &Wdiamond &wtipped &Wkatana&n";
		m_nVal4 = SLASH;
		m_pCarriedby->AddAffect(CCharacter::AFFECT_AIR_SHIELD, sAffect::MANUAL_AFFECT, 1000, bSendMsg);
		break;
	}
   }
}

void CObject::DelayedBlastFireball(short nAffect) {
   CCharacter *pTarget;
   POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
   while (pos) {
	pTarget = m_pInRoom->GetNextCharacter(pos);
	pTarget->SpellDefense(m_nVal1, pTarget, 41);
   }
   m_pInRoom->SendToRoom("&RThe &rfi&Rreba&rll&R &Yexplodes&R torching EVERYONE!\r\n");
   m_nState = STATE_DELETE;
}


//////////////////////////////////
//	Weapon Special Battle Proc
//

void CObject::CastMagicMissile(short nAffect) {
   CCharacter *pTarget;
   if (m_pCarriedby != NULL && (pTarget = m_pCarriedby->IsFighting())) {
	if (DIE(100) < CAST_PCT_MAGIC_MISSILE) {
	   pTarget->TakeDamage(m_pCarriedby, DIE(4), true);
	   m_pCarriedby->m_MsgManager.SpellMsg((CString) "Magic Missile", SKILL_WORKED, m_pCarriedby, pTarget);
	   if (pTarget->IsDead()) {
		m_pCarriedby->m_MsgManager.SpellMsg((CString) "Magic Missile", SKILL_KILLED, m_pCarriedby, pTarget);
		return;
	   }
	}
   }
}


//////////////////////////////////
//	Death's Matriarch Proc
//

void CObject::DeathsMatriarch(short nAffect) {
   CCharacter *pTarget;
   CString strToChar;
   if (m_pCarriedby != NULL && (pTarget = m_pCarriedby->IsFighting())) {
	if (DIE(100) < DEATHS_MATRIARCH_PCT) {
	   m_pCarriedby->GetRoom()->SendToRoom("&mDeath's Matriarch&L, carried by %s&L, &Cglows &Levilly and lashes out at %s&L!&n\r\n", m_pCarriedby, pTarget);
	   m_pCarriedby->SendToChar("&LYour &mDeath's Matriarch &Cglows &Levilly and lashes out at your target!&n\r\n");
	   strToChar.Format("&L%s&L's &mDeath's Matriarch &Cglows &Levilly and lashes out at YOU!&n\r\n", m_pCarriedby->GetRaceOrName(pTarget).cptr());
	   pTarget->SendToChar(strToChar);
	   short nTimes = m_pCarriedby->StatCheck(DEX) ? DIE(3) + 1 : DIE(6) + 1;
	   while (nTimes--) {
		m_pCarriedby->GetRoom()->SendToRoom("&mDeath's Matriarch&L bites hard into %s&L's &rflesh&L!&n\r\n", pTarget);
		pTarget->SendToChar("&mDeath's Matriarch&L bites hard into &WYOU&L!&n\r\n");
		pTarget->TakeDamage(m_pCarriedby, m_pCarriedby->GetLevel() < 50 ? DIE(50) : DIE(200), true);
		if (pTarget->IsDead()) {
		   m_pCarriedby->GetRoom()->SendToRoom("&L%s&L's Death's Matriarch&L sucks the remaining life from %s&L!&n\r\n", m_pCarriedby, pTarget);
		   strToChar.Format("&L%s&L's &mDeath's Matriarch&L sucks the remaining life from YOU!&n\r\n", m_pCarriedby->GetRaceOrName(pTarget).cptr());
		   pTarget->SendToChar(strToChar);
		   return;
		}
	   }
	}
   }
}


//////////////////////////////////
//	Divine Fate's Proc
//

void CObject::DivineFate(short nAffect) {
   CCharacter *pTarget;
   if (m_pCarriedby != NULL && (pTarget = m_pCarriedby->IsFighting())) {
	if (DIE(100) < DIVINE_FATE_PCT) {
	   if (!m_pCarriedby->IsGood()) {
		m_pCarriedby->SendToChar(m_AffectInfo[nAffect].m_strAffectRate);
		m_pCarriedby->GetRoom()->SendToRoom(m_AffectInfo[nAffect].m_strAffectRateRoom, m_pCarriedby);
		pTarget = m_pCarriedby;
	   }
	   m_pCarriedby->SendToChar(m_AffectInfo[nAffect].m_strBattleRound);
	   m_pCarriedby->GetRoom()->SendToRoom(m_AffectInfo[nAffect].m_strBattleRoom, m_pCarriedby);
	   pTarget->TakeDamage(m_pCarriedby, DIE(300) + 300, true);
	   if (pTarget->IsDead()) {
		m_pCarriedby->m_MsgManager.SpellMsg((CString) "full harm", SKILL_KILLED, m_pCarriedby, pTarget);
	   } else {
		m_pCarriedby->m_MsgManager.SpellMsg((CString) "full harm", SKILL_WORKED, m_pCarriedby, pTarget);
	   }
	}
   }
}




/////////////////////////////////////////////
//	Calls battle function for each object
//  This function may be a performance problem later!!!

void CObject::DoBattle() {
   for (short i = 0; i < TOTAL_OBJ_AFFECTS; i++) {
	if (m_Affects[i].HasAffect() && m_AffectInfo[i].m_pFncBattleRound != NULL) {
	   (this->*m_AffectInfo[i].m_pFncBattleRound)(i);
	}
   }
}

/////////////////////////////////////
//	Called from Character functions SyncWearAffects,
//	LoadObjects, and Save
//	we only do wearaffects we do NOT add ANY CHARACTER HERE!

void CObject::WearAffects(bool bRemoving) {
   if (m_pCarriedby == NULL) {
	ErrorLog << GetObjName() << " Vnum: " << GetVnum() << " trying to be worn but has a null m_pCarriedby" << endl;
	return;
   }
   if (bRemoving) {
	RemoveAffect(OBJ_AFFECT_CURRENTLY_WORN);
   } else {
	AddAffect(OBJ_AFFECT_CURRENTLY_WORN, sAffect::MANUAL_AFFECT);
   }
   POSITION pos = ObjAffects.GetStartPosition();
   sObjectWearAffects *pAffect;
   short nChange;
   while (pos) {
	pAffect = ObjAffects.GetNext(pos);
	nChange = pAffect->Affect;
	switch (pAffect->Location) {
	   case LOC_NONE:
		break;
	   case LOC_STRADD:
		m_pCarriedby->m_pAttributes->AffectedStats[STRADD] += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_pAttributes->AffectedStats[STRADD] = RANGE(m_pCarriedby->m_pAttributes->AffectedStats[STRADD], 100, 25);
		break;
	   case LOC_STR:
		m_pCarriedby->m_pAttributes->AffectedStats[STR] += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_pAttributes->AffectedStats[STR] = RANGE(m_pCarriedby->m_pAttributes->AffectedStats[STR], 100, 25);
		break;
	   case LOC_AGI:
		m_pCarriedby->m_pAttributes->AffectedStats[AGI] += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_pAttributes->AffectedStats[AGI] = RANGE(m_pCarriedby->m_pAttributes->AffectedStats[AGI], 100, 25);
		break;
	   case LOC_DEX:
		m_pCarriedby->m_pAttributes->AffectedStats[DEX] += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_pAttributes->AffectedStats[DEX] = RANGE(m_pCarriedby->m_pAttributes->AffectedStats[DEX], 100, 25);
		break;
	   case LOC_INT:
		m_pCarriedby->m_pAttributes->AffectedStats[INT] += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_pAttributes->AffectedStats[INT] = RANGE(m_pCarriedby->m_pAttributes->AffectedStats[INT], 100, 25);
		break;
	   case LOC_WIS:
		m_pCarriedby->m_pAttributes->AffectedStats[WIS] += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_pAttributes->AffectedStats[WIS] = RANGE(m_pCarriedby->m_pAttributes->AffectedStats[WIS], 100, 25);
		break;
	   case LOC_CON:
		m_pCarriedby->m_pAttributes->AffectedStats[CON] += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_pAttributes->AffectedStats[CON] = RANGE(m_pCarriedby->m_pAttributes->AffectedStats[CON], 100, 25);
		break;
	   case LOC_CHA:
		m_pCarriedby->m_pAttributes->AffectedStats[CHA] += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_pAttributes->AffectedStats[CHA] = RANGE(m_pCarriedby->m_pAttributes->AffectedStats[CHA], 100, 25);
		break;
	   case LOC_POW:
		m_pCarriedby->m_pAttributes->AffectedStats[POW] += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_pAttributes->AffectedStats[POW] = RANGE(m_pCarriedby->m_pAttributes->AffectedStats[POW], 100, 25);
		break;
	   case LOC_LUCK:
		m_pCarriedby->m_pAttributes->AffectedStats[LUCK] += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_pAttributes->AffectedStats[LUCK] = RANGE(m_pCarriedby->m_pAttributes->AffectedStats[LUCK], 100, 25);
		break;
	   case LOC_CLASS:
		//need to overload CCharacter to be able to make a new
		//player class and load this players stuff to that one.
		//TODO
		break;
	   case LOC_LEVEL:
		m_pCarriedby->m_nLevel += bRemoving ? -nChange : nChange;
		m_pCarriedby->m_nLevel = RANGE(m_pCarriedby->GetLevel(), 1, LVL_LAST_MORTAL);
		break;
	   case LOC_AGE:
		//TODO
		break;
	   case LOC_WIEGHT:
		m_pCarriedby->m_pAttributes->AffectedWeight += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   case LOC_HEIGHT:
		m_pCarriedby->m_pAttributes->AffectedHeight += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   case LOC_MANA:
		m_pCarriedby->m_nManaPts += bRemoving ? -pAffect->Affect : pAffect->Affect;
		m_pCarriedby->m_nMaxManaPts += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   case LOC_HITPOINTS:
		m_pCarriedby->m_nCurrentHpts += bRemoving ? -pAffect->Affect : pAffect->Affect;
		m_pCarriedby->m_nMaxHpts += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   case LOC_MOVE:
		m_pCarriedby->m_nMovePts += bRemoving ? -pAffect->Affect : pAffect->Affect;
		m_pCarriedby->m_nMaxMovePts += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   case LOC_EXP:
		m_pCarriedby->m_nExperience += bRemoving ? -pAffect->Affect : pAffect->Affect;
		if (m_pCarriedby->GetExp((m_pCarriedby->GetLevel() + 1 > LVL_GREATER_GOD) ? LVL_GREATER_GOD : m_pCarriedby->GetLevel() + 1) < m_pCarriedby->m_nExperience) {
		   m_pCarriedby->AdvanceLevel(true, true);
		} else if (m_pCarriedby->GetExp((m_pCarriedby->GetLevel() - 1 < 1) ? 1 : m_pCarriedby->GetLevel() - 1) > m_pCarriedby->m_nExperience) {
		   m_pCarriedby->AdvanceLevel(false, true);
		}
		break;
	   case LOC_AC:
		m_pCarriedby->m_nAC -= bRemoving ? -nChange : nChange;
		m_pCarriedby->m_nAC = RANGE(m_pCarriedby->m_nAC, -100, 100);
		break;
	   case LOC_HITROLL:
		m_pCarriedby->m_nToHit += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   case LOC_DAMROLL:
		m_pCarriedby->m_nDamRoll += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   case LOC_SAVE_PARA:
		m_pCarriedby->m_pAttributes->m_pnSaves[CAttributes::SAVE_PARA] += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   case LOC_SAVE_POISON:
		m_pCarriedby->m_pAttributes->m_pnSaves[CAttributes::SAVE_POISON] += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   case LOC_SAVE_BREATHE:
		m_pCarriedby->m_pAttributes->m_pnSaves[CAttributes::SAVE_BREATH] += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   case LOC_SAVE_SPELL:
		m_pCarriedby->m_pAttributes->m_pnSaves[CAttributes::SAVE_SPELL] += bRemoving ? -pAffect->Affect : pAffect->Affect;
		break;
	   default:
		ErrorLog << "Bad location in CObject::WearAffects()!\r\n";
		break;
	}
   }

   if (IsAffectedBy(OBJ_AFFECT_HONE_WEAPON)) {
	if (bRemoving) {
	   m_pCarriedby->m_nToHit--;
	   m_pCarriedby->m_nDamRoll--;
	} else {
	   m_pCarriedby->m_nDamRoll++;
	   m_pCarriedby->m_nToHit++;
	}
   }
   if (IsAffectedBy(OBJ_AFFECT_MISSED_HONE)) {
	if (bRemoving) {
	   m_pCarriedby->m_nToHit++;
	   m_pCarriedby->m_nDamRoll++;
	} else {
	   m_pCarriedby->m_nToHit--;
	   m_pCarriedby->m_nDamRoll--;
	}
   }
}

////////////////////////////////////////
//	Create an object from a saved object
///////////////////////////////////////

CObject::CObject(const sObjectSave & ObjSave, CCharacter *pChar, CRoom *pRoom) {
#ifdef _DEBUG
   if (pChar && pRoom) {
	ErrorLog << "Trying to create object with both character and room ptr set!" << endl;
	assert(0);
   }
#endif
   ExtraDesc.SetDeleteData();
   ObjAffects.SetDeleteData();
   m_nVnum = ObjSave.m_nVnum;
   m_nObjType = ObjSave.m_nObjType;
   m_nVal0 = ObjSave.m_nVal0;
   m_nVal1 = ObjSave.m_nVal1;
   m_nVal2 = ObjSave.m_nVal2;
   m_nVal3 = ObjSave.m_nVal3;
   m_nVal4 = ObjSave.m_nVal4;
   m_nWeight = ObjSave.m_nWeight;
   m_nCost = ObjSave.m_nCost;
   m_nAffBit = ObjSave.m_nAffBit;
   m_nWearBit = ObjSave.m_nWearBit;

   m_nState = ObjSave.m_nState;

   m_strObjAlias = ObjSave.m_strObjAlias;
   m_strObjName = ObjSave.m_strObjName;
   m_strObjDesc = ObjSave.m_strObjDesc;
   m_strObjSittingDesc = ObjSave.m_strObjSittingDesc;

   Set(pChar, pRoom);

   int i;
   for (i = 0; i < TOTAL_OBJ_AFFECTS; i++) {
	m_Affects[i] = ObjSave.m_Affects[i];
   }
   for (i = 0; i < MAX_EXTRA_DESC && !ObjSave.m_ExtraDesc[i].IsEmpty(); i++) {
	ExtraDesc.Add(new CObjectDesc(ObjSave.m_ExtraDesc[i].GetKeyWord(), ObjSave.m_ExtraDesc[i].GetExtraDesc()));
   }
   for (i = 0; i < MAX_WEAR_AFFECTS && !ObjSave.m_WearAffects[i].IsEmpty(); i++) {
	ObjAffects.Add(new sObjectWearAffects(ObjSave.m_WearAffects + i));
   }
}

CString CObject::GetIdentifyString() {
   CString strIdent, strType, strTypeInfo = "";
   switch (m_nObjType) {
	case ITEM_TYPE_LIGHT:
	   strType = "Light";
	   break;
	case ITEM_TYPE_SCROLL:
	   strType = "Scroll";
	   break;
	case ITEM_TYPE_STAFF:
	   strType = "Staff";
	   break;
	case ITEM_TYPE_ARMOR:
	   strType = "Armor";
	   strTypeInfo.Format("AC Adjustment: %d", GetACChange());
	   break;
	case ITEM_TYPE_CONTAINER: case ITEM_TYPE_FOUNTAIN:
	   strType = "Container";
	   break;
	case ITEM_TYPE_WEAPON:
	   strType = "Weapon";
	   strTypeInfo.Format("Weapon Damage: %dD%d vs Med, %dD%d vs Large", GetNoOfDice(SIZE_MEDIUM),
		   GetDiceType(SIZE_MEDIUM), GetNoOfDice(SIZE_LARGE), GetDiceType(SIZE_LARGE));
	   break;
	case ITEM_TYPE_POTION:
	   strType = "Potion";
	   break;
	case ITEM_TYPE_PORTAL:
	   strType = "Portal";
	   break;
	default:
	   strType = "Not Known";
	   break;
   }
   strIdent.Format("Name: %s \r\nAlias: %s \r\nCost: %d \r\nWeight: %d \r\nType: %s \r\n%s \r\n",
	   m_strObjName.cptr(), m_strObjAlias.cptr(), m_nCost, m_nWeight, strType.cptr(), strTypeInfo.cptr());

   return strIdent;
}

///////////////////////////////////
//	if the wear affects function
//	is told to add object character affects
//	then this function is call.  We always add
//	or remove the character affects unless we are
//	loading or saving the character

void CObject::AddCharAffects(bool bRemoving, bool bSendMsg) {
   if (m_pCarriedby == NULL) {
	ErrorLog << "Addchar affects is trying to be called with a null m_pcarriedby " << GetObjName() << " Vnum : " << GetVnum() << endl;
	return;
   }
   if (IsAffectedBy(OBJ_AFFECT_HASTE)) {
	if (bRemoving)
	   m_pCarriedby->RemoveAffect(CCharacter::AFFECT_HASTE, bSendMsg);
	else
	   m_pCarriedby->AddAffect(CCharacter::AFFECT_HASTE, sAffect::MANUAL_AFFECT, 0, bSendMsg);
   }
   if (IsAffectedBy(OBJ_AFFECT_HIDE)) {
	if (bRemoving)
	   m_pCarriedby->RemoveAffect(CCharacter::AFFECT_HIDING, bSendMsg);
	else
	   m_pCarriedby->AddAffect(CCharacter::AFFECT_HIDING, sAffect::MANUAL_AFFECT, 0, bSendMsg);
   }
   if (IsAffectedBy(OBJ_AFFECT_COLD_SHIELD)) {
	if (bRemoving)
	   m_pCarriedby->RemoveAffect(CCharacter::AFFECT_COLD_SHIELD, bSendMsg);
	else
	   m_pCarriedby->AddAffect(CCharacter::AFFECT_COLD_SHIELD, sAffect::MANUAL_AFFECT, 0, bSendMsg);
   }
   if (IsAffectedBy(OBJ_AFFECT_DETECT_INVIS)) {
	if (bRemoving)
	   m_pCarriedby->RemoveAffect(CCharacter::AFFECT_DI, bSendMsg);
	else
	   m_pCarriedby->AddAffect(CCharacter::AFFECT_DI, sAffect::MANUAL_AFFECT, 0, bSendMsg);
   }
   if (IsAffectedBy(OBJ_AFFECT_AIR_SHIELD)) {
	if (bRemoving)
	   m_pCarriedby->RemoveAffect(CCharacter::AFFECT_AIR_SHIELD, bSendMsg);
	else
	   m_pCarriedby->AddAffect(CCharacter::AFFECT_AIR_SHIELD, sAffect::MANUAL_AFFECT, (750 + DIE(250)), bSendMsg);
   }
   if (IsAffectedBy(OBJ_AFFECT_CONTINUAL_LIGHT)) {
	if (m_pCarriedby != NULL) {
	   if (bRemoving) {
		m_pCarriedby->RemoveAffect(CCharacter::AFFECT_LIGHT);
	   } else {
		m_pCarriedby->AddAffect(CCharacter::AFFECT_LIGHT, sAffect::MANUAL_AFFECT, m_Affects[OBJ_AFFECT_CONTINUAL_LIGHT].m_nValue, bSendMsg);
	   }
	}
   }
   if (IsAffectedBy(OBJ_AFFECT_BOX)) {
	BlackBox(bRemoving, bSendMsg);
   }
}

///////////////////////////////////
//	I believe you can only look at an object
//  if it is in your possession therefore
//	m_pCarriedby should be set so we'll 
//	sent it to m_pcarriedBy

void CObject::DoLook(CString &str) {
   assert(m_pCarriedby);
   str.Format("%sYou look closely at %s\r\n",
	   str.cptr(),
	   m_strObjDesc.cptr());
}

///////////////////////////////////
//	Save
//	We get Save file name via a function go 
//	we can get different names to allow us to save
//	character objects, corpses, ect.

void CObject::Save(std::ofstream &s, int nWornOn, short nPos) {
   if (!s) {
	CString str;
	str.Format("Can't open object file for player: %s", m_pCarriedby->GetName().cptr());
	CError Err(NON_CRITICAL_ERROR, str.cptr());
	throw &Err;
   } else {
	sObjectSave ObjSave(this, nWornOn, nPos);
	//if eq pos is not empty build CObjecSave and save eq
	s.write((char *) &ObjSave, sizeof(sObjectSave));
	//Save any contents of this object
	//this could be the starting point of a recurisive call
	SaveContents(s);
   }
}

////////////////////////
//	Base class for loot
//	here we just say you can only loot corpses
//

void CObject::Loot(CCharacter *pCh, bool bFromRess) {
   pCh->SendToChar("You can only loot corpses\r\n");
}

///////////////////////////
//	base class for loot
//	here we just say you can only loot corpses

void CObject::Loot(CString str, short nHowMany, CCharacter *pCh) {
   pCh->SendToChar("You can only loot corpses\r\n");
}

////////////////////////////////////////
//	Populate string with affect wording etc
//

CString CObject::GetExtraString() {
   CString str;
   if (IsAffectedBy(OBJ_AFFECT_CONTINUAL_LIGHT)) {
	str += " &Wglowing&n";
   }
   if (IsAffectedBy(OBJ_AFFECT_INVISIBLE)) {
	str += " &binvis&n";
   }
   if (IsAffectedBy(OBJ_AFFECT_HIDDEN)) {
	str += " &lHidden&n";
   }
   return str;
}

///////////////////////////////////
//	Canbeworn by 
//	Check to see if item its anti to the character

bool CObject::CanbeWornBy(const CCharacter *pCh) {
   int nBits = 0;
   //check alignment
   if (pCh->IsGood()) {
	nBits |= ITEM_ANTI_GOOD;
   } else if (pCh->IsEvil()) {
	nBits |= ITEM_ANTI_EVIL;
   } else {
	nBits |= ITEM_ANTI_NEUTRAL;
   }

   //check race war
   if (pCh->IsRace(CRacialReference::RACE_NEUTRAL)) {
	nBits |= ITEM_ANTI_NEUTRAL_RACE;
   } else if (pCh->IsRace(CRacialReference::RACE_ALIEN)) {
	nBits |= ITEM_ANTI_ALIEN_RACE;
   } else if (pCh->IsRace(CRacialReference::RACE_EVIL)) {
	nBits |= ITEM_ANTI_EVIL_RACE;
   } else {
	nBits |= ITEM_ANTI_GOOD_RACE;
   }

   //check class
   //there has got to be a better way to do this...
   //but I can't think of it.  I hate having 3 switch statements
   //for class dependant stuff.  CalcRaceClass() (2 overloads)
   //and here
   switch (pCh->GetClass()) {
	case CLASS_WARRIOR:
	   nBits |= ITEM_ANTI_WARRIOR;
	   break;
	case CLASS_ROGUE:
	   nBits |= ITEM_ANTI_ROGUE;
	   break;
	case CLASS_MAGE:
	   nBits |= ITEM_ANTI_MAGE;
	   break;
	case CLASS_PSIONICIST:
	   nBits |= ITEM_ANTI_PSIONICIST;
	   break;
	case CLASS_MINDFLAYER:
	   nBits |= ITEM_ANTI_MINDFLAYER;
	   break;
	case CLASS_EMPATH:
	   nBits |= ITEM_ANTI_EMPATH;
	   break;
	case CLASS_CLERIC:
	   nBits |= ITEM_ANTI_CLERIC;
	   break;
	case CLASS_SHAMAN:
	   nBits |= ITEM_ANTI_SHAMAN;
	   break;
	case CLASS_DRUID:
	   nBits |= ITEM_ANTI_DRUID;
	   break;
	case CLASS_PALADIN:
	   nBits |= ITEM_ANTI_PALADIN;
	   break;
	case CLASS_ANTI_PALADIN:
	   nBits |= ITEM_ANTI_ANTIPALADIN;
	   break;
	case CLASS_DEFILER:
	   nBits |= ITEM_ANTI_DEFILER;
	   break;
	case CLASS_RANGER:
	   nBits |= ITEM_ANTI_RANGER;
	   break;
	default:
	   ErrorLog << "default in CanbeWornBy" << endl;
	   break;
   }
   if (IsAffectBit(nBits)) {
	CString strToChar;
	strToChar.Format("%s rejects you flying from your hand onto the ground.\r\n",
		(char *) GetObjName());
	pCh->SendToChar(strToChar);
	strToChar.Format("%s flies from %s's hand onto the ground in a flash of light.\r\n",
		(char *) GetObjName(), "%s");
	pCh->GetRoom()->SendToRoom(strToChar, pCh);
	return false;
   }
   return true;
}

void CObject::Open(const CCharacter *pCh, bool bOpen) {
   if (bOpen) {
	pCh->SendToChar("You can only open containers!\r\n");
   } else {
	pCh->SendToChar("You can only close containers!\r\n");
   }
}

/////////////////////
//	virtual default

CRoom *CObject::Add(CCharacter *pCh) {
   pCh->SendToChar("You can't enter that!\r\n");
   return NULL;
}

///////////////////
//	Get Object's name if char can see it

const char *CObject::GetObjName(const CCharacter *pCh) {
   return(pCh->CanSee(this) ? m_strObjName.cptr() : "something");
}

//////////////////////////
//	Use default fnc
//	virtual

void CObject::Use(CCharacter *pUser) {
   pUser->SendToChar("You can't do that to that object.\r\n");
}

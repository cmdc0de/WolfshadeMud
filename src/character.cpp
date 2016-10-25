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
 *  implementaion for ccharacter
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "money.h"
#include "transport.h"
#include "shop.room.h"
#include "bank.room.h"
#include "transportroom.h"
#include "object.save.h"
#include "stdlib.h"
#ifndef _WOLFSHADE_WINDOWS_
#include <errno.h>
#endif

extern CGlobalVariableManagement GVM;
extern CRandom GenNum;

void StackObjects(const CList<CObject *> &Objs, const CCharacter *pLooker, CString &strLook, bool bInInventory);

const CMessageManager CCharacter::m_MsgManager;
bool CCharacter::m_bInit = false;
//increase GAME_VAR_AC_WOULD_HIT # to increase hit % while
//decreasing it will decrease hit %
short CCharacter::GAME_VAR_AC_WOULD_HIT = 75;
//changes the chance to dodge for anyone in the game
short CCharacter::GAME_VAR_DEFENSE_DODGE = 300;
//changes the chance to parry for anyone in the game
short CCharacter::GAME_VAR_DEFENSE_PARRY = 200;
//changes the chance for anyone to riposte in the game
short CCharacter::GAME_VAR_DEFENSE_RIPOSTE = 300;
//changes the bonus/penatly for fighting someone big or smaller
short CCharacter::GAME_VAR_SIZE_AC_ADJUST = 10;
//change to slow down or speed up skill ups
short CCharacter::DIE_ROLL_FOR_SKILL_UP = 1000; //changes are set up from 1-1000 this gives us .1 % percision

const int CCharacter::PULSES_PER_MOVEMENT_REGENERATION_TIME = (CMudTime::PULSES_PER_REAL_SECOND * 4);
const int CCharacter::PULSES_PER_REGENERATION_TIME = (CMudTime::PULSES_PER_REAL_SECOND * 6);
const int CCharacter::PULSES_PER_MANA_REGENERATION_TIME = (CMudTime::PULSES_PER_REAL_SECOND * 7);
const int CCharacter::CHECK_AGE_DEATH = CMudTime::PULSES_PER_MUD_YEAR;
const long CCharacter::CHARACTER_VOID_TIME = CMudTime::PULSES_PER_REAL_MIN * 10;
const long CCharacter::GOD_VOID_TIME = CMudTime::PULSES_PER_REAL_HOUR;
const int CCharacter::LEVELS_PER_DAMAGE_EXP_BONUS = 3; //each 3 levels higher than hitter you get a 10% exp bonus
const int CCharacter::LEVELS_PER_KILL_EXP_BONUS = 2;
const int CCharacter::LEVELS_PER_GROUP_KILL_EXP_BONUS = 2;
const int CCharacter::LEVELS_PER_DAMAGE_EXP_PENATLY = 2;
const int CCharacter::LEVELS_PER_KILL_EXP_PENATLY = 1;
const int CCharacter::LEVELS_PER_GROUP_KILL_EXP_PENATLY = 1;
//LOAD VALUES in pct
//not const so that we can change them during game play in 
//the future
short CCharacter::LOAD_WHAT_LOAD = 20;
short CCharacter::LOAD_LIGHT = 30;
short CCharacter::LOAD_MODERATE = 50;
short CCharacter::LOAD_HEAVY = 70;
short CCharacter::LOAD_VERY_HEAVY = 90;
short CCharacter::LOAD_IMMOBILIZING = 100;
//number of levels before you recieve another practicesession
const short CCharacter::LVLS_PER_PRACTICE_SESSION = 3;
const short CCharacter::MIN_SKILL_ABILITY_TO_TEACH = 10;
const short CCharacter::PC_TO_PC_TEACHER_PENATLY = 3;
const short CCharacter::MIN_SPELL_ABILITY_TO_TEACH = 20;
const short CCharacter::CHANCE_TO_SEE = 50; //%
//vision
const short CCharacter::SEES_RED_DOTS = 2;
const short CCharacter::SEES_ALL = 1;
const short CCharacter::SEES_NOTHING = 0;
const char * CCharacter::strPageInfo = "[b]egining, [n]ext page, [p]revious page, [q]uit\r\n";
const char *CCharacter::CastingMsg[] = {
  "Casting: %s :\r\n",
  "Casting: %s :*\r\n",
  "Casting: %s :**\r\n",
  "Casting: %s :***\r\n",
  "Casting: %s :****\r\n",
  "Casting: %s :*****\r\n",
  "Casting: %s :******\r\n",
  "Casting: %s :*******\r\n",
  "Casting: %s :********\r\n",
  "Casting: %s :*********\r\n",
  "Casting: %s :**********\r\n",
  "Casting: %s :***********\r\n",
  "Casting: %s :************\r\n",
  "Casting: %s :*************\r\n",
  "Casting: %s :**************\r\n",
  "Casting: %s :***************\r\n",
  "Casting: %s :****************\r\n",
  "Casting: %s :*****************\r\n",
  "Casting: %s :******************\r\n",
  "Casting: %s :*******************\r\n",
  "Casting: %s :********************\r\n",
  "Casting: %s :*********************\r\n",
  "Casting: %s :**********************\r\n",
  "Casting: %s :***********************\r\n",
  "Casting: %s :************************\r\n",
  "Casting: %s :*************************\r\n",
  "Casting: %s :**************************\r\n",
  "Casting: %s :***************************\r\n",
  "Casting: %s :****************************\r\n",
  "Casting: %s :*****************************\r\n"
};

//constuctor used to make a mob

CCharacter::CCharacter(CMobPrototype &mob, CRoom *pPutInRoom)
: CCharacterAttributes(mob, pPutInRoom) {
  InitStatics();
  m_bMakeCorpseOnDeath = true;
  m_lVoidTime = 0;
  //CTrack info Made in CNPC class constructor
  m_nMaxHpts = m_nCurrentHpts = mob.GetMinHits();
  //set max and current hpts to min hits then
  //mob manager advance mob to correct level.
  //why not just do it all here?  
  //well advance level calls a pure virutal function
  //and you can't call a pure virutal from the constructor
  m_pDescriptor = NULL;
  m_pInRoom = pPutInRoom;
  assert(m_pInRoom);
  if (mob.GetAC()<(100 - m_pAttributes->GetACAdjust())) {
	m_nAC = mob.GetAC();
  } else {
	m_nAC = 100 - m_pAttributes->GetACAdjust();
  }
  m_Equipment = new CEqInfo[MAX_EQ_POS];
  m_nDamRoll = m_pAttributes->GetDamageBonus();
  m_nToHit = m_pAttributes->GetToHitBonus();
  m_pFighting = m_pSpiedby = m_pSpyingOn = NULL;
  m_lLagTime = 0;
  m_nWeightCarried = 0;
  m_pSwitchedTo = NULL;
}

//constructor of players entering the game
// Here we only initialize values ....we call all affects and virtual functions
// from initializecharacter()

CCharacter::CCharacter(CCharIntermediate *ch)
: CCharacterAttributes(ch) {
  InitStatics();
  m_bMakeCorpseOnDeath = true;
  m_lVoidTime = 0;
  m_pDescriptor = ch->m_pDesc;
  if (!(m_pInRoom = CWorld::FindRoom(ch->GetLastRentRoom()))) {
	if (!(m_pInRoom = CWorld::FindRoom(m_pAttributes->GetHomeTown()))) {
	  assert(0);
	}
	ErrorLog << m_strName << " rent room could not be found. Room # is " << m_lLastRentRoom << endl;
  }
  m_Equipment = new CEqInfo[MAX_EQ_POS];
  //Make a track info
  m_pTrackInfo = new CTrackInfo();
  //can't call resetcharvars b/c it's virtual
  m_nAC = 100 - m_pAttributes->GetACAdjust();
  m_nDamRoll = m_pAttributes->GetDamageBonus();
  m_nToHit = m_pAttributes->GetToHitBonus();

  m_pSwitchedTo = m_pFighting = m_pSpiedby = m_pSpyingOn = NULL;
  m_lLagTime = 0;
  m_nWeightCarried = 0;
}

CCharacter::~CCharacter() {
  delete [] m_Equipment;
  delete m_pTrackInfo;
  //do not delete descriptor
  //we do it for npc in npc class
}

//////////////////////////////////////////
//	Virtual Function clean up
//	Use to be in destructor but
//	we had problems still needing the polymorism
//	during destruction, and that just won't work =)
//	AKA. "A None stops following you" because the virtual function
//	IsNPC() won't work correctly in the desturctor.
//	So this function is called before any character is removed from the game.
//	written by: Demetrius Comes

void CCharacter::CleanUp() {
  if (m_Group.IsInGroup(this)) {
	m_Group.Remove(this);
  }
  if (IsBeingSpied()) {
	CString strToChar;
	strToChar.Format("Your spy target %s has left the game.\r\n",
		m_strName.cptr());
	m_pSpiedby->SendToChar(strToChar);
	m_pSpiedby->m_pSpyingOn = NULL;
	m_pSpiedby = NULL;
  }
  if (IsSpying()) {
	m_pSpyingOn->m_pSpiedby = NULL;
	m_pSpyingOn = NULL;
  }
  if (IsSwitched()) {
	DoReturn();
  }
  m_pTrackInfo->RemoveAllTrackTargets(this);
  m_pTrackInfo->RemoveFromAllTrackers(this);
  m_Master.CleanUp(this);
  m_Consent.CleanUp(this);
  m_Follow.CleanUp(this);
  //if they don't leave a corpse drop their objects in the room
  if (m_bMakeCorpseOnDeath == false) {
	PutAllEqInRoom();
  }
}

////////////////////////////////////////
//	Removes all Eq from a players and puts it in the room
//	they are currently in...used for !makecorpse
//	and res

void CCharacter::PutAllEqInRoom() {
  CObject *pObj;
  for (int i = 0; i < MAX_EQ_POS; i++) {
	if (!m_Equipment[i].IsEmpty()) {
	  GetRoom()->Add(RemoveEquipment(i, false));
	}
  }
  POSITION pos = this->Inventory.GetStartPosition();
  while (pos) {
	pObj = this->Inventory.GetNext(pos);
	GetRoom()->Add(pObj);
	Remove(pObj, pos);
  }
}

CEqInfo &CCharacter::GetEq(short nPos) {
  return m_Equipment[nPos];
}

bool CCharacter::IsNPC() const {
  return false;
}

//called by all to look at objects, characters, ect

void CCharacter::DoLook() {
  CString SubCommand, strToChar;
  CCharacter *pLookat;
  CObject *pObj;

  SubCommand = CurrentCommand.GetWordAfter(1);
  if (SubCommand.IsEmpty()) {
	SendToChar(GetRoom()->DoRoomLook(this));
	return;
  }

  int nDot, nNum = 1;
  if ((nDot = SubCommand.Find('.')) != -1) {
	nNum = SubCommand.Left(nDot).MakeInt();
	SubCommand = SubCommand.Right(nDot);
  }

  if ((pLookat = m_pInRoom->FindCharacterByAlias(SubCommand, nNum, this)) != NULL) {
	DoCharacterLook(pLookat);
  }//this needs to check inventory and room!!!!!!
  else if (SubCommand.Compare("in")) {
	bool bDummy;
	if ((pObj = GetTarget(2, true, bDummy))) {
	  CString strContents;
	  if (pObj->IsType(ITEM_TYPE_CONTAINER)) {
		if (((CContainer *) pObj)->IsOpen()) {
		  pObj->DoLook(strContents);
		  pObj->GetContents(this, strContents);
		  SendToChar(strContents);
		} else {
		  SendToChar("You have to open it first.\r\n");
		}
	  } else {
		SendToChar("You can only look in containers!\r\n");
	  }
	} else {
	  SendToChar("Look in what?\r\n");
	}
  } else if ((pObj = FindInInventory(SubCommand, nNum)) != NULL) {
	CString strContents;
	pObj->DoLook(strContents);
	SendToChar(strContents);
  } else {
	SendToChar("Look at what?\r\n");
  }
  return;
}

void CCharacter::SendToChar(CString Message, bool bColorize) const {
  if ((!IsLinkDead() && !IsSwitched())
	  || (!IsLinkDead() && IsSwitched() && Prefers(PREFERS_TELLS_WHEN_SWITCHED))) {
	if (bColorize) {
	  Message.Colorize(Prefers(PREFERS_COLOR));
	}
	if (Prefers(PREFERS_EXTRA_RETURNS)) {
	  Message += "\r\n";
	}
	SendToSocket(Message);
	if (IsBeingSpied()) {
	  CString strToChar;
	  strToChar.Format("&WSpy Info of %s.&n\r\n",
		  GetName().cptr());
	  m_pSpiedby->SendToChar(strToChar);
	  m_pSpiedby->SendToChar(Message, false); //no sense in colorizing it twice!
	  m_pSpiedby->SendToChar(CString("&WEnd spy info*****&n\r\n"));
	}
  }
}

/////////////////////////////////
//	HasCommand
//	Checks to see if the player is lagged
//	if so check to see if it is because
//	they are casting...if they are casting
//	check to see if there target is still in the
//	room...if they are then send stars...we return
//	false so that we don't move command out of the
//	command buffer and into currentcommand string.
//	because when lag time is up we will process the command
//	the second time and actually cast instead of just setting
//	up _CURRENT_SPELL struct.
//	written by: Demetrius Comes
//////////////////////////////////////

bool CCharacter::HasCommand() {
  FadeAffects(1); //1 for 1 pulse

  if (--m_lLagTime > 0) {
	return false;
  }

  //reset lag time
  m_lLagTime = 0;
  //if we are casting just return so we process the
  //command this is currently in the currentcommand string
  if (IsAffectedBy(AFFECT_CASTING)) {
	return true;
  }

  //this can be an else if b/c if they are sitting flee will stand them up
  if (this->IsNPC()
	  && InPosition(POS_FIGHTING)
	  && !InPosition(POS_STANDING)
	  && CommandBufEmpty()) {
	AddCommand(CString("stand"));
  }
  if (InPosition(POS_SLEEPING)) {
	if (InPosition(POS_STANDING)) {
	  if (DIE(100) < 30) {
		this->TakeDamage(this, DIE(4), true, false);
		SendToChar("You awake with a thud!\r\n");
		GetRoom()->SendToRoom("%s awakes with a thud!\r\n", this);
		RemovePosition(POS_SLEEPING);
		UpDatePosition(POS_RECLINING, false);
	  }
	} else if (InPosition(POS_SITTING)) {
	  if (DIE(100) < 10) {
		this->TakeDamage(this, 1, true, false);
		SendToChar("You slump down but keep snoozing.\r\n");
		GetRoom()->SendToRoom("%s slumps to the ground but keeps sleeping.\r\n", this);
		m_nPosition = POS_RECLINING | POS_SLEEPING;
		UpDatePosition();
	  }
	}
  }

  if (IsLinkDead())
	return false;

  int returnat;
  //is there a return carrage in the command buffer
  if ((returnat = m_pDescriptor->CommandBuf.Find('\n', '\r')) == -1)
	return false;

  //we have a command so reset void timer
  this->m_lVoidTime = 0;

  CString tmp(m_pDescriptor->CommandBuf.Left(returnat));
  //remove command from buffer
  m_pDescriptor->CommandBuf = m_pDescriptor->CommandBuf.Right(returnat + 1);

  tmp.SkipSpaces();
  if (!IsPlaying()) {
	CurrentCommand = tmp;
	DoOtherState();
	EndCurrentCommand();
	return false;
  } else {
	if (tmp.IsEmpty()) {
	  EndCurrentCommand();
	  return false;
	} else if (tmp[0] == '!') {
	  CurrentCommand = LastCommand;
	} else {
	  CurrentCommand = tmp;
	}
  }
  //return true so we process the command
  return true;
}

void CCharacter::SendToChar(char * Message, bool bColorize) const {
  CString strToChar(Message);
  SendToChar(strToChar, bColorize);
}
//puts current command as last command

void CCharacter::EndCurrentCommand() {
  LastCommand = CurrentCommand;
  SendPrompt();
  CurrentCommand.Empty();
}

//looks at an character

void CCharacter::DoCharacterLook(CCharacter *ch) {
  CString strLook;
  if (ch->IsNPC()) {
	strLook.Format("%s\r\n%s\r\n%s [%s] has a condition of %s\r\n", ch->GetName().cptr(), ch->GetLongDescription().cptr(),
		ch->GetName().cptr(), GetSizeName(ch->m_pAttributes->GetSize()), ch->GetCondition());
  } else if (this->CanSeeName(ch)) {
	strLook.Format("%s\r\n%s\r\n%s is a %s [%s] and has a condition of %s\r\n", ch->GetRaceOrName(ch).cptr()
		, ch->GetLongDescription().cptr(),
		ch->GetName().cptr(), GVM.GetColorRaceName(ch->m_pAttributes->GetRace()),
		GetSizeName(ch->m_pAttributes->GetSize()), ch->GetCondition());
  } else {
	strLook.Format("%s\r\n%s\r\n%s [%s] has a condition of %s\r\n",
		ch->GetRaceOrName(this).cptr(),
		ch->GetLongDescription().cptr(),
		ch->GetRaceOrName(this).cptr(),
		GetSizeName(ch->m_pAttributes->GetSize()),
		ch->GetCondition());
  }
  ch->AddAffectString(strLook);
  strLook += "\r\n";
  ch->EqList(this, strLook);
  if (IsGod()) {
	CString strInventory;
	strInventory.Format("You use your godly powers to peek into %s inventory.\r\n",
		CCharacterAttributes::GetPronoun(ch->GetSex()));
	StackObjects(ch->Inventory, this, strInventory, true);
	if (strInventory.IsEmpty()) {
	  strInventory = "Nothing\r\n";
	}
	strLook += strInventory;
  }
  SendToChar(strLook);
  if (this == ch) {
	GetRoom()->SendToRoom("%s looks at %self.\r\n", this, GetPronoun(GetSex()));
  } else {
	CString strToChar;
	strToChar.Format("%s looks at you.\r\n",
		GetRaceOrName(ch).cptr());
	ch->SendToChar(strToChar);
	GetRoom()->SendToRoom("%s looks at %s\r\n", this, ch);
  }
}

//glances at a character

void CCharacter::DoGlance() {
  CString strLook;
  CCharacter *pTarget = GetTarget();
  if (!pTarget) {
	return;
  }
  strLook.Format("%s [%s] has a condition of %s\r\n",
	  pTarget->GetRaceOrName(this).cptr(),
	  GetSizeName(pTarget->m_pAttributes->GetSize()),
	  pTarget->GetCondition());
  pTarget->AddAffectString(strLook);
  SendToChar(strLook);
}

///////////////////////////
//	Adds to string all the affects that 
//	have look messages.

void CCharacter::AddAffectString(CString &strLook) {
  CString strAdd;
  if (IsAffectedBy(AFFECT_STONE_SKIN)) {
	strAdd.Format("&L%s's skin is made of stone!&n\r\n", (char *) GetPronoun(GetSex()));
	strLook += strAdd;
  }
  if (IsAffectedBy(AFFECT_AIR_SHIELD)) {
	strAdd.Format("&WA pulsating shield of air surrounds %s body!&n\r\n", (char *) GetPronoun(GetSex()));
	strLook += strAdd;
  }
  if (IsAffectedBy(AFFECT_COLD_SHIELD)) {
	strAdd.Format("&B%s body is encased in killing ice!&n\r\n", (char *) GetPronoun(GetSex()));
	strLook += strAdd;
  }
  if (IsAffectedBy(AFFECT_FIRE_SHIELD)) {
	strAdd.Format("&R%s body is surrounded by burning flames!&n\r\n", (char *) GetPronoun(GetSex()));
	strLook += strAdd;
  }
  if (IsAffectedBy(AFFECT_HEAT_SHIELD)) {
	strAdd.Format("&R%s body is surrounded by a &yhaze of heat!&n\r\n", (char *) GetPronoun(GetSex()));
	strLook += strAdd;
  }
  if (IsAffectedBy(AFFECT_BIOFEEDBACK)) {
	strAdd.Format("&G%s body is encased in a glowing mist!&n\r\n", (char *) GetPronoun(GetSex()));
	strLook += strAdd;
  }
  if (IsAffectedBy(AFFECT_GUKS_GLOWING_GLOBES)) {
	strAdd.Format("&R%d globes float around %s body!&n\r\n", m_Affects[AFFECT_GUKS_GLOWING_GLOBES].m_nValue, (char *) GetPronoun(GetSex()));
	strLook += strAdd;
  }
  if (IsAffectedBy(AFFECT_FAERIE_FIRE)) {
	strAdd.Format("&m%s body is surrounded by a dull glowing fire!&n\r\n", (char *) GetPronoun(GetSex()));
	strLook += strAdd;
  }

}

////////////////////////////////
//	DoMove 
//	this function has been rewritten serveral times
//	Now we only take a directional string...it should have the
//	entire word (e.g. "north", "south", etc)
//	we then use the Croom function CanEnter to insure we can
//	enter the run we are trying to get in...such as no doors are closed
//	the room isn't a GOD_ROOM and your not a god etc
//	written by: Demetrius Comes
//	8-24-98
////////////////////////////////////////////

void CCharacter::DoMove(CString strDirection, CObject *pObjBeingDraged) {
  CRoom *pToRoom;
  CString strReverse;
  //can enter checks for doors, affects characters may need (like fly etc)
  if (!this->IsAffectedBy(AFFECT_CASTING)
	  && (pToRoom = GetRoom()->CanEnter(this, strDirection, strReverse)) != NULL) {
	short nCost = pToRoom->GetMovementCost();
	short nLoadPct = GetLoadPct(pObjBeingDraged);

	if (nLoadPct >= LOAD_IMMOBILIZING) {
	  SendToChar("The weight of all your items is keeping you from moving.\r\n");
	  return;
	} else if (nLoadPct >= LOAD_VERY_HEAVY)
	  nCost <<= 1;
	else if (nLoadPct >= LOAD_HEAVY)
	  nCost += (nCost >> 1);
	else if (nLoadPct <= LOAD_WHAT_LOAD)
	  nCost >>= 1;
	else if (nLoadPct <= LOAD_LIGHT)
	  nCost -= (nCost >> 1);
	//if your crawling * by 2
	if (!InPosition(POS_STANDING)) {
	  nCost <<= 1;
	}
	//reduce again if flying
	if (IsAffectedBy(AFFECT_FLYING)) {
	  nCost >>= 1;
	}
	nCost = nCost > 0 ? nCost : 0;
	//never take them below 0
	//per john's request
	if ((m_nMovePts - nCost) <= 0) {
	  SendToChar("You are too exhausted to go on!\r\n");
	} else {
	  m_nMovePts -= nCost;
	  m_nMovePts = m_nMovePts<-100 ? -100 : m_nMovePts;

	  CString strToChar, strPrep, strToRoom("%s ");
	  CRoom *pOldRoom = m_pInRoom;
	  m_pInRoom->Remove(this);
	  if (!IsAffectedBy(AFFECT_SNEAKING)) {
		if (pObjBeingDraged != NULL) {
		  strPrep.Format("leaves %s dragging %s behind %s.\r\n", strDirection.cptr(),
			  pObjBeingDraged->GetObjName(),
			  CCharacterAttributes::GetPronoun(GetSex()));

		  strToChar.Format("You leave %s dragging %s behind you.\r\n",
			  strDirection.cptr(),
			  pObjBeingDraged->GetObjName(this));
		  SendToChar(strToChar);
		} else {
		  strPrep.Format("%s %s.\r\n",
			  (InPosition(POS_STANDING) ? "leaves" : "crawls"),
			  strDirection.cptr());
		  strToChar.Format("You %s %s.\r\n",
			  (InPosition(POS_STANDING) ? "leave" : "crawl"),
			  strDirection.cptr());
		  SendToChar(strToChar);
		}
		strToRoom += strPrep;
		m_pInRoom->SendToRoom(strToRoom, this);
	  }
	  //set room pts
	  m_pInRoom = pToRoom;
	  //add player to new room list
	  this->GetRoom()->Add(this);
	  //move the object to this room
	  if (pObjBeingDraged != NULL) {
		pObjBeingDraged->GetRoom()->Remove(pObjBeingDraged);
		this->GetRoom()->Add(pObjBeingDraged);
	  }

	  SendToChar(GetRoom()->DoRoomLook(this));

	  //reset str for room
	  if (!IsAffectedBy(AFFECT_SNEAKING)) {
		strToRoom = "%s ";
		if (strReverse.Compare("below") || strReverse.Compare("above")) {
		  if (pObjBeingDraged != NULL) {
			strPrep.Format("enters from %s dragging %s behind %s.\r\n",
				strReverse.cptr(),
				pObjBeingDraged->GetObjName(),
				CCharacterAttributes::GetPronoun(GetSex()));
		  } else {
			strPrep.Format("%s from %s.\r\n",
				(InPosition(POS_STANDING) ? "enters" : "crawls in"),
				strReverse.cptr());
		  }
		} else {
		  if (pObjBeingDraged != NULL) {
			strPrep.Format("enters from the %s dragging %s behind %s.\r\n",
				strReverse.cptr(),
				pObjBeingDraged->GetObjName(),
				CCharacterAttributes::GetPronoun(GetSex()));
		  } else {
			strPrep.Format("%s from the %s.\r\n",
				(InPosition(POS_STANDING) ? "enters" : "crawls in"),
				strReverse.cptr());
		  }
		}
		strToRoom += strPrep;
		m_pInRoom->SendToRoom(strToRoom, this);
	  }

	  POSITION pos = m_Follow.m_Followers.GetStartPosition();
	  CCharacter *pFollower;
	  while (pos) {
		pFollower = m_Follow.m_Followers.GetNext(pos);
		if (pOldRoom->IsInRoom(pFollower) && pFollower->InPosition(POS_STANDING)
			&& !pFollower->InPosition(POS_FIGHTING)
			&& !pFollower->IsAffectedBy(AFFECT_CASTING)) {
		  //send message to follower
		  strToChar.Format("You follow %s\r\n",
			  pFollower->m_Follow.m_pCharacterBeingFollowed->GetRaceOrName(this).cptr());
		  pFollower->SendToChar(strToChar);

		  //now we use recursion.
		  RemoveAffect(AFFECT_HIDING);
		  pFollower->DoMove(strDirection);
		  pFollower->SendPrompt();
		}
	  }
	}
  }
}

///////////////////////////
//	DoGet()
//	gets an object from the room
//	as well as other objects

void CCharacter::DoGet() {
  CObject *pObj;
  CString Com(CurrentCommand.GetWordAfter(1));
  bool bAll = false;
  int nDot, nNum = 1;
  if ((nDot = Com.Find('.')) != -1) {
	nNum = Com.Left(nDot).MakeInt();
	if (nNum == -1 && Com.Left(nDot).Compare("all")) {
	  bAll = true;
	}
	Com = Com.Right(nDot);
  }
  //if 3rd word is empty them we are getting from room
  if (CurrentCommand.GetWordAfter(2).IsEmpty()) {
	//meaning we are getting somethign from the room
	if (Com.Compare("all")) {
	  if (!m_pInRoom->m_CashInRoom.IsEmpty()) {
		m_CashOnHand += m_pInRoom->m_CashInRoom;
		m_pInRoom->m_CashInRoom = 0;
		m_pInRoom->SendToRoom("%s gets a pile of coins.\r\n", this);
	  }
	  POSITION pos = m_pInRoom->m_ObjInRoom.GetStartPosition();
	  while (pos) {
		pObj = m_pInRoom->m_ObjInRoom.GetNext(pos);
		try {
		  GetSingleItem(pObj);
		  m_pInRoom->m_ObjInRoom.Remove(pObj, pos);
		} catch (CError *pError) {
		  if (pError->IsType(STOP_ERROR)) {
			pos = NULL;
		  }
		  SendToChar(CString(pError->GetMessage()));
		}
	  }
	} else if (sMoney::IsCash(Com)) {
	  try {
		m_pInRoom->m_CashInRoom.FromString(Com, m_CashOnHand, nNum);
	  } catch (CError *pErr) {
		SendToChar(CString(pErr->GetMessage()));
	  } catch (char *strMoney) {
		CString strToChar;
		strToChar.Format("You get some %s.\r\n", strMoney);
		SendToChar(strToChar);
		m_pInRoom->SendToRoom("%s gets some %s.\r\n", this, CString(strMoney));
	  }
	} else if ((pObj = GetTarget(1, false, bAll))) {
	  if (bAll) {
		do {
		  try {
			GetSingleItem(pObj);
			m_pInRoom->Remove(pObj);
		  } catch (CError *pError) {
			SendToChar(CString(pError->GetMessage()));
		  }
		} while ((pObj = GetRoom()->FindObjectInRoom(Com, 1, this)));
	  } else {
		try {
		  GetSingleItem(pObj);
		  m_pInRoom->Remove(pObj);
		} catch (CError *pError) {
		  SendToChar(CString(pError->GetMessage()));
		}
	  }
	} else {
	  SendToChar("Get what?\r\n");
	}
  } else {
	bool bDummy;
	CObject *pCon = GetTarget(2, true, bDummy);
	if (pCon) {
	  if (pCon->IsType(ITEM_TYPE_CONTAINER)) {
		if (!((CContainer *) pCon)->IsOpen()) {
		  SendToChar("Don't you think you should open it first?\r\n");
		} else {
		  if (Com.Compare("all")) {
			((CContainer *) pCon)->GiveContentsToCarrier();
		  } else {
			pObj = ((CContainer *) pCon)->FindInContents(Com, nNum);
			if (!pObj) {
			  SendToChar("Get what?\r\n");
			} else if (bAll) {
			  do {
				try {
				  GetObjFromObj(pObj, (CContainer *) pCon);
				} catch (CError *pError) {
				  SendToChar(CString(pError->GetMessage()));
				  if (pError->IsType(STOP_ERROR)) {
					break;
				  }
				}
			  } while (((CContainer *) pCon)->FindInContents(Com, 1));
			} else {
			  try {
				GetObjFromObj(pObj, (CContainer *) pCon);
			  } catch (CError *pError) {
				SendToChar(CString(pError->GetMessage()));
			  }
			}
		  }
		}
	  } else {
		SendToChar("You can't get anything from that.\r\n");
	  }
	} else {
	  SendToChar("Get from what?\r\n");
	}
  }
}

////////////////////////////
//	getobj from another object

void CCharacter::GetObjFromObj(CObject *pObj, CContainer *pCon) {
  //insure it's in the container
  //and we are not trying to get an object from itself
  if (!pCon->Contains(pObj) || pObj == pCon) {
	CError Err(MESSAGE_ONLY, "Get what from what?\r\n");
	throw &Err;
  } else {
	//remove the wieght of the container so we don't add it twice
	Remove(pCon);
	if (this->Add(pObj, false)) {
	  //send messages
	  CString str;
	  str.Format("You get %s from %s.\r\n",
		  pObj->GetObjName(),
		  pCon->GetObjName());
	  SendToChar(str);
	  str.Format("%s gets %s from %s\r\n",
		  "%s",
		  pObj->GetObjName(),
		  pCon->GetObjName());
	  GetRoom()->SendToRoom(str, this);
	  //remove object from container
	  pCon->Remove(pObj);
	  //readd container
	  Add(pCon);
	} else {
	  Add(pCon);
	  CError Err(STOP_ERROR, "You can't lift that!\r\n");
	  throw &Err;
	}
  }
}

void CCharacter::DoInventory() {
  CString strInventory;
  StackObjects(Inventory, this, strInventory, true);
  if (strInventory.IsEmpty()) {
	strInventory = "You aren't carrying anything\r\n";
  }
  SendToChar(strInventory);
}

////////////////////////////////////////
//  SendPrompt
//	Intelligent function that knows what
//	type of prompt to send to character
//	written by: Demetrius Comes
//	6/6/98
/////////////////////////////////////

void CCharacter::SendPrompt() {
  CString strPrompt, strHits, strMana, strMoves;
  short nDiff;
  nDiff = (m_nCurrentHpts * 100) / m_nMaxHpts;
  if (nDiff < 10)
	strHits = "&F&R";
  else if (nDiff < 30)
	strHits = "&r";
  else if (nDiff < 80)
	strHits = "&y";
  else
	strHits = "&g";

  nDiff = (m_nManaPts * 100) / m_nMaxManaPts;
  if (nDiff < 30)
	strMana = "&r";
  else if (nDiff < 80)
	strMana = "&y";
  else
	strMana = "&g";

  nDiff = (m_nMovePts * 100) / m_nMaxMovePts;
  if (nDiff < 30)
	strMoves = "&r";
  else if (nDiff < 80)
	strMoves = "&y";
  else
	strMoves = "&g";

  strPrompt.Format("&g<%s%d&g/%d %s%d&g/%d %s%d&g/%d&g>&n%s",
	  strHits.cptr(), m_nCurrentHpts, m_nMaxHpts,
	  strMana.cptr(), m_nManaPts, m_nMaxManaPts,
	  strMoves.cptr(), m_nMovePts, m_nMaxMovePts,
	  (Prefers(PREFERS_COMPACT) ? "" : "\r\n"));

  if (InPosition(POS_FIGHTING) && IsFighting()) {
	CCharacter *pTank = this->IsFighting()->IsFighting();
	if (pTank) {
	  strPrompt.Format("%s<Tank: %s -> %s -- Target: %s -> %s>\r\n",
		  strPrompt.cptr(),
		  pTank->GetRaceOrName(this).cptr(),
		  pTank->GetCondition(),
		  IsFighting()->GetRaceOrName(this).cptr(),
		  IsFighting()->GetCondition());
	} else {
	  strPrompt.Format("%s<Target: %s -> %s>\r\n",
		  strPrompt.cptr(),
		  IsFighting()->GetRaceOrName(this).cptr(),
		  IsFighting()->GetCondition());
	}
  }
  SendToChar(strPrompt);
}

////////////////////////////////////////////
//DoSay
//	Handles talking to room
// written 4/3/98
///////////////////////////////////////////

void CCharacter::DoSay() {
  CString strToRoom(CurrentCommand.GetWordAfter(1, true));
  if (strToRoom.IsEmpty()) {
	SendToChar("Say what?\r\n");
  } else {
	if (!this->IsGod()) {
	  strToRoom.Colorize(false);
	}
	CString strToChar;
	strToChar.Format("You say '%s'\r\n", strToRoom.cptr());
	SendToChar(strToChar);
	m_pInRoom->SendToRoom("%s says '%s'\r\n", this, strToRoom.cptr());
  }
}

////////////////////////////////////////////
//	DoEmote
//	Handles emoting to room
//	written 4/3/98
///////////////////////////////////////////

void CCharacter::DoEmote() {
  CString strToRoom(CurrentCommand.GetWordAfter(1, true));
  if (!this->IsGod()) {
	strToRoom.Colorize(false);
  }
  CString strToChar;
  strToChar.Format("%s %s\r\n", GetName().cptr(), strToRoom.cptr());
  SendToChar(strToChar);
  m_pInRoom->SendToRoom("%s %s\r\n", this, strToRoom);
}


/////////////////////////
//DoTell
//	Handles Tells to players
//	written: 4/3/98
///////////////////////////

void CCharacter::DoTell() {
  CString strTo(CurrentCommand.GetWordAfter(1));

  CCharacter *pTarget = GVM.FindCharacter(strTo, this);

  if (!pTarget || pTarget->IsNPC()) {
	SendToChar("Tell who?\r\n");
  } else if (pTarget == this) {
	SendToChar("STOP talking to yourself!\r\n");
  } else if (!pTarget->Prefers(PREFERS_TELLS)) {
	strTo.Format("%s doesn't feel like talking to anyone.\r\n", pTarget->GetName().cptr());
	SendToChar(strTo);
  } else if (pTarget->InPosition(POS_SLEEPING)) {
	strTo.Format("%s can't hear anyone right now.\r\n", pTarget->GetRaceOrName(this).cptr());
	SendToChar(strTo);
  } else {
	CString strSentence(CurrentCommand.GetWordAfter(2, true));
	if (strSentence.IsEmpty()) {
	  SendToChar("Tell em what?\r\n");
	} else {
	  if (!this->IsGod()) {
		strSentence.Colorize(false);
	  }
	  strTo.Format("&WYou tell %s '%s'&n\r\n", strTo.cptr(), strSentence.cptr());

	  strSentence.Format("&W%s tells you '%s'&n\r\n", m_strName.cptr(),
		  strSentence.cptr());

	  SendToChar(strTo);
	  pTarget->SendToChar(strSentence);
	}
  }
}

//////////////////////////////////////////
// DoOtherState
//	This function will handle confirmations, writing, ect
////////////////////////////////////////////

void CCharacter::DoOtherState() {
  if (!m_pDescriptor)
	return;

  CString strToChar;
  int nPos, nPos1;
  switch (m_pDescriptor->GetState()) {
	case STATE_CONFIRM_SHUTDOWN:
	  switch (CurrentCommand[0]) {
		case 'Y': case 'y':
		  m_pDescriptor->ChangeState(STATE_PLAYING);
		  CGame::ShutDownWolfshade();
		  break;
		default:
		  m_pDescriptor->ChangeState(STATE_PLAYING);
		  break;
	  }
	  break;
	case STATE_PAGING:
	  switch (CurrentCommand[0]) {
		case 'b': case 'B':
		  SendCharPage(m_PageInfo.m_strPage);
		  break;
		case 'n': case 'N':
		  nPos = m_PageInfo.m_strPage.Find('\n', (m_nByScreenSize + m_PageInfo.m_nReturnsSent));
		  if (m_PageInfo.m_strPage.GetLength() <= nPos || nPos == -1) {
			strToChar.Format("%s\r\n%s\r\n",
				m_PageInfo.m_strPage.Right(m_PageInfo.m_strPage.Find('\n', m_PageInfo.m_nReturnsSent)).cptr(),
				"[b]egining, [p]revious page, [q]uit\r\n");
		  } else {
			strToChar.Format("%s\r\n%s\r\n",
				m_PageInfo.m_strPage(m_PageInfo.m_strPage.Find('\n', m_PageInfo.m_nReturnsSent), nPos).cptr(),
				strPageInfo);
			m_PageInfo.m_nReturnsSent += m_nByScreenSize;
		  }
		  SendToChar(strToChar);
		  break;
		case 'p': case 'P':
		  if (m_PageInfo.m_nReturnsSent - m_nByScreenSize <= 0) {
			SendCharPage(m_PageInfo.m_strPage);
		  } else {
			nPos = m_PageInfo.m_strPage.Find('\n', (m_PageInfo.m_nReturnsSent - m_nByScreenSize));
			nPos1 = m_PageInfo.m_strPage.Find('\n', m_PageInfo.m_nReturnsSent);
			nPos1 = nPos1 == -1 ? m_PageInfo.m_strPage.GetLength() : nPos1;
			strToChar.Format("%s\r\n%s\r\n",
				m_PageInfo.m_strPage(nPos, nPos1).cptr(),
				strPageInfo);
			m_PageInfo.m_nReturnsSent -= m_nByScreenSize;
		  }
		  SendToChar(strToChar);
		  break;
		default: //case 'q': case 'Q':
		  m_pDescriptor->ChangeState(STATE_PLAYING);
		  m_PageInfo.Empty();
		  break;
	  }
	  break;
	case STATE_FROZEN:
	{
	  CString str(CurrentCommand.GetWordAfter(0));
	  if (str.Compare("say") || str.Compare("'")) {
		DoSay();
	  } else {
		SendToChar("You are frozen you can only use the command say.\r\n");
	  }
	}
	  break;
	default:
	  ErrorLog << "Unhandled state in DoOtherState! switching state back to playing!" << endl;
	  m_pDescriptor->ChangeState(STATE_PLAYING);
	  break;
  }
}

///////////////////////////////////////
//Changes state to confirm shutdown
// written 4/4/98
//////////////////////////////////////

void CCharacter::ConfirmShutDown() {
  SendToChar("Are you sure you want to shut down the MUD?\r\n");
  m_pDescriptor->ChangeState(STATE_CONFIRM_SHUTDOWN);
}

void CCharacter::DoAttributes() {
  static const char * strFormat =
	  "	            &Y--==Attributes for &W%s&Y==--\r\n"
	  "&WRace: &L%s   &WAge: &L%d   &WClass: %s   Size: &L%s&n\r\n"
	  "&WLevel:  &g%d      &WHit Points: &g%d/%d &W Mana Points: &R%d/%d\r\n"
	  "&WMovement Points: %d/%d  Height: %d   Weight: %d\r\n"
	  "&mSTR:&M%4d  &mAGI:&M%4d  &mLCK:&M%4d \r\n"
	  "&mINT:&M%4d  &mWIS:&M%4d  &mCHA:&M%4d \r\n"
	  "&mCON:&M%4d  &mPOW:&M%4d  &mDEX:&M%4d \r\n"
	  "&nSaving Throws: %s \r\n"
	  "&bAC: &B%d &n     HitRoll: %d     DamRoll: %d \r\n"
	  "&WAli&wgnm&Lent: %d &n\r\n"
	  "Weight Carried: %s \r\n"
	  "Experience: %d\r\n"
	  "Worshipping: %s\r\n";

  CString strToChar;
  CString strSaves;
  m_pAttributes->SaveAttribute(strSaves);
  strToChar.Format(strFormat, m_strName.cptr(), GVM.GetColorRaceName(m_pAttributes->GetRace()),
	  m_pAttributes->CalcAge(), GVM.GetColorClass(m_pAttributes->GetClass()),
	  GetSizeName(this->GetSize()),
	  m_nLevel, m_nCurrentHpts, m_nMaxHpts, m_nManaPts, m_nMaxManaPts,
	  m_nMovePts, m_nMaxMovePts, m_pAttributes->AffectedHeight,
	  m_pAttributes->AffectedWeight, m_pAttributes->AffectedStats[STR],
	  m_pAttributes->AffectedStats[AGI],
	  m_pAttributes->AffectedStats[LUCK], m_pAttributes->AffectedStats[INT],
	  m_pAttributes->AffectedStats[WIS], m_pAttributes->AffectedStats[CHA],
	  m_pAttributes->AffectedStats[CON], m_pAttributes->AffectedStats[POW],
	  m_pAttributes->AffectedStats[DEX], strSaves.cptr(),
	  CalcAC(NULL), m_nToHit, m_nDamRoll, m_nAlignment,
	  GetLoadWording(), m_nExperience, GVM.GetColorGod(GetGod()));

  SendToChar(strToChar);
}

///////////////////////////////////
//	DoSkills
//	Sends Skills To character
//	written by: Demetrius Comes
//	4/26/98
///////////////////////////////////

void CCharacter::DoSkills() {
  CString strToChar("You have the following proficiencies\r\n");
  this->GetProficiency().GetSkillSets(m_nSkillSphere, strToChar);
  strToChar += "\r\nYou have the following skills:\r\n";
  for (int i = 0; i < MAX_MUD_SKILLS; i++) {
	if (GetSkill(i) != 0) {
	  if (this->IsNPC() || this->IsGod()) {
		strToChar.Format("%s%-30s\t%d\r\n", strToChar.cptr(),
			FindSkill(i).cptr(), GetSkill(i));
	  } else {
		strToChar.Format("%s%-30s\t%s\r\n", strToChar.cptr(),
			FindSkill(i).cptr(), SkillWording(GetSkill(i)));
	  }
	}
  }
  SendCharPage(strToChar);
}

//////////////////////////////////
//	Defense
//	Did my attacker hit?
//	Did I dodge it?
//	Did I repo it?
//  Ya hit me how much damange?
//	written by: Demetrius Comes
//	5/10/98
//////////////////////////////////

void CCharacter::Defense(short int nACWouldHit, short nPos, CCharacter * pAttacker) {
  short nWeaponMsg;
  if (pAttacker->m_Equipment[nPos].IsEmpty()
	  || !pAttacker->m_Equipment[nPos].GetObject()->IsWeapon()) {
	nWeaponMsg = PUNCH;
  } else {
	nWeaponMsg = pAttacker->m_Equipment[nPos]->GetMessageType();
  }
  // greater than 0 we don't hit
  if ((nACWouldHit - this->CalcAC(pAttacker)) > 0) {
	m_MsgManager.SendMiss(pAttacker, this, nWeaponMsg);
	return;
  }
  //we can only dodge and parry if we can fight
  if (CanFight()) {
	if (CanSeeInRoom(GetRoom()) == SEES_NOTHING) {
	  SkillUp(SKILL_BLIND_FIGHTING);
	}
	//I don't like that fact that we check for dodge before
	//we check for parry...make this function virtual and re-write for
	//different classes.  A warrior would try to Parry and repo first
	//right?
	if ((InPosition(POS_STANDING) && DIE(GAME_VAR_DEFENSE_DODGE) <= GetSkill(SKILL_DODGE))
		|| (!InPosition(POS_STANDING) && DIE(GAME_VAR_DEFENSE_DODGE) <= (GetSkill(SKILL_DODGE) / 2))) {
	  m_MsgManager.SkillMsg(this, pAttacker, SKILL_DODGE, SKILL_WORKED);
	  return;
	} else {
	  SkillUp(SKILL_DODGE);
	}
	//did we parry?
	if (GetSkill(SKILL_PARRY)
		&& ((InPosition(POS_STANDING) && DIE(GAME_VAR_DEFENSE_PARRY) <= GetSkill(SKILL_PARRY))
		|| (!InPosition(POS_STANDING) && DIE(GAME_VAR_DEFENSE_PARRY) <= (GetSkill(SKILL_PARRY) >> 2)))) {
	  if (GetSkill(SKILL_RIPOSTE)
		  && ((InPosition(POS_STANDING) && DIE(GAME_VAR_DEFENSE_RIPOSTE) <= GetSkill(SKILL_RIPOSTE))
		  || (!InPosition(POS_STANDING) && DIE(GAME_VAR_DEFENSE_RIPOSTE) <= (GetSkill(SKILL_RIPOSTE) >> 3)))) {
		m_MsgManager.SkillMsg(this, pAttacker, SKILL_RIPOSTE, SKILL_WORKED);
		//ripo's are with first weapon only
		pAttacker->Defense(this->CalcACWouldHit(), m_pAttributes->m_pRaceReference->GetWeaponPosition(1), this);
		return;
	  } else {
		SkillUp(SKILL_RIPOSTE);
		m_MsgManager.SkillMsg(this, pAttacker, SKILL_PARRY, SKILL_WORKED);
		return;
	  }
	} else {
	  SkillUp(SKILL_PARRY);
	}
  }
  int nDam = DoProtectionMelee(pAttacker, pAttacker->CalcDamage(nPos, this));
  //OK now we know we've been hit
  TakeDamage(pAttacker, nDam, true);
  m_MsgManager.SendHit(pAttacker, this, nWeaponMsg, nDam);
}

///////////////////////////////
//	CalcAC
//	Calc players current AC
//	TAke in acct position & size
//	of attacker

short int CCharacter::CalcAC(CCharacter *pAttacker) {
  short nAcAdjust = 0;
  if (pAttacker != NULL) {
	//for every 2 sizes you are smaller than your attacker you get GAME_VAR_SIZE_AC_ADJUST decrease in AC
	//or for every 2 sizes you are larget than your attacker you get a GAME_VAR_SIZE_AC_ADJUST increase in AC
	//so the larger you opponite is than you the harder it is for them to hit you and visa versa
	nAcAdjust += ((m_pAttributes->GetSize() - pAttacker->m_pAttributes->GetSize()) / 2) * GAME_VAR_SIZE_AC_ADJUST;
  }
  if (InPosition(POS_SLEEPING)) {
	nAcAdjust += 190; //95% increase
  } else if (InPosition(POS_STUNNED)) {
	nAcAdjust += 60; //30% increase
  } else if (InPosition(POS_RECLINING)) {
	nAcAdjust += 40; //20% increase
  } else if (InPosition(POS_SITTING)) {
	nAcAdjust += 20; //10% increase
  }

  //weight changes
  short nLoadPct = GetLoadPct();
  if (nLoadPct >= LOAD_IMMOBILIZING) {
	nAcAdjust += 150;
  } else if (nLoadPct >= LOAD_VERY_HEAVY) {
	nAcAdjust += 40;
  } else if (nLoadPct >= LOAD_HEAVY) {
	nAcAdjust += 20;
  }
  return RANGE(m_nAC + nAcAdjust, 100, -100);
}

///////////////////////////////
//  CalcDamage()
//  Checks for a weapon 
//  if no weapon uses bare hand damage
//  other wise weapon damage
//  written by: Demetrius Comes
//  5/11/98
////////////////////////////////

short int CCharacter::CalcDamage(short nPos, CCharacter *pDefender) {
  short nDam = m_nDamRoll;
  if (m_Equipment[nPos].IsEmpty()
	  || !m_Equipment[nPos].GetObject()->IsWeapon()) {
	nDam += ROLL(m_nTypeBareHandDamDice, m_nNoBareHandDamDice)
		+ m_nPlusBareHandDamDice + (GetSkill(SKILL_BARE_HAND_FIGHTING) / 5);
	SkillUp(SKILL_BARE_HAND_FIGHTING);
	if (IsAffectedBy(AFFECT_FISTS_OF_STONE)) nDam += 2;
	if (IsAffectedBy(AFFECT_VAMPIRIC_TOUCH) && m_Equipment[nPos].IsEmpty()) {
	  TakeDamage(pDefender, -nDam, true, false);
	}
	if (IsAffectedBy(AFFECT_PSIONIC_LEECH) && m_Equipment[nPos].IsEmpty()) {
	  GiveMana(pDefender, nDam);
	}
  } else {
	nDam += ROLL(m_Equipment[nPos]->GetDiceType(pDefender->m_pAttributes->GetSize()),
		m_Equipment[nPos]->GetNoOfDice(pDefender->m_pAttributes->GetSize()));
  }
  //if they are not fighting or standing double dam
  if ((!(pDefender->InPosition(POS_FIGHTING))) ||
	  (!(pDefender->InPosition(POS_STANDING)))) {
	nDam <<= 1;
  }
  if (DIE(1000) <= 10) //0.5%
  {
	SendToChar("&YYou score a critical hit!&n\r\n");
	nDam <<= 1;
  }
  return nDam;
}


/////////////////////////////////
//  Attack()
// here we actually try to hit the thing
//  we are fighting.
// written by: Demetrius Comes
//  5/13/98
///////////////////////////////

void CCharacter::Attack(bool bOnlyOneAttack) {
  if (!m_pFighting || m_pFighting->IsDead()) {
	ErrorLog << m_strName << " is in fight LL and not fighting anyone." << endl;
	return;
  }
  if (IsAffectedBy(AFFECT_CASTING)) {
	return;
  }
  //remove affect function only removes them
  //if they where affected by them to begin with
  RemoveAffect(AFFECT_INVISIBLE);
  RemoveAffect(AFFECT_HIDING);
  if (this->m_Master.IsMasterOf(IsFighting())) {
	m_Master.Remove(IsFighting());
  }
  if (this->m_Follow.HasFollower(IsFighting())) {
	this->m_Follow.Remove(IsFighting());
  }
  short j = 0, nAttacks;
  bool bIsSecondHeld = false;
  bool bIs2h = false;
  //get max number of attacks
  short nMaxAttacks = bOnlyOneAttack ? 1 : GetMaxWeaponPositions();
  assert(nMaxAttacks);
  //this is asserts you forgot to call calcMaxWeaponPositions in Race Class
  for (int i = 0; i < nMaxAttacks; i++) {
	nAttacks = 0;
	//gets the eq positions from the race class
	j = GetWeaponPosition(i + 1);

	if ((!m_Equipment[j].IsEmpty() && m_Equipment[j]->IsWeapon())) {
	  if (m_Equipment[j].IsWorn(WEAR_POS_DUAL_WIELD)) {
		if (DIE(100) <= GetSkill(SKILL_DUAL_WIELD)) {
		  nAttacks += IsAffectedBy(AFFECT_HASTE) ? 2 : 1;
		  nAttacks += IsAffectedBy(AFFECT_BLOODLUST) ? 1 : 0;
		  if (DIE(100) <= GetSkill(SKILL_DOUBLE_ATTACK)) {
			nAttacks++;
		  } else {
			SkillUp(SKILL_DOUBLE_ATTACK);
		  }
		  if (DIE(100) <= GetSkill(SKILL_TRIPLE_ATTACK)) {
			nAttacks++;
		  } else {
			SkillUp(SKILL_TRIPLE_ATTACK);
		  }
		} else {
		  SkillUp(SKILL_DUAL_WIELD);
		}
	  } else {
		//this works b/c wear_pos_wield_two_handed is always
		//in first hand!
		if (m_Equipment[j].IsWorn(WEAR_POS_WIELD_TWO_HANDED)) {
		  bIs2h = !bIs2h;
		}
		//everyone get at least 1 attack with a weapon
		nAttacks += IsAffectedBy(AFFECT_HASTE) ? 2 : 1;
		nAttacks += IsAffectedBy(AFFECT_BLOODLUST) ? 1 : 0;
		if (DIE(100) <= GetSkill(SKILL_DOUBLE_ATTACK)) {
		  nAttacks++;
		  if (DIE(100) <= GetSkill(SKILL_TRIPLE_ATTACK)) {
			nAttacks++;
		  } else {
			SkillUp(SKILL_TRIPLE_ATTACK);
		  }
		} else {
		  SkillUp(SKILL_DOUBLE_ATTACK);
		}
	  }
	} else if (m_Equipment[j].IsEmpty()) {
	  //if the last wielded position was not 2h
	  if (!bIs2h) {
		//we get one punch if our primary hand is
		//empty (primary meaning the WEAR_POS_DUAL bit is not set)
		if (!(m_pAttributes->m_pRaceReference->GetWearBit(j) & WEAR_POS_DUAL_WIELD)) {
		  nAttacks += IsAffectedBy(AFFECT_HASTE) ? 2 : 1;
		  nAttacks += IsAffectedBy(AFFECT_BLOODLUST) ? 1 : 0;
		  if (DIE(100) <= GetSkill(SKILL_DOUBLE_ATTACK)) {
			nAttacks++;
			if (DIE(100) <= GetSkill(SKILL_TRIPLE_ATTACK)) {
			  nAttacks++;
			} else {
			  SkillUp(SKILL_TRIPLE_ATTACK);
			}
		  } else {
			SkillUp(SKILL_DOUBLE_ATTACK);
		  }
		}//if the Position we are on is WEAR_POS_DUAL 
		  //and WEAR_POS_DUAL should only be set for off hand
		  //then we have to make a BARE_HAND_FIGHTING check to get
		  //another punch
		else {
		  if (DIE(100) <= GetSkill(SKILL_BARE_HAND_FIGHTING)) {
			nAttacks += IsAffectedBy(AFFECT_HASTE) ? 2 : 1;
			nAttacks += IsAffectedBy(AFFECT_BLOODLUST) ? 1 : 0;
			if (DIE(100) <= GetSkill(SKILL_DOUBLE_ATTACK)) {
			  nAttacks++;
			} else {
			  SkillUp(SKILL_DOUBLE_ATTACK);
			}
			if (DIE(100) <= GetSkill(SKILL_TRIPLE_ATTACK)) {
			  nAttacks++;
			} else {
			  SkillUp(SKILL_TRIPLE_ATTACK);
			}
		  } else {
			SkillUp(SKILL_BARE_HAND_FIGHTING);
		  }
		}
	  }//if it was flip the flag
	  else {
		bIs2h = false;
	  }
	} else if (m_Equipment[j].IsWorn(WEAR_POS_HOLD)) {
	  if (!bIsSecondHeld) {
		//no checks for other skills
		nAttacks += IsAffectedBy(AFFECT_HASTE) ? 2 : 1;
		nAttacks += IsAffectedBy(AFFECT_BLOODLUST) ? 1 : 0;
		bIsSecondHeld = true;
	  } else {
		if (DIE(100) <= GetSkill(SKILL_BARE_HAND_FIGHTING)) {
		  nAttacks += IsAffectedBy(AFFECT_HASTE) ? 2 : 1;
		  nAttacks += IsAffectedBy(AFFECT_BLOODLUST) ? 1 : 0;
		}
		bIsSecondHeld = false;
	  }
	}

	//while(nAttacks-- && GetRoom()->IsInRoom(this->IsFighting()))
	while (nAttacks-- && this->IsInRoom(this->IsFighting())) {
	  this->IsFighting()->Defense(this->CalcACWouldHit(), j, this);
	  if (this->IsFighting() == NULL
		  || this->IsFighting()->IsDead()
		  || !this->IsInRoom(IsFighting())) {
		this->AssertFighting(NULL);
		return;
	  }
	}
	if (!m_Equipment[j].IsEmpty()) {
	  m_Equipment[j].GetObject()->DoBattle();
	}
  }
}

////////////////////////////////////
//	GetCondition
//	wording for condition of character in battle
//	written by: Demetrius Comes
//	6/6/98
/////////////////////////////////////

const char *CCharacter::GetCondition() {
  if (InPosition(POS_INCAP))
	return "&RIncapacitated&n";
  else if (InPosition(POS_MORTALED))
	return "&RMortally wounded&n";
  short nHealth = (m_nCurrentHpts * 100) / m_nMaxHpts;

  if (nHealth >= 100)
	return "&GExcellent&n";
  else if (nHealth > 90)
	return "&yFew scratches&n";
  else if (nHealth > 65)
	return "&YSmall wounds&n";
  else if (nHealth > 45)
	return "&MSerious wounds&n";
  else if (nHealth > 20)
	return "&mNasty wounds&n";
  else if (nHealth > 5)
	return "&RSevere wounds&n";
  else
	return "&rCritical&n";
}

void CCharacter::DoCast(const CInterp *pInterp, bool bForceCast) {
  SendToChar("Leave the casting upto the mages!\r\n");
}

void CCharacter::DoThroatSlit(const CInterp *pInterp, CCharacter *pTarget) {
  SendToChar("Only evil assassins can slit throats properly!\r\n");
}

//////////////////////////////////
//	DoBash
//
/////////////////////////////////

void CCharacter::DoBash(const CInterp *pInterp, CCharacter *pTarget) {
  if (!GetSkill(SKILL_BASH)) {
	SendToChar("Learn how first!\r\n");
  } else if (pTarget == this) {
	SendToChar("Ok you bash yourself around for a while.\r\n");
  } else if (pTarget) {
	bool bHasShield = false;
	for (int i = 0; i < MAX_EQ_POS; i++) {
	  if (m_Equipment[i].GetWearBit() & WEAR_POS_SHIELD && !m_Equipment[i].IsEmpty()) {
		bHasShield = true;
	  }
	}
	CString strToChar, strToRoom;
	short int Adjust = 0;
	if (!pTarget->InPosition(POS_STANDING)) {
	  Adjust = DIE(25) + 25; // add up to 50% chance to miss if target is not standing.
	}
	if (!bHasShield) {
	  SendToChar("Bashing without a shield is tough, but you try anyway...\r\n");
	  Adjust += DIE(25) + 50;
	}
	if ((short int) DIE(100) + Adjust <= GetSkill(SKILL_BASH)) {
	  if (pTarget->MakeSkillSave(SKILL_BASH, 0)) {
		m_MsgManager.SkillSaveMsg(pTarget, this, "bash");
		LagChar(CMudTime::PULSES_PER_BATTLE_ROUND);
	  } else {
		pTarget->TakeDamage(this, GetSkill(SKILL_BASH) / 5, true);
		if (pTarget->IsDead()) {
		  pInterp->KillChar(pTarget);
		  m_MsgManager.SkillMsg(this, pTarget, SKILL_BASH, SKILL_KILLED);
		} else {
		  m_MsgManager.SkillMsg(this, pTarget, SKILL_BASH, SKILL_WORKED);
		  pTarget->UpDatePosition(POS_SITTING, false);
		  pTarget->LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * (DIE(3) + 1));
		}
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

/////////////////////////////////////
//	BackStab for the non rogues
//	we just backstab no instant kill double BS etc
////////////////////////////////////

void CCharacter::DoBackStab(const CInterp *pInterp, CCharacter *pTarget) {
  if (!GetSkill(SKILL_BACKSTAB)) {
	SendToChar("Try learning the skill first.\r\n");
  } else if (!pTarget) {
	SendToChar("Stab who?\r\n");
  } else if (pTarget == this) {
	SendToChar("Now how are you going to get behind yourself?\r\n");
  } else if (IsFighting()) {
	SendToChar("You can't do that while fighting!\r\n");
  } else {
	CString strToChar, strToRoom;
	int StabDmg;
	CString strWeaponName;
	short nDieRoll = 100;
	bool bHadPiercer = false;
	for (int i = 0; i < MAX_EQ_POS; i++) {
	  if (!m_Equipment[i].IsEmpty() && m_Equipment[i]->IsWeapon()
		  && m_Equipment[i]->GetMessageType() == PIERCE) {
		bHadPiercer = true;
		//it's harder if target is fighting.
		if (pTarget->IsFighting()) {
		  nDieRoll += 100 - GetSkill(SKILL_BACKSTAB);
		}
		strWeaponName = m_Equipment[i].GetObject()->GetObjName();
		if (DIE(nDieRoll) <= GetSkill(SKILL_BACKSTAB)) {
		  if (pTarget->MakeSkillSave(SKILL_BACKSTAB, 0)) {
			m_MsgManager.SkillSaveMsg(pTarget, this, "backstab");
			pTarget->Defense(CalcACWouldHit(), i, this);
		  } else {
			StabDmg = (GetSkill(SKILL_BACKSTAB) / 50 + 1) * CalcDamage(i, pTarget);
			pTarget->TakeDamage(this, StabDmg, true);
			if (!pTarget->IsDead()) {
			  strToChar.Format("%s makes a strange sound as you place %s in %s back.\r\n", pTarget->GetRaceOrName(this).cptr(), strWeaponName.cptr(), pTarget->GetPronoun(pTarget->GetSex()));
			  SendToChar(strToChar);
			  strToChar.Format("From out of nowhere %s places %s in YOUR back!\r\n", GetRaceOrName(pTarget).cptr(), strWeaponName.cptr());
			  pTarget->SendToChar(strToChar);
			  strToRoom = "%s makes a strange sound as %s places ";
			  strToChar.Format("%s in thier back.\r\n", strWeaponName.cptr());
			  GetRoom()->SendToRoom(strToRoom += strToChar, pTarget, this);
			} else {
			  strToChar.Format("%s is suddenly very silent as you place %s in %s back.\r\n", pTarget->GetRaceOrName(this).cptr(), strWeaponName.cptr(), (char *) pTarget->GetPronoun(pTarget->GetSex()));
			  SendToChar(strToChar);
			  strToChar.Format("From out of nowhere %s places %s in YOUR back, it is the last thing you ever feel.\r\n", GetRaceOrName(pTarget).cptr(), strWeaponName.cptr());
			  pTarget->SendToChar(strToChar);
			  strToRoom = "%s makes a strange sound as %s places ";
			  strToChar.Format("%s in thier back, making a strange sound, a lot of blood and a corpse.\r\n", strWeaponName.cptr());
			  GetRoom()->SendToRoom(strToRoom += strToChar, pTarget, this);
			  pInterp->KillChar(pTarget);
			  LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * 2);
			  return;
			}
		  }
		} else {
		  SkillUp(SKILL_BACKSTAB);
		  pTarget->Defense(CalcACWouldHit(), i, this);
		}
		AddFighters(pTarget);
	  }
	  if (bHadPiercer) {
		i = MAX_EQ_POS; // Only 1 stab for non rogue!
		LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * 4);
	  }
	}
	if (!bHadPiercer) {
	  SendToChar("Try a piercing weapon next time fool.\r\n");
	}
  }
}

void CCharacter::DoCircle(const CInterp *pInterp) {
  SendToChar("As Roguish as you think you are, you are not roguish enough to circle someone!\r\n");
}

void CCharacter::DoBeg() {
  SendToChar("Your just not Roguish enough to beg properly!\r\n");
}

void CCharacter::DoBerserk() {
  SendToChar("You say: 'EEEEEEyyyyyaaaahhhhh' and that's about all you can do.\r\n");
}

void CCharacter::DoDisarm(const CInterp *pInterp, CCharacter *pTarget) {
  SendToChar("Dis arm, Dat Arm, what the hell are you trying to do?\r\n");
}

void CCharacter::DoHitAll(const CInterp *pInterp) {
  SendToChar("Hit all of what?\r\n");
}

void CCharacter::DoIntimidate(const CInterp *pInterp) {
  SendToChar("I'm sorry, you're just not tough enough to intimidate anyone.\r\n");
}

void CCharacter::DoWeaponFlurry(const CInterp *pInterp) {
  if (!GetSkill(SKILL_WEAPON_FLURRY)) {
	SendToChar("Do what?\r\n");
  } else if (!IsFighting()) {
	SendToChar("You need to be in battle to make such an amazing feet.\r\n");
  } else {
	CCharacter *pTarget = IsFighting();
	short nSkill = GetSkill(SKILL_WEAPON_FLURRY);
	short nDie = DIE(100);
	CString strToChar;
	for (int i = 0; i < MAX_EQ_POS; i++) {
	  if (!m_Equipment[i].IsEmpty() && m_Equipment[i].GetObject()->IsWeapon()) {
		if (nDie <= nSkill) {
		  m_MsgManager.SkillMsg(this, pTarget, SKILL_WEAPON_FLURRY, SKILL_WORKED);
		  //skill gives you an attack for every skill/30 (1-4)
		  for (int j = 0; j <= (nSkill / 40 + 1); j++) {
			pTarget->Defense(CalcACWouldHit(), i, this);
			if (pTarget->IsDead()) {
			  pInterp->KillChar(pTarget);
			  j = nSkill;
			}
		  }
		}//if they fail to do it and fail dexterity check -10,you hit yourself
		else if (!StatCheck(DEX, -10)) {
		  strToChar.Format("&YYou rush your attack _WAY_ too much, hitting yourself!&n\r\n",
			  (char *) m_Equipment[i].GetObject()->GetObjName());
		  SendToChar(strToChar);
		  this->TakeDamage(this, DIE(10), true);
		  SkillUp(SKILL_WEAPON_FLURRY);
		} else {
		  SendToChar("You can't seem to move your hands fast enough to get a flurry going.\r\n");
		  SkillUp(SKILL_WEAPON_FLURRY);
		}
		i = MAX_EQ_POS;
	  }
	}
	LagChar(3 * CMudTime::PULSES_PER_BATTLE_ROUND);
  }
}

void CCharacter::DoDragonPunch(const CInterp *pInterp, CCharacter *pTarget) {
  SendToChar("You've heard of such a move, but don't know how to do it.\r\n");
}

void CCharacter::DoQuiveringPalm(const CInterp *pInterp, CCharacter *pTarget) {
  SendToChar("We don't want to know why your palm quivers...\r\n");
}

void CCharacter::DoBearHug(const CInterp *pInterp, CCharacter *pTarget) {
  if (!GetSkill(SKILL_BEAR_HUG)) {
	SendToChar("Learn how first!\r\n");
  } else if (!pTarget) {
	SendToChar("Hug Who?\r\n");
  } else if (pTarget == this) {
	SendToChar("You hug yourself tightly.\r\n");
  } else {
	short nSkill = GetSkill(SKILL_BEAR_HUG) + this->GetProficiency().CalcBonus(m_nSkillSphere) * 10;
	short nDie;
	if (IsFighting()) {
	  nDie = DIE(150);
	} else {
	  nDie = DIE(100);
	}
	CString strToChar;
	if (nDie <= nSkill) {
	  pTarget->TakeDamage(this, DIE(nSkill) + GetSkill(SKILL_BEAR_HUG) *
		  (this->GetProficiency().CalcBonus(m_nSkillSphere) >= 0 ? this->GetProficiency().CalcBonus(m_nSkillSphere) : 0), true);
	  if (pTarget->IsDead()) {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_BEAR_HUG, SKILL_KILLED);
		pInterp->KillChar(pTarget);
	  } else {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_BEAR_HUG, SKILL_WORKED);
		AddFighters(pTarget);
	  }
	} else {
	  m_MsgManager.SkillMsg(this, pTarget, SKILL_BEAR_HUG, SKILL_DIDNT_WORK);
	  SkillUp(SKILL_BEAR_HUG);
	  AddFighters(pTarget);
	}
	LagChar((4 * CMudTime::PULSES_PER_BATTLE_ROUND) - (this->GetProficiency().CalcBonus(m_nSkillSphere)) * CMudTime::PULSES_PER_BATTLE_ROUND);
  }
}

void CCharacter::DoLayHands() {
  SendToChar("Um no... Queer.\r\n");
}

void CCharacter::DoDirtToss(CCharacter *pTarget) {
  SendToChar("Ewww and get your hands dirty? Never.\r\n");
}

void CCharacter::DoCharm() {
  SendToChar("Your charisma will never be high enough to charm someone!\r\n");
}

void CCharacter::DoChant() {
  SendToChar("You chant, making strange sounds all the way, resulting in nothing. \r\n");
}

///////////////////////////////////
//	AdvanceLevel
//	Calculates points for advancing 
//	or losing a level
//	calls pure virtual function GetMaxHitPointsPerLevel()
//	written by: Demetrius Comes
////////////////////////////////

int CCharacter::AdvanceLevel(bool bIncreaseLevel, bool bSendMsg, bool bIsFollower) {
  int nHits = m_pAttributes->GetConHitBonus() +
	  (bIsFollower ? DIE(20) : DIE(this->GetMaxHitPointsPerLevel()));
  int nMana = this->GetManaAdjust() + 1;

  if (bIncreaseLevel) {
	m_nLevel++;
	m_nCurrentHpts += nHits;
	m_nMaxHpts += nHits;
	m_nMaxManaPts += nMana;
	m_nManaPts += nMana;
	if (!IsNPC() && GetLevel() == LVL_IMMORT) {
	  GVM.AddGod(this);
	  m_nPreference |= PREFERS_PETITION | PREFERS_GOD_CHAT | PREFERS_GOD_SEES_ALL | PREFERS_GOD_LIGHT;
	  SendToChar("Welcome to immortality!\r\n");
	}
	if (!(GetLevel() % LVLS_PER_PRACTICE_SESSION) &&
		!IsGod()) {
	  m_nPracticeSessions++;
	  if (bSendMsg) {
		SendToChar("Your experience has earned you another practice session.\r\n");
	  }
	}
  } else {
	//if the level is right and they are not a god and they miss a luck stat check
	if (!(GetLevel() % LVLS_PER_PRACTICE_SESSION) &&
		!IsGod() && !this->StatCheck(LUCK)) {
	  //lost a practice session
	  m_nPracticeSessions--;
	  if (bSendMsg) {
		SendToChar("Your lack of luck costs you a practice session.\r\n");
	  }
	}
	//if we are demoting a god
	//remove god bits
	if (GetLevel() == LVL_IMMORT) {
	  GVM.RemoveGod(this);
	  m_nPreference = 0;
	  SendToChar("You fall from your godly ways.\r\n");
	}
	m_nLevel--;
	m_nCurrentHpts -= nHits;
	m_nMaxHpts -= nHits;
	m_nMaxManaPts -= nMana;
	m_nManaPts -= nMana;
  }
  if ((this->GetExp(m_nLevel) > m_nExperience)) {
	m_nExperience = this->GetExp(m_nLevel);
  }

  static CString strIncrease("&WYou raise a level!\r\n&n");
  static CString strDecrease("&RYou lose a level!\r\n&n");

  if (bSendMsg) {
	bIncreaseLevel ? SendToChar(strIncrease) : SendToChar(strDecrease);
  }
  return nHits;
}

/////////////////////////////
//	Calculate AC WouldHit
///////////////////////////

short int CCharacter::CalcACWouldHit() {
  //increase DIE # to increase hit % while
  //decreasing it will decrease hit %
  short int nRetVal = (this->GetThaco() - m_nToHit - DIE(GAME_VAR_AC_WOULD_HIT));
  if (DIE(100) <= GetSkill(SKILL_OFFENSE)) {
	//so we might even subtract another 10
	nRetVal -= GetSkill(SKILL_OFFENSE) / 4;
  } else {
	SkillUp(SKILL_OFFENSE);
  }
  return(nRetVal < -100 ? -100 : nRetVal > 100 ? 100 : nRetVal);
}

//////////////////////////////
//	Save
//	Saves the character
//	to disk
//////////////////////////////

void CCharacter::Save(bool bDidJustDie) {
  if (!IsNPC()) {
	CString ObjDir(sObjectSave::DIR);
	ObjDir += m_strName;
	//open object file this opens it and over writes it
	std::ofstream ObjFile(ObjDir.cptr(), std::ios_base::binary | std::ios_base::out);
	if (bDidJustDie) {
	  //just close the object file to blank it out
	  ObjFile.close();
	  //remove all affects quitely
	  RemoveAllAffects(false);
	  //Remove all eq wear affects we don't have to remove character affects they
	  //are already gone.
	  for (short i = 0; i < MAX_EQ_POS; i++) {
		if (!m_Equipment[i].IsEmpty()) {
		  m_Equipment[i].GetObject()->WearAffects(true);
		}
	  }
	  //reduce hitpoints etc for death
	  m_nCurrentHpts = m_nMaxHpts >> 2;
	  m_nManaPts = m_nMaxManaPts >> 2;
	  m_nMovePts = m_nMaxMovePts >> 2;
	  //now we save them
	  this->SaveToFile(true);
	} else //they did not just die
	{
	  short i;
	  try {
		//Save eq being worn and removing all wear affects from those objects
		for (i = 0; i < MAX_EQ_POS; i++) {
		  if (!m_Equipment[i].IsEmpty()) {
			m_Equipment[i].GetObject()->Save(ObjFile, m_Equipment[i].GetWearBit(), i);
			//this removes the affects of wearing the object...
			//this is only done to save the character...otherwise we would have
			//to keep base stats and affected stats for each attribute a 
			//an object can affect...we dont need to remove character affects here
			//because we will remove all affects before we save the character
			m_Equipment[i].GetObject()->WearAffects(true);
			//this way we don't have affects given to a character by an object
			m_Equipment[i].GetObject()->AddCharAffects(true, false); //true = remove, false= nomsg
		  }
		}
		//save inventory stuff
		POSITION pos = Inventory.GetStartPosition();
		CObject *pObj;
		while (pos) {
		  pObj = Inventory.GetNext(pos);
		  pObj->Save(ObjFile, sObjectSave::INVENTORY, sObjectSave::INVENTORY);
		}
	  } catch (CError *pError) {
		ErrorLog << pError->GetMessage() << endl;
	  }
	  //now all objects are saved
	  ObjFile.close();
	  //Remove all affects on character quitely
	  //REMEBER AFFECTS BY OBJECTS HAVE ALREADY BEEN REMOVED FROM THE CHARACTER
	  for (i = 0; i < TOTAL_AFFECTS; i++) {
		if (IsAffectedBy(i) && m_AffectInfo[i].m_pFncApply != NULL) {
		  (this->*m_AffectInfo[i].m_pFncApply)(i, true); //true = remove
		}
	  }

	  this->SaveToFile(true);

	  //reapply all affects quitely
	  //MUST DO THIS BEFORE WE RE-ADD the affects from the objects
	  //b/c right now they are out of the affect array if we add them
	  //then readd all the affects we will double add all the affects
	  //from the objects
	  for (i = 0; i < TOTAL_AFFECTS; i++) {
		if (IsAffectedBy(i) && m_AffectInfo[i].m_pFncApply != NULL) {
		  (this->*m_AffectInfo[i].m_pFncApply)(i, false); //true = remove
		}
	  }
	  //NOW add affects from objects currently being worn
	  for (i = 0; i < MAX_EQ_POS; i++) {
		if (!m_Equipment[i].IsEmpty()) {
		  //This adds all the wear affects back on to the
		  //character for each object worn
		  //but we don't re-add character affects from the object
		  //or we will add them twice!
		  m_Equipment[i].GetObject()->WearAffects(false);
		  //readd character affects with no message
		  m_Equipment[i].GetObject()->AddCharAffects(false, false);
		}
	  }
	}
  }
}

///////////////////////////////
//	Dowear
//	Allows the character to wear
//	an item
//	written by: Demetrius Comes
//	7/5/98
///////////////////////////////

void CCharacter::DoWear() {
  //is the object in there inventory?
  CString strItem(CurrentCommand.GetWordAfter(1));
  CObject *pObj;
  CString strToChar;
  int nDot, nNum = 1;

  if ((nDot = strItem.Find('.')) != -1) {
	nNum = strItem.Left(nDot).MakeInt();
	strItem = strItem.Right(nDot);
  }

  if (strItem.Compare("all")) {
	POSITION pos = Inventory.GetStartPosition();
	if (!pos) {
	  SendToChar("You don't have anything to wear?\r\n");
	  return;
	}
	while (pos) {
	  pObj = Inventory.GetNext(pos);
	  //pass false so we don't spam everyone!
	  if (pObj->CanbeWornBy(this)) {
		if (m_pAttributes->m_pRaceReference->PutEqOn(this, pObj, false)) {
		  Remove(pObj, pos);
		}
	  } else {
		Remove(pObj, pos);
		GetRoom()->Add(pObj);
	  }
	}
	SendToChar("You fully equip yourself\r\n");
	if (GetSex() == MALE) {
	  m_pInRoom->SendToRoom("%s fully equips %self.\r\n", this, CString(GetPronoun(m_pAttributes->GetSex())));
	} else {
	  m_pInRoom->SendToRoom("%s fully equips %sself.\r\n", this, CString(GetPronoun(m_pAttributes->GetSex())));
	}
  } else if ((pObj = FindInInventory(strItem, nNum)) && !pObj->IsWeapon()) {
	//Ok we found the object.
	//but can we wear it?
	if (pObj->CanbeWornBy(this)) {
	  if (m_pAttributes->m_pRaceReference->PutEqOn(this, pObj)) {
		Remove(pObj);
	  }
	} else {
	  Remove(pObj);
	  GetRoom()->Add(pObj);
	}
  } else if (pObj && pObj->IsWeapon()) {
	SendToChar("Wield weapons you don't wear them!\r\n");
  } else {
	SendToChar("Wear what?\r\n");
  }
}

////////////////////////////////
//	DoEquipment
//	Send equipment player is wearing to him/her/it
///////////////////////////////

void CCharacter::DoEquipment() {
  CString strToChar;
  EqList(this, strToChar);
  if (strToChar.IsEmpty()) {
	strToChar = "You not wearing anything!\r\n";
  }
  SendToChar(strToChar);
}

///////////////////////
//	DoWield()
//	Allows character to wield a weapon
//	written by: Demetrius Comes
////////////////////////

void CCharacter::DoWield() {
  CObject *pObj;
  CString strToChar;
  CString strItem(CurrentCommand.GetWordAfter(1));
  int nDot, nNum = 1;

  if ((nDot = strItem.Find('.')) != -1) {
	nNum = strItem.Left(nDot).MakeInt();
	strItem = strItem.Right(nDot);
  }

  if ((pObj = FindInInventory(strItem, nNum)) && pObj->IsWeapon()) {
	//Ok we found the object.
	if (pObj->CanbeWornBy(this)) {
	  if (m_pAttributes->m_pRaceReference->PutEqOn(this, pObj)) {
		Remove(pObj);
	  }
	} else {
	  Remove(pObj);
	  GetRoom()->Add(pObj);
	}
  } else {
	SendToChar("Wield what?\r\n");
  }
}

////////////////////////////////////
//	Overloaded UpdatePOsition
//	Set the characters position
//	I have attempted to put all logic for when a player
//	can sit stand, recl, etc.
//	written by: Demetrius Comes
//	5/13/98
//	updated: 8/3/98
///////////////////////////////////

void CCharacter::UpDatePosition(UINT nPosition, bool bSendStr) {
  CString strToRoom;
  switch (nPosition) {
	case POS_PREP:
	  if (InPosition(POS_RESTING)) {
		m_nPosition = POS_PREP | POS_RESTING;
		return;
	  }
	  SendToChar("You must be resting to prepare spells.\r\n");
	  break;
	case POS_STANDING:
	  if (InPosition(POS_PREP)) {
		if (bSendStr) {
		  SendToChar("You stop preparing your spells and stand up.\r\n");
		  m_pInRoom->SendToRoom("%s packs up %s spell supplies.\r\n", this, CString(Pronouns[GetSex()]));

		}
	  }
	  if (InPosition(POS_RESTING) || InPosition(POS_SITTING)
		  || InPosition(POS_RECLINING)) {
		if (bSendStr) {
		  SendToChar("You stand up.\r\n");
		  m_pInRoom->SendToRoom("%s stands up.\r\n", this);
		}
	  } else if (InPosition(POS_STANDING)) {
		if (bSendStr) {
		  SendToChar("You are already standing.\r\n");
		  return;
		}
	  }
	  m_nPosition = InPosition(POS_FIGHTING) ? POS_STANDING | POS_FIGHTING : POS_STANDING;
	  break;
	case POS_FIGHTING:
	  if (InPosition(POS_SLEEPING)) {
		if (bSendStr) {
		  SendToChar("Wow what a way to wake up!\r\n");
		}
		m_nPosition &= ~POS_SLEEPING;
		m_nPosition |= POS_RECLINING;
	  }
	  if (InPosition(POS_PREP)) {
		if (bSendStr) {
		  SendToChar("Your attacks interupts your preparations!\r\n");
		}
		m_nPosition &= ~POS_PREP;
	  }
	  m_nPosition |= POS_FIGHTING;
	  break;
	case POS_RESTING:
	  if (InPosition(POS_STANDING) && !InPosition(POS_FIGHTING)) {
		if (bSendStr) {
		  SendToChar("You sit down and rest.\r\n");
		  m_pInRoom->SendToRoom("%s sits down and rests.\r\n", this);
		  m_nPosition = POS_RESTING | POS_SITTING;
		}
	  } else if (InPosition(POS_SITTING) && !InPosition(POS_FIGHTING)) {
		if (bSendStr) {
		  SendToChar("You relax a bit.\r\n");
		  m_pInRoom->SendToRoom("%s relaxes a bit.\r\n", this);
		  m_nPosition = POS_RESTING | POS_SITTING;
		}
	  } else if (InPosition(POS_RECLINING)) {
		if (bSendStr) {
		  SendToChar("You are already lying down..sit up first!\r\n");
		}
	  } else if (InPosition(POS_FIGHTING)) {
		if (bSendStr) {
		  SendToChar("You can't rest your fighting!\r\n");
		}
	  } else if (InPosition(POS_SLEEPING)) {
		if (bSendStr) {
		  SendToChar("Wake up first!\r\n");
		}
	  } else if (InPosition(POS_RESTING)) {
		if (bSendStr) {
		  SendToChar("You are already resting!\r\n");
		}
	  } else {
		SendToChar("You shouldn't see this message from UpdatePosition POS_RESTING\r\n");
	  }
	  break;
	case POS_SITTING:
	  if (InPosition(POS_PREP)) {
		if (bSendStr) {
		  SendToChar("You stop your preparations.\r\n");
		}
	  }
	  if (InPosition(POS_STANDING)) {
		if (bSendStr) {
		  SendToChar("You sit down.\r\n");
		  m_pInRoom->SendToRoom("%s sits down.\r\n", this);
		}
	  } else if (InPosition(POS_RECLINING)) {
		if (bSendStr) {
		  SendToChar("You stop lying around and sit up.\r\n");
		  m_pInRoom->SendToRoom("%s stops lying around and sits up.\r\n", this);
		}
	  } else if (InPosition(POS_RESTING)) {
		if (bSendStr) {
		  SendToChar("You stop resting and sit up.\r\n");
		  m_pInRoom->SendToRoom("%s stops resting and sits up.\r\n", this);
		}
	  }
	  m_nPosition = InPosition(POS_FIGHTING) ? POS_FIGHTING | POS_SITTING : POS_SITTING;
	  break;
	case POS_SLEEPING:
	  if (InPosition(POS_PREP)) {
		if (bSendStr) {
		  SendToChar("You stop your preparations.\r\n");
		}
	  }
	  if (InPosition(POS_FIGHTING)) {
		if (bSendStr) {
		  SendToChar("Sleep and fight?\r\n");
		}
	  } else if (InPosition(POS_STANDING)) {
		if (bSendStr) {
		  SendToChar("You fall asleep on your feet... heeh dumb ass.\r\n");
		  m_pInRoom->SendToRoom("%s falls asleep on %s feet", this, CString(Pronouns[GetSex()]));
		  m_nPosition = POS_SLEEPING | POS_STANDING;
		}
	  } else if (InPosition(POS_RESTING)
		  || InPosition(POS_SITTING)) {
		if (bSendStr) {
		  SendToChar("You fall asleep sitting up.\r\n");
		  GetRoom()->SendToRoom("%s falls asleep sitting up.\r\n", this);
		  m_nPosition = POS_SITTING | POS_SLEEPING;
		}
	  } else {
		if (bSendStr) {
		  SendToChar("You fall asleep.\r\n");
		  m_nPosition = POS_RECLINING | POS_SLEEPING;
		}
	  }
	  break;
	case POS_RECLINING:
	  if (InPosition(POS_PREP)) {
		if (bSendStr) {
		  SendToChar("You stop your preparations.\r\n");
		}
	  }
	  if (InPosition(POS_FIGHTING)) {
		if (bSendStr) {
		  SendToChar("You can't do that in the middle of a fight!\r\n");
		  return;
		}
	  } else if (InPosition(POS_RECLINING)) {
		if (bSendStr) {
		  SendToChar("You are already lying down!\r\n");
		  return;
		}
	  } else if (InPosition(POS_STANDING)) {
		if (bSendStr) {
		  SendToChar("You drop flat to your belly.\r\n");
		  m_pInRoom->SendToRoom("%s drops flat to %s belly.\r\n", this, CString(Pronouns[GetSex()]));
		}
	  } else {
		if (bSendStr) {
		  SendToChar("You lie down.\r\n");
		  m_pInRoom->SendToRoom("%s lies down.\r\n", this);
		}
	  }
	  m_nPosition = POS_RECLINING;
	  break;
	case POS_STUNNED:
	  if (InPosition(POS_PREP)) {
		SendToChar("You stop your preparations.\r\n");
	  }
	  if (bSendStr) {
		SendToChar("&CYou are stunned!&n\r\n");
		GetRoom()->SendToRoom("&C%s&C is stunned!&n\r\n", this);
	  }
	  m_nPosition |= InPosition(POS_FIGHTING) ? POS_STUNNED | POS_FIGHTING : POS_STUNNED;
	  break;
	case POS_KOED:
	  m_nPosition = POS_RECLINING | POS_KOED;
	case POS_DEAD:
	  m_nPosition = POS_DEAD;
	  break;
	case POS_INCAP:
	  if (InPosition(POS_PREP)) {
		if (bSendStr) {
		  SendToChar("You stop your preparations.\r\n");
		}
	  }
	  m_nPosition = POS_INCAP;
	  break;
	case POS_MORTALED:
	  if (InPosition(POS_PREP)) {
		if (bSendStr) {
		  SendToChar("You stop your preparations.\r\n");
		}
	  }
	  m_nPosition = POS_MORTALED;
	  break;
	default:
	  ErrorLog << "Bad Position Passed to updateposition" << endl;
	  break;
  }
}

/////////////////////////////////
//	Updates the position of the character
//	by looking at current hit points
//	written by: Demetrius Comes
//	5/11/98
/////////////////////////////////

void CCharacter::UpDatePosition() {
  if (m_nCurrentHpts <= HITPTS_OF_DEATH) {
	m_nPosition = POS_DEAD | POS_RECLINING;
  } else if (m_nCurrentHpts >= -10 && m_nCurrentHpts < -6) {
	GetRoom()->SendToRoom("%s slumps to the ground.\r\n", this);
	SendToChar("You fall to the ground in pain.\r\n");
	m_nPosition = POS_MORTALED | POS_RECLINING;
  } else if (m_nCurrentHpts >= -6 && m_nCurrentHpts < -2) {
	m_nPosition |= POS_INCAP;
  } else if (m_nCurrentHpts >= -2 && m_nCurrentHpts < 0) {
	m_nPosition |= InPosition(POS_FIGHTING) ? POS_FIGHTING | POS_STUNNED : POS_STUNNED;
  } else {
	m_nPosition &= ~(POS_MORTALED | POS_INCAP | POS_STUNNED);
  }
}

/////////////////////////////
//	FindInInventory
//	Given a string we check
//	object aliases for a match
//	written by: Demetrius Comes
/////////////////////////////

CObject * CCharacter::FindInInventory(CString & strItem, short nNum) {
  POSITION pos = Inventory.GetStartPosition();
  CObject *pObj;
  short nCount = 0;
  while (pos) {
	pObj = Inventory.GetNext(pos);
	if (pObj->ByAlias(strItem)
		&& this->CanSee(pObj)) {
	  if (++nCount == nNum) {
		return pObj;
	  }
	}
  }
  return NULL;
}

///////////////////////////
//	EqList
//	build a string of equipment being worn by player
//	I'm separating this from DoEquipment
//	so we can use it for lookatcharacter function too.
//	written by: Demetrius Comes
//////////////////////////

void CCharacter::EqList(const CCharacter *pLooker, CString & strEq) {
  for (int i = 0; i < MAX_EQ_POS; i++) {
	if (!m_Equipment[i].IsEmpty() &&
		pLooker->CanSee(m_Equipment[i].GetObject())) {
	  strEq.Format("%s%s\t%s", strEq.cptr(),
		  m_pAttributes->m_pRaceReference->GetEqPosName(m_Equipment[i].GetWearBit()),
		  m_Equipment[i]->GetObjName());

	  CString str = m_Equipment[i]->GetExtraString();
	  if (!str.IsEmpty()) {
		str.SkipSpaces();
		strEq.Format("%s (%s)\r\n", strEq.cptr(), str.cptr());
	  } else {
		strEq += "\r\n";
	  }
	}
  }
}

///////////////////////
//	DoRemove
//	Remove item from equipment array
//	add to inventory
//	written by: Demetrius Comes
/////////////////////////

void CCharacter::DoRemove() {
  CString strToRemove(CurrentCommand.GetWordAfter(1));
  int j = 0;
  CString strToChar;
  bool bRemoveAll = strToRemove.Compare("all");
  bool bFound = false;

  for (int i = MAX_EQ_POS - 1; i >= 0; i--) {
	if (!m_Equipment[i].IsEmpty() &&
		(m_Equipment[i]->ByAlias(strToRemove) || bRemoveAll)) {
	  bFound = true;
	  strToChar.Format("You remove %s.\r\n",
		  m_Equipment[i]->GetObjName());
	  SendToChar(strToChar);
	  m_pInRoom->SendToRoom("%s removes %s.\r\n", this, m_Equipment[i]->GetObjName());
	  Add(RemoveEquipment(i, true), false, true);
	  if (!bRemoveAll) {
		return;
	  }
	}
  }
  if (!bFound) {
	SendToChar("Remove what?\r\n");
  }
}

//////////////////////////
//	Prepare overriden for casting classes
/////////////////////////

void CCharacter::DoPrepare() {
  SendToChar("What are you going to prepare for?\r\n");
  return;
}

/////////////////////////////////////////////////
//	DoPractice
//	Check to see if the character wants to practice a valid skill
//	check to see if they can learn the skill,
//	Do they add a new proficiency?
//	If they are a GOD then let them learn the skill but warn them
//	they really should have been able to.  We still prevent mages
//	from learning skills for warriors etc.
//	written by: Demetrius Comes
//	mofified on 6-15-99

void CCharacter::DoPractice(CString strToPractice, const CCharacter *pTeacher) {
  short nSkill;
  skill_type nPctCanLearn;

  if ((nSkill = FindSkill(strToPractice)) != -1) {
	CString strToChar;
	if ((nPctCanLearn = pTeacher->CanTeach(this, nSkill, true)) > 0) {
	  const CSkillInfo * pSkillInfo = this->GetSkillInfo(nSkill);
	  CString strSets;
	  //can you learn this skill
	  if (pSkillInfo->CanLearn()) {
		if (HasSkillSet(this->GetProficiency().GetNeededPriorSkillKnowledge(pSkillInfo))) {
		  sMoney Cost;
		  if (CanAffordTraining(Cost, nSkill, pTeacher, true)) {
			if (m_nPracticeSessions > 0) {
			  //subtract a practice session
			  m_nPracticeSessions--;
			  m_Skills[nSkill] += DIE(nPctCanLearn);

			  if (HasSkillSet(pSkillInfo->GetSet())) {
				strToChar.Format("You learn the skill of %s from %s.\r\n",
					FindSkill(nSkill).cptr(),
					pTeacher->GetRaceOrName(this).cptr());
			  } else {
				m_nSkillSphere |= pSkillInfo->GetSet();
				this->GetProficiency().GetSkillSets(pSkillInfo->GetSet(), strSets, false);
				strToChar.Format("You add the proficiency of %s to your honors,\r\n as you learn the skill %s from %s.\r\n",
					strSets.cptr(),
					FindSkill(nSkill).cptr(),
					pTeacher->GetRaceOrName(this).cptr());
			  }
			  if (!Cost.IsEmpty()) {
				strToChar.Format("%sThe training costs you: %s\r\n", strToChar.cptr(), Cost.GetMoneyString().cptr());
			  }
			  SendToChar(strToChar);
			  strToChar.Format("You teach %s the skill of %s.\r\n",
				  GetRaceOrName(this).cptr(),
				  FindSkill(nSkill).cptr());
			  pTeacher->SendToChar(strToChar);
			} else {
			  SendToChar("You must earn a practice session before you can do that.\r\n");
			}
		  } else {
			SendToChar("You can't seem to afford the training.\r\n");
		  }
		} else if (IsGod()) {
		  m_Skills[nSkill] += DIE(nPctCanLearn);
		  m_nSkillSphere |= pSkillInfo->GetSet();
		  this->GetProficiency().GetSkillSets(pSkillInfo->GetSet(), strSets, false);
		  strToChar.Format("You learn the skill %s from %s but only b/c your a god.\r\n",
			  FindSkill(nSkill).cptr(),
			  pTeacher->GetRaceOrName(this).cptr());
		  SendToChar(strToChar);
		} else {
		  this->GetProficiency().GetSkillSets(pSkillInfo->GetSet(), strSets, false);
		  strToChar.Format("You must first learn the proficiency %s.\r\n",
			  strSets.cptr());
		  SendToChar(strToChar);
		}
	  } else //end (pSkillInfo->CanLearn())
	  {
		SendToChar("You can't learn that skill!\r\n");
	  }
	}
  } else //end ((nSkill = FindSkill(strToPractice))!=-1)
  {
	SendToChar("Practice what?\r\n");
  }
}

////////////////////////////////////
//	DoSpells
//	Default if they can't cast then
//	just tell them they don't know any spells
//////////////////////////////////

void CCharacter::DoSpells() {
  SendToChar("You don't know any spells!\r\n");
}

///////////////////////////////////
//	DoScore
//	Shows Character info about themself
//	written by: Demetrius Comes
//	8/3/98
/////////////////////////////////

void CCharacter::DoScore() {
  static const char * strFormat =
	  "                Score information for %s\r\n"
	  "Level: %d   Race: %s    Class: %s     Sex: %s\r\n"
	  "Hit points: %d(%d)   Moves: %d(%d)   Mana: %d(%d)\r\n"
	  "Playing time: &W%s&n\r\n"
	  "Cash On Hand: %s \r\n"
	  "Cash In Bank: %s \r\n"
	  "Position: %s \r\n"
	  "Practice Sessions: %d\r\n"
	  "Frags: %.2f\r\n"
	  "Enchantments: \r\n"
	  "%s\r\n";
  CString strToChar, strTime, strEnchant;
  m_pAttributes->TimePlayed(strTime);
  GetEnchantments(strEnchant);
  strToChar.Format(strFormat, m_strName.cptr(), m_nLevel, GVM.GetColorRaceName(GetRace()),
	  GVM.GetColorClass(GetClass()), GetSexName(GetSex()), m_nCurrentHpts, m_nMaxHpts,
	  m_nMovePts, m_nMaxMovePts, m_nManaPts, m_nMaxManaPts,
	  strTime.cptr(),
	  m_CashOnHand.GetMoneyString().cptr(),
	  (m_pBankCash == NULL ? "" : m_pBankCash->GetMoneyString().cptr()),
	  GetPositionNames(), m_nPracticeSessions, m_fFrags, strEnchant.cptr());
  SendToChar(strToChar);
}

///////////////////////////////
//	Give the character looking at me
//	do they see my name or my race?
//	written by: Demetrius Comes
///////////////////////////

CString CCharacter::GetRaceOrName(const CCharacter *pCh) const {
  short nVision = pCh->CanSeeInRoom(this->GetRoom());
  //if pCh is affected by blindness || pCh can't see in the room (this) is in
  //or he just can't see me (b/c of invis or something) we say someone
  if (nVision == SEES_NOTHING || !pCh->CanSeeChar(this)) {
	return "someone";
  } else if (nVision == SEES_RED_DOTS) {
	return "&rheated figure&n";
  } else if (pCh->CanSeeName(this)) {
	return m_strName;
  } else {
	const char *strRace = GVM.GetRaceName(GetRace());
	CString str;
	if (tolower(strRace[0]) == 'a' || tolower(strRace[0]) == 'e' ||
		tolower(strRace[0]) == 'i' || tolower(strRace[0]) == 'o' ||
		tolower(strRace[0]) == 'u') {
	  str.Format("An %s", GVM.GetColorRaceName(GetRace()));
	} else {
	  str.Format("A %s", GVM.GetColorRaceName(GetRace()));
	}
	return str;
  }
}

////////////////////////
//	DoList
//	create a string send it the room ptr
//	then send the string to char
///////////////////////

void CCharacter::DoList() {
  CString strToChar;
  m_pInRoom->DoList(strToChar);
  SendToChar(strToChar);
}

//////////////////////////
//	DoReport
//	written by: John Comes
/////////////////////////

void CCharacter::DoReport() {
  CString RepMsg, RepMsg2, MsgToRoom;
  MsgToRoom = "%s says '";
  RepMsg.Format("I have %d (%d) hit points, %d (%d) mana points and %d (%d) movement points.'\r\n"
	  , m_nCurrentHpts, m_nMaxHpts, m_nManaPts, m_nMaxManaPts, m_nMovePts, m_nMaxMovePts);
  MsgToRoom += RepMsg;
  GetRoom()->SendToRoom(MsgToRoom, this);
  RepMsg2 = "You say '";
  SendToChar(RepMsg2 += RepMsg);
  return;
}

////////////////////////////////
//	DoThink
//	written by: John Comes
//////////////////////////////

void CCharacter::DoThink(const CInterp *pInterp) {
  SendToChar("Think all you want, only the psioncists know how to really think!\r\n");
}


////////////////////////////////
//	DoHeadButt
//	written by: John Comes
//////////////////////////////

void CCharacter::DoHeadButt(const CInterp *pInterp, CCharacter *pTarget) {
  if (!GetSkill(SKILL_HEAD_BUTT)) {
	SendToChar("You don't want to mess up your pretty little head do you?\r\n");
  } else if (!pTarget) {
	SendToChar("It helps if their head exists!\r\n");
  } else if (pTarget == this) {
	SendToChar("Em your wierd.\r\n");
  } else {
	short nSkill = GetSkill(SKILL_HEAD_BUTT) + this->GetProficiency().CalcBonus(m_nSkillSphere) * 10;
	short nDie = DIE(100);
	if (nDie <= nSkill) {
	  pTarget->TakeDamage(this, 2 * DIE(nSkill) + nSkill + 3 * m_nDamRoll, true);
	  if (pTarget->IsDead()) {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_HEAD_BUTT, SKILL_KILLED);
		pInterp->KillChar(pTarget);
	  } else {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_HEAD_BUTT, SKILL_WORKED);
		AddFighters(pTarget);
	  }
	} else {
	  TakeDamage(this, nSkill / 2, true, false);
	  m_MsgManager.SkillMsg(this, pTarget, SKILL_HEAD_BUTT, SKILL_DIDNT_WORK);
	  SkillUp(SKILL_HEAD_BUTT);
	  AddFighters(pTarget);
	}
	if (nSkill - nDie < -50) {
	  UpDatePosition(POS_STUNNED, true);
	}
	if (nSkill - nDie > 100) {
	  pTarget->UpDatePosition(POS_STUNNED, true);
	}
	LagChar((4 * CMudTime::PULSES_PER_BATTLE_ROUND) - (this->GetProficiency().CalcBonus(m_nSkillSphere)) * CMudTime::PULSES_PER_BATTLE_ROUND);
  }
}

//////////////////////////////
// DoCalledShot
// John Comes 7/27/99
//////////////////////////////

void CCharacter::DoCalledShot(const CInterp *pInterp) {
  if (!GetSkill(SKILL_CALLED_SHOT)) {
	SendToChar("Learn how first!\r\n");
  } else if (!this->IsFighting()) {
	SendToChar("You can't call a shot if you're not fighting anyone.\r\n");
  } else {
	CString strShot(CurrentCommand.GetWordAfter(1));
	int nBonus = this->GetProficiency().CalcBonus(m_nSkillSphere);
	int nDie;
	short nSkill = GetSkill(SKILL_CALLED_SHOT) + nBonus * 10;
	bool bWorked = false;
	CCharacter *pTarget = this->IsFighting();
	CString strMessage;
	//switch to shot called
	if (strShot.Compare("head")) {
	  //called head shot, have a die out of possible 230 if -3 bonus
	  nDie = DIE(100) - (nBonus * 10) + 100;
	  if (nDie < nBonus) {
		strMessage.Format("You neatly decapitate %s with your shot to the head!", pTarget->GetName().cptr());
		SendToChar(strMessage);
		this->GetRoom()->SendToRoom("%s neatly decapitates %s with a shot to the head!", this, pTarget);
		pInterp->KillChar(pTarget);
	  } else if (nDie <= nSkill) {
		bWorked = true;
		pTarget->TakeDamage(this, DIE(nSkill) + GetSkill(SKILL_CALLED_SHOT) * (nBonus >= 0 ? nBonus : 0), true);
	  }
	} else if (strShot.Compare("leg")) {
	  nDie = DIE(100) - nBonus * 10 + 50;
	  if (nDie < nBonus * 10) {
		strMessage.Format("Your shot to the leg causes %s to fall!", pTarget->GetName().cptr());
		SendToChar(strMessage);
		this->GetRoom()->SendToRoom("%s's shot to the leg causes %s to fall!", this, pTarget);
		pTarget->UpDatePosition(POS_SITTING, false);
	  } else if (nDie <= nSkill) {
		bWorked = true;
		pTarget->TakeDamage(this, DIE(nSkill) + GetSkill(SKILL_CALLED_SHOT) * (nBonus >= 0 ? nBonus : 0), true);
	  }
	} else {
	  nDie = DIE(100) - nBonus * 10;
	  if (nDie <= nSkill) {
		bWorked = true;
		pTarget->TakeDamage(this, DIE(nSkill) + GetSkill(SKILL_CALLED_SHOT) *
			(nBonus >= 0 ? nBonus : 0), true);
	  }
	}
	if (bWorked) {
	  if (pTarget->IsDead()) {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_CALLED_SHOT, SKILL_KILLED);
		pInterp->KillChar(pTarget);
	  } else {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_CALLED_SHOT, SKILL_WORKED);
	  }
	} else {
	  m_MsgManager.SkillMsg(this, pTarget, SKILL_CALLED_SHOT, SKILL_DIDNT_WORK);
	  SkillUp(SKILL_CALLED_SHOT);
	}
	LagChar((5 * CMudTime::PULSES_PER_BATTLE_ROUND) - (nBonus) * CMudTime::PULSES_PER_BATTLE_ROUND);
  }
}

////////////////////////////////
//	DoShieldRush
//	written by: John Comes
//////////////////////////////

void CCharacter::DoShieldRush(const CInterp *pInterp, CCharacter *pTarget) {
  if (!GetSkill(SKILL_SHIELD_RUSH)) {
	SendToChar("You don't know how to do that!\r\n");
	return;
  }
  bool bHasShield = false;
  for (int i = 0; i < MAX_EQ_POS; i++) {
	if (m_Equipment[i].GetWearBit() & WEAR_POS_SHIELD && !m_Equipment[i].IsEmpty()) {
	  bHasShield = true;
	}
  }
  if (bHasShield) {
	short nSkill = GetSkill(SKILL_SHIELD_RUSH) + this->GetProficiency().CalcBonus(m_nSkillSphere) * 10;
	short nDie = DIE(100);
	if (pTarget) {
	  CString strToChar, strToRoom;
	  short int Adjust = 0;
	  if (!pTarget->InPosition(POS_STANDING)) {
		Adjust = DIE(25) + 25; // add up to 50% chance to miss if target is not standing.
	  }
	  if (nDie + Adjust <= nSkill) {
		pTarget->UpDatePosition(POS_SITTING, false);
		pTarget->LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * (DIE(3) + 1));
		pTarget->TakeDamage(this, nSkill / 5, true);
		if (pTarget->IsDead()) {
		  m_MsgManager.SkillMsg(this, pTarget, SKILL_SHIELD_RUSH, SKILL_KILLED);
		  pInterp->KillChar(pTarget);
		} else {
		  m_MsgManager.SkillMsg(this, pTarget, SKILL_SHIELD_RUSH, SKILL_WORKED);
		}
		if (nSkill - nDie > 75) {
		  pTarget->UpDatePosition(POS_STUNNED, true);
		}
		if (nSkill - nDie > 90) {
		  GetRoom()->SendToRoom("%s takes advantage of the shield rush!\r\n", this);
		  SendToChar("You take advantage of the shield rush!\r\n");
		  for (int i = 0; i < MAX_EQ_POS; i++) {
			if (!m_Equipment[i].IsEmpty() && m_Equipment[i]->IsWeapon()) {
			  for (int j = 0; j < nSkill / 30; j++) {
				pTarget->Defense(CalcACWouldHit() - this->GetProficiency().CalcBonus(m_nSkillSphere) * 10, i, this);
				if (pTarget->IsDead()) j = nSkill / 30;
			  }
			  i = MAX_EQ_POS;
			}
		  }
		}
	  } else {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_SHIELD_RUSH, SKILL_DIDNT_WORK);
		UpDatePosition(POS_SITTING, false);
		SkillUp(SKILL_SHIELD_RUSH);
	  }
	  AddFighters(pTarget);
	  LagChar(CMudTime::PULSES_PER_REAL_SECOND * 3);
	} else {
	  SendToChar("Shield Rush who?\r\n");
	}
  } else {
	SendToChar("A SHIELD rush requires a SHIELD... duh.\r\n");
  }
}


////////////////////////////////
//	DoWill
//	written by: John Comes
//////////////////////////////

void CCharacter::DoWill(const CInterp *pInterp) {
  SendToChar("Your Will is nothing, Talk to an Empath, weaking!\r\n");
}



//////////////////////////
//	Characters don't have vnums only mobs
//	for PC's we return there File ID and for 
//	mobs we return there vnum
// written by Demetrius Comes

long CCharacter::GetVnum() {
  //TODO:
  //look into speeding this up...we might be able
  //to save the file position when we bring the
  //	player into the game...but that's too much work
  //	for the benifit right now
  long lFilePos;
  if (m_PlayerFilePos.Lookup(m_strName, lFilePos)) {
	return lFilePos;
  } else {
	assert(0);
	return -1;
  }
}

/////////////////////////////////////
//	DoBuy 
//	Purchase something from the store
//	

void CCharacter::DoBuy(const CInterp * pInterp) {
  if (!m_pInRoom->IsShop()) {
	SendToChar("Em try being in a shop first!\r\n");
  } else {
	CString strItem(CurrentCommand.GetWordAfter(1));
	int nNum = strItem.MakeInt();
	long lCost;
	if ((lCost = ((CShopRoom *) m_pInRoom)->GetCost(nNum)) == -1) {
	  SendToChar("Buy what?\r\n");
	} else if (!((CShopRoom *) m_pInRoom)->CanPillage() &&
		!((CShopRoom *) m_pInRoom)->IsShopKeeperInRoom()) {
	  SendToChar("There doesn't see to be anyone here right now.\r\n");
	} else if (!m_CashOnHand.Take(lCost)) {
	  SendToChar("You don't have enought to purchase that!\r\n");
	} else {
	  room_vnum lCreateRoom = NOWHERE;
	  obj_vnum nObjVnum = ((CShopRoom *) m_pInRoom)->Buy(nNum, lCreateRoom);
	  CObject *pObj;
	  //do we create the object on the character or in a room
	  if (lCreateRoom != NOWHERE) {
		pObj = pInterp->CreateObj(nObjVnum, CWorld::FindRoom(lCreateRoom));
	  } else {
		pObj = pInterp->CreateObj(nObjVnum, ((CCharacter *) NULL));
		if (pObj != NULL && !Add(pObj)) {
		  SendToChar("You hands seem to be too full.\r\n");
		  pObj->SetDelete();
		  return;
		}
	  }
	  if (!pObj) {
		SendToChar("Sorry the that item can not be sold at this time.\r\n");
		ErrorLog << "BUILDER: " << GetName() << " just tried to purchase vnum: " << nObjVnum << " from shop at room vnum: " << GetRoom()->GetVnum()
			<< " failed b/c invalid vnum on object or room to be loaded into. " << endl;
		m_CashOnHand += lCost;
	  } else {
		CString strToChar;
		strToChar.Format("You have purchased the %s\r\n", (char *) pObj->GetObjName());
		SendToChar(strToChar);
	  }
	}
  }
}

/////////////////////////////
//	Sell
//	Allows a character to sell things
//	to a shop
//	

void CCharacter::DoSell(const CInterp * pInterp) {
  if (!m_pInRoom->IsShop()) {
	SendToChar("Em try being in a shop first!\r\n");
	return;
  }
  if (!((CShopRoom *) m_pInRoom)->IsShopKeeperInRoom()) {
	SendToChar("There no one here to sell to!\r\n");
	return;
  }
  bool bAll;
  CObject *pObj = GetTarget(1, true, bAll);
  if (!pObj) {
	SendToChar("You don't have that item to sell!\r\n");
  } else {
	long lPrice = ((CShopRoom *) m_pInRoom)->AddToSell(pObj, pInterp->GetObjManager());
	sMoney Price(lPrice);
	m_CashOnHand += lPrice;

	CString strToChar;
	strToChar.Format("You have sold the %s for %s\r\n", (char *) pObj->GetObjName(),
		Price.GetMoneyString().cptr());
	SendToChar(strToChar);
	Remove(pObj);
	CObjectManager::Remove(pObj);
  }
}

////////////////////////////
//	Virtual function to get mana adjustment
//	base is just int

short CCharacter::GetManaAdjust() {
  return m_pAttributes->GetManaAdjustInt();
}

///////////////////////////
//	virtual function
//	so we can overload for mana if we need it
//	written by: Demetrius Comes

CString CCharacter::GetVitalStats() {
  CString str;
  str.Format("%-10s [%5d/%5d] [%5d/%5d] [%5d/%5d]", m_strName.cptr(),
	  m_nCurrentHpts, m_nMaxHpts, m_nManaPts, m_nMaxManaPts, m_nMovePts, m_nMaxMovePts);
  return CString(str);
}

//////////////////////////////
//	Allows a drop objects

void CCharacter::DoDrop() {
  CString Com(CurrentCommand.GetWordAfter(1));
  CObject *pObj;
  int nDot, nNum = 1;
  if ((nDot = Com.Find('.')) != -1) {
	nNum = Com.Left(nDot).MakeInt();
	if (nNum == -1) {
	  if (!Com.Left(nDot).Compare("all")) {
		SendToChar("Drop how many?\r\n");
		return;
	  }
	}
	Com = Com.Right(nDot);
  }
  if (Com.Compare("all")) {
	POSITION pos = Inventory.GetStartPosition();
	while (pos) {
	  pObj = Inventory.GetNext(pos);
	  if (DropSingleItem(pObj)) {
		Remove(pObj, pos);
	  }
	}
  } else if (sMoney::IsCash(Com)) {
	try {
	  m_CashOnHand.FromString(Com, m_pInRoom->m_CashInRoom, nNum);
	} catch (CError *pErr) {
	  SendToChar(CString(pErr->GetMessage()));
	} catch (char *strMoney) {
	  CString strToChar;
	  strToChar.Format("You drop some %s.\r\n", strMoney);
	  SendToChar(strToChar);
	  m_pInRoom->SendToRoom("%s drops some %s.\r\n", this, CString(strMoney));
	}
  } else if (nNum != -1 && (pObj = FindInInventory(Com, nNum))) {
	if (DropSingleItem(pObj)) {
	  Remove(pObj);
	}
  }//nNum = -1 if all.
  else if (nNum == -1 && (pObj = FindInInventory(Com, 1))) {
	do {
	  if (DropSingleItem(pObj)) {
		Remove(pObj);
	  }
	} while ((pObj = FindInInventory(Com, 1)));
  } else {
	SendToChar("Drop what?\r\n");
  }
}

/////////////////////////////////
//	Drops a single item
//	set up as a bool incase we have an object 
//	your not allowed to drop

bool CCharacter::DropSingleItem(CObject * pObj) {
  CString strInfo;
  bool bRemoveObj = true;
  strInfo.Format("You drop the %s\r\n", (char *) pObj->GetObjName());
  SendToChar(strInfo);
  strInfo = pObj->GetObjName();
  m_pInRoom->SendToRoom("%s drops the %s\r\n", this, strInfo);

  m_pInRoom->Add(pObj, false);
  return bRemoveObj;
}

/////////////////////////////
//	gets a single item from a room
//	We throw out of here with an message if
//	item is not takeable or it's too heavy

void CCharacter::GetSingleItem(CObject * pObj) {
  CString strInfo;
  if (!pObj->IsAffectBit(CObject::ITEM_NOT_TAKEABLE)) {
	if (this->Add(pObj, false)) {
	  strInfo.Format("You get the %s\r\n", pObj->GetObjName());
	  SendToChar(strInfo);
	  m_pInRoom->SendToRoom("%s gets the %s\r\n", this, pObj->GetObjName());
	} else {
	  strInfo.Format("You can't lift %s\r\n", pObj->GetObjName());
	  CError Err(STOP_ERROR, strInfo.cptr());
	  throw &Err;
	}
  } else {
	strInfo.Format("%s is not take-able.\r\n",
		pObj->GetObjName());
	CError Err(MESSAGE_ONLY, strInfo.cptr());
	throw &Err;
  }
}

/////////////////////////////
//	Gets Stuff off the corpse
//	lags any player that doesn't have the skill loot
//	written by: Demetrius Comes
////////////////////////////

void CCharacter::DoLoot() {
  CString strTwo(CurrentCommand.GetWordAfter(1));
  int nDot, nNum = 1;
  if ((nDot = strTwo.Find('.')) != -1) {
	nNum = strTwo.Left(nDot).MakeInt();
	if (nNum == -1) {
	  if (!strTwo.Left(nDot).Compare("all")) {
		SendToChar("Loot how many?\r\n");
		return;
	  }
	}
	strTwo = strTwo.Right(nDot);
  }

  if (strTwo.IsEmpty()) {
	SendToChar("Loot who?\r\n");
  } else {
	CString strThird(CurrentCommand.GetWordAfter(2));
	CObject *pCon;
	//if the third is empty then loot <corpse name> is being done
	//so give it all to them
	if (strThird.IsEmpty()) {
	  if ((pCon = GetRoom()->FindObjectInRoom(strTwo, nNum, this))) {
		pCon->Loot(this);
	  } else {
		SendToChar("Loot what?\r\n");
	  }
	} else {
	  int nNum2 = 1;
	  if ((nDot = strThird.Find('.')) != -1) {
		nNum2 = strThird.Left(nDot).MakeInt();
		if (nNum2 == -1) {
		  if (!strThird.Left(nDot).Compare("all")) {
			SendToChar("Loot how many?\r\n");
			return;
		  }
		}
		strThird = strThird.Right(nDot);
	  }
	  if ((pCon = GetRoom()->FindObjectInRoom(strThird, nNum2, this))) {
		//pass the str of the thing they want, the number (-1 = all)
		//and the player that wants it
		pCon->Loot(strTwo, nNum, this);
	  } else {
		SendToChar("Loot what?\r\n");
	  }
	}
  }
  if ((int) DIE(100) > GetSkill(SKILL_LOOT)) {
	LagChar(CMudTime::PULSES_PER_BATTLE_ROUND);
  } else if (GetSkill(SKILL_LOOT) > 0) {
	SkillUp(SKILL_LOOT);
  }
}

//////////////////////////////
//	Has player take damager
//	has the attacker gain exp;
////////////////////////////

void CCharacter::GiveMana(CCharacter *pDefender, int nMana) {
  if (nMana == 0 || (this->Prefers(PREFERS_NO_HASSLE) && this->IsGod())) {
	return;
  }
  //figure in the PC TO PC damanger adjustment
  //TODO virtual and add to npc class and remove this if statement
  if (!IsNPC() && !pDefender->IsNPC() && nMana > 0) {
	nMana = (nMana * CMudTime::GetPCYearDamageAdjustment()) / 100;
  }
  this->m_nManaPts += RANGE(nMana, 50, 0);
  this->m_nManaPts = RANGE(m_nManaPts, m_nMaxManaPts, 0);
}

//////////////////////////////
//	Has player take damager
//	has the attacker gain exp;
////////////////////////////

void CCharacter::TakeDamage(CCharacter *pAttacker, int nDam, bool bFromMeele, bool bGiveExp) {
  if (nDam == 0 || (this->Prefers(PREFERS_NO_HASSLE) && this->IsGod())) {
	return;
  }
  //figure in the PC TO PC damanger adjustment
  //make sure nDam is greater than 0 so that we don't pentalize
  //healing spells, potions etc.
  if (!IsNPC() && !pAttacker->IsNPC() && nDam > 0) {
	nDam = (nDam * CMudTime::GetPCYearDamageAdjustment()) / 100;
  }
  m_nCurrentHpts = (m_nCurrentHpts - nDam) > m_nMaxHpts ? m_nMaxHpts : m_nCurrentHpts - nDam;
  UpDatePosition();
  if (pAttacker != this && bGiveExp) {
	//make dam pos
	nDam = (nDam >= 0) ? nDam : -nDam;
	//reduce if from meele
	int nExp = bFromMeele ? nDam >> 2 : nDam;
	//always get atleast one
	nExp = nExp <= 0 ? 1 : nExp;
	//give exp
	pAttacker->GainExp(this, nExp);
  }
}

///////////////////////////
//	GainExp
//	private only called from takedamage
//	if the defender is killed
//	give them exp for death and hit
//	check if grouped
///////////////////////////

void CCharacter::GainExp(CCharacter *pDefender, int nExp) {
  if (m_nLevel != LVL_IMP) {
	short nLevelDiff = pDefender->GetLevel() - this->GetLevel();
	if (nLevelDiff > 0) {
	  nLevelDiff = (nLevelDiff / LEVELS_PER_DAMAGE_EXP_BONUS)*10;
	  nExp += (nExp * nLevelDiff) / 100;
	} else {
	  nLevelDiff = (nLevelDiff / LEVELS_PER_DAMAGE_EXP_PENATLY)*10;
	  //+= b/c nLvl is neg
	  nExp += (nExp * nLevelDiff) / 100;
	  nExp = nExp < 0 ? 1 : nExp;
	}
	m_nExperience += nExp;
	if (pDefender->IsDead()) {
	  CString strLog;
	  MudLog << "DEATH: " << GetName() << " has just killed " << pDefender->GetName() << endl(strLog);
	  GVM.SendToGods(strLog, PREFERS_GOD_LOG_DEATHS, pDefender);
	  //adjust alignment
	  AdjustAlignment(pDefender);
	  int nExperience;
	  if (pDefender->IsNPC()) {
		//add thier exp as well.
		//this exp value is what is in ascii file as well as any exp they 
		nExperience = pDefender->GetMaxHits() + pDefender->GetExtraExp();
	  } else {
		nExperience = pDefender->GetMaxHits();
	  }
	  //gives partial frags to group members in the same room
	  //group takes care of if they are groupped or not!
	  m_Group.GroupGainExp(nExperience, this, pDefender);
	  if (!IsGod() && !pDefender->IsGod()) {
		//take exp from dead player
		//Experience Calculation fixed 2-12-99 DAC
		//we were missing () around the subtractions so Mult. was happening first.
		nExperience = ((pDefender->GetExp(pDefender->GetLevel()) - pDefender->GetExp((pDefender->GetLevel()) - 1)) * (EXP_LOST_IN_DEATH)) / 100;
		pDefender->m_nExperience -= nExperience;
		pDefender->m_nExperience = pDefender->m_nExperience > 0 ? pDefender->m_nExperience : 1;
		if (pDefender->m_nExperience < pDefender->GetExp(pDefender->m_nLevel)) {
		  //bug fix #41 by DAC
		  pDefender->AdvanceLevel(false, true);
		  //end
		  pDefender->m_nExperience = pDefender->m_nExperience <= 0 ? 1 : pDefender->m_nExperience;
		}
	  }
	}
	if (GetLevel() < LVL_LAST_MORTAL && (GetExp(GetLevel() + 1) < m_nExperience)) {
	  AdvanceLevel(true, true);
	}
  }
}

///////////////////////////
//	give one object to another character
//	written by: Demetrius Comes
//////////////////////////

void CCharacter::DoGive() {
  CString strObj(CurrentCommand.GetWordAfter(1));
  int nDot, nNum = 1;
  if ((nDot = strObj.Find('.')) != -1) {
	nNum = strObj.Left(nDot).MakeInt();
	strObj = strObj.Right(nDot);
  }

  CCharacter *pTarget;
  CObject *pObj;
  if ((pTarget = GetTarget(2, false))) {
	if (pTarget == this) {
	  SendToChar("Stop trying to give stuff to yourself.\r\n");
	} else if (sMoney::IsCash(strObj)) {
	  try {
		m_CashOnHand.FromString(strObj, pTarget->m_CashOnHand, nNum);
	  } catch (CError *pErr) {
		SendToChar(CString(pErr->GetMessage()));
	  } catch (char *strMoney) {
		CString strToChar;
		strToChar.Format("You give some %s to %s.\r\n", strMoney, pTarget->GetRaceOrName(this).cptr());
		SendToChar(strToChar);
		strToChar.Format("%s gives you some %s.\r\n", GetRaceOrName(pTarget).cptr(),
			strMoney);
		pTarget->SendToChar(strToChar);
		m_pInRoom->SendToRoom("%s gives some %s to %s.\r\n", this, CString(strMoney), pTarget);
	  }
	} else {
	  if ((pObj = FindInInventory(strObj, nNum))) {
		CString strToChar;
		if (pTarget->Add(pObj)) {
		  strToChar.Format("You give %s to %s.\r\n", pObj->GetObjName(),
			  pTarget->GetRaceOrName(this).cptr());
		  SendToChar(strToChar);
		  Remove(pObj);
		  strToChar.Format("You recieve %s from %s.\r\n",
			  pObj->GetObjName(), GetRaceOrName(pTarget).cptr());
		  pTarget->SendToChar(strToChar);
		  GetRoom()->SendToRoom("%s gives %s to %s.\r\n", this, (char *) pObj->GetObjName(), pTarget);
		} else {
		  strToChar.Format("%s's hands seem to be full.\r\n",
			  pTarget->GetRaceOrName(this).cptr());
		  SendToChar(strToChar);
		}
	  } else {
		SendToChar("You don't have that!\r\n");
	  }
	}
  } else {
	SendToChar("Give what to who?\r\n");
  }
}

void CCharacter::DoSneak(const CInterp *pInterp) {
  SendToChar("Only the mighty Rogue can Sneak!\r\n");
}

void CCharacter::DoHide() {
  CString strObj(CurrentCommand.GetWordAfter(1));
  if (strObj.IsEmpty()) {
	SendToChar("Peak a boo.\r\n");
  } else {
	bool bAll;
	CObject *pObj = GetTarget(1, false, bAll);
	if (pObj == NULL) {
	  SendToChar("Hide what?\r\n");
	} else if (!StatCheck(LUCK)) {
	  SendToChar("Hrm still looks out in the open.\r\n");
	} else {
	  pObj->AddAffect(CObject::OBJ_AFFECT_HIDDEN, sAffect::MANUAL_AFFECT);
	  CString strToChar;
	  strToChar.Format("You hide the %s.\r\n",
		  pObj->GetObjName());
	  SendToChar(strToChar);
	}
  }
}

///////////////////////////////////////
//	DoConsider
//	
//
//////////////////////////////////////

void CCharacter::DoConsider() {

  CCharacter *pTarget = GetTarget();
  if (!pTarget) {
	SendToChar("Consider who?\r\n");
  } else {
	CString str("%s sizes up %s with a quick glance.\r\n");
	GetRoom()->SendToRoom(str, this, pTarget);
	str.Format("%s sizes you up with a quick glance.\r\n", GetRaceOrName(pTarget).cptr());
	pTarget->SendToChar(str);
	int LevDif = pTarget->GetLevel() - GetLevel();
	if (LevDif == 0)
	  SendToChar("Aye tis about the same skill level, methinks.\r\n");
	else if (LevDif == 1)
	  SendToChar("Aye tis slighly tougher, this one.\r\n");
	else if (LevDif == 2)
	  SendToChar("This may be require some luck, methinks.\r\n");
	else if (LevDif >= 3 && LevDif <= 4)
	  SendToChar("Heh, Good luck.\r\n");
	else if (LevDif >= 5 && LevDif <= 10)
	  SendToChar("Are ye mad?\r\n");
	else if (LevDif >= 11 && LevDif <= 20)
	  SendToChar("Aye! Ye ARE mad!\r\n");
	else if (LevDif >= 21 && LevDif <= 30)
	  SendToChar("Ye hear the city of the dead calling, don't ye?\r\n");
	else if (LevDif >= 31)
	  SendToChar("Foolish Mortal, Don't be stupid.\r\n");
	else if (LevDif == -1)
	  SendToChar("Aye, easy it is.\r\n");
	else if (LevDif == -2)
	  SendToChar("Methinks this is on the easy side.\r\n");
	else if (LevDif <= -3 && LevDif >= -4)
	  SendToChar("A little below you, no?\r\n");
	else if (LevDif <= -5 && LevDif >= -10)
	  SendToChar("Are ye going to waste yer time?\r\n");
	else if (LevDif <= -11 && LevDif >= -20)
	  SendToChar("Ye ARE going to waste yer time!\r\n");
	else if (LevDif <= -21 && LevDif >= -30)
	  SendToChar("Methinks you would have more challenge with a chicken.\r\n");
	else if (LevDif <= -31)
	  SendToChar("Foolish Mortal, Don't cough or you'll kill it.\r\n");
  }
}

/////////////////////////////////
//	DoBandage()
//	
////////////////////////////////

void CCharacter::DoBandage() {
  CCharacter *pTarget = GetTarget(1, false);
  if (!pTarget) {
	SendToChar("Bandage who?\r\n");
  } else if (pTarget->InPosition(POS_INCAP)) {
	CString strToChar;
	pTarget->TakeDamage(this, -DIE(3), false, false);
	pTarget->UpDatePosition();
	GetRoom()->SendToRoom("%s bandages %s, who's wounds stop &rbleeding&n as much.\r\n", this, pTarget);
	strToChar.Format("You bandage %s's wounds so that they don't &rbleed&n as much.\r\n", pTarget->GetRaceOrName(this).cptr());
	SendToChar(strToChar);
	LagChar(CMudTime::PULSES_PER_REAL_SECOND * DIE(3));
	SkillUp(SKILL_BANDAGE);
  } else {
	SendToChar("Why do they need bandaged?\r\n");
  }
}

////////////////////////////////
//	Set up initial skills for newbies
//
////////////////////////////////

void CCharacter::InitSkills() {
  for (int i = 0; i < MAX_MUD_SKILLS; i++) {
	m_Skills[i] = this->GetSkillInfo(i)->GetMin();
  }
}

////////////////////////////
//	SkillUp
//	Given a skill, we check to insure they
//	have this skill, if so we
//	get skill info and see if they skill up.
//	written by: Demetrius Comes
//	10-12-98
/////////////////////////////

void CCharacter::SkillUp(short nSkill) {
  if (GetSkill(nSkill)) {
	if (this->GetSkillInfo(nSkill)->GetMax() > GetSkill(nSkill)
		&& ((this->GetSkillInfo(nSkill)->GetPct() + MIN(this->GetLearingBonus(), 3)) >= DIE(DIE_ROLL_FOR_SKILL_UP))) {
	  //actually changing the skill do not use getskill!
	  m_Skills[nSkill]++;
	  CString strToChar;
	  strToChar.Format("&cYou feel your skill in %s increasing.&n\r\n",
		  FindSkill(nSkill).cptr());
	  SendToChar(strToChar);
	}
  }
}

///////////////////////////
//	GetLearingBonus()
//	calling function is SkillUp and max it will allow is 3
//	Base classes just use wisdom
//	but can be overridden
//	written by: Demetrius Comes
///////////////////////////

short CCharacter::GetLearingBonus() {
  return m_pAttributes->AffectedStats[WIS] / 40;
}

/////////////////////////////
//	Regenerate
//	Regens hit points
//	mana points
//	move points
//	written by: Demetrius Comes
////////////////////////////

void CCharacter::Regenerate() {
  if (!(CMudTime::GetCurrentPulse() % PULSES_PER_REGENERATION_TIME)) {
	short nPts = 1;
	if (InPosition(POS_RESTING) || InPosition(POS_SITTING)) {
	  nPts++;
	} else if (InPosition(POS_RECLINING) && InPosition(POS_SLEEPING)) {
	  nPts += 3;
	} else if (InPosition(POS_SLEEPING) || InPosition(POS_RECLINING)) {
	  nPts += 2;
	}
	if (m_pAttributes->m_pRaceReference->HasRegen()) {
	  nPts *= 3;
	}
	if (IsAffectedBy(AFFECT_CHANT_REGENERATION)) {
	  nPts += GetSkill(SKILL_CHANT_REGENERATION) / 20;
	}
	if (GetRoom()->IsAffectedBy(CRoom::ROOM_AFFECT_FAST_HEAL)) {
	  nPts *= 2;
	}
	if ((m_nCurrentHpts += nPts) > m_nMaxHpts) {
	  m_nCurrentHpts = m_nMaxHpts;
	}
  }
  if (!(CMudTime::GetCurrentPulse() % PULSES_PER_MOVEMENT_REGENERATION_TIME)) {
	short nMovePts = 2;
	if (InPosition(POS_RESTING)) {
	  nMovePts++;
	} else if (InPosition(POS_RECLINING) || InPosition(POS_SLEEPING)) {
	  nMovePts *= 2;
	}
	if ((m_nMovePts += nMovePts) > m_nMaxMovePts) {
	  m_nMovePts = m_nMaxMovePts;
	}
  }
  if (!(CMudTime::GetCurrentPulse() % PULSES_PER_MANA_REGENERATION_TIME)) {
	m_nManaPts += this->ManaCanRegain();
  }
}

///////////////////////////////
//	ManaCanRegain
//	virtual function
//	

short CCharacter::ManaCanRegain(bool bTotal) {
  if (m_nManaPts >= m_nMaxManaPts) {
	return 0;
  } else if (bTotal) {
	return(m_nMaxManaPts - m_nManaPts < 0 ? 0 : m_nMaxManaPts - m_nManaPts);
  } else {
	//by using the getmanaadjust we don't have
	//to over ride this function for psi's because
	//they only thing that would change is the
	//mana per Turn;
	short nManaRegain = this->GetManaAdjust();
	//if they are not meditating or fail skill
	//check then they lose 1/2 mana they would regain
	if (!IsAffectedBy(CCharacter::AFFECT_MEDITATE)) {
	  nManaRegain /= 2;
	  nManaRegain = nManaRegain > 0 ? nManaRegain : 1;
	} else if (IsAffectedBy(CCharacter::AFFECT_MEDITATE)
		&& DIE(100) > GetSkill(SKILL_MEDITATE)) {
	  nManaRegain /= 2;
	  SkillUp(SKILL_MEDITATE);
	}
	if ((m_nManaPts + nManaRegain) >= m_nMaxManaPts) {
	  return m_nMaxManaPts - m_nManaPts;
	} else {
	  return nManaRegain;
	}
  }
}

//////////////////////////
//	DoMediate
//	Set affect on character for mediation
//	written by: Demetrius Comes
//////////////////////////

void CCharacter::DoMediate() {
  if (GetSkill(SKILL_MEDITATE)) {
	SendToChar("You start meditating.\r\n");
	AddAffect(AFFECT_MEDITATE, sAffect::MANUAL_AFFECT);
  } else {
	SendToChar("You don't know how to mediate.\r\n");
  }
}

/////////////////////////////
//	DoForget
//	Basically the duh function if not a preparing caster
//	written by: Demetrius Comes
//	12-7-98
////////////////////////////

void CCharacter::DoForget() {
  SendToChar("Forgetting things is a part of growing old.\r\n");
}

/////////////////////////
//	wear
//	interface so we can give a character
//  usually an NPC and have them wear a 
//	particular object
//	written by: Demetrius Comes
//

void CCharacter::Wear(CObject * pObj) {
  if (!Inventory.DoesContain(pObj)) {
	ErrorLog << "Inventory does not contain object vnum " << pObj->GetVnum() << endl;
  }
  if (pObj->CanbeWornBy(this)) {
	if (m_pAttributes->m_pRaceReference->PutEqOn(this, pObj)) {
	  Remove(pObj);
	}
  } else {
	ErrorLog << "Mob : " << this->GetVnum() << " can not wear " << pObj->GetObjName() << " Vnum: " << pObj->GetVnum() << endl;
  }
}

///////////////////////////
//	DoEnter
//	Enters a portal, ship etc

void CCharacter::DoEnter() {
  bool bAll;
  CObject *pObj = this->GetTarget(1, false, bAll);
  if (pObj) {
	CRoom *pRoom;
	if ((pRoom = pObj->Add(this))) {
	  GetRoom()->SendToRoom("%s enters %s.\r\n", this, pObj);
	  GetRoom()->Remove(this);
	  m_pInRoom = pRoom;
	  GetRoom()->Add(this);
	  AddCommand(CString("look"));
	}
  } else {
	SendToChar("Enter What?\r\n");
  }
}

////////////////////////
//	Dummy function for virtual classes

void CCharacter::SendSpellList() {
  SendToChar("You don't have a spell list!\r\n");
}

//////////////////////////
//	Remove
//	Searches the inventory first
//	if not found searches eq...
//	as of 1-23-99 this function is
//  only called by CObject when object
//	is marked for deletion
//////////////////////////

void CCharacter::RemoveObjFromChar(CObject * pObj) {
  if (Inventory.DoesContain(pObj)) {
	Inventory.Remove(pObj);
  } else {
	for (int i = 0; i < MAX_EQ_POS; i++) {
	  if (m_Equipment[i].GetObject() == pObj) {
		RemoveEquipment(i, true);
		return;
	  }
	}
  }
}

//////////////////////////////////////////
//	Get Target
//	
//	Returns a pointer to a CCharacter of the target
//	If target is not in room, returns NULL
//	If no name is placed into target, returns person you are fighting
//	John Comes 1-25-99
//	Updated:
//	if we are fighting then if we typed the char's alias or not 
//	use the same rules, give back the pointer if who (this) is fighting
//	is not dead and bAllowfight = true we are fighting return IsFighting()
//	else use all vision checks for light
/////////////////////////////////////////

CCharacter * CCharacter::GetTarget(int nWordAfter, bool bAllowFight) {
  CString strTarget(CurrentCommand.GetWordAfter(nWordAfter));
  int nDot, nNum = 1;
  if ((nDot = strTarget.Find('.')) != -1) {
	nNum = strTarget.Left(nDot).MakeInt();
	strTarget = strTarget.Right(nDot);
  }
  if (strTarget.IsEmpty()) {
	if (IsFighting() && bAllowFight && !IsFighting()->IsDead() && GetRoom()->IsInRoom(IsFighting())) {
	  return IsFighting();
	}
	return NULL;
  } else {
	CCharacter *pTarget = GetRoom()->FindCharacterByAlias(strTarget, nNum, this);

	if (bAllowFight && IsFighting() && pTarget == IsFighting() && !IsFighting()->IsDead()) {
	  return pTarget;
	} else {
	  if (!pTarget || !CanSeeChar(pTarget)
		  || (CanSeeInRoom(GetRoom()) == SEES_NOTHING && pTarget != this)
		  || pTarget->IsDead()) {
		return NULL;
	  }
	  return pTarget;
	}
  }
}

//////////////////////////////////////////
//	Get Target
//	
//	Returns a pointer to a CObject of the target
//	If target is not in room or in inventory, returns NULL
//	If no name is placed into target, returns person you are fighting
//	Demetrius Comes 2-14-99
/////////////////////////////////////////

CObject * CCharacter::GetTarget(int nWordAfter, bool bInventory, bool &bAll) {
  bAll = false;
  CString strTarget(CurrentCommand.GetWordAfter(nWordAfter));
  int nDot, nNum = 1;
  if ((nDot = strTarget.Find('.')) != -1) {
	nNum = strTarget.Left(nDot).MakeInt();
	if (strTarget.Compare("all")) {
	  bAll = true;
	}
	strTarget = strTarget.Right(nDot);
  }
  CObject *pTarget = bInventory ? FindInInventory(strTarget, nNum) : GetRoom()->FindObjectInRoom(strTarget, nNum, this);

  if (!pTarget || !CanSee(pTarget)) {
	return NULL;
  }
  return pTarget;
}

////////////////////////////////////////
//	AddFighters
//	I did an offensive act, now add us to
//	the fighting list if we're not there.
//	John Comes 1-25-99
//	bool bAddMeTo added 1-6-99
//	if false we don't add the this pointer to
//	fightLL ...used for spell attacks
//	bAddMeTo is defaulted to = true
////////////////////////////////////////

void CCharacter::AddFighters(CCharacter *pTarget, bool bAddMeTo) {
  if (!pTarget->IsDead() && !this->IsDead()
	  && pTarget != this) {
	if (!pTarget->IsFighting()) {
	  CGame::AddFighter(pTarget);
	  pTarget->AssertFighting(this);
	}
	if (bAddMeTo && !this->IsFighting()) {
	  CGame::AddFighter(this);
	  AssertFighting(pTarget);
	}
  }
}

/////////////////////////////////////////
//	Stat Check
//	if random number is less than stat
//	return false
//	John Comes 1-26-99
/////////////////////////////////////////

bool CCharacter::StatCheck(short int nStat, short nModifier) {
  short int nNum = DIE(100) + 1;
  if (nNum <= m_pAttributes->AffectedStats[nStat] + nModifier) {
	return true;
  }
  return false;
}

////////////////////////////////////////
//	Experience
//	Tells how close you are to next level
//
//	John Comes 1-26-99
/////////////////////////////////////////

void CCharacter::DoExperience() {
  float nPercent = (float) GetExpInLevel() / (float) GetExpGap();
  if (nPercent <= 0.1) {
	SendToChar("&BYou have just begun your trek to the next level.&n\r\n");
  } else if (nPercent >= 0.1 && nPercent <= 0.2) {
	SendToChar("&BYou still have a long way to your next level.&n\r\n");
  } else if (nPercent >= 0.2 && nPercent <= 0.3) {
	SendToChar("&BYou are one quarter of the way to your next level.&n\r\n");
  } else if (nPercent >= 0.3 && nPercent <= 0.4) {
	SendToChar("&BYou have gained some progress along the path to your next level.&n\r\n");
  } else if (nPercent >= 0.4 && nPercent <= 0.5) {
	SendToChar("&BYou are close to the half way point towards your next level.&n\r\n");
  } else if (nPercent == 0.5) {
	SendToChar("&BYou are at the half way point!&n\r\n");
  } else if (nPercent >= 0.5 && nPercent <= 0.6) {
	SendToChar("&BYou have just passed the half way point towards your next level.&n\r\n");
  } else if (nPercent >= 0.6 && nPercent <= 0.7) {
	SendToChar("&BYou are well on your way to the next level.&n\r\n");
  } else if (nPercent >= 0.7 && nPercent <= 0.8) {
	SendToChar("&BYou are three quarters of the way to your next level.&n\r\n");
  } else if (nPercent >= 0.8 && nPercent <= 0.9) {
	SendToChar("&BYou are almost ready to attain your next level.&n\r\n");
  } else if (nPercent >= 0.9) {
	SendToChar("&BYou should level anytime now!&n\r\n");
  }
}

//////////////////////////////////
//	DoBodySlam
//	John Comes 1-28-99
/////////////////////////////////

void CCharacter::DoBodySlam(const CInterp *pInterp, CCharacter *pTarget) {
  if (!GetSkill(SKILL_BODYSLAM)) {
	SendToChar("Learn how first!\r\n");
  } else if (pTarget == this) {
	SendToChar("You slam yourself around for a while.\r\n");
  } else if (pTarget) {
	if (CanBodySlam(pTarget)) {
	  SendToChar("Pick on someone your own size.\r\n");
	} else {
	  CString strToChar, strToRoom;
	  short int Adjust = 0;
	  if (!pTarget->InPosition(POS_STANDING)) {
		Adjust = DIE(25) + 25; // add up to 50% chance to miss if target is not standing.
	  }
	  if ((short int) DIE(100) + Adjust <= GetSkill(SKILL_BODYSLAM)) {
		if (pTarget->MakeSkillSave(SKILL_BODYSLAM, 0)) {
		  m_MsgManager.SkillSaveMsg(pTarget, this, "bodyslam");
		  UpDatePosition(POS_SITTING, false);
		  LagChar(CMudTime::PULSES_PER_BATTLE_ROUND);
		} else {
		  pTarget->TakeDamage(this, GetSkill(SKILL_BODYSLAM) / 5, true);
		  if (pTarget->IsDead()) {
			pInterp->KillChar(pTarget);
			m_MsgManager.SkillMsg(this, pTarget, SKILL_BODYSLAM, SKILL_KILLED);
		  } else {
			m_MsgManager.SkillMsg(this, pTarget, SKILL_BODYSLAM, SKILL_WORKED);
			if (!pTarget->StatCheck(CON)) {
			  pTarget->UpDatePosition(POS_STUNNED, false);
			}
			pTarget->UpDatePosition(POS_SITTING, false);
			pTarget->LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * (DIE(3) + 1));
		  }
		}
	  } else {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_BODYSLAM, SKILL_DIDNT_WORK);
		if (!StatCheck(CON)) {
		  pTarget->UpDatePosition(POS_STUNNED, false);
		}
		UpDatePosition(POS_SITTING, false);
		LagChar(CMudTime::PULSES_PER_REAL_SECOND * 3);
	  }
	  SkillUp(SKILL_BODYSLAM);
	  AddFighters(pTarget);
	}
  } else {
	SendToChar("BodySlam who?\r\n");
  }
}

//////////////////////////////////
//	DoKick
//	John Comes 1-28-99
/////////////////////////////////

void CCharacter::DoKick(const CInterp *pInterp, CCharacter *pTarget) {
  if (!GetSkill(SKILL_KICK)) {
	SendToChar("Learn how first!\r\n");
  } else if (pTarget == this) {
	SendToChar("You kick yourself for a while.\r\n");
  } else if (pTarget) {
	CString strToChar, strToRoom;
	int nAdjust = GetClass() == CLASS_WARRIOR ? DIE(3) : 1;
	if ((short int) DIE(100) <= GetSkill(SKILL_KICK)) {
	  pTarget->TakeDamage(this, GetSkill(SKILL_KICK) / 2 * nAdjust, true);
	  if (pTarget->IsDead()) {
		pInterp->KillChar(pTarget);
		m_MsgManager.SkillMsg(this, pTarget, SKILL_KICK, SKILL_KILLED);
	  } else {
		m_MsgManager.SkillMsg(this, pTarget, SKILL_KICK, SKILL_WORKED);
	  }
	} else {
	  m_MsgManager.SkillMsg(this, pTarget, SKILL_KICK, SKILL_DIDNT_WORK);
	  SkillUp(SKILL_KICK);
	}
	AddFighters(pTarget);
	LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * 2);
  } else {
	SendToChar("Kick who?\r\n");
  }
}


//////////////////////////////////
//	
//	Ya hit me with a spell how much damange?
//	written by: John Comes
//	2-2-99
//////////////////////////////////

void CCharacter::SpellDefense(int nDam, CCharacter * pAttacker, short nLevel) {
  nDam = DoProtectionSpells(pAttacker, nDam, nLevel);
  if (nDam > 0) {
	AddMemory(pAttacker);
  }
  TakeDamage(pAttacker, nDam, false);
}

//////////////////////////////
//	Load the objects on a player from
//	save

void CCharacter::LoadObjects() {
  CString strObjFile(sObjectSave::DIR);
  strObjFile += m_strName;
  std::ifstream ObjFile(strObjFile.cptr(), std::ios_base::binary | std::ios_base::in);
  //seek to the end
  ObjFile.seekg(0, std::ios_base::end);
  //get file pos for size
  long lSize = ObjFile.tellg();
  //is it corrupt
  if (lSize % sizeof(sObjectSave) && lSize > 0) {
	ErrorLog << m_strName << " Object File is currupt !\r\n" << endl;
	//will want to do more with this later!
  } else if (lSize > 0) {
	//reset the file pointer
	ObjFile.seekg(0, std::ios_base::beg);
	sObjectSave ObjSave;
	CObject *pObj;
	while (ObjFile.good() && (lSize != ObjFile.tellg())) {
	  ObjFile.read((char *) &ObjSave, sizeof(sObjectSave));
	  //put in inventory or eq
	  pObj = CObjectManager::CalObjectPointer(ObjSave, this, NULL);

	  if (pObj != NULL) {
		pObj->LoadContents(ObjFile, ObjSave.GetContentsCount());
		CObjectManager::Add(pObj);
		if (ObjSave.InventoryObj()) {
		  //this is causing the m_pCarried by pointer to be NULL!
		  //Inventory.Add(pObj,false);
		  //the add function takes care of weight and pointers for us
		  this->Add(pObj, false, true);
		} else {
		  //(CObject * pObj, UINT nWearBit, int nPos, bool bApplyCharAffects)
		  //re-wear eq but with out sending any messages!  two reasons
		  //we don't want characters seeing them selves re-equip...and
		  //this gets called from a constructor and if we send messages
		  //we will call a virtual function
		  WearEquipment(pObj, ObjSave.GetWornOnBit(), ObjSave.GetWearPos(), false);
		}
	  } else {
		CString str;
		ErrorLog << GetName() << " has a currupt/removed object in thier object save file. " << ObjSave.m_strObjName << " vnum: " << ObjSave.m_nVnum << endl;
		str.Format("The object %s is currupt in your object file...please contact a god about this problem.",
			ObjSave.m_strObjName);
		SendToChar(str);
	  }
	}
  }
}

void CCharacter::DoIdentify() {
  SendToChar("You don't know how.\r\n");
  return;
}

void CCharacter::DoAppraise() {
  SendToChar("You don't know how.\r\n");
  return;
}

/////////////////////////
//	Hone Weapon
//
//
//	John Comes 3-1-99
//////////////////////////

void CCharacter::DoHoneWeapon() {
  if (!GetSkill(SKILL_HONE_WEAPON)) {
	SendToChar("You don't know how to do that!\r\n");
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
  if ((pObj = FindInInventory(strItem, nNum)) && pObj->IsWeapon()) {
	if (pObj->IsAffectedBy(CObject::OBJ_AFFECT_HONE_WEAPON)) {
	  SendToChar("This weapon is already honed!\r\n");
	  return;
	}
	if (pObj->IsAffectedBy(CObject::OBJ_AFFECT_MISSED_HONE)) {
	  SendToChar("This weapon has been honed so poorly it can't be fixed.\r\n");
	  return;
	}
	nNum = DIE(100);
	if (nNum <= GetSkill(SKILL_HONE_WEAPON)) {
	  pObj->AddAffect(CObject::OBJ_AFFECT_HONE_WEAPON);
	  m_MsgManager.SkillMsg(this, this, SKILL_HONE_WEAPON, SKILL_WORKED);
	} else if (nNum - GetSkill(SKILL_HONE_WEAPON) > 75) {
	  pObj->AddAffect(CObject::OBJ_AFFECT_MISSED_HONE);
	  m_MsgManager.SkillMsg(this, this, SKILL_HONE_WEAPON, SKILL_KILLED);
	  SkillUp(SKILL_HONE_WEAPON);
	} else {
	  m_MsgManager.SkillMsg(this, this, SKILL_HONE_WEAPON, SKILL_DIDNT_WORK);
	  SkillUp(SKILL_HONE_WEAPON);
	}
	LagChar(CMudTime::PULSES_PER_REAL_SECOND * 4);
	SkillUp(SKILL_HONE_WEAPON);
  } else {
	SendToChar("You can only hone weapons.\r\n");
  }
}



//////////////////////////
//	DoWorship
//
//	Changes m_nWorshipedGod to whatever god is entered
//	John Comes 2-12-99

void CCharacter::DoWorship() {
  if (GetLevel() < 20) {
	SendToChar("You must reach the level of &W20&n to worship a god.\r\n");
  } else if (GetGod() != GOD_NONE) {
	SendToChar("You are already worshipping a god!\r\n");
  } else {
	CString strGod = CurrentCommand.GetWordAfter(1);
	CString strToChar;
	int i = 0;
	while (!strGod.Compare(GVM.GetGodName(i)) && i < TOTAL_GODS) {
	  i++;
	}
	if (strGod.Compare(GVM.GetGodName(i))) {
	  m_nWorshipedGod = i;
	  strToChar.Format("You are now worshipping %s.\r\n", GVM.GetColorGod(i));
	  SendToChar(strToChar);
	} else {
	  SendToChar("There is no God by that name.\r\n");
	}
  }
}

/////////////////////////////////////
//	Moves Character to Home Room after they leave the FuguePlane
//
//

void CCharacter::LeaveFuguePlane() {
  CString strToRoom, strToChar, strGod;
  if (GetGod() == GOD_NONE) {
	strGod = "&wNecrogorgan the &LLi&rch &wGod of the &LDead&n";
  } else {
	strGod = GVM.GetColorGod(GetGod());
  }
  strToRoom.Format("%s &wflies in and returns ", strGod.cptr());
  strToChar = strToRoom;
  strToRoom += "%s to the realm of the living.\r\n";
  GetRoom()->SendToRoom(strToRoom, this);
  GetRoom()->Remove(this);
  if ((m_pInRoom = CWorld::FindRoom(m_lPlayerSetHomeTown)) == NULL) {
	if ((m_pInRoom = CWorld::FindRoom(m_pAttributes->GetHomeTown())) == NULL) {
	  ErrorLog << GetName() << "'s home town or player set home town could not be found!" << endl;
	  SendToChar("You home town could not be found...contact a god.\r\n");
	  m_pInRoom = CWorld::FindRoom(FUGUE_PLANE);
	}
  }
  m_lLastRentRoom = m_pInRoom->GetVnum();
  GetRoom()->Add(this);
  GetRoom()->SendToRoom(strToRoom, this);
  AddCommand((CString) "look");
  strToChar += "you to the realm of the living...\r\n";
  SendToChar(strToChar);
}


///////////////////////////////////////
//	Sets Time spent in the Fugue Plane for a character
//
//	John Comes 2-12-99

void CCharacter::FugueTime() {
  CString strChar = CurrentCommand.GetWordAfter(1);

  CCharacter *pTarget = GVM.FindCharacter(strChar, this);
  if (!pTarget) {
	SendToChar("Set who's Fugue Plane time??\r\n");
	return;
  }
  CString strTime = CurrentCommand.GetWordAfter(2);
  CString strToChar;
  int nTime = strTime.MakeInt();
  pTarget->SetFugueTime(nTime * CMudTime::PULSES_PER_REAL_SECOND);
  strToChar.Format("%s's Fugue Plane time is now: %s seconds.\r\n", pTarget->GetName().cptr(), strTime.cptr());
  SendToChar(strToChar);
}


////////////////////////////
//	Quick Strike
// John Comes 2-12-99

void CCharacter::DoStrike(const CInterp *pInterp, CCharacter *pTarget) {
  if (!GetSkill(SKILL_QUICK_STRIKE)) {
	SendToChar("You don't know how to do that!");
  } else if (!pTarget) {
	SendToChar("Strike Who?\r\n");
  } else {
	for (int i = 0; i < MAX_EQ_POS; i++) {
	  if (!m_Equipment[i].IsEmpty() && m_Equipment[i]->IsWeapon()) {
		if (DIE(100) <= GetSkill(SKILL_QUICK_STRIKE)) {
		  pTarget->Defense(CalcACWouldHit(), i, this);
		  if (pTarget->IsDead()) {
			pInterp->KillChar(pTarget);
		  }
		  i = MAX_EQ_POS;
		} else {
		  SkillUp(SKILL_QUICK_STRIKE);
		}
		AddFighters(pTarget);
	  }
	}
	LagChar((4 * CMudTime::PULSES_PER_BATTLE_ROUND) - (this->GetProficiency().CalcBonus(m_nSkillSphere)) * CMudTime::PULSES_PER_BATTLE_ROUND);
  }
}

///////////////////////////////
//	puts object in an object

bool CCharacter::PutObjInObj(CObject *pObj, CContainer *pCon) {
  CString strToChar;
  //can't put an object inside itself
  if (pObj == pCon) {
	SendToChar("How are you going to do that?\r\n");
	return false;
  } else {
	//remove the wieght of the container...we do this
	//b/c the wieght affect of an object decreases when it
	// is in an container.
	m_nWeightCarried -= pCon->GetWeight();
	//add object to container if it will fit
	if (pCon->Add(pObj)) {
	  m_nWeightCarried += pCon->GetWeight();
	  //send message
	  strToChar.Format("You put %s in %s\r\n",
		  pObj->GetObjName(),
		  pCon->GetObjName());
	  SendToChar(strToChar);
	  strToChar.Format("%s puts %s in %s\r\n",
		  "%s",
		  pObj->GetObjName(),
		  pCon->GetObjName());
	  GetRoom()->SendToRoom(strToChar, this);
	  return true;
	} else {
	  m_nWeightCarried += pCon->GetWeight();
	  strToChar.Format("%s won't fit in %s\r\n",
		  pObj->GetObjName(),
		  pCon->GetObjName());
	  SendToChar(strToChar);
	  return false;
	}
  }
}
/////////////////////////
//	DoPut
//	put obj in obj

void CCharacter::DoPut() {
  CObject *pObj;
  bool bDummy;
  //Get 3rd word and check to see if it's an object
  //in the inventory
  CObject *pCon = GetTarget(2, true, bDummy);
  if (pCon) {
	//insure object is a container
	if (pCon->IsType(ITEM_TYPE_CONTAINER)) {
	  if (!((CContainer *) pCon)->IsOpen()) {
		SendToChar("You have to open it first!\r\n");
	  } else {
		//Is the second word all eg 'put all bp'
		if (CurrentCommand.GetWordAfter(1).Compare("all")) {
		  POSITION pos = Inventory.GetStartPosition();
		  while (pos) {
			pObj = Inventory.GetNext(pos);
			if (pObj != pCon) {
			  if (PutObjInObj(pObj, (CContainer *) pCon)) {
				Remove(pObj, pos);
			  }
			}
		  }
		}//for put <obj name> <container>
		  //if there isn't an object in inventory
		else if (!(pObj = GetTarget(1, true, bDummy))) {
		  SendToChar("Put what?\r\n");
		} else {
		  //insure we don't say 'put bp bp'
		  //must be 'put bp 2.bp'
		  if (pCon != pObj) {
			if (PutObjInObj(pObj, (CContainer *) pCon)) {
			  Remove(pObj);
			}
		  } else {
			SendToChar("How are you going to do that?\r\n");
		  }
		}
	  }
	} else {
	  SendToChar("You can only put stuff in containers.\r\n");
	}
  } else {
	SendToChar("Put in what?\r\n");
  }
}


//////////////////////////////////////
//	Release memory for objects
//	runs though All calling delete contents
//	That removes each object from Global LL
//	and from inventory or Equipment arrray

void CCharacter::ReleaseMemoryForObjects() {
  //Delete contents is possibly recursive so
  // we delete the contents, remove from non
  //memory delocating LL (INventory) then
  //Remove from global LL that delocates memory
  for (int i = 0; i < MAX_EQ_POS; i++) {
	if (!m_Equipment[i].IsEmpty()) {
	  //delete contents
	  m_Equipment[i].GetObject()->DeleteContents();
	  //delete from global LL
	  CObjectManager::Remove(RemoveEquipment(i, false));
	}
  }
  POSITION pos = Inventory.GetStartPosition();
  CObject *pObj;
  while (pos) {
	pObj = Inventory.GetNext(pos);
	pObj->DeleteContents();
	Remove(pObj, pos);
	CObjectManager::Remove(pObj);
  }
}



//
// Trip
// Written by John Comes
// 9/20/98

void CCharacter::DoTrip(const CInterp *pInterp, CCharacter *pTarget) {
  if (GetSkill(SKILL_TRIP) <= 0) {
	SendToChar("You must learn how to first.\r\n");
  } else if (!pTarget) {
	SendToChar("Try fighting him first?\r\n");
  } else if (pTarget == this) {
	SendToChar("Ok you trip yourself.\r\n");
  } else {
	CString strToChar, strToRoom;
	if (DIE(100) <= GetSkill(SKILL_TRIP)) {
	  if (pTarget->MakeSkillSave(SKILL_TRIP, 0)) {
		m_MsgManager.SkillSaveMsg(pTarget, this, "trip");
		LagChar(CMudTime::PULSES_PER_BATTLE_ROUND);
	  } else {
		strToChar.Format("%s trips &Yyou&n up.\r\n&RYou fall to the ground!&n\r\n", GetRaceOrName(pTarget).cptr());
		pTarget->SendToChar(strToChar);
		strToChar.Format("You make a nifty move and trip up %s!\r\n", pTarget->GetRaceOrName(this).cptr());
		SendToChar(strToChar);
		strToRoom = "%s makes a nifty move and trips up %s!\r\n";
		GetRoom()->SendToRoom(strToRoom, this, pTarget);
		pTarget->UpDatePosition(POS_SITTING, false);
		pTarget->LagChar(CMudTime::PULSES_PER_REAL_SECOND * DIE(3));
		pTarget->TakeDamage(this, GetSkill(SKILL_TRIP) / 10, true);
		if (pTarget->IsDead()) {
		  //TODO
		  //should be sending SKILL_KILLED!
		  pInterp->KillChar(pTarget);
		} else if (DIE(100) > GetSkill(SKILL_TRIP)) {
		  strToChar.Format("&WYou take %s &Wdown with you!&n\r\n", GetRaceOrName(pTarget).cptr());
		  pTarget->SendToChar(strToChar);
		  strToChar.Format("&W%s &Wtakes &Yyou &Wdown too!&n\r\n", pTarget->GetRaceOrName(this).cptr());
		  SendToChar(strToChar);
		  strToRoom = "&W%s &Wis taken down with %s!&n\r\n";
		  GetRoom()->SendToRoom(strToRoom, this, pTarget);
		  UpDatePosition(POS_SITTING, false);
		  TakeDamage(this, DIE(5), true);
		  if (IsDead()) {
			pInterp->KillChar(this);
		  }
		}
	  }
	} else {
	  SkillUp(SKILL_TRIP);
	  strToChar.Format("%s tries to trip you up, but you are too agile.\r\n", GetRaceOrName(pTarget).cptr());
	  pTarget->SendToChar(strToChar);
	  strToChar.Format("You try to trip %s but &Yyou&n fall down!\r\n", pTarget->GetRaceOrName(this).cptr());
	  SendToChar(strToChar);
	  strToRoom = "%s tries to trip up %s but falls down instead!\r\n";
	  GetRoom()->SendToRoom(strToRoom, this, pTarget);
	  UpDatePosition(POS_SITTING, false);
	  TakeDamage(this, DIE(5), true);
	  if (IsDead()) {
		pInterp->KillChar(this);
	  }
	}
	AddFighters(pTarget);
	LagChar(CMudTime::PULSES_PER_REAL_SECOND * 3);
  }
}

//////////////////////////////
//	CheckAge
//	returns true if character is too old
//	False if not

bool CCharacter::CheckAge() {
  if ((!(CMudTime::GetCurrentPulse() % CHECK_AGE_DEATH))
	  && m_pAttributes->TooOld()
	  && m_pDescriptor != NULL
	  && !this->IsNPC()) {
	AddAffect(AFFECT_AGE_DEATH, sAffect::MANUAL_AFFECT);
	return true;
  }
  return false;
}

///////////////////////////////////////
//	Reset variables in character class for Syncwearaffects
//	virtual so that we can change for classes 
//	ie. paladins get a +1/+1 while wield 2 h

void CCharacter::ResetCharVars() {
  m_nAC = 100 - m_pAttributes->GetACAdjust();
  m_nDamRoll = m_pAttributes->GetDamageBonus();
  m_nToHit = m_pAttributes->GetToHitBonus();
}

///////////////////////////////////////////////
//	Removes the non character affects 
//	then reapplys them all...
//	I just couldn't think of other way to handle this
//	Obj gives +4 str you have 98 str str is capp-ed at 100
//  so we only add 2 to your str. Then you remove the item
//	we remove 4 guess what you now have a 96 str. 
//	I could have just capped what can be returned from GetAttributes()
//	but we want to be able to add +MAX EQ stuff and that won't allow that to happen!
//	So we'll go this route ....better ideas?
///////////////////////////////////////////
//	should only be called by weareq and removeeq functions

void CCharacter::SyncWearAffects() {
  //had to do this because wear affects especially ones like
  //LOC_STR if they pushed wearer over max then removed it all the affect
  //did not get removed!
  int i;
  for (i = 0; i < MAX_EQ_POS; i++) {
	if (!m_Equipment[i].IsEmpty()) {
	  m_Equipment[i].GetObject()->WearAffects(true);
	  m_Equipment[i].GetObject()->AddCharAffects(true, false);
	}
  }
  //removing the character affects is probably being very conservative
  //but we'll do it for now....actually we have to do it b/c ResetCharVars()
  //resets armor class etc so if we don't re-apply the character affects
  //someone that has the spirit armor affect on them will effectively lose
  //it when they wear or remove any piece of EQ
  //the real question is do we need to call ResetCharVars() therefore
  //forcing us to reapply all affects?  for now yes...but I'm not sure
  for (i = 0; i < TOTAL_AFFECTS; i++) {
	if (IsAffectedBy(i) && m_AffectInfo[i].m_pFncApply != NULL) {
	  (this->*m_AffectInfo[i].m_pFncApply)(i, true); //true = remove
	}
  }
  //copy base stats back
  m_pAttributes->Reset();
  this->ResetCharVars();
  //readd affects before we add object affects so we don't have character
  //affects twice
  for (i = 0; i < TOTAL_AFFECTS; i++) {
	if (IsAffectedBy(i) && m_AffectInfo[i].m_pFncApply != NULL) {
	  (this->*m_AffectInfo[i].m_pFncApply)(i, false); //true = remove
	}
  }
  //readd wear affects 
  for (i = 0; i < MAX_EQ_POS; i++) {
	if (!m_Equipment[i].IsEmpty()) {
	  m_Equipment[i].GetObject()->WearAffects(false);
	  m_Equipment[i].GetObject()->AddCharAffects(false, false);
	}
  }
}

///////////////////////////////
//	This is the only way eq should be
//	put into Eq array!

void CCharacter::WearEquipment(CObject * pObj, UINT nWearBit, int nPos, bool bSendMsg) {
  m_Equipment[nPos] = CEqInfo(pObj, nWearBit);
  m_nWeightCarried += pObj->GetWeight();
  if (pObj->IsBeingCarriedby() != this) {
	ErrorLog << GetName() << " is not set as carrier for " << pObj->GetObjName() << " correcting" << endl;
	pObj->Set(this, NULL);
  }
  pObj->WearAffects(false);
  pObj->AddCharAffects(false, bSendMsg);
  SyncWearAffects();
}

////////////////////////////////
//	This is the only way equipment
//	should be removed from m_equipment array

CObject *CCharacter::RemoveEquipment(int nPos, bool bSendMsg) {
  m_nWeightCarried -= m_Equipment[nPos].GetObject()->GetWeight();
  m_Equipment[nPos]->WearAffects(true);
  m_Equipment[nPos]->AddCharAffects(true, bSendMsg);
  CObject *pObj = m_Equipment[nPos].GetObject();
  m_Equipment[nPos].Empty();
  SyncWearAffects();
  return pObj;
}

//////////////////////////////////
//	Function to kill a character or mob
//	Only for gods
//	John Comes  3/9/99
//////////////////////////////////

void CCharacter::DoGodKill(const CInterp *pInterp) {
  CCharacter *pTarget = GetTarget();
  //only imp can delte other gods
  if (pTarget != NULL && (!pTarget->IsGod() || GetLevel() == LVL_IMP)) {
	CString str;
	pInterp->KillChar(pTarget);
	str.Format("You just killed %s.\r\n", pTarget->GetRaceOrName(this).cptr());
	SendToChar(str);
  } else if (pTarget == NULL) {
	SendToChar("Who?\r\n");
  } else {
	SendToChar("You can't gkill another god.\r\n");
  }
}

//////////////////////////////////
//	gets only a certain number of objects from
//	the inventory...used for rogue peeking

void CCharacter::GetInventoryNames(CString & str, int nNumOfItems) {
  POSITION pos = Inventory.GetStartPosition();
  CObject *pObj;
  int nCount = 0;
  while (pos && nCount <= nNumOfItems) {
	pObj = Inventory.GetNext(pos);
	if (CanSee(pObj)) {
	  str.Format("%s%s\r\n", str.cptr(), pObj->GetObjName());
	}
	nCount++;
  }
}

///////////////////////////////
//	Where character can set all
//  togs

void CCharacter::DoTog() {
  static const char *strPlayerHelp =
	  "				Player tog Help\r\n"
	  "	tog <screen | size> #	tog color\r\n"
	  "	tog wimpy #			tog <exit | auto>\r\n"
	  "	tog map inside			tog brief\r\n"
	  "	tog map outside			tog tells\r\n"
	  "	tog compact			tog ooc\r\n"
	  "   tog returns			tog paging\r\n\r\n";

  static const char *strGodHelp =
	  "				God tog help\r\n"
	  "	tog no_hassle			tog switchedinfo\r\n"
	  "	tog god_light			tog sees_all\r\n"
	  "	tog petitions			tog <god_chat | chat>\r\n"
	  "	tog log deaths			tog log zones\r\n"
	  "	tog log connections		tog who\r\n"
	  "	tog invis\r\n";

  static const char *strPlayers =
	  "			Player options:\r\n"
	  "	Type tog help for help menu\r\n"
	  "	Screen &USize&n:	%d		&UColor&n:		%s\r\n"
	  "	&UWimpy&n:		%d		&UAuto&n Exit:	%s\r\n"
	  "	&UMap (Inside)&n:	%s		&UBrief&n:		%s\r\n"
	  "	&UMap (OutSide)&n:	%s		&UTells&n:		%s\r\n"
	  "	&UCompact&n:	%s		&UOOC&n:		%s\r\n"
	  "	Extra &UReturns&n:	%s		&UPaging&n: %s\r\n";

  static const char* strGods =
	  "%s\r\n"
	  "			God options:\r\n"
	  "	No_Hassle:		 %s	    SwitchedInfo:	%s\r\n"
	  "	God_Light:		 %s	    See All:		%s\r\n"
	  "	Petitions:		 %s	    God_Chat:		%s\r\n"
	  "	Log Deaths:		 %s	    Log Zones:		%s\r\n"
	  "	Log Connections:	%s	    who:		%s\r\n"
	  "	God Invis		 %s\r\n";

  static const char *strAnswer[2] = {"off", "on"};
  CString strTog(CurrentCommand.GetWordAfter(1));
  CString strToChar;
  if (strTog.IsEmpty()) {
	strToChar.Format(strPlayers, (int) m_nByScreenSize,
		strAnswer[Prefers(PREFERS_COLOR)],
		(int) m_nWimpy,
		strAnswer[Prefers(PREFERS_AUTO_EXIT)],
		strAnswer[Prefers(PREFERS_MAP_INSIDE)],
		strAnswer[Prefers(PREFERS_BRIEF)],
		strAnswer[Prefers(PREFERS_MAP_OUTSIDE)],
		strAnswer[Prefers(PREFERS_TELLS)],
		strAnswer[Prefers(PREFERS_COMPACT)],
		strAnswer[Prefers(PREFERS_OOC)],
		strAnswer[Prefers(PREFERS_EXTRA_RETURNS)],
		strAnswer[Prefers(PREFRES_NO_PAGING)]);
	if (IsGod()) {
	  strToChar.Format(strGods, strToChar.cptr(),
		  strAnswer[Prefers(PREFERS_NO_HASSLE)],
		  strAnswer[Prefers(PREFERS_TELLS_WHEN_SWITCHED)],
		  strAnswer[Prefers(PREFERS_GOD_LIGHT)],
		  strAnswer[Prefers(PREFERS_GOD_SEES_ALL)],
		  strAnswer[Prefers(PREFERS_PETITION)],
		  strAnswer[Prefers(PREFERS_GOD_CHAT)],
		  strAnswer[Prefers(PREFERS_GOD_LOG_DEATHS)],
		  strAnswer[Prefers(PREFERS_GOD_LOG_ZONES)],
		  strAnswer[Prefers(PREFERS_GOD_CONNECTION_LOG)],
		  strAnswer[Prefers(PREFERS_GOD_ON_WHO)],
		  strAnswer[Prefers(PREFERS_GOD_INVIS)]);
	}
	SendToChar(strToChar);
  } else if (strTog.Compare("help")) {
	strToChar = strPlayerHelp;
	if (IsGod()) {
	  strToChar += strGodHelp;
	}
	SendToChar(strToChar);
  } else {
	if (strTog.Compare("screen") || strTog.Compare("size")) {
	  int nNum = CurrentCommand.GetWordAfter(2).MakeInt();
	  if (nNum != -1 && nNum < 80) //just so they don't set it too hight
	  {
		m_nByScreenSize = nNum;
		strToChar.Format("Your screen size has been set to %d\r\n", nNum);
	  } else {
		strToChar = "Set it how large?...80 is max\r\n";
	  }
	} else if (strTog.Compare("color")) {
	  TogPreference(PREFERS_COLOR);
	  if (Prefers(PREFERS_COLOR)) {
		strToChar = "You now see in &Rcolor&n.\r\n";
	  } else {
		strToChar = "You rush back to the fifties to see black and white.\r\n";
	  }
	} else if (strTog.Compare("wimpy")) {
	  int nNum = CurrentCommand.GetWordAfter(2).MakeInt();
	  if (nNum >= 0 && nNum <= m_nMaxHpts) {
		m_nWimpy = nNum;
		strToChar.Format("You will now flee at %d and less.\r\n",
			(int) m_nWimpy);
	  } else if (nNum > m_nMaxHpts) {
		strToChar = "You can't set it higher than your max hit points fool!\r\n";
	  } else {
		strToChar = "What good will that do you?\r\n";
	  }
	} else if (strTog.Compare("auto") || strTog.Compare("exit")) {
	  TogPreference(PREFERS_AUTO_EXIT);
	  strToChar.Format("You tog auto exit %s\r\n",
		  strAnswer[Prefers(PREFERS_AUTO_EXIT)]);
	} else if (strTog.Compare("brief")) {
	  TogPreference(PREFERS_BRIEF);
	  strToChar.Format("You tog brief %s\r\n",
		  strAnswer[Prefers(PREFERS_BRIEF)]);
	} else if (strTog.Compare("map")) {
	  CString strMap(CurrentCommand.GetWordAfter(2));
	  if (strMap.Compare("inside")) {
		TogPreference(PREFERS_MAP_INSIDE);
		strToChar.Format("Your inside map preference is now %s\r\n",
			strAnswer[Prefers(PREFERS_MAP_INSIDE)]);
	  } else if (strMap.Compare("outside")) {
		TogPreference(PREFERS_MAP_OUTSIDE);
		strToChar.Format("Your outside map preference is now %s\r\n",
			strAnswer[Prefers(PREFERS_MAP_OUTSIDE)]);
	  } else {
		strToChar = "There is only an inside and an outside map ...currently =)\r\n";
	  }
	} else if (strTog.Compare("tell")) {
	  TogPreference(PREFERS_TELLS);
	  strToChar.Format("You turn %s the ablility for others to talk with you.\r\n",
		  strAnswer[Prefers(PREFERS_TELLS)]);
	} else if (strTog.Compare("compact")) {
	  TogPreference(PREFERS_COMPACT);
	  strToChar.Format("You turn %s compact prompt mode.\r\n",
		  strAnswer[Prefers(PREFERS_COMPACT)]);
	} else if (strTog.Compare("ooc")) {
	  TogPreference(PREFERS_OOC);
	  strToChar.Format("You turn ooc %s.\r\n",
		  strAnswer[Prefers(PREFERS_OOC)]);
	} else if (strTog.Compare("returns")) {
	  TogPreference(PREFERS_EXTRA_RETURNS);
	  strToChar.Format("Extra returns will now be turned %s.\r\n",
		  strAnswer[Prefers(PREFERS_EXTRA_RETURNS)]);
	} else if (strTog.Compare("paging")) {
	  TogPreference(PREFRES_NO_PAGING);
	  strToChar.Format("You turn paging %s\r\n",
		  strAnswer[Prefers(PREFRES_NO_PAGING)]);
	}
	//god togs
	if (IsGod()) {
	  if (strTog.Compare("no_hassle")) {
		TogPreference(PREFERS_NO_HASSLE);
		strToChar = (Prefers(PREFERS_NO_HASSLE) ? "You will no longer be hassled.\r\n" : "You will allow mobs to hassle you.\r\n");
	  } else if (strTog.Compare("switchedinfo")) {
		TogPreference(PREFERS_TELLS_WHEN_SWITCHED);
		strToChar.Format("You will %s information set to %s when switched\r\n",
			Prefers(PREFERS_TELLS_WHEN_SWITCHED) ? "get" : "not get", GetName().cptr());
	  } else if (strTog.Compare("god_light")) {
		TogPreference(PREFERS_GOD_LIGHT);
		strToChar = (Prefers(PREFERS_GOD_LIGHT)) ? "You god like light will now light up the room\r\n" : "You turn off your god like light.\r\n";
	  } else if (strTog.Compare("sees_all")) {
		TogPreference(PREFERS_GOD_SEES_ALL);
		strToChar = (Prefers(PREFERS_GOD_SEES_ALL)) ? "You will now see all in the game.\r\n" : "You turn off your god sight.\r\n";
	  } else if (strTog.Compare("petition")) {
		TogPreference(PREFERS_PETITION);
		strToChar = (Prefers(PREFERS_PETITION)) ? "You start to hear the whines of the mortals\r\n" : "You turn off the crys of the mortals.\r\n";
	  } else if (strTog.Compare("god_chat") || strTog.Compare("chat")) {
		TogPreference(PREFERS_GOD_CHAT);
		strToChar = (Prefers(PREFERS_GOD_CHAT)) ? "You will hear the gods chatter.\r\n" : "You turn off the god chatter.\r\n";
	  } else if (strTog.Compare("log")) {
		CString strLog(CurrentCommand.GetWordAfter(2));
		if (strLog.Compare("deaths")) {
		  TogPreference(PREFERS_GOD_LOG_DEATHS);
		  strToChar = (Prefers(PREFERS_GOD_LOG_DEATHS)) ? "You will now see all deaths in the game.\r\n" : "You stop watching the deaths in the game.\r\n";
		} else if (strLog.Compare("zones")) {
		  TogPreference(PREFERS_GOD_LOG_ZONES);
		  strToChar = (Prefers(PREFERS_GOD_LOG_ZONES)) ? "You will now receive zone reset messages.\r\n" : "You stop recieving zone reset messages.\r\n";
		} else if (strLog.Compare("connections")) {
		  TogPreference(PREFERS_GOD_CONNECTION_LOG);
		  strToChar = (Prefers(PREFERS_GOD_CONNECTION_LOG)) ? "You will now receive new connection messages.\r\n" : "You stop recieving new connection messages.\r\n";
		}
	  } else if (strTog.Compare("who")) {
		TogPreference(PREFERS_GOD_ON_WHO);
		strToChar = (Prefers(PREFERS_GOD_ON_WHO)) ? "You will now be seen on who list.\r\n" : "No player will see you on who list.\r\n";
	  } else if (strTog.Compare("invis")) {
		TogPreference(PREFERS_GOD_INVIS);
		strToChar.Format("Your turn god invis %s.\r\n",
			strAnswer[Prefers(PREFERS_GOD_INVIS)]);
	  }
	}
	if (strToChar.IsEmpty()) {
	  SendToChar("Tog what?\r\n");
	} else {
	  SendToChar(strToChar);
	}
  }
}

////////////////////////////////
//	Returns a character to there body
//	if they are switched

void CCharacter::DoReturn() {
  if (IsSwitched()) {
	m_pSwitchedTo->SetSwitched(NULL);
	m_pSwitchedTo = NULL;
	SendToChar("You RETURN to your body!\r\n");
  } else {
	if (!IsGod()) {
	  SendToChar("Huh?\r\n");
	} else {
	  SendToChar("You are not switched to anyone!\r\n");
	}
  }
}

/////////////////////////////////
//	switches you to an NPC
//	so your commands give you what the 
//	NPC sees.

void CCharacter::DoSwitch() {
  CCharacter *pTarget = GetTarget(1); //look in room first

  if (!pTarget) {
	//if we don't find switch target in room
	//look global
	CString strTarget(CurrentCommand.GetWordAfter(1));
	pTarget = GVM.FindCharacter(strTarget, NULL);
  }

  //pass in null so we don't use sight limitations
  if (pTarget) {
	if (pTarget->IsNPC() && !pTarget->IsSwitchedTo()) {
	  CString strToChar;
	  strToChar.Format("You take over the body of %s.\r\n", pTarget->GetName().cptr());
	  SendToChar(strToChar);
	  m_pSwitchedTo = pTarget;
	  pTarget->SetSwitched(this);
	} else if (!pTarget->IsSwitchedTo()) {
	  SendToChar("Someone else is already switched to that NPC.\r\n");
	} else {
	  SendToChar("You can only switch to NPC's.\r\n");
	}
  } else {
	SendToChar("Switch to who?\r\n");
  }
}

///////////////////////////////////////
//	Sends pages to the character instead of
//	all entire string

void CCharacter::SendCharPage(CString str) {
  int nPos = str.Find('\n', m_nByScreenSize);
  if (nPos == -1 || !Prefers(PREFRES_NO_PAGING)) {
	SendToChar(str);
  } else {
	m_pDescriptor->ChangeState(STATE_PAGING);
	m_PageInfo.m_strPage = str;
	m_PageInfo.m_nReturnsSent = m_nByScreenSize;
	CString strToChar;
	strToChar.Format("%s\r\n%s\r\n", str.Left(nPos).cptr(),
		strPageInfo);
	SendToChar(strToChar);
  }
}

///////////////////////////////
//	DoAssist
//	This function used the interperter class
//	to check to insure we have a good target then
//	call this function...I wish I would have done
//	alot more commands with way.
//	written by: Demetrius Comes
//	6-1-99
//this will allow us to call this function directly
//	like from mob AI with out worrying about string errors 
//	b/c mobs don't have unque names

void CCharacter::DoAssist(CCharacter * pTarget) {
  if (pTarget) {
	if (!IsFighting()) {
	  if (pTarget != this && pTarget->IsFighting()) {
		CString strToChar;
		strToChar.Format("You assist %s.\r\n", pTarget->GetRaceOrName(this).cptr());
		SendToChar(strToChar);
		strToChar.Format("%s assists you.\r\n", GetRaceOrName(pTarget).cptr());
		pTarget->SendToChar(strToChar);
		GetRoom()->SendToRoom("%s assists %s.\r\n", this, pTarget);
		AddFighters(pTarget->IsFighting());
	  } else if (!pTarget->IsFighting() && pTarget != this) {
		SendToChar("That person is not fighting!\r\n");
	  } else if (pTarget == this) {
		SendToChar("You can't assist yourself!\r\n");
	  }
	} else {
	  SendToChar("You are already fighting!\r\n");
	}
  } else {
	SendToChar("Assist who?\r\n");
  }
}

////////////////////////////////////////////////
//	this uses a similar set up as DoAssist

void CCharacter::DoKill(const CInterp * pInterp, CCharacter * pTarget) {
  if (!pTarget) {
	SendToChar("Fight who?\r\n");
  } else if (pTarget == this) {
	SendToChar("No killing one's self!\r\n");
  } else if (!IsFighting()) {
	AddFighters(pTarget);
	if ((short) DIE(150) <= pTarget->GetSkill(SKILL_BATTLE_READINESS)) {
	  m_MsgManager.SkillMsg(this, pTarget, SKILL_BATTLE_READINESS, SKILL_WORKED);
	} else {
	  Attack(true);
	}
	if (pTarget->IsDead()) {
	  pInterp->KillChar(pTarget);
	}
  } else if (pTarget != IsFighting()) {
	if ((int) DIE(100) <= GetSkill(SKILL_SWITCH)) {
	  //made the check switch targets
	  AssertFighting(pTarget);
	  SendToChar("You switch targets.\r\n");
	  GetRoom()->SendToRoom("&Y%s&Y switches targets...\r\n&n", this);
	  LagChar((long) ((.5) * CMudTime::PULSES_PER_DO_VIOLENCE));
	  if (!pTarget->IsFighting()) {
		pTarget->AssertFighting(this);
		CGame::AddFighter(pTarget);
	  }
	} else {
	  SendToChar("You become confused.\r\n");
	  AssertFighting(NULL);
	  pInterp->RemoveFighter(this);
	  SkillUp(SKILL_SWITCH);
	  LagChar((long) ((.75) * CMudTime::PULSES_PER_DO_VIOLENCE));
	  return;
	}
  } else {
	SendToChar("Switch to who?\r\n");
  }
}

//////////////////////////////////////////
//  Can this teach pCharToBeTaught skill number nSKill
//	return turn 0 if can't teach the skill

skill_type CCharacter::CanTeach(const CCharacter * pCharToBeTaught, short nSkillToLearn, bool bIsSkill) const {
  skill_type bRetVal = 0;
  CString strToChar;
  if (m_Consent.GetConsented() == pCharToBeTaught) {
	//you can't learn from someone you can't see there
	//name -- eg you can't be a drow elf and learn from a pixie
	if (pCharToBeTaught->CanSeeName(this) && pCharToBeTaught->CanSeeChar(this)) {
	  if (IsGoodEnoughToTeach(pCharToBeTaught, nSkillToLearn, bIsSkill)) {
		bRetVal = ((GetSkill(nSkillToLearn) - pCharToBeTaught->GetSkill(nSkillToLearn)) / PC_TO_PC_TEACHER_PENATLY);
	  } else {
		strToChar.Format("%s doesn't know that well enought to teach you.\r\n",
			this->GetRaceOrName(pCharToBeTaught).cptr());
		pCharToBeTaught->SendToChar(strToChar);
	  }
	} else {
	  pCharToBeTaught->SendToChar("You have to be able to see your teacher, and thier friend!\r\n");
	}
  } else {
	strToChar.Format("You will need %s's consent first!\r\n",
		this->GetRaceOrName(pCharToBeTaught).cptr());
	pCharToBeTaught->SendToChar(strToChar);
  }
  return bRetVal;
}

///////////////////////////////////////////////////
//	Gets the cost of training if player can afford it
//	we take the cash away and charge them
//	other wise just return false;
//	written by: Demetrius Comes
//	6-17-99

bool CCharacter::CanAffordTraining(sMoney & Cost, short nSkill, const CCharacter * pTeacher, bool bIsSkill) {
  Cost = pTeacher->GetTrainingCosts(this, nSkill, bIsSkill);
  if (m_CashOnHand.Take(Cost)) {
	return true;
  }
  return false;
}

////////////////////////////////////////////
//	Sends Prices to character for practicing

void CCharacter::BuildPracticePriceSkills(const CCharacter * pToBeTaught, CString &str) const {
  sMoney Cost;
  short i;
  str.Format("%sI will charge you the following:\r\n", str.cptr());
  for (i = 0; i < MAX_MUD_SKILLS; i++) {
	if (this->IsGoodEnoughToTeach(pToBeTaught, i, true)) {
	  Cost = GetTrainingCosts(pToBeTaught, i, true);
	  str.Format("%s%-30s\t%s\r\n",str.cptr(),FindSkill(i).cptr(),
		  Cost.GetMoneyString().cptr());
	}
  }
}

//////////////////////////////////////////
//	Allows a character to ask a player or NPC a question

void CCharacter::DoAsk(const CCharacter * pTarget) {
  if (pTarget == this) {
	SendToChar("I don't know how much do you cost?\r\n");
  } else if (pTarget == NULL) {
	SendToChar("Learn from who?\r\n");
  } else if (!pTarget->IsNPC()) {
	CString strAsk;
	strAsk.Format("You ask %s '%s'\r\n",
		pTarget->GetRaceOrName(this).cptr(),
		CurrentCommand.GetWordAfter(2, true).cptr());
	SendToChar(strAsk);
	strAsk.Format("%s asks you '%s'\r\n",
		GetRaceOrName(pTarget).cptr(),
		CurrentCommand.GetWordAfter(2, true).cptr());
	pTarget->SendToChar(strAsk);
	GetRoom()->SendToRoom("%s asks %s a question.\r\n", this, pTarget);
  } else if (pTarget->Prefers(CNPC::TEACHER) && pTarget->IsNPC()) {
	CString strPrice(CurrentCommand.GetWordAfter(2));
	CString strToChar;
	if (strPrice.Compare("price")) {
	  CString strSkillOrSpell(CurrentCommand.GetWordAfter(3));
	  if (strSkillOrSpell.Compare("skill")) //skill
	  {
		pTarget->BuildPracticePriceSkills(this, strToChar);
	  } else if (strSkillOrSpell.Compare("spell")) //spell
	  {
		pTarget->BuildPracticePriceSpells(this, strToChar);
	  } else {
		strToChar = "syntax: ask <person> question or ask <person> price <skill|spell>.\r\n";
	  }
	} else {
	  strToChar = "syntax: ask <person> question or ask <person> price <skill|spell>.\r\n";
	}
	SendCharPage(strToChar);
  } else {
	SendToChar("I don't teach scum like you!\r\n");
  }
}

///////////////////////////////////////////////
//	Virtual:
//	For non casters

void CCharacter::BuildPracticePriceSpells(const CCharacter * pToBeTaught, CString & str) const {
  str.Format("%sDo I look like a caster?\r\n", str.cptr());
}

/////////////////////////////
//	Randomly generate a number from 0 to NUM_OF_DOORS
//	if number == NUM_OF_DOORS they fail the flee
//	if not then if there is a door in that direction 
//	they will flee

void CCharacter::DoFlee() {
  if (IsAffectedBy(AFFECT_CASTING)) {
	RemoveAffect(AFFECT_CASTING);
	SendToChar("In your panic you stop casting\r\n");
	GetRoom()->SendToRoom("In %s's panic, %s stops casting", this,
		CCharacterAttributes::GetPronoun(this->GetSex()));
  } else {

	//-1 because DIE return 1-NUMBER not 0 through NUMBER
	short nRoll = DIE(NUM_OF_DOORS) - 1;

	if (nRoll >= NUM_OF_DOORS || !GetRoom()->IsExit(nRoll)) {
	  SendToChar(CString("&wPANIC!!! You can't get out.&n\r\n"));
	  GetRoom()->SendToRoom("%s tries to flee but can't.\r\n", this);
	} else if (!InPosition(POS_STANDING)) {
	  SendToChar("You scramble to your feet.\r\n");
	  GetRoom()->SendToRoom("%s scrambles to their feet.\r\n", this);
	  UpDatePosition(POS_STANDING, false);
	} else {
	  CString strReverse, strDirection, strToChar;
	  strToChar.Format("You attempt to flee and flee %s\r\n", CRoom::strDir[nRoll]);
	  SendToChar(strToChar);
	  GetRoom()->SendToRoom("%s panics and attempts to flee.\r\n", this);
	  AssertFighting(NULL);
	  DoMove(CRoom::strDir[nRoll]);
	}
  }
}

///////////////////////////////////
//	Allow one character to rescue another
//	Do not allow a switch though!

void CCharacter::DoRescue(CCharacter * pTarget) {
  if (!GetSkill(SKILL_RESCUE)) {
	SendToChar("Try learning the skill first.\r\n");
  } else if (!pTarget) {
	SendToChar("Rescue who?\r\n");
  } else if (!pTarget->IsFighting()) {
	SendToChar("But no one is fighting them.\r\n");
  }//can only rescue a tank
  else if (pTarget->IsFighting()->IsFighting() != pTarget) {
	SendToChar("Rescue them from what?\r\n");
  } else {
	if (DIE(100) <= GetSkill(SKILL_RESCUE)) {
	  CCharacter *pAttacker = pTarget->IsFighting();
	  m_MsgManager.SkillMsg(this, pTarget, SKILL_RESCUE, SKILL_WORKED);
	  pAttacker->AssertFighting(this);
	  pTarget->AssertFighting(NULL);
	  CGame::AddFighter(this);
	} else {
	  m_MsgManager.SkillMsg(this, pTarget, SKILL_RESCUE, SKILL_DIDNT_WORK);
	}
	LagChar(CMudTime::PULSES_PER_BATTLE_ROUND * 1);
  }
}

/////////////////////////////////////
//	checks to see if I can body slam target

bool CCharacter::CanBodySlam(CCharacter * pTarget) {
  if (pTarget->GetSize() - GetSize() > 2 || pTarget->GetSize() - GetSize() < -2) {
	return true;
  }
  return false;
}

/////////////////////////////////////////////
//	AdjustAlignment
//	The alignment difference / 100 is applied to your align	
//	If your alignment difference /100 is 0 then you have a 50% chance of it moving 1 point
//  John Comes :  11-3-99
//////////////////////////////////////////

void CCharacter::AdjustAlignment(const CCharacter * pCh) {
  int nDiff = ((GetAlignment() - pCh->GetAlignment()) / 100);
  if (nDiff == 0) {
	if (DIE(100) < 50) {
	  if (GetAlignment() < 0) {
		nDiff++;
	  } else {
		nDiff--;
	  }
	}
  }
  m_nAlignment += nDiff;
  m_nAlignment = m_nAlignment > 1000 ? 1000 : m_nAlignment<-1000 ? -1000 : m_nAlignment;
}


//////////////////////////////////
//	given a room
//	we pass back how well (this) character can see
//	can return on of three types of vision
//	SEES_ALL, SEES_NOTHING, SEES_RED_DOTS
//
//	Vision works like the following chart:
//	
//	vision type    |     LIGHT LEVEL  | Can See
//	ULTRAVISION    |      DARK        | YES
//	INTRAVISION    |      Dark        | RED dots
//	day sight      |      dark        | no
//	ultravision    |      lit         | % chance to see characters in the room and room name
//	intravision    |      lit         | red dots
//	day sight      |      lit         | % chance to see characters in the room and room name
//	ultravision    |      bright      | no
//	infravision    |      bright      | no (it's all hot)
//	day sight      |      bright      | yes
//  day and ultravision | lit         | yes
//
////////////////////////////////////////////////

short CCharacter::CanSeeInRoom(const CRoom *pRoom) const {
  if (InPosition(POS_SLEEPING)) {
	return SEES_NOTHING;
  } else if (IsAffectedBy(AFFECT_BLINDNESS)) {
	return SEES_NOTHING;
  }//if we are a god that has PREFERS_GOD_LIGHT
	//we can see everyone in all conditions
  else if (Prefers(PREFERS_GOD_LIGHT)) {
	return SEES_ALL;
  } else {
	//now check for room light level
	int nCheckLight;
	//is the room bright
	int nRoomBrightness = pRoom->GetRoomBrightness();
	if (nRoomBrightness == CRoom::ROOM_TWILIGHT) {
	  nCheckLight = SEES_ALL;
	} else if (nRoomBrightness == CRoom::ROOM_BRIGHT) {
	  //if they can see in the light 
	  if (HasInnate(INNATE_DAY_SIGHT)) {
		nCheckLight = SEES_ALL;
	  } else {
		//other wise nothing
		nCheckLight = SEES_NOTHING;
	  }
	} else if (nRoomBrightness == CRoom::ROOM_LIT) {
	  //if we have both we can see
	  if (HasInnate(INNATE_ULTRAVISION) &&
		  HasInnate(INNATE_DAY_SIGHT)) {
		nCheckLight = SEES_ALL;
	  }//we if just one (ULTRA or DAYSIGHT) a chance to see
	  else if (HasInnate(INNATE_ULTRAVISION) ||
		  HasInnate(INNATE_DAY_SIGHT)) {
		//give them a chance to see
		if (DIE(100) <= CHANCE_TO_SEE) {
		  nCheckLight = SEES_ALL;
		}//if they fail the chance and they have infra they can
		  //at least see dots
		else if (HasInnate(INNATE_INFRAVISION)) {
		  nCheckLight = SEES_RED_DOTS;
		} else {
		  nCheckLight = SEES_NOTHING;
		}
	  }//do we just have infra ...no race only has infra through...
		//and it should stay like THAT
	  else if (HasInnate(INNATE_INFRAVISION)) {
		nCheckLight = SEES_RED_DOTS;
	  } else {
		nCheckLight = SEES_NOTHING;
	  }
	} else //room is dark
	{
	  //ultra vision can see
	  if (HasInnate(INNATE_ULTRAVISION)) {
		nCheckLight = SEES_ALL;
	  }//infra vision see red dots
	  else if (HasInnate(INNATE_INFRAVISION)) {
		nCheckLight = SEES_RED_DOTS;
	  }//day sight can't see
	  else {
		nCheckLight = SEES_NOTHING;
	  }
	}
	return nCheckLight;
  }
}

/////////////////////////////
//	can the character pointer by
//	by (this) see the character pointed
//	to by pCh

bool CCharacter::CanSeeChar(const CCharacter *pCh) const {
  bool bRetVal = true;
  if (pCh != NULL) {
	if (this->InPosition(POS_SLEEPING)) {
	  bRetVal = false;
	} else if (this->IsGod() && this->Prefers(PREFERS_GOD_SEES_ALL)) {
	  bRetVal = true;
	}//illthids can't see undead
	else if (GetRace() == RACE_ILLITHID && pCh->GetRace() == RACE_UNDEAD) {
	  bRetVal = false;
	} else if (pCh == this && !IsAffectedBy(AFFECT_BLINDNESS)) {
	  bRetVal = true;
	} else {
	  if (pCh->IsGod() && pCh->Prefers(PREFERS_GOD_INVIS)) {
		bRetVal = false;
	  } else if (pCh->IsAffectedBy(AFFECT_HIDING)) {
		bRetVal = false;
	  } else if (pCh->IsAffectedBy(AFFECT_INVISIBLE)
		  && !this->IsAffectedBy(AFFECT_DETECT_INVISIBILITY)) {
		bRetVal = false;
	  } else if (this->IsAffectedBy(AFFECT_BLINDNESS)) {
		bRetVal = false;
	  }
	}
  } else {
	bRetVal = false;
  }
  return bRetVal;
}

/////////////////////////////
//	Opens a door...container...etc

void CCharacter::DoOpen(bool bOpen) {
  CString str(CurrentCommand.GetWordAfter(1));
  CString strDir(CurrentCommand.GetWordAfter(2));
  short nDir;

  if (!strDir.IsEmpty()
	  && (nDir = GetRoom()->IsKeyWordForDoor(str, strDir)) != NOWHERE) {
	GetRoom()->OpenDoor(nDir, this, bOpen);
  } else {
	bool bDummy;
	CObject *pObj = this->GetTarget(1, true, bDummy);
	if (pObj != NULL) {
	  pObj->Open(this, bOpen);
	} else {
	  bOpen ? SendToChar("Open what?\r\n") : SendToChar("Close what?\r\n");
	}
  }
}

//////////////////////////
//	Get Load or weight wording for attributes cmd
//	

const char * CCharacter::GetLoadWording() {
  short nLoadPct = GetLoadPct();
  const char *str;
  if (nLoadPct >= LOAD_IMMOBILIZING)
	str = "immobilizing";
  else if (nLoadPct >= LOAD_VERY_HEAVY)
	str = "very heavy";
  else if (nLoadPct >= LOAD_HEAVY)
	str = "heavy";
  else if (nLoadPct >= LOAD_MODERATE)
	str = "moderate";
  else if (nLoadPct >= LOAD_LIGHT)
	str = "light";
  else
	str = "what load?";

  return str;
}

///////////////////////////////
//	SetSpell level
//	default handle

bool CCharacter::SetSpell(const CCharacter *pCh, CString strSpell, skill_type nValue) {
  CString strToChar;
  strToChar.Format("%s don't know how to cast spells.\r\n", GetName().cptr());
  pCh->SendToChar(strToChar);
  return false;
}

//////////////////////////////////
//	Remove the character from a transport room

void CCharacter::DoDisembark() {
  if (GetRoom()->IsTransportRoom()) {
	CRoom *pRoom = ((CTransportRoom *) GetRoom())->GetTransport()->GetRoom();
	CTransport *pTrans = ((CTransportRoom *) GetRoom())->GetTransport();
	if (!pRoom->IsWaterRoom()) {
	  GetRoom()->SendToRoom("%s leaves the %s.\r\n", this, pTrans);
	  GetRoom()->Remove(this);
	  this->m_pInRoom = pRoom;
	  GetRoom()->Add(this);
	  GetRoom()->SendToRoom("%s steps off the %s.\r\n",
		  this, pTrans->GetObjName());
	  AddCommand("look");
	} else {
	  SendToChar("That would be a long swim.\r\n");
	}
  } else {
	SendToChar("You only disembark from transports.\r\n");
  }
}

////////////////////////////
//	Sends the news to the character
//	

void CCharacter::SendNews() {
  SendCharPage(NEWS);
}

/////////////////////////////
//	Allows a character to start the process of
//	Base implementation 
//  This function only gets the track started
//	we will add an affect and have the affect functions
//	do the actual tracking etc
//  UPDATE:
//	WE can let affect functions to the tracking...why?
//	because the FadeAffects function removes the affect
//	the affect is not time based it is ablity based so
//	we should NOT use a timed affect...
//  UPDATE: //again!
//	we will set the character's track up so that it uses
//	pointers even though this will mean that when 
//	characters log NPC will no longer be tracking them.
//	we can just override the Track() to take care of it
//	me thinks

void CCharacter::DoTrack() {
  CString strTarget(CurrentCommand.GetWordAfter(1));
  CString strToChar;
  if (GetSkill(SKILL_TRACK) <= 0) {
	strToChar = "Learn how to first!\r\n";
  } else if (strTarget.Compare("off")) {
	strToChar = "You stop tracking.\r\n";
	m_pTrackInfo->RemoveAllTrackTargets(this);
  } else if (!m_pTrackInfo->m_TrackList.IsEmpty()
	  && !strTarget.IsEmpty()) {
	//PC's can only track one character at a time!
	//but I used a link list so NPC's can track more
	//and maybe we'll let PC's track more than one later
	//this way the code is already set up for it
	strToChar = "You are already tracking someone else.\r\n";
  } else {
	//use NULL so we don't use vision constraints
	CCharacter *pTarget = GVM.FindCharacter(strTarget, NULL);
	if (pTarget == NULL && strTarget.IsEmpty()) {
	  strToChar = "You are currently tracking ";
	  CString str(m_pTrackInfo->GetTrackList(this));
	  if (str.IsEmpty()) {
		strToChar += "no-one.\r\n";
	  } else {
		strToChar += str;
	  }
	} else if (pTarget == NULL) {
	  strToChar = "Track who?\r\n";
	} else if (pTarget == this) {
	  strToChar = "You track yourself down...woot!\r\n";
	} else if (pTarget->GetRoom()->GetZone() != GetRoom()->GetZone()) {
	  strToChar.Format("You can't find a trace of %s.\r\n",
		  strTarget.cptr());
	} else {
	  if (m_pTrackInfo->IsTracking(pTarget)) {
		m_pTrackInfo->RemoveTrackTarget(pTarget, this);
		//if they are tracking give them their name
		//always
		strToChar.Format("You stop tracking %s\r\n",
			pTarget->GetName().cptr());
	  } else {
		m_pTrackInfo->AddTrackTarget(pTarget, this);
		strToChar.Format("You start looking for %s trail.\r\n",
			pTarget->GetName().cptr());
	  }
	}
  }
  SendToChar(strToChar);
}

///////////////////////////
//	Allows a group member to split coins with 
//	thier group

void CCharacter::DoSplit() {
  if (!this->m_Group.IsGrouped()) {
	SendToChar("You have to be grouped to split.\r\n");
  } else {
	CString strAmount(CurrentCommand.GetWordAfter(1));
	int nAmount = strAmount.MakeInt();
	CString strCash(CurrentCommand.GetWordAfter(2));
	if (sMoney::IsCash(strCash)
		&& (strAmount.Compare("coins")
		|| nAmount != -1)) {
	  sMoney MoneyToSplit;
	  try {
		if (strAmount.Compare("coins")) {
		  strCash = "coins";
		}
		m_CashOnHand.FromString(strCash, MoneyToSplit, nAmount);
	  } catch (CError *pErr) {
		SendToChar(CString(pErr->GetMessage()));
	  }//if we catch a string everything worked cool
	  catch (char *str) {
		//str; //just to stop a warning
		m_Group.Split(this, MoneyToSplit);
	  }
	} else {
	  SendToChar("syntax: split coins\r\nsplit <amount> <plat|gold|silver|copper>\r\n");
	}
  }
}

////////////////////////////////////////
//	Here we do the actual tracking 
//	Here we will look for people to track
//	and move the character in the direction
//	needed

void CCharacter::Track() {
  if (!InPosition(POS_FIGHTING)
	  && InPosition(POS_STANDING)
	  && !m_pTrackInfo->m_TrackList.IsEmpty()) {
	int nTrackPulse;
	//the better their skill the faster they track
	if (GetSkill(SKILL_TRACK) <= 25)
	  nTrackPulse = CMudTime::PULSES_PER_MUD_HOUR / 15;
	else if (GetSkill(SKILL_TRACK) <= 50)
	  nTrackPulse = CMudTime::PULSES_PER_MUD_HOUR / 20;
	else if (GetSkill(SKILL_TRACK) <= 75)
	  nTrackPulse = CMudTime::PULSES_PER_MUD_HOUR / 30;
	else
	  nTrackPulse = CMudTime::PULSES_PER_MUD_HOUR / 60;
	//Is it time?
	if (!(CMudTime::GetCurrentPulse() % nTrackPulse)) {
	  CList<CCharacter *>TargetsToRemove;
	  POSITION pos = m_pTrackInfo->m_TrackList.GetStartPosition();
	  CCharacter *pCh;
	  CString strToChar;
	  while (pos) {
		pCh = m_pTrackInfo->m_TrackList.GetNext(pos);
		if (GetRoom()->DoesContain(pCh)) {
		  strToChar.Format("%s is in this room!\r\n",
			  pCh->GetRaceOrName(this).cptr());
		  SendToChar(strToChar);
		  TargetsToRemove.Add(pCh);
		} else if (DIE(100) <= GetSkill(SKILL_TRACK)) {
		  short nDir;
		  CLinkList<CRoom> UsedLL;
		  if ((nDir = GetRoom()->LocateChar(pCh, UsedLL, GetSkill(SKILL_TRACK) >> 1)) != NOWHERE) {
			CRoom *pToRoom = GetRoom()->GetRoomToThe(nDir);
			assert(pToRoom);
			//should we do this for pc's or just npcs?
			if (!GetRoom()->IsDoorOpen(nDir)) {
			  GetRoom()->OpenDoor(nDir, this, true);
			}
			DoMove(CRoom::strDir[nDir]);
			//stop the hunt
			pos = NULL;
			//Are we in the room our target is in
			if (GetRoom()->DoesContain(pCh)) {
			  TargetsToRemove.Add(pCh);
			  strToChar.Format("Your profound skill at track as lead you to %s.\r\n",
				  pCh->GetName().cptr());
			  SendToChar(strToChar);
			}
		  }
		} else {
		  SkillUp(SKILL_TRACK);
		  strToChar.Format("You've lost %s's trail.\r\n",
			  pCh->GetRaceOrName(this).cptr());
		  SendToChar(strToChar);
		  TargetsToRemove.Add(pCh);
		}
		//remove the targets we tryed to track
		pos = TargetsToRemove.GetStartPosition();
		while (pos) {
		  m_pTrackInfo->RemoveTrackTarget(TargetsToRemove.GetNext(pos), this);
		}
	  }
	}
  }
}

////////////////////////////////////
//	Allows players to exchange silver for gold
//	etc
//  TODO

void CCharacter::DoExchange() {
  if (!GetRoom()->IsType(CRoom::TYPE_BANK)) {
	return;
  }
  int nAmount, nExchanged;
  try {
	if ((nAmount = CurrentCommand.GetWordAfter(1).MakeInt()) != -1) {
	  bool bFound = true;
	  CString str1(CurrentCommand.GetWordAfter(2));
	  CString str2(CurrentCommand.GetWordAfter(3));
	  if (sMoney::IsCash(str1)
		  && sMoney::IsCash(str2)
		  && !str1.Compare("coins")
		  && !str2.Compare("coins")) {
		int nLeft;
		switch (str1[0]) {
		  case 'c': case 'C':
			nAmount = m_CashOnHand.RemoveCopper(nAmount);
			str1 = sMoney::m_strCopper;
			switch (str2[0]) {
			  case 's': case 'S':
				nLeft = nAmount % sMoney::SILVER_IN_BASE;
				nAmount = nAmount / sMoney::SILVER_IN_BASE;
				str2 = sMoney::m_strSilver;
				nExchanged = nAmount - ((nAmount * ((CBankRoom *) GetRoom())->GetCToS()) / 100);
				m_CashOnHand.AddSilver(nExchanged);
				break;
			  case 'g': case 'G':
				nLeft = nAmount % sMoney::GOLD_IN_BASE;
				nAmount = nAmount / sMoney::GOLD_IN_BASE;
				str2 = sMoney::m_strGold;
				nExchanged = nAmount - ((nAmount * ((CBankRoom *) GetRoom())->GetCToG()) / 100);
				m_CashOnHand.AddGold(nExchanged);
				break;
			  case 'p': case 'P':
				nLeft = nAmount % sMoney::PLAT_IN_BASE;
				nAmount = nAmount / sMoney::PLAT_IN_BASE;
				str2 = sMoney::m_strPlat;
				nExchanged = nAmount - ((nAmount * ((CBankRoom *) GetRoom())->GetGToP()) / 100);
				m_CashOnHand.AddPlat(nExchanged);
				break;
			  default:
				bFound = false;
				break;
			}
			m_CashOnHand.AddCopper(nLeft);
			break;
		  case 's': case 'S':
			nAmount = m_CashOnHand.RemoveSilver(nAmount);
			str1 = sMoney::m_strSilver;
			switch (str2[0]) {
			  case 'g':case 'G':
				nLeft = nAmount % (sMoney::GOLD_IN_BASE / sMoney::SILVER_IN_BASE);
				nAmount = nAmount / (sMoney::GOLD_IN_BASE / sMoney::SILVER_IN_BASE);
				str2 = sMoney::m_strGold;
				nExchanged = nAmount - ((nAmount * ((CBankRoom *) GetRoom())->GetSToG()) / 100);
				m_CashOnHand.AddGold(nExchanged);
				break;
			  case 'p': case 'P':
				nLeft = nAmount % (sMoney::PLAT_IN_BASE / sMoney::SILVER_IN_BASE);
				nAmount = nAmount / (sMoney::PLAT_IN_BASE / sMoney::SILVER_IN_BASE);
				str2 = sMoney::m_strPlat;
				nExchanged = nAmount - ((nAmount * ((CBankRoom *) GetRoom())->GetSToP()) / 100);
				m_CashOnHand.AddPlat(nExchanged);
				break;
			  default:
				bFound = false;
				break;
			}
			m_CashOnHand.AddSilver(nLeft);
			break;
		  case 'g': case 'G':
			nAmount = m_CashOnHand.RemoveGold(nAmount);
			str1 = sMoney::m_strGold;
			switch (str2[0]) {
			  case 'p': case 'P':
				nLeft = nAmount % (sMoney::PLAT_IN_BASE / sMoney::GOLD_IN_BASE);
				nAmount = nAmount / (sMoney::PLAT_IN_BASE / sMoney::GOLD_IN_BASE);
				str2 = sMoney::m_strPlat;
				nExchanged = nAmount - ((nAmount * ((CBankRoom *) GetRoom())->GetGToP()) / 100);
				m_CashOnHand.AddPlat(nExchanged);
				break;
			  default:
				bFound = false;
				break;
			}
			m_CashOnHand.AddGold(nLeft);
			break;
		  default:
			bFound = false;
			break;
		}
	  }
	  if (bFound) {
		CString strToChar;
		strToChar.Format("You exchange %d %s for %d %s.\r\n",
			nAmount, str1.cptr(), nExchanged, str2.cptr());
		SendToChar(strToChar);
		GetRoom()->SendToRoom("%s exchanges some cash.\r\n", this);
	  }
	}
	SendToChar("syntax: exchange <amount> <from> <to>\r\n");
  } catch (CError *e) {
	SendToChar(e->GetMessage());
  }
}

//////////////////////////////////
//	Allows player to deposit or withdraw cash
//	from the bank

void CCharacter::GetMoneyFromBank(bool bWithdraw) {
  if (!GetRoom()->IsType(CRoom::TYPE_BANK)) {
	SendToChar("Trying going to a bank!\r\n");
	return;
  }
  if (m_pBankCash != NULL) {
	CString strMoney(CurrentCommand.GetWordAfter(1));
	bool bAll = false;
	int nDot, nNum = 1;
	if ((nDot = strMoney.Find('.')) != -1) {
	  nNum = strMoney.Left(nDot).MakeInt();
	  if (nNum == -1 && strMoney.Left(nDot).Compare("all")) {
		bAll = true;
	  }
	  strMoney = strMoney.Right(nDot);
	}
	if (sMoney::IsCash(strMoney)) {
	  try {
		if (bWithdraw) {
		  //TODO this is a bug weight is not being updated!
		  m_pBankCash->FromString(strMoney, m_CashOnHand, nNum);
		} else {
		  m_CashOnHand.FromString(strMoney, (*m_pBankCash), nNum);
		}
	  } catch (CError *pErr) {
		SendToChar(CString(pErr->GetMessage()));
	  } catch (char *strMoney) {
		CString strToChar;
		strToChar.Format("You %s %s.\r\n", (bWithdraw ? "withdraw" : "deposit"), strMoney);
		SendToChar(strToChar);
	  }
	} else {
	  SendToChar("syntax: <withdraw | deposit> <amount>.<coin type> or just coins\r\n");
	}
  } else {
	ErrorLog << GetName() << " is trying to deposit or withdraw cash but dosn't have a bank!" << endl;
  }
}

//////////////////////////////
//	Actually calls the write function

void CCharacter::SendToSocket(const CString &str) const {
  sPlayerDescriptor *pDesc = (sPlayerDescriptor *) m_pDescriptor;
  if (send(pDesc->m_ConnectedSocket, str.cptr(), str.GetLength(), 0) < 0) {
#ifdef _WOLFSHADE_WINDOWS_
	if (WSAGetLastError() != WSAEWOULDBLOCK)
#else
#ifdef EWOULDBLOCK
	if (errno == EWOULDBLOCK)
	  errno = EAGAIN;
#endif /* EWOULDBLOCK */
	if (errno != EAGAIN)
#endif //woflshade_windows
	{
	  ErrorLog << "Socket " << pDesc->m_ConnectedSocket << " bad write for player " << GetName() << endl;
	  pDesc->ChangeState(STATE_LINK_DEAD);
	}
  }
}

////////////////////////////////////
//	Sends Info to OOC

void CCharacter::DoOOC() {
  if (!Prefers(PREFERS_OOC)) {
	SendToChar("You can only talk on OOC if you can hear ooc. Tog it on first!\r\n");
  } else {
	CString strMsg(CurrentCommand.GetWordAfter(1, true));
	if (strMsg.IsEmpty()) {
	  SendToChar("What do you want to say over OOC?\r\n");
	} else {
	  if (!IsGod()) {
		strMsg.Colorize(false);
	  }
	  CString str;
	  str.Format("&gYou annouce over ooc: &G%s&n\r\n", strMsg.cptr());
	  SendToChar(str);
	  str.Format("&g%s annouces over ooc: &G%s&n\r\n", GetName().cptr(),
		  strMsg.cptr());
	  GVM.SendToOOC(str, this);
	}
  }
}

/////////////////////////////////////
//	Allows a player to drag objects

void CCharacter::DoDrag() {
  bool bAll;
  CObject *pObj = GetTarget(1, false, bAll);
  if (!pObj) {
	SendToChar("Drag what?\r\n");
  } else {
	CString strToWhere(CurrentCommand.GetWordAfter(2));
	DoMove(strToWhere, pObj);
  }
}

////////////////////////////
//	Is the (this) good enought to teach pToBeTaught

bool CCharacter::IsGoodEnoughToTeach(const CCharacter *pToBeTaught, short nSkill, bool bSkill) const {
  if (bSkill) {
	if (GetSkill(nSkill)>(pToBeTaught->GetSkill(nSkill) + MIN_SKILL_ABILITY_TO_TEACH)) {
	  return true;
	} else {
	  return false;
	}
  } else {
	if (m_Spells[nSkill]>(pToBeTaught->m_Spells[nSkill] + MIN_SPELL_ABILITY_TO_TEACH)) {
	  return true;
	} else {
	  return false;
	}
  }
}

///////////////////////////////
//	Allows a character to bandage themselves
//

void CCharacter::DoFirstAid() {
  if (IsAffectedBy(AFFECT_NO_BANDAGE_TIME)) {
	SendToChar("Your wounds have been bandaged already.\r\n");
  } else {
	m_nCurrentHpts += DIE(6) + (GetSkill(SKILL_FIRST_AID) / 10);
	m_nCurrentHpts = m_nCurrentHpts > m_nMaxHpts ? m_nMaxHpts : m_nCurrentHpts;
	AddAffect(AFFECT_NO_BANDAGE_TIME, CMudTime::PULSES_PER_MUD_DAY / ((GetSkill(SKILL_FIRST_AID) + 10) / 10));
	SkillUp(AFFECT_NO_BANDAGE_TIME);
	SendToChar("You bandage your wounds.\r\n");
	GetRoom()->SendToRoom("%s bandage their wounds.\r\n", this);
  }
}

////////////////////////////////
//	Checks to see if char should auto flee then does it
//	flee in this mud works as follows...auto flee that is
//	you can only auto flee from meele combat the rest you
//	must flee

void CCharacter::DoAutoFlee() {
  //CanFight used so we don't have in_cap mobs fleeing
  if ((m_nCurrentHpts <= m_nWimpy)
	  && CanFight() && IsFighting()
	  && !HasMaster()) {
	DoFlee();
  }
}

//////////////////////////////
//	Allows a player to set thier hometown

void CCharacter::DoHomeTown() {
  if (GetRoom()->IsAffectedBy(CRoom::ROOM_AFFECT_INN)) {
	m_lPlayerSetHomeTown = GetRoom()->GetVnum();
	SendToChar("Your home has been set.\r\n");
  } else {
	SendToChar("You can only set you hometown from an inn.\r\n");
  }
}

void CCharacter::ThinkerDefense(int nDam, CCharacter *pCaster) {
  if (MakeSavingThrow(CAttributes::SAVE_SPELL)) {
	nDam >>= 1;
  }
  if (nDam > 0) {
	AddMemory(pCaster);
  }
  TakeDamage(pCaster, nDam, false);
}

///////////////////////////////////
//allows gods to change attributes

void CCharacter::GodChange() {
  CString strTarget(CurrentCommand.GetWordAfter(1));
  const char *s = "syntax: gchange <player name> <attribute | namne | hitpoints | mana | moves | frags | alignment> <value>\r\n"
	  "syntax: gchange <player name> <male | female | none>\r\n"
	  "syntax: gchange <player name> <race | class> <race name | class name>\r\n";

  CCharacter *pTarget = GVM.FindCharacter(strTarget, this);
  if (!pTarget) {
	SendToChar(s);
  } else {
	CString strAtt(CurrentCommand.GetWordAfter(2));
	CString strValue(CurrentCommand.GetWordAfter(3, true));
	CString strToChar;
	int nValue;
	if (strAtt.Compare("frags")) {
	  float fbefore = pTarget->m_fFrags;
	  pTarget->m_fFrags = atof(strValue.cptr());
	  strToChar.Format("%s has changed your frag total to %.2f.\r\n",
		  GetName().cptr(), pTarget->m_fFrags);
	  pTarget->SendToChar(strToChar);
	  strToChar.Format("You change %s's frag total from %.2f to %.2f.\r\n",
		  pTarget->GetName().cptr(), fbefore, pTarget->m_fFrags);
	  SendToChar(strToChar);
	  pTarget->Save();
	} else if (strAtt.Compare("alignment")) {
	  short nAlign = pTarget->m_nAlignment;
	  pTarget->m_nAlignment = atoi(strValue.cptr());
	  strToChar.Format("%s has changed your alignment to %d.\r\n",
		  GetName().cptr(), pTarget->m_nAlignment);
	  pTarget->SendToChar(strToChar);
	  strToChar.Format("You change %s's alignment from %d to %d.\r\n",
		  pTarget->GetName().cptr(), nAlign, pTarget->m_nAlignment);
	  SendToChar(strToChar);
	  pTarget->Save();
	} else if (strAtt.Compare("name")) {
	  CString strOldName;
	  strValue = strValue.GetWordAfter(0); //remove spaces etc
	  strValue[0] = toupper(strValue[0]);
	  pTarget->DeleteChar();
	  strOldName = pTarget->m_strName;
	  pTarget->m_strName = strValue;
	  pTarget->m_strAlias.Format("%s %s", m_strName.cptr(), GVM.GetRaceName(pTarget->GetRace()));
	  pTarget->Save();
	  strToChar.Format("%s has changed your name to %s\r\n",
		  GetName().cptr(), pTarget->m_strName.cptr());
	  pTarget->SendToChar(strToChar);
	  strToChar.Format("You change %s's name to %s\r\n",
		  strOldName.cptr(), pTarget->m_strName.cptr());
	  SendToChar(strToChar);
	} else if ((nValue = strValue.MakeInt()) == -1) {
	  if (strAtt.Compare("female")) {
		pTarget->m_pAttributes->SetSex(FEMALE);
	  } else if (strAtt.Compare("male")) {
		pTarget->m_pAttributes->SetSex(MALE);
	  } else if (strAtt.Compare("none")) {
		pTarget->m_pAttributes->SetSex(NONE);
	  } else if (strAtt.Compare("class")) {
		//TODO
		SendToChar("NOT IN YET!\r\n");
		return;
	  } else if (strAtt.Compare("race")) {
		short nRace = GVM.GetRaceNum(strValue);
		if (nRace != -1) {
		  short nLevel = pTarget->GetLevel();
		  short nPrac = pTarget->m_nPracticeSessions;
		  short i;
		  if (pTarget->IsGod()) {
			GVM.RemoveGod(pTarget);
		  }
		  //TODO do we want to do this this way?
		  //should prolly have a function that removes
		  //object affects and char affects without
		  //putting them on and a function we can
		  //call after we level them back up
		  //syncwearaffects could use these two functions
		  pTarget->Save(true);
		  pTarget->m_pAttributes->SetRace(nRace);
		  pTarget->m_strAlias.Format("%s %s", m_strName.cptr(), GVM.GetRaceName(pTarget->GetRace()));
		  pTarget->m_nCurrentHpts = pTarget->m_nMaxHpts = pTarget->m_pAttributes->GetStartingHitPoints();
		  pTarget->m_nMovePts = pTarget->m_nMaxMovePts = pTarget->m_pAttributes->GetStartingMoves();
		  pTarget->m_nManaPts = pTarget->m_nMaxManaPts = pTarget->m_pAttributes->GetStartingMana();
		  pTarget->m_nLevel = 0;
		  for (i = 0; i < nLevel; i++) {
			pTarget->AdvanceLevel(true, false);
		  }
		  pTarget->m_nPracticeSessions = nPrac;
		  pTarget->SyncWearAffects();
		  pTarget->Save();
		  strToChar.Format("%s has changed your race to %s.\r\n",
			  GetName().cptr(),
			  GVM.GetColorRaceName(pTarget->GetRace()));
		  pTarget->SendToChar(strToChar);
		  strToChar.Format("You change %s's race to %s.\r\n",
			  pTarget->GetName().cptr(),
			  GVM.GetColorRaceName(pTarget->GetRace()));
		  SendToChar(strToChar);
		} else {
		  SendToChar("Which race?\r\n");
		}
		return;
	  } else {
		SendToChar(s);
		return;
	  }
	  strToChar.Format("You change %s's sex to %s.\r\n",
		  pTarget->GetName().cptr(), strAtt.cptr());
	  SendToChar(strToChar);
	  strToChar.Format("%s has changed your sex to %s.\r\n",
		  GetName().cptr(), strAtt.cptr());
	  pTarget->SendToChar(strToChar);
	  pTarget->Save();
	} else {
	  nValue = RANGE(nValue, 120, 25);
	  if (strAtt.Compare("strength")) {
		pTarget->m_pAttributes->BaseStats[STR] = nValue;
	  } else if (strAtt.Compare("Intelligence")) {
		pTarget->m_pAttributes->BaseStats[INT] = nValue;
	  } else if (strAtt.Compare("wisdom")) {
		pTarget->m_pAttributes->BaseStats[WIS] = nValue;
	  } else if (strAtt.Compare("agility")) {
		pTarget->m_pAttributes->BaseStats[AGI] = nValue;
	  } else if (strAtt.Compare("charisma")) {
		pTarget->m_pAttributes->BaseStats[CHA] = nValue;
	  } else if (strAtt.Compare("Dexterity")) {
		pTarget->m_pAttributes->BaseStats[DEX] = nValue;
	  } else if (strAtt.Compare("luck")) {
		pTarget->m_pAttributes->BaseStats[LUCK] = nValue;
	  } else if (strAtt.Compare("constitution")) {
		pTarget->m_pAttributes->BaseStats[CON] = nValue;
	  } else if (strAtt.Compare("power")) {
		pTarget->m_pAttributes->BaseStats[POW] = nValue;
	  } else if (strAtt.Compare("HitPoints")) {
		nValue = strValue.MakeInt();
		nValue = nValue < 1 ? 1 : nValue;
		pTarget->m_nMaxHpts = nValue;
		pTarget->m_nCurrentHpts = pTarget->m_nCurrentHpts > pTarget->m_nMaxHpts ? pTarget->m_nMaxHpts : pTarget->m_nCurrentHpts;
	  } else if (strAtt.Compare("mana")) {
		nValue = strValue.MakeInt();
		nValue = nValue < 1 ? 1 : nValue;
		pTarget->m_nMaxManaPts = nValue;
		pTarget->m_nManaPts = pTarget->m_nManaPts > pTarget->m_nMaxManaPts ? pTarget->m_nMaxManaPts : pTarget->m_nManaPts;
	  } else if (strAtt.Compare("moves")) {
		nValue = strValue.MakeInt();
		nValue = nValue < 1 ? 1 : nValue;
		pTarget->m_nMaxMovePts = nValue;
		pTarget->m_nMovePts = pTarget->m_nMovePts > pTarget->m_nMaxMovePts ? pTarget->m_nMaxMovePts : pTarget->m_nMovePts;
	  } else {
		SendToChar(s);
		return;
	  }
	  strToChar.Format("You change %s's %s to %d.\r\n",
		  pTarget->GetName().cptr(),
		  strAtt.cptr(), nValue);
	  SendToChar(strToChar);
	  strToChar.Format("Your %s has been changed by %s.\r\n",
		  strAtt.cptr(), GetName().cptr());
	  pTarget->SendToChar(strToChar);
	  pTarget->SyncWearAffects();
	  pTarget->Save();
	}
  }
}

//////////////////////////////////////
//	Allows players to search for doors, objects and characters
//	search <object Name> OR just search
/////////////////////////////////////

void CCharacter::DoSearch(CObject *pCon) {
  if (this->StatCheck(LUCK)) {
	CString str;
	CObject *pObj;
	if (pCon == NULL) {
	  //look for characters first
	  CCharacter *pCh;
	  if ((pCh = GetRoom()->IsAnyCharacterAffectedBy(CCharacter::AFFECT_HIDING))) {
		pCh->RemoveAffect(AFFECT_HIDING);
		str.Format("You find %s hiding here!\r\n",
			pCh->GetRaceOrName(this).cptr());
		SendToChar(str);
		str.Format("%s points directly at you!\r\n",
			GetRaceOrName(pCh).cptr());
		pCh->SendToChar(str);
		GetRoom()->SendToRoom("%s points our %s hidding here.\r\n", this, pCh);
	  } else if (GetRoom()->OpenSecretDoor()) {
		SendToChar("You find a secret door!\r\n");
		GetRoom()->SendToRoom("%s finds a secret door!\r\n", this);
	  } else if ((pObj = GetRoom()->IsObjectAffectedBy(CObject::OBJ_AFFECT_HIDDEN))) {
		pObj->RemoveAffect(CObject::OBJ_AFFECT_HIDDEN);
		str.Format("You find %s.\r\n", pObj->GetObjName(this));
		SendToChar(str);
		GetRoom()->SendToRoom("%s finds %s.\r\n", this, pObj->GetObjName());
	  } else {
		SendToChar("You didn't find anything.\r\n");
	  }

	} else {
	  if ((pObj = pCon->IsContentsAffectedBy(CObject::OBJ_AFFECT_HIDDEN))) {
		pObj->RemoveAffect(CObject::OBJ_AFFECT_HIDDEN);
		str.Format("You find %s in the %s",
			(const char *) pObj->GetObjName(this),
			(const char *) pObj->GetObjName(this));
		SendToChar(str);
		GetRoom()->SendToRoom("%s finds a %s.\r\n", this, (const char *) pObj->GetObjName());
	  } else {
		SendToChar("You didn't find anything.\r\n");
	  }
	}
  } else {
	SendToChar("You don't find anything.\r\n");
  }
}

////////////////////////////
//	GetSkill
//	Use this function get the skill level of any character
//	we can also put in pluses or minues

skill_type CCharacter::GetSkill(short nSkill) const {
  skill_type nRetVal;
  //if they fail the skill blind check and they are affectedby blind
  bool bBlindMinus = (DIE(100) > m_Skills[SKILL_BLIND_FIGHTING] && CanSeeInRoom(GetRoom()) == SEES_NOTHING);
  switch (nSkill) {
	case SKILL_KICK: case SKILL_BASH: case SKILL_HEAD_BUTT: case SKILL_BACKSTAB:
	case SKILL_OFFENSE: case SKILL_CIRCLE: case SKILL_QUICK_STRIKE:
	  nRetVal = bBlindMinus ? m_Skills[nSkill] >> 1 : m_Skills[nSkill];
	  break;
	case SKILL_DODGE:
	  nRetVal = (m_Skills[SKILL_DODGE] + m_pAttributes->DodgeBonus()) / (bBlindMinus ? 4 : 1);
	  break;
	case SKILL_PARRY:
	  nRetVal = (m_Skills[SKILL_PARRY] + m_pAttributes->GetParryBonus()) / (bBlindMinus ? 8 : 1);
	  break;
	case SKILL_RIPOSTE:
	  nRetVal = (m_Skills[SKILL_RIPOSTE] + m_pAttributes->GetRiposteBonus()) / (bBlindMinus ? 16 : 1);
	  break;
	default:
	  nRetVal = m_Skills[nSkill];
	  break;
  }
  return RANGE(nRetVal, MAX_SKILL_LEVEL, 0);
}

////////////////////////////////
//  pass in the skill you want the character to save against
//	and an optional adjustment factor

bool CCharacter::MakeSkillSave(short nSkillToSaveAgainst, short Adjustments) {
  skill_type nSkillPct = Adjustments;
  switch (nSkillToSaveAgainst) {
	case SKILL_BODYSLAM:
	  nSkillPct += GetSkill(SKILL_DODGE) >> 1;
	  break;
	case SKILL_BASH:
	  nSkillPct += GetSkill(SKILL_DODGE) >> 2;
	  break;
	case SKILL_BACKSTAB:
	  nSkillPct += GetSkill(SKILL_BATTLE_READINESS) >> 2;
	  break;
	case SKILL_TRIP:
	  nSkillPct += (skill_type) m_pAttributes->GetStat(DEX) >> 2;
	  break;
  }
  return(DIE(101) <= nSkillPct);
}

/////////////////////////////////
//	InitializeCharacter
//	virtual function used to initialize characters
//	not using the constructor to do this becuase we may need
//	to call some virtual funtions

void CCharacter::InitializeCharacter() {
  //remove affects that should never come from a character
  //entering the game
  RemoveAffect(AFFECT_CASTING, false);
  RemoveAffect(AFFECT_MEDITATE, false);
  if (m_pInRoom->GetVnum() == FUGUE_PLANE) {
	AddAffect(AFFECT_FUGUE_PLANE, GetFugueTime(), 0);
  }
  //readd affects with no messages
  //THIS MUST BE DONE BEFORE WE LOAD THE OBJECTS
  //b/c if an object puts an affect on the character we would add it twice
  //we the objects get reworn they will re-apply thier wear affects
  for (short i = 0; i < TOTAL_AFFECTS; i++) {
	//now we have copied over the affects that were saved but
	//the affects have not been reapplyied to the character yet
	//so we call the apply function for each affect that the character
	//has.
	if (IsAffectedBy(i) && m_AffectInfo[i].m_pFncApply != NULL) {
	  (this->*m_AffectInfo[i].m_pFncApply)(i, false);
	}
  }
  //////////////
  //load players objects
  this->LoadObjects();
  //load spells has to be called here because it relies on
  //virtual functions therefore can't be called from a constructor
  this->LoadSpells();
  this->UpDatePosition(POS_STANDING, false);
}

///////////////////////////////////////////////
//	Is the character have enough exp for the next level?

bool CCharacter::EnoughExpForNextLevel(bool bLastMortalLevelRestrict) {
  //they have to have enough exp and be less than lvl_last_mortal
  //or bLastMortalLevelRestrict has to be false
  if ((GetLevel() < LVL_LAST_MORTAL
	  || bLastMortalLevelRestrict == false)
	  && GetLevel() < LVL_IMMORT
	  && (GetExp(this->GetLevel() + 1) < this->m_nExperience)) {
	return true;
  }
  return false;
}

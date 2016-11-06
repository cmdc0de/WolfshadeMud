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
 * Message manager implementation
 *
 * 5/5/98
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"

#define COMMENT_CHARACTER '*'
#define NO_ENTRY_CHARACTER '#'
#define SPACE ' '
#define SKILL_MSG_FILE "files/skill_messages.txt"
#define SPELL_MSG_FILE "files/spell_messages.txt"
#define SOCIAL_FILE "files/socials.txt"
#define AFFECT_FILE "files/affect_messages.txt"

//////////////////////////////////
//	ReadMessage
//	The message struct know how to read itself
//	we get the file and read in the messages
//	5/5/98
//  written by: Demetrius Comes
//////////////////////////////////

void CMsg::ReadMessages(CAscii & MsgFile) {
   char msg[256];
   int i = 0;

   while (MsgFile.good() && i < m_nMsg) {
	MsgFile.getline(msg, 256);
	if (*msg != COMMENT_CHARACTER && *msg != SPACE) {
	   if (*msg != NO_ENTRY_CHARACTER) {
		m_pMsg[i].Format("%s\r\n", msg);
	   }
	   i++;
	}
   }
}


const char *CMessageManager::m_AttackMessages[] ={
   "Your %s %s kills %s.\r\n",
   "%s kills you with %s %s %s.\r\n",
   "'s %s %s kills ",
   "You miss %s with your %s.\r\n",
   "%s misses you with %s %s.\r\n",
   "%s misses %s with %s %s.\r\n",
   "Your %s %s %s %s.\r\n",
   "%s's %s %s %s you.\r\n",
   "'s %s %s %s "//,
   //"Your attempt to hit his %sholyness %s fails\r\n",
   //"%s tries to hit %s %sholyness %s connects... but do you really want that?\r\n",
   //"%s tries to hit %s %sholyness %s connects... but do you really want that?\r\n"
};

const char *CMessageManager::m_WeaponTypes[] ={
   "*reserved*",
   "hit",
   "pierce",
   "poke",
   "slash",
   "whip",
   "bite",
   "punch",
   "bludgeon",
   "crush"
};

const char *CMessageManager::m_HitMessages[] ={
   "*reserved*",
   "pitiful",
   "mediocre",
   "fine",
   "hard",
   "nasty",
   "massive",
   "awesome",
   "devastating",
   "godly"
};

const char *CMessageManager::m_DamMessages[] ={
   "*reserved*",
   "grazes",
   "hits",
   "wounds",
   "slaughters",
   "nearly destroys",
   "absolutely pummels",
   "sprays a mist of &rblood&n around"
};


/////////////////////////////////////////////
//	SendHit
//	Send Hit message
//	written by: Demetrius Comes
//	5/19/98
//////////////////////////////////////////////

void CMessageManager::SendHit(CCharacter * pAttacker, CCharacter * pDefender, short nWeaponType, short nDam) const {
   CString Msg, strDam, strHit;
   short nMsg, nDiff;
   if (nDam < 5) nMsg = HIT_PITIFUL;
   else if (nDam < 10) nMsg = HIT_MEDIOCRE;
   else if (nDam < 20) nMsg = HIT_FINE;
   else if (nDam < 30) nMsg = HIT_HARD;
   else if (nDam < 50) nMsg = HIT_NASTY;
   else if (nDam < 70) nMsg = HIT_MASSIVE;
   else if (nDam < 90) nMsg = HIT_AWESOME;
   else if (nDam < 130) nMsg = HIT_DEVASTATING;
   else nMsg = HIT_GODLY;
   strHit = m_HitMessages[nMsg];
   nDiff = (pDefender->GetCurrentHpts() * 100) / pDefender->GetMaxHits();
   //if you lightly slash, you don't get a killer message
   if (nMsg > 2) {
	if (nDiff < 1) nMsg = DAM_BLOOD;
	else if (nDiff < 5) nMsg = DAM_DESTROY;
	else if (nDiff < 10) nMsg = DAM_PUMMEL;
	else if (nDiff < 15) nMsg = DAM_SLAUGHTER;
	else if (nDiff < 30) nMsg = DAM_WOUND;
	else if (nDiff < 70) nMsg = DAM_HIT;
	else nMsg = DAM_GRAZE;
   }
   strDam = m_DamMessages[nMsg];
   CString Msg2 = "%s";
   if (pDefender->IsDead()) {
	Msg.Format(m_AttackMessages[TO_DAMAGER_DEATH],
		strHit.cptr(), m_WeaponTypes[nWeaponType],
		pDefender->GetRaceOrName(pAttacker).cptr());
	pAttacker->SendToChar(Msg);
	Msg.Format(m_AttackMessages[TO_DAMAGEE_DEATH], pAttacker->GetRaceOrName(pDefender).cptr(),
		CCharacterAttributes::GetPronoun(pAttacker->GetSex()), strHit.cptr(),
		m_WeaponTypes[nWeaponType]);
	pDefender->SendToChar(Msg);
	Msg.Format(m_AttackMessages[TO_ROOM_DEATH], strHit.cptr(),
		m_WeaponTypes[nWeaponType]);
	Msg2 += Msg;
	Msg2 += "%s.\r\n";
	pDefender->GetRoom()->SendToRoom(Msg2, pAttacker, pDefender);
   } else {
	Msg.Format(m_AttackMessages[TO_DAMAGER_HIT], strHit.cptr(), m_WeaponTypes[nWeaponType],strDam.cptr(),
		pDefender->GetRaceOrName(pAttacker).cptr());
	pAttacker->SendToChar(Msg);
	Msg.Format(m_AttackMessages[TO_DAMAGEE_HIT], pAttacker->GetRaceOrName(pDefender).cptr(),
		strHit.cptr(), m_WeaponTypes[nWeaponType], strDam.cptr());
	pDefender->SendToChar(Msg);
	Msg.Format(m_AttackMessages[TO_ROOM_HIT], strHit.cptr(), m_WeaponTypes[nWeaponType], strDam.cptr());
	Msg2 += Msg;
	Msg2 += "%s\r\n";
	pDefender->GetRoom()->SendToRoom(Msg2, pAttacker, pDefender);
   }
}




/////////////////////////////////////////////////
// SendMiss()
//	Send Miss message to attacker, defender
//	 and to room
//	written by Demetrius Comes
//	5/19/98
///////////////////////////////////////////////

void CMessageManager::SendMiss(CCharacter * pAttacker, CCharacter * pDefender, short nWeaponType) const {
   //should be checking for god level!
   CString Msg;
   Msg.Format(m_AttackMessages[TO_DAMAGER_MISS], pDefender->GetRaceOrName(pAttacker).cptr(), m_WeaponTypes[nWeaponType]);
   pAttacker->SendToChar(Msg);
   Msg.Format(m_AttackMessages[TO_DAMAGEE_MISS], pAttacker->GetRaceOrName(pDefender).cptr(),
	   CCharacterAttributes::GetPronoun(pAttacker->GetSex()), m_WeaponTypes[nWeaponType]);
   pDefender->SendToChar(Msg);
   pDefender->GetRoom()->SendToRoom(m_AttackMessages[TO_ROOM_MISS],
	   pAttacker, pDefender, CString(CCharacterAttributes::GetPronoun(pAttacker->GetSex())),
	   CString(m_WeaponTypes[nWeaponType]));
}
/////////////////////////////
//	Constructor
//	Protected so that only CCharacter can use this class
//	Boots all the messages
//	written by: Demetrius
//	5/4/98
//////////////////////////////

CMessageManager::CMessageManager() {
   m_pSpellMsg = NULL;
   m_pSocialMap = NULL;
   m_pAffectMsg = NULL;
   MudLog << "Booting Skills File" << endl;
   BootSkillsFile();
   MudLog << "Booting SpellsFile (mage, cleric, empath, psi, and flayer)" << endl;
   BootMessageFile(SPELL_MSG_FILE, TOTAL_DAM_MESSAGES, (m_pSpellMsg = new CMap<CString, CMsg *>(127)));
   MudLog << "Booting social file" << endl;
   BootMessageFile(SOCIAL_FILE, TOTAL_SOCIAL_MSG, (m_pSocialMap = new CMap<CString, CMsg *>(127, 2)));
   MudLog << "Booting Affect files" << endl;
   BootMessageFile(AFFECT_FILE, TOTAL_AFFECT_MESSAGES, (m_pAffectMsg = new CMap<CString, CMsg *>(127)));
}

//////////////////////////////////////
//	SkillWorked
//	send skill worked message to all
//	written by: Demetrius Comes
//	6/2/98
/////////////////////////////////////

void CMessageManager::SkillMsg(CCharacter * pDoer, CCharacter * pDoie, int nSkill, int nMessageType) const {
   int nMsg = nMessageType == SKILL_WORKED ? TO_SKILL_DOER_WORKED : nMessageType == SKILL_KILLED ? TO_SKILL_DOER_DEATH : TO_SKILL_DOER_DIDNT_WORK;

   CString Msg = m_SkillMsg[nSkill].m_pMsg[nMsg];
   if (!Msg.IsEmpty()) {
	BuildMessage(Msg, pDoer, pDoie, pDoer);
	pDoer->SendToChar(Msg);
   }
   if (!m_SkillMsg[nSkill].m_pMsg[++nMsg].IsEmpty()) {
	Msg = m_SkillMsg[nSkill].m_pMsg[nMsg];
	BuildMessage(Msg, pDoer, pDoie, pDoie);
	pDoie->SendToChar(Msg);
   }
   if (!m_SkillMsg[nSkill].m_pMsg[++nMsg].IsEmpty()) {
	SendToRoom(m_SkillMsg[nSkill].m_pMsg[nMsg], pDoer, pDoie);
   }
}

///////////////////////////////////////////
//	BuildMessage()
//	Remove the $n and $N etc checks race and name with onlooker
//	written by: Demetrius Comes
//	6/2/98
/////////////////////////////////////////

void CMessageManager::BuildMessage(CString & strMsg, CCharacter * pDoer, CCharacter * pDoie, CCharacter *pOnLooker) const {
   //both doer and doie should never be NULL at the same time!
   assert(!(pDoer == NULL && pDoie == NULL));

   CString strTmp;
   int nLocation = 0;

   while ((nLocation = strMsg.Find('$')) != -1) {
	strTmp += strMsg.Left(nLocation);
	//if they can see all so the subsitution
	switch (strMsg[nLocation + 1]) {
		//for each sub ech to see if on looker can see the character
		//if so give them race or name, if not give them someone
	   case 'n':
		strTmp += pDoie != NULL ? pDoie->GetRaceOrName(pOnLooker) : CString();
		break;
	   case 'N':
		strTmp += pDoer != NULL ? pDoer->GetRaceOrName(pOnLooker) : CString();
		break;
	   case 'p':
		strTmp += pDoie != NULL ? pOnLooker->CanSeeChar(pDoie) ? CCharacterAttributes::GetPronoun(pDoie->GetSex()) : "someone" : "";
		break;
	   case 'P':
		strTmp += pDoer != NULL ? pOnLooker->CanSeeChar(pDoer) ? CCharacterAttributes::GetPronoun(pDoer->GetSex()) : "someone" : "";
		break;
	   default:
		ErrorLog << "bad $ in BuildMessage:" << strTmp << strMsg.Right(nLocation + 1) << endl;
		break;
	}
	strMsg = strMsg.Right(nLocation + 1);
   }
   strTmp += strMsg;
   strMsg = strTmp;
}

/////////////////////////////
//	Boots skill file
////////////////////////////

void CMessageManager::BootSkillsFile() {
   CAscii SkillMsgFile(SKILL_MSG_FILE, std::ios_base::in);

   if (!SkillMsgFile) {
	ErrorLog << "No Skills Message File!!!\r\n";
   } else {
	m_SkillMsg = new CMsg[MAX_MUD_SKILLS];
	CString Msg(127);
	int i;

	while (SkillMsgFile.good()) {
	   SkillMsgFile.getline(Msg.ptr(), 127);
	   if (Msg[0] != COMMENT_CHARACTER && Msg[0] != SPACE && Msg != "") {
		i = Msg.MakeInt();
		if (i>-1 && i < MAX_MUD_SKILLS) {
		   m_SkillMsg[i].ReadMessages(SkillMsgFile);
		}
	   }
	}
   }
}

///////////////
//	destructor
/////////////

CMessageManager::~CMessageManager() {
   delete [] m_SkillMsg;
   POSITION pos = m_pSpellMsg != NULL ? m_pSpellMsg->GetStartingPosition() : NULL;
   CMsg *pMsg;
   while (pos) {
	pMsg = m_pSpellMsg->GetNext(pos);
	delete pMsg;
   }
   delete m_pSpellMsg;
   pos = m_pSocialMap != NULL ? m_pSocialMap->GetStartingPosition() : NULL;
   while (pos) {
	pMsg = m_pSocialMap->GetNext(pos);
	delete pMsg;
   }
   delete m_pSocialMap;
   pos = m_pAffectMsg != NULL ? m_pAffectMsg->GetStartingPosition() : NULL;
   while (pos) {
	pMsg = m_pAffectMsg->GetNext(pos);
	delete pMsg;
   }
   delete m_pAffectMsg;
}

//////////////////////////////////
//	Sends spell messages

void CMessageManager::SpellMsg(CString strSpell, int nMessageType, CCharacter * pDoer, CCharacter * pDoie) const {
   CMsg *pMsg;
   if (!m_pSpellMsg->Lookup(strSpell, pMsg)) {
	ErrorLog << "No spell messages for " << strSpell << endl;
	return;
   }
   int nMsg = nMessageType == SKILL_WORKED ? TO_SKILL_DOER_WORKED : nMessageType == SKILL_KILLED ? TO_SKILL_DOER_DEATH : TO_SKILL_DOER_DIDNT_WORK;
   CString Msg = pMsg->m_pMsg[nMsg];
   if (pDoie != pDoer && pDoer != NULL) {
	BuildMessage(Msg, pDoer, pDoie, pDoer);
	pDoer->SendToChar(Msg);
   }
   Msg = pMsg->m_pMsg[++nMsg];
   if (pDoie != NULL) {
	BuildMessage(Msg, pDoer, pDoie, pDoie);
	pDoie->SendToChar(Msg);
   }
   SendToRoom(pMsg->m_pMsg[++nMsg], pDoer, pDoie);
}

///////////////////////////////
//	SendsSocialMessage
//	if array pos 2 is empty then the
//	social is self only
///////////////////////////////

void CMessageManager::SocialMsg(const CMsg *pMsg, CCharacter * pDoer, CCharacter * pDoie) const {
   CString strMsg;
   if (pMsg->m_pMsg[TARGET_TO_SELF_SOCIAL].IsEmpty() || !pDoie) {
	strMsg = pMsg->m_pMsg[SELF_TO_SELF_SOCIAL];
	BuildMessage(strMsg, pDoer, pDoie, pDoer);
	pDoer->SendToChar(strMsg);

	SendToRoom(pMsg->m_pMsg[SELF_TO_ROOM_SOCIAL], pDoer, NULL);
   } else {
	strMsg = pMsg->m_pMsg[TARGET_TO_SELF_SOCIAL];
	BuildMessage(strMsg, pDoer, pDoie, pDoer);
	pDoer->SendToChar(strMsg);

	strMsg = pMsg->m_pMsg[TARGET_TO_TARGET_SOCIAL];
	BuildMessage(strMsg, pDoer, pDoie, pDoie);
	pDoie->SendToChar(strMsg);

	SendToRoom(pMsg->m_pMsg[TARGET_TO_ROOM_SOCIAL], pDoer, pDoie);
   }
}

//////////////////////////////////////
//	SendToRoom
//	Used to send messages to rooms for spells, socials and affects

void CMessageManager::SendToRoom(CString strMsg, CCharacter * pDoer, CCharacter * pDoie) const {
   CString strToRoom;
   POSITION pos = pDoer->GetRoom()->GetFirstCharacterInRoom();
   CCharacter *pch;
   while (pos) {
	pch = pDoer->GetRoom()->GetNextCharacter(pos);
	//insure we are not sending to doer or doie
	if (pch != pDoer && pch != pDoie && !pch->InPosition(POS_SLEEPING)) {
	   strToRoom = strMsg;
	   BuildMessage(strToRoom, pDoer, pDoie, pch);
	   pch->SendToChar(strToRoom);
	}
   }
}

//////////////////////////////
//	SendAffectMsg
//  Send the apply affect messages

void CMessageManager::SendAffectMsg(CString strAffect, CCharacter * pCh, short nMsg) const {
   if (!strAffect.IsEmpty()) {
	CMsg *pMsg;
	if (!m_pAffectMsg->Lookup(strAffect, pMsg)) {
	   ErrorLog << "No affect messages for " << strAffect << endl;
	} else {
	   CString Msg = pMsg->m_pMsg[nMsg];
	   if (!Msg.IsEmpty()) {
		BuildMessage(Msg, pCh, NULL, pCh);
		pCh->SendToChar(Msg);
	   }
	   Msg = pMsg->m_pMsg[++nMsg];
	   if (!Msg.IsEmpty()) {
		SendToRoom(Msg, pCh, NULL);
	   }
	}
   }
}

void CMessageManager::BootMessageFile(const char *strFile, int nMessages, CMap<CString, CMsg *> *&pMap, int nHas) {
   CAscii MsgFile(strFile, std::ios_base::in);

   if (!MsgFile) {
	ErrorLog << "The Message File: " << strFile << " does not exist" << endl;
   } else {
	char Msg[127];
	CMsg *pMsg;

	while (MsgFile.good()) {
	   MsgFile.getline(Msg, 127);
	   if (Msg[0] != COMMENT_CHARACTER &&
		   Msg[0] != SPACE &&
		   Msg[0] != '\0') {
		if (!pMap->Lookup(Msg, pMsg)) {
		   pMsg = new CMsg(nMessages);
		   pMsg->ReadMessages(MsgFile);
		   pMap->Add(Msg, pMsg);
		} else {
		   ErrorLog << "There are two message sets for spell:" << Msg << endl;
		   //need to skip nMessages lines
		   int nSkip = nMessages;
		   while (nSkip--) {
			MsgFile.getline(Msg, 127);
		   }
		}
	   }
	}
   }
}

////////////////////////////////
//	Send skill save message

void CMessageManager::SkillSaveMsg(CCharacter *pSaved, CCharacter *pAttacker, const char *strSkill) const {
   CString str;
   str.Format("You side step %s's %s.\r\n",
	   pAttacker->GetRaceOrName(pSaved).cptr(),
	   strSkill);
   pSaved->SendToChar(str);
   str.Format("%s side steps your %s.\r\n",
	   pSaved->GetRaceOrName(pAttacker).cptr(),
	   strSkill);
   pAttacker->SendToChar(str);
   str.Format("$N side steps $n's %s.\r\n", strSkill);
   SendToRoom(str, pSaved, pAttacker);
}

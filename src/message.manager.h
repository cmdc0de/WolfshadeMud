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
* Message Manager
*
*
*/
#ifndef _MESSAGE_MANAGER_H_
#define _MESSAGE_MANAGER_H_
//message types
#define NONE 0
#define HIT 1
#define PIERCE 2
#define POKE 3
#define SLASH 4
#define WHIP 5
#define BITE 6
#define PUNCH 7
#define BLUDGEON 8
#define CRUSH 9
#define TOTAL_WEAPON_MESSAGES 10

#define HIT_NONE 0
#define HIT_PITIFUL 1
#define HIT_MEDIOCRE 2
#define HIT_FINE 3
#define	HIT_HARD 4
#define HIT_NASTY 5
#define HIT_MASSIVE 6
#define HIT_AWESOME 7
#define HIT_DEVASTATING 8
#define HIT_GODLY 9
#define TOTAL_HIT_MESSAGES 10

#define DAM_NONE 0
#define DAM_GRAZE 1
#define DAM_HIT 2
#define DAM_WOUND 3
#define DAM_SLAUGHTER 4
#define DAM_DESTROY 5
#define DAM_PUMMEL 6
#define DAM_BLOOD 7
#define TOTAL_DAM_MSG 8

#define TO_DAMAGER_DEATH 0
#define TO_DAMAGEE_DEATH 1
#define TO_ROOM_DEATH 2
#define TO_DAMAGER_MISS 3
#define TO_DAMAGEE_MISS 4
#define TO_ROOM_MISS 5
#define TO_DAMAGER_HIT 6
#define TO_DAMAGEE_HIT 7
#define TO_ROOM_HIT 8
#define TOTAL_DAM_MESSAGES 9 //no god messages except for hit attack msg

#define TO_SKILL_DOER_DEATH 0 //hehe doer and doie =)
#define TO_SKILL_DOIE_DEATH 1
#define TO_ROOM_SKILL_DEATH 2
#define TO_SKILL_DOER_DIDNT_WORK 3
#define TO_SKILL_DOIE_DIDNT_WORK 4
#define TO_SKILL_ROOM_DIDNT_WORK 5
#define TO_SKILL_DOER_WORKED 6
#define TO_SKILL_DOIE_WORKED 7
#define TO_SKILL_ROOM_WORKED 8

#define SKILL_WORKED 1
#define SKILL_DIDNT_WORK 2
#define SKILL_KILLED 3

#define MIN_SOCIAL_LETTERS 3
#define TOTAL_SOCIAL_MSG 5
#define SELF_TO_SELF_SOCIAL 0
#define SELF_TO_ROOM_SOCIAL 1
#define TARGET_TO_SELF_SOCIAL 2
#define TARGET_TO_TARGET_SOCIAL 3
#define TARGET_TO_ROOM_SOCIAL 4

//affect msg
#define TOTAL_AFFECT_MESSAGES 4
#define MSG_APPLY_AFFECT 0
#define MSG_APPLY_AFFECT_ROOM 1
#define MSG_END_AFFECT 2
#define MSG_END_AFFECT_ROOM 3

class CMsg
{
public:
	short m_nMsg;
	CString *m_pMsg;
	void ReadMessages(CAscii &MsgFile);
	CMsg(short nMsg=TOTAL_DAM_MESSAGES) {m_pMsg = new CString[nMsg];m_nMsg = nMsg;}
	~CMsg() {delete [] m_pMsg;}
};

class CMessageManager
{
	//CCharacter friend it is the only thing that can access
	friend class CCharacter;
private:
	void BootSkillsFile();
	void BootMessageFile(const char *strFile,int nMessages,CMap<CString, CMsg *> *&pMap, int nHash=127);
	static const char* m_AttackMessages[];
	static const char* m_WeaponTypes[];
	static const char* m_HitMessages[];
	static const char* m_DamMessages[];
	CMsg *m_SkillMsg;
	CMap<CString,CMsg *> *m_pSpellMsg;
	CMap<CString,CMsg *> *m_pSocialMap;
	CMap<CString,CMsg *> *m_pAffectMsg;
protected:
	void SendToRoom(CString strMsg,CCharacter *pDoer,CCharacter *pDoie) const;
	void SendHit(CCharacter *pAttacker,CCharacter *pDefender,short nWeaponType, short nDam) const;
	void SendMiss(CCharacter *pAttacker,CCharacter *pDefender,short nWeaponType) const;
	CMessageManager();
	void BuildMessage(CString &strMsg,CCharacter *pDoer,CCharacter *pDoie,CCharacter *pOnLooker) const;
public:
	void SkillSaveMsg(CCharacter *pSaved, CCharacter *pAttacker,const char *strSkill) const;
	void SendAffectMsg(CString strAffect, CCharacter * pCh,short nMsg) const;
	void SkillMsg(CCharacter *pDoer, CCharacter *pDoie,int nSkill,int nMessageType) const;
	void SocialMsg(const CMsg *pMsg, CCharacter *pDoer,CCharacter *pDoie=NULL) const;
	inline const CMsg *IsSocial(CString strSocial) const;
	void SpellMsg(CString strSpell,int nMessageType, CCharacter * pDoer, CCharacter * pDoie) const;
	 ~CMessageManager();
};

inline const CMsg *CMessageManager::IsSocial(CString strSocial)  const
{
	CMsg *pMsg;
	if(m_pSocialMap->Lookup(strSocial.cptr(),pMsg))
		return pMsg;
	return NULL;
}
#endif

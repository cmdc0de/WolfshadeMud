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
/*
*
*	stores code for CCharacter::CGroup class
*	stored here because CCharacter class is just
*	too big
*	And master code
*/

#include "stdinclude.h"
#pragma hdrstop

////////////////////////////////////////
//	AddToGroup
//	This function handles about all the
//	actual group functionality
//	checks to see if we need to make a new group
//	aka the Grouper isn't grouped already
//	that the grouper is the head, etc etc
//	written by: Demetrius Comes
void CCharacter::sGroup::AddToGroup(CCharacter *pGrouper,CCharacter *pCh) 
{
	CString strToChar;
	if(pGrouper->GetGroup().IsGrouped())
	{
		//group self leave the group
		if(pCh==pGrouper)
		{
			pCh->SendToChar("You leave the group.\r\n");
			Remove(pCh);
		}
		//you can't group if your not the leader
		//of the group
		else if(!pGrouper->GetGroup().IsHead(pGrouper))
		{
			pGrouper->SendToChar("You are not the group leader.\r\n");
		}
		//if he's already in the group remove him
		else if(pGrouper->GetGroup().IsInGroup(pCh))
		{
			if(pCh!=m_pGroup->GetFirst())
			{
				strToChar.Format("You have just been kicked out of %s's group.\r\n",(char *)pGrouper->GetRaceOrName(pCh));
				pCh->SendToChar(strToChar);
				pGrouper->GetRoom()->SendToRoom("%s has just been kick out of %s's group.\r\n",pCh,pGrouper);
			}
			Remove(pCh);
		}
		//check to insure the groupie isn't in another group
		else if(pCh->GetGroup().IsGrouped())
		{
			strToChar.Format("%s is in another group.\r\n",(char *)pCh->GetRaceOrName(pGrouper));
			pGrouper->SendToChar(strToChar);
		}
		// you have to be consented
		else if(pCh->GetConsented()!=pGrouper)
		{
			strToChar.Format("You do not have %s consent.\r\n",
				CCharacterAttributes::GetPronoun(pCh->GetSex()));
			pGrouper->SendToChar(strToChar);
		}
		//ok add pCh to pGroupers group
		else
		{
			pGrouper->GetGroup().m_pGroup->Add(pCh);
			pCh->GetGroup().SetGroup(pGrouper->GetGroup().m_pGroup);
			strToChar.Format("You are now a member of %s's group.\r\n",(char *)pGrouper->GetName());
			pCh->SendToChar(strToChar);
			strToChar.Format("%s is now a member of your group.\r\n",(char *)pCh->GetName());
			pGrouper->SendToChar(strToChar);
		}
	}
	//the grouper is not grouped make sure
	//he's not trying to group self
	else if(pGrouper!=pCh)
	{
		if(pCh->GetConsented()!=pGrouper)
		{
			strToChar.Format("You do not have %s consent.\r\n",
				CCharacterAttributes::GetPronoun(pCh->GetSex()));
			pGrouper->SendToChar(strToChar);
		}
		else if(pCh->GetGroup().IsGrouped())
		{
			strToChar.Format("%s is in another group.\r\n",
				(char *)pCh->GetRaceOrName(pGrouper));
			pGrouper->SendToChar(strToChar);
		}
		else
		{
			//make a new group and make pGrouper head
			NewGroup(pGrouper);
			// add new char
			m_pGroup->Add(pCh);
			pCh->GetGroup().SetGroup(pGrouper->GetGroup().m_pGroup);
			strToChar.Format("You are now a member of %s's group.\r\n",(char *)pGrouper->GetRaceOrName(pCh));
			pCh->SendToChar(strToChar);
			strToChar.Format("%s is now a member of your group.\r\n",(char *)pCh->GetName());
			pGrouper->SendToChar(strToChar);
		}
	}
	//he's trying to group self without
	//having a group to leave
	else
	{
		pGrouper->SendToChar("You have no group to leave.\r\n");
	}
}

/////////////////////////////////////
//	Disband //remove all from the group
////////////////////////////////////
void CCharacter::sGroup::Disband(CCharacter *pCh)
{
	if(pCh->GetGroup().IsGrouped())
	{
		if(IsHead(pCh))
		{
			RemoveAll();
		}
		else
		{
			pCh->SendToChar("You have to be the group leader to disband the group.\r\n");
		}
	}
	else
	{
		pCh->SendToChar("You not even a part of a group!\r\n");
	}
}

/////////////////////////
//	Send A message to the whole
// group except the char passed
//	written by: Demetrius comes
void CCharacter::sGroup::SendToGroup(CString strToChar,CCharacter *pCh)
{
	POSITION pos = m_pGroup->GetStartPosition();
	CCharacter *pCurrent;
	CString str;
	while(pos)
	{
		pCurrent = m_pGroup->GetNext(pos);
		if(pCh==NULL)
		{
			pCurrent->SendToChar(strToChar);
		}
		else if(pCurrent!=pCh)
		{
			str.Format(strToChar,(char *)pCh->GetRaceOrName(pCurrent));
			pCurrent->SendToChar(str);
		}
	}
}

/////////////////////////
//	Guks Mantle
//	Gives Divine Armor to group
//	John Comes 3-14-99
void CCharacter::sGroup::GuksMantle(CCharacter *pCaster, short nTime,short nAmount)
{
	if(IsGrouped())
	{
		POSITION pos = m_pGroup->GetStartPosition();
		CCharacter *pCurrent;
		while(pos)
		{
			pCurrent = m_pGroup->GetNext(pos);
			pCurrent->AddAffect(AFFECT_DIVINE_ARMOR,nTime,nAmount);
		}
	}
	else
	{
		pCaster->AddAffect(AFFECT_DIVINE_ARMOR,nTime,nAmount);
	}
}

/////////////////////////
//	Duth's Warmth
//	Gives Full Heal to group
//	John Comes 3-15-99
void CCharacter::sGroup::DuthsWarmth(CCharacter *pCaster,short nAmount)
{
	if(IsGrouped())
	{
		POSITION pos = m_pGroup->GetStartPosition();
		CCharacter *pCurrent;
		while(pos)
		{
			pCurrent = m_pGroup->GetNext(pos);
			if(pCurrent->GetRoom() == pCaster->GetRoom())
			{
				m_MsgManager.SpellMsg((CString)"Full Heal",SKILL_WORKED,pCaster,pCurrent);
				pCurrent->TakeDamage(pCaster,nAmount,false);
			}
		}
	}
	else
	{
		pCaster->TakeDamage(pCaster,nAmount,false);
		m_MsgManager.SpellMsg((CString)"Full Heal",SKILL_WORKED,pCaster,pCaster);
	}
}

//////////////////////////
//	Removes a character from a group
//	if the group is 2 chars before the remove
//	we disband the entire group NO 1 PERSON GROUPS
//	we also check to see if the leader has left the
//	group if so we tell the next char in LL that
//	they are now leader
//	written by: Demetrius Comes
void CCharacter::sGroup::Remove(CCharacter *pCh)
{
	//should be <= 2 because we haven't removed the 2nd member yet
	if((m_pGroup->GetCount() <= 2))
	{
		RemoveAll();
	}
	else
	{
		CCharacter *pHead;
		CString strToChar;
		if(IsHead(pCh))
		{
			//they are leader remove them from the group LL
			m_pGroup->Remove(pCh);
			pHead=m_pGroup->GetFirst();
			//send message to new group leader
			strToChar.Format("%s has left the group. You are now leader.\r\n",(char *)pCh->GetRaceOrName(pHead));
			pHead->SendToChar(strToChar);
		}
		else
		{
			pHead=m_pGroup->GetFirst();
			//send message to head of gropu they pCh is leaving
			strToChar.Format("%s has left your group.\r\n",(char *)pCh->GetRaceOrName(pHead));
			pHead->SendToChar(strToChar);
			//remove them from group LL
			m_pGroup->Remove(pCh);
		}
		//we are ungrouping pCh so null their m_pgroup pointer
		pCh->GetGroup().SetGroup(NULL);
	}
}
		
/////////////////////////////
//	NullGroup
//	We use this to disband a group
//	NULL out all group members group pts
//	then pass back the ptr to group for deletion
//	written by: Demetrius Comes
//////////////////////////////
CList<CCharacter *> *CCharacter::sGroup::NullGroup()
{
	POSITION pos = m_pGroup->GetStartPosition();
	while(pos)
	{
		m_pGroup->GetNext(pos)->GetGroup().SetGroup(NULL);
	}
	return m_pGroup;
}

/////////////////////////////////
//	BuildGroupString
//	Build the vital stat string for
//	groups
//	written by: Demetrius Comes
////////////////////////////////
void CCharacter::sGroup::BuildGroupString(CString & strGroup)
{
	if(!m_pGroup)
	{
		strGroup = "You aren't a member of a group.\r\n";
	}
	else
	{
		POSITION pos = m_pGroup->GetStartPosition();
		CCharacter *pCh;
		while(pos)
		{
			pCh = m_pGroup->GetNext(pos);
			if(IsHead(pCh))
			{
				strGroup.Format("%s%s (Head)\r\n",(char *)strGroup,
					(char *)pCh->GetVitalStats());
			}
			else
			{
				strGroup.Format("%s%s\r\n",(char *)strGroup,
					(char *)pCh->GetVitalStats());
			}
		}
	}
}

/////////////////////////////////////
//	Split
//	split cash with members of group in the
//	same room
/////////////////////////////////////
void CCharacter::sGroup::Split(CCharacter *pCh, sMoney MoneyToSplit)
{
	if(m_pGroup!=NULL)
	{
		//Get number of group in same room.
		int nCount = 0;
		POSITION pos = m_pGroup->GetStartPosition();
		while(pos)
		{
			if(pCh->GetRoom()==m_pGroup->GetNext(pos)->GetRoom())
			{
				nCount++;
			}
		}
		//something very wrong if count is not greater than one
		assert(nCount>0);
		//don't do all this if there is only one
		//group member
		if(nCount>1)
		{
			//calculate money for each in the group
			sMoney GroupMoney;
			GroupMoney = (MoneyToSplit/nCount);
			if(GroupMoney.IsEmpty())
			{
				pCh->SendToChar("There isn't enough to go around.\r\n");
			}
			else
			{
				//give remainder to splitter
				sMoney MoneyToSpliter;
				MoneyToSpliter = (MoneyToSplit%nCount) + GroupMoney;

				CString strToGroup;
				strToGroup.Format("You receive your share of the booty: %s\r\n",
					(char *)GroupMoney.GetMoneyString());
				CString strToChar;
				strToChar.Format("You split some the booty with your group.\r\nYour share is: %s.\r\n",
					(char *)MoneyToSpliter.GetMoneyString());
			
				pos = m_pGroup->GetStartPosition();
				CCharacter *pTarget;
				while(pos)
				{
					pTarget = m_pGroup->GetNext(pos);
					if(pTarget->GetRoom()==pCh->GetRoom())
					{
						if(pCh!=pTarget)
						{
							//give them their share
							pTarget->m_CashOnHand+=GroupMoney;
							pTarget->SendToChar(strToGroup);
						}
						else
						{
							//give the spliter thier share
							//plus what ever didn't split evenly
							pTarget->m_CashOnHand+=MoneyToSpliter;
							pTarget->SendToChar(strToChar);
						}					
					}
				}
			}
		}
		else
		{
			//give all the cash back to them
			pCh->m_CashOnHand+=MoneyToSplit;
			pCh->SendToChar("There isn't anyone around to share in the booty.\r\n");
		}
	}
	else
	{
		ErrorLog << pCh->GetName() << " Is trying to split but isn't a member of a group!" << endl;
	}
}

////////////////////////////////
//	Give exp to group
//	Basically this stops power leveled twinks
//	for every 10 levels that you are lower than the
//	max level person in your group you lose 1/2 the
//	exp you normally would get.
//	this will probably need tweeked.
//	written by: Demetrius Comes
//////////////////////////////
#define LVLS_WITH_NO_EXP_PENTALITY 10
void CCharacter::sGroup::GroupGainExp(int nExp, CCharacter *pKiller, CCharacter *pDeadChar)
{
	static const char* pMsg = "You recieve your share of experience.\r\n";
	short nLvlDiff;
	if(pKiller->m_Group.IsGrouped())
	{
		short nMembers = 0;
		short nMaxGroupLvl=0;
		short nTotalLvls = 0;
		POSITION pos = m_pGroup->GetStartPosition();
		CCharacter *pCh;
		//count up group members in the room
		//and add total lvl of group
		while(pos)
		{
			pCh = m_pGroup->GetNext(pos);
			if(pKiller->GetRoom()==pCh->GetRoom())
			{
				nMaxGroupLvl = (nMaxGroupLvl < pCh->GetLevel()) ? pCh->GetLevel() : nMaxGroupLvl;
				nMembers++;
				nTotalLvls+=pCh->GetLevel();
			}
		}
		pos = m_pGroup->GetStartPosition();
		bool bWasFragged = false;
		//level exp bonus calcualted off of highest member in the group
		nLvlDiff = pDeadChar->GetLevel() - nMaxGroupLvl;
		//each LEVELS_PER_GROUP_KILL_EXP_BONUS levels above you, you get a 10% exp bonus
		if(nLvlDiff>0)
		{
			nLvlDiff = (nLvlDiff/LEVELS_PER_GROUP_KILL_EXP_BONUS)*10;
			nExp+= (nExp*nLvlDiff)/100;
		}
		else
		{
			nLvlDiff = (nLvlDiff/LEVELS_PER_GROUP_KILL_EXP_PENATLY)*10;
			//+= b/c nLvl is neg
			nExp += (nExp*nLvlDiff)/100;
			nExp = nExp < 0 ? 1 : nExp;
		}
		//nExp now has TOTAL exp for group
		while(pos)
		{
			pCh = m_pGroup->GetNext(pos);
			//if they are in the same room as the killer
			if(pKiller->GetRoom()==pCh->GetRoom())
			{
				//Find the pct the char is lvl wise to group
				short nPct = (pCh->GetLevel()*100)/nTotalLvls;
				//for every LVLS_WITH_NO_EXP we reduce % exp they get
				nPct = nPct/(((nMaxGroupLvl-pCh->GetLevel())/LVLS_WITH_NO_EXP_PENTALITY)+1);
				pCh->m_nExperience+= (nPct*nExp)/100;
				pCh->SendToChar(pMsg);
				if(pCh->EnoughExpForNextLevel())
				{
					pCh->AdvanceLevel(true,true);
				}
				//check for frag if you can't see name it's a frag
				if(pCh->IsKillFrag(pDeadChar))
				{
					bWasFragged = true;
					//if they are the killer they get 1 frag
					if(pCh==pKiller)
					{
						pKiller->m_fFrags+=(float)1.0;
					}
					//else they get part of the frag
					else
					{
						float fFrag = ((float)1/(float)nMembers);
						fFrag = fFrag<(float)0.1 ? (float)0.1 : fFrag;
#ifdef _DEBUG
						if(fFrag>(float)0.5)
						{
							ErrorLog << "fFrag is greater than 0.5 in group gain!" << endl;
						}
#endif
						pCh->m_fFrags+=fFrag;
					}
					//save um so frag list changes
					pCh->Save();
				}
			}
		}
		if(bWasFragged)
		{
			pDeadChar->m_fFrags-=(float)1.0;
			//save em
			pDeadChar->Save();
		}
	}
	else
	{
		nLvlDiff = pDeadChar->GetLevel() - pKiller->GetLevel();
		//each LEVELS_PER_KILL_EXP_BONUS levels above you, you get a 10% exp bonus
		if(nLvlDiff>0)
		{
			nLvlDiff = (nLvlDiff/LEVELS_PER_KILL_EXP_BONUS)*10;
			nExp+= (nExp*nLvlDiff)/100;
		}
		else
		{
			nLvlDiff = (nLvlDiff/LEVELS_PER_KILL_EXP_PENATLY)*10;
			//+= b/c nLvl is neg
			nExp += (nExp*nLvlDiff)/100;
			nExp = nExp < 0 ? 1 : nExp;
		}
		pKiller->m_nExperience+=nExp;
		pKiller->SendToChar(pMsg);
		if(pKiller->EnoughExpForNextLevel())
		{
			pKiller->AdvanceLevel(true,true);
		}
		if(pKiller->IsKillFrag(pDeadChar))
		{
			//bonus for solo p-kill
			pKiller->m_fFrags+=(float)1.5;
			pDeadChar->m_fFrags-=(float)1.0;
			//save them so frag list changes
			pKiller->Save();
			pDeadChar->Save();
		}
	}
}

void CCharacter::sFollower::SetCharToFollow(CCharacter *pCh) 
{
	m_pCharacterBeingFollowed = pCh;
}

void CCharacter::sFollower::Remove(CCharacter *pCh) 
{
	CString strToChar;
	//send message to pch
	strToChar.Format("You stop following %s.\r\n",
		(char *)pCh->m_Follow.m_pCharacterBeingFollowed->GetRaceOrName(pCh));
	pCh->SendToChar(strToChar);
	//send mesg to who they are following
	strToChar.Format("%s stops following you.\r\n",
		(char *)pCh->GetRaceOrName(pCh->m_Follow.m_pCharacterBeingFollowed));
	pCh->m_Follow.m_pCharacterBeingFollowed->SendToChar(strToChar);
	//remove them from followersLL
	m_Followers.Remove(pCh);
	//the who they are following to null
	pCh->m_Follow.SetCharToFollow(NULL);
}


///////////////////////////////////////////
//sMAster

const short CCharacter::sMaster::MASTERED_OFF=-1;
void CCharacter::sMaster::SetMaster(CCharacter *pMaster, CCharacter *pMasterie,int nTime, bool bMakeFollow) 
{
	if(pMaster!=NULL)
	{
		m_pMaster = pMaster;
		pMaster->m_Master.Add(pMasterie);
		m_nTimeIAmMastered=nTime;
		if(bMakeFollow)
		{
			CString strCommand("follow ");
			strCommand+=m_pMaster->IsNPC() ? m_pMaster->GetAlias().GetWordAfter(0) : m_pMaster->GetName();
			pMasterie->AddCommand(strCommand);
		}
	}
	else
	{
		m_nTimeIAmMastered = MASTERED_OFF;
		pMasterie->DoBreakMastersHold();
		m_pMaster = NULL;
	}
}

////////////////////////////
//	Reduce Masters hold
//	called from reducechar affects
//	to see if it is time to break that hold
void CCharacter::sMaster::ReduceMastersHold(int nTime, CCharacter *pOwner)
{
	if(m_nTimeIAmMastered!=MASTERED_OFF)
	{
		m_nTimeIAmMastered-=nTime;
		if(m_nTimeIAmMastered<0)
		{
			pOwner->GetMaster()->m_Master.Remove(pOwner);
		}
	}
}

void CCharacter::sMaster::Remove(CCharacter *pCh)
{
	CString strToChar;
	strToChar.Format("You are free of %s's hold.\r\n",
		(char *)pCh->m_Master.GetMaster()->GetRaceOrName(pCh));
	pCh->SendToChar(strToChar);
	strToChar.Format("You lose your hold on %s.\r\n",
		(char *)pCh->GetRaceOrName(pCh->GetMaster()));
	pCh->GetMaster()->SendToChar(strToChar);
	pCh->SetMaster(NULL,MASTERED_OFF);
	POSITION pos = NULL;
	m_MasterOf.Remove(pCh,pos);
}

///////////////////////////////////
//	Clean up pointers and send messages
//	basically duplicates remove except
//	we actually have to postion
void CCharacter::sMaster::CleanUp(CCharacter *pOwner)
{
	if(HasMaster())
	{
		GetMaster()->m_Master.Remove(pOwner);
	}

	POSITION pos = m_MasterOf.GetStartPosition();
	CCharacter *pCh;
	CString strToChar;
	while(pos)
	{
		pCh = m_MasterOf.GetNext(pos);
		strToChar.Format("You are free of %s's hold.\r\n",
			(char *)pCh->m_Master.m_pMaster->GetRaceOrName(pCh));
		pCh->SendToChar(strToChar);
		pCh->SetMaster(NULL,MASTERED_OFF);
		m_MasterOf.Remove(pCh,pos);
	}
}

////////////////////////////////
// Follower functions
//  clean up pointers and send messages
void CCharacter::sFollower::CleanUp(CCharacter *pOwner)
{
	if(IsFollowing())
	{
		m_pCharacterBeingFollowed->m_Follow.Remove(pOwner);
	}

	CString strToChar;
	CCharacter *pFollower;
	POSITION pos = m_Followers.GetStartPosition();
	while(pos)
	{
		pFollower = m_Followers.GetNext(pos);
		strToChar.Format("You stop following %s.\r\n",
			(char *)(pFollower->IsNPC() ? pFollower->GetName() : pFollower->m_Follow.m_pCharacterBeingFollowed->GetRaceOrName(pFollower)));
		pFollower->SendToChar(strToChar);
		pFollower->m_Follow.m_pCharacterBeingFollowed = NULL;
	}
}

///////////////////////
//	When the player logs or dies
//	tell all the people that consent this player
//	That you no longer are in the game
void CCharacter::sConsent::CleanUp(CCharacter *pOwner)
{
	if(GetConsented())
	{
		GetConsented()->m_Consent.m_CharsThatConsentedYou.Remove(pOwner);
	}
	POSITION pos = this->m_CharsThatConsentedYou.GetStartPosition();
	CCharacter *pCh;
	while(pos)
	{
		pCh = m_CharsThatConsentedYou.GetNext(pos);
		pCh->SendToChar("The person you consent has left the game.\r\n");
		pCh->m_Consent.m_pCharYouConsented = NULL;
	}
}


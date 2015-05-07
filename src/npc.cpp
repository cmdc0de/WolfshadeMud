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
////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
// NPC.cpp: implementation of the CNPC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "npc.h"

extern CRandom GenNum;
extern CGlobalVariableManagement GVM;
const int CNPC::DO_AI = CMudTime::PULSES_PER_REAL_SECOND/2;
const short CNPC::SMART_SCAVENGER_PCT = 30;
const short CNPC::SCAVENGER_PCT = 15;
const short CNPC::MAX_PCT_SCAVENGE = 65; //%
const short CNPC::MAX_MOB_SKILL = 90;
const int CNPC::PULSES_PER_DO_MOB_MOVEMENT=CMudTime::PULSES_PER_REAL_SECOND*2;
short CNPC::PCT_MOB_MOVEMENT = 10;
const int CNPC::SENTINEL	=(1<<0);
const int CNPC::SCAVENGER	=(1<<1);
const int CNPC::AWARE		=(1<<2);
const int CNPC::AGGRESSIVE	=(1<<3);
const int CNPC::STAYZONE	=(1<<4);
const int CNPC::FLEEING		=(1<<5);
const int CNPC::AGGROEVIL	=(1<<6);
const int CNPC::AGGROGOOD	=(1<<7);
const int CNPC::AGGRONEUTRAL= (1<<8);
const int CNPC::MEMORY		=(1<<9);
const int CNPC::AGGROEVILRACE =(1<<10);
const int CNPC::AGGROGOODRACE =(1<<11);
const int CNPC::AGGRONEUTRALRACE= (1<<12);
const int CNPC::AGGROALIENRACE =(1<<13);
const int CNPC::NOBASH		=	(1<<14);
const int CNPC::TRACK		=	(1<<15);
const int CNPC::TEACHER		=	(1<<16);
const int CNPC::GUARD_GOOD 	=	(1<<17);  	//will assist good alignment
const int CNPC::GUARD_EVIL	=	(1<<18);	//will assist evil alignment
const int CNPC::GUARD_NEUTRAL=	(1<<19);	//will assist nuetral alignment
const int CNPC::GUARD_NPC	=	(1<<20);	//will assist npc
const int CNPC::GUARD_CHARISMA=	(1<<21);	//will assist character with higher charisma
const int CNPC::GUARD_RACE	=	(1<<22); 	//will assist character of same race
const int CNPC::GUARD_GOOD_RACE=	(1<<23); 	//will assist good side of race war
const int CNPC::GUARD_EVIL_RACE	=(1<<24); 	//will assist evil side of race war
const int CNPC::GUARD_NEUTRAL_RACE=	(1<<25); 	//will assist neutral side of race war
const int CNPC::GUARD_ALIEN_RACE=	(1<<26); 	//will assist alien side of race ware
const int CNPC::PERM_DI = (1<<27);
const int CNPC::AGGRO_MOB =(AGGRESSIVE | AGGROEVIL | AGGROGOOD | AGGRONEUTRAL | AGGROEVILRACE | AGGROGOODRACE | AGGRONEUTRALRACE);
const int CNPC::GUARD =(GUARD_GOOD | GUARD_EVIL | GUARD_NEUTRAL | GUARD_NPC | GUARD_CHARISMA | GUARD_RACE | GUARD_GOOD_RACE | GUARD_EVIL_RACE | GUARD_NEUTRAL_RACE | GUARD_ALIEN_RACE);
const int CNPC::COLISEUM_TIME = 15 * CMudTime::PULSES_PER_REAL_MIN; //Coliseum goes off every 15 mins

//TODO make static const
#define MIN_HIT_POINTS_FOR_NPC_AGGRO 15

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNPC::CNPC(CMobPrototype &mob, CRoom *pPutInRoom)
	:CCharacter(mob,pPutInRoom)
{
	m_pSwitched = NULL;
	m_nVNum = mob.GetVnum();
	m_nThaco = mob.GetThaco();
	m_nExtraExp = mob.GetExperience();
	m_strDefaultPosDesc = mob.GetDefaultPosDesc();
	m_nDefaultPos = mob.GetDefaultPos();
	m_pDescriptor = new sDescriptor(STATE_PLAYING);
	m_pTrackInfo = new CNPCTrackInfo();
}

CNPC::~CNPC()
{
	delete m_pDescriptor;
	//must set descriptor = NULL because the varaible
	//is located in the CCharacter put deleted here
	//b/c mobs get a different descriptor than do pc's
	m_pDescriptor = NULL;
}

//////////////////////////////////////
//  virtual override
void CNPC::CleanUp()
{
	if(m_pSwitched!=NULL)
	{
		m_pSwitched->DoReturn();
		m_pSwitched = NULL;
	}
	CCharacter::CleanUp();
}


bool CNPC::IsNPC() const
{
	return true;
}

//////////////////////////////////////
//	Adds the character to the npc's memory
//	if the mob prefers memory
//	if the mob prefers track we'll add the attacker
//	to the track target list as well
void CNPC::AddMemory(CCharacter *pFighting)
{
	assert(pFighting);
	//if this asserts it is being called at the wrong place!
	//should only be called from assertFighting()
	CCharacter *pCh = pFighting->m_Master.HasMaster() ? pFighting->m_Master.GetMaster() : pFighting;
	long lPcNum = pCh->GetVnum();
	//NPC's will not remember or Track NPC's
	if(!pCh->IsNPC())
	{
		if(Prefers(MEMORY) 
		&& (m_Memory.IsEmpty() || 
		!m_Memory.DoesContain(lPcNum)))
		{
			m_Memory.Add(lPcNum);
		}
		if(Prefers(TRACK))
		{
			m_pTrackInfo->AddTrackTarget(pCh,this);
		}
	}
}
/////////////////////////////////
//	If we are switched send messages to person switched to
//////////////////////////////////
void CNPC::SendToChar(char * strMsg,bool bColorize) const
{
	this->SendToChar(CString(strMsg),bColorize);
}
/////////////////////////////////
//	If we are switched send messages to person switched to
//	if we are switched and we are not link dead
//	we send the string... check to see if the pc that
//	is switched to this npc is being spied if so...send
//	info to spier.
//	Why not just use CCharacter::SendToChar...
//	Then we can't use tog PREFERS_TELLS_WHEN_SWITCHED
//////////////////////////////////
void CNPC::SendToChar(CString strMsg,bool bColorize) const
{
	if(m_pSwitched!=NULL)
	{
		if(!IsLinkDead())
		{
			if(bColorize)
			{
				strMsg.Colorize(m_pSwitched->Prefers(PREFERS_COLOR));
			}
			m_pSwitched->SendToSocket(strMsg);
			if(m_pSwitched->IsBeingSpied())
			{
				CString strToChar;
				strToChar.Format("&WSpy Info of %s.&n\r\n",
					(char *)GetName());
				m_pSwitched->m_pSpiedby->SendToChar(strToChar);
				m_pSwitched->m_pSpiedby->SendToChar(strMsg,false); //no sense in colorizing it twice!
				m_pSwitched->m_pSpiedby->SendToChar(CString("&WEnd spy info*****&n\r\n"));
			}
		}
	}
}

////////////////////////////////
//	NPC calculations for thaco etc
///////////////////////////////
short int CNPC::CalcACMobWouldHit()
{
	short int nRetVal = (m_nThaco - m_nToHit) - DIE(100);
	if(DIE(100)<= GetSkill(SKILL_OFFENSE))
	{
		//so we might even subtract another 10
		nRetVal-=GetSkill(SKILL_OFFENSE)/4;
	}
	else
	{
		SkillUp(SKILL_OFFENSE);
	}
	return (nRetVal < -100 ? -100 : nRetVal > 100 ? 100 : nRetVal); 
}

CString CNPC::GetDefaultPosDesc()
{
	return m_strDefaultPosDesc;
}


///////////////////////////////////
//	Here is where the mob does basic mob ai
//	Aggro, memory, scarvenger
//  return after each if so we insure
//	we never do two pieces of AI in the same turn
//	that would be unfair! =(
//	written by: Demetrius Comes
void CNPC::DoAI(const CInterp *pInterp)
{
	bool bGeneralCheck = (!IsLagged() && CommandBufEmpty());
	if(bGeneralCheck)
	{
		if(m_Master.HasMaster() && m_Master.GetMaster()->IsFighting())
		{
			DoHelpMaster(pInterp);
		}
		else if(!IsFighting() && !this->HasMaster())
		{
			DoColiseumAI();
			DoWCMilitiaAI();
			DoSwampMohrAI();
			DoKipuAI();
			if(Prefers(MEMORY))
			{
				if(DoMemoryAI())
				{
					return;
				}
			}
			if(Prefers(AGGRO_MOB) || Prefers(GUARD))
			{
				if(CheckForAggroAndGuarding(pInterp))
				{
					return;
				}
			}
			if(this->m_nMaxHpts!=this->m_nCurrentHpts
				&& !InPosition(POS_KOED)
				&& !InPosition(POS_INCAP)
				&& !InPosition(POS_STUNNED)
				&& !IsAffectedBy(AFFECT_NO_BANDAGE_TIME))
			{
				AddCommand("first aid");
				return;
			}
			if(!m_pInRoom->IsAffectedBy(CRoom::ROOM_AFFECT_NO_SCAVENGE)
				&& !InPosition(POS_SLEEPING) && !InPosition(POS_RECLINING)
				&& !InPosition(POS_INCAP) && !InPosition(POS_KOED)
				&& Prefers(SCAVENGER) && (DIE(100)<=SCAVENGER_PCT))
			{
				CObject *pObj;
				if(DIE(100)<=SMART_SCAVENGER_PCT)
				{
					pObj = m_pInRoom->GetMostValuableObject();
				}
				else
				{
					pObj = m_pInRoom->GetRandomObject();
				}
				//if we have an object and we are not 
				//carring more than 65% of our max wieght...
				//we can scavange
				if(pObj!=NULL && 
					(m_nWeightCarried < (m_pAttributes->GetMaxWeight()*MAX_PCT_SCAVENGE)/100))
				{
					try
					{
						GetSingleItem(pObj);
						m_pInRoom->Remove(pObj);
						CString str;
						str.Format("%s %s",
							pObj->IsWeapon() ? "wield" : "wear",
							(char *)pObj->GetAlias().GetWordAfter(0));
						AddCommand(str);
						return;
					}
					catch(CError *pError)
					{
						SendToChar(CString(pError->GetMessage()));
						return;
					}
				}
			}
		}
		else if(IsFighting())
		{
			if(!InPosition(POS_STANDING))
			{
				AddCommand(CString("stand"));
				return;
			}
			DoColiseumFightingAI();
			DoSwampMohrBattleAI();
		}
	}
}

//////////////////////////////
//	Check to see if I am aggro to the character
//	passed in
bool CNPC::IsAggro(CCharacter * pCh)
{
	if((!pCh->IsNPC() && pCh->Prefers(PREFERS_NO_HASSLE))
		|| pCh->IsDead() || pCh==this
		|| (m_Master.IsAMaster() && m_Master.IsMasterOf(pCh))
		|| GetMaster()==pCh)
	{
		return false;
	}
	bool bRetVal = false;
	if(Prefers(AGGRO_MOB))
	{
		//aggressive mobs are not aggressive to 
		//other mobs but are to PC's!
		if(!pCh->IsNPC() && Prefers(AGGRESSIVE))
		{
			bRetVal = true;
		}
		if(Prefers(AGGROEVIL) && pCh->IsEvil())
		{
			bRetVal = true;
		}
		else if(Prefers(AGGROGOOD) && pCh->IsGood())
		{
			bRetVal = true;
		}
		else if(Prefers(AGGRONEUTRAL) && pCh->IsNeutral())
		{
			bRetVal = true;
		}
		else if(Prefers(AGGROEVILRACE) && pCh->IsRace(CRacialReference::RACE_EVIL))
		{
			bRetVal = true;
		}
		else if(Prefers(AGGROGOODRACE) && pCh->IsRace(CRacialReference::RACE_GOODIES))
		{
			bRetVal = true;
		}
		else if(Prefers(AGGRONEUTRALRACE) && pCh->IsRace(CRacialReference::RACE_NEUTRAL))
		{
			bRetVal = true;
		}
		else if(Prefers(AGGROALIENRACE) && pCh->IsRace(CRacialReference::RACE_ALIEN))
		{
			bRetVal = true;
		}
	}
	return bRetVal;
}

//////////////////////////////
//	If Character in memory remove them
void CNPC::RemoveFromMemory(CCharacter *pCh)
{
	if(!m_Memory.IsEmpty())
	{
		long tmp = pCh->GetVnum();
		POSITION pos = NULL;
		m_Memory.Remove(tmp,pos);
	}
	//Remove them from Track memory as well
	((CNPCTrackInfo *)m_pTrackInfo)->RemoveTrackTargetPerm(pCh,this);
}

/////////////////////////////////////
//	Called m_pSwitched return function
void CNPC::DoReturn()
{
	//had to do this b/c if a player orders there followers return
	if(m_pSwitched!=NULL)
	{
		m_pSwitched->DoReturn();
	}
}

////////////////////////////////////
//	Same as characters except we have
//	the NPC type togs
void CNPC::DoTog()
{
	static const char *strAnswer[] = {"No","Yes"};
	static const char *strTog = 
"				NPC Togs:\r\n" 
"Sentinel: %s          \tScavenger: %s        \tAware: %s\r\n" 
"Aggressive: %s        \tStayZone: %s         \tFleeing: %s\r\n"
"AggroEvil: %s         \tAggroGood: %s        \tAggroNeutral: %s\r\n"
"Memory: %s            \tAggro Evil Race: %s  \tAggo Good Race: %s\r\n"
"Aggro Neutral Race: %s\tAggro Neutral Race %s\tNo Bash: %s\r\n"
"Track %s\tWimpy:  %d\tTeacher: %s\r\n"
"Perm DI: %s\r\n";

	CString strToNPC;
	strToNPC.Format(strTog,strAnswer[Prefers(SENTINEL)],
		strAnswer[Prefers(SCAVENGER)],
		strAnswer[Prefers(AWARE)],
		strAnswer[Prefers(AGGRESSIVE)],
		strAnswer[Prefers(STAYZONE)],
		strAnswer[Prefers(FLEEING)],
		strAnswer[Prefers(AGGROEVIL)],
		strAnswer[Prefers(AGGROGOOD)],
		strAnswer[Prefers(AGGRONEUTRAL)],
		strAnswer[Prefers(MEMORY)],
		strAnswer[Prefers(AGGROEVILRACE)],
		strAnswer[Prefers(AGGROGOODRACE)],
		strAnswer[Prefers(AGGRONEUTRALRACE)],
		strAnswer[Prefers(AGGROALIENRACE)],
		strAnswer[Prefers(NOBASH)],
		strAnswer[Prefers(TRACK)],
		m_nWimpy,
		strAnswer[Prefers(TEACHER)],
		strAnswer[Prefers(PERM_DI)]);
	SendToChar(strToNPC);
}

void CNPC::DoAttributes()
{
	static const char * strFormat =
	"	            &Y--==Attributes for &W%s&Y==--\r\n" 
	"&WRace:   &L%s	    &WAge: &L%d          &W Class: %s \r\n"
	"&WLevel:  &g%d      &WHit Points: &g%d/%d &W Mana Points: &R%d/%d\r\n"
	"&WMovement Points: %d/%d  Height: %d   Weight: %d\r\n"
	"&mSTR:&M%4d  &mAGI:&M%4d  &mLCK:&M%4d \r\n"
	"&mINT:&M%4d  &mWIS:&M%4d  &mCHA:&M%4d \r\n"
	"&mCON:&M%4d  &mPOW:&M%4d  &mDEX:&M%4d \r\n"
	"&nSaving Throws: %s \r\n"
	"&bAC: &B%d &n     HitRoll: %d     DamRoll: %d \r\n"
	"&WAli&wgnm&Lent: %d &n\r\n"
	"Weight Carried: %d \r\n"
	"Experience I have: %d\r\n"
	"Extra Exp given if you kill me: %d\r\n"
	"Worshipping: %s\r\n"
	"Thaco: %d  Vnum: %d\r\n";

	CString strToChar;
	CString strSaves;
	m_pAttributes->SaveAttribute(strSaves);
	strToChar.Format(strFormat,(char *)m_strName,GVM.GetColorRaceName(m_pAttributes->GetRace()),
		m_pAttributes->CalcAge(),GVM.GetColorClass(m_pAttributes->GetClass()),		
		m_nLevel,m_nCurrentHpts,m_nMaxHpts,m_nManaPts,m_nMaxManaPts,
		m_nMovePts,m_nMaxMovePts,m_pAttributes->GetHeight(),
		m_pAttributes->GetWeight(),m_pAttributes->GetStat(STR),
		m_pAttributes->GetStat(AGI),
		m_pAttributes->GetStat(LUCK),m_pAttributes->GetStat(INT),
		m_pAttributes->GetStat(WIS),m_pAttributes->GetStat(CHA),
		m_pAttributes->GetStat(CON),m_pAttributes->GetStat(POW),
		m_pAttributes->GetStat(DEX),(char *)strSaves,
		CalcAC(NULL),m_nToHit,m_nDamRoll,m_nAlignment,
		m_nWeightCarried,m_nExperience,m_nExtraExp,GVM.GetColorGod(GetGod()),
		m_nThaco, m_nVNum);
	
	SendToChar(strToChar);
}

/////////////////////////////////////////////
//	Mobs have to be flaged as teachers 
//	
skill_type CNPC::CanTeach(const CCharacter * pCharToBeTaught, short nSkill,bool bIsSkill) const
{
	skill_type bRetVal = 0;
	CString strToChar;
	if(IsTeacher())
	{
		//you can't learn from someone you can't see there
		//name -- eg you can't be a drow elf and learn from a pixie
		if(pCharToBeTaught->CanSeeName(this) && pCharToBeTaught->CanSeeChar(this))
		{
			if(IsGoodEnoughToTeach(pCharToBeTaught,nSkill,bIsSkill))
			{
				bRetVal =  (((GetSkill(nSkill) - pCharToBeTaught->GetSkill(nSkill))>>2)+1);
			}
			else
			{
				strToChar.Format("%s doesn't know that well enought to teach you.\r\n",
					(char *)this->GetRaceOrName(pCharToBeTaught));
				pCharToBeTaught->SendToChar(strToChar);
			}
		}
		else
		{
			pCharToBeTaught->SendToChar("You have to be able to see your teacher, and thier friend!\r\n");
		}
	}
	else
	{
		strToChar.Format("%s tells you 'Do I look like I want to teach you anything!'\r\n",
			(char *)this->GetRaceOrName(pCharToBeTaught));
		pCharToBeTaught->SendToChar(strToChar);
	}
	return bRetVal;
}

//////////////////////////////////////////////////
//	Gets the cost to train from a mob
//
sMoney CNPC::GetTrainingCosts(const CCharacter * pToBeTrained, short nSkill, bool bIsSkill) const
{
	skill_type nSkillDiff;
	if(bIsSkill)
	{
		nSkillDiff = GetSkill(nSkill) - pToBeTrained->GetSkill(nSkill);
	}
	else
	{
		nSkillDiff = GetSkill(nSkill) - pToBeTrained->GetSkill(nSkill);
	}

	sMoney Cost(1);
	//int nDen = (MAX_MOB_SKILL/(MAX_MOB_SKILL/((nSkillDiff/3)+1)));
	//Cost = (base_money)((nSkillDiff*sMoney::PLAT_IN_BASE)/nDen);
	//TODO return Cost;
	return Cost;
}

///////////////////////////////
//	Virtual:
//	Default would be just to assist
//	warriors might rescue or bodyslam
void CNPC::DoHelpMaster(const CInterp *pInterp)
{
#ifdef _DEBUG
	assert(m_Master.GetMaster());
#endif
	if(this->GetRoom()->DoesContain(m_Master.GetMaster()))
	{
		//we always try to rescue our master if he is the tank
		if(m_Master.GetMaster()->IsFighting()->IsFighting()==m_Master.GetMaster())
		{
			if(GetSkill(SKILL_RESCUE)>0 &&
				DIE(100) < GetSkill(SKILL_RESCUE))
			{
				DoRescue(m_Master.GetMaster());
				//if we are still not fighting
				//aka we failed the rescue
				if(IsFighting()==NULL)
				{
					DoAssist(m_Master.GetMaster());
				}
			}
		}
		else if(IsFighting()==NULL)
		{
			//if your master is NOT the tank and you can bodyslam try to 
			DoAIAttack(pInterp,m_Master.GetMaster()->IsFighting());
		}
	}
}

////////////////////////////////////////
//	Over ridden Advance level
//	so we can set some preferences like wimpy for
//	fleeing mobs
///////////////////////////////////////
int CNPC::AdvanceLevel(bool bIncreaseLevel,bool bSendMsg,bool bIsFollower)
{
	int nHits = m_pAttributes->GetConHitBonus() + 
		(bIsFollower ? DIE(20) : DIE(this->GetMaxHitPointsPerLevel()));
	int nMana = (this->GetManaAdjust() + 1)*2;

	if(bIncreaseLevel)
	{
		m_nLevel++;
		m_nCurrentHpts += nHits;
		m_nMaxHpts += nHits;
		m_nMaxManaPts += nMana;
		m_nManaPts += nMana;
	}
	else
	{
		nHits = CCharacter::AdvanceLevel(bIncreaseLevel,bSendMsg,bIsFollower);
	}
	if((this->GetExp(m_nLevel) > m_nExperience))
	{
		m_nExperience = this->GetExp(m_nLevel);
	}
	if(Prefers(FLEEING))
	{
		m_nWimpy = (m_nMaxHpts*(DIE(5)+15))/100;
		//wimpy will be set at 16-21% of max hit points
	}
	else
	{
		m_nWimpy = 0;
	}
	return nHits;
}

/////////////////////////////////////////
//	Track for NPC's
//	here we check to see if mob is tracking anyone
//	if not we check pulses and mob mob if we need to
//	Call the NPC::MoveMob function
void CNPC::Track()
{
	//Add our track targets that have rented if need be
	((CNPCTrackInfo *)m_pTrackInfo)->AddRentedTargets(this);

	if(!InPosition(POS_FIGHTING) 
		&& !IsFighting()
		&& !InPosition(POS_DEAD)
		&& !InPosition(POS_KOED)
		&& !InPosition(POS_STUNNED)
		&& !InPosition(POS_MORTALED)
		&& !InPosition(POS_INCAP)
		&& InPosition(POS_STANDING)
		&& !m_pTrackInfo->m_TrackList.IsEmpty()
		//they must have at least 1/4 hits to track
		&& (m_nCurrentHpts>(m_nMaxHpts>>2)))
	{
		int nTrackPulse;
		//the better their skill the faster they track
		//and we give mobs a slight bonus here too
		if(GetSkill(SKILL_TRACK)<=25)
			nTrackPulse = CMudTime::PULSES_PER_MUD_HOUR/15;
		else if(GetSkill(SKILL_TRACK)<=50)
			nTrackPulse = CMudTime::PULSES_PER_MUD_HOUR/20;
		else if(GetSkill(SKILL_TRACK)<=75)
			nTrackPulse = CMudTime::PULSES_PER_MUD_HOUR/30;
		else
			nTrackPulse = CMudTime::PULSES_PER_MUD_HOUR/60;
		bool bFoundChar = false;
		//Is it time?
		if(!(CMudTime::GetCurrentPulse()%nTrackPulse))
		{
			POSITION pos = m_pTrackInfo->m_TrackList.GetStartPosition();
			CCharacter *pCh;
			CString strToChar;
			while(pos)
			{
				pCh = m_pTrackInfo->m_TrackList.GetNext(pos);
				if(!GetRoom()->DoesContain(pCh))
				{
					if(DIE(100)<=m_Skills[SKILL_TRACK])
					{
						short nDir;
						CLinkList<CRoom> UsedLL;
						if((nDir = GetRoom()->LocateChar(pCh,UsedLL,GetSkill(SKILL_TRACK)>>1))!=NOWHERE)
						{
							CRoom *pToRoom = GetRoom()->GetRoomToThe(nDir);
							assert(pToRoom);
							//should we do this for pc's or just npcs?
							if(!GetRoom()->IsDoorOpen(nDir))
							{
								GetRoom()->OpenDoor(nDir,this,true);
							}
							DoMove(CRoom::strDir[nDir]);
							//don't keep looking for next char
							pos = NULL;
						}
					}
					else
					{
						SkillUp(SKILL_TRACK);
					}
				}
			}
			if(!bFoundChar)
			{
				MoveMob();
			}
		}
	}
	else if(!InPosition(POS_FIGHTING) 
		&& !IsFighting()
		&& !InPosition(POS_DEAD)
		&& !InPosition(POS_KOED)
		&& !InPosition(POS_STUNNED)
		&& !InPosition(POS_MORTALED)
		&& !InPosition(POS_INCAP)
		&& InPosition(POS_STANDING))
	{
		MoveMob();
	}
}

///////////////////////
//	Mobs any character but is basically
//	set up only to be called for mobs
//  Assume we are not tracking b/c this function
//	is only called by the track function
//////////////////////
void CNPC::MoveMob()
{
	if((!(CMudTime::GetCurrentPulse()%PULSES_PER_DO_MOB_MOVEMENT)) 
		&& DIE(100)<PCT_MOB_MOVEMENT)
	{
		if(IsTeacher())
		{
			if(DIE(100)>40)
			{
				return;
			}
		}
		if((!(Prefers(SENTINEL))) && !m_Master.HasMaster())
		{
			if(!InPosition(POS_FIGHTING) && InPosition(POS_STANDING))
			{
				CString strRev;
				CString strDir(CRoom::strDir[DIE(NUM_OF_DOORS)-1]);
				CRoom *pMoveRoom; 
				if((pMoveRoom=GetRoom()->CanEnter(this,strDir,strRev))!=NULL)
				{
					if(Prefers(STAYZONE))
					{
						if(pMoveRoom->IsZone(m_pInRoom->GetZone()))
						{
							AddCommand(strDir);
						}
					}
					else 
					{
						AddCommand(strDir);
					}
				}
			}
		}
	}
}

////////////////////////////////
//	Check to see if a player that has attacked 
// me before is in room
bool CNPC::DoMemoryAI()
{
	CString str;
	POSITION pos;
	CCharacter *pCh;
	if(!m_Memory.IsEmpty())
	{
		pos = m_pInRoom->GetFirstCharacterInRoom();
		while(pos)
		{
			pCh=m_pInRoom->GetNextCharacter(pos);
			if(!pCh->IsNPC() && !pCh->Prefers(PREFERS_NO_HASSLE))
			{
				if(m_Memory.DoesContain(pCh->GetVnum()))
				{
					if(!InPosition(POS_STANDING))
					{
						AddCommand(CString("stand"));
					}
					//I don't like using add command here but in this 
					//case it is ok since we are only dealing with PC's
					str.Format("kill %s",(char *)pCh->GetName());
					if(m_Master.IsAMaster())
					{
						CString strFollow("order followers");
						strFollow+=str;
						AddCommand(strFollow);
					}
					AddCommand(str);
					return true;
				}
			}
		}
	}
	return false;
}

//////////////////////////////////////
//	return true if we have taken an action in here
//	we check each character in the room and see if we are aggro
//	or do we need to assist them (guarding)
//	 Both in one function so we only run the chars in room LL once!
bool CNPC::CheckForAggroAndGuarding(const CInterp *pInterp)
{
	POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
	CCharacter *pCh;
	while(pos)
	{
		pCh=m_pInRoom->GetNextCharacter(pos);
		if(this->CanSeeChar(pCh) && this->CanSeeInRoom(pCh->GetRoom())
			&& ((m_nCurrentHpts*100/m_nMaxHpts)>MIN_HIT_POINTS_FOR_NPC_AGGRO))
		{
			if(Prefers(AGGRO_MOB) && IsAggro(pCh))
			{
				//if we are a master and our followers
				//are in this room tell them to attack first
				if(m_Master.IsAMaster())
				{
					POSITION posFol = m_Master.m_MasterOf.GetStartPosition();
					CCharacter *pFol;
					while(posFol)
					{
						pFol = m_Master.m_MasterOf.GetNext(posFol);
						if(!pFol->IsFighting()
							&& pFol->CanSeeChar(pCh)
							&& pFol->CanSeeInRoom(pCh->GetRoom())!=SEES_NOTHING 
							&& pFol->GetRoom()==this->GetRoom())
						{
							pFol->DoKill(pInterp,pCh);
							if(pCh->IsDead())
							{
								//if he's dead return
								//we the master doesn't try to attack
								//and crash the game b/c the character
								//is already dead
								return true;
							}
						}
					}
					//it is possible to have one NPC attack
					//another so we can't use addcommand because
					//with pc's I have unique names to work with here NPC's I don't
					if(!InPosition(POS_STANDING))
					{
						AddCommand(CString("stand"));
					}	
					else
					{
						this->DoAIAttack(pInterp,pCh);
						if(pCh->IsDead())
						{
								return true;
						}
					}
					//we know we have one to attack
					//we only attack one at a time
					//and we should do anymore AI
					return true;
				}
				else
				{
					this->DoAIAttack(pInterp,pCh);
					return true;
				}
			} //end if aggro
			else if(Prefers(GUARD) && pCh->IsFighting())
			{
				if(Prefers(GUARD_NPC))
				{
					if(pCh->IsNPC() && !pCh->IsFighting()->IsNPC())
					{
						DoAssist(pCh);
						return true;
					}
				}
				if(Prefers(GUARD_RACE))
				{
					if(GetRace()==pCh->GetRace()
						&& pCh->IsFighting()->GetRace()!=GetRace())
					{
						DoAssist(pCh);
						return true;
					}
				}
				if(Prefers(GUARD_GOOD_RACE))
				{
					if(pCh->IsRace(CRacialReference::RACE_GOODIES)
						&& !pCh->IsFighting()->IsRace(CRacialReference::RACE_GOODIES))
					{
						DoAssist(pCh);
						return true;
					}
				}
				if(Prefers(GUARD_EVIL_RACE))
				{
					if(pCh->IsRace(CRacialReference::RACE_EVIL)
						&& !pCh->IsFighting()->IsRace(CRacialReference::RACE_EVIL))
					{
						DoAssist(pCh);
						return true;
					}
				}	
				if(Prefers(GUARD_NEUTRAL_RACE))
				{
					if(pCh->IsRace(CRacialReference::RACE_NEUTRAL)
						&& !pCh->IsFighting()->IsRace(CRacialReference::RACE_NEUTRAL))
					{	
						DoAssist(pCh);
						return true;
					}
				}	
				if(Prefers(GUARD_ALIEN_RACE))
				{
					if(pCh->IsRace(CRacialReference::RACE_ALIEN)
						&& !pCh->IsFighting()->IsRace(CRacialReference::RACE_ALIEN))
					{
						DoAssist(pCh);
						return true;
					}
				}
				if(Prefers(GUARD_CHARISMA))
				{
					if(pCh->GetCharisma() > pCh->IsFighting()->GetCharisma())
					{
						DoAssist(pCh);
						return true;
					}
				}
				if(Prefers(GUARD_GOOD))
				{
					if(pCh->IsGood() && !pCh->IsFighting()->IsGood())
					{
						DoAssist(pCh);
						return true;
					}
				}
				if(Prefers(GUARD_EVIL))
				{
					if(pCh->IsEvil() && !pCh->IsFighting()->IsEvil())
					{
						DoAssist(pCh);
						return true;
					}
				}
				if(Prefers(GUARD_NEUTRAL))
				{
					if(pCh->IsNeutral() && !pCh->IsFighting()->IsNeutral())
					{
						DoAssist(pCh);
						return true;
					}
				}
			} //end if guard
		} //end if can see char
	} //end while
	return false;
}

///////////////////////////////
//  CalcDamage()
//  Checks for a weapon 
//  if no weapon uses bare hand damage
//  other wise weapon damage
//  same as pc's except mobs get barehand
//	damage added to weapon damage
//  written by: Demetrius Comes
//  5/11/98
////////////////////////////////
short int CNPC::CalcDamage(short nPos,CCharacter *pDefender)
{
    if(m_Equipment[nPos].IsEmpty()
		|| !m_Equipment[nPos].GetObject()->IsWeapon())
    {
        return CCharacter::CalcDamage(nPos,pDefender);
    }
    else
    {
        short nDam = ROLL(m_Equipment[nPos]->GetDiceType(pDefender->m_pAttributes->GetSize()),
            m_Equipment[nPos]->GetNoOfDice(pDefender->m_pAttributes->GetSize()));
		//for mobs we add the bare hand dam as well
		nDam += ROLL(m_nTypeBareHandDamDice,m_nNoBareHandDamDice) + m_nPlusBareHandDamDice;

		//if they are not fighting or standing double dam
		if((!(pDefender->InPosition(POS_FIGHTING))) || 
        (!(pDefender->InPosition(POS_STANDING))))
	    {
		    nDam <<= 1;
	    }
		if(DIE(1000)<=5) //0.5%
		{
			nDam <<=1;
		}
		return nDam;
    }
}

/////////////////////////////////
//	Does the npc know the skill or spell well enough to teach it to player
bool CNPC::IsGoodEnoughToTeach(const CCharacter *pToBeTaught, short nSkill, bool bSkill) const
{
	if(bSkill)
	{
		if(GetSkill(nSkill) > (pToBeTaught->GetSkill(nSkill)+MIN_SKILL_ABILITY_TO_TEACH))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if(m_Spells[nSkill] > (pToBeTaught->m_Spells[nSkill]+MIN_SPELL_ABILITY_TO_TEACH))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

/////////////////////////////////
//	InitializeCharacter
//	virtual function used to initialize characters
//	not using the constructor to do this becuase we may need
//	to call some virtual funtions
void CNPC::InitializeCharacter()
{
	UpDatePosition(m_nDefaultPos,false);
	if(this->Prefers(PERM_DI))
	{
		this->AddAffect(CCharacter::AFFECT_DI,sAffect::PERM_AFFECT,0,false);
	}
}

///////////////////////////////
//	The way pets break control
void CNPC::DoBreakMastersHold()
{
	if(!InPosition(POS_DEAD))
	{
		if(GetRace()==RACE_UNDEAD)
		{
			int nDie = DIE(5);
			m_nPreference = 0;
			for(register int i=0;i<nDie;i++)
			{
				m_nPreference |= (1<<(DIE(32)-1));
			}
			//remove bit we don't allow
			m_nPreference&=~CNPC::NOBASH;
			m_nPreference&=~CNPC::TRACK;
			m_nPreference&=~CNPC::TEACHER;
			if(!GetMaster()->StatCheck(CHA,+10))
			{
				AddMemory(GetMaster());
				CString str;
				str.Format("%s did not like being under your control.\r\n",
					(char *)GetRaceOrName(GetMaster()));
				GetMaster()->SendToChar(str);
			}
		}
		else
		{
			if(!GetMaster()->StatCheck(CHA) 
				&& DIE(100)<3)
			{
				m_nPreference|=MEMORY;
				AddMemory(GetMaster());
				CString str;
				str.Format("%s did not like being under your control.\r\n",
					(char *)GetRaceOrName(GetMaster()));
				GetMaster()->SendToChar(str);
			}
			else
			{
				//kill them
				m_nCurrentHpts = HITPTS_OF_DEATH;
				UpDatePosition();
			}
		}
	}
}

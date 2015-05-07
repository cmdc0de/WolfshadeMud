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
// thinker.h: interface for the Cthinker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THINKER_H__57179D7C_2996_11D3_8D53_00105A27514A__INCLUDED_)
#define AFX_THINKER_H__57179D7C_2996_11D3_8D53_00105A27514A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "character.h"

template<class Thinker>
class CThinker : virtual public CCharacter  
{
protected:
	//struct stores all information about the spell being casted
	CCurrentSpell<Thinker> m_CurrentSpell;
	bool m_bHasSpelledUp;
protected:
	CThinker(CCharIntermediate *ch) : CCharacter(ch) {m_bHasSpelledUp = false;}
	CThinker(CMobPrototype &mob,CRoom *pPutInRoom)
		: CCharacter(mob,pPutInRoom) {m_bHasSpelledUp = false;}
protected:
	virtual void BuildPracticePriceSpells(const CCharacter *pToBeTaught,CString &str) const;
	virtual bool SetSpell(const CCharacter *pCh,CString strSpell, skill_type nValue);
	virtual const CSpell<Thinker> *GetSpellInfo(CString strSpell) const=0;
	virtual const CMap<CString,CSpell<Thinker> *> *GetSpellTable() const=0;
	virtual short GetPctSpellUp(const CSpell<Thinker> *pSpell) const=0;
	virtual void SpellUp(const CSpell<Thinker> *pSpell);
	virtual void DoThink(const CInterp *pInterp);
	virtual int CalcSpellDam(const CSpell<Thinker> * pSpell);
	virtual bool LostConcentration();
	virtual void DoPractice(CString strToPractice,const CCharacter *pTarget);
	virtual bool CastingTargetInRoom();
	virtual void DoSpells();
	virtual void OffensiveSpell(const CInterp * pInterp, const CSpell<Thinker> * pSpell);
public:
	virtual bool HasCommand();
	virtual ~CThinker(){;}
};

///////////////////////////////
//	Over ridden for casters
//	Allows a god to the effecticy of a spell
///////////////////////////////
template<class Thinker>
bool CThinker<Thinker>::SetSpell(const CCharacter *pCh,CString strSpell, skill_type nValue)
{
	const CSpell<Thinker> *pSpell;
	if((pSpell = this->GetSpellInfo(strSpell))!=NULL)
	{
		m_Spells[pSpell->GetSpellID()] = nValue>MAX_SKILL_LEVEL ? MAX_SKILL_LEVEL : nValue;
		CString strToChar;
		strToChar.Format("%s makes you feel more comfortable casting %s.\r\n",
			(char *)pCh->GetName(),(char *)pSpell->GetColorizedName());
		SendToChar(strToChar);
		strToChar.Format("You increase %s's skill in casting %s to %d.\r\n",
			(char *)GetName(),
			(char *)pSpell->GetColorizedName(),
			nValue);
		pCh->SendToChar(strToChar);
		return true;
	}
	return false;
}

///////////////////////////////
//	Like Skill up but for spells
template<class Thinker>
void CThinker<Thinker>::SpellUp(const CSpell<Thinker> *pSpell)
{
	if((!m_bHasSpelledUp)  
		&& m_Spells[pSpell->GetSpellID()]<=MAX_SKILL_LEVEL
		&& (this->GetPctSpellUp(pSpell) >= DIE(101)) 
		&& IsAffectedBy(CCharacter::AFFECT_CASTING))
	{
		m_bHasSpelledUp = true;
		m_Spells[pSpell->GetSpellID()]+=DIE(this->GetLearingBonus());
		CString strToChar;
		strToChar.Format("&cYou feel more comfortable casting %s&n\r\n",
			(char *)pSpell->GetColorizedName());
		SendToChar(strToChar);
	}
}

template<class Thinker>
void CThinker<Thinker>::DoThink(const CInterp *pInterp)
{
	CString CastMsg;
	static const char *pTick = "'";
	//if they aren't casting check all information and insure
	//they can cast the spell
	if(!IsAffectedBy(CCharacter::AFFECT_CASTING))
	{
		int first = CurrentCommand.Find(pTick[0]);
		int second = CurrentCommand.Find(pTick[0],2);

		if(first == -1 || second == -1)
		{
			SendToChar("Spells must be inclosed in single quotes!\r\n");
			return;
		}

		CString strSpell(CurrentCommand(first+1,second-1));

		const CSpell<Thinker> *pSpell = NULL;

		if(!(pSpell = GetSpellInfo(strSpell)))
		{
			SendToChar("Think what?\r\n");
			return;
		}
		//take do they have enough to cast the spell?
		if(m_nManaPts-(pSpell->GetMana()) < 0)
		{
			SendToChar("You need more mana!\r\n");
			return;
		}
		if(!m_Spells[pSpell->GetSpellID()] || pSpell->GetMinLevel()>this->GetLevel())
		{
			SendToChar("You do not have knowledge of that spell.\r\n");
			return;
		}
		CString strTarget(CurrentCommand.Right(second+1));
		strTarget.SkipSpaces();
		CCharacter *pTarget=NULL;

		if((pSpell->GetAffectedArea() & AREA_TARGET))
		{
			CString strTmp(CurrentCommand);
			CurrentCommand = strTarget;
			pTarget = GetTarget(0,true);
			CurrentCommand=strTmp;
			if(!pTarget)
			{
				SendToChar("At Who?\r\n");
				return;
			}
		}
		if((pSpell->GetAffectedArea() & AREA_SELF)) pTarget = this;
		//now we know we can cast it so set casting bit
		//and set up all information in current spell struct
		m_lLagTime = pSpell->GetLagTime();
		AddAffect(CCharacter::AFFECT_CASTING,sAffect::MANUAL_AFFECT);
		m_CurrentSpell.m_pSpell = pSpell;
		m_CurrentSpell.m_strTarget = strTarget;
		m_CurrentSpell.m_pTarget = pTarget;
		if(m_CurrentSpell.m_pSpell->IsOffensive())
		{
			if(IsAffectedBy(CCharacter::AFFECT_INVISIBLE))
			{
				RemoveAffect(CCharacter::AFFECT_INVISIBLE);
			}
			if((m_CurrentSpell.m_pSpell->GetAffectedArea() & AREA_TARGET) 
				&& m_CurrentSpell.m_pTarget!=NULL)
			{
				if(this->m_Master.IsMasterOf(m_CurrentSpell.m_pTarget))
				{
					this->m_Master.Remove(m_CurrentSpell.m_pTarget);
				}
				if(this->m_Follow.HasFollower(m_CurrentSpell.m_pTarget))
				{
					this->m_Follow.Remove(m_CurrentSpell.m_pTarget);
				}
			}
		}
		CastMsg = "&LYou start to gather your thoughts...&n\r\n";
		SendToChar(CastMsg);
		m_bHasSpelledUp = false; //set so we can increase skill in this spell
	
	}
	//they already were casting so we know casting time is up
	//other wise we wouldn't be in here.
	else
	{
		//remove casting 
		RemoveAffect(CCharacter::AFFECT_CASTING);
		//cast spell
		if(this->CastingTargetInRoom())
		{
			SendToChar("&LYou complete your thought and form them into &creality...&n\r\n");
			m_CurrentSpell.m_pSpell->Cast(pInterp,(Thinker *)this);
			m_nManaPts-=m_CurrentSpell.m_pSpell->GetMana();
		}
		else
		{
			SendToChar("Who?\r\n");
		}
		m_CurrentSpell.Empty();
	}
}

////////////////////////////
//	Calc spell damage
//	we have not fully defined how we will calculate
//	damange for psi's yet
template<class Thinker>
int CThinker<Thinker>::CalcSpellDam(const CSpell<Thinker> * pSpell)
{
	return pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]);
}

///////////////////////////
//	LostConcentration()
//	Checks to see if we lose concentration
//	on the spell we are casting.
template<class Thinker>
bool CThinker<Thinker>::LostConcentration()
{
	if(m_nLevel < (m_CurrentSpell.m_pSpell->GetMinLevel()+5))
	{
		//Do int check and see if concen was lost.
		return !StatCheck(INT);
	}
	return false;
}

template<class Thinker>
void CThinker<Thinker>::DoPractice(CString strToPractice, const CCharacter *pTeacher)
{
	const CSpell<Thinker> *pSpell;
	CString strToChar;
	if((pSpell = this->GetSpellInfo(strToPractice)))
	{
		skill_type nPctLearn;
		if(this->GetLevel()<pSpell->GetMinLevel())
		{
			SendToChar("You need more levels first!\r\n");			
		}
		else if((nPctLearn = pTeacher->CanTeach(this,pSpell->GetSpellID(),false))>0)
		{
			if(m_Spells[pSpell->GetSpellID()]==0)
			{
				sMoney Cost;
				if(CanAffordTraining(Cost,pSpell->GetSpellID(),pTeacher,false))
				{
					strToChar.Format("You learn the spell %s.\r\n",(char *)pSpell->GetColorizedName());
					m_Spells[pSpell->GetSpellID()] = (skill_type) DIE(nPctLearn);
					m_nSpellSphere |= pSpell->GetSphere();
					
					if(!Cost.IsEmpty())
					{
						strToChar.Format("%s%s\r\n",
							(char *) strToChar,
							(char *) Cost.GetMoneyString());
					}
					SendToChar(strToChar);
				}
				else
				{
					SendToChar("You can't seem to afford the training.\r\n");
				}
			}
			else
			{
				SendToChar("You already know that spell.\r\n");
			}
		}
	}
	else
	{
		CCharacter::DoPractice(strToPractice,pTeacher);
	}
}

//////////////////////////////////
//	Spells
//	Send character the spells they know
//	as well as how good they are at that spell
/////////////////////////////////
template<class Thinker>
void CThinker<Thinker>::DoSpells()
{
	CSpell<Thinker> *pSpell;
	POSITION pos = GetSpellTable()->GetStartingPosition();
	CString strToChar;

	strToChar+="Spells Known:\r\n";
	while(pos)
	{
		pSpell = GetSpellTable()->GetNext(pos);
		if(m_Spells[pSpell->GetSpellID()])
		{
			strToChar.Format("%s%-30s\t%s\r\n",(char *) strToChar,
				(char *)pSpell->GetColorizedName(),this->SkillWording(m_Spells[pSpell->GetSpellID()]));
		}
	}
	SendToChar(strToChar);
}

///////////////////////
//	Take our Target and see if it's 
//	still in the room
template<class Thinker>
bool CThinker<Thinker>::CastingTargetInRoom()
{
	if(m_CurrentSpell.m_pTarget)
	{
		return (m_CurrentSpell.m_pTarget->IsDead() ? false : this->IsInRoom(m_CurrentSpell.m_pTarget));
	}
	else if(m_CurrentSpell.m_pSpell->GetAffectedArea() & AREA_ROOM ||
		m_CurrentSpell.m_pSpell->GetAffectedArea() == 0)
	{
		return true;
	}
	ErrorLog << "casting bit set when it shouldn't be!\r\n" << endl;
	return false;
}

///////////////////////////////
//	Overridden HasCommand from CCharacter class
//////////////////////////////
template<class Thinker>
bool CThinker<Thinker>::HasCommand()
{
	CString strToChar;
	if(IsAffectedBy(CCharacter::AFFECT_CASTING))
	{
		if(!CastingTargetInRoom())
		{
			//don't return here so we get the next command
			RemoveAffect(CCharacter::AFFECT_CASTING);
			SendToChar("You target has left the room!\r\n");
			GetRoom()->SendToRoom("&R%s's face turns pale!&n\r\n",this);
			m_lLagTime=0;
		}
		else if(!InPosition(POS_STANDING))
		{
			SendToChar("&WYou can't do this while not standing!&n\r\n&RYou stop your spell!&n\r\n");
			GetRoom()->SendToRoom("&R%s's face turns pale!&n\r\n",this);
			RemoveAffect(CCharacter::AFFECT_CASTING);
		}
		if((m_lLagTime-1) && !((m_lLagTime-1)%(CMudTime::PULSES_PER_CASTING_ROUND * 2)))
		{
			SpellUp(m_CurrentSpell.m_pSpell);
			if(LostConcentration())
			{
				RemoveAffect(CCharacter::AFFECT_CASTING);
				SendToChar("You lose your concentration.\r\n");
				m_pInRoom->SendToRoom("%s face blushes.\r\n",this);
				m_lLagTime=0;
				SendPrompt();
			}
		}
	}
	//run base has command
	return CCharacter::HasCommand();
}

//////////////////////////////////
//	OffensiveSpell
//	this function runs for all Psionicist offensive spells
//	do checks to see if we can cast in this room etc.
//	next check to see if spell is room affect
//	or target
//	written by: Demetrius Comes
template<class Thinker>
void CThinker<Thinker>::OffensiveSpell(const CInterp * pInterp, const CSpell<Thinker> * pSpell)
{
	if(m_pInRoom->IsAffectedBy(CRoom::ROOM_AFFECT_PEACEFUL))
	{
		SendToChar("This place is far to peacefull.\r\n");
		return;
	}
	CCharacter *pTarget;
	if(pSpell->GetAffectedArea() & AREA_ROOM)
	{
		//get first character in room
		POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
		while(pos)
		{
			pTarget = m_pInRoom->GetNextCharacter(pos);
			//make sure it's not me
			//later will need to protect group as well
			if(pTarget!=this && !m_Group.IsInGroup(pTarget))
			{
				pTarget->ThinkerDefense(CalcSpellDam(pSpell),this);
				if(pTarget->IsDead())
				{
					m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_KILLED,this,pTarget);
					pInterp->KillChar(pTarget);
				}
				else
				{
					m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget);
					//if target isn't make it fight
					AddFighters(pTarget,false);
				}
			}
		}
	}
	//we are casting at self or someone else not a room
	else
	{
		pTarget = m_CurrentSpell.m_pTarget;
		if(!CastingTargetInRoom())
		{
			SendToChar("Who?\r\n");
		}
		else
		{
			pTarget->ThinkerDefense(CalcSpellDam(pSpell),this);
			if(pTarget->IsDead())
			{
				m_MsgManager.SpellMsg(m_CurrentSpell.m_pSpell->GetSpellName(),SKILL_KILLED,this,pTarget);
				pInterp->KillChar(pTarget);
			}
			else
			{
				m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget);
				AddFighters(pTarget,false);
			}
		}
	}
}

template<class Thinker>
void CThinker<Thinker>::BuildPracticePriceSpells(const CCharacter *pToBeTaught,CString &str) const
{
	if(this->IsTeacher())
	{
		sMoney Cost;
		str.Format("%sI can teach you the following spells:\r\n",(char *)str);
		POSITION pos = GetSpellTable()->GetStartingPosition();
		const CSpell<Thinker> *pSpell;
		while(pos)
		{
			pSpell = GetSpellTable()->GetNext(pos);
			if(this->IsGoodEnoughToTeach(pToBeTaught,pSpell->GetSpellID(),false))
			{
				Cost = GetTrainingCosts(pToBeTaught,pSpell->GetSpellID(),false);
				str.Format("%s%-30s\t%s\r\n",
					(char *)str,
					(char *)pSpell->GetSpellName(),
					(char *)Cost.GetMoneyString());
			}
		}
	}
	else
	{
		str = "My skills are too value-able to me to teach anyone.\r\n";
	}
}
#endif // !defined(AFX_THINKER_H__57179D7C_2996_11D3_8D53_00105A27514A__INCLUDED_)

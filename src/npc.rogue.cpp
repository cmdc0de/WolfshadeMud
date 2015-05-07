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
#include "stdinclude.h"
#pragma hdrstop
#include "npc.classes.h"

CMobAIManager<CNPCRogue> CNPCRogue::m_MobAI(&CNPCRogue::DoGeneralAI);
bool CNPCRogue::m_bInitNpcRogue = false;
short CNPCRogue::RATE_MEMORY_ROGUE_FLEES = 30;


CNPCRogue::CNPCRogue(CMobPrototype &mob, CRoom *pPutInRoom)
	:CRogue(mob,pPutInRoom), CNPC(mob,pPutInRoom),CCharacter(mob,pPutInRoom)
{
	//can't use mob the this->GetLevel because it will always be 0
	//we don't advance level until after npc is created
	//because pure virtuals are called from advance level
	for(register int i = 0;i<MAX_MUD_SKILLS;i++)
	{
		if(m_SkillInfo[i].CanLearn() &&
			HasSkillSet(m_SkillInfo[i].GetSet()))
		{
			//if they have this skill set give them
			//an automatic bonus of 1/4 of there level
			m_Skills[i] = MIN(MAX_MOB_SKILL,(mob.GetLevel()+DIE((mob.GetLevel()>>1)+1)+(m_Proficiency.CalcBonus(m_nSkillSphere)*10)));
		}
	}
}

CNPCRogue::~CNPCRogue()
{

}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////
short CNPCRogue::GetMaxHitPointsPerLevel()
{
	short nLevel = GetLevel() + 1;
	//9 is Base
	short nHits = (2^((nLevel+1)/10) * nLevel * 9);
	return nHits;
}

void CNPCRogue::DoAI(const CInterp *pInterp)
{
	m_MobAI.RunMobAI(pInterp,m_nVNum,this);
}

/////////////////////////////////
//	This should be basically the same for everyone
//	TODO lookinto moving this to NPC class
void CNPCRogue::DoGeneralAI(const CInterp *pInterp)
{
	//if our command buffer is empty we aren't lagged
	//and we aren't casting
	if(CommandBufEmpty() && !IsLagged())
	{
		if(IsFighting() && this->IsInRoom(IsFighting()))
		{
			//we don't can't a general offensive spell
			//run CNPC DOAI
			if(!InPosition(POS_STANDING) || !GeneralOffense(pInterp))
			{
				CNPC::DoAI(pInterp);
			}
		}
		else
		{
			//if we don't run a defensive spell
			//so NPC ai stuff
			//decision made 1-1-2000 to not do defensive AI
			//if mob is a pet
			if(!InPosition(POS_STANDING) 
				|| HasMaster()
				|| !GeneralDefense(pInterp))
			{
				CNPC::DoAI(pInterp);
			}
		}
	}
}

////////////////////////////////
//	we use this to register vnums with ai functions
void CNPCRogue::DoInit()
{
	if(m_bInitNpcRogue)
	{
		return;
	}
	m_bInitNpcRogue = true;
	//m_MobAI.RegisterAIFnc(24001,&CNPCMage::DoGreyAI);
}

////////////////////////////////////
//  rogues...if they have memory they will try to 
//  flee then hide
//  If they don't they may try to trip, kick, dirttoss
bool CNPCRogue::GeneralOffense(const CInterp *pInterp)
{
	bool bRetVal = false;
	if(DIE(200)<GetLevel())
	{
		short nDie = DIE(100);
		if(Prefers(MEMORY) && nDie<RATE_MEMORY_ROGUE_FLEES)
		{
			CRoom *pRoom = GetRoom();
			this->DoFlee();
			if(pRoom!=GetRoom())
			{
				AddCommand("Hide");
			}
			bRetVal = true;
		}
		else
		{
			if(nDie<15)
			{
				if(!IsFighting()->IsAffectedBy(CCharacter::AFFECT_BLINDNESS))
				{
					this->DoDirtToss(IsFighting());
					bRetVal = true;
				}
			}
			else if(nDie<30)
			{
				this->DoKick(pInterp,IsFighting());
				bRetVal = true;
			}
			else if(nDie<45)
			{
				this->DoThroatSlit(pInterp,IsFighting());
				bRetVal = true;
			}
			else if(nDie<60)
			{
				if(IsFighting()->InPosition(POS_STANDING))
				{
					this->DoTrip(pInterp,IsFighting());
					bRetVal = true;
				}
			}	
		}
	}
	return bRetVal;
}


/////////////////////////////////////////
//	Rogue mobs will try to sneak, hide
//	perhap even try to 
bool CNPCRogue::GeneralDefense(const CInterp *pInterp)
{
	bool bRetVal = false;
	if(DIE(1000)<GetLevel()>>2)
	{
		short nDie = DIE(100);
		if(nDie<15)
		{
			if(!IsAffectedBy(CCharacter::AFFECT_SNEAKING))
			{
				this->DoSneak(pInterp);
				bRetVal = true;
			}
		}
		else if(nDie<30)
		{
			if(!IsAffectedBy(CCharacter::AFFECT_HIDING))
			{
				this->DoHide();
				bRetVal = true;
			}
		}
	}
	return bRetVal;
}

void CNPCRogue::DoAIAttack(const CInterp *pInterp, CCharacter *pTarget) 
{
	bool bHasPiecer = false;
	short nWeaponPos;
	for(register int i=0;i<this->GetMaxWeaponPositions();i++)
	{
		nWeaponPos = this->GetWeaponPosition(i+1);
		if(!m_Equipment[nWeaponPos].IsEmpty() 
			&& m_Equipment[nWeaponPos].GetObject()->IsWeapon()
			&& m_Equipment[nWeaponPos].GetObject()->GetMessageType() == PIERCE)
		{
			bHasPiecer = true;
			break;
		}
	}
	if(bHasPiecer)
	{
		this->DoBackStab(pInterp,pTarget);
	}
	else
	{
		short nDie = DIE(100);
		if(nDie<10)
		{
			this->DoTrip(pInterp,pTarget);
		}
		else if(nDie<30)
		{
			this->DoKick(pInterp,pTarget);
		}
		else
		{
			this->DoKill(pInterp,pTarget);
		}
	}
}

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

CMobAIManager<CNPCWarrior> CNPCWarrior::m_MobAI(&CNPCWarrior::DoGeneralAI);
bool CNPCWarrior::m_bInitNPCClass = false;

CNPCWarrior::CNPCWarrior(CMobPrototype &mob, CRoom *pPutInRoom)
	:CWarrior(mob,pPutInRoom),CNPC(mob,pPutInRoom),CCharacter(mob,pPutInRoom)
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

CNPCWarrior::~CNPCWarrior()
{

}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////
short CNPCWarrior::GetMaxHitPointsPerLevel()
{
	short nLevel = GetLevel() + 1;
	//10 is Base for warrior
	short nHits = (2^((nLevel+1)/10) * nLevel * 10);
	return nHits;
}

void CNPCWarrior::DoAIAttack(const CInterp *pInterp, CCharacter *pTarget) 
{
	bool bHasShield = false;
	short nWeaponPos;
	for(register int i=0;i<this->GetMaxWeaponPositions();i++)
	{
		nWeaponPos = this->GetWeaponPosition(i+1);
		if(!m_Equipment[nWeaponPos].IsEmpty() 
			&& m_Equipment[nWeaponPos].GetObject()->IsWeapon()
			&& m_Equipment[nWeaponPos].IsWorn(WEAR_POS_SHIELD))
		{
			bHasShield = true;
			break;
		}
	}
	if(bHasShield && pTarget->InPosition(POS_STANDING))
	{
		this->DoBash(pInterp,pTarget);
	}
	else
	{
		short nDie = DIE(100);
		if(nDie<20 && pTarget->InPosition(POS_STANDING))
		{
			this->DoTrip(pInterp,pTarget);
		}
		else if(nDie<60)
		{
			this->DoKick(pInterp,pTarget);
		}
		else if(CanBodySlam(pTarget) && GetSkill(SKILL_BODYSLAM)>40
			&& pTarget->InPosition(POS_STANDING))
		{
			this->DoBodySlam(pInterp,pTarget);	
		}
		else
		{
			this->DoKill(pInterp,pTarget);
		}
	}
}

void CNPCWarrior::DoAI(const CInterp *pInterp)
{
	m_MobAI.RunMobAI(pInterp,m_nVNum,this);
}

////////////////////////////////
//	we use this to register vnums with ai functions
void CNPCWarrior::DoInit()
{
	if(m_bInitNPCClass)
	{
		return;
	}
	m_bInitNPCClass = true;
	//m_MobAI.RegisterAIFnc(24001,&CNPCMage::DoGreyAI);
}

/////////////////////////////////
//	This should be basically the same for everyone
//	TODO lookinto moving this to NPC class
void CNPCWarrior::DoGeneralAI(const CInterp *pInterp)
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

////////////////////////////////////
//  warriors general offense
///////////////////////////////////
bool CNPCWarrior::GeneralOffense(const CInterp *pInterp)
{
	bool bRetVal = false;
	//only go ai if we roll under our level
	if(DIE(200)<GetLevel() && IsFighting()->InPosition(POS_STANDING))
	{
		bool bHasShield = false;
		short nWeaponPos;
		for(register int i=0;i<this->GetMaxWeaponPositions();i++)
		{
			nWeaponPos = this->GetWeaponPosition(i+1);
			if(!m_Equipment[nWeaponPos].IsEmpty() 
				&& m_Equipment[nWeaponPos].IsWorn(WEAR_POS_SHIELD))
			{
				bHasShield = true;
				break;
			}
		}
		short nDie = DIE(100);
		if(bHasShield 
			&& (IsFighting()->InPosition(POS_STANDING)
			|| nDie<10)) // only give a 10% of bashing if target isn't standing
		{
			if(nDie<35)
			{
				if(GetSkill(SKILL_QUICK_STRIKE)>0
					&& nDie<15)
				{
					this->DoStrike(pInterp,IsFighting());
					bRetVal = true;
				}
				else if(GetSkill(SKILL_WEAPON_FLURRY)>0
					&& nDie<25)
				{
					this->DoWeaponFlurry(pInterp);
					bRetVal = true;
				}
				else
				{
					if(nDie<20)
					{
						this->DoKick(pInterp,IsFighting());
						bRetVal = true;
					}
					else
					{
						this->DoTrip(pInterp,IsFighting());
						bRetVal = true;
					}
				}
			}
			else
			{
				this->DoBash(pInterp,IsFighting());
				bRetVal = true;
			}
		}
		else
		{
			if(GetSkill(SKILL_QUICK_STRIKE)>0
				&& nDie<18)
			{
				this->DoStrike(pInterp,IsFighting());
				bRetVal = true;
			}
			else if(GetSkill(SKILL_WEAPON_FLURRY)>0
				&& nDie<36)
			{
				this->DoWeaponFlurry(pInterp);
				bRetVal = true;
			}
			else if(GetSkill(SKILL_DRAGON_PUNCH)>0
				&& nDie<54)
			{
				this->DoDragonPunch(pInterp,IsFighting());
				bRetVal = true;
			}
			else if(GetSkill(SKILL_HEAD_BUTT)>40 //too much advantage if they koed
				&& nDie<72)
			{
				this->DoHeadButt(pInterp,IsFighting());
				bRetVal = true;
			}
			else
			{
				//may even try to bash with out a shield
				if(nDie<2)
				{
					this->DoBash(pInterp,IsFighting());
					bRetVal = true;
				}
				else if(nDie<40)
				{
					this->DoKick(pInterp,IsFighting());
					bRetVal = true;
				}
				else
				{
					this->DoTrip(pInterp,IsFighting());
					bRetVal = true;
				}
			}
		}
	}
	return bRetVal;
}

bool CNPCWarrior::GeneralDefense(const CInterp *pInterp)
{
	bool bRetVal = false;
	//we only do general defensive if we roll under
	//our level / 4
	if(DIE(1000)<GetLevel()>>2) 
	{
		if(GetSkill(SKILL_CHANT_DEFENSE)>0
			&& !IsAffectedBy(CCharacter::AFFECT_CHANT_DEFENSE))
		{
			AddCommand("chant defense");
		}
		else if(GetSkill(SKILL_CHANT_HONOR)>0
			&& !IsAffectedBy(CCharacter::AFFECT_CHANT_HONOR))
		{
			AddCommand("chant honor");
		}
		else if(GetSkill(SKILL_CHANT_REGENERATION)>0
			&& !IsAffectedBy(CCharacter::AFFECT_CHANT_REGENERATION))
		{
			AddCommand("chant regeneration");
		}
	}
	return bRetVal;
}

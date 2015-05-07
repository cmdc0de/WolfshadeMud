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


CNPCCleric::CNPCCleric(CMobPrototype &mob,CRoom *pPutInRoom)
	:CCleric(mob,pPutInRoom), CNPC(mob,pPutInRoom),CCharacter(mob,pPutInRoom)
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
	int nSkill;
	POSITION pos = m_SpellTable.GetStartingPosition();
	CSpell<CCleric> *pSpell;
	while(pos)
	{
		pSpell = m_SpellTable.GetNext(pos);
		if(IsOfSphere(pSpell->GetSphere()) &&
			mob.GetLevel() >= pSpell->GetMinLevel())
		{
			nSkill = DIE(mob.GetLevel()+1)+mob.GetLevel();
			m_Spells[pSpell->GetSpellID()] = MIN(MAX_MOB_SKILL,nSkill);
		}
	}
}

CNPCCleric::~CNPCCleric()
{

}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////
short CNPCCleric::GetMaxHitPointsPerLevel()
{
	short nLevel = GetLevel() + 1;
	//8 is Base
	short nHits = (2^((nLevel+1)/10) * nLevel * 8);
	return nHits;

}
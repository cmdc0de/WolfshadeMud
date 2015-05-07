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
//////////////////////////////////////////////////////////////// armor.cpp: implementation of the CArmor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "armor.h"

int CArmor::SHADOW_ARMOR_PULSE=CMudTime::PULSES_PER_REAL_MIN*2;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArmor::CArmor(CObjectPrototype *obj, CCharacter *PersonCarriedby, CRoom *pInRoom)
	: CObject(obj,PersonCarriedby,pInRoom)
{
	m_nACAjustment = obj->Val0;
}	

CArmor::CArmor(const sObjectSave &ObjSave,CCharacter *pChar,CRoom *pRoom)
	: CObject(ObjSave,pChar,pRoom)
{
	m_nACAjustment = ObjSave.m_nVal0;
}

CArmor::~CArmor()
{

}

void CArmor::WearAffects(bool bRemoving)
{
	if(m_pCarriedby)
	{
		if(bRemoving)
		{
			m_pCarriedby->AdjustAC(m_nACAjustment);
		}
		else
		{
			m_pCarriedby->AdjustAC(-m_nACAjustment);
		}
	}
	CObject::WearAffects(bRemoving);
}

void CArmor::FadeAffects(long lTimeToRemove)
{
	if(IsAffectedBy(OBJ_AFFECT_SHADOW_ARMOR)
		&& IsAffectedBy(OBJ_AFFECT_CURRENTLY_WORN)
		&& !(CMudTime::GetCurrentPulse()%SHADOW_ARMOR_PULSE))
	{
		if(m_pCarriedby)
		{
			if(m_pCarriedby->IsAffectedBy(CCharacter::AFFECT_HIDING))
			{
				m_pCarriedby->RemoveAffect(CCharacter::AFFECT_HIDING,false);//remove it silently
				m_pCarriedby->AddAffect(CCharacter::AFFECT_HIDING,sAffect::MANUAL_AFFECT,0,false); 
			}
			else
			{
				m_pCarriedby->AddAffect(CCharacter::AFFECT_HIDING,sAffect::MANUAL_AFFECT,0,true); 
			}
			if(m_pCarriedby->IsAffectedBy(CCharacter::AFFECT_STONE_SKIN))
			{
				m_pCarriedby->RemoveAffect(CCharacter::AFFECT_STONE_SKIN,false);//remove with no message
				m_pCarriedby->AddAffect(CCharacter::AFFECT_STONE_SKIN,sAffect::MANUAL_AFFECT,500+DIE(200),false); //reapply
			}
			else
			{
				m_pCarriedby->AddAffect(CCharacter::AFFECT_STONE_SKIN,sAffect::MANUAL_AFFECT,500+DIE(200),true); //reapply
			}
		}
	}
	CObject::FadeAffects(lTimeToRemove);
}
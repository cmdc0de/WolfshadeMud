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
#include "object.misc.h"

const short CPotion::POTION_SPECIAL_PROC=-1;
const short CPotion::POTION_ARMOR=0;
const short CPotion::POTION_BARK_SKIN=1;
const short CPotion::POTION_SPIRIT_ARMOR=2;
const short CPotion::POTION_STONE_SKIN=3;
const short CPotion::POTION_CURE=4;
const short CPotion::POTION_INVIS=5;
const short CPotion::POTION_DI=6;
const short CPotion::POTION_DETECT_MAGIC=7;
#define VNUM_POTION_51 33
#define VNUM_POTION_52 34
CMap<obj_vnum,CPotion::sPotionSpecial> *CPotion::m_pSpecialProc = new CMap<obj_vnum,CPotion::sPotionSpecial>(10);
bool CPotion::m_bInitPotion = false;

CPotion::CPotion(const sObjectSave & ObjSave, CCharacter *pChar,CRoom *pRoom)
	:CObject(ObjSave,pChar,pRoom)
{
	InitPotionStatics();
}

CPotion::CPotion(CObjectPrototype *obj, CCharacter *PersonCarriedby, CRoom *pInRoom)
	:CObject(obj,PersonCarriedby,pInRoom)
{
	InitPotionStatics();
}

CPotion::~CPotion()
{

}

void CPotion::InitPotionStatics()
{
	if(m_bInitPotion)
		return;
	m_bInitPotion = true;
	m_pSpecialProc->Add(VNUM_POTION_51,sPotionSpecial(&CPotion::GreaterThan50));
	m_pSpecialProc->Add(VNUM_POTION_52,sPotionSpecial(&CPotion::GreaterThan50));
}

///////////////////////////////
//	potions to level players greater than 50
void CPotion::GreaterThan50(CCharacter *pUser)
{
	if(pUser->IsGod())
	{
		pUser->SendToChar("Your already a god!\r\n");
	}
	else if(pUser->EnoughExpForNextLevel(false))
	{
		pUser->AdvanceLevel(true,true);
	}
	else
	{
		pUser->SendToChar("You don't have enough experience.\r\n");
	}
	pUser->Remove(this);
	this->SetDelete();
}
//////////////////////////////
//	Use
//	Virutal overload for potions
void CPotion::Use(CCharacter *pUser)
{
	CString str;
	str.Format("You quaff the %s.\r\n",
		GetObjName(pUser));
	pUser->SendToChar(str);
	pUser->GetRoom()->SendToRoom("%s quaffs %s.\r\n",
		pUser,this);
	if(m_nVal0==POTION_SPECIAL_PROC)
	{
		sPotionSpecial Proc;
		if(m_pSpecialProc->Lookup(GetVnum(),Proc))
		{
			(this->*Proc.m_pFnc)(pUser);
		}
		else
		{
			ErrorLog << this->m_strObjName << " is marked for a special proc but doesn't have one assigned. Vnum is " << GetVnum() << endl;
		}
	}
	else
	{
		switch(m_nVal0)
		{
		case POTION_ARMOR:
			pUser->AddAffect(CCharacter::AFFECT_ARMOR,(CMudTime::PULSES_PER_MUD_HOUR*m_nVal1),m_nVal2,true);
			break;
		case POTION_BARK_SKIN:
			pUser->AddAffect(CCharacter::AFFECT_BARKSKIN,(CMudTime::PULSES_PER_MUD_HOUR*m_nVal1),m_nVal2,true);
			break;
		case POTION_SPIRIT_ARMOR:
			pUser->AddAffect(CCharacter::AFFECT_SPIRIT_ARMOR,(CMudTime::PULSES_PER_MUD_HOUR*m_nVal1),m_nVal2,true);
			break;
		case POTION_STONE_SKIN:
			pUser->AddAffect(CCharacter::AFFECT_STONE_SKIN,(CMudTime::PULSES_PER_MUD_HOUR*m_nVal1),m_nVal2,true);
			break;
		case POTION_CURE:
			pUser->TakeDamage(pUser,-(ROLL(m_nVal1,m_nVal2)+m_nVal3),false,false);
			pUser->SendToChar("You feel better.\r\n");
			break;
		case POTION_INVIS:
			pUser->AddAffect(CCharacter::AFFECT_INVISIBILITY,(CMudTime::PULSES_PER_MUD_HOUR*m_nVal1),0,true);
			break;
		case POTION_DI:
			pUser->AddAffect(CCharacter::AFFECT_DI,(CMudTime::PULSES_PER_MUD_HOUR*m_nVal1),m_nVal2,true);
			break;
		case POTION_DETECT_MAGIC:
			pUser->AddAffect(CCharacter::AFFECT_DETECT_MAGIC,(CMudTime::PULSES_PER_MUD_HOUR*m_nVal1),m_nVal2,true);
			break;
		default:
			ErrorLog << this->m_strObjName << " has an unknown potion type specified for val0" << endl;
			break;
		}
		//remove the potion from the game
		pUser->Remove(this);
		this->SetDelete();
	}
}

/////////////////////////////////
//	Checks for any out landish mistakes with potions
//	sort of a sanity check	
bool CPotion::IsValidObject()
{
	bool bRetVal = false;
	switch(m_nVal0)
	{
	case POTION_SPECIAL_PROC:
		bRetVal = true;
		break;
	case POTION_ARMOR: case POTION_BARK_SKIN: case POTION_SPIRIT_ARMOR:
	case POTION_STONE_SKIN:
		bRetVal = (m_nVal1>=0 && m_nVal2>=0 && m_nVal3==-1 && m_nVal4==-1);
		break;
	case POTION_CURE:
		bRetVal = (m_nVal1>0 && m_nVal2>0 && m_nVal3>=0 && m_nVal4==-1);
		break;
	case POTION_INVIS: case POTION_DI: case POTION_DETECT_MAGIC:
		bRetVal = (m_nVal1>0 && m_nVal2==-1 && m_nVal3==-1 && m_nVal4==-1);
		break;
	default:
		bRetVal = false;
		break;
	}
	return bRetVal;
}
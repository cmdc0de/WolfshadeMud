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
// light.cpp: implementation of the CLight class.
//
//////////////////////////////////////////////////////////////////////
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "light.h"

const int CLight::LIGHT_LIT = 1;
const int CLight::LIGHT_NOT_LIT = 0;
const int CLight::FOREVER_LIT = -1;
const int CLight::LIGHT_BURNED_OUT = -2;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

///////////////////////////////
//	m_nVal0 = Light Power(1-6), 
//	m_nVal1 = Capacity of light in Hours converted to pulses
//	m_nVal2 = Has it been lit
//	m_nVal3 = What is it's current power //power changes with time
//  m_nVal4 = Currnet Capacity in hours converted to pulses
CLight::CLight(CObjectPrototype *obj, CCharacter *PersonCarriedby, CRoom *pInRoom)
	:CObject(obj,PersonCarriedby,pInRoom)
{
	if(m_nVal1==FOREVER_LIT)
	{
		m_nVal2 = FOREVER_LIT;
	}
	else
	{
		m_nVal2 = LIGHT_NOT_LIT;
	}
	//convert mud hours to pulses 
	m_nVal1*=CMudTime::PULSES_PER_MUD_HOUR;
	m_nVal3 = m_nVal0; //set current power to original
	m_nVal4 = m_nVal1;
}

CLight::CLight(const sObjectSave & ObjSave, CCharacter *pChar,CRoom *pRoom)
	:CObject(ObjSave,pChar,pRoom)
{
	if(ObjSave.m_nVal2==LIGHT_LIT && m_pCarriedby!=NULL)
	{
		m_pCarriedby->AddAffect(CCharacter::AFFECT_LIGHT,sAffect::MANUAL_AFFECT,m_nVal3,false);
	}
}

CLight::~CLight()
{
	if(!CGame::IsShuttingDown())
	{
		if(m_pCarriedby!=NULL)
		{
			m_pCarriedby->RemoveAffect(CCharacter::AFFECT_LIGHT);
		}
	}
}

/////////////////////////////////
//	over ridden so we can add or remove light from character
/////////////////////////////////
void CLight::AddCharAffects(bool bRemoving,bool bSendMsg)
{
	//m_pCarriedby should always be set if this is called
	if(bRemoving)
	{
		m_pCarriedby->RemoveAffect(CCharacter::AFFECT_LIGHT);
	}
	else
	{
		
		m_pCarriedby->AddAffect(CCharacter::AFFECT_LIGHT,sAffect::MANUAL_AFFECT,m_nVal3,false);
		if(m_nVal2==LIGHT_NOT_LIT && m_nVal2!=FOREVER_LIT)
		{
			CString strToChar;
			strToChar.Format("You light the %s.\r\n",
				GetObjName());
			m_pCarriedby->SendToChar(strToChar);
			m_nVal2=LIGHT_LIT;
		}
	}
	CObject::AddCharAffects(bRemoving,bSendMsg);
}


////////////////////////////////
//	over ridden to check if we need to decrease light power
///////////////////////////////
void CLight::FadeAffects(long lTimeToRemove)
{
	if(m_nVal2!=FOREVER_LIT)
	{
		if(m_nVal4>0 && m_nVal2!=LIGHT_BURNED_OUT)
		{
			//current time greater than 1/2 original time
			if((m_nVal1>>1) < m_nVal4)
			{
				m_nVal4-=(int)lTimeToRemove;
				//if it less or equal to half now?
				if((m_nVal1>>1) >= m_nVal4)
				{
					//reduce power to 1/2 orignal
					m_nVal3 = m_nVal0>>1;
					if(m_pCarriedby)
					{
						//have to use reduce affect instead of remove
						//b/c we can't call sync wear affects like we
						//do for wearing or removing
						m_pCarriedby->ReduceAffect(CCharacter::AFFECT_LIGHT,sAffect::MANUAL_AFFECT,m_nVal3,false);
					}
				}
			}
			//if time greater than 1/4 original time
			else if((m_nVal1>>2) < m_nVal4)
			{
				m_nVal4-=(int)lTimeToRemove;
				//if it less or equal to 1/4 now
				if((m_nVal1>>2) >= m_nVal4)
				{
					//reduce power to 1/4 original
					m_nVal3 = m_nVal0>>2;
					if(m_pCarriedby)
					{
						m_pCarriedby->ReduceAffect(CCharacter::AFFECT_LIGHT,sAffect::MANUAL_AFFECT,m_nVal3,false);
					}
				}
			}
			//just remove time
			else
			{
				m_nVal4-=(int)lTimeToRemove;
			}
		}
		else
		{
			if(m_pCarriedby)
			{
				m_pCarriedby->ReduceAffect(CCharacter::AFFECT_LIGHT,sAffect::MANUAL_AFFECT,m_nVal3,false);
			}
			m_nVal2 = LIGHT_BURNED_OUT; //set the light to be burnt out
			m_nVal3 = 0; //set it's light capacity to 0
			m_nVal4 = 0; //for good measure set the current time capacity off
		}
	}
	CObject::FadeAffects(lTimeToRemove);
}

////////////////////////////
//	If lite add to string
CString CLight::GetExtraString()
{
	if(IsLit())
	{
		CString str = CObject::GetExtraString();
		return str+= " &YLit&n";
	}
	return CObject::GetExtraString();
}

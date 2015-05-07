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
// corpse.cpp: implementation of the CCorpse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "corpse.h"

#define CORPSE_TIME  m_nVal0
#define CORPSE_LEVEL m_nVal1 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCorpse::CCorpse(CCharacter *pCh) : CContainer(pCh)
{
	m_bWasPC = (!pCh->IsNPC());
	CORPSE_TIME=0;
	CORPSE_LEVEL=pCh->GetLevel();
	if(pCh->IsNPC())
	{
		AddTime(CMudTime::PULSES_PER_REAL_MIN * 15);
	}
	else
	{
		AddTime(CMudTime::PULSES_PER_REAL_HOUR * 2);
	}
	m_nAffBit |= ITEM_NOT_TAKEABLE;
	m_nCurrentWeight = m_nWeight;
	CObject *pObj;
	for(register int i=0;i<MAX_EQ_POS;i++)
	{
		pObj = pCh->m_Equipment[i].GetObject();
		if(pObj)
		{
			pCh->RemoveEquipment(i,false);
			this->Add(pObj);
		}
	}
	POSITION pos = pCh->Inventory.GetStartPosition();
	while(pos)
	{
		pObj = pCh->Inventory.GetNext(pos);
		this->Add(pObj);
	}
	m_CashInContainer = pCh->m_CashOnHand;
	pCh->m_CashOnHand = 0;
	m_nCurrentWeight += m_CashInContainer.GetWeight();
}

CCorpse::CCorpse(const sObjectSave & ObjSave, CRoom *pRoom)
	:CContainer(ObjSave,NULL,pRoom)
{
	m_bWasPC = true; //only PC corpses are saved
	m_nAffBit |= ITEM_NOT_TAKEABLE;
}

void CCorpse::AddTime(int nPulses)
{
	CORPSE_TIME+=nPulses;
}

short CCorpse::GetCorpseLevel()
{
	return CORPSE_LEVEL;
}

CCorpse::~CCorpse()
{
	if(!CGame::IsShuttingDown())
	{
		assert(m_pInRoom);
		m_pInRoom->GetCash()+=m_CashInContainer;
		POSITION pos = Contents.GetStartPosition();
		while(pos)
		{
			m_pInRoom->Add(Contents.GetNext(pos));
		}
		//we don't need to remove contents that we
		//add to room because we are about to destruct
		m_pInRoom->Remove(this);
		m_pInRoom->SendToRoom("A &wm&Wa&wg&Wi&wc&Wa&wl &nwind dissolves a corpse into nothing.\r\n");
	}
}

///////////////////////////////////
//	over ridden so we don't check for weight limitations
//	we put everything into corpses
bool CCorpse::Add(CObject *pObj)
{
	//DO NOT SAVE CORPSES HERE! YOU WILL OVER WRITE FILE
	//AS IT IS BEING READ FROM...besides we never add
	//anything to a corpse unless we just died and them
	//we save the corpses after creation
	//CObjectManager::SaveCorpses();
	//dont check wieght. just add it
	m_nCurrentWeight+=pObj->GetWeight();
	Contents.Add(pObj);
	pObj->Set(NULL,NULL);
	return true;
}

/////////////////////////////////////
//	Over ridden so we do nothing with weight like we do 
//	in CContainer
void CCorpse::Remove(CObject *pObj)
{
	Contents.Remove(pObj);
	//we don't want to save every time we remove something we'll put smarter code in the loot command
	//CObjectManager::SaveCorpses();
}

void CCorpse::DeleteContents()
{
	CContainer::DeleteContents();
	CObjectManager::SaveCorpses();
}

//////////////////////////////////
//	Allows the plater to loot the corpse
void CCorpse::Loot(CCharacter *pCh,bool bFromRess)
{
	if(m_CashInContainer.IsEmpty() && Contents.IsEmpty())
	{
		if(!bFromRess)
		{
			pCh->SendToChar("There is nothing to loot.\r\n");
		}
	}
	else
	{
		CString strToChar;
		if(pCh->Add(m_CashInContainer,bFromRess)) //if true we force add
		{
			if(!m_CashInContainer.IsEmpty())
			{
				if(m_CashInContainer.GetBase()==1)
				{
					if(!bFromRess)
					{
						pCh->SendToChar("You get a single coin from the corpse.\r\n");
						pCh->GetRoom()->SendToRoom("%s gets a single coin from the corpse.\r\n",pCh);
					}
				}
				else
				{
					if(!bFromRess)
					{
						strToChar.Format("You get %s from the corpse.\r\n",
							(char *)m_CashInContainer.GetMoneyString());
						pCh->SendToChar(strToChar);
						pCh->GetRoom()->SendToRoom("%s gets a pile of coins from the corpse.\r\n",pCh);
					}
				}
				m_CashInContainer = 0;
			}
		}
		else
		{
			pCh->SendToChar("You can't seem to lift all the coins!\r\n");
		}
		POSITION pos = Contents.GetStartPosition();
		CObject *pObj;
		CList<CObject *> ObjTaken;
		while(pos)
		{
			pObj = Contents.GetNext(pos);
			if(pCh->Add(pObj,false,bFromRess))
			{
				if(!bFromRess)
				{
					strToChar.Format("You get %s from the corpse.\r\n",
						(char *)pObj->GetDesc());
					pCh->SendToChar(strToChar);
					pCh->GetRoom()->SendToRoom("%s gets %s from a corpse.\r\n",
						pCh,pObj->GetDesc());
				}
				ObjTaken.Add(pObj);
			}
			else
			{
				pCh->SendToChar("You can't seem to lift it\r\n");
			}
		}
		pos = ObjTaken.GetStartPosition();
		while(pos)
		{
			pObj = ObjTaken.GetNext(pos);
			Remove(pObj);
		}
		//Just call SaveCorpses once instead over every time we remove something
		CObjectManager::SaveCorpses();
		pCh->Save();
	}
}

///////////////////////////////
//	Allows players to loot one thing etc
void CCorpse::Loot(CString str, short nHowMany, CCharacter *pCh)
{
	CString strToChar;
	CObject *pObj;
	bool bSaveCorpse = false;
	//Is it cash?
	if(m_CashInContainer.IsCash(str))
	{
		sMoney Cash;
		try
		{
			m_CashInContainer.FromString(str,Cash,nHowMany);
		}
		catch(CError *pErr)
		{
			pCh->SendToChar(CString(pErr->GetMessage()));
		}
		catch(char *strMoney)
		{
			//can they take the wieght?
			if(pCh->Add(Cash))
			{
				strToChar.Format("You loot some %s.\r\n",strMoney);
				pCh->SendToChar(strToChar);
				pCh->GetRoom()->SendToRoom("%s loots some %s.\r\n",pCh,CString(strMoney));
				bSaveCorpse = true;
			}
			else
			{
				pCh->SendToChar("You can't seem to carry that many coins!\r\n");
				//add the cash back to the corpse
				m_CashInContainer+=Cash;
			}
		}
	}
	else if(nHowMany==-1) //meaning "all" at this point
	{
		short nTimes = 0;
		//get objects with alias "str" until there are no
		//more
		CObject *pObj2;
		while((pObj2 = FindInContents(str,1)))
		{
			//do this do when we run out of objects
			//pObj2 will be null but pObj won't be
			pObj = pObj2;
			//can they lift it?
			if(pCh->Add(pObj,false))
			{
				nTimes++;
				Remove(pObj);
			}
			else
			{
				strToChar.Format("%s is too heavy for you.\r\n",
					pObj->GetObjName());
				pCh->SendToChar(strToChar);
				if(nTimes==0)
				{
					//return here so we don't it the last else
					//if nTimes = 0
					return;
				}
			}
		}
		if(nTimes > 1)
		{
			strToChar.Format("You get (%d) %s from the corpse.\r\n",nTimes,
				pObj->GetObjName());
			pCh->SendToChar(strToChar);
			strToChar.Format("%s gets (%d) %s from the corpse.\r\n",
				"%s",nTimes,pObj->GetObjName());
			pCh->GetRoom()->SendToRoom(strToChar,pCh);
			bSaveCorpse = true;
		}
		else if(nTimes==1)
		{
			strToChar.Format("You get %s from the corpse.\r\n",
				pObj->GetObjName());
			pCh->SendToChar(strToChar);
			pCh->GetRoom()->SendToRoom("%s gets %s from the corpse.\r\n",pCh,pObj->GetObjName());
			bSaveCorpse = true;
		}
		else
		{
			pCh->SendToChar("Loot What?\r\n");
		}
	}
	else if(pObj = FindInContents(str,nHowMany))
	{
		if(pCh->Add(pObj,false))
		{
			Remove(pObj);
			strToChar.Format("You get %s from the corpse.\r\n",
				pObj->GetObjName());
			pCh->SendToChar(strToChar);
			pCh->GetRoom()->SendToRoom("%s gets %s from the corpse.\r\n",pCh,pObj->GetObjName());
			bSaveCorpse = true;
		}
		else
		{
			strToChar.Format("%s is too heavy for you.\r\n",
					pObj->GetObjName());
				pCh->SendToChar(strToChar);
		}
	}
	else
	{
		pCh->SendToChar("Loot what?\r\n");
	}
	if(bSaveCorpse)
	{
		CObjectManager::SaveCorpses();
		pCh->Save();
	}
}

/////////////////////////////////
//	You can't open and close corpses so
//	we'll call the CObject::Open
void CCorpse::Open(const CCharacter *pCh, bool bOpen)
{
	CObject::Open(pCh,bOpen);
}

/////////////////////////////
//	Virtual fadaffects
void CCorpse::FadeAffects(long lTimeToRemove)
{
	CORPSE_TIME-=lTimeToRemove;
	if(CORPSE_TIME<=0)
	{
		SetDelete();
	}
	CContainer::FadeAffects(lTimeToRemove);
}

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
// ShopRoom.cpp: implementation of the CShopRoom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "shop.room.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShopRoom::CShopRoom() : CRoom()
{
	m_ObjectToSell.SetDeleteData();
	m_bCanPillage = false;
	m_lShopKeeperVnum = -1;
	m_nPerctBuy = m_nPerctSell = 0;
}

CShopRoom::~CShopRoom()
{

}

void CShopRoom::ReadShop(CAscii & Infile,const CObjectManager *pObjManager)
{
	CString temp(MAX_FILE_STRING_SIZE);
	short nLoot;
	Infile.getline(temp,MAX_FILE_STRING_SIZE);
	if(temp==(char *)"ENDFILE")
	{
		if(Infile.rdstate()!=ios::eofbit)
		{
			Infile.clear(ios::eofbit);
		}
		//if we are at end of file throw out
		CError Err(NONERROR,"ENDFILE Marker hit!");
		throw &Err;
	}
	Infile.NumericFromBuf(temp,"%l",&m_lVnum);
	
	Infile.Numeric("%b %b %l %b",&m_nPerctSell,&m_nPerctBuy,&m_lShopKeeperVnum,&nLoot);
	m_bCanPillage = nLoot>0 ? true : false;
	obj_vnum nObjVnum;
	room_vnum lRoomVnum;
	short nNumToSell;
	Infile.getline(temp,MAX_FILE_STRING_SIZE);
	//must at least sell one thing
	sObjToSell *pObj;
	while((!temp.Compare("ENDSHOP")) && Infile.good())
	{
		Infile.NumericFromBuf(temp,"%d %b %l",&nObjVnum,&nNumToSell,&lRoomVnum);
		if((*pObjManager)[nObjVnum])
		{
			m_ObjectToSell.Add((pObj = new sObjToSell(nObjVnum,lRoomVnum,nNumToSell,pObjManager->GetObjName(nObjVnum),pObjManager->GetValue(nObjVnum))));
			pObj->IncreaseBy(m_nPerctSell);
		}
		else
		{
			ErrorLog << "Trying to load an obj into shop Vnum: " << m_lVnum << " that doens't exist. Object Vnum: " << nObjVnum << endl;
		}
		Infile.getline(temp,MAX_FILE_STRING_SIZE);
	}
}

CShopRoom::CShopRoom(CRoom * pRoom, CShopRoom &shop) : CRoom(pRoom)
{
	m_ObjectToSell.SetDeleteData();
	m_bCanPillage = shop.m_bCanPillage;
	m_lShopKeeperVnum = shop.m_lShopKeeperVnum;
	m_nPerctBuy = shop.m_nPerctBuy;
	m_nPerctSell = shop.m_nPerctSell;
	POSITION pos = shop.m_ObjectToSell.GetStartPosition();
	while(pos)
	{
		m_ObjectToSell.Add(new sObjToSell((*shop.m_ObjectToSell.GetNext(pos))));
	}
	m_nRoomType = TYPE_SHOP;
}

//////////////////////////////
//	DoList
//	Show All items that are sellable in this shop
//////////////////////////////
void CShopRoom::DoList(CString & strList)
{
	strList = "Items for sale:\r\n";
	POSITION pos = m_ObjectToSell.GetStartPosition();
	sObjToSell *pObj;
	short nCount=0;
	CString strMoney;
	while(pos)
	{
		pObj = m_ObjectToSell.GetNext(pos);
		pObj->MoneyString(strMoney);
		strList.Format("%s(%d) %s Cost: %s\r\n",
			(char *)strList,++nCount,
			(char *) pObj->GetDescription(),
			(char *)strMoney);
	}
}

bool CShopRoom::IsShop() const
{
	return true;
}

/////////////////////////
//	GetCost
//	takes the number that shows up in the
// list command and returns the cost of the
//	item so we can check to see if we have 
//	enough cash.  -1 is return if the number
//	is not listed.
//	written by: Demetrius Comes
////////////////////////////////
long CShopRoom::GetCost(short nNum)
{
	short nCount = 0;
	POSITION pos = m_ObjectToSell.GetStartPosition();
	sObjToSell *pObjSell;
	while(pos)
	{
		pObjSell = m_ObjectToSell.GetNext(pos);
		if(++nCount==nNum)
		{
			return pObjSell->GetValue();
		}
	}
	return -1;
}

//////////////////////////////
//	Buy
//	take the number found in the list
//	command and returns the Objects Vnum
//	and removes the object from the list
//	written by: Demetrius Comes
obj_vnum CShopRoom::Buy(short nNum,room_vnum &lCreateIn)
{
	short nCount = 0;
	obj_vnum nVnum = -1;
	POSITION pos = m_ObjectToSell.GetStartPosition();
	sObjToSell *pObjSell;
	while(pos)
	{
		pObjSell = m_ObjectToSell.GetNext(pos);
		if(++nCount==nNum)
		{
			nVnum = pObjSell->GetVnum();
			lCreateIn = pObjSell->GetCreateRoom();
			if(pObjSell->GetNumber()!=-1)
			{
				--(*pObjSell);
			}
			if(pObjSell->GetNumber()==0)
			{
				m_ObjectToSell.Remove(pObjSell,pos);
			}
			pos = NULL;
		}
	}
	return nVnum;
}

///////////////////////////
//	Run though NPC's in room 
//	to see if shop keeper here
////////////////////////////
bool CShopRoom::IsShopKeeperInRoom()
{
	POSITION pos = m_CharactersInRoom.GetStartPosition();
	CCharacter *pCh;
	while(pos)
	{
		pCh = m_CharactersInRoom.GetNext(pos);
		if(pCh->IsNPC() && pCh->GetVnum()==m_lShopKeeperVnum)
		{
			return true;
		}
	}
	return false;
}

//////////////////////////////
//	AddToSell
//	return the selling price
//	and add the object to the sell list
//
base_money CShopRoom::AddToSell(CObject * pObj,const CObjectManager *pObjectManager)
{
	POSITION pos = m_ObjectToSell.GetStartPosition();
	sObjToSell *pSell;
	bool bFound=false;
	while(pos && !bFound)
	{
		pSell=m_ObjectToSell.GetNext(pos);
		if(pSell->GetVnum()==pObj->GetVnum())
		{
			bFound=true;
			++(*pSell);
		}
	}
	if(!bFound)
	{
		m_ObjectToSell.Add((pSell = new sObjToSell(pObj->GetVnum(),-1, //we don't buy back boats etc that are loaded into rooms
			1,pObjectManager->GetObjName(pObj->GetVnum()),pObjectManager->GetValue(pObj->GetVnum()))));
		pSell->IncreaseBy(m_nPerctSell);
	}
	base_money lCost = pObjectManager->GetValue(pObj->GetVnum());
	return (lCost-=((lCost*m_nPerctBuy)/100));
}

bool CShopRoom::CanCampInRoom()
{
	return false;
}

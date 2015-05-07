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
#include "money.h"

CString sMoney::m_strPlat("platinum");
CString sMoney::m_strGold("gold");
CString sMoney::m_strSilver("silver");
CString sMoney::m_strCopper("copper");
const short sMoney::PLAT_IN_BASE=1000;
const short sMoney::GOLD_IN_BASE=100;
const short sMoney::SILVER_IN_BASE=10;

///////////////////////
//	Overloaded operator =
//	Convers base copper
//	to other currency
////////////////////////
sMoney & sMoney::operator =(base_money lValue)
{
	Base = lValue;
	UINT lRemainder = lValue%PLAT_IN_BASE;
	Plat = lValue/PLAT_IN_BASE;
	lValue = lRemainder;
	lRemainder = lValue%GOLD_IN_BASE;
	Gold = lValue/GOLD_IN_BASE;
	lValue = lRemainder;
	Silver = lValue/SILVER_IN_BASE;
	Copper = lValue%SILVER_IN_BASE;	
	return *this;
}

///////////////////////
//	Overloaded operator =
//	Convers base copper
//	to other currency
////////////////////////
sMoney & sMoney::operator +=(base_money lValue)
{
	Base += lValue;
	UINT lRemainder = lValue%PLAT_IN_BASE;
	Plat += lValue/PLAT_IN_BASE;
	lValue = lRemainder;
	lRemainder = lValue%GOLD_IN_BASE;
	Gold += lValue/GOLD_IN_BASE;
	lValue = lRemainder;
	Silver += lValue/SILVER_IN_BASE;
	Copper += lValue%SILVER_IN_BASE;	
	return *this;
}

///////////////////////////////////
//	IncreaseBy
//	Used by shop keepers to
//	increase the cost of the object etc
//	
void sMoney::IncreaseBy(short nPercent)
{
	*this+=(Base+=(nPercent*Base/100));
}

//////////////////////////////////
//	Builds Money String
/////////////////////////////////
CString sMoney::GetMoneyString()
{
	CString strMoney;
	strMoney.Format("&WPlatinum: %u&n &YGold: %u&n &wSilver: %u&n &yCopper: %u&n",
		Plat,Gold,Silver,Copper);
	return strMoney;
}

//////////////////////////////////
//	RoomString
////////////////////////////////
void sMoney::RoomString(CString & str)
{
	if(Base==1)
	{
		str+="A single coin lies here.\r\n";
	}
	else if(Base>0)
	{
		str+="A pile of coins lie here.\r\n";
	}
}

//////////////////////////
//	Add Cash
////////////////////////
sMoney & sMoney::operator +=(const sMoney & money)
{
	Copper += money.Copper;
	Silver += money.Silver;
	Gold += money.Gold;
	Plat += money.Plat;
	ReCalcBase();
	return *this;
}

//////////////////////////////
//	FromString
//	check the string ...Is it cash we are talking about?
//	if so exchange.  Take from this monehy add add to other
//	The remove functions through a CError if not enough cash.
//	written by: Demetrius Comes
void sMoney::FromString(CString & strMoney, sMoney & ToAdd, int nCoins)
{
	if(strMoney.Compare(m_strPlat.Left(strMoney.GetLength())))
	{
		ToAdd.AddPlat(RemovePlat(nCoins));
		throw "platinum";
	}
	else if(strMoney.Compare(m_strGold.Left(strMoney.GetLength())))
	{
		ToAdd.AddGold(RemoveGold(nCoins));
		throw "gold";
	}
	else if(strMoney.Compare(m_strSilver.Left(strMoney.GetLength())))
	{
		ToAdd.AddSilver(RemoveSilver(nCoins));
		throw "silver";
	}
	else if(strMoney.Compare(m_strCopper.Left(strMoney.GetLength())))
	{
		ToAdd.AddCopper(RemoveCopper(nCoins));
		throw "copper";
	}
	else if(strMoney.Compare("coins"))
	{
		if(nCoins!=-1 && nCoins!=1)
		{
			CError Err(MESSAGE_ONLY,"You can't specify coins without type!\r\n");
			throw &Err;
		}
		if(IsEmpty())
		{
			CError Err(MESSAGE_ONLY,"You can't find any coins.\r\n");
			throw &Err;
		}
		ToAdd+=*this;
		*this=0;
		throw "coins";
	}
}

bool sMoney::IsCash(CString &str)
{
	if(str.IsEmpty())
	{
		return false;
	}
	else if(str.Compare(m_strPlat.Left(str.GetLength())) 
		|| str.Compare(m_strGold.Left(str.GetLength()))
		|| str.Compare(m_strSilver.Left(str.GetLength()))
		|| str.Compare(m_strCopper.Left(str.GetLength()))
		|| str.Compare("coins"))
	{
		return true;
	}
	return false;
}

///////////////////////////
//	Add to sMoney together 
sMoney sMoney::operator +(const sMoney &r)
{
	sMoney Add(*this);
	Add.Copper += r.Copper;
	Add.Gold += r.Gold;
	Add.Silver += r.Silver;
	Add.Plat += r.Plat;
	Add.ReCalcBase();
	return Add;
}

///////////////////////////
//	Divide everything by nNUm
sMoney sMoney::operator /(int nNum)
{
	sMoney Div(*this);
	Div.Copper/=nNum;
	Div.Gold/=nNum;
	Div.Silver/=nNum;
	Div.Plat/=nNum;
	Div.ReCalcBase();
	return Div;
}

////////////////////////////
//	REturn the modules
sMoney sMoney::operator %(const int r)
{
	sMoney Mod(*this);
	Mod.Copper%=r;
	Mod.Gold%=r;
	Mod.Silver%=r;
	Mod.Plat%=r;
	Mod.ReCalcBase();
	return Mod;
}

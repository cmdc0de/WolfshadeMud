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
// ShopRoom.h: interface for the CShopRoom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHOPROOM_H__84882743_36BD_11D2_81AA_00600834E9F3__INCLUDED_)
#define AFX_SHOPROOM_H__84882743_36BD_11D2_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "room.h"

class CShopRoom : public CRoom  
{
	friend class CWorld;
private:
	struct sObjToSell
	{
	private:
		room_vnum m_lRoomVnum; //Room to create object in -1 if in buyers inventory
		obj_vnum m_nObjVnum;
		sMoney m_Value;
		short m_nNumber;
		CString m_strDescription;
	public:
		sObjToSell(obj_vnum nObjVnum,room_vnum lRoomVnum, short nNum,CString str,base_money lValue) 
		{m_nObjVnum=nObjVnum;m_lRoomVnum=lRoomVnum,m_nNumber=nNum;m_strDescription=str;m_Value = lValue;}
		sObjToSell(const sObjToSell& r)
		{m_nObjVnum=r.m_nObjVnum;m_lRoomVnum=r.m_lRoomVnum;m_nNumber=r.m_nNumber;m_strDescription=r.m_strDescription;m_Value=r.m_Value;}
		room_vnum GetCreateRoom() {return m_lRoomVnum;}
		obj_vnum GetVnum() {return m_nObjVnum;}
		short GetNumber() {return m_nNumber;}
		base_money GetValue() {return m_Value.GetBase();}
		void IncreaseBy(short nIncrease) {m_Value.IncreaseBy(nIncrease);}
		void MoneyString(CString &str) {str=m_Value.GetMoneyString();}
		CString &GetDescription() {return m_strDescription;}
		sObjToSell &operator ++() {m_nNumber++;return *this;}
		sObjToSell &operator --() {m_nNumber--;return *this;}
	};
protected:
	short m_nPerctSell,
		m_nPerctBuy;
	mob_vnum m_lShopKeeperVnum;
	bool m_bCanPillage;
	CLinkList<sObjToSell> m_ObjectToSell;
protected:
	void ReadShop(CAscii &Infile,const CObjectManager *pObjManager);
	CShopRoom(CRoom *pRoom,CShopRoom &shop);
	CShopRoom();
public:
	virtual bool CanCampInRoom();
	base_money AddToSell(CObject *pObj, const CObjectManager *pObjectManager);
	bool IsShopKeeperInRoom();
	bool CanPillage() {return m_bCanPillage;}
	obj_vnum Buy(short nNum,room_vnum &lCreateRoom);
	base_money GetCost(short nNum);
	virtual bool IsShop() const;
	virtual void DoList(CString &strList);
	virtual ~CShopRoom();
};

#endif // !defined(AFX_SHOPROOM_H__84882743_36BD_11D2_81AA_00600834E9F3__INCLUDED_)

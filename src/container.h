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
// Container.h: interface for the CContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTAINER_H__C7342DA1_4671_11D2_81AA_00600834E9F3__INCLUDED_)
#define AFX_CONTAINER_H__C7342DA1_4671_11D2_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "object.h"
#include "money.h"
#include "list.h"

class CContainer : public CObject
{
protected:
	static const int CONTAINER_OPEN;//a	1
	static const int CONTAINER_CLOSED;//	b	2
	static const int CONTAINER_LOCKED;//	c	4
	static const int CONTAINER_UNPICKABLE;//d	8
protected:
	sMoney m_CashInContainer;
	int m_nCurrentWeight;
	CList<CObject *> Contents;
protected:
	void Remove(CObject *pObj,POSITION &pos);
	CContainer(CCharacter *pCh);
public:
	virtual bool Add(CObject *pObj);
	virtual void Remove(CObject *pObj);
	virtual int GetContentsCount() {return Contents.GetCount();}
	virtual void GetContents(const CCharacter *pLooker,CString &str);
	virtual void Examine(const CCharacter *pLooker,CString & strExamine);
	virtual ~CContainer();
	virtual void  LoadContents(std::ifstream &ObjFile, short nCount);
	virtual void SaveContents(std::ofstream &ObjFile);
	virtual void DeleteContents();
	virtual int GetWeight() const {return m_nCurrentWeight;} 
	virtual void Open(const CCharacter *pCh,bool bOpening);
public:
	CObject * IsContentsAffectedBy(short nAffect);
	bool IsOpen() {return ((m_nVal1 & CONTAINER_OPEN)==CONTAINER_OPEN);}
	bool IsClosed() {return ((m_nVal1 & CONTAINER_OPEN)!=CONTAINER_OPEN);}
	bool IsLocked() {return ((m_nVal1 & CONTAINER_LOCKED)==CONTAINER_LOCKED);}
	bool CanBeOpened() {return ((m_nVal1 & CONTAINER_UNPICKABLE)!=CONTAINER_UNPICKABLE);}
	void GiveContentsToCarrier();
	bool Contains(CObject *pObj) {return Contents.DoesContain(pObj);}
	CContainer(const sObjectSave & ObjSave, CCharacter *pChar,CRoom *pRoom);
	CObject * FindInContents(CString &strObj,short nNum);
	CContainer(CObjectPrototype *obj, CCharacter *PersonCarriedby=NULL, CRoom *pInRoom = NULL);
};
#endif // !defined(AFX_CONTAINER_H__C7342DA1_4671_11D2_81AA_00600834E9F3__INCLUDED_)


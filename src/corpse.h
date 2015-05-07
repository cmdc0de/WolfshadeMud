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
// corpse.h: interface for the CCorpse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CORPSE_H__42912981_4531_11D3_9733_0000E85B2B37__INCLUDED_)
#define AFX_CORPSE_H__42912981_4531_11D3_9733_0000E85B2B37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "container.h"

class CCorpse : public CContainer  
{
private:
	bool m_bWasPC;
public:
	virtual bool Add(CObject *pObj);
	virtual void Remove(CObject *pObj);
	virtual void DeleteContents();
public:
	virtual void FadeAffects(long lTimeToRemove);
	virtual void Open(const CCharacter *pCh,bool bOpen);
	virtual void Loot(CString str, short nHowMany, CCharacter *pCh);
	virtual void Loot(CCharacter *pCh,bool bFromRess = false);
	bool WasPc() {return m_bWasPC;}
	void AddTime(int nPulses);
	short GetCorpseLevel();
	CCorpse(CCharacter *pCh);
	CCorpse(const sObjectSave & ObjSave, CRoom *pRoom);
	virtual ~CCorpse();

};

#endif // !defined(AFX_CORPSE_H__42912981_4531_11D3_9733_0000E85B2B37__INCLUDED_)

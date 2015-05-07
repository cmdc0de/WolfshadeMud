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
//////////////////////////////////////////////////////////////// armor.h: interface for the CArmor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARMOR_H__3B26B861_93EA_11D3_9733_0000E85B2B37__INCLUDED_)
#define AFX_ARMOR_H__3B26B861_93EA_11D3_9733_0000E85B2B37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "object.h"

///////////////////////////////////////////
//	Armor Class
////////////////////////////////////////////
//changes to cobject ...wearaffects needs to be virtual
//remove shadow armor functions and affects
//add object affect is_being_worn

class CArmor : public CObject
{
	friend class CObjectManager;
private:
	static int SHADOW_ARMOR_PULSE;
protected:
	//we are moving towards removing m_Val0-4 in CObject class b/c CObject class will be
	//abstract
	short m_nACAjustment;
protected:
	CArmor(CObjectPrototype *obj, CCharacter *PersonCarriedby=NULL, CRoom *pInRoom = NULL);
public:
	CArmor(const sObjectSave &ObjSave,CCharacter *pChar,CRoom *pRoom);
public:
	virtual void WearAffects(bool bRemoving);
	virtual void FadeAffects(long lTimeToRemove);
	virtual ~CArmor();
};
#endif // !defined(AFX_ARMOR_H__3B26B861_93EA_11D3_9733_0000E85B2B37__INCLUDED_)

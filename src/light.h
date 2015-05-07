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
// light.h: interface for the CLight class.
//
// For any kind of object that main purpose is a light
//	over view of how light works...The light when worn adds
//	the affect CCharacter::AFFECT_LIGHT to the character...
//	in turn the ApplyFnc for CCharacter::AFFECT_LIGHT removes
//	and readds the players to there current room.  The CRoom::Add
//	fnc checks the character for CCharacter::AFFECT_LIGHT and add it's
//	value to CRoom::ROOM_AFFECT_LIGHT...we then use CRoom::ROOM_AFFECT_LIGHT
//	to calculate whether the room is LIT, BRIGHT or DARK
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHT_H__730A7E21_573F_11D3_9733_0000E85B2B37__INCLUDED_)
#define AFX_LIGHT_H__730A7E21_573F_11D3_9733_0000E85B2B37__INCLUDED_

#include "wolfshade.string.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "object.h"

class CLight : public CObject  
{
	friend class CObjectManager;
protected:
	CLight(CObjectPrototype *obj, CCharacter *PersonCarriedby=NULL, CRoom *pInRoom = NULL);
	CLight(const sObjectSave & ObjSave, CCharacter *pChar,CRoom *pRoom);
	bool IsLit() {return (m_nVal2!=LIGHT_BURNED_OUT && m_nVal2!=LIGHT_NOT_LIT);}
public: //statics
	static const int LIGHT_LIT;
	static const int LIGHT_NOT_LIT;
	static const int LIGHT_BURNED_OUT;
	static const int FOREVER_LIT;
public:
	virtual void AddCharAffects(bool bRemoving,bool bSendMsg);
	virtual void FadeAffects(long lTimeToRemove);
	virtual CString GetExtraString();
	virtual ~CLight();

};

#endif // !defined(AFX_LIGHT_H__730A7E21_573F_11D3_9733_0000E85B2B37__INCLUDED_)

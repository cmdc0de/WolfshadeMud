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
#ifndef _MISC_OBJECTS_H_
#define _MISC_OBJECTS_H_

#include "object.h"
class CPotion : public CObject
{
	friend class CObjectManager;
private:
	struct sPotionSpecial
	{
		void (CPotion::*m_pFnc)(CCharacter *pUser);
		sPotionSpecial(void (CPotion::*p)(CCharacter *pU)=NULL){m_pFnc = p;}
	};
	friend struct sPotionSpecial;
private:
	void InitPotionStatics();
	static bool m_bInitPotion;
protected:
	static const short POTION_ARMOR;
	static const short POTION_BARK_SKIN;
	static const short POTION_SPIRIT_ARMOR;
	static const short POTION_STONE_SKIN;
	static const short POTION_CURE;
	static const short POTION_INVIS;
	static const short POTION_DI;
	static const short POTION_DETECT_MAGIC;
	static const short POTION_SPECIAL_PROC;
	static CMap<obj_vnum,sPotionSpecial> *m_pSpecialProc;
protected:
	CPotion(const sObjectSave & ObjSave, CCharacter *pChar,CRoom *pRoom);
	CPotion(CObjectPrototype *obj, CCharacter *PersonCarriedby=NULL, CRoom *pInRoom = NULL);
	void GreaterThan50(CCharacter *pUser);
public:
	virtual void Use(CCharacter *pUser);
	virtual bool IsValidObject();
public:
	~CPotion();
};
#endif
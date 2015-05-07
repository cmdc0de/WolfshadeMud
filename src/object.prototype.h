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
/***************************************************
*
*  CObjectPrototype.h
*
*  Written 8/4/97
*
*  by: DAC
*****************************************************/

#ifndef _OBJECTPROTOTYPE_H_
#define _OBJECTPROTOTYPE_H_

#include "linklist.h"

#define ITEM_TYPE_CORPSE 0
//#define ITEM_TYPE_FOOD	1 UNUSED
#define ITEM_TYPE_LIGHT	2
#define ITEM_TYPE_SCROLL	3
//#define ITEM_TYPE_WAND	4 UNUSED
#define ITEM_TYPE_STAFF	5
#define ITEM_TYPE_ARMOR	6
//#define ITEM_TYPE_DRINKCONTAINER	UNUSED
#define ITEM_TYPE_CONTAINER	8
#define ITEM_TYPE_FOUNTAIN	9
#define ITEM_TYPE_WEAPON	10
#define ITEM_TYPE_POTION	11
#define ITEM_TYPE_PORTAL    12
#define ITEM_TYPE_SPELL_OBJ 13
#define ITEM_TYPE_TRANSPORT 14

//affect locations
#define AFF_LOC_NONE 0
#define AFF_LOC_STRADD 1
#define AFF_LOC_STR 2
#define AFF_LOC_DEX 3
#define AFF_LOC_INT 4
#define AFF_LOC_WIS 5
#define AFF_LOC_CON 6
#define AFF_LOC_CHA 7
#define AFF_LOC_LUCK 8
#define AFF_LOC_CLASS 9
#define AFF_LOC_LEVEL 10
#define AFF_LOC_AGE 11
#define AFF_LOC_WEIGHT 12
#define AFF_LOC_HIEGHT 13
#define AFF_LOC_MANA 14
#define AFF_LOC_HITPOINTS 15
#define AFF_LOC_MOVE 16
#define AFF_LOC_EXP 17
#define AFF_LOC_AC 18
#define AFF_LOC_HITROLL 19
#define AFF_LOC_DAMROLL 20
#define AFF_LOC_SAVE_PARA 21
#define AFF_LOC_SAVE_ROD 22
#define AFF_LOC_SAVE_BREATHE 23
#define AFF_LOC_SAVE_SPELL 24
#define AFF_LOC_SAVE_POLY 25

//Materials
#define NONE 0
#define WOOD 1
#define STEEL 2
#define SPANISH_STEEL 3
#define ADIMANTITE 4
#define MITHRIL 5
#define LEATHER 6
#define GOLD 7
#define NICKLE 8
#define TOTAL_MATERIALS 9
#define TOTAL_AFFECT_TYPES 27
#define TOTAL_OBJECT_TYPES 15

class CRoom;
//storage for the affects an object will produce when worn
struct sObjectWearAffects
{
	friend class CObjectPrototype;
	friend class CObject;
protected:
	short int Location, Affect;
public:
	bool IsEmpty() const {return Affect==0;}
	sObjectWearAffects(){Location = 0;Affect = 0;}
	sObjectWearAffects(const sObjectWearAffects *right)
	{Location = right->Location;Affect = right->Affect;}
};

//stores objects descriptions
class CObjectDesc
{
	friend class CObjectPrototype;
	friend class CObject;
protected:
	CString KeyWord,
		ExtraDesc;
public:
	CObjectDesc(){;} //do nothing
	CObjectDesc(const char *strKeyWord,const char*strExtraDesc)
	{KeyWord = strKeyWord;ExtraDesc = strExtraDesc;}
	CObjectDesc(CObjectDesc *r)
		{KeyWord = r->KeyWord;ExtraDesc = r->ExtraDesc;}
	CString GetKeyWord() const {return KeyWord;}
	CString GetExtraDesc() const {return ExtraDesc;}
	CObjectDesc &operator=(const CObjectDesc &r)
	{KeyWord = r.KeyWord;ExtraDesc=r.ExtraDesc;return *this;}
};

class CObjectPrototype
{
	//protected constructor to force all communication to go though
	//the manager class
	friend class CObjectManager;
private:
	static const char *MaterialNames[TOTAL_MATERIALS];
	static const char *TypeNames[TOTAL_OBJECT_TYPES];
	static const char *AffectNames[TOTAL_AFFECT_TYPES];
public:
	static const char *GetTypeName(short n);
	static const char *GetMaterialName(short n);
	static const char *GetAffectName(short n);
protected:
	CObjectPrototype();
	CObjectPrototype(CObjectPrototype *pObj);
	CObjectPrototype(short nAffect,CRoom *pRoom, int nPulses);
	CObjectPrototype(short nType, int nDam);
	void ReadObj(CAscii &in);
	void VStat(CString &ToChar);

public:
	int Vnum;
	short int ObjType, 
		Material,
		NumberInGame;
	
	int Cost,
		Weight, 
		Val0, 
		Val1, 
		Val2, 
		Val3, 
		Val4,
		AffBit,
		WearBit;
	
	CString ObjAlias, 
		ObjName, 
		ObjDesc, 
		ObjSittingDesc;

	CLinkList<sObjectWearAffects> ObjAffects;
	CLinkList<CObjectDesc> ObjExtraDesc;
public:
	short int GetNumberInGame() {return NumberInGame;}
	virtual ~CObjectPrototype();
};

#endif

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
//
//////////////////////////////////////////////////////////////
// ObjectSave.h: interface for the CObjectSave class.
//
//	structure is used to save all objects to disk...do not
//	use a class with in this struct or you will risk over
//	writing the v_ptr when you ifstream::read
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTSAVE_H__9DADEF63_BB52_11D2_81AB_00600834E9F3__INCLUDED_)
#define AFX_OBJECTSAVE_H__9DADEF63_BB52_11D2_81AB_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MAX_WEAR_AFFECTS 8
#define MAX_EXTRA_DESC 3

struct sObjectSave  
{
	friend class CObject;
protected:
	struct sSaveExtraDesc
	{
	protected:
		bool m_bEmpty;
		char m_strKeyWord[MAX_OBJ_SHORT_STR],
			m_strExtraDesc[MAX_OBJ_LONG_STR];
	public:
		sSaveExtraDesc() {m_bEmpty = true;}
		bool IsEmpty() const {return m_bEmpty;}
		const char *GetKeyWord() const {return m_strKeyWord;}
		const char *GetExtraDesc() const {return m_strExtraDesc;}
		sSaveExtraDesc &operator=(const CObjectDesc &r)
		{
			strcpy(m_strKeyWord,(char *)r.GetKeyWord());
			strcpy(m_strExtraDesc,(char *)r.GetExtraDesc());
			m_bEmpty = false;
			return *this;
		}
	};
public:
	int m_nVnum, 
		m_nVal0, 
		m_nVal1, 
		m_nVal2, 
		m_nVal3,
		m_nVal4,
		m_nWeight,
		m_nCost,
		m_nAffBit, 
		m_nWearBit, //could be multiple eq positions
		m_nWornOn; //What wear bit was it worn on

	long m_lSaveRoom;

	short m_nState,
		m_nObjType,
		m_nEqPos, //position in eq -1 if not worn
		m_nContents;
	//with eqpos and wornon bit we can set CEqInfo like it
	//was before save

	char m_strObjAlias[MAX_OBJ_SHORT_STR],
		m_strObjName[MAX_OBJ_SHORT_STR], 
		m_strObjDesc[MAX_OBJ_LONG_STR], 
		m_strObjSittingDesc[MAX_OBJ_LONG_STR];

	sAffect m_Affects[TOTAL_OBJ_AFFECTS];
	sSaveExtraDesc m_ExtraDesc[MAX_EXTRA_DESC];
	sObjectWearAffects m_WearAffects[MAX_WEAR_AFFECTS];
public:
	static const short INVENTORY;
	static const short IN_OBJ;
	static const short IN_ROOM;
	static const char *DIR;
public:
	sObjectSave();
	sObjectSave(CObject *pObj,int nWornOn, short nPos);
	long GetSaveRoom() {return m_lSaveRoom;}
	short GetContentsCount() {return m_nContents;}
	bool InventoryObj() {return m_nEqPos==INVENTORY;}
	bool IsType(short nType) {return nType==m_nObjType;}
	short GetWearPos() {return m_nEqPos;}
	int GetWornOnBit() {return m_nWornOn;}
	~sObjectSave();
};

#endif // !defined(AFX_OBJECTSAVE_H__9DADEF63_BB52_11D2_81AB_00600834E9F3__INCLUDED_)



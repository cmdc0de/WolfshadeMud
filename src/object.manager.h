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
/*
*
* CObjectManager
*	Manages all Object PRototypes
*
*/

#ifndef _OBJECT_MANAGER_H_
#define _OBJECT_MANAGER_H_
#include "object.prototype.h"
#include "error.h"
#include "hash.h"
#include "log.h"

class CObjectManager
{
private:
	struct sSpecialProc
	{
		void (CObjectManager::*m_pFncSpecialPrc)(CObject *pObj);
		sSpecialProc() {m_pFncSpecialPrc = NULL;}
		sSpecialProc(void (CObjectManager::*pFncSpecialPrc)(CObject *pObj))
		{m_pFncSpecialPrc = pFncSpecialPrc;}
	};
private:
	static CMap<int, sSpecialProc *> m_SpecialProcs;
	static bool m_bInit;
	static CLinkList<CObject> ObjectsInMud;
	static CList<CCorpse *> CorpsesInMud;
	static const char *CORPSE_FILE;
private:
	void Remove(CObject *pObj, POSITION pos);
	void InitStatics();
	void BootObjects();
	CMap<int,CObjectPrototype *> m_ObjectPrototypeMap;
	CString OBJINDEX;
public:
	static void Add(CObject *pObj, bool bCanSaveCorpses = true);
	static void Remove(CObject *pObj);
	static void SaveCorpses();
	static CObject * CalObjectPointer(CObjectPrototype *obj, CCharacter *PersonCarriedby, CRoom *pInRoom);
	static CObject * CalObjectPointer(const sObjectSave & ObjSave, CCharacter *pChar,CRoom *pRoom);
	static const CString DEFAULT_OBJINDEX;
public:
	void LoadCorpses();
	void ReduceObjectAffects(long lToReduce);
	CString LocateObject(CString strAlias,const CCharacter *pCh);
	void VStat(CString &ToChar,int lVnum);
	CObject * Create(obj_vnum nObjVnum, CRoom *pRealRoom, short nMax);
	void GetVnums(CString &strToChar,CString strAlias);
	CObject * Create(obj_vnum nObjVnum,CCharacter *NewMob);
	void CreatePortal(CRoom *pTargetRoom,CRoom *pCasterRoom, short nAffect, int nPulses);
	void CreateFireball(CRoom *pTargetRoom,int nPulses, int nValue);
	~CObjectManager();
	CObjectManager(CString strOpts);
	inline const CObjectPrototype *operator[] (obj_vnum lObjVnum) const;
	CString GetObjName(obj_vnum lObjVnum) const;
	inline base_money GetValue(obj_vnum lObjVnum) const;
protected:
	void DoSpecialProc(CObject *pObj);
	void AddGeneralProc(CObject *pObj);
};

inline base_money CObjectManager::GetValue(obj_vnum nObjVnum) const
{
	CObjectPrototype *pObjProto;
	if(m_ObjectPrototypeMap.Lookup(nObjVnum,pObjProto))
	{
		return pObjProto->Cost;
	}
	return 1;
}

inline const CObjectPrototype *CObjectManager::operator[](obj_vnum nObjVnum) const
{
	CObjectPrototype *pObjProto;
	if(m_ObjectPrototypeMap.Lookup(nObjVnum,pObjProto))
	{
		return pObjProto;
	}
	return NULL;
}
#endif
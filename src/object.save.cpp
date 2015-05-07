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
// ObjectSave.cpp: implementation of the CObjectSave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "object.save.h"

const short sObjectSave::INVENTORY = -1;
const short sObjectSave::IN_OBJ=-2;
const short sObjectSave::IN_ROOM = -3;
const char *sObjectSave::DIR = "players/plobj/";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

sObjectSave::sObjectSave(CObject *pObj, int nWornOn, short nPos)
{
	memset(m_strObjAlias,'\0',sizeof(char)*MAX_OBJ_SHORT_STR);
	memset(m_strObjName,'\0',sizeof(char)*MAX_OBJ_SHORT_STR); 
	memset(m_strObjDesc,'\0',sizeof(char)*MAX_OBJ_LONG_STR);
	memset(m_strObjSittingDesc,'\0',sizeof(char)*MAX_OBJ_LONG_STR);
	if(nWornOn==IN_ROOM)
	{
		assert(pObj->m_pInRoom);
		m_lSaveRoom = pObj->m_pInRoom->GetVnum();
	}
	m_nVnum = pObj->m_nVnum;
	m_nObjType = pObj->m_nObjType;
	m_nVal0 = pObj->m_nVal0;
	m_nVal1 = pObj->m_nVal1;
	m_nVal2 = pObj->m_nVal2;
	m_nVal3 = pObj->m_nVal3;
	m_nVal4 = pObj->m_nVal4;
	m_nWeight = pObj->m_nWeight;
	m_nCost = pObj->m_nCost;
	m_nAffBit = pObj->m_nAffBit;
	m_nWearBit = pObj->m_nWearBit;
	m_nState = pObj->m_nState;
	m_nWornOn = nWornOn;
	m_nEqPos = nPos;
	//we populate this so we get the correct objects
	//in the correct containers.
	m_nContents = pObj->GetContentsCount();

	strncpy(m_strObjAlias,(char *)pObj->m_strObjAlias,(pObj->m_strObjAlias.GetLength() > MAX_OBJ_SHORT_STR ? MAX_OBJ_SHORT_STR : pObj->m_strObjAlias.GetLength()));
	//make sure we put in null bit incase string is too long.
	m_strObjAlias[MAX_OBJ_SHORT_STR-1] = '\0';
	strncpy(m_strObjName, (char *)pObj->m_strObjName,(pObj->m_strObjName.GetLength() > MAX_OBJ_SHORT_STR ? MAX_OBJ_SHORT_STR : pObj->m_strObjName.GetLength()));
	m_strObjName[MAX_OBJ_SHORT_STR-1] = '\0';
	strncpy(m_strObjDesc, (char *)pObj->m_strObjDesc,(pObj->m_strObjDesc.GetLength()> MAX_OBJ_LONG_STR ? MAX_OBJ_LONG_STR : pObj->m_strObjDesc.GetLength()));
	m_strObjDesc[MAX_OBJ_LONG_STR-1] = '\0';
	strncpy(m_strObjSittingDesc,(char *)pObj->m_strObjSittingDesc,(pObj->m_strObjSittingDesc.GetLength()>MAX_OBJ_LONG_STR ? MAX_OBJ_LONG_STR : pObj->m_strObjSittingDesc.GetLength()));
	m_strObjSittingDesc[MAX_OBJ_LONG_STR-1] = '\0';

	memcpy(m_Affects,pObj->m_Affects,(sizeof(sAffect)*TOTAL_OBJ_AFFECTS));
	register short i=0;
	
	POSITION pos = pObj->ExtraDesc.GetStartPosition();
	while(pos && i<MAX_EXTRA_DESC)
	{
		m_ExtraDesc[i++] = (*(pObj->ExtraDesc.GetNext(pos)));
	}
	i=0;
	pos = pObj->ObjAffects.GetStartPosition();
	while(pos && i<MAX_WEAR_AFFECTS)
	{
		m_WearAffects[i++] = (*(pObj->ObjAffects.GetNext(pos)));
	}
}

sObjectSave::~sObjectSave()
{

}

sObjectSave::sObjectSave()
{
	m_nVnum = 0;
	m_nObjType = 0;
	m_nVal0 = 0;
	m_nVal1 = 0;
	m_nVal2 = 0;
	m_nVal3 = 0;
	m_nVal4 = 0;
	m_nWeight = 0;
	m_nCost = 0;
	m_nAffBit = 0;
	m_nWearBit = 0;
	m_nState = 0;
	m_nEqPos = INVENTORY;
	m_lSaveRoom = NOWHERE;

	memset(m_strObjAlias,'\0',sizeof(char)*MAX_OBJ_SHORT_STR);
	memset(m_strObjName, '\0',sizeof(char)*MAX_OBJ_SHORT_STR);
	memset(m_strObjDesc, '\0',sizeof(char)*MAX_OBJ_LONG_STR);
	memset(m_strObjSittingDesc,'\0',sizeof(char)*MAX_OBJ_LONG_STR);
}



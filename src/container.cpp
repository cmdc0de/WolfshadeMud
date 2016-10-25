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
// Container.cpp: implementation of the CContainer class.
//
//////////////////////////////////////////////////////////////////////
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "container.h"
#include "object.manager.h"
#include "object.save.h"

extern void StackObjects(const CList<CObject *> &Objs,const CCharacter *pLooker,CString &strLook,bool bInInventory);
#define CONTAINER_STATE m_nVal1
const int CContainer::CONTAINER_OPEN=(1<<0);//a	1
const int CContainer::CONTAINER_CLOSED=(1<<1);//	b	2
const int CContainer::CONTAINER_LOCKED=(1<<2);//	c	4
const int CContainer::CONTAINER_UNPICKABLE=(1<<3);//d	8


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContainer::~CContainer()
{

}

//////////////////////////////
//	Makes a corpse
//	transfers all eq to container
//////////////////////////////
CContainer::CContainer(CCharacter * pCh) 
	:CObject(pCh)
{
	
}


////////////////////////////
//	Overloaded so
//	we show the contents of the container
///////////////////////////
void CContainer::Examine(const CCharacter *pLooker, CString & strExamine)
{
	CObject::Examine(pLooker,strExamine);
	if(!m_CashInContainer.IsEmpty())
	{
		if(m_CashInContainer.GetBase()==1)
		{
			strExamine+="A single coin.\r\n";
		}
		else
		{
			strExamine+="A pile of coins.\r\n";
		}
	}
	StackObjects(Contents,pLooker,strExamine,true);
}

///////////////////////////
//	FindInContents
//	looks for an object by its alias 
//	in the contents of the container
//	written by: Demetrius Comes
///////////////////////////
CObject * CContainer::FindInContents(CString & strObj, short nNum)
{
	POSITION pos = Contents.GetStartPosition();
	short nCount = 0;
	CObject *pObj;
	while(pos)
	{
		pObj = Contents.GetNext(pos);
		if((pObj->GetAlias().Find(strObj) != -1))
		{
			if(++nCount==nNum)
			{
				return pObj;
			}
		}
	}
	return NULL;
}

CContainer::CContainer(CObjectPrototype *obj, CCharacter *PersonCarriedby, CRoom *pInRoom) :
		CObject(obj,PersonCarriedby,pInRoom)
{
	m_nCurrentWeight = m_nWeight;
}

CContainer::CContainer(const sObjectSave & ObjSave, CCharacter * pChar, CRoom *pRoom)
:CObject(ObjSave,pChar,pRoom)
{
	m_nCurrentWeight = m_nWeight;
}

//////////////////////////
//	Add Object to container
bool CContainer::Add(CObject * pObj)
{
	if((m_nCurrentWeight+pObj->GetWeight())<m_nVal0)
	{
		Contents.Add(pObj);
		//puting item into container so we give a weight adjust
		m_nCurrentWeight += pObj->GetWeight();
		pObj->Set(NULL,NULL);
		return true;
	}
	return false;
}

//////////////////////////
//	Remove object from container
void CContainer::Remove(CObject *pObj)
{
	Contents.Remove(pObj);
	//when we remove the object we subtract the weight
	//with the adjustment
	m_nCurrentWeight-=pObj->GetWeight();
	m_nCurrentWeight = m_nCurrentWeight<m_nWeight ? m_nWeight : m_nCurrentWeight;
}

void CContainer::Remove(CObject *pObj, POSITION &pos)
{
	Contents.Remove(pObj,pos);
	//when we remove the object we subtract the weight
	//with the adjustment
	m_nCurrentWeight-=pObj->GetWeight();
	m_nCurrentWeight = m_nCurrentWeight<m_nWeight ? m_nWeight : m_nCurrentWeight;
}

void CContainer::GetContents(const CCharacter *pLooker, CString &str)
{
	if(Contents.IsEmpty())
	{
		str += "Nothing.\r\n";
	}
	else
	{
		StackObjects(Contents,pLooker,str,true);
	}
}

///////////////////////////////////
//	Save Containers contents
//	This can be called recursively!
void CContainer::SaveContents(std::ofstream & ObjFile)
{
	POSITION pos = Contents.GetStartPosition();
	CObject *pObj;
	while(pos)
	{
		pObj = Contents.GetNext(pos);
		pObj->Save(ObjFile,sObjectSave::IN_OBJ,sObjectSave::IN_OBJ);
		//save the object and it's contents if needed.
		//The recursiveness comes in if we have a container
		//inside a container  
		//BUT WE ALWAYS SAVE THE OBJECT THEN IT'S CONTENTS!
		pObj->SaveContents(ObjFile);
	}
}

////////////////////////////////
//	LoadContens
//  Could be called recrusively
void CContainer::LoadContents(std::ifstream & ObjFile, short nCount)
{
	sObjectSave ObjSave;
	CObject *pObj;
	for(int i =0;i<nCount;i++)
	{
		ObjFile.read((char *)&ObjSave,sizeof(sObjectSave));
		
		if(!(pObj = CObjectManager::CalObjectPointer(ObjSave,NULL,NULL)))
		{
			ErrorLog << "Trying to create unknow object in CContainer::LoadContents." << endl;
		}
		else
		{
			this->Add(pObj); //add to the Contents LL
			//false passed so we don't try to save if object is a corpse
			// why b/c when game boots it calls Load contents and it's reading from
			//	the corpse file...we don't want to write to the
			//  same file we are readiing from...=)
			CObjectManager::Add(pObj,false);
			//since we save the object then it's contents
			//we must load the object then load it's contents!
			pObj->LoadContents(ObjFile,ObjSave.GetContentsCount());
		}
	}
}

//////////////////////////////
//	Give contents to person carring
void CContainer::GiveContentsToCarrier()
{
	//in sure we are carried by someone.
	assert(m_pCarriedby);
	POSITION pos = Contents.GetStartPosition();
	CObject *pObj;
	int nCount = 0;
	while(pos)
	{
		pObj = Contents.GetNext(pos);
		//move containers objects to carrier
		//remove the container from the carried so we don't have double wieght
		m_pCarriedby->Remove(this);
		if(m_pCarriedby->Add(pObj))
		{
			nCount++;
			Remove(pObj,pos);
		}
		else
		{
			m_pCarriedby->SendToChar("You can't seem to carry any more.\r\n");
			if(nCount==0)
			{
				m_pCarriedby->Add(this);
				return;
			}
		}
		m_pCarriedby->Add(this);
	}
	if(nCount==0)
	{
		m_pCarriedby->SendToChar("It's empty!\r\n");
	}
	else
	{
		//send messages
		CString str;
		str.Format("You get a bunch of stuff from %s.\r\n",
			GetObjName());
		m_pCarriedby->SendToChar(str);
		str.Format("%s gets a bunch of stuff from %s.\r\n",
			"%s",
			GetObjName());
		m_pCarriedby->GetRoom()->SendToRoom(str,m_pCarriedby);
	}
}

//////////////////////////////
//	Deletes the contents of a container
//	Possibly Recursive 
void CContainer::DeleteContents()
{
	POSITION pos = Contents.GetStartPosition();
	CObject *pObj;
	while(pos)
	{
		pObj = Contents.GetNext(pos);
		pObj->DeleteContents();
		Remove(pObj,pos);
		CObjectManager::Remove(pObj);
	}
}

////////////////////////////////
//	Opens the container
//	
void CContainer::Open(const CCharacter *pCh,bool bOpening)
{
	CString strToChar;
	if(bOpening && IsOpen())
	{
		strToChar = "It's already open!\r\n";
	}
	else if(!bOpening && IsClosed())
	{
		strToChar = "It's already closed!\r\n";
	}
	else
	{
		if(CanBeOpened() && !IsLocked())
		{
			if(bOpening)
			{
				CONTAINER_STATE = CONTAINER_OPEN;
				strToChar.Format("You open the %s\r\n",
					(char *)GetObjName());
				pCh->GetRoom()->SendToRoom("%s opens %s\r\n",pCh,GetObjName());
			}
			else
			{
				CONTAINER_STATE = CONTAINER_CLOSED;
				strToChar.Format("You close the %s\r\n",
					(char *)GetObjName());
				pCh->GetRoom()->SendToRoom("%s closes %s\r\n",pCh,GetObjName());
			}
		}
		else if(IsLocked())
		{
			strToChar = "It seems to be locked.\r\n";
		}
		else
		{
			strToChar = "It won't budge.\r\n";
		}
	}
	pCh->SendToChar(strToChar);
}

/////////////////////////////////////
//	Checks to see if any of the objects in this container
//	are affected by nAffect if so passes it back
CObject * CContainer::IsContentsAffectedBy(short nAffect)
{
	POSITION pos = this->Contents.GetStartPosition();
	CObject * pObj;
	while(pos)
	{
		pObj = this->Contents.GetNext(pos);
		if(pObj->IsAffectedBy(nAffect))
		{
			return pObj;
		}
	}
	return NULL;
}

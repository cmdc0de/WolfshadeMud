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
*  LinkList Class with no memory management
*
*/
//LinkList.h

#ifndef _LINKLIST_H_
#define _LINKLIST_H_

#include <iostream>

template <class BaseClass>
class CLinkList
{
private:
	struct Node
	{
	public:
		BaseClass *Data;
		Node *NextNode;
		Node(BaseClass *ptr){Data = NULL;Data = ptr;NextNode = NULL;}
	};
private:
	Node *m_pLast, *m_pFirst;
	int m_nCount;
	bool m_bDestroyAll;
		
public:
	CLinkList() {m_bDestroyAll = false;m_nCount = 0;m_pLast = m_pFirst = NULL;}
	~CLinkList() {Destroy();}
	void Destroy(void); 
	void SetDeleteData(bool data=true) {m_bDestroyAll = data;}
	void Add(BaseClass *ptr);
	void AddBegining(BaseClass *ptr);
	//use if not in a loop
	void Remove(BaseClass *ptr);
	//use if in a loop and you have a POSITION variable
	//for why look at implementation.
	void Remove(BaseClass *ptr,POSITION &pos);
	inline BaseClass* GetLast() const;
	inline BaseClass* GetNext(POSITION &pos) const;
	inline POSITION GetStartPosition() const;
	inline bool DoesContain(const BaseClass *ptr) const;
	BaseClass* GetFirst() const {return m_pFirst ? m_pFirst->Data : NULL;}
	bool IsEmpty() {return m_nCount==0;}
	int GetCount() {return m_nCount;}
};

template<class BaseClass>
inline bool CLinkList<BaseClass>::DoesContain(const BaseClass *ptr) const
{
	POSITION pos = GetStartPosition();
	while(pos)
	{
		if(ptr==GetNext(pos))
		{
			return true;
		}
	}
	return false;
}

template<class BaseClass>
inline BaseClass * CLinkList<BaseClass>::GetLast() const
{
	return m_pLast->Data;
}

template<class BaseClass>
inline BaseClass * CLinkList<BaseClass>::GetNext(POSITION &pos) const
{
	if(pos==NULL)
		return NULL;

	Node *tmp = ((Node *)pos);

	pos = ((POSITION) tmp->NextNode);
	return tmp->Data;
}

template<class BaseClass>
inline POSITION CLinkList<BaseClass>::GetStartPosition() const
{
	return m_nCount==0 ? NULL : ((POSITION) m_pFirst);
}

template<class BaseClass>
void CLinkList<BaseClass>::Destroy()
{
	Node *temp, *current;
	current = m_pFirst;
	while(current)
	{
		temp = current;
		current = current->NextNode;
		if(m_bDestroyAll)
		{
			delete temp->Data;
		}
		delete temp;
		m_nCount--;
	}
	m_pFirst = m_pLast = NULL;
#ifdef _DEBUG
	if(m_nCount!=0)
		ErrorLog << "Bad LL!" << endl;
#endif
}

template <class BaseClass>
void CLinkList<BaseClass>::Add(BaseClass *ptr)
{
	if(!ptr)
	{
		CError Err(CRITICAL_ERROR,"NULL pointer pass to add in LL class\n");
		throw &Err;
	}

	if(m_pFirst==NULL)
	{
		m_pFirst = new Node(ptr);
		assert(m_pFirst);
		m_pLast = m_pFirst;
	}
	else
	{
		m_pLast->NextNode = new Node(ptr);
		m_pLast = m_pLast->NextNode;
	}
	m_nCount++;
}

template <class BaseClass>
void CLinkList<BaseClass>::AddBegining(BaseClass *ptr)
{
	if(!ptr)
	{
		CError Err(CRITICAL_ERROR,"NULL pointer pass to add in LL class\n");
		throw &Err;
	}
	Node *tmp = m_pFirst;
	if(m_pFirst==m_pLast && !m_pFirst)
	{
		m_pFirst = m_pLast = new Node(ptr);
	}
	else
	{
		m_pFirst = new Node(ptr);
		m_pFirst->NextNode = tmp;
	}
	m_nCount++;
}

////////////////////////////////////
//	Remove()
//	Removes a Node that points to the
//	baseclass ptr passed to it.
//	DO NOT use this remove if you are
//	removing while in a loop.  Because you
//	might remove the Node that your current POSITION
//	variable points at.  Use overloaded Remove
//	written by: Demetrius Comes
///////////////////////////////////////
template <class BaseClass>
void CLinkList<BaseClass>::Remove(BaseClass *ptr)
{
	POSITION pos = NULL;
	Remove(ptr,pos);
}

////////////////////////////////////////////
//	Overloaded Remove()
//	This remove still removes the node that holdes
//	the baseclass ptr passed to it.  But if you are in 
//	a loop you will have a POSITION variable, and I have to
//	check to see if that POSITION (NODE) is the one we are going
//	to delete if so then I need to change it for you.
//	written by: Demetrius Comes
/////////////////////////////////////////////////
template <class BaseClass>
void CLinkList<BaseClass>::Remove(BaseClass *ptr,POSITION &pos) 
{
	if(ptr == NULL)
		return;

	Node *pToBeDeleted, *current, *prev;
	current = prev = m_pFirst;

	while(current)
	{
		if(current->Data == ptr)
		{
			pToBeDeleted = current;
			if(pToBeDeleted==m_pFirst)
			{
				if(m_pFirst == m_pLast)
				{
					m_pFirst = m_pLast = NULL;
				}
				else
				{
					m_pFirst = m_pFirst->NextNode;
				}
			}
			else
			{
				if(pToBeDeleted != m_pLast)
				{
					prev->NextNode = pToBeDeleted->NextNode;
				}
				else
				{
					prev->NextNode = NULL;
					m_pLast = prev;
				}
			}
			if(m_bDestroyAll)
			{
				delete pToBeDeleted->Data;
			}
			if(pos==((POSITION)pToBeDeleted))
			{
				pos = (POSITION)pToBeDeleted->NextNode;
			}
			delete pToBeDeleted;
			if(--m_nCount<=0)
			{
				m_pFirst = m_pLast = NULL;
			}
			return;
		}
		prev = current;
		current = current->NextNode;
	}
}
#endif
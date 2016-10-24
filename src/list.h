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
///////////////////////////////////////
//	LinkList class that can do other things
//	besides pointers...

#ifndef _LIST_H_
#define _LIST_H_

template<class T>
class CList
{
private:
	struct sNode
	{
	protected:
		T Value;
		sNode *m_pNext;
	protected:
		
	public:
		sNode(T &Val) {Value = Val;m_pNext = NULL;}
		sNode *&GetNext() {return m_pNext;}
		T &GetData() {return Value;}
		bool operator==(T &T1) {return Value==T1;}
	};
	friend struct sNode;
protected:
	int m_nCount;
	sNode *m_pFirst,
		*m_pLast;
public:
	void Destroy();
	CList() {m_pFirst = m_pLast = 0;m_nCount = 0;}
	void Add(T &Value,bool bAddEnd=true);
	void Remove(T &Value, POSITION &pos);
	void Remove(T &Value) {POSITION pos = NULL;Remove(Value,pos);}
	inline POSITION GetStartPosition() const;
	inline T &GetNext(POSITION &pos) const;
	bool IsEmpty() const {return m_nCount==0;}
	int GetCount() const {return m_nCount;}
	bool DoesContain(T Value);
	T &GetFirst() const {assert(m_pFirst);return m_pFirst->GetData();}
	virtual ~CList() {Destroy();}
};

template<class T>
bool CList<T>::DoesContain(T Value)
{
	sNode *pCurrent = m_pFirst;
	while(pCurrent)
	{
		if(*pCurrent==Value)
		{
			return true;
		}
		pCurrent = pCurrent->GetNext();
	}
	return false;
}

template<class T>
void CList<T>::Destroy() 
{
	sNode *pNode = m_pFirst;
	sNode *pDel;
	while(pNode)
	{
		pDel = pNode;
		pNode = pNode->GetNext();
		delete pDel;
		m_nCount--;
	}
#ifdef _DEBUG
	if(m_nCount!=0)
	{
		ErrorLog << m_nCount << " counts left in LL" << endl;
	}
#endif
}

template<class T>
inline T &CList<T>::GetNext(POSITION &pos) const
{
	sNode *tmp = ((sNode *)pos);

	pos = ((POSITION) tmp->GetNext());
	return tmp->GetData();
}

template<class T>
inline POSITION CList<T>::GetStartPosition() const
{
	return m_nCount==0 ? NULL : ((POSITION) m_pFirst);
}

template<class T>
void CList<T>::Add(T &Value,bool bAddEnd)
{
	if(m_pFirst==NULL)
	{
		assert(m_nCount==0);
		m_pFirst = m_pLast = new sNode(Value);
	}
	else
	{
		if(bAddEnd)
		{
			m_pLast->GetNext() = new sNode(Value);
			m_pLast = m_pLast->GetNext();
		}
		else
		{
			sNode *pTmp = m_pFirst;
			m_pFirst = new sNode(Value);
			m_pFirst->GetNext() = pTmp;
		}
	}
	m_nCount++;
}

template<class T>
void CList<T>::Remove(T &Value, POSITION &pos)
{
	sNode *pCurrent, *pPrev;
	pCurrent = pPrev = m_pFirst;
	while(pCurrent)
	{
		if(*pCurrent==Value)
		{
			if(pCurrent==m_pFirst)
			{
				if(m_pFirst==m_pLast)
				{
					m_pFirst=m_pLast = NULL;
				}
				else
				{
					m_pFirst = m_pFirst->GetNext();
				}
			}
			else
			{
				if(pCurrent!=m_pLast)
				{
					pPrev->GetNext() = pCurrent->GetNext();
				}
				else
				{
					m_pLast = pPrev;
					m_pLast->GetNext() = NULL;
				}
			}
			if(pCurrent==(sNode *)pos)
			{
				pos = (POSITION) pCurrent->GetNext();
			}
			m_nCount--;
			delete pCurrent;
			pCurrent = NULL;
		}
		else
		{
			pPrev = pCurrent;
			pCurrent = pCurrent->GetNext();
		}
	}
}
#endif

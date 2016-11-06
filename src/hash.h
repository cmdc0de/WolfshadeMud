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
/*****************************************
 *
 *	Map class with no memory management
 *	use this for static type hash tables used for
 *	reference not hash tables you'll be adding a deleting
 *	from often.
 *
 *******************************************/

#ifndef _MAP_H_
#define _MAP_H_

#include <string>
#include <assert.h>
#include <iostream>

UINT HashKey(const char *key, int nLetters);
UINT HashKey(CString &, int nLetters);
UINT HashKey(long key, int nLetters);

template<class KEY, class VALUE>
class CMap {
    //friend class CMemoryAllocator;
protected:

    struct sNode {
    private:
        //static CMemoryAllocator<CNode> m_Memory;
        int m_nLettersToHash;
    public:
        KEY m_key;
        VALUE m_value;
        sNode *m_pNext;

        sNode(KEY key, int nLettersToHash) {
            m_key = key;
            m_nLettersToHash = nLettersToHash;
            m_pNext = NULL;
        }

        bool operator==(long nVal) {
            return m_key == nVal;
        }
        //overloaded == for CString so if we aren't hashing by the entire word
        //then we don't compare by complete word

        int operator==(CString &key) {
            if (m_nLettersToHash == -1)
                return m_key.Compare(key);
            else if (key.GetLength() < m_nLettersToHash)
                return false;
            else if (m_key.GetLength() == key.GetLength())
                return m_key.Compare(key);
            else
                return m_key.Left(key.GetLength()).Compare(key);
        }
        //void * operator new (size_t t) {return m_Memory.Allocate();}
        //void operator delete (void *ptr) {m_Memory.Deallocate((CNode*)ptr);}
    };
    sNode **m_pHashTable;
    int m_nCount,
    m_nLettersToHash;
    UINT m_nHashTableSize;
    sNode *GetAssocAt(KEY key, UINT &nHash) const;

public:
    CMap(UINT nHashTableSize = 127, int nLettersToHash = -1);
    POSITION GetStartingPosition() const;
    VALUE &GetNext(POSITION &pos) const;
    VALUE &GetNext(KEY &key, POSITION &pos) const;
    int GetCount() const;
    bool IsEmpty() const;
    bool Lookup(KEY key, VALUE& Value) const;
    bool RemoveKey(KEY key);
    void Add(KEY key, VALUE value);
    void RemoveAll();
    UINT GetHashTableSize() const;
    ~CMap();
};

/////////////////////////////////////////////////////////////////////////////
// CMap<KEY, VALUE> inline functions

template<class KEY, class VALUE>
inline int CMap<KEY, VALUE>::GetCount() const {
    return m_nCount;
}

template<class KEY, class VALUE>
inline bool CMap<KEY, VALUE>::IsEmpty() const {
    return m_nCount == 0;
}

template<class KEY, class VALUE>
inline UINT CMap<KEY, VALUE>::GetHashTableSize() const {
    return m_nHashTableSize;
}

/////////////////////////////////////////////////////////////////////////////
// CMap<KEY, VALUE> out-of-line functions

/////////////////////////////////////
//	Cmap constructor
//	nHashbyLetters is for maps that are 
//	Hashed with char *
//	So we can hash by all letters or any number
//	if -1 we hash be all.
/////////////////////////////////////

template<class KEY, class VALUE>
CMap<KEY, VALUE>::CMap(UINT nHashTableSize, int nLettersToHash) {
    m_nHashTableSize = nHashTableSize;
    m_nLettersToHash = nLettersToHash;
    m_pHashTable = NULL;
    m_pHashTable = new sNode *[m_nHashTableSize];
    memset(m_pHashTable, 0, sizeof (sNode *) * m_nHashTableSize);
    m_nCount = 0;
}

////////////////////////////////////
//	Add a value to the hash table
//	for string we don't check the number of letters tobe hashed
//	because other wise if we add "rest" and "restore" we will
//	never see the rest.
//////////////////////////////////////

template<class KEY, class VALUE>
void CMap<KEY, VALUE>::Add(KEY key, VALUE value) {
    //if KEY is string then hash it with nLettersToHash 
    //but then just add it to hash table
    UINT nHash = HashKey(key, m_nLettersToHash) % m_nHashTableSize;
    sNode *pNode = m_pHashTable[nHash];
    m_nCount++;
    m_pHashTable[nHash] = new sNode(key, m_nLettersToHash);
    m_pHashTable[nHash]->m_value = value;
    m_pHashTable[nHash]->m_pNext = pNode;
}

/////////////////////////////////////
//	Lookup
//	Hash the key and look for it in the link list off
//	the table index.
//	written by: Demetrius Comes
////////////////////////////////////

template<class KEY, class VALUE>
bool CMap<KEY, VALUE>::Lookup(KEY key, VALUE &Value) const {
    UINT nHash = HashKey(key, m_nLettersToHash) % m_nHashTableSize;
    for (sNode *pNode = m_pHashTable[nHash]; pNode != NULL; pNode = pNode->m_pNext) {
        if (*pNode == key) {
            Value = pNode->m_value;
            return true;
        }
    }
    return false;
}

//////////////////////////////////
//	RemoveAll
//	Walk the hash table and each link list
//	at each index and delete only the nodes
//	written by: Demetrius Comes
////////////////////////////////////

template<class KEY, class VALUE>
void CMap<KEY, VALUE>::RemoveAll() {
    for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++) {
        sNode* pNode, *pPreNode;
        for (pNode = m_pHashTable[nHash]; pNode != NULL;) {
            pPreNode = pNode;
            pNode = pNode->m_pNext;
            delete pPreNode;
            m_nCount--;
        }
    }

    delete [] m_pHashTable;
    m_pHashTable = NULL;
    m_nCount = 0;
}

///////////////////////////////////
//	GetStartingPosition
//	Return the first node in hash table
//	casted as an unsigned int *&
//	written by: Demetrius Comes
//////////////////////////////////

template<class KEY, class VALUE>
POSITION CMap<KEY, VALUE>::GetStartingPosition() const {
    if (m_pHashTable == NULL)
        return NULL;

    for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++) {
        if (m_pHashTable[nHash]) {
            return ((POSITION) m_pHashTable[nHash]);
        }
    }
    return NULL;
}

/////////////////////////////////////
//	GetNext
//	Allows you to walk hash table like
//	a link list
//	written by: Demetrius Comes
////////////////////////////////////

template<class KEY, class VALUE>
VALUE &CMap<KEY, VALUE>::GetNext(POSITION &pos) const {
    sNode *pNode = NULL;
    //if we still have things in link list
    //return next one
    UINT nHash;
    pNode = ((sNode *) pos);
    if (!pNode->m_pNext) {
        nHash = HashKey(pNode->m_key, m_nLettersToHash) % m_nHashTableSize;
        nHash++;
        for (; nHash < m_nHashTableSize; nHash++) {
            if (m_pHashTable[nHash]) {
                pos = (POSITION) m_pHashTable[nHash];
                return pNode->m_value;
            }
        }
        pos = 0;
    } else {
        pos = ((POSITION) pNode->m_pNext);
    }
    return pNode->m_value;
}

/////////////////////////////////////
//	GetNext
//	Allows you to walk hash table like
//	a link list
//	written by: Demetrius Comes
////////////////////////////////////

template<class KEY, class VALUE>
VALUE &CMap<KEY, VALUE>::GetNext(KEY &key, POSITION &pos) const {
    sNode *pNode = NULL;
    //if we still have things in link list
    //return next one
    UINT nHash;
    pNode = ((sNode *) pos);
    if (!pNode->m_pNext) {
        nHash = HashKey(pNode->m_key, m_nLettersToHash) % m_nHashTableSize;
        nHash++;
        for (; nHash < m_nHashTableSize; nHash++) {
            if (m_pHashTable[nHash]) {
                pos = (POSITION) m_pHashTable[nHash];
                key = pNode->m_key;
                return pNode->m_value;
            }
        }
        pos = 0;
    } else {
        pos = ((POSITION) pNode->m_pNext);
    }
    key = pNode->m_key;
    return pNode->m_value;
}

//////////////////////////////////
//	Destructor
//  Call remove all to delete node memory
//	written by: Demetrius Comes
////////////////////////////////////

template<class KEY, class VALUE>
CMap<KEY, VALUE>::~CMap() {
    RemoveAll();
}

/////////////////////////////////////
//	GetAssocAt
//	Return node for the key we are looking for
//	written by: Demetrius Comes
////////////////////////////////////

template<class KEY, class VALUE>
typename CMap<KEY, VALUE>::sNode* CMap<KEY, VALUE>::GetAssocAt(KEY key, UINT &nHash) const {
    nHash = HashKey(key, m_nLettersToHash) % m_nHashTableSize;

    if (m_pHashTable == NULL)
        return NULL;

    sNode* pNode;
    for (pNode = m_pHashTable[nHash]; pNode != NULL; pNode = pNode->m_pNext) {
        if (*pNode == key)
            return pNode;
    }
    return NULL;
}
///////////////////////////////////////
//	RemoveKey
//	Removes a single node for a given key
//	written by: Demetrius Comes
////////////////////////////////////

template<class KEY, class VALUE>
bool CMap<KEY, VALUE>::RemoveKey(KEY key)
// remove key - return TRUE if removed
{
    if (m_pHashTable == NULL)
        return false; // nothing in the table

    sNode* pPrevNode;
    //no nodes at element
    UINT nHash = HashKey(key, m_nLettersToHash) % m_nHashTableSize;

    for (sNode* pNode = m_pHashTable[nHash]; pNode != NULL; pNode = pNode->m_pNext) {
        if (*pNode == key) {
            // remove it
            if (pNode == m_pHashTable[nHash]) {
                m_pHashTable[nHash] = pNode->m_pNext;
            } else {
                // remove from list
                //and repair link list
                pPrevNode->m_pNext = pNode->m_pNext;
            }
            delete pNode;
            m_nCount--;
            return true;
        }
        pPrevNode = pNode;
    }
    return false; // not found
}
#endif
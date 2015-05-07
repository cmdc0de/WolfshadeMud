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
* CMemoryAllocator
*	own memory management
*  Written by: Demetrius Comes
*  1/18/98
*/

#ifndef _MEMORYALLOCATOR_H_
#define _MEMORYALLOCATOR_H_

#include <memory.h>
#include <iostream>

#ifndef _WOLFSHADE_WINDOWS_
#define max(a,b) (a>b ? a : b)
#endif

template<class TYPE>
class CMemoryAllocator
{
protected:
	//link list struct
	struct Free
	{
		Free *Next;
	};
protected:
	unsigned int m_nAllocatedUnitSize, //size of unit to allocate
		m_nUnitSizeRequested, //size requested
		m_nBlockSize, //should be some multible of m_nAllocatedUnitSize
		m_nAllocatedBlocks, //size of memory new'ed
		m_nFreeUnits,  //to be implemented
		m_nStartingNumOfBlocks;  //number of blocks to start

	TYPE **m_pArrayOfPtsToBlocks;  //array of points
	Free *m_pStartOfFreeList;	//link list of free units
	void AllocateMore();  //allocates more memory

public:
	CMemoryAllocator(unsigned int StartingBlocks, unsigned int UnitsPerBlock, unsigned int UnitSize);
	~CMemoryAllocator();
	TYPE *Allocate(); //takes memory from free list
	void Deallocate(TYPE *Memory);  //adds memory to free list
};

////////////////////////////////////////
//Constructor
//	Here we new a starting amount of memory and break it up into
//	units.  We then add those united to the free link list.
//////////////////////////////////////////
template<class TYPE>
CMemoryAllocator<TYPE>::CMemoryAllocator(unsigned int StartingBlocks, unsigned int UnitsPerBlock, unsigned int UnitSize)
{
	m_nFreeUnits = 0;
	m_pStartOfFreeList = NULL;

	m_nStartingNumOfBlocks = StartingBlocks;
	m_nUnitSizeRequested = UnitSize;
	m_nAllocatedUnitSize = max(m_nUnitSizeRequested,sizeof(Free));
//#ifdef _DEBUG for memory overwritting later
//	m_nAllocatedUnitSize += (2*sizeof(int));
//#endif
	//block size is number of units and # per block
	m_nBlockSize = m_nAllocatedUnitSize * UnitsPerBlock;
	
	//Make the array of pointers
	m_pArrayOfPtsToBlocks = (TYPE**)::operator new (sizeof(TYPE *) * m_nStartingNumOfBlocks);
	//Set the number of allocated blocks... gonna keep the number of 
	//starting blocks...later I would like to put a cap on the number of free units such taht
	//free units can never be large the the num of total blocks started with
	m_nAllocatedBlocks = m_nStartingNumOfBlocks;
	Free *NewBlock, *Tail;
	
	for(register unsigned int i = 0;i<m_nAllocatedBlocks;i++)
	{
		m_pArrayOfPtsToBlocks[i] = (TYPE *) ::operator new (sizeof(TYPE *) * m_nBlockSize);
		memset(m_pArrayOfPtsToBlocks[i],'\0',m_nBlockSize);
		NewBlock = ((Free *) ((char *) m_pArrayOfPtsToBlocks[i]));
		m_nFreeUnits++;
		if(!i)
			m_pStartOfFreeList = NewBlock;
		else  //add to end of link list;
			Tail->Next = NewBlock;
		for(register unsigned int j = m_nAllocatedUnitSize;j<m_nBlockSize;j+=m_nAllocatedUnitSize,NewBlock = NewBlock->Next)
		{
			NewBlock->Next = ((Free *) ((char *) NewBlock+m_nAllocatedUnitSize));
			m_nFreeUnits++;
		}
		Tail = NewBlock; 
		//NewBlock->Next should be NULL b/c of memset!
	}
}

//deletes all free and used memory!
template<class TYPE>
CMemoryAllocator<TYPE>::~CMemoryAllocator()
{
	for(register unsigned int i = 0;i<m_nAllocatedBlocks;i++)
	{
		//do not just use delete here if you have the delete
		//operator overloaded you'll call the TYPE::delete not
		//the global delete function like we want!!!!!!!!
		::delete ((void *)m_pArrayOfPtsToBlocks[i]);
	}
	delete m_pArrayOfPtsToBlocks;
}

template<class TYPE>
TYPE *CMemoryAllocator<TYPE>::Allocate()
{
	//check free list
	if(!m_pStartOfFreeList)
		AllocateMore(); //if empty allocate more memory

	//get a pointer to free memory
	Free *Memory = m_pStartOfFreeList;

	//more the starting point of the free list down
	m_pStartOfFreeList = m_pStartOfFreeList->Next;

	m_nFreeUnits--;
	if(m_nFreeUnits<0)
		std::cout << "Free units negative!" << std::endl;

	memset(Memory,'\0',m_nAllocatedUnitSize);
	return (TYPE *)Memory;
}
//deallocate memory by adding the memory to free list
template<class TYPE>
void CMemoryAllocator<TYPE>::Deallocate(TYPE *Memory)
{
	if(!Memory)
		return; //if point is NULL just return
	Free *Temp = m_pStartOfFreeList; //store the first pointer
	memset(Memory,'\0',m_nAllocatedUnitSize);
	m_nFreeUnits++;
	m_pStartOfFreeList = ((Free *) (char *)Memory); //reset free list
	m_pStartOfFreeList->Next = Temp; //add the rest of the list
}

//Alocates one more block of memory
template<class TYPE>
void CMemoryAllocator<TYPE>::AllocateMore()
{
	unsigned int NewAllocateElement = m_nAllocatedBlocks;
	//new a new array of pointers
	TYPE **temp = (TYPE **) ::operator new(sizeof(TYPE *) * (++m_nAllocatedBlocks));
	//memcopy olds pointers to new array.
	memcpy(temp,m_pArrayOfPtsToBlocks,(sizeof(TYPE *) * NewAllocateElement));
	//delete old array of pointers
	delete m_pArrayOfPtsToBlocks;
	
	m_pArrayOfPtsToBlocks = temp;
	
	//add pointer to new element
	m_pArrayOfPtsToBlocks[NewAllocateElement] = (TYPE *) ::operator new(sizeof(TYPE *) * m_nBlockSize);
	memset(m_pArrayOfPtsToBlocks[NewAllocateElement],'\0',m_nBlockSize);
	
	Free *NewBlock = ((Free *) ((char *) m_pArrayOfPtsToBlocks[NewAllocateElement]));
	m_nFreeUnits++;

	if(m_pStartOfFreeList)
		std::cout << "allocating memory but still memory in free list!" << std::endl;

	m_pStartOfFreeList = NewBlock;
	
	//j is only used to keep count so we don't over write memory 
	for(register unsigned int j = m_nAllocatedUnitSize;j<m_nBlockSize;j+=m_nAllocatedUnitSize,NewBlock = NewBlock->Next)
	{
		NewBlock->Next = ((Free *) ((char *) NewBlock+m_nAllocatedUnitSize));
		m_nFreeUnits++;
	}
}
#endif

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
*  String ultility class!
*	Written by: Demetrius Comes
*	9/29/97
*	
*/

#ifndef _WOLFSHADESTRING_H_
#define _WOLFSHADESTRING_H_

#include <iostream>
#include <string>
#include <ctype.h>
#include <assert.h>
#include <sstream>
#include <stdlib.h>
#include "constants.h"
#include "memory.allocator.h"

class CString
{
	friend std::ostream &operator<<(std::ostream &,const CString &);
	friend std::istream &operator>>(std::istream &, CString &);

private:
	char *sPtr;
	int m_Length;

protected:
	static CMemoryAllocator<char> &GetEight();
	static CMemoryAllocator<char> &GetThirtyTwo();
	static CMemoryAllocator<char> &GetOneTwentyEight();
	static CMemoryAllocator<char> &GetFiveTwelve();
	static CMemoryAllocator<char> &GetTenTwentyFour();
	static char m_strEmpty[1];
	inline void *operator new(size_t size);
	inline void operator delete (void *ptr,size_t size);
	inline bool IsNumber(char c) {return (c>='0' && c<='9');}

public:
	CString FindBetween(char cOne, char cTwo);
	inline short Count(char nToCount);
	inline bool Compare(const char *right);
	inline short Find(char LookFor,int nHowMany);
	inline long MakeLong();
	inline int MakeInt();
	inline short Find(char First,char Second=-1);
	inline short Find(CString SubString) const;
	inline CString(const char * = "");
	explicit inline CString(int num);
	inline CString(const CString &);
	inline const CString &operator=(const CString &);
	inline const CString &operator=(const char *);
	inline CString &operator+=(const CString &);
	inline CString &operator+=(const char *);
	inline CString &operator+=(const char);
	int operator!() const {return m_Length==0;}
	int GetLength() const {return m_Length;}
	bool IsEmpty() const {return (m_Length==0);}
	inline void Empty();
	inline char &operator[](int pos);
	inline void SkipSpaces();
	void Colorize(bool bReally = true);
	CString GetWordAfter(int num, bool bAllAfter = false);
	void RemoveFunkyChars();
	CString Right(int position) const;
	CString Left(int Position) const;
	void Format(const char *strFormat, ...);
	void ToLower();
	inline operator char  *();
	inline operator const char *() const;
	~CString() {operator delete (sPtr, m_Length+1);}
	const CString &operator=(const int);
	const CString &operator=(const unsigned int i) {return (operator=((int) i));}
	const CString &operator=(const long );
	const CString &operator=(const float );
	CString &operator+=(const int i);
	CString &operator+=(const unsigned int i){return (operator+=((int) i));}
	CString &operator+=(const long );
	CString &operator+=(const float );
	bool operator==(const CString &right){return strcmp(sPtr,right.sPtr)==0;}
	bool operator==(const char *right)const {return strcmp(sPtr,right)==0;}
	bool operator!=(const CString &right) const {return strcmp(sPtr,right.sPtr)!=0;}
	bool operator!=(const char *right) const {return strcmp(sPtr,right)!=0;}
	CString operator()(int,int) const;
};

inline short CString::Count(char nToCount)
{
	short nNum = 0, count = 0;
	while(*(sPtr+count)!='\0')
	{
		if(*(sPtr+count)==nToCount)
		{
			nNum++;
		}
		count++;
	}
	return nNum;
}

inline void CString::SkipSpaces()
{
	char *tmp = sPtr;
	if(*tmp!=' ')
		return;
	while(*tmp==' ') tmp++;
	*this = tmp;
}

inline void* CString::operator new(size_t size)
{
	if(size==1)
		return m_strEmpty;
	else if(size<=8)
		return GetEight().Allocate();
	else if(size<=32)
		return GetThirtyTwo().Allocate();
	else if(size<=128)
		return GetOneTwentyEight().Allocate();
	else if(size<=512)
		return GetFiveTwelve().Allocate();
	else if(size<=1024)
		return GetTenTwentyFour().Allocate();
	else
	{	
		char *p = new char[size];
		memset(p,'\0',(sizeof(char)*size));
		return p;
	}
}

inline void CString::operator delete(void *ptr, size_t size)
{
	if(size==1)
		return;
	else if(size<=8)
		GetEight().Deallocate((char *)ptr);
	else if(size<=32)
		GetThirtyTwo().Deallocate((char *)ptr);
	else if(size<=128)
		GetOneTwentyEight().Deallocate((char *)ptr);
	else if(size<=512)
		GetFiveTwelve().Deallocate((char *)ptr);
	else if(size<=1024)
		GetTenTwentyFour().Deallocate((char *)ptr);
	else
		delete ptr;
}

inline char &CString::operator[](int pos) 
{
	assert(pos>=0&&pos<=m_Length);
	return sPtr[pos];
}

inline void CString::Empty()
{
	operator delete (sPtr, m_Length+1);
	sPtr = NULL;
	m_Length = 0;
	sPtr = (char *)operator new(m_Length+1);
}

inline CString::CString(int num) 
{
	m_Length = num;
	sPtr = (char *)operator new (m_Length+1);
}

inline CString::CString(const char *s)
{
	m_Length = strlen(s);
	sPtr = (char *)operator new (m_Length+1);
	strcpy(sPtr,s);
}

inline CString::CString(const CString &copy)
{
	m_Length=copy.m_Length;
	sPtr = (char *)operator new(m_Length+1);
	strcpy(sPtr,copy.sPtr);
}

inline int CString::MakeInt()
{
	if(sPtr[0] >= '0' && sPtr[0] <= '9')
	{
		int tmp = atoi(sPtr);
		return tmp;
	}
	return -1;
	
}

inline long CString::MakeLong()
{
	if(sPtr[0] >= '0' && sPtr[0] <= '9')
	{
		int tmp = atol(sPtr);
		return tmp;
	}
	return -1;
}

inline const CString &CString::operator=(const CString &right)
{
	if(&right!=this)
	{
		operator delete (sPtr, m_Length+1);
		//m_Length = right.m_Length;
		m_Length = strlen(right.sPtr);
		sPtr = (char *)operator new(m_Length+1);
		strcpy(sPtr,right.sPtr);
	}
	return *this;
}

inline const CString &CString::operator=(const char *right)
{
	//incase we have a case like skip spaces
	char *pTmp = sPtr;
	int nTmp = m_Length;
	m_Length = strlen(right);
	sPtr = (char *)operator new(m_Length+1);
	strcpy(sPtr,right);

	operator delete (pTmp, nTmp+1);
	
	return *this;
}

inline CString &CString::operator+=(const CString &right)
{
	char *tempPtr = sPtr;
	int tmpLength = m_Length;
	//m_Length +=right.m_Length;
	m_Length += strlen(right.sPtr);
	sPtr = (char *)operator new(m_Length+1);
	strcpy(sPtr,tempPtr);
	strcat(sPtr,right.sPtr);
	operator delete (tempPtr, tmpLength+1);
	
	return *this;
}

inline CString &CString::operator+=(const char *right)
{
	char *tempPtr = sPtr;
	int tmpLength = m_Length;
	m_Length +=strlen(right);
	sPtr = (char *)operator new(m_Length+1);
	strcpy(sPtr,tempPtr);
	strcat(sPtr,right);
	operator delete (tempPtr, tmpLength+1);

	return *this;
}

inline CString &CString::operator+=(const char right)
{
	char *tempPtr = sPtr;
	m_Length++;
	sPtr = (char *)operator new(m_Length+1);
	strcpy(sPtr,tempPtr);
	strcat(sPtr,&right);
	operator delete (tempPtr, m_Length);

	return *this;
}

inline CString CString::operator()(int index, int sublength) const
{
	assert(index >=0 && index < m_Length && sublength >=0);

	CString sub;
	
	sub.m_Length = (sublength - index)+1;
	
	sub.sPtr = (char *)operator new(sub.m_Length+1);
	strncpy(sub.sPtr,(sPtr+index),sub.m_Length);

	return sub;
}

//Function returns index of character
inline short CString::Find(char First, char Second)
{
	short c1 = 0;
	while(*(sPtr+c1)!='\0')
	{
		if(*(sPtr+c1)==First || (Second!=-1 && *(sPtr+c1)==Second))
			return c1;
		c1++;
	}
	return -1;
}

inline short CString::Find(char LookFor,int nHowMany)
{
	if(!nHowMany)
	{
		std::cerr << "error being overlooked in CString Find\r\n" << std::endl;
		return -1;
	}

	short c1 = 0, retValue = 0;
	while(*(sPtr+c1)!='\0')
	{
		if(*(sPtr+c1)==LookFor && ++retValue == nHowMany)
			return c1;
		else
			c1++;
	}
	return -1;
}

inline CString::operator char  *()
{
	if(!m_Length)
	{
		return "";
	}
	return sPtr;
}

inline CString::operator const char  *() const
{
	if(!m_Length)
	{
		return "";
	}
	return sPtr;
}

///////////////////////////////////////////////
//  Try to find a word or a partial of the word.
//	use the Substring to find the at least the
//	beginings of a word.  
//	written by: Demetrius Comes

inline short CString::Find(CString SubString) const
{
	short c1 = 0, c2 = 0;

	SubString.SkipSpaces();
	if(SubString.IsEmpty())
		return -1;
	
	while(*(sPtr+c1)!='\0')
	{
		if(tolower(*(sPtr+c1))==tolower(*(SubString.sPtr+c2)))
		{
			c2++;
		}
		else
		{
			//ok first letter didn't match
			//so lets search for begining of next word
			//because at least the begining of the word
			//must match
			c2=0;
			do
			{
				c1++;
				if(*(sPtr+c1)=='\0')
				{
					return -1;
				}
			}while((*(sPtr+c1)!=' '));
		}
		if(c2 == SubString.GetLength() && 
			(*(sPtr+(c1+1))==' ' || *(sPtr+(c1+1))=='\0'))
		{
			return c1;
		}
		else if (c2 == SubString.GetLength())
		{
			c2=0;
		}
		c1++;
	}
	return -1;
}

//case insensative compare
inline bool CString::Compare(const char *right) 
{
	int sCount = 0, rCount = 0;
	while(*(sPtr+sCount)!='\0')
	{
		if(tolower(*(sPtr+sCount))!=tolower(*(right+rCount)))
		{
			return false;
		}
		sCount++;
		rCount++;
	}
	return strlen(sPtr)==strlen(right) ? true : false;
}
#endif

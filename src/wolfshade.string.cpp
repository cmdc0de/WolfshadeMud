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
* CString Implementation
*	Written by: Demetrius Comes
*	9/29/97
*/

#include "stdinclude.h"
#pragma hdrstop
#include <iomanip.h>
#include <stdarg.h>
#include "color.h"
#include <stdio.h>
#define MAX_OUTPUT_BUF 8172

char CString::m_strEmpty[1] = {""};

CMemoryAllocator<char> &CString::GetEight()
{
	static CMemoryAllocator<char> *Eight = 
		new CMemoryAllocator<char>(10,10,8);
	return *Eight;
}
CMemoryAllocator<char> &CString::GetThirtyTwo()
{
	static CMemoryAllocator<char> *ThrityTwo = 
		new CMemoryAllocator<char>(10,10,32);
	return *ThrityTwo;
}
CMemoryAllocator<char> &CString::GetOneTwentyEight()
{
	static CMemoryAllocator<char> *OneTwentyEight = 
		new CMemoryAllocator<char>(5,10,128);
	return *OneTwentyEight;
}
CMemoryAllocator<char> &CString::GetFiveTwelve()
{
	static CMemoryAllocator<char> *FiveTwelve = 
		new CMemoryAllocator<char>(5,10,512);
	return *FiveTwelve;
}

CMemoryAllocator<char> &CString::GetTenTwentyFour()
{
	static CMemoryAllocator<char> *TenTwentyFour = 
		new CMemoryAllocator<char>(5,5,1024);
	return *TenTwentyFour;
}

const CString &CString::operator=(const int right)
{
	operator delete (sPtr, m_Length+1);
	char temp[32];

	ostrstream temp1(temp,32);
	temp1 << right << ends;  //ends must be here to place the '\0'
	m_Length = strlen(temp);
	sPtr = (char *)operator new(m_Length+1);
	strcpy(sPtr,temp);

	return *this;
}

CString &CString::operator+=(const int right)
{
	char *tempPtr = sPtr;
	char temp[32];
	int tmpLength = m_Length;
	
	ostrstream temp1(temp,32);
	temp1 << right << ends;
	m_Length +=strlen(temp);
	sPtr = (char *)operator new (m_Length+1);
		
	strcpy(sPtr,tempPtr);
	strcat(sPtr,temp);
	operator delete (tempPtr, tmpLength+1);

	return *this;
}

const CString &CString::operator=(const long right)
{
	operator delete (sPtr, m_Length+1);
	char temp[64];
	
	ostrstream temp1(temp,64);
	temp1 << right << ends;
	m_Length =strlen(temp);
	sPtr = (char *)operator new (m_Length+1);
		
	strcat(sPtr,temp);

	return *this;
}

const CString &CString::operator=(const float right)
{
	operator delete (sPtr, m_Length+1);
	char temp[64];
	ostrstream temp1(temp,64);

	temp1 << setprecision(2) << right << ends;
	m_Length +=strlen(temp);
	sPtr = (char *)operator new (m_Length+1);
	
	strcat(sPtr,temp);

	return *this;
}

CString &CString::operator+=(const long right)
{
	char *tempPtr = sPtr;
	int tmpLength = m_Length;
	char temp[128];
	ostrstream temp1(temp,128);

	temp1 << right << ends;
	m_Length +=strlen(temp);
	sPtr = (char *)operator new (m_Length+1);
	
	strcpy(sPtr,tempPtr);
	strcat(sPtr,temp);
	operator delete (tempPtr, tmpLength+1);

	return *this;
}

CString &CString::operator+=(const float right)
{
	char *tempPtr = sPtr;
	int tmpLength = m_Length;
	char temp[64];
	ostrstream temp1(temp,64);

	temp1 << setprecision(2) << right << ends;
	m_Length +=strlen(temp);
	sPtr = (char *)operator new (m_Length+1);
	
	strcpy(sPtr,tempPtr);
	strcat(sPtr,temp);
	operator delete (tempPtr, tmpLength+1);

	return *this;
}

void CString::ToLower()
{
	char *temp = sPtr;
	sPtr = (char *)operator new(m_Length+1);
	for(register int i=0;i<m_Length;i++)
		*(sPtr+i) = tolower(*(temp+i));
	operator delete (temp, m_Length+1);
}

istream &operator>>(istream &input, CString &s)
{
	char t[MAX_FILE_STRING_SIZE];
	input >> setw(MAX_FILE_STRING_SIZE) >> t;
	s = t;
	return input;
}

ostream &operator<<(ostream &output, const CString &s)
{
	output << s.sPtr;
	return output;
}

////////////////////////////////
//  Format
//	like sprintf...in fact we call it at the end
//	one advange is that it is buffer overflow safe (hehe I hope)
//	we calculate how big of a buffer we need before we
//	call sprintf
//	written by: Demetrius Comes
void CString::Format(const char * strFormat, ...)
{
	va_list argList;
	//init arg list
	va_start(argList, strFormat);

	//save the arg list
	va_list argListSave = argList;

	int nTotal = 0, nLength = 0, nPercision = 0, nWidth = 0;
	int nPos = 0;
	//while not null byte
	while(*(strFormat+nPos)!='\0')
	{
		//look for the %
		while(*(strFormat+nPos)!='\0' && *(strFormat+nPos)!='%')
		{
			nPos++;nTotal++;
		}
		//we find a %
		if(*(strFormat+nPos)!='\0')
		{
			nPos++; //advance to next thing
			nWidth = nPercision = nLength = 0;
			if(*(strFormat+nPos)=='-' || *(strFormat+nPos)=='+')
			{
				//throw away the + and -
				nPos++;nTotal++;
			}
			switch(*(strFormat+nPos))
			{
			case '%': //handle two % in a row
				nTotal+=2;
				nPos++;
				break;
			case '1':case '2':case '3':case '4':case '5':
			case '6':case '7':case '8':case '9':case '0':
				{
					int nNumPos = nPos;
					nPos++;
					while(*(strFormat+nPos)!='\0' && 
						*(strFormat+nPos)!='.' &&
						IsNumber(*(strFormat+nPos)))
					{
						nPos++;
					}
					//get width wanted
					nWidth = atoi((strFormat+nNumPos));
				}
				break;
			case '.':
				{
					int nNumPos = nPos;
					nPos++;
					while(*(strFormat+nPos)!='\0' &&
						IsNumber(*(strFormat+nPos)))
					{
						nPos++;
					}
					//get percision wanted
					nPercision = atoi(strFormat+nNumPos);
				}
				break;
			}
			//we'll be really safe!
			nWidth = nWidth + nPercision;
	
			switch(*(strFormat+nPos))
			{
			// single characters
			case 'c': case 'C':
				nLength = 2;
				va_arg(argList, char);
				break;
			// strings
			case 's': case 'S':
				{
					char * pstr = va_arg(argList, char *);
					if (pstr == NULL)
					   nLength = 6;  // "(null)"
					else
					{
					   nLength = strlen(pstr);
					   nLength = max(1, nLength);
					}
				}
				break;
			case 'd':case 'i':case 'u':case 'x':case 'X':case 'o':
				va_arg(argList, int);
				nLength = sizeof(int)*8; //should work
				break;
			case 'e':case 'g':case 'G':case 'f':
				va_arg(argList, double);
				nLength = sizeof(double)*8;
				break;
			case 'p':
				va_arg(argList, void*);
				nLength = sizeof(void *)*8;
				break;
			// no output
			case 'n':
				va_arg(argList, int*);
				break;
			}
			nTotal+=MAX(nLength,nWidth);
		}
	}
	va_end(argList);

	//Make our buffer
	CString strBuf(this->GetLength()+nTotal);
	//char strBuf[8094];
	//reset argList
	va_start(argListSave, strFormat);
	vsprintf(strBuf,strFormat, argListSave);
	va_end(argListSave);
	//= will truncate any extra space allocated
	(*this)=strBuf;
}

//do not include character at Position!
CString CString::Left(int Position) const
{
	if(Position ==-1)
		return *this;

	if(Position < 1)
		return "";

	if(Position > m_Length)
		return (*this)(0,m_Length);
	
	return (*this)(0,Position-1);
}
//do not include character at Position!
CString CString::Right(int Position) const
{
	if((Position+1) > (m_Length-1))
		return "";

	return (*this)(Position+1,m_Length-1);
}

void CString::RemoveFunkyChars()
{
	char *tmp = (char *)operator new(m_Length+1);
	register int c1 = 0, c2 = 0;
	while(*(sPtr+c1)!='\0')
	{
		if(*(sPtr+c1)!='\b' || *(sPtr+c1)!='\t')
		{
			*(tmp+c2) = *(sPtr+c1);
			c2++;
		}
		c1++;
	}
	*(tmp+c2) = '\0';
	*this = tmp;
}

//////////////////////////////////////
//Gets the word after the number passed in or gets
//everything after that word if bAllAfter is TRUE
CString CString::GetWordAfter(int num, bool bAllAfter)
{
	char *tmp = sPtr;
	int nStartWord = 0, nEndWord = 0, nWord = 0;
	while(nWord <= num)
	{
		nStartWord = nEndWord;
		while(*tmp==' ')
		{
			if(*tmp=='\0')
				return "";
			tmp++;
			nStartWord++;
		}

		//skipping spaces
		nEndWord = nStartWord;
		while(*tmp!=' ')
		{
			if(*tmp=='\0')
				break;
			tmp++;
			nEndWord++;
		}
		nWord++;
	}
	if(bAllAfter)
	{
		nEndWord = m_Length;
	}
	return nStartWord >= nEndWord ? CString() : operator()(nStartWord,nEndWord-1);
}

void CString::Colorize(bool bReally)
{
	register int j=0,p=0,k;
	int max;
	char *color;
	int nNumOfColors = Count('&');
	//magic number 12....no ansi color is bigger than 12 bytes
	//so we count the number of & times it by 12 and that should 
	//give us a big enough buffer to work with...yes it's chessy
	//but it'll work for now
	CString output(this->GetLength()+(nNumOfColors*12));
	while(sPtr[j]!='\0')
	{
		color = NULL;
		if(sPtr[j]=='&')
		{
			switch(sPtr[j+1])
			{
			case 'n':
				color = CNRM;break;
			case 'r':
				color = CRED;break;
			case 'g':
				color = CGRN;break;
			case 'y':
				color = CYEL;break;
			case 'b':
				color = CBLU;break;
			case 'm':
				color = CMAG;break;
			case 'c':
				color = CCYN;break;
			case 'w':
				color = CWHT;break;
			case 'l':
				color = CBLK;break;
			case 'R':
				color = BRED;break;
			case 'G':
				color = BGRN;break;
			case 'Y':
				color = BYEL;break;
			case 'B':
				color = BBLU;break;
			case 'M':
				color = BMAG;break;
			case 'C':
				color = BCYN;break;
			case 'W':
				color = BWHT;break;
			case 'L':
				color = BBLK;break;
			case 'e':
				color = BKRED;break;
			case 'k':
				color = BKGRN;break;
			case 'p':
				color = BKYEL;break;
			case 'u':
				color = BKBLU;break;
			case 'a':
				color = BKMAG;break;
			case 'f':
				color = BKCYN;break;
			case 'h':
				color = BKWHT;break;
			case 'i':
				color = BKBLK;break;
			case 'U':
				color = UNDER;break;
			case 'F':
				color = FLASH;break;
			default:
				color = NULL;break;
			}
		}
		if(color && bReally)
		{
			max=strlen(color);
			j+=2;
			for(k=0;k<max;k++)
			{ 
				output[p] = color[k]; 
				p++; 
			}
		}
		else if(color==NULL)
		{
			output[p] = sPtr[j];
			j++;p++;
		}
		else //don't colorize
		{
			j+=2;
		}
	}
	
	output[p] = '\0';
	*this = output;
}

///////////////////////////////////////////
//	Finds the string between two tokens
CString CString::FindBetween(char cOne, char cTwo)
{
	int nCounter = 0, nPosOne = -1, nPosTwo = -1;
	bool bFound = false;
	while(sPtr[nCounter]!='\0' && !bFound)
	{
		if(sPtr[nCounter]==nPosOne && nPosOne==-1)
		{
			nPosOne = nCounter;
		}
		else if(sPtr[nCounter]==nPosTwo && nPosOne!=-1 &&
			nPosTwo==-1)
		{
			nPosTwo = nCounter;
			bFound = true;
		}
		else
		{
			nCounter++;
		}
	}
	if(!bFound)
	{
		return CString();
	}
	else
	{
		return (*this)(nPosOne+1,nPosTwo-1);
	}
}

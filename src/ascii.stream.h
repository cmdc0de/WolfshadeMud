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
* CAscii Class
*
*	This class does alot of the ascii file parsing for us
*
*
*/

#ifndef _ASCII_STREAM_H
#define _ASCII_STREAM_H

#include <fstream>
#include "wolfshade.string.h"
#include "constants.h"

#define MAX_FILE_STRING_SIZE 2048

class CAscii : public std::ifstream
{
public:
	CAscii & getline(char *,int nLength,char delim = '\n');
	void NumericFromBuf(CString &strBuf,const char *strFormat,...);
	int Count(CString Prefix,CString delim);
	bool ReadTill(CString delim);
	int Count(CString delim);
	int AsciiConvert(char *);
	void ReadFileString(CString &strValue, char delim = '~');
	void Numeric(const char *pFormat, ...);
	CAscii(CString file,long bits) : std::ifstream(file,bits){;}
};
#endif
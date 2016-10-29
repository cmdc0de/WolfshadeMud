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
 * Cascii implementation
 *
 *
 *
 */

#include "ascii.stream.h"
#include "constants.h"
#include "error.h"
#include "log.h"
#include <stdlib.h>
#include <ctype.h>
#include <sstream>
#include <stdarg.h>

CAscii::CAscii(CString file, long bits) : std::ifstream() {
    open(file.cptr(), (std::ios_base::openmode)bits);
}

///////////////////////////////////////////
//  function name: Numeric
//	returns: void
//	description:	this function takes format string and 
//		a variable argument list.  searches the format string
//		for % then uses the next letter as a key to know what
//		to pull off the stack next.  This works much like sscanf in C
//		but focus to what we are trying to accomplish
///////////////////////////////////////////// 

void CAscii::Numeric(const char *pFormat, ...) {
    CString strFormat(pFormat);
    char Temp[MAX_FILE_STRING_SIZE] = {'\0'};
    char tempchar;
    char strTemp[MAX_FILE_STRING_SIZE] = {'\0'};
    va_list vaList;
    float tempfloat;
    long templong;
    int pos, tempint;
    short int tempshort;

    getline(Temp, MAX_FILE_STRING_SIZE);
    std::string strBuf(&Temp[0], MAX_FILE_STRING_SIZE);
    std::istringstream Values(strBuf);

    // init variable list
    va_start(vaList, pFormat);
    while ((pos = strFormat.Find('%')) != -1) {
        try {
            switch (strFormat[pos + 1]) {
                case '%':
                    ErrorLog << "Error can't have 2 % in a row in format string" << endl;
                    break;
                case 'd': case 'D':
                    Values >> tempint;
                    (*va_arg(vaList, int *)) = tempint;
                    break;
                case 'b': case 'B':
                    Values >> tempshort;
                    (*va_arg(vaList, short int *)) = tempshort;
                    break;
                case 'l': case 'L':
                    Values >> templong;
                    (*va_arg(vaList, long *)) = templong;
                    break;
                case 'c': case 'C':
                    Values >> tempchar;
                    (*va_arg(vaList, char *)) = tempchar;
                    break;
                case 'f': case 'F':
                    Values >> tempfloat;
                    (*va_arg(vaList, float *)) = tempfloat;
                    break;
                case 's': case 'S':
                    Values >> strTemp;
                    strcpy(va_arg(vaList, char *), strTemp);
                    break;
                default:
                    ErrorLog << "Numeric doesn't support %" << strFormat[pos + 1] << endl;
                    break;
            }
            // reset format string
            strFormat = strFormat.Right(pos + 1);
        } catch (...) {
            ErrorLog << "Error in numeric line : " << Temp << endl;
            return;
        }
    }
    va_end(vaList);
}

////////////////////////////////////////////////////
//	Name:  ReadFileString
//
//	Reads in a string from a file until the delim is hit

void CAscii::ReadFileString(CString &strValue, char delim) {
    CString Temp(MAX_FILE_STRING_SIZE);
    if (delim != EOF) {
        bool bEnd = false;
        while (!bEnd && good()) {
            //get everything until the \n
            getline(Temp.ptr(), MAX_FILE_STRING_SIZE);
            int nPos;
            //do we find the delim?
            if ((nPos = Temp.Find((char) delim)) != -1) {
                //yes
                strValue += Temp.Left(nPos);
                bEnd = true;
            } else {
                strValue.Format("%s%s\r\n",
                        strValue.cptr(),
                        Temp.cptr());
            }
        }
    } else {
        strValue.Empty();
        while (good()) {
            getline(Temp.ptr(), MAX_FILE_STRING_SIZE);
            strValue.Format("%s%s\r\n",
                    strValue.cptr(),
                    Temp.cptr());
        }
    }
}

//////////////////////////////////////
// Name: AsciiConvert
//	This takes a string of letters and converts
//	them to bit vectors
///////////////////////////////////////

int CAscii::AsciiConvert(char *flag) {
    long flags = 0;
    bool is_number = true;
    char *p = NULL;

    for (p = flag; *p; p++) {
        if (islower(*p))
            flags |= 1 << (*p - 'a');
        else if (isupper(*p))
            flags |= 1 << (25 + (*p - 'A'));

        if (!isdigit(*p))
            is_number = false;
    }

    if (is_number)
        flags = atol(flag);

    return flags;
}

////////////////////////////////////////////////
// Name:  Count
//	This function counts the number of 'delim' in
//	a file
///////////////////////////////////////////////

int CAscii::Count(CString delim, CString prefix) {
    CString temp(256), temp1(256), file1(256);
    int count = 0;
    CString Error;

    while (good()) {
        *this >> temp1;
        temp = prefix;
        temp += temp1;
        //std::ifstream CurrentFile(temp,std::ios::in|ios::nocreate);
        std::ifstream CurrentFile(temp.cptr(), std::ios::in);
        if (!CurrentFile) {
            Error.Format("File will not open: %s ", temp.cptr());
            CError Err(CRITICAL_ERROR, Error.cptr());
            throw &Err;
        }
        while (CurrentFile.good()) {
            CurrentFile.getline(file1.ptr(), MAX_FILE_STRING_SIZE);
            if (file1 == delim)
                count++;
        }
        if (!CurrentFile.eof())
            ErrorLog << "Stoped before end of file:" << temp << endl;
        CurrentFile.close();
    }
    clear();
    seekg(0);
    return count;
}

///////////////////////////////////
//	Name Readtill
//	This function helps with error recovery
//	we can read to the next delim if there was a problem
//	while reading the last one.  Example reading in a mob
//	an error happens, we use readtill to read to the next one
//	without so that we can skip the one with the error
/////////////////////////////////////

bool CAscii::ReadTill(CString delim) {
    CString Line(MAX_FILE_STRING_SIZE);

    while (good()) {
        getline(Line.ptr(), MAX_FILE_STRING_SIZE);
        if (Line == delim)
            return true;
    }

    if (!eof()) {
        ErrorLog << "Error in ReadTill waiting for delim->" << delim << endl;
        return false;
    }
    return true;
}

//counts the number of delim in the file

int CAscii::Count(CString delim) {
    CString temp(256);
    int count = 0;

    while (good()) {
        getline(temp.ptr(), 256);
        if (temp == delim)
            count++;
    }
    clear();
    seekg(0);
    return count;
}

////////////////////////////////
//	Numeric
//	constructs an istrstream and 
//	reads in all variables
///////////////////////////////

void CAscii::NumericFromBuf(CString &strBuf, const char *pFormat, ...) {
    CString strFormat(pFormat);
    char tempchar;
    char strTemp[MAX_FILE_STRING_SIZE] = {'\0'};
    va_list vaList;
    float tempfloat;
    long templong;
    int pos, tempint;
    short int tempshort;

    std::string strBuf1(strBuf.cptr(), strBuf.GetLength());
    std::istringstream Values(strBuf1);

    // init variable list
    va_start(vaList, pFormat);
    while ((pos = strFormat.Find('%')) != -1) {
        try {
            switch (strFormat[pos + 1]) {
                case '%':
                    ErrorLog << "Error can't have 2 % in a row in format string" << endl;
                    break;
                case 'd': case 'D':
                    Values >> tempint;
                    (*va_arg(vaList, int *)) = tempint;
                    break;
                case 'b': case 'B':
                    Values >> tempshort;
                    (*va_arg(vaList, short int *)) = tempshort;
                    break;
                case 'l': case 'L':
                    Values >> templong;
                    (*va_arg(vaList, long *)) = templong;
                    break;
                case 'c': case 'C':
                    Values >> tempchar;
                    (*va_arg(vaList, char *)) = tempchar;
                    break;
                case 'f': case 'F':
                    Values >> tempfloat;
                    (*va_arg(vaList, float *)) = tempfloat;
                    break;
                case 's': case 'S':
                    Values >> strTemp;
                    strcpy(va_arg(vaList, char *), strTemp);
                    break;
                default:
                    ErrorLog << "Numeric doesn't support %" << strFormat[pos + 1] << endl;
                    break;
            }
            // reset format string
            strFormat = strFormat.Right(pos + 1);
        } catch (...) {
            ErrorLog << "Error in numeric line : " << strBuf << endl;
            return;
        }
    }
    va_end(vaList);
}


//////////////////////////////////////////////////
//	We over ride the getline function so we can take care
//	of ascii differences in OS's.  Such as in the windows world
//	when you hit return you get a \n at the end of the line
//	in the linux world you get a \r\n
//	and that is a bad thing when you expect something like
//	ENDROOM and you get ENDROOM\r so we will always strip off
//	\r\n if the \r exists
//	written by: Demetrius Comes

CAscii & CAscii::getline(char *buf, int nLength, char delim) {
    std::ifstream::getline(buf, nLength, delim);
    int nLen = strlen(buf);
    if (nLen && buf[nLen - 1] == '\r') {
        buf[nLen - 1] = '\0';
    }
    return *this;
}

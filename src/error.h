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
*	CError class.  this is our exception handling
*	we throw by reference and catch by pointer
*	the copy constructor is overloaded so that it can't be
*	thrown by value.
*	
*	Use should be as follows:
*		throw &CError(errortype,error message)
*/

#ifndef _ERROR_H_
#define _ERROR_H_
#include <string.h>

const short NONERROR = 0; //message only
const short MESSAGE_ONLY = NONERROR;
const short NON_CRITICAL_ERROR = 1;
const short CRITICAL_ERROR = 2;
const short STOP_ERROR = 3;
const short MAX_ERROR_MESSAGE_SIZE = 512;

class CError
{
private:
	// Error message must be char * otherwise CString will go out of
	//scope and memory deleted
	char m_strErrorMessage[MAX_ERROR_MESSAGE_SIZE];
	short m_ErrorType;
public:
	CError(const CError &copy);
	bool IsType(short nError) {return m_ErrorType==nError;}
	void SetError(short ErrorType, char *ErrorWording);
	void SetError(char *ErrorWording);
	short GetError() {return m_ErrorType;}
	char *GetMessage() {return m_strErrorMessage;}
	inline CError(short ErrorType,char *strErrorMessage);
};

inline CError::CError(short nErrorType,char *strErrorMessage)
{
	strcpy(m_strErrorMessage,strErrorMessage);
	m_ErrorType = nErrorType;
}
#endif
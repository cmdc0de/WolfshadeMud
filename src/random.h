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
*  Random Class
*	Generates random numbers
*
**/
#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <stdlib.h>
#include <time.h>

class CRandom
{
private:
	int m_data;
public:
	CRandom() {m_data = 0;srand((unsigned)time(NULL));}
	inline int Roll(unsigned int dice);
	inline int Roll(unsigned int dice,unsigned int Times);
};

inline int CRandom::Roll(unsigned int dice)
{
	return (rand()%dice)+1;
}
inline int CRandom::Roll(unsigned int dice, unsigned int Times)
{
	m_data = 0;
	for(register unsigned short int i = 0;i<Times;i++)
	{
		m_data+=(rand()%dice)+1;
	}
	return m_data;
}

#endif

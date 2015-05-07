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
// bank.room.cpp: implementation of the CBank class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "bank.room.h"
#include "money.h"

short CBankRoom::m_nCToS=10;
short CBankRoom::m_nCToG=20;
short CBankRoom::m_nCToP=30;
short CBankRoom::m_nSToG=10;
short CBankRoom::m_nSToP=20;
short CBankRoom::m_nGToP=10;

CBankRoom::~CBankRoom()
{

}

void CBankRoom::DoList(CString &strList)
{
	static const char *strPrice = 
		"I can exchange:\r\n"
		"%s to %s at %d%%\r\n"
		"%s to %s at %d%%\r\n"
		"%s to %s at %d%%\r\n"
		"%s to %s at %d%%\r\n"
		"%s to %s at %d%%\r\n"
		"%s to %s at %d%%\r\n";

	strList.Format(strPrice,
		(char *)sMoney::m_strCopper,(char *)sMoney::m_strSilver,m_nCToS,
		(char *)sMoney::m_strCopper,(char *)sMoney::m_strGold,m_nCToG,
		(char *)sMoney::m_strCopper,(char *)sMoney::m_strPlat,m_nCToP,
		(char *)sMoney::m_strSilver,(char *)sMoney::m_strGold,m_nSToG,
		(char *)sMoney::m_strSilver,(char *)sMoney::m_strPlat,m_nSToP,
		(char *)sMoney::m_strGold,(char *)sMoney::m_strPlat,m_nGToP);
}
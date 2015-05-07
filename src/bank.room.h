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
//
//	Bank room class
/////////////////////////////////////////////////////////////

#ifndef _BANK_H_
#define _BANK_H_

class CBankRoom : public CRoom
{
	friend class CWorld;
private:
	static short m_nCToS,
		m_nCToG,
		m_nCToP,
		m_nSToG,
		m_nSToP,
		m_nGToP;
protected:
	CBankRoom(CRoom *pRoom) : CRoom(pRoom){;}
	CBankRoom() : CRoom() {;}
public:
	short GetCToS() {return m_nCToS;}
	short GetCToG() {return m_nCToG;}
	short GetCToP() {return m_nCToP;}
	short GetSToG() {return m_nSToG;}
	short GetSToP() {return m_nSToP;}
	short GetGToP() {return m_nGToP;}
public:
	virtual bool CanCampInRoom(){return false;}
	virtual void DoList(CString &str);
	virtual ~CBankRoom();
};
#endif
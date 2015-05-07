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
#include "stdinclude.h"
#pragma hdrstop
#include "list.h"

// only used for static stuff
///////////////////////////////////////////////////////////////
//	using new'ed static memory b/c every complier seems to handle
//	destroying static members at different times.  So we'll just 
//	take the memory from the heap instead of the stack and 
//	eliminate that problem

CMemoryAllocator<CList<long>::sNode> &CList<long>::sNode::GetAllocator()
{
	CMemoryAllocator<CList<long>::sNode> *p = new CMemoryAllocator<CList<long>::sNode>(1,10,sizeof(CList<long>::sNode));
	 return *p;
}

CMemoryAllocator<CList<CCharacter *>::sNode> &CList<CCharacter *>::sNode::GetAllocator()
{
	static CMemoryAllocator<CList<CCharacter *>::sNode> *p = 
		new CMemoryAllocator<CList<CCharacter *>::sNode> (1,10,sizeof(CList<CCharacter *>::sNode));
	return *p;
}

CMemoryAllocator<CList<CObject *>::sNode> &CList<CObject *>::sNode::GetAllocator()
{
	static CMemoryAllocator<CList<CObject *>::sNode> *p = 
		new CMemoryAllocator<CList<CObject *>::sNode>(1,10,sizeof(CList<CObject *>::sNode));
	return *p;
}

CMemoryAllocator<CList<CCorpse *>::sNode> &CList<CCorpse *>::sNode::GetAllocator()
{
	static CMemoryAllocator<CList<CCorpse *>::sNode> *p = 
		new CMemoryAllocator<CList<CCorpse *>::sNode>(1,10,sizeof(CList<CCorpse *>::sNode));
	return *p;
}

CMemoryAllocator<CList<CString>::sNode> &CList<CString>::sNode::GetAllocator()
{
	static CMemoryAllocator<CList<CString>::sNode> *p = 
		new CMemoryAllocator<CList<CString>::sNode>(1,10,sizeof(CList<CString>::sNode));
	return *p;
}

CMemoryAllocator<CList<const CSpell<CMage> *>::sNode> &CList<const CSpell<CMage> *>::sNode::GetAllocator()
{
	static CMemoryAllocator<CList<const CSpell<CMage> *>::sNode> *p = 
		new CMemoryAllocator<CList<const CSpell<CMage> *>::sNode>(1,10,sizeof(CList<const CSpell<CMage> *>::sNode));
	return *p;
}

CMemoryAllocator<CList<CRoom *>::sNode> &CList<CRoom *>::sNode::GetAllocator()
{
	static CMemoryAllocator<CList<CRoom *>::sNode> *p = 
		new CMemoryAllocator<CList<CRoom *>::sNode>(1,10,sizeof(CList<CRoom *>::sNode));
	return *p;
}

CMemoryAllocator<CList<int>::sNode> &CList<int>::sNode::GetAllocator()
{
	static CMemoryAllocator<CList<int>::sNode> *p = 
		new CMemoryAllocator<CList<int>::sNode>(1,5,sizeof(CList<int>::sNode));
	return *p;
}
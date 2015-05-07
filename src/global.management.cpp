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
*
*	Global management implemenation
*
*
*/

#include "stdinclude.h"
#pragma hdrstop
#include "global.management.h"

bool CGlobalVariableManagement::m_bCreated = false;

const char *CGlobalVariableManagement::Classes[TOTAL_CLASSES]= 
{
	"RESERVED",
	"Warrior",
	"Cleric",
	"Mage",
	"Rogue",
	"Druid",
	"Ranger",
	"Defiler",
	"Paladin",
	"Anti-Paladin",
	"Shaman",
	"Psionicist",
	"Empath",
	"Mind Flayer"
};

const char *CGlobalVariableManagement::ColorClasses[TOTAL_CLASSES]= 
{
	"RESERVED",
	"&wWarrior&n",
	"&CCleric&n",
	"&YMage&n",
	"&LRogue&n",
	"&gDruid&n",
	"&GRanger&n",
	"&rDefiler&n",
	"&WPaladin&n",
	"&RAnti-Paladin&n",
	"&cShaman&n",
	"&mPsionicist&n",
	"&bEmpath&n",
	"&MMind Flayer&n"
};

const char *CGlobalVariableManagement::Races[TOTAL_RACES]= 
{
	"RESERVED",
	"Troll",
	"Ogre",
	"Orc",
	"Gnoll",
	"Duergar Dwarf",
	"Drow Elf",
	"Half-Drow Elf",
	"Githyanki",
	"Draconian",
	"Imp",
	"Gnome",
	"Grey Elf",
	"Kender",
	"Mountain Dwarf",
	"Brownie",
	"Pixie",
	"Centaur",
	"Saurial",
	"Half-Grey Elf",
	"Human",
	"Thri-Kreen",
	"Minotaur",
	"Half-Orc",
	"Barbarian",
	"Kenku",
	"Mul",
	"Illithid",
	"None",
	"Undead",
	"Lich",
	"Elemental"
};

const char *CGlobalVariableManagement::ColorRaces[TOTAL_RACES]= 
{
	"RESERVED",
	"&gTroll&n",
	"&bOgre&n",
	"&LOrc&n",
	"&LGn&yoll&n",
	"&rDuergar&n",
	"&mDrow &mElf&n",
	"&wHalf-&mDrow &mElf&n",
	"&cGith&Lyanki&n",
	"&gDraco&rnian&n",
	"&cImp&n",
	"&RGnome&n",
	"&cGrey Elf&n",
	"&GKen&Wder&n",
	"&YDwarf&n",
	"&yBrownie&n",
	"&WPixie&n",
	"&yCen&gtaur&n",
	"&GSaurial&n",
	"&wHalf-&cGrey Elf&n",
	"&wHuman&n",
	"&GThri&W-&YKreen&n",
	"&LMino&rtaur&n",
	"&wHalf-&LOrc&n",
	"&LBarb&barian&n",
	"&wKe&Lnk&wu&n",
	"&BMul&n",
	"&MIllithid&n",
	"None",
	"&LUndead&n",
	"&LLi&rch&n",
	"&YElemental&n"
};

const char *CGlobalVariableManagement::Gods[TOTAL_GODS]= 
{
	"NONE",
	"Guk",
	"Arkan",
	"Nngh",
	"Duth",
	"Ilsensine"
};

const char *CGlobalVariableManagement::ColorGods[TOTAL_GODS]= 
{
	"NONE",
	"&wGuk the &LOrcish &wGod of &MMagic&n",
	"&wArkan the &LBarb&barian &wGod of &LBattle&n",
	"&wNngh the &LMino&rtaur &wGod of &rChaos&n",
	"&wDuth the Human God of &LBa&wlan&Wce&n",
	"&wIlsensine the &MIllithid &wGod of &MMind Flayers&w"
};

CGlobalVariableManagement::CGlobalVariableManagement()
{
	if(m_bCreated)
	{
		ErrorLog << "TRIED TO MAKE A SECOND GLOBAL CLASS!!!!\n" << endl;
		return;
	}
	m_bCreated = true;
	PlayerLL.SetDeleteData();
}

CGlobalVariableManagement::~CGlobalVariableManagement()
{
	
}

////////////////////////////////////////////////////////
//Find Character
//Finds a character by name in the global link list
//probably want to move this to a hash table look up when we implemented it
//written: 4/3/98
////////////////////////////////////////////////////////
CCharacter * CGlobalVariableManagement::FindCharacter(CString strName,CCharacter *pSelf)
{
	int nDot,nNum=1;
	if((nDot = strName.Find('.'))!=-1)
	{
		nNum = strName.Left(nDot).MakeInt();
		strName = strName.Right(nDot);
	}
	//pSelf can be NULL!
	//are we trying to find ourself?
	if(CCharacterAttributes::IsSelf(strName))
	{
		return pSelf;
	}

	CCharacter *ch;
	POSITION pos = PlayerLL.GetStartPosition();
	int nCount = 0;

	while(pos)
	{
		ch = PlayerLL.GetNext(pos);
		if(pSelf!=NULL)
		{
			if((ch->GetAlias().Find(strName) != -1))
			{
				if(++nCount==nNum)
				{
					//are they on the correct side of war?
					if(pSelf->CanSeeChar(ch)
						&& pSelf->CanSeeName(ch)
						&& pSelf->CanSeeInRoom(ch->GetRoom()))
					{
						return ch;
					}
					else
					{
						return NULL;
					}
				}
			}
		}
		else
		{
			if(ch->GetAlias().Find(strName)!= -1)
			{
				if(++nCount==nNum)
				{
					return ch;
				}
			}
		}
	}
	return NULL;
}

///////////////////////////////
//	Sends a string to all characters
///////////////////////////////
void CGlobalVariableManagement::SendToAll(CString strToAll, CCharacter * pCharDontSend)
{
	POSITION pos = PlayerLL.GetStartPosition();
	CCharacter *pCh;
	while(pos)
	{
		pCh = PlayerLL.GetNext(pos);
		if(pCh!=pCharDontSend)
		{
			pCh->SendToChar(strToAll);
		}
	}
}

////////////////////////////////
//	SendToOOC
void CGlobalVariableManagement::SendToOOC(CString strToOOC, CCharacter *pNoSendChar)
{
	POSITION pos = PlayerLL.GetStartPosition();
	CCharacter *pCh;
	while(pos)
	{
		pCh = PlayerLL.GetNext(pos);
		if(pCh!=pNoSendChar && pCh->Prefers(CCharacterAttributes::PREFERS_OOC))
		{
			pCh->SendToChar(strToOOC);
		}
	}
}

//////////////////////////////////////
//	SEND to gods...togs if for prefers
//	so we only send to gods that want petitions etc
void CGlobalVariableManagement::SendToGods(CString strToSend, int nTogs, CCharacter *pCharNotToSendTo)
{
	POSITION pos = m_GodLL.GetStartPosition();
	CCharacter *pCur;
	while(pos)
	{
		pCur = m_GodLL.GetNext(pos);
		if(pCur!=pCharNotToSendTo && pCur->Prefers(nTogs))
		{
			pCur->SendToChar(strToSend);
		}
	}
}

////////////////////////////////////
//	Send a message to all characters in outdoor rooms
void CGlobalVariableManagement::SendToOutdoors(CString strMsg)
{
	CCharacter *pCh;
	POSITION pos = PlayerLL.GetStartPosition();
	while(pos)
	{
		pCh = PlayerLL.GetNext(pos);
		if(pCh->GetRoom()->IsOutdoors())
		{
			pCh->SendToChar(strMsg);
		}
	}
}

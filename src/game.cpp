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
*  Implementation of CGame
*
*/
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"

CCharacter * CalcClassPtr(CCharIntermediate *ch);

extern CGlobalVariableManagement GVM;
bool CGame::m_bShutDown = false;
CList<CCharacter *> CGame::m_FightLL;

//////////////////////////////////////////////////////////
//	statics

//////////////////////////////////////////////////
//	Non STatics
CGame::CGame(CString strOpts)
{
	m_bShutDown = false;

	m_pZoneManager = NULL;
	m_pMobManager = NULL;
	m_pObjectManager = NULL;
	m_pWorld = NULL;
	int nPos;

	// dynamically allocate so that later we can have mini mud ect
	if((nPos=strOpts.Find("-z"))==-1)
	{
		m_pZoneManager = new CZoneManager(CZoneManager::DEFAULT_ZONEINDEX);
	}
	else
	{
		m_pZoneManager = new CZoneManager(strOpts.Right(nPos+1).GetWordAfter(0));
	}
	if((nPos=strOpts.Find("-m"))==-1)
	{
		m_pMobManager = new CMobManager(CMobManager::DEFAULT_MOBINDEX);
	}
	else
	{
		m_pMobManager = new CMobManager(strOpts.Right(nPos+1).GetWordAfter(0));
	}
	if((nPos=strOpts.Find("-o"))==-1)
	{
		m_pObjectManager = new CObjectManager(CObjectManager::DEFAULT_OBJINDEX);
	}
	else
	{
		m_pObjectManager = new CObjectManager(strOpts.Right(nPos+1).GetWordAfter(0));
	}
	if((nPos=strOpts.Find("-r"))==-1)
	{
		m_pWorld = new CWorld(m_pObjectManager,m_pMobManager,CWorld::DEFAULT_REALINDEX);
	}
	else
	{
		m_pWorld = new CWorld(m_pObjectManager,m_pMobManager,strOpts.Right(nPos+1).GetWordAfter(0));
	}
	//must be after world boots ;)
	m_pZoneManager->ResetZones(true,*m_pObjectManager,*m_pMobManager);

	m_pInterpetor = new CInterp(this);

	m_pObjectManager->LoadCorpses();

	m_pConnections = new CSocket(strOpts);
	MudLog << "Size of CCharacter Class: " << int(sizeof(CCharacter)) << endl;
}

CGame::~CGame()
{
	delete m_pConnections;
	delete m_pInterpetor;
	delete m_pObjectManager;
	delete m_pMobManager;
	delete m_pZoneManager;
	//must be last
	delete m_pWorld;
	CMage::CleanNewedStaticMemory();
	CPsionicist::CleanNewedStaticMemory();
	CEmpath::CleanNewedStaticMemory();
	CCleric::CleanNewedStaticMemory();
	CDefiler::CleanNewedStaticMemory();
	CRanger::CleanNewedStaticMemory();
	CMindflayer::CleanNewedStaticMemory();
	CShaman::CleanNewedStaticMemory();
	CDruid::CleanNewedStaticMemory();
	CPaladin::CleanNewedStaticMemory();
	CAntiPaladin::CleanNewedStaticMemory();
}

//this is it the main guy the big cheese
void CGame::Heartbeat(CMudTime &MudTime)
{
	int lPulse = 0;	
	while(!m_bShutDown)
	{
		lPulse = CMudTime::GetCurrentPulse();
		//we get commands every pulse
		m_pConnections->GetCommandsFromSockets();

		//if(!(Pulse%CHECK_CONNECTIONS))
		//{
			m_pConnections->NewConnections();  //return bool maybe use throws instead
			GetCharactersFromQ(m_pConnections->GetCharQ());
		//}
		m_pConnections->DoNotCharsYet();
		//GetCharctersFrom Q
		DoCharacterCommands(lPulse);

		if(!(lPulse%CMudTime::PULSES_PER_DO_VIOLENCE))
		{
			DoViolence();
		}
		if(!(lPulse%CMudTime::PULSES_PER_CHECK_ZONE_RESET))
		{
			m_pZoneManager->ResetZones(false,*m_pObjectManager,*m_pMobManager);
		}
		if(!(lPulse%CMudTime::REDUCE_OBJECT_TIMERS))
		{
			m_pObjectManager->ReduceObjectAffects(CMudTime::REDUCE_OBJECT_TIMERS);
		}
		MudTime.NextPulse();
	}
	DoNiceShutdown();
}

//gets the characters from the CSocket Q and puts them in the game.
void CGame::GetCharactersFromQ(CLinkList<CCharIntermediate> &NewChars)
{
	CCharIntermediate *CurrentNewChar = NULL;
	POSITION pos = NewChars.GetStartPosition();
	CCharacter *NewCharacter;

	if(!pos)
		return;
	CurrentNewChar = NewChars.GetNext(pos);
	//Do we want to do this if there's 20 ppl in Q all 20 ppl must
	//execute before we move to next step.  Or do we just want to 
	//Do One person?
	//ONE!!!!!!
	NewCharacter = CalcClassPtr(CurrentNewChar);
	long lPos = 0;
	if(NewCharacter->IsNewCharacter())
	{
		if(NewCharacter->ShouldBeImp())
		{
			for(int i=0;i<LVL_IMP;i++)
			{
				NewCharacter->AdvanceLevel(true,false);
			}
			NewCharacter->SendToChar("You will be imp.\r\n");
		}
		else
		{
			NewCharacter->AdvanceLevel(true,false);
		}
		//Add player to player file pos
		NewCharacter->Save(false);
		MudLog << "New player : ";
	}
	else if(NewCharacter->IsGod())
	{
		GVM.AddGod(NewCharacter);
	}
	CString strGodMsg;
	MudLog << NewCharacter->GetName() << " has entered the game" << endl;
	//GVM.SendToGods(strGodMsg,CCharacterAttributes::PREFERS_GOD_CONNECTION_LOG,NULL);
	//Add to gobal LL after we advance level
	GVM.Add(NewCharacter);
	NewCharacter->GetRoom()->SendToRoom("%s enters the game.\r\n",NewCharacter);
	NewCharacter->GetRoom()->Add(NewCharacter);
	NewCharacter->DoLook();
	NewCharacter->SendPrompt();
	NewCharacter->CheckAge();
	
	NewChars.Remove(CurrentNewChar,pos);
}

//checks to see if character has a command if so sends it to the 
//interpetor.
void CGame::DoCharacterCommands(const int lPulse)
{
	CCharacter *ch = NULL;
	POSITION pos = GVM.PlayerLL.GetStartPosition();
	while(pos)
	{
		ch = GVM.PlayerLL.GetNext(pos);
		//moved the regeneration in here
		// so we don't have to run thought the LL again... 
		ch->Regenerate();
		if(!(lPulse%CMudTime::PULSES_PER_CHECK_REMOVE_STUN))
		{
			if(ch->InPosition(POS_STUNNED) && ch->StatCheck(CON))
			{
				ch->RemovePosition(POS_STUNNED);
			}
		}
		if(!(lPulse%CMudTime::PULSES_PER_CHECK_REMOVE_KOED))
		{
			if(ch->InPosition(POS_KOED) && ch->StatCheck(CON))
			{
				ch->RemovePosition(POS_KOED);
			}
		}
		Autosave(ch,true);
		ch->CheckAge();
		///////////////////////////////
		//	do track
		//  For NPC's this will also move mob (as in regular mob movement)
		//	if it's time to
		ch->Track();
		///////////////////////////////
		//check if it is void time
		// if they have a command m_lVoidTime is set to 0 is HasCommand fnc
		//	each time CheckVoid is fun m_lVoidTime is incremented
		//  check void returns false for all NPC's
		if(ch->CheckVoid())
		{
			ch->Save();
			if(!ch->IsLinkDead())
			{
				ch->SendToChar("Voiding ....");
				m_pConnections->AddCharFromDeath(ch);
			}
			ch->ReleaseMemoryForObjects();
			ch->GetRoom()->Remove(ch);
			GVM.Remove(ch,pos);
		}
		else if(ch->ShouldCutLink())
		{
			m_pConnections->KillSocket(ch->KillSocket());
		}
		//rent or camp command
		else if(ch->ShouldRent())
		{
			if(ch->IsNPC())
			{
				ErrorLog << "Trying to rent an NPC!\r\n" << endl;
			}
			else
			{
				ch->Save();
				m_pConnections->AddCharFromDeath(ch);
				ch->ReleaseMemoryForObjects();
				ch->GetRoom()->Remove(ch);
				GVM.Remove(ch,pos);
			}
		}
		else if(ch->IsDead() && ch->IsAffectedBy(CCharacter::AFFECT_AGE_DEATH))
		{
			//uncomment the 2 lines below and you will have
			//age death with no corpse and eq left after words
			//TODO DISCUSS!
			//StopAllFromFighting(ch);
			KillChar(ch);
			ch->DeleteChar();
			ch->RemoveAffect(CCharacter::AFFECT_AGE_DEATH);
			//ch->ReleaseMemoryForObjects();
			m_pConnections->KillSocket(ch->KillSocket());
			GVM.Remove(ch,pos);
		}
		else if(ch->IsDead())
		{
			KillChar(ch);
			if(!ch->IsNPC() && !ch->IsLinkDead())
			{
				m_pConnections->AddCharFromDeath(ch);
			}
			if(m_FightLL.DoesContain(ch))
			{
				ErrorLog << "removing " << ch->GetName() << " after death!" << endl;
				m_FightLL.Remove(ch);
			}
			GVM.Remove(ch,pos);
		}
		else if(ch->HasCommand())
		{
			m_pInterpetor->InterpretCommand(ch);
		}
		//we do NPC stuff only if they don't already have
		//a command
		else if(ch->IsNPC())
		{
			//only do ai if they are in correct position
			if((!(lPulse%CNPC::DO_AI))
				&& !ch->InPosition(POS_SLEEPING)
				&& !ch->InPosition(POS_KOED)
				&& !ch->InPosition(POS_INCAP))
			{
				ch->DoAI(m_pInterpetor);
			}
		}
	}
}

////////////////////////////////////
//	DoViolence()
//	Ok here we do some killin
//	cycle through fight link list
//	see if person you are fighting is fighting
//	if not make him fight you (add to LL)
//	next ATTACK
//  If you are dead we remove you from fight LL
//	and kill you we do the same to your target if they die
//	if either can't fight then we assertfighting NULL
//	and remove them you or your target from the LL
//	then we run though and send prompt to all
//	if they aren't fighting remove them from LL
//	written by: Demetrius Comes
//	5/13/98
////////////////////////////////////
void CGame::DoViolence()
{
	POSITION pos = m_FightLL.GetStartPosition();
	CCharacter *pCh, *pDefender;

	while(pos)
	{
		pCh = m_FightLL.GetNext(pos);
		
		//are we fighting and can we fight
		if((pDefender = pCh->IsFighting()) 
			&& pCh->CanFight() 
			&& !pDefender->IsDead()
			&& pCh->IsInRoom(pDefender))
		{
			//if are defender isn't fighting 
			//and not in the fightLL make him fight
			if(!pDefender->IsFighting() 
				&& pDefender->CanFight()
				&& !m_FightLL.DoesContain(pDefender))
			{
				m_FightLL.Add(pDefender);
				pDefender->AssertFighting(pCh);
			}
			//Add memory for NPC's and track target
			pCh->AddMemory(pDefender);
			//attack target
			pCh->Attack();
			//check for autoflee
			pDefender->DoAutoFlee();

			//if you can't fight stop fighting
			if(!pCh->CanFight() || !pCh->IsFighting()
				|| !pCh->IsInRoom(pDefender))
			{
				pCh->AssertFighting(NULL);
				//must remove them from LL other wise
				//KillChar calls stopallfromfighting
				//and that will remove them from LL with
				//OUT having the position variable!
				m_FightLL.Remove(pCh,pos);
				//if your dead from you from LL and kill you
				//KillChar set all characters fighting you to 
				//assertfighting(NULL)
				if(pCh->IsDead())
				{
					KillChar(pCh);
				}
			}
			//if your defender can't fight stop them from fighting
			if(!pDefender->CanFight() || !pDefender->IsFighting()
				|| !pCh->IsInRoom(pDefender))
			{
				pDefender->AssertFighting(NULL);
				//must remove them from LL other wise
				//KillChar calls stopallfromfighting
				//and that will remove them from LL with
				//OUT having the position variable!
				m_FightLL.Remove(pDefender,pos);
				if(pDefender->IsDead())
				{
					KillChar(pDefender);
				}
			}
		}
		//your not fighting anyone or the person your 
		//fighting has left the room.
		else
		{
			pCh->AssertFighting(NULL);
			m_FightLL.Remove(pCh,pos);
		}
	}
	pos = m_FightLL.GetStartPosition();
	while(pos)
	{
		pCh = m_FightLL.GetNext(pos);
		pCh->SendPrompt();
	}
}

//////////////////////////////////////
//	Kill Character
//	Remove from Room LL
//	Remove from Game LL
//	save char
//	Make a CCharIntermediate setting state to menu
//	Add to CSocket::NotCharYet
//	written by: Demetrius Comes
//	5/19/98
//////////////////////////////////////
void CGame::KillChar(CCharacter * pDeadChar)
{
	//if player is not in a room then thay have already been killed
	if(pDeadChar->GetRoom()->DoesContain(pDeadChar))
	{
		StopAllFromFighting(pDeadChar);
		long lPos = 0;
		//send death message
		pDeadChar->GetRoom()->DeathCry(pDeadChar);
		//now remove from room
		pDeadChar->GetRoom()->Remove(pDeadChar);
		pDeadChar->UpDatePosition(POS_DEAD,false);
		//put save here instead of in DoCharacterCommands
		//that way if we crash after they died there is no 
		//chance of the death not saving
		pDeadChar->SetFuguePlane();
		//add the corpse to the room
		if(pDeadChar->ShouldMakeCorpse())
		{
			CObject *pObj = new CCorpse(pDeadChar);
			pDeadChar->GetRoom()->Add(pObj, false);
			CObjectManager::Add(pObj);
		}
		if(!pDeadChar->IsNPC())
		{
			pDeadChar->Save(true);
		}
		else
		{
			m_pMobManager->ReduceNumberInGame(pDeadChar->GetVnum());
		}
	}
}

////////////////////////////////
//	Autosave
//	Saves all players currently
//	connnected.
//	written by: Demetrius Comes
//	7/2/98
/////////////////////////////
void CGame::Autosave(CCharacter *pSaveChar, bool bSendMsg)
{
	if(!pSaveChar->IsNPC() && 
		(!(CMudTime::GetCurrentPulse()%CMudTime::PULSES_PER_DO_AUTO_SAVE)))
	{
		pSaveChar->Save(false);
		if(bSendMsg)
		{
			pSaveChar->SendToChar("Autosaving...\r\n");
		}
	}
}

//////////////////////////
//	Remove all persons fighting pCH
/////////////////////////
void CGame::StopAllFromFighting(CCharacter * pCh)
{
	POSITION pos = GVM.PlayerLL.GetStartPosition();
	CCharacter *pCurrent;
	while(pos)
	{
		pCurrent = GVM.PlayerLL.GetNext(pos);
		if(pCurrent==pCh)
		{
			m_FightLL.Remove(pCh);
			pCh->AssertFighting(NULL);
		}
		if(pCurrent->IsFighting()==pCh)
		{
			pCurrent->AssertFighting(NULL);
			m_FightLL.Remove(pCh);
		}
		if(pCh->IsDead())
		{
			pCurrent->RemoveFromMemory(pCh);
		}
	}
}

/////////////////////////////////////////
//	Static:
//	Add a fighter to the LL
void CGame::AddFighter(CCharacter *pFighter)
{
	if(m_FightLL.DoesContain(pFighter))
	{
//		ErrorLog << "Trying to add " << pFighter->GetName() << " twice to fight LL." << endl;
		return;
	}
	m_FightLL.Add(pFighter);
}

//////////////////////////////////////
//	Save everyone before shutting down!
void CGame::DoNiceShutdown()
{
	POSITION pos = GVM.PlayerLL.GetStartPosition();
	CCharacter *pCh;
	while(pos)
	{
		pCh = GVM.PlayerLL.GetNext(pos);
		pCh->Save();
		pCh->SendToChar("Game is shutting down...\r\n");
	}
}

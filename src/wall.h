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
#ifndef _WALLS_H_
#define _WALLS_H_

#include "npc.classes.h"


template<class X>
class CWall : public X
{
private:
	bool m_bIsAnimated;
	short m_nDir;
	int m_nTimeToLive;
	CWall<X> *m_pOtherSide;
protected:
	void SyncWithOtherSide();
public:
	CWall(const sWallInfo *pWallInfo, CMobPrototype &mob, CMobPrototype &OtherSide, CRoom *pPutInRoom);
	CWall(CWall<X> *p,CMobPrototype &mob,CRoom *pRoom);
public:
	virtual ~CWall();
	virtual void Attack(bool bOnlyOneAttack = false);
	virtual void DoAI(const CInterp *pInterp);
	virtual void TakeDamage(CCharacter *pAttacker,int nDam,bool bFromMeele, bool bGiveExp);
	virtual int AdvanceLevel(bool IncreaseLevel,bool bSendMsg = true,bool bIsFollower = false);
	virtual void FadeAffects(int nTime);
};

template<class X>
void CWall<X>::FadeAffects(int nTime)
{
	m_nTimeToLive-=nTime;
	if(m_nTimeToLive<=0)
	{
		//kill them
		m_nCurrentHpts = HITPTS_OF_DEATH;
		UpDatePosition();
	}
	X::FadeAffects(nTime);
}

template<class X>
void CWall<X>::SyncWithOtherSide()
{
	if(m_pOtherSide)
	{
		m_pOtherSide->m_nCurrentHpts = this->m_nCurrentHpts;
			//Make them clones of each other
		m_pOtherSide->m_nMaxHpts = this->m_nMaxHpts;
		m_pOtherSide->m_nMaxMovePts = m_nMovePts = 0;
		m_pOtherSide->m_nMaxManaPts = this->m_nMaxManaPts;
		m_pOtherSide->m_nManaPts = this->m_nManaPts;
	}
}

template<class X>
int CWall<X>::AdvanceLevel(bool IncreaseLevel,bool bSendMsg,bool bIsFollower)
{
	int n = X::AdvanceLevel(IncreaseLevel,bSendMsg,bIsFollower);
	SyncWithOtherSide();
	return n;
}

template<class X>
void CWall<X>::TakeDamage(CCharacter *pAttacker,int nDam,bool bFromMeele, bool bGiveExp)
{
	//no exp from attacking a wall unless animated
	X::TakeDamage(pAttacker,nDam,bFromMeele,m_bIsAnimated);
	SyncWithOtherSide();
}
template<class X>
void CWall<X>::DoAI(const CInterp *pInterp)
{
	//walls don't do defensive ai
	if(IsFighting() && m_bIsAnimated)
		X::DoAI(pInterp);
}

template<class X>
void CWall<X>::Attack(bool bOnlyOneAttack)
{
	if(m_bIsAnimated)
		X::Attack(bOnlyOneAttack);
}

template<class X>
CWall<X>::CWall(CWall<X> *p,CMobPrototype &mob,CRoom *pRoom)
	: X(mob,pRoom), CCharacter(mob,pRoom)
{
	m_bIsAnimated = false;
	m_bMakeCorpseOnDeath = false;
	m_nTimeToLive = p->m_nTimeToLive;
	m_nPreference = CNPC::SENTINEL;
	m_nDir = CWorld::ReverseDirection(p->m_nDir);
	assert(GetRoom());
	GetRoom()->Wall(m_nDir,false);
	GetRoom()->Add(this);
	//only add this side mob manager will add the other side
	GVM.Add(this);
	m_pOtherSide = p;
}

template<class X>
CWall<X>::CWall(const sWallInfo *pWallInfo, CMobPrototype &mob, CMobPrototype &OtherSide, CRoom *pPutInRoom)
	: X(mob,pPutInRoom), CCharacter(mob,pPutInRoom)
{
	m_bIsAnimated = false;
	m_bMakeCorpseOnDeath = false;
	m_nPreference = CNPC::SENTINEL;
	m_nDir = pWallInfo->m_nDirection;
	m_nTimeToLive = RANGE(pWallInfo->m_nTimeToLive,(CMudTime::PULSES_PER_REAL_MIN*15),CMudTime::PULSES_PER_REAL_MIN);
	assert(GetRoom());
	GetRoom()->Wall(m_nDir,false);
	GetRoom()->Add(this);
	m_pOtherSide = NULL;
	CRoom *pRoom = GetRoom()->GetRoomToThe(m_nDir);
	//incase there isn't a door in that direction
	if(pRoom!=NULL)
	{
		switch(GetClass())
		{
		case CLASS_WARRIOR:
			m_pOtherSide = new CWall<CNPCWarrior>(this,OtherSide,pRoom);
			break;
		default:
			assert(0);
		}
	}
	SyncWithOtherSide();
}

template<class X>
CWall<X>::~CWall()
{
	if(!CGame::IsShuttingDown())
	{
		GetRoom()->Wall(m_nDir,true);
		if(m_pOtherSide!=NULL)
		{
			m_pOtherSide->m_nCurrentHpts = HITPTS_OF_DEATH;
			m_pOtherSide->UpDatePosition();
			m_pOtherSide->m_pOtherSide = NULL;
		}
	}
}
#endif

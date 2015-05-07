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
#include "trackinfo.h"

extern CGlobalVariableManagement GVM;

//////////////////////////
//	CTrackInfo
CTrackInfo::CTrackInfo()
{

}

CTrackInfo::~CTrackInfo()
{

}

CString CTrackInfo::GetTrackList(CCharacter *pCh)
{
	POSITION pos = m_TrackList.GetStartPosition();
	CString str;
	while(pos)
	{
		str.Format("%s%s\r\n",
			(char *)str,
			(char *)m_TrackList.GetNext(pos)->GetName());
	}
	return str;
}

void CTrackInfo::RemoveAllTrackTargets(CCharacter *pTracker)
{
	POSITION pos = m_TrackList.GetStartPosition();
	while(pos)
	{
		RemoveTrackTarget(m_TrackList.GetNext(pos),pTracker);
	}
}

////////////////////////////////////////
//	Remove the trackie from the trackers LL
//	and removes the tracker from the trackie's LL
void CTrackInfo::RemoveTrackTarget(CCharacter *pTarget,CCharacter *pTracker,bool bRemove)
{
	m_TrackList.Remove(pTarget);
	if(bRemove)
	{
		pTarget->m_pTrackInfo->RemoveFromTrackedBy(pTracker);
	}
}

void CTrackInfo::RemoveFromAllTrackers(CCharacter *pTrackie)
{
	POSITION pos = m_TrackedBy.GetStartPosition();
	CCharacter *pTracker;
	while(pos)
	{
		pTracker = m_TrackedBy.GetNext(pos);
		//must pass in false so we don't try to remove from trackedby LL 
		//while we are looping through it
		pTracker->m_pTrackInfo->RemoveTrackTarget(pTrackie,pTracker,false);
	}
}

void CTrackInfo::AddTrackTarget(CCharacter *pTrackie,CCharacter *pTracker)
{
	if(!m_TrackList.DoesContain(pTrackie))
	{
		m_TrackList.Add(pTrackie);
		pTrackie->m_pTrackInfo->AddCharToTrackedBy(pTracker);
	}
}

//////////////////////////////////////
//	NPCTrackinfo
//////////////////////////////////////

const int CNPCTrackInfo::CHECK_RENTED_TARGETS = (CMudTime::PULSES_PER_REAL_MIN);

/////////////////////////////////////
//	Add Rented Targets
/////////////////////////////////////
void CNPCTrackInfo::AddRentedTargets(CCharacter *pTracker)
{
	if(!(CMudTime::GetCurrentPulse()%CHECK_RENTED_TARGETS))
	{
		POSITION pos = m_RentedTrackTargets.GetStartPosition();
		CCharacter *pCh;
		CString strName;
		while(pos)
		{
			strName = m_RentedTrackTargets.GetNext(pos);
			if((pCh = GVM.FindCharacter(strName,pTracker))!=NULL)
			{
				AddTrackTarget(pCh,pTracker);
				m_RentedTrackTargets.Remove(strName,pos);
			}
		}
	}
}

////////////////////////////////////////////
//for npcs we remove them from LL but add them
//to a STring LL to keep there names so even if they
//rent we can still track them later
void CNPCTrackInfo::RemoveTrackTarget(CCharacter *pTarget,CCharacter *pTracker,bool bRemove)
{
	//only do this for PC's
	//and if they are not already in there
	if(!pTarget->IsNPC() 
		&& !m_RentedTrackTargets.DoesContain(pTarget->GetName()))
	{
		CString strName(pTarget->GetName());
		m_RentedTrackTargets.Add(strName);
	}
	CTrackInfo::RemoveTrackTarget(pTarget,pTracker,bRemove);
}

////////////////////////////////////////////
//	RemoveTackTargetPerm...
//	Right now only being called from RemoveMemory
//	Which is only called when a character dies!
//	Remove the target from the rented LL 
//	and from our tracklist using the CTrackInfo::RemvoeTrackTarget
//	NOT CNPCTrackInfo::RemvoeTrackTarget or we will just
//	re-add them to the Link LIst	
void CNPCTrackInfo::RemoveTrackTargetPerm(CCharacter *pTrackie,CCharacter *pTracker) 
{
	CString strName(pTrackie->GetName());
	m_RentedTrackTargets.Remove(strName);
	CTrackInfo::RemoveTrackTarget(pTrackie,pTracker,true);
}

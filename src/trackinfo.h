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

#ifndef _TRACKINFO_H_
#define _TRACKINFO_H_

class CTrackInfo
{
public:
	CList<CCharacter *>m_TrackList; //who am I tracking
	CList<CCharacter *>m_TrackedBy; //who is tracking me
public:
	CTrackInfo();
	//add the trackie to this->m_TrackList and the Tracker to the pTarckie->m_TrackedBy List
	void AddTrackTarget(CCharacter *pTrackie,CCharacter *pTracker);
	void AddCharToTrackedBy(CCharacter *pTracker) {m_TrackedBy.Add(pTracker);}
	CString GetTrackList(CCharacter *pCh);
	bool IsTracking(CCharacter *pCh) {return m_TrackList.DoesContain(pCh);}
	void RemoveAllTrackTargets(CCharacter *pTracker);
	void RemoveFromAllTrackers(CCharacter *pTrackie);
	void RemoveFromTrackedBy(CCharacter *pCh) {m_TrackedBy.Remove(pCh);}
public: //virtual 
	virtual void RemoveTrackTarget(CCharacter *pTarget,CCharacter *pTracker,bool bRemove = true);
	virtual ~CTrackInfo();
};

class CNPCTrackInfo : public CTrackInfo
{
protected:
	static const int CHECK_RENTED_TARGETS;
public:
	CList<CString> m_RentedTrackTargets;
public:
	CNPCTrackInfo() : CTrackInfo(){;}
	virtual ~CNPCTrackInfo(){;}
	//for npcs we remove them from LL but add them
	//to a STring LL to keep there names so even if they
	//rent we can still track them later
	virtual void RemoveTrackTarget(CCharacter *pTarget,CCharacter *pTracker,bool bRemove = true);
	void RemoveTrackTargetPerm(CCharacter *pTrackie,CCharacter *pTracker);
	void AddRentedTargets(CCharacter *pTracker);
};

#endif
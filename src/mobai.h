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
// MobAI.h: interface for the CMobAI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOBAI_H__E732C843_13BC_11D3_844F_00600834E9F3__INCLUDED_)
#define AFX_MOBAI_H__E732C843_13BC_11D3_844F_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

///////////////////////////////////////////////////
//	Ok all mob ai type stuff is going to go here.
//	Reasons:  If we where to put it in each mob class
//	we would duplicate a large amount of code.
//	We can't put in NPC class because the AI is really
//	class based.

template<class ClassType>
class CMobAIManager  
{
private:
	class sMobAI
	{
	public:
		mob_vnum m_Vnum;
		bool (ClassType::*m_pAIFnc)(const CInterp *pInterp);
	public:
		sMobAI() {m_Vnum = 0;m_pAIFnc = (bool (ClassType::*)(const CInterp *pInterp))NULL;}
		sMobAI(mob_vnum mnum,bool (ClassType::*pAIFnc)(const CInterp *pInterp)){m_Vnum = mnum;m_pAIFnc = pAIFnc;}
		sMobAI(const sMobAI &r) {m_Vnum = r.m_Vnum;m_pAIFnc = r.m_pAIFnc;}
	};
	void (ClassType::*m_pGeneralFnc)(const CInterp *pInterp);
protected:
	CMap<mob_vnum,sMobAI> m_MobSpecials;
public:
	CMobAIManager(void (ClassType::*GenFnc)(const CInterp *pInterp)){m_pGeneralFnc = GenFnc;}
	void RegisterAIFnc(mob_vnum MobNum,bool (ClassType::*pAIFnc)(const CInterp *pInterp))
	{
		m_MobSpecials.Add(MobNum,sMobAI(MobNum,pAIFnc));
	}
	void RunMobAI(const CInterp *pInterp,mob_vnum MobNum, ClassType *pMob);
	virtual ~CMobAIManager(){;}
};

//////////////////////////////////////////
//	RunMobAI
template<class ClassType>
void CMobAIManager<ClassType>::RunMobAI(const CInterp *pInterp, mob_vnum MobNum, ClassType *pMob)
{
	sMobAI MobAI;
	if(m_MobSpecials.Lookup(MobNum,MobAI))
	{
#ifdef _DEBUG
		if(MobAI.m_pAIFnc==NULL)
		{
			ErrorLog << "Mob AI function is null for vnum: " << pMob->GetVnum() << endl;
			return;
		}
#endif
		if(!(pMob->*(MobAI.m_pAIFnc))(pInterp))
		{
#ifdef _DEBUG
			if(m_pGeneralFnc==NULL)
			{
				ErrorLog << "Mobs of class " << pMob->GetClass() << endl;
				return;
			}
#endif
			(pMob->*m_pGeneralFnc)(pInterp);
		}
	}
	else
	{
		(pMob->*m_pGeneralFnc)(pInterp);
	}
}

#endif // !defined(AFX_MOBAI_H__E732C843_13BC_11D3_844F_00600834E9F3__INCLUDED_)
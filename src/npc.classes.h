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
//////////////////////////////////////////////////////////////
// NPCClasses.h: interface for the CNPCWarrior class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPCCLASSES_H__5A4A7242_FCBA_11D1_81AA_00600834E9F3__INCLUDED_)
#define AFX_NPCCLASSES_H__5A4A7242_FCBA_11D1_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "npc.h"
#include "warrior.h"
#include "rogue.h"
#include "mage.h"
#include "psionicist.h"
#include "mindflayer.h"
#include "empath.h"
#include "cleric.h"
#include "defiler.h"
#include "ranger.h"
#include "shaman.h"
#include "druid.h"
#include "paladin.h"
#include "anti.paladin.h"
#include "mobai.h"
#include "list.h"

class CNPCWarrior : public CWarrior, public CNPC  
{
private:
	void DoInit();
protected:
	static CMobAIManager<CNPCWarrior> m_MobAI;
	static bool m_bInitNPCClass;
protected:
	virtual void DoGeneralAI(const CInterp *pInterp);
	virtual bool GeneralOffense(const CInterp *pInterp);
	virtual bool GeneralDefense(const CInterp *pInterp);
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
	virtual void DoAIAttack(const CInterp *pInterp, CCharacter *pTarget);
public:
	CNPCWarrior(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCWarrior();
	virtual short GetMaxHitPointsPerLevel();
	virtual void DoAI(const CInterp *pInterp);
};

class CNPCDefiler : public CDefiler, public CNPC  
{
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
public:
	CNPCDefiler(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCDefiler();
	virtual short GetMaxHitPointsPerLevel();
};


class CNPCDruid : public CDruid, public CNPC  
{
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
public:
	CNPCDruid(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCDruid();
	virtual short GetMaxHitPointsPerLevel();
};

class CNPCPaladin : public CPaladin, public CNPC  
{
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
public:
	CNPCPaladin(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCPaladin();
	virtual short GetMaxHitPointsPerLevel();
};


class CNPCAntiPaladin : public CAntiPaladin, public CNPC  
{
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
public:
	CNPCAntiPaladin(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCAntiPaladin();
	virtual short GetMaxHitPointsPerLevel();
};



class CNPCRanger : public CRanger, public CNPC  
{
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
public:
	CNPCRanger(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCRanger();
	virtual short GetMaxHitPointsPerLevel();
};


class CNPCRogue : public CRogue, public CNPC
{
private:
	void DoInit();
protected:
	static CMobAIManager<CNPCRogue> m_MobAI;
	static bool m_bInitNpcRogue;
	static short RATE_MEMORY_ROGUE_FLEES;
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
	virtual void DoGeneralAI(const CInterp *pInterp);
	virtual bool GeneralOffense(const CInterp *pInterp);
	virtual bool GeneralDefense(const CInterp *pInterp);
	virtual void DoAIAttack(const CInterp *pInterp, CCharacter *pTarget);
public:
	CNPCRogue(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCRogue();
	virtual short GetMaxHitPointsPerLevel();
	virtual void DoAI(const CInterp *pInterp);
};

class CNPCMage : public CMage, public CNPC
{
	friend class CMobAIManager<CNPCMage>;
private:
	bool IsGreaterElemental(CCharacter *pCh);
	void DoInit();
protected:
	virtual void DoCast(const CInterp *pInterp, bool bForceCast);
	virtual void DoGeneralAI(const CInterp *pInterp);
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
	virtual bool LostConcentration() {return false;} //mobs don't loose concentration
protected:
	static bool m_bInit;
	static CMobAIManager<CNPCMage> m_MobAI;
	static CList<const CSpell<CMage> *> m_SpellAI[LVL_IMP+1];
	static const short MAX_GREY_MAGE_FOLLOWERS;
	static const short MAX_GREY_MAGE_MAJOR_ELEMENTALS;
	static const short MIN_MANA_FOR_DEFENSIVE; //%
	static const int DO_DEFENSIVE_AI;
protected:
	bool GeneralDefense();
	bool GeneralOffense();
	bool DoGreyAI(const CInterp *pInterp);
public:
	virtual void DoAI(const CInterp *pInterp);
	CNPCMage(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCMage();
	virtual short GetMaxHitPointsPerLevel();
};

class CNPCPsionicist : public CPsionicist, public CNPC
{
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
public:
	CNPCPsionicist(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCPsionicist();
	virtual short GetMaxHitPointsPerLevel();
};

class CNPCMindflayer : public CMindflayer, public CNPC
{
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
public:
	CNPCMindflayer(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCMindflayer();
	virtual short GetMaxHitPointsPerLevel();
};

class CNPCEmpath : public CEmpath, public CNPC
{
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
public:
	CNPCEmpath(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCEmpath();
	virtual short GetMaxHitPointsPerLevel();
};


class CNPCCleric : public CCleric, public CNPC
{
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
public:
	CNPCCleric(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCCleric();
	virtual short GetMaxHitPointsPerLevel();
};

class CNPCShaman : public CShaman, public CNPC
{
protected:
	virtual short int CalcACWouldHit() {return CalcACMobWouldHit();}
public:
	CNPCShaman(CMobPrototype &mob, CRoom *pPutInRoom);
	virtual ~CNPCShaman();
	virtual short GetMaxHitPointsPerLevel();
};

#endif // !defined(AFX_NPCCLASSES_H__5A4A7242_FCBA_11D1_81AA_00600834E9F3__INCLUDED_)

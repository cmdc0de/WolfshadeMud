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
// Mage.h: interface for the CMage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAGE_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)
#define AFX_MAGE_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "hash.h"
//#include "spell.h"
#include "caster.h"

class CMage : public CCaster <CMage>
{	
//static members
protected:
	static CProficiency m_Proficiency;
	static CString m_strProficiency;
	static bool StaticsInitialized;
	static CSkillInfo m_SkillInfo[MAX_MUD_SKILLS];
	static CString m_strSpellList;
	//stores spells
	static CMap<CString,CSpell<CMage> *> m_SpellTable;
	//stores thing the mage can minor create
	static CMap<CString,long> m_CanMinorCreate;
	const static short int m_pThaco[];
	static const int m_Experience[LVL_IMP+1];
	//public static const stuff
public:
	static const int GENERAL;
	static const int SPHERE_DESERT;
	static const int SPHERE_TUNDRA;
	static const int SPHERE_MOUNTAIN;
	static const int SPHERE_FOREST;
	static const int SPHERE_SWAMP;
	static const int SPHERE_OCEAN;
	static const int SPHERE_GRAY;
	static const short TOTAL_NON_GRAY_SPHERES;
//none static members
protected:
	//struct stores all information about the spell being casted
	//do nothing on mob construction
	CMage(CMobPrototype &mob,CRoom *pPutInRoom);
	inline short CalcNumOfSpheres();
	short GetTotalNonGraySpheres() {return TOTAL_NON_GRAY_SPHERES;}
protected:
	virtual const CMap<CString,CSpell<CMage> *> *GetSpellTable() const{return &m_SpellTable;}
	virtual CProficiency &GetProficiency(){return m_Proficiency;}
	virtual void SendProficiency();
	virtual int GetExp(short nLvl);
	virtual short GetMaxHitPointsPerLevel();
	virtual void DoSpells();
	virtual void SendSpellList();
	virtual const CSkillInfo *GetSkillInfo(short nSkill);
	virtual int GetExpGap() {return m_Experience[GetLevel()+1] - m_Experience[GetLevel()];}
	virtual int GetExpInLevel() {return m_nExperience - m_Experience[GetLevel()];}
	virtual const CSpell<CMage> *GetSpellInfo(CString strSpell);
	virtual int CalcSpellDam(const CSpell<CMage> * pSpell);
	virtual short GetPctSpellUp(const CSpell <CMage> * pSpell);
	virtual void DoPractice(CString strToPractice, const CCharacter *pTarget);
	
protected:
	void InitStatics();
	void InitSkillInfo();
	void InitGreySphereSpells();
	void InitSwapSphereSpells();
	void InitMountainSphereSpells();
	void InitForestSphereSpells();
	void InitTundraSphereSpells();
	void InitDesertSphereSpells();
	void InitOceanSphereSpells();
	void InitMagelingSpells();
	void InitMultiSphereSpells();
	void BuildWhatCanBeMinorCreated();
	void BuildSpellList();
	void CreateElemental(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void SandBlast(const CInterp * pInterp, const CSpell<CMage> * pSpell);
	void AirShield(const CInterp * pInterp, const CSpell<CMage> * pSpell);
	void MinorCreation(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void ConjureElemental(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void FindFamiliar(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void SummonSwarm(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void Cantrip(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void RockToMud(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void Mirage(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void Wall(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void Relocate(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void CounterSpell(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void DispelMagic(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void MassInvisibility(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void SpellAffectAdd(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void PreserveCorpse(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void AnimateDead(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void BecomeLich(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void Scrye(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void GuksMagicPortal(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void GuksGlowingGlobes(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void MagicMissile(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void DelayedBlastFireball(const CInterp *pInterp,const CSpell<CMage> *pSpell);
	void CreateMagicElemental(const CInterp *pInterp,const CSpell<CMage> *pSpell);
public:
	virtual short GetThaco();
	virtual ~CMage();
public:
	CMage(CCharIntermediate *ch);
	static void CleanNewedStaticMemory();
	//static consts;
public:
	static const short MAGE_MINOR_CREATION;
	static const short MAGE_BARKSKIN;
	static const short MAGE_LIGHTNINGBOLT;
	static const short MAGE_FIREBALL;
	static const short MAGE_GUKSANGRYFIST;
	static const short MAGE_GUKSFISTFLURRY;
	static const short MAGE_GUKSWICKEDGLANCE;
	static const short MAGE_DUTHSSNOWBALL;
	static const short MAGE_DUTHSSNOWBALLSWARM;
	static const short MAGE_FIRESTORM;
	static const short MAGE_METEORSWARM;
	static const short MAGE_HURRICANE;
	static const short MAGE_MAGICMISSILE;
	static const short MAGE_PEBBLESWARM;
	static const short MAGE_SANDPILLAR;
	static const short MAGE_ROCKSWARM;
	static const short MAGE_SANDSTORM;
	static const short MAGE_BURNINGHANDS;
	static const short MAGE_FIREARROWS;
	static const short MAGE_IMMOLATE;
	static const short MAGE_INCINERATE;
	static const short MAGE_INCENDENTARYCLOUD;
	static const short MAGE_PINENEEDLES;
	static const short MAGE_SCALDINGBLAST;
	static const short MAGE_SALTSPRAY;
	static const short MAGE_WICKEDRAIN;
	static const short MAGE_HAILSTORM;
	static const short MAGE_WATERBASH;
	static const short MAGE_GUKSFLICKINGFINGER;
	static const short MAGE_GUKSSLAPPINGHAND;
	static const short MAGE_DUTHSSNOWSTORM;
	static const short MAGE_STONE_SKIN;
	static const short MAGE_COLD_SHIELD;
	static const short MAGE_CANTRIP;
	static const short MAGE_HEAT_SHIELD;
	static const short MAGE_SAND_BLAST;
	static const short MAGE_SUMMON_SWARM;
	static const short MAGE_FIND_FAMILIAR;
	static const short MAGE_DESERT_PORTAL;
	static const short MAGE_FISTS_OF_STONE;
	static const short MAGE_DETECT_MAGIC;
	static const short MAGE_AUDIBLE_GLAMOUR;
	static const short MAGE_ROCK_TO_MUD;
	static const short MAGE_MIRAGE;
	static const short MAGE_WALL_OF_STONE;
	static const short MAGE_AIRY_PORTAL;
	static const short MAGE_CAMOUFLAGE;
	static const short MAGE_AIR_SHIELD;
	static const short MAGE_AIR_CUSHION;
	static const short MAGE_COUNTER_SPELL;
	static const short MAGE_VULTURE_FLIGHT;
	static const short MAGE_DETECT_INVISIBILITY;
	static const short MAGE_DISPEL_MAGIC;
	static const short MAGE_INVISIBILITY;
	static const short MAGE_MASS_INVISIBILITY;
	static const short MAGE_MIST_FORM;
	static const short MAGE_ENSHROUD_MIST;
	static const short MAGE_ETHER_TRAVEL;
	static const short MAGE_MIST_WALK;
	static const short MAGE_MIST_SHIELD;
	static const short MAGE_MIST_BARRIER;
	static const short MAGE_MIRROR_IMAGE;
	static const short MAGE_ARMOR;
	static const short MAGE_CALL_HEART;
	static const short MAGE_PRESERVE;
	static const short MAGE_EMBALM;
	static const short MAGE_CONJURE_AIR_ELEMENTAL;
	static const short MAGE_CONJURE_EARTH_ELEMENTAL;
	static const short MAGE_CONJURE_WATER_ELEMENTAL;
	static const short MAGE_CONJURE_FIRE_ELEMENTAL;
	static const short MAGE_CREATE_ICE_ELEMENTAL;
	static const short MAGE_CREATE_TOMB_ELEMENTAL;
	static const short MAGE_CONJURE_FOG_ELEMENTAL;
	static const short MAGE_CONJURE_GRAVE_ELEMENTAL;
	static const short MAGE_CONJURE_BLOOD_ELEMENTAL;
	static const short MAGE_CONJURE_PYRE_ELEMENTAL;
	static const short MAGE_CREATE_COAL_ELEMENTAL;
	static const short MAGE_CREATE_ASH_ELEMENTAL;
	static const short MAGE_CREATE_STEAM_ELEMENTAL;
	static const short MAGE_CREATE_PLASMA_ELEMENTAL;
	static const short MAGE_CREATE_MIST_ELEMENTAL;
	static const short MAGE_CREATE_SMOKE_ELEMENTAL;
	static const short MAGE_CREATE_SLIME_ELEMENTAL;
	static const short MAGE_CREATE_ROCK_ELEMENTAL;
	static const short MAGE_CREATE_MUD_ELEMENTAL;
	static const short MAGE_CREATE_GORE_ELEMENTAL;
	static const short MAGE_DEXTERITY;
	static const short MAGE_STRENGTH;
	static const short MAGE_AGILITY;
	static const short MAGE_ANIMATE_DEAD;
	static const short MAGE_CREATE_SPECTRE;
	static const short MAGE_CREATE_GHOST;
	static const short MAGE_CREATE_GHOUL;
	static const short MAGE_CREATE_WIGHT;
	static const short MAGE_CREATE_WRAITH;
	static const short MAGE_BECOME_LICH;
	static const short MAGE_WALL_OF_AIR;
	static const short MAGE_WALL_OF_BONES;
	static const short MAGE_WALL_OF_BRAMBLES;
	static const short MAGE_WALL_OF_FLAMES;
	static const short MAGE_WALL_OF_GLASS;
	static const short MAGE_WALL_OF_ICE;
	static const short MAGE_WALL_OF_SAND;
	static const short MAGE_WALL_OF_WATER;
	static const short MAGE_EARTHENALIZE;
	static const short MAGE_FAERIE_FLIGHT;
	static const short MAGE_FIRE_SHIELD;
	static const short MAGE_SCRYE;
	static const short MAGE_FORESTAL_ARMOR;
	static const short MAGE_GUKS_MAGIC_PORTAL;
	static const short MAGE_GUKS_INTERPOSING_HAND;
	static const short MAGE_GUKS_GLOWING_GLOBES;
	static const short MAGE_HASTE;
	static const short MAGE_FAERIE_FIRE;
	static const short MAGE_DELAYED_BLAST_FIREBALL;
	static const short MAGE_CREATE_MAGIC_ELEMENTAL;
	static const short MAGE_ACID_MISSILE;
	static const short MAGE_EXPLODING_SPHERES;
	static const short MAGE_AWAKENED_NIGHTMARE;
	static const short MAGE_HELL_FIRE;
	static const short MAGE_PRISMATIC_SHIELD;
	static const short MAGE_FIRE_AND_ICE;
};

inline short CMage::CalcNumOfSpheres()
{
	short nSpheres = 0;
	for(register int i = 0;i<GetTotalNonGraySpheres();i++)
	{
		if(m_nSpellSphere & (1<<i))
		{
			nSpheres++;
		}
	}
	return nSpheres;
}
#endif // !defined(AFX_MAGE_H__BE815C02_0053_11D2_81AA_00600834E9F3__INCLUDED_)

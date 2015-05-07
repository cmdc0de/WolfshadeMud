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
// Mage.cpp: implementation of the CMage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "mage.h"
#include "portal.h"
//////////////////////////////
//	Statics
////////////////////////////

#define MAX_RELOCATE_LAG 6

extern CRandom GenNum;

extern CGlobalVariableManagement GVM;

bool CMage::StaticsInitialized = false;
CSkillInfo CMage::m_SkillInfo[MAX_MUD_SKILLS];
CString CMage::m_strSpellList;
CProficiency CMage::m_Proficiency;
CString CMage::m_strProficiency("You class has the following proficiencies:\r\n");
const int CMage::GENERAL = (1<<0);
const int CMage::SPHERE_DESERT = (1<<1);
const int CMage::SPHERE_TUNDRA = (1<<2);
const int CMage::SPHERE_MOUNTAIN = (1<<3);
const int CMage::SPHERE_FOREST = (1<<4);
const int CMage::SPHERE_SWAMP = (1<<5);
const int CMage::SPHERE_OCEAN = (1<<6);
const int CMage::SPHERE_GRAY = (SPHERE_DESERT | SPHERE_TUNDRA | SPHERE_MOUNTAIN | SPHERE_FOREST | SPHERE_SWAMP | SPHERE_OCEAN);
const short CMage::TOTAL_NON_GRAY_SPHERES = 7;

const int CMage::m_Experience[LVL_IMP+1] = 
{
	/*0*/	0,		/*1*/	1,		/*2*/   300,	/*3*/   900,
	/*4*/	1800,	/*5*/	3000,	/*6*/   4500,	/*7*/   6300,
	/*8*/	8400,	/*9*/	10800,	/*10*/  13500,	/*11*/  16500,
	/*12*/  23100,	/*13*/  30300,	/*14*/  38100,	/*15*/  46500,
	/*16*/  55500,	/*17*/  65100,	/*18*/  75300,	/*19*/  86100,
	/*20*/  97500,	/*21*/  109500,	/*22*/  134700,	/*23*/  161100,
	/*24*/  188700,	/*25*/  217500,	/*26*/  247500,	/*27*/  278700,
	/*28*/  311100,	/*29*/  344700,	/*30*/  379500,	/*31*/  451500,
	/*32*/  525900,	/*33*/  602700,	/*34*/  681900,	/*35*/  763500,
	/*36*/  847500,	/*37*/  933900,	/*38*/  1022700,/*39*/  1113900,
	/*40*/  1207500,/*41*/  1399500,/*42*/  1596300,/*43*/  1797900,
	/*44*/  2004300,/*45*/  2215500,/*46*/  2431500,/*47*/  2652300,
	/*48*/  2877900,/*49*/  3108300,/*50*/  3343500,/*51*/  3583500,
	/*52*/  4073100,/*53*/  4572300,/*54*/  5081100,/*55*/  5599500,
	/*56*/  6127500,/*57*/  6665100,/*58*/  7212300,/*59*/  7769100,
	/*60*/  8335500

};

const short int CMage::m_pThaco[] = 
{
	/*1,02,03,04,05,06,07,08,09,10*/
	100,98,97,95,94,92,91,89,88,86,
	 85,83,82,80,79,77,76,75,73,71,
	 70,68,67,65,64,62,61,59,58,56,
	 55,53,51,49,48,46,45,43,42,40,
	 39,37,36,35,33,32,30,29,27,26,
	 25,23,22,20,19,17,16,14,13,11 /*51-60*/
};

//CMemoryAllocator<CMap<CString,char *,CMageSpell>::CNode> CMap<CString,char *,CMageSpell>::CNode::m_Memory(1,10,sizeof(CMap<CString,char *,CMageSpell>::CNode));
CMap<CString,CSpell<CMage> *> CMage::m_SpellTable((26*MIN_SPELL_LETTERS),MIN_SPELL_LETTERS);
CMap<CString,long> CMage::m_CanMinorCreate;

////////////////////////////
//Spell id's for mages
//	How spell id's work...each class defines a spell id for each spell
//	this number is then used to reference the ability to cast that spell
//	in m_Spells[].
const short CMage::MAGE_MINOR_CREATION=0;
const short CMage::MAGE_BARKSKIN=1;
const short CMage::MAGE_LIGHTNINGBOLT=2;
const short CMage::MAGE_FIREBALL=3;
const short CMage::MAGE_GUKSANGRYFIST=4;
const short CMage::MAGE_GUKSFISTFLURRY=5;
const short CMage::MAGE_GUKSWICKEDGLANCE=6;
const short CMage::MAGE_DUTHSSNOWBALL=7;
const short CMage::MAGE_DUTHSSNOWBALLSWARM=8;
const short CMage::MAGE_FIRESTORM=9;
const short CMage::MAGE_METEORSWARM=10;
const short CMage::MAGE_HURRICANE=11;
const short CMage::MAGE_MAGICMISSILE=12;
const short CMage::MAGE_PEBBLESWARM=13;
const short CMage::MAGE_SANDPILLAR=14;
const short CMage::MAGE_ROCKSWARM=15;
const short CMage::MAGE_SANDSTORM=16;
const short CMage::MAGE_BURNINGHANDS=17;
const short CMage::MAGE_FIREARROWS=18;
const short CMage::MAGE_IMMOLATE=19;
const short CMage::MAGE_INCINERATE=20;
const short CMage::MAGE_INCENDENTARYCLOUD=21;
const short CMage::MAGE_PINENEEDLES=22;
const short CMage::MAGE_SCALDINGBLAST=23;
const short CMage::MAGE_SALTSPRAY=24;
const short CMage::MAGE_WICKEDRAIN=25;
const short CMage::MAGE_HAILSTORM=26;
const short CMage::MAGE_WATERBASH=27;
const short CMage::MAGE_GUKSFLICKINGFINGER=28;
const short CMage::MAGE_GUKSSLAPPINGHAND=29;
const short CMage::MAGE_DUTHSSNOWSTORM=30;
const short CMage::MAGE_STONE_SKIN=31;
const short CMage::MAGE_COLD_SHIELD=32;
const short CMage::MAGE_CANTRIP=33;
const short CMage::MAGE_HEAT_SHIELD=34;
const short CMage::MAGE_SAND_BLAST=35;
const short CMage::MAGE_SUMMON_SWARM=36;
const short CMage::MAGE_FIND_FAMILIAR=37;
const short CMage::MAGE_DESERT_PORTAL=38;
const short CMage::MAGE_FISTS_OF_STONE=39;
const short CMage::MAGE_DETECT_MAGIC=40;
const short CMage::MAGE_AUDIBLE_GLAMOUR=41;
const short CMage::MAGE_ROCK_TO_MUD=42;
const short CMage::MAGE_MIRAGE=43;
const short CMage::MAGE_WALL_OF_STONE=44;
const short CMage::MAGE_AIRY_PORTAL=45;
const short CMage::MAGE_CAMOUFLAGE=46;
const short CMage::MAGE_AIR_SHIELD=47;
const short CMage::MAGE_AIR_CUSHION=48;
const short CMage::MAGE_COUNTER_SPELL=49;
const short CMage::MAGE_VULTURE_FLIGHT=50;
const short CMage::MAGE_DETECT_INVISIBILITY=51;
const short CMage::MAGE_DISPEL_MAGIC=52;
const short CMage::MAGE_INVISIBILITY=53;
const short CMage::MAGE_MASS_INVISIBILITY=54;
const short CMage::MAGE_MIST_FORM=55;
const short CMage::MAGE_ENSHROUD_MIST=56;
const short CMage::MAGE_ETHER_TRAVEL=57;
const short CMage::MAGE_MIST_WALK=58;
const short CMage::MAGE_MIST_SHIELD=59;
const short CMage::MAGE_MIST_BARRIER=60;
const short CMage::MAGE_MIRROR_IMAGE=61;
const short CMage::MAGE_ARMOR=62;
const short CMage::MAGE_CALL_HEART=63;
const short CMage::MAGE_PRESERVE=64;
const short CMage::MAGE_EMBALM=65;
const short CMage::MAGE_CONJURE_AIR_ELEMENTAL=66;
const short CMage::MAGE_CONJURE_EARTH_ELEMENTAL=67;
const short CMage::MAGE_CONJURE_WATER_ELEMENTAL=68;
const short CMage::MAGE_CONJURE_FIRE_ELEMENTAL=69;
const short CMage::MAGE_CREATE_ICE_ELEMENTAL=70;
const short CMage::MAGE_CREATE_TOMB_ELEMENTAL=71;
const short CMage::MAGE_CONJURE_FOG_ELEMENTAL=72;
const short CMage::MAGE_CONJURE_GRAVE_ELEMENTAL=73;
const short CMage::MAGE_CONJURE_BLOOD_ELEMENTAL=74;
const short CMage::MAGE_CONJURE_PYRE_ELEMENTAL=75;
const short CMage::MAGE_CREATE_COAL_ELEMENTAL=76;
const short CMage::MAGE_CREATE_ASH_ELEMENTAL=77;
const short CMage::MAGE_CREATE_STEAM_ELEMENTAL=78;
const short CMage::MAGE_CREATE_PLASMA_ELEMENTAL=79;
const short CMage::MAGE_CREATE_MIST_ELEMENTAL=80;
const short CMage::MAGE_CREATE_SMOKE_ELEMENTAL=81;
const short CMage::MAGE_CREATE_SLIME_ELEMENTAL=82;
const short CMage::MAGE_CREATE_ROCK_ELEMENTAL=83;
const short CMage::MAGE_CREATE_MUD_ELEMENTAL=84;
const short CMage::MAGE_CREATE_GORE_ELEMENTAL=85;
const short CMage::MAGE_DEXTERITY=86;
const short CMage::MAGE_STRENGTH=87;
const short CMage::MAGE_AGILITY=88;
const short CMage::MAGE_ANIMATE_DEAD=89;
const short CMage::MAGE_CREATE_SPECTRE=90;
const short CMage::MAGE_CREATE_GHOST=91;
const short CMage::MAGE_CREATE_GHOUL=92;
const short CMage::MAGE_CREATE_WIGHT=93;
const short CMage::MAGE_CREATE_WRAITH=94;
const short CMage::MAGE_BECOME_LICH=95;
const short CMage::MAGE_WALL_OF_AIR=96;
const short CMage::MAGE_WALL_OF_BONES=97;
const short CMage::MAGE_WALL_OF_BRAMBLES=98;
const short CMage::MAGE_WALL_OF_FLAMES=99;
const short CMage::MAGE_WALL_OF_GLASS=100;
const short CMage::MAGE_WALL_OF_ICE=101;
const short CMage::MAGE_WALL_OF_SAND=102;
const short CMage::MAGE_WALL_OF_WATER=103;
const short CMage::MAGE_EARTHENALIZE=104;
const short CMage::MAGE_FAERIE_FLIGHT=105;
const short CMage::MAGE_FIRE_SHIELD=106;
const short CMage::MAGE_SCRYE=107;
const short CMage::MAGE_FORESTAL_ARMOR=108;
const short CMage::MAGE_GUKS_MAGIC_PORTAL=109;
const short CMage::MAGE_GUKS_INTERPOSING_HAND=110;
const short CMage::MAGE_GUKS_GLOWING_GLOBES=111;
const short CMage::MAGE_HASTE=112;
const short CMage::MAGE_FAERIE_FIRE=113;
const short CMage::MAGE_DELAYED_BLAST_FIREBALL=114;
const short CMage::MAGE_CREATE_MAGIC_ELEMENTAL = 115;
const short CMage::MAGE_ACID_MISSILE = 116;
const short CMage::MAGE_EXPLODING_SPHERES = 117;
const short CMage::MAGE_AWAKENED_NIGHTMARE = 118;
const short CMage::MAGE_HELL_FIRE =119;
const short CMage::MAGE_PRISMATIC_SHIELD =120;
const short CMage::MAGE_FIRE_AND_ICE = 121;


//////////////////////////////

///////////////////////////////////////
//	InitSpellTable
//	Initalizes the spells and the things
//	this type of caster can cast and minor
//	create.
//	written by: Demetrius Comes
////////////////////////////////////
void CMage::InitStatics()
{
	if(StaticsInitialized)
	{
		return;
	}
	StaticsInitialized = true;
	InitSkillInfo();
	InitMultiSphereSpells();
	InitMagelingSpells();
	InitOceanSphereSpells();
	InitDesertSphereSpells();
	InitTundraSphereSpells();
	InitForestSphereSpells();
	InitMountainSphereSpells();
	InitSwapSphereSpells();
	InitGreySphereSpells();	
	BuildWhatCanBeMinorCreated();
	BuildSpellList();
}

/////////////////////////////
//	Sets up the skills a mage can learn
//	
void CMage::InitSkillInfo()
{											/*max,min,pct*/
// Base Skills
	m_SkillInfo[SKILL_TRACK]=CSkillInfo(GENERAL,50,0,12);
	m_SkillInfo[SKILL_BODYSLAM]=CSkillInfo(GENERAL,50,1,10);
    m_SkillInfo[SKILL_BANDAGE]=CSkillInfo(GENERAL,100,1,100);
    m_SkillInfo[SKILL_BASH]=CSkillInfo(GENERAL,25,0,10);
    m_SkillInfo[SKILL_DODGE]=CSkillInfo(GENERAL,100,1,10);
    m_SkillInfo[SKILL_DUAL_WIELD]=CSkillInfo(GENERAL,75,0,10);
    m_SkillInfo[SKILL_OFFENSE]=CSkillInfo(GENERAL,100,1,10);
    m_SkillInfo[SKILL_FIRST_AID]=CSkillInfo(GENERAL,100,1,10);
    m_SkillInfo[SKILL_MOUNT]=CSkillInfo(GENERAL,100,1,10);
    m_SkillInfo[SKILL_BACKSTAB]=CSkillInfo(GENERAL,50,0,10);
    m_SkillInfo[SKILL_TUNDRA_MAGIC]=CSkillInfo(GENERAL,100,1,100);
    m_SkillInfo[SKILL_DESERT_MAGIC]=CSkillInfo(GENERAL,100,1,100);
    m_SkillInfo[SKILL_MOUNTAIN_MAGIC]=CSkillInfo(GENERAL,100,1,100);
    m_SkillInfo[SKILL_FOREST_MAGIC]=CSkillInfo(GENERAL,100,1,100);
    m_SkillInfo[SKILL_SWAMP_MAGIC]=CSkillInfo(GENERAL,100,1,100);
    m_SkillInfo[SKILL_FORAGE]=CSkillInfo(GENERAL,100,1,100);
    m_SkillInfo[SKILL_RESCUE]=CSkillInfo(GENERAL,75,1,10);
    m_SkillInfo[SKILL_SWITCH]=CSkillInfo(GENERAL,100,1,10);
    m_SkillInfo[SKILL_TRIP]=CSkillInfo(GENERAL,50,0,10);
    m_SkillInfo[SKILL_KICK]=CSkillInfo(GENERAL,50,0,10);
    m_SkillInfo[SKILL_CLERIC_MAGIC]=CSkillInfo(GENERAL,50,0,10);
    m_SkillInfo[SKILL_SHAMAN_MAGIC]=CSkillInfo(GENERAL,75,0,10);
    m_SkillInfo[SKILL_BLIND_FIGHTING]=CSkillInfo(GENERAL,50,1,10);

    m_SkillInfo[SKILL_THROW_WEAPON]=CSkillInfo(GENERAL,50,0,10);
    m_SkillInfo[SKILL_PREPARE]=CSkillInfo(GENERAL,100,1,50);
    m_SkillInfo[SKILL_MEDITATE]=CSkillInfo(GENERAL,100,1,50);
    m_SkillInfo[SKILL_QUICK_CHANT]=CSkillInfo(GENERAL,100,1,50);

	//profeciencies
	m_Proficiency.Add("General",0,0,0,GENERAL,0);
	m_Proficiency.GetProficiencyNames(m_strProficiency);
	m_strProficiency.Format("%s\r\n%-30s\t%-20s\r\n",
		(char *)m_strProficiency,"Skill Name","Proficiency Name");
	register int i;
	CString str;
	for(i=0;i<MAX_MUD_SKILLS;i++)
	{
		if(m_SkillInfo[i].CanLearn())
		{
			str.Format("%s%-30s\t%-20s\r\n",
				(char *)str,
				(char *)FindSkill(i),
				(char *)m_Proficiency.GetSingleProficiencyName(m_SkillInfo[i].GetSet()));
		}
	}
	m_strProficiency+=str;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMage::CMage(CCharIntermediate *ch) : CCaster<CMage>(ch),CCharacter(ch)
{
	InitStatics();
	//can't put in CCharacter construct because
	//it will be called before us
	if(IsNewbie())
	{
		InitSkills();
		m_nSkillSphere |= GENERAL;
	}
}

CMage::CMage(CMobPrototype &mob,CRoom *pPutInRoom)
	: CCaster<CMage>(mob,pPutInRoom),CCharacter(mob,pPutInRoom)
{
	InitStatics();
	//everyone gets general
	m_nSkillSphere |= GENERAL;
}

CMage::~CMage()
{
}

///////////////////////
//	GetSkillInfo
//
///////////////////////
const CSkillInfo* CMage::GetSkillInfo(short nSkill)
{
	return &m_SkillInfo[nSkill];
}

/////////////////////////
//  GetThaco()
//	pure virtual in CCharacter
///////////////////////
short CMage::GetThaco()
{
	return m_pThaco[m_nLevel];
}

////////////////////////////
//	GetMaxHitPointsPerLevel()
//	Pure Vitual in CCharacter
////////////////////////////
short CMage::GetMaxHitPointsPerLevel()
{
	return 4;
}

/////////////////////////////////////
//	Spell - MinorCreation
//	creates an object in a room
//	see if they can minor create object
//	if they can't return
//	if they can create the object and put
//	it in the room.
//	written by: Demetrius Comes
////////////////////////////////////
void CMage::MinorCreation(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	long lVnum;

	if(!m_CanMinorCreate.Lookup(m_CurrentSpell.m_strTarget,lVnum))
	{
		SendToChar("Create What???\r\n");
		return;
	}

	CObject *pObj = pInterp->CreateObj(lVnum,GetRoom());

	if(!pObj)
	{
		SendToChar("That object could not be created!\r\n");
		return;
	}
	
	CString strToChar;
	strToChar.Format("You magically create a %s\r\n",pObj->GetObjName(this));

	SendToChar(strToChar);
	m_pInRoom->SendToRoom("%s magically creates a %s\r\n",this,pObj);
}

void CMage::Cantrip(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	CString strToRoom(CurrentCommand.GetWordAfter(2,true));
	strToRoom.Format("%s \r\n",(char *)strToRoom);
	m_pInRoom->SendToRoom(strToRoom);
}

////////////////////////////////////
//	ConjureElemental
//	Creates an elemental from a random roll
//	written by: John Comes
////////////////////////////////////
void CMage::ConjureElemental(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	long lElemental;
	CString strToChar;
	short nLevel = (DIE(2)==1) ? GetLevel()+DIE(5) : GetLevel()-DIE(5);
	if(m_nAlignment > 1)
	{
		if(pSpell->IsOfSphere(SPHERE_MOUNTAIN))
		{
			lElemental = VNUM_FIRE_ELEMENTAL;
			strToChar = "&RA Fire Elemental appears in a burst of Flame!&n\r\n";
		}
		else if(pSpell->IsOfSphere(SPHERE_TUNDRA))
		{
			lElemental = VNUM_AIR_ELEMENTAL;
			strToChar = "&CAn Air Elemental appears with a gust of wind!&n\r\n";
		}
		else if(pSpell->IsOfSphere(SPHERE_DESERT))
		{
			lElemental = VNUM_EARTH_ELEMENTAL;
			strToChar = "&yAn Earth Elemental bursts from the rock at your feet!&n\r\n";
		}
		else
		{
			lElemental = VNUM_WATER_ELEMENTAL;
			strToChar = "&BA Water Elemental forms for the moisture around you!&n\r\n";
		}
	}
	else
	{
		if(pSpell->IsOfSphere(SPHERE_MOUNTAIN))
		{
			lElemental = VNUM_PYRE_ELEMENTAL;
			strToChar = "&RA &LP&ry&Lr&re &RElemental appears in a burst of Flame!&n\r\n";
		}
		else if(pSpell->IsOfSphere(SPHERE_TUNDRA))
		{
			lElemental = VNUM_FOG_ELEMENTAL;
			strToChar = "&cA &wF&co&wg &cElemental appears with a gust of wind!&n\r\n";
		}
		else if(pSpell->IsOfSphere(SPHERE_DESERT))
		{
			lElemental = VNUM_GRAVE_ELEMENTAL;
			strToChar = "&yA &LG&yr&La&yv&Le &yElemental bursts from the rock at your feet!&n\r\n";
		}
		else
		{
			lElemental = VNUM_BLOOD_ELEMENTAL;
			strToChar = "&BA &rBlood &BElemental forms from the moisture around you!&n\r\n";
		}
	}

	CCharacter *pMob = pInterp->CreateMob(lElemental,GetRoom(),CalcSpellDam(pSpell),nLevel);
	if(this->CurrentlyMasterOf()<this->CalcNumOfSpheres())
	{
		pMob->SetMaster(this,(m_Spells[pSpell->GetSpellID()]*CMudTime::PULSES_PER_MUD_HOUR));
	}
	else
	{
		strToChar.Format("%s&r%s resists your magical hold.\r\nYou do not control %s.&n\r\n",
			(char *)strToChar, (char *)pMob->GetName(),(char *)pMob->GetName());
	}
	pMob->SetNoCorpse();
	SendToChar(strToChar);

	m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pMob);

	if(lElemental == VNUM_FIRE_ELEMENTAL || lElemental == VNUM_PYRE_ELEMENTAL)
	{
		pMob->AddAffect(CCharacter::AFFECT_HEAT_SHIELD,sAffect::PERM_AFFECT,0);
	}
	else if(lElemental == VNUM_FOG_ELEMENTAL || lElemental == VNUM_AIR_ELEMENTAL)
	{
		pMob->AddAffect(CCharacter::AFFECT_INVISIBLE,sAffect::PERM_AFFECT,0);
	}
	else if(lElemental == VNUM_EARTH_ELEMENTAL || lElemental == VNUM_GRAVE_ELEMENTAL)
	{
		pMob->AddAffect(CCharacter::AFFECT_STONE_SKIN,sAffect::MANUAL_AFFECT,CalcSpellDam(pSpell));
	}
}

//////////////////////////////////
//	Spells
//	Send character the spells they know
//	as well as how good they are at that spell
/////////////////////////////////
void CMage::DoSpells()
{
	CSpell<CMage> *pSpell;
	POSITION pos = m_SpellTable.GetStartingPosition();
	CString strToChar("Spheres known: \r\n");
	if(m_nSpellSphere==0)
	{
		strToChar+="None\r\n";
	}
	else if(IsOfSphere(SPHERE_GRAY))
	{
		strToChar+="&LGray&n\r\n";
	}
	else
	{
		if(IsOfSphere(SPHERE_DESERT))
		{
			strToChar+="&YDesert&n\r\n";
		}
		if(IsOfSphere(SPHERE_TUNDRA))
		{
			strToChar+="&WTundra&n\r\n";
		}
		if(IsOfSphere(SPHERE_MOUNTAIN))
		{
			strToChar+="&RMountain&n\r\n";
		}
		if(IsOfSphere(SPHERE_FOREST))
		{
			strToChar+="&GForest&n\r\n";
		}
		if(IsOfSphere(SPHERE_SWAMP))
		{
			strToChar+="&LSwamp&n\r\n";
		}
		if(IsOfSphere(SPHERE_OCEAN))
		{
			strToChar+="&BOcean&n\r\n";
		}
	}
	strToChar+="Spells Known:\r\n";
	while(pos)
	{
		pSpell = m_SpellTable.GetNext(pos);
		if(m_Spells[pSpell->GetSpellID()])
		{
			strToChar.Format("%s%-30s\t%s\r\n",(char *) strToChar,
				(char *)pSpell->GetColorizedName(),this->SkillWording(m_Spells[pSpell->GetSpellID()]));
		}
	}
	SendCharPage(strToChar);
}

/////////////////////////////
//	Not really needed for real OS
//	but we'll be friendly to 95/98 =)
//////////////////////////////
void CMage::CleanNewedStaticMemory()
{
	POSITION pos = m_SpellTable.GetStartingPosition();
	while(pos)
	{
		delete m_SpellTable.GetNext(pos);
	}
}

////////////////////
//	Calculates spell damage.
///////////////////
int CMage::CalcSpellDam(const CSpell<CMage> * pSpell)
{
	// if its a mageling or gray sphere
	// the damage is the ((X D X)*Skill%)/100 + ExtraDam
	if(pSpell->GetSphere()==0 || //mageling spell
		(pSpell->IsOfSphere(SPHERE_GRAY)))
	{
		return pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]);
	}
	else
	{
		// the damage is the (((X D X)*Skill%)/100 + ExtraDam)/Number of sphers
		short nSpheres = CalcNumOfSpheres();
		//if not a mageling spell we better have at least one
		//sphere.
		if(nSpheres==0)
		{
			ErrorLog << GetName() << " is casting spell " << pSpell->GetSpellName() << " but has no spheres and needs at least one" << endl;
			m_nSpellSphere |= pSpell->GetSphere();
		}
		return pSpell->GetDamage(m_Spells[pSpell->GetSpellID()])/nSpheres;
	}
}

///////////////////////
//	GetExp
//	returns exp for level
///////////////////////
int CMage::GetExp(short nLvl)
{
	return m_Experience[nLvl];
}

////////////////////////////////
//  SandBlast
//  John Comes 10-20-98
///////////////////////////////
void CMage::SandBlast(const CInterp * pInterp, const CSpell<CMage> * pSpell)
{
	if(m_pAttributes->MakeSavingThrow(CAttributes::SAVE_SPELL))  //arbitrary number untill saves are put in
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,m_CurrentSpell.m_pTarget); 
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]),0);
	}
	else
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,m_CurrentSpell.m_pTarget); 
	}
}
 
///////////////////////
//	Summon Swarm
//
//	John Comes 10-21-98
//////////////////////
void CMage::SummonSwarm(const CInterp * pInterp, const CSpell<CMage> * pSpell)
{
	if(m_pInRoom->IsAffectedBy(CRoom::ROOM_AFFECT_PEACEFUL))
	{
		SendToChar("This place is far to peacefull.\r\n");
		return;
	}
	SendToChar("You conjure forth a &GSwarm&n of animals!\r\n");
	GetRoom()->SendToRoom("%s conjures forth a &GSwarm&n of animals!\r\n",this);
	mob_vnum lSwarm = DIE(4)+9; // Mob Vnum 10-13
	short int iMaxMobs;
	iMaxMobs = CalcSpellDam(pSpell);
	CCharacter *pTarget;
	POSITION pos;
	register int i;
	for(i = 0;i < iMaxMobs;i++)
	{
		pos = m_pInRoom->GetFirstCharacterInRoom();
		while(pos && (i < iMaxMobs))
		{
			pTarget = m_pInRoom->GetNextCharacter(pos);
				//make sure it's not me
			if(pTarget!=this && !m_Group.IsInGroup(pTarget) && !m_Master.IsMasterOf(pTarget))
			{
				CCharacter *pMob = pInterp->CreateMob(lSwarm,GetRoom());
				if(pMob!=NULL)
				{
					GetRoom()->SendToRoom("%s charges in and attacks %s!\r\n",pMob,pTarget);
					pTarget->AddFighters(pMob,false);
					//pMob->SetMaster(this,false);
					i++;
				}
				else
				{
					ErrorLog << "Summon swarm has a bad vnum to create mobs!" << endl;
					SendToChar("Report this error!\r\n");
				}
			}
		}
	}
}

////////////////////////////////////
//	Find Familiar
//	Creates an animal from a random roll
//	written by: John Comes 10-21-98
////////////////////////////////////
void CMage::FindFamiliar(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	mob_vnum Familiar = DIE(8)+9; //mobs 10-17 in 1.mob
	CCharacter *pMob = pInterp->CreateMob(Familiar,GetRoom());
	if(pMob!=NULL)
	{
		m_pInRoom->SendToRoom("%s calls to the wild for a familiar.\r\n",this);
		if(this->CurrentlyMasterOf()<this->CalcNumOfSpheres())
		{
			pMob->SetMaster(this,(m_Spells[pSpell->GetSpellID()]*CMudTime::PULSES_PER_MUD_HOUR));
		}
		else
		{
			CString str;
			str.Format("&r%s resists your magical hold.\r\nYou do not control %s.&n\r\n",
				(char *)pMob->GetName(),(char *)pMob->GetName());
			SendToChar(str);
		}
	}
	else
	{
		SendToChar("No friends will find you today!\r\n");
		ErrorLog << "Missing mob prototype for FindFamiliar" << endl;
	}
}

/////////////////////////
//	Relocate
//
//	Relocate Type Spells
//	John Comes 10-21-98 / Modified 12-21-98
/////////////////////////
void CMage::Relocate(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	CCharacter *pTarget = GVM.FindCharacter(m_CurrentSpell.m_strTarget,this);
	if(!pTarget || pTarget->IsNPC())
	{
		SendToChar("That person is not currently playing!\r\n");
	}
	else if(pTarget->IsGod())
	{
		SendToChar("You can't do that do a God!\r\n");
	}
	else
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget); 
		//change the room the caster is in
		GetRoom()->Remove(this);
		this->m_pInRoom = pTarget->GetRoom();
		m_pInRoom->Add(this);
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_KILLED,this,pTarget);
		//SKILL_KILLED is the message for the completion of this spell
		AddCommand((CString)"look");
		LagChar((MAX_RELOCATE_LAG - CalcSpellDam(pSpell)) * CMudTime::PULSES_PER_REAL_SECOND);
	}
}

///////////////////////////////
//	Rock to Mud
//	Does not currently have saving throws
//	Target falls if failed, doesn't if pass
//		
//   11-25-98 John Comes
//////////////////////////////
void CMage::RockToMud(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	CCharacter *pTarget;
	pTarget = m_CurrentSpell.m_pTarget;
	if(m_pAttributes->MakeSavingThrow(CAttributes::SAVE_SPELL) && (pTarget->InPosition(POS_STANDING))) 
	{
		pTarget->UpDatePosition(POS_SITTING,false);
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pTarget); 
		pTarget->LagChar(CalcSpellDam(pSpell));
	}
	else
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,pTarget); 
	}
	return;
}

/////////////////////////////////
//   Mirage
//
//	Leaves an object with the sitting desc of what is typed.
//
//	11-25-98 John Comes
////////////////////////////////
void CMage::Mirage(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	if(m_CurrentSpell.m_strTarget.IsEmpty())
	{
		SendToChar("Create what mirage?\r\n");
	}
	else
	{
		CObject *pObj = pInterp->CreateObj(VNUM_MIRAGE,GetRoom());
		if(pObj!=NULL)
		{
			pObj->SetObjectSittingDesc(m_CurrentSpell.m_strTarget);
			pObj->SetObjectDesc(m_CurrentSpell.m_strTarget);
			pObj->AddAffect(CObject::OBJ_AFFECT_MIRAGE,CalcSpellDam(pSpell),0);
		}
		else
		{
			AddCommand("bug can not find vnum for mirage!\r\n");
		}
	}
}


//////////////////////////////////
//	Wall of Stone
//	Leaves a Perm-Para'd mob in the room blocking an enterance
//	NEED TO FIX FOR DIFFERENT MOBS FOR DIRECTIONS AND ADD THE PERM PARA
//	11-26-98 John Comes
//////////////////////////////////
void CMage::Wall(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	short nDir = CRoom::StringToDirection(m_CurrentSpell.m_strTarget);
	if(nDir==NOWHERE)
	{
		SendToChar("Where do you want the wall?\r\n");
	}
	else if(nDir==UP)
	{
		SendToChar("Do you really think a wall could go there?\r\n");
	}
	else
	{
		sWallInfo WallInfo;
		WallInfo.m_nDirection = nDir;
		WallInfo.m_nLevel = this->GetLevel();
		WallInfo.m_nWallType = pSpell->GetWallType();
		WallInfo.m_nTimeToLive = m_Spells[pSpell->GetSpellID()]/7;
		CCharacter *pMob = pInterp->CreateWall(&WallInfo,GetRoom());
		CString strToChar;
		CString strToRoom = "%s";
		strToChar.Format("You create a %s to the %s!\r\n",(char *)pMob->GetName(),(char *)m_CurrentSpell.m_strTarget);
		SendToChar(strToChar);
		strToChar.Format(" creates a %s to the %s!\r\n",(char *)pMob->GetName(),(char *)m_CurrentSpell.m_strTarget);
		strToRoom += strToChar;
		GetRoom()->SendToRoom(strToRoom,this);
	}
}

///////////////////////////////
//	Air Shield
//	Similar to Stone Skin
//	Copied from Stone Skin
//	12-23-98
void CMage::AirShield(const CInterp * pInterp, const CSpell<CMage> * pSpell)
{
	m_CurrentSpell.m_pTarget->AddAffect(
		pSpell->GetSpellAffect(),CalcSpellDam(pSpell),((m_Spells[pSpell->GetSpellID()]*25)+m_nLevel)/CalcNumOfSpheres());
}

//////////////////////////
//	CounterSpell 
//	If failed save, target will stop casting
//
//	John Comes	12-23-98
///////////////////////////
void CMage::CounterSpell(const CInterp *pInterp, const CSpell<CMage> *pSpell)
{
	if(!m_CurrentSpell.m_pTarget->IsAffectedBy(CCharacter::AFFECT_CASTING))
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,m_CurrentSpell.m_pTarget);		
		return;
	}
	if((short int)DIE(150) <= CalcSpellDam(pSpell)) //SAVING THROW NEEDED
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,m_CurrentSpell.m_pTarget);		
		m_CurrentSpell.m_pTarget->RemoveAffect(CCharacter::AFFECT_CASTING);
		return;
	}
	else
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,m_CurrentSpell.m_pTarget);	
	}
}

//////////////////////////
//	Dispel Magic
//	If failed save or has consent all affects
//	Will be removed
//	John Comes	12-28-98
///////////////////////////
void CMage::DispelMagic(const CInterp *pInterp, const CSpell<CMage> *pSpell)
{
	if(m_CurrentSpell.m_pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL)
		|| m_CurrentSpell.m_pTarget->GetConsented() == this || m_CurrentSpell.m_pTarget == this) 
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,m_CurrentSpell.m_pTarget);		
		m_CurrentSpell.m_pTarget->RemoveAllAffects(true);
		return;
	}
	else
	{
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_DIDNT_WORK,this,m_CurrentSpell.m_pTarget);	
	}
}

////////////////////////////////
//  Mass Invisibilty
//  Sets Timing for Invisibilty for Room
//	John Comes 12-28-98
///////////////////////////////
void CMage::MassInvisibility(const CInterp * pInterp, const CSpell<CMage> * pSpell)
{
	short nDam = CalcSpellDam(pSpell);
	GetRoom()->AddAffectToCharacters(pSpell->GetSpellAffect(),nDam,0,true);
}

////////////////////////////////
//  SpellAffectAdd for General Spells (no reference affects)
//  Sets Timing for General Spells that add affects
//	John Comes 12-28-98
///////////////////////////////
void CMage::SpellAffectAdd(const CInterp * pInterp, const CSpell<CMage> * pSpell)
{
	switch(pSpell->GetSpellID())
	{
	case MAGE_ARMOR:
		m_CurrentSpell.m_pTarget->AddAffect(CCharacter::AFFECT_ARMOR,CalcSpellDam(pSpell),10);
		m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,m_CurrentSpell.m_pTarget);
		break;
	case MAGE_FAERIE_FIRE:
		m_CurrentSpell.m_pTarget->AddAffect(CCharacter::AFFECT_FAERIE_FIRE,CalcSpellDam(pSpell),-(m_Spells[pSpell->GetSpellID()]/5));
		AddFighters(m_CurrentSpell.m_pTarget,false);
		break;
	case MAGE_BARKSKIN:
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),20);
		break;
	case MAGE_FORESTAL_ARMOR:
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),20);
		break;
	case MAGE_STRENGTH:
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),10);
		break;
	case MAGE_DEXTERITY:
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),10);
		break;
	case MAGE_AGILITY:
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),10);
		break;
	case MAGE_AUDIBLE_GLAMOUR:
		if(m_CurrentSpell.m_pTarget->MakeSavingThrow(CAttributes::SAVE_SPELL))
		{
			m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),0);
		}
		break;
	case MAGE_STONE_SKIN:
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),((m_Spells[pSpell->GetSpellID()]*20)+m_nLevel)/CalcNumOfSpheres());
		break;
	case MAGE_PRISMATIC_SHIELD:
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),((m_Spells[pSpell->GetSpellID()]*20)+m_nLevel));
		m_CurrentSpell.m_pTarget->AddAffect(CCharacter::AFFECT_ARMOR,CalcSpellDam(pSpell),10);
		m_CurrentSpell.m_pTarget->AddAffect(CCharacter::AFFECT_AIR_CUSHION,CalcSpellDam(pSpell),((m_Spells[pSpell->GetSpellID()]*25)+m_nLevel));
	default:
		//default does not use SpellMsgs
		m_CurrentSpell.m_pTarget->AddAffect(pSpell->GetSpellAffect(),CalcSpellDam(pSpell),0);
		break;
	}
}

////////////////////////
//	Sends spell list to character
//
void CMage::SendSpellList()
{
	SendCharPage(m_strSpellList);
}

////////////////////////
//	Preserve Corpse
//	Used for Preserve and Embalm
//	John Comes: 12-29-98
//////////////////////////
void CMage::PreserveCorpse(const CInterp *pInterp, const CSpell<CMage> *pSpell)
{
	int nDot,nNum = 1;
	CString strTarget(m_CurrentSpell.m_strTarget);
	if((nDot = strTarget.Find('.'))!=-1)
	{
		nNum = strTarget.Left(nDot).MakeInt();
		if(nNum==-1)
		{
			SendToChar("Preserve how many?\r\n");
			return;
		}
		strTarget = strTarget.Right(nDot);
	}

	if(strTarget.IsEmpty())
	{
		SendToChar("Preserve what?\r\n");
	}
	CCorpse *pCon;
	if((pCon = (CCorpse *)m_pInRoom->FindObjectInRoom(strTarget,nNum,this))
		&& pCon->IsType(ITEM_TYPE_CORPSE))
	{
		int nDam = CalcSpellDam(pSpell);
		CString strToChar;
		pCon->AddTime(nDam);
		GetRoom()->SendToRoom("The &rcorpse &Wglows &nbriefly...\r\n");
		strToChar.Format("You have Preserved the corpse for %d mud hours.\r\n",(nDam / CMudTime::PULSES_PER_MUD_HOUR));
		SendToChar(strToChar);
	}
	else if(!pCon)
	{
		SendToChar("Preserve what?\r\n");
	}
	else
	{
		SendToChar("You can only preserve corpses.\r\n");
	}
}

////////////////////////////////////
//	Create Elemental
//	Creates Elemental from a 2 Elementals
//	written by: John Comes
////////////////////////////////////
void CMage::CreateElemental(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	CCharacter *pFirst,*pSecond;
	int nDot,nNum1=1, nNum2=1;
	CString strTarget1(m_CurrentSpell.m_strTarget.GetWordAfter(0));
	CString strTarget2(m_CurrentSpell.m_strTarget.GetWordAfter(1));

	if((nDot = strTarget1.Find('.'))!=-1)
	{
		nNum1 = strTarget1.Left(nDot).MakeInt();
		strTarget1 = strTarget1.Right(nDot);
	}
	if((nDot = strTarget2.Find('.'))!=-1)
	{
		nNum2 = strTarget2.Left(nDot).MakeInt();
		strTarget2 = strTarget2.Right(nDot);
	}
	if((pFirst = GetRoom()->FindCharacterByAlias(strTarget1,nNum1,this)))
	{
		if(!this->m_Master.IsMasterOf(pFirst))
		{
			SendToChar("Your not its master!\r\n");
			return;
		}
		if(pSecond = GetRoom()->FindCharacterByAlias(strTarget2,nNum2,this))
		{
			if(!this->m_Master.IsMasterOf(pSecond))
			{
				SendToChar("Your not its master!\r\n");
				return;
			}
		}
		else
		{
			SendToChar("That elemental is not here.\r\n");
		}
	}
	else
	{
		SendToChar("That elemental is not here.\r\n");
	}

	if(pFirst==NULL || pSecond==NULL)
	{
		SendToChar("You don't have the correct elementals!\r\n");
		return;
	}
	
	if(pFirst->GetVnum() != pSpell->GetFirstElementalVnum() 
		|| pSecond->GetVnum() != pSpell->GetSecondElementalVnum() 
		|| pFirst == pSecond)
	{
		if(pFirst->GetVnum() != pSpell->GetSecondElementalVnum() 
			|| pSecond->GetVnum() != pSpell->GetFirstElementalVnum())
		{
			SendToChar("You are trying to combine the wrong elementals!\r\n");
			return;
		}
	}
	
	long lElemental = pSpell->GetElementalCreatedVnum();
	short nLevel = (DIE(2)==1) ? GetLevel()+DIE(5) : GetLevel()-DIE(5);

	CCharacter *pMob = pInterp->CreateMob(lElemental,
		GetRoom(),pFirst->GetMaxHits() + pSecond->GetMaxHits(),nLevel);

	if(this->CurrentlyMasterOf()<this->CalcNumOfSpheres())
	{
		pMob->SetMaster(this,(m_Spells[pSpell->GetSpellID()]*CMudTime::PULSES_PER_MUD_HOUR));
	}
	else
	{
		CString str;
		str.Format("&r%s resists your magical hold.\r\nYou do not control %s.&n\r\n",
			(char *)pMob->GetName(),(char *)pMob->GetName());
		SendToChar(str);
	}
	pMob->SetNoCorpse();
	m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pMob);		
	
	if(lElemental == VNUM_ICE_ELEMENTAL)
	{
		pMob->AddAffect(CCharacter::AFFECT_COLD_SHIELD,sAffect::PERM_AFFECT,0);
	}
	else if(lElemental == VNUM_SLIME_ELEMENTAL)
	{
		pMob->AddAffect(CCharacter::AFFECT_BIOFEEDBACK,sAffect::PERM_AFFECT,0);
	}
	else if(lElemental == VNUM_STEAM_ELEMENTAL || lElemental == VNUM_ASH_ELEMENTAL || lElemental == VNUM_COAL_ELEMENTAL)
	{
		pMob->AddAffect(CCharacter::AFFECT_HEAT_SHIELD,sAffect::PERM_AFFECT,0);
	}
	else if(lElemental == VNUM_MIST_ELEMENTAL)
	{
		pMob->AddAffect(CCharacter::AFFECT_INVISIBLE,sAffect::MANUAL_AFFECT,0);
	}
	else if(lElemental == VNUM_ROCK_ELEMENTAL || lElemental == VNUM_TOMB_ELEMENTAL)
	{
		pMob->AddAffect(CCharacter::AFFECT_STONE_SKIN,sAffect::MANUAL_AFFECT,pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]));
		pMob->AddAffect(CCharacter::AFFECT_FISTS_OF_STONE,sAffect::PERM_AFFECT,0);
	}
	else if(lElemental == VNUM_SMOKE_ELEMENTAL)
	{
		pMob->AddAffect(CCharacter::AFFECT_AIR_CUSHION,sAffect::PERM_AFFECT,0);
	}
	pInterp->KillChar(pFirst);
	pInterp->KillChar(pSecond);
}

////////////////////////
//SendPRoficiency layout
void CMage::SendProficiency()
{
	CString strProf;
	strProf.Format("Current Proficiency Bonus: %d\r\n%s",
		m_Proficiency.CalcBonus(m_nSkillSphere),
		(const char *)m_strProficiency);
	SendCharPage(strProf);
}

////////////////////////
//	Animate Dead
//	Animates corpse to a skeleton or zombie
//	John Comes: 12-29-98
//////////////////////////
void CMage::AnimateDead(const CInterp *pInterp, const CSpell<CMage> *pSpell)
{
	int nDot,nNum = 1;
	CString strTarget(m_CurrentSpell.m_strTarget);
	if((nDot = strTarget.Find('.'))!=-1)
	{
		nNum = strTarget.Left(nDot).MakeInt();
		if(nNum==-1)
		{
			SendToChar("Animate how many?\r\n");
			return;
		}
		strTarget = strTarget.Right(nDot);
	}

	if(strTarget.IsEmpty())
	{
		SendToChar("Animate What??\r\n");
	}
	else
	{
		CCorpse *pCon;
		if((pCon = (CCorpse *)m_pInRoom->FindObjectInRoom(strTarget,nNum,this))
			&& pCon->IsType(ITEM_TYPE_CORPSE))
		{
			//calculate level limit 
			int nLevelLimit = m_Spells[pSpell->GetSpellID()]/10;
			nLevelLimit = nLevelLimit<3 ? 3 : nLevelLimit;
			if(pSpell->GetMinLevel() - pCon->GetCorpseLevel() > nLevelLimit)
			{
				SendToChar("The Corpse is not powerful enough for you to raise.\r\n");
			}
			else
			{
				sUndeadInfo UndeadInfo;
				UndeadInfo.m_nLevel = pCon->GetCorpseLevel()/CalcNumOfSpheres();
				UndeadInfo.m_nUndeadType = pSpell->GetUndeadType();;
				//If this is Animate dead type will be 0, so make a random Skeleton or Zombie
				if(UndeadInfo.m_nUndeadType==0)
				{
					if(DIE(100) < m_Spells[pSpell->GetSpellID()])
					{
						UndeadInfo.m_nUndeadType = VNUM_UNDEAD_ZOMBIE;
					}
					else
					{
						UndeadInfo.m_nUndeadType = VNUM_UNDEAD_SKELETON;
					}
				}
				
				CCharacter *pMob = pInterp->CreateUndead(&UndeadInfo,GetRoom());
				if(pMob!=NULL)
				{
					GetRoom()->SendToRoom("&L%s&L breathes &rlife&L in to a corpse...&n\r\n",this);
					pMob->AddCommand((CString) "emote looks to the sky and says, 'I'm alive!'\r\n");
					if(this->CurrentlyMasterOf()<this->CalcNumOfSpheres())
					{
						pMob->SetMaster(this,(m_Spells[pSpell->GetSpellID()]*CMudTime::PULSES_PER_MUD_HOUR));
					}
					else
					{
						CString str;
						str.Format("%r%s resists your magical hold.\r\nYou do not control %s.&n\r\n",
							(char *)pMob->GetName(),(char *)pMob->GetName());
						SendToChar(str);
					}
					CObjectManager::Remove(pCon);
				}
				else
				{
					SendToChar("You should never see this bug in create undead!\r\n");
					AddCommand("bug Bad Undead Type in mage class.");
				}
			}
		}
		else if(!pCon)
		{
			SendToChar("Animate What??\r\n");
		}
		else
		{
			SendToChar("You can only animate corpses.\r\n");
		}
	}
}

////////////////////////////////////
//	Become Lich
//	Permanently changes race to lich
//	Written by John Comes 1-12-99
//	rewritten by: Demetrius Comes
//	1-3-2000
////////////////////////////////////
void CMage::BecomeLich(const CInterp *pInterp, const CSpell<CMage> *pSpell)
{
	short nLevel = GetLevel();
	CString strToChar;
	this->PutAllEqInRoom();
	if(IsGod())
	{
		GVM.RemoveGod(this);
	}
	Save(true); //save them as if they died
	m_pAttributes->SetRace(RACE_LICH);
	m_pAttributes->ChangeRace(RACE_LICH);
	m_strAlias.Format("%s %s",(char *)m_strName,GVM.GetRaceName(RACE_LICH));
	m_nCurrentHpts = m_nMaxHpts = m_pAttributes->GetStartingHitPoints();
	m_nMovePts = m_nMaxMovePts = m_pAttributes->GetStartingMoves();
	m_nManaPts = m_nMaxManaPts = m_pAttributes->GetStartingMana();
	int nSessions = m_nPracticeSessions;
	m_nLevel=0;
	for(register int i = 1; i < nLevel;i++)
	{
		AdvanceLevel(true,false);
	}
	m_nPracticeSessions = nSessions;
	this->Save();
	strToChar = "&LYou &rhowl&L in &Rpain&L as the flesh falls from your form and the life is sucked from you.\r\n&LYou feel a &Bcold chill&L well up inside you and you are reborn.\r\n&rReborn as a powerful &LLICH!&n\r\n";
	SendToChar(strToChar);
}


/////////////////////////////////////
//	Scrye
//	Just like Clarivoyance
//
//	John Comes 1-20-99
/////////////////////////////////////
void CMage::Scrye(const CInterp *pInterp, const CSpell<CMage> *pSpell)
{
	CString strToChar, strRoomLook;
	CCharacter *pTarget = GVM.FindCharacter(m_CurrentSpell.m_strTarget,this);
	
	if(!pTarget)
	{
		SendToChar("That person is not currently playing!\r\n");
	}
	else if(pTarget->IsGod())
	{
		SendToChar("You can't do that to a god!\r\n");
	}
	else
	{
		strRoomLook = pTarget->GetRoom()->DoRoomLook(this);
		strToChar.Format("&WYou extend your sights outward....&n\r\n%s",(char *)strRoomLook);
		SendToChar(strToChar);
		pTarget->SendToChar((CString) "&LYou feel like someone is watching you...&n\r\n");
		if(pTarget->IsNPC())
		{
			pTarget->AddCommand((CString) "say Is someone watching me?\r\n");
		}
	}
}

/////////////////////////
//	Guk's Magic Portal
//
//	Portal Spell
//	John Comes 12-9-98
/////////////////////////
void CMage::GuksMagicPortal(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	CCharacter *pTarget = GVM.FindCharacter(m_CurrentSpell.m_strTarget,this);
	
	if(pTarget == NULL
		|| pTarget->IsNPC() 
		|| !CanSeeChar(pTarget) 
		|| CanSeeInRoom(pTarget->GetRoom())!=CCharacter::SEES_ALL
		|| pTarget->IsGod())
	{
		SendToChar("That person is not currently playing!\r\n");
	}
	else
	{
		pInterp->CreatePortal(pTarget->GetRoom(),this->GetRoom(),CPortal::GUKS_MAGIC_PORTAL,CalcSpellDam(pSpell));
		pTarget->GetRoom()->SendToRoom("&WIn a &Ys&Gw&Ri&Mr&Gl &Wof color a &RMagic &YPortal &Wappears!&n\r\n");
		GetRoom()->SendToRoom("&WIn a &Ys&Gw&Ri&Mr&Gl &Wof color a &RMagic &YPortal &Wappears!&n&n\r\n");
  }
}


////////////////////////////////
//  Guks Glowing Globes
//  Sets Timing for GGG
//	John Comes 1-21-99
///////////////////////////////
void CMage::GuksGlowingGlobes(const CInterp * pInterp, const CSpell<CMage> * pSpell)
{
//affect,pulses,value
	int nDam = CalcSpellDam(pSpell);
	AddAffect(CCharacter::AFFECT_GUKS_GLOWING_GLOBES,sAffect::MANUAL_AFFECT,nDam);
	CString strToChar;
	strToChar.Format("&R%d globes start spinning around you!&n\r\n",nDam);
	SendToChar(strToChar);
}

/////////////////////
//	Magic Missile
//
void CMage::MagicMissile(const CInterp * pInterp, const CSpell<CMage> *pSpell)
{
	short nMissiles = 5;
	if(GetLevel() <= 16)
	{
		nMissiles = GetLevel() / 4 + 1;
	}
	for(register int i = 0;i < nMissiles;i++)
	{
		OffensiveSpell(pInterp,pSpell);
	}
}

/////////////////////////////////////
//	Delayed Blast Fireball
//
//	John Comes : 2-11-99
//

void CMage::DelayedBlastFireball(const CInterp * pInterp, const CSpell<CMage> *pSpell)
{
	int nPulses = (m_CurrentSpell.m_strTarget.MakeInt()) * CMudTime::PULSES_PER_REAL_SECOND;
	pInterp->CreateFireball(this->GetRoom(),nPulses,CalcSpellDam(pSpell));
	GetRoom()->SendToRoom("&RA small smouldering fireball forms in the room...&n\r\n");
}

const CSpell<CMage> *CMage::GetSpellInfo(CString strSpell)
{
	CSpell<CMage> *pSpell;
	if(m_SpellTable.Lookup(strSpell,pSpell))
	{
		return pSpell;
	}
	return NULL;
}

//////////////////////////////////
//	gives back the pct for a spell up
short CMage::GetPctSpellUp(const CSpell <CMage> * pSpell)
{
	return m_pAttributes->GetInt()/5;
}

////////////////////////////////////
//	For mages they can practice any spell
//	as long as it isn't a gray spell...
//  they must have all other spheres first
void CMage::DoPractice(CString strToPractice,const CCharacter *pTeacher)
{
	const CSpell<CMage> *pSpell;
	CString strToChar;
	if((pSpell = this->GetSpellInfo(strToPractice)))
	{
		if(this->GetRace()==RACE_LICH && !pSpell->IsOfSphere(SPHERE_SWAMP))
		{
			SendToChar("Liches may only learn swamp sphere.\r\n");
		}
		else if(!pSpell->IsOfSphere(SPHERE_GRAY) || IsGod() || 
			(pSpell->IsOfSphere(SPHERE_GRAY) && IsOfSphere(SPHERE_GRAY)))
		{
			CCaster<CMage>::DoPractice(strToPractice,pTeacher);
		}
		else
		{
			SendToChar("Only mages of the gray sphere can learn that spell!\r\n");
		}
	}
	else
	{
		CCharacter::DoPractice(strToPractice, pTeacher);
	}
}



////////////////////////////////////
//	Create Magic Elemental
//	Creates Magic Elemental from a 4 Elementals
//	written by: John Comes  7/19/99
////////////////////////////////////
void CMage::CreateMagicElemental(const CInterp *pInterp,const CSpell<CMage> *pSpell)
{
	CCharacter *pFirst,*pSecond,*pThird,*pForth;
	int nDot,nNum1 = 1, nNum2 = 1, nNum3 = 1, nNum4 = 1;
	long nElementals[14][2];
	nElementals[0][0] = VNUM_PYRE_ELEMENTAL ;
	nElementals[1][0] = VNUM_COAL_ELEMENTAL ;
	nElementals[2][0] = VNUM_ASH_ELEMENTAL ;
	nElementals[3][0] = VNUM_STEAM_ELEMENTAL ;
	nElementals[4][0] = VNUM_PLASMA_ELEMENTAL ;
	nElementals[5][0] = VNUM_MIST_ELEMENTAL ;
	nElementals[6][0] = VNUM_SMOKE_ELEMENTAL ;
	nElementals[7][0] = VNUM_ICE_ELEMENTAL ;
	nElementals[8][0] = VNUM_SLIME_ELEMENTAL ;
	nElementals[9][0] = VNUM_ROCK_ELEMENTAL ;
	nElementals[10][0] = VNUM_TOMB_ELEMENTAL ;
	nElementals[11][0] = VNUM_MUD_ELEMENTAL ;
	nElementals[12][0] = VNUM_GORE_ELEMENTAL ;
	nElementals[13][0] = VNUM_ICE_ELEMENTAL ;

	register int i;
	for(i = 0;i <= 13;i++) nElementals[i][1] = 0;
	CString strTarget1(m_CurrentSpell.m_strTarget.GetWordAfter(0));
	CString strTarget2(m_CurrentSpell.m_strTarget.GetWordAfter(1));
	CString strTarget3(m_CurrentSpell.m_strTarget.GetWordAfter(2));
	CString strTarget4(m_CurrentSpell.m_strTarget.GetWordAfter(3));
	
	//TODO write this much more cleanly

	if((nDot = strTarget1.Find('.'))!=-1)
	{
		nNum1 = strTarget1.Left(nDot).MakeInt();
		strTarget1 = strTarget1.Right(nDot);
	}
	if((nDot = strTarget2.Find('.'))!=-1)
	{
		nNum2 = strTarget2.Left(nDot).MakeInt();
		strTarget2 = strTarget2.Right(nDot);
	}
	if((nDot = strTarget3.Find('.'))!=-1)
	{
		nNum3 = strTarget3.Left(nDot).MakeInt();
		strTarget3 = strTarget3.Right(nDot);
	}
	if((nDot = strTarget4.Find('.'))!=-1)
	{
		nNum4 = strTarget4.Left(nDot).MakeInt();
		strTarget4 = strTarget4.Right(nDot);
	}
	if((pFirst = GetRoom()->FindCharacterByAlias(strTarget1,nNum1,this)))
	{
		if(!this->m_Master.IsMasterOf(pFirst))
		{
			SendToChar("Your not its master!\r\n");
			return;
		}
		if(pSecond = GetRoom()->FindCharacterByAlias(strTarget2,nNum2,this))
		{
			if(!this->m_Master.IsMasterOf(pSecond))
			{
				SendToChar("Your not its master!\r\n");
				return;
			}
			if(pThird = GetRoom()->FindCharacterByAlias(strTarget3,nNum3,this))
			{
				if(!this->m_Master.IsMasterOf(pThird))
				{
					SendToChar("Your not its master!\r\n");
					return;
				}
				if(pForth = GetRoom()->FindCharacterByAlias(strTarget4,nNum4,this))
				{
					if(!this->m_Master.IsMasterOf(pForth))
					{
						SendToChar("Your not its master!\r\n");
						return;
					}
				}
				else
				{
					SendToChar("That elemental is not here.\r\n");
					return;
				}
			}
			else
			{
				SendToChar("That elemental is not here.\r\n");
				return;
			}
		}
		else
		{
			SendToChar("That elemental is not here.\r\n");
			return;
		}
	}
	else
	{
		SendToChar("That elemental is not here.\r\n");
		return;
	}
	for(i = 0; i <= 13;i++)
	{
		if(pFirst->GetVnum() == nElementals[i][0] || 
			pSecond->GetVnum() == nElementals[i][0] ||
			pThird->GetVnum() == nElementals[i][0] ||
			pForth->GetVnum() == nElementals[i][0])
		{
			nElementals[i][1] = 1;
		}
	}
	short nNumElementals = 0;
	for(i = 0;i <= 13;i++) 
	{
		if(nElementals[i][1]) nNumElementals++;
	}
	if(pFirst==NULL || pSecond==NULL || pThird==NULL || pForth==NULL || nNumElementals != 4)
	{
		SendToChar("You don't have the correct elementals!\r\n");
		return;
	}
	short nLevel = (DIE(2)==1) ? GetLevel()+DIE(5) : GetLevel()-DIE(5);
	CCharacter *pMob = pInterp->CreateMob(VNUM_MAGIC_ELEMENTAL,
		GetRoom(),pFirst->GetMaxHits() + pSecond->GetMaxHits() + pThird->GetMaxHits() + pForth->GetMaxHits(),
		nLevel);

	pMob->SetMaster(this,(m_Spells[pSpell->GetSpellID()]*CMudTime::PULSES_PER_MUD_HOUR));
	pMob->SetNoCorpse();
	m_MsgManager.SpellMsg(pSpell->GetSpellName(),SKILL_WORKED,this,pMob);		
	pMob->AddAffect(CCharacter::AFFECT_COLD_SHIELD,sAffect::PERM_AFFECT,0);
	pMob->AddAffect(CCharacter::AFFECT_INVISIBLE,sAffect::MANUAL_AFFECT,0);
	pMob->AddAffect(CCharacter::AFFECT_STONE_SKIN,sAffect::MANUAL_AFFECT,pSpell->GetDamage(m_Spells[pSpell->GetSpellID()]));
	pMob->AddAffect(CCharacter::AFFECT_FISTS_OF_STONE,sAffect::PERM_AFFECT,0);
	pMob->AddAffect(CCharacter::AFFECT_AIR_CUSHION,sAffect::PERM_AFFECT,0);
	pInterp->KillChar(pFirst);
	pInterp->KillChar(pSecond);
	pInterp->KillChar(pThird);
	pInterp->KillChar(pForth);
}

//////////////////////////////////
//	BuildSpellList
//	Only called once to static init m_strSpellLIst
void CMage::BuildSpellList()
{
	POSITION pos = m_SpellTable.GetStartingPosition();
	bool bReturn = false;
	CSpell<CMage> *pCurrentSpell;
	while(pos)
	{
		pCurrentSpell = m_SpellTable.GetNext(pos);
		m_strSpellList.Format("%s  (%-2d) %-25s",
			(char *)m_strSpellList,
			pCurrentSpell->GetMinLevel(),
			(char *)pCurrentSpell->GetColorizedName());
		if(bReturn)
		{
			bReturn = false;
			m_strSpellList+="\r\n";
		}
		else
		{
			bReturn = true;
		}
	}
	m_strSpellList+="\r\n";
}

////////////////////////////
//	Add to hash table what can be 
//  minor created.  Called once to 
//	init m_CanMinorCreate
void CMage::BuildWhatCanBeMinorCreated()
{
						//obj name   vnum
	m_CanMinorCreate.Add("bag", 902);
	m_CanMinorCreate.Add("club",903);
	m_CanMinorCreate.Add("tree",904);
	m_CanMinorCreate.Add("pants",905);
	m_CanMinorCreate.Add("boots",906);
	m_CanMinorCreate.Add("gloves",907);
	m_CanMinorCreate.Add("bp",908);
	m_CanMinorCreate.Add("backpack",908);
	m_CanMinorCreate.Add("dagger",910);
}

///////////////////////////
//	Inits multi spheres spells
void CMage::InitMultiSphereSpells()
{
	// MULTICOLOR
	m_SpellTable.Add("Meteorswarm",new CSpell<CMage>("Meteorswarm","&YMeteor&Rswarm&n",MAGE_METEORSWARM,SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,
	 5,10,100,100,8,10,46,&CMage::OffensiveSpell,SPHERE_MOUNTAIN | SPHERE_DESERT));
	m_SpellTable.Add("Create Ice Elemental",new CSpell<CMage>("Create Ice Elemental","&BCreate &WIce &BElemental&n",MAGE_CREATE_ICE_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_AIR_ELEMENTAL,VNUM_WATER_ELEMENTAL,VNUM_ICE_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_OCEAN | SPHERE_TUNDRA));
	m_SpellTable.Add("Create Slime Elemental",new CSpell<CMage>("Create Slime Elemental","&BCreate &WSlime &BElemental&n",MAGE_CREATE_SLIME_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_FOG_ELEMENTAL,VNUM_BLOOD_ELEMENTAL,VNUM_SLIME_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_OCEAN | SPHERE_TUNDRA));
	m_SpellTable.Add("Create Rock Elemental",new CSpell<CMage>("Create Rock Elemental","&WCreate &YRock &WElemental&n",MAGE_CREATE_ROCK_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_AIR_ELEMENTAL,VNUM_EARTH_ELEMENTAL,VNUM_ROCK_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_DESERT | SPHERE_TUNDRA));
	m_SpellTable.Add("Create Tomb Elemental",new CSpell<CMage>("Create Tomb Elemental","&WCreate &YTomb &WElemental&n",MAGE_CREATE_TOMB_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_FOG_ELEMENTAL,VNUM_GRAVE_ELEMENTAL,VNUM_TOMB_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_DESERT | SPHERE_TUNDRA));
	m_SpellTable.Add("Create Coal Elemental",new CSpell<CMage>("Create Coal Elemental","&RCreate &YCoal &RElemental&n",MAGE_CREATE_COAL_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_EARTH_ELEMENTAL,VNUM_FIRE_ELEMENTAL,VNUM_COAL_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_DESERT | SPHERE_MOUNTAIN));
	m_SpellTable.Add("Create Ash Elemental",new CSpell<CMage>("Create Ash Elemental","&RCreate &YAsh &RElemental&n",MAGE_CREATE_ASH_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_GRAVE_ELEMENTAL,VNUM_PYRE_ELEMENTAL,VNUM_ASH_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_DESERT | SPHERE_MOUNTAIN));
	m_SpellTable.Add("Create Steam Elemental",new CSpell<CMage>("Create Steam Elemental","&RCreate &BSteam &RElemental&n",MAGE_CREATE_STEAM_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_WATER_ELEMENTAL,VNUM_FIRE_ELEMENTAL,VNUM_STEAM_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_OCEAN | SPHERE_MOUNTAIN));
	m_SpellTable.Add("Create Plasma Elemental",new CSpell<CMage>("Create Plasma Elemental","&RCreate &BPlasma &RElemental&n",MAGE_CREATE_PLASMA_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_BLOOD_ELEMENTAL,VNUM_PYRE_ELEMENTAL,VNUM_PLASMA_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_OCEAN | SPHERE_MOUNTAIN));
	m_SpellTable.Add("Create Mist Elemental",new CSpell<CMage>("Create Mist Elemental","&RCreate &WMist &RElemental&n",MAGE_CREATE_MIST_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_AIR_ELEMENTAL,VNUM_FIRE_ELEMENTAL,VNUM_MIST_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_TUNDRA | SPHERE_MOUNTAIN));
	m_SpellTable.Add("Create Smoke Elemental",new CSpell<CMage>("Create Smoke Elemental","&RCreate &WSmoke &RElemental&n",MAGE_CREATE_SMOKE_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_FOG_ELEMENTAL,VNUM_PYRE_ELEMENTAL,VNUM_SMOKE_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_TUNDRA | SPHERE_MOUNTAIN));
	m_SpellTable.Add("Create Mud Elemental",new CSpell<CMage>("Create Mud Elemental","&YCreate &BMud &YElemental&n",MAGE_CREATE_MUD_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_EARTH_ELEMENTAL,VNUM_WATER_ELEMENTAL,VNUM_MUD_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_DESERT | SPHERE_OCEAN));
	m_SpellTable.Add("Create Gore Elemental",new CSpell<CMage>("Create Gore Elemental","&YCreate &BGore &YElemental&n",MAGE_CREATE_GORE_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,VNUM_GRAVE_ELEMENTAL,VNUM_BLOOD_ELEMENTAL,VNUM_GORE_ELEMENTAL,10,9,41,&CMage::CreateElemental,SPHERE_DESERT | SPHERE_OCEAN));
	m_SpellTable.Add("Fire and Ice",new CSpell<CMage>("Fire and Ice","&RFire &wand &WIce&n",MAGE_FIRE_AND_ICE,SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,
	 5,10,100,100,8,10,46,&CMage::OffensiveSpell,SPHERE_MOUNTAIN | SPHERE_TUNDRA));
}

////////////////////////
//	 INits mageling spells
void CMage::InitMagelingSpells()
{
	// MAGELING
	m_SpellTable.Add("Magic Missile",new CSpell<CMage>("Magic Missile","Magic Missile",MAGE_MAGICMISSILE,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		1,2,4,0,28,1,1,&CMage::MagicMissile,0));
	m_SpellTable.Add("Minor Creation",new CSpell<CMage>("Minor Creation","Minor creation",MAGE_MINOR_CREATION,SPELL_NO_AFFECT,AREA_NONE,
		1,0,0,0,28,1,1,&CMage::MinorCreation,0));
	m_SpellTable.Add("Acid Missiles",new CSpell<CMage>("Acid Missiles","Acid Missiles",MAGE_ACID_MISSILE,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		2,2,4,2,25,2,6,&CMage::MagicMissile,0));
	m_SpellTable.Add("Exploding Spheres",new CSpell<CMage>("Exploding Spheres","Exploding Spheres",MAGE_EXPLODING_SPHERES,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		2,3,5,10,23,3,11,&CMage::OffensiveSpell,0));
	m_SpellTable.Add("Cantrip",new CSpell<CMage>("Cantrip","Cantrip",MAGE_CANTRIP,SPELL_NO_AFFECT,AREA_NONE,
		1,0,0,0,28,1,1,&CMage::Cantrip,0));
    m_SpellTable.Add("Find Familiar",new CSpell<CMage>("Find Familiar","Find Familiar",MAGE_FIND_FAMILIAR,SPELL_NO_AFFECT,AREA_NONE,
		6,0,0,0,28,1,1,&CMage::FindFamiliar,0));
	m_SpellTable.Add("Fists of Stone",new CSpell<CMage>("Fists of Stone","Fists Of Stone",MAGE_FISTS_OF_STONE,CCharacter::AFFECT_FISTS_OF_STONE,AREA_SELF,
		4,2,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),25,2,6,&CMage::SpellAffectAdd,0));
	m_SpellTable.Add("Detect Magic",new CSpell<CMage>("Detect Magic","Detect Magic",MAGE_DETECT_MAGIC,CCharacter::AFFECT_DETECT_MAGIC,AREA_TARGET | SPELL_DEFENSIVE,
		4,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),25,2,6,&CMage::SpellAffectAdd,0));
	m_SpellTable.Add("Dispel Magic",new CSpell<CMage>("Dispel Magic","Dispel Magic",MAGE_DISPEL_MAGIC,SPELL_NO_AFFECT,AREA_TARGET,
		4,8,10,20,23,3,11,&CMage::DispelMagic,0));
	m_SpellTable.Add("Audible Glamour",new CSpell<CMage>("Audible Glamour","Audible Glamour",MAGE_AUDIBLE_GLAMOUR,CCharacter::AFFECT_AUDIBLE_GLAMOUR,AREA_TARGET,
		4,2,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),25,2,6,&CMage::SpellAffectAdd,0));
	m_SpellTable.Add("Armor",new CSpell<CMage>("Armor","Armor",MAGE_ARMOR,CCharacter::AFFECT_ARMOR,AREA_TARGET | SPELL_DEFENSIVE,
		4,CMudTime::PULSES_PER_MUD_HOUR,30,(CMudTime::PULSES_PER_MUD_HOUR>>1),25,2,6,&CMage::SpellAffectAdd,0));
	m_SpellTable.Add("Dexterity",new CSpell<CMage>("Dexterity","Dexterity",MAGE_DEXTERITY,CCharacter::AFFECT_DEXTERITY,AREA_TARGET,
		6,CMudTime::PULSES_PER_MUD_HOUR,60,(CMudTime::PULSES_PER_MUD_HOUR>>1),26,3,11,&CMage::SpellAffectAdd,0));
	m_SpellTable.Add("Strength",new CSpell<CMage>("Strength","Strength",MAGE_STRENGTH,CCharacter::AFFECT_STRENGTH,AREA_TARGET,
		6,CMudTime::PULSES_PER_MUD_HOUR,60,(CMudTime::PULSES_PER_MUD_HOUR>>1),26,3,11,&CMage::SpellAffectAdd,0));
	m_SpellTable.Add("Agility",new CSpell<CMage>("Agility","Agility",MAGE_AGILITY,CCharacter::AFFECT_AGILITY,AREA_TARGET,
		6,CMudTime::PULSES_PER_MUD_HOUR,60,(CMudTime::PULSES_PER_MUD_HOUR>>1),26,3,11,&CMage::SpellAffectAdd,0));
}

////////////////////////
//	Inits Ocean sphere spells
//	init's static variable m_SpellTable
void CMage::InitOceanSphereSpells()
{
	// OCEAN
	m_SpellTable.Add("Scalding Blast",new CSpell<CMage>("Scalding Blast","&BScalding Blast&n",MAGE_SCALDINGBLAST,
		SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,3,4,4,20,27,4,16,&CMage::OffensiveSpell,SPHERE_OCEAN));
	m_SpellTable.Add("Salt Spray",new CSpell<CMage>("Salt Spray","&BSalt Spray&n",MAGE_SALTSPRAY,
		SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,4,5,7,20,20,6,26,&CMage::OffensiveSpell,SPHERE_OCEAN));
	m_SpellTable.Add("Wicked Rain",new CSpell<CMage>("Wicked Rain","&BWicked Rain&n",MAGE_WICKEDRAIN,
		SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,4,5,10,60,15,7,31,&CMage::OffensiveSpell,SPHERE_OCEAN));
	m_SpellTable.Add("Hail Storm",new CSpell<CMage>("Hail Storm","&BHail Storm&n",MAGE_HAILSTORM,
		SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,5,5,10,60,10,8,36,&CMage::OffensiveSpell,SPHERE_OCEAN));
	m_SpellTable.Add("Water Bash",new CSpell<CMage>("Water Bash","&BWater Bash&n",MAGE_WATERBASH,
		SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,6,5,50,100,10,9,41,&CMage::OffensiveSpell,SPHERE_OCEAN));
	m_SpellTable.Add("Mist Form", new CSpell<CMage>("Mist Form","&BMist Form&n",MAGE_MIST_FORM,CCharacter::AFFECT_MIST_FORM,AREA_TARGET,
		5,15,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),18,7,31,&CMage::SpellAffectAdd,SPHERE_OCEAN));
	m_SpellTable.Add("Enshroud Mist", new CSpell<CMage>("Enshroud Mist","&BEnshroud Mist&n",MAGE_ENSHROUD_MIST,CCharacter::AFFECT_MIST_FORM,AREA_ROOM | SPELL_DEFENSIVE,
		5,15,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),10,9,41,&CMage::MassInvisibility,SPHERE_OCEAN));
	m_SpellTable.Add("Ether Travel",new CSpell<CMage>("Ether Travel","&BEther Travel&n",MAGE_ETHER_TRAVEL,SPELL_NO_AFFECT,AREA_NONE,
		6,1,MAX_RELOCATE_LAG,0,8,10,46,&CMage::Relocate,SPHERE_OCEAN));
	m_SpellTable.Add("Mist Walk",new CSpell<CMage>("Mist Walk","&BMist Walk&n",MAGE_MIST_WALK,CCharacter::AFFECT_MIST_WALK,AREA_TARGET,
		6,40,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),10,9,41,&CMage::SpellAffectAdd,SPHERE_OCEAN));
	m_SpellTable.Add("Mist Shield", new CSpell<CMage>("Mist Shield","&BMist Shield&n",MAGE_MIST_SHIELD,CCharacter::AFFECT_MIST_SHIELD,AREA_TARGET | SPELL_DEFENSIVE,
		5,5,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),20,6,26,&CMage::SpellAffectAdd,SPHERE_OCEAN));
	m_SpellTable.Add("Mist Barrier", new CSpell<CMage>("Mist Barrier","&BMist Barrier&n",MAGE_MIST_BARRIER,CCharacter::AFFECT_MIST_BARRIER,AREA_TARGET,
		5,2,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),18,7,31,&CMage::SpellAffectAdd,SPHERE_OCEAN));
    m_SpellTable.Add("Conjure Water Elemental",new CSpell<CMage>("Conjure Water Elemental","&BConjure Water Elemental&n",MAGE_CONJURE_WATER_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		8,100,16,400,23,5,21,&CMage::ConjureElemental,SPHERE_OCEAN));
	m_SpellTable.Add("Conjure Blood Elemental",new CSpell<CMage>("Conjure Blood Elemental","&BConjure Blood Elemental&n",MAGE_CONJURE_BLOOD_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		8,100,16,400,23,5,21,&CMage::ConjureElemental,SPHERE_OCEAN));
	m_SpellTable.Add("Wall of Water",new CSpell<CMage>("Wall of Water","&BWall of Water&n",MAGE_WALL_OF_WATER,SPELL_NO_AFFECT,AREA_NONE,
		8,0,0,VNUM_WALL_OF_WATER,20,5,21,&CMage::Wall,SPHERE_OCEAN));
}

////////////////////////
//	INits desert sphere spells
void CMage::InitDesertSphereSpells()
{
	// DESERT
	m_SpellTable.Add("Sand Pillar",new CSpell<CMage>("Sand Pillar","&YSand Pillar&n",MAGE_SANDPILLAR,SPELL_NO_AFFECT, AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		4,2,50,75,20,7,31,&CMage::OffensiveSpell,SPHERE_DESERT));
	m_SpellTable.Add("Rock Swarm",new CSpell<CMage>("Rock Swarm","&YRock Swarm&n",MAGE_ROCKSWARM,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		5,3,50,75,15,8,36,&CMage::OffensiveSpell,SPHERE_DESERT));
	m_SpellTable.Add("Sand Storm",new CSpell<CMage>("Sand Storm","&YSand Storm&n",MAGE_SANDSTORM,SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		5,6,70,75,10,9,41,&CMage::OffensiveSpell,SPHERE_DESERT));
	m_SpellTable.Add("Pebble Swarm",new CSpell<CMage>("Pebble Swarm","&YPebble Swarm&n",MAGE_PEBBLESWARM,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		4,4,10,20,25,4,16,&CMage::OffensiveSpell,SPHERE_DESERT));
	m_SpellTable.Add("Stone Skin", new CSpell<CMage>("Stone Skin","&YStone Skin&n",MAGE_STONE_SKIN,CCharacter::AFFECT_STONE_SKIN,AREA_TARGET | SPELL_DEFENSIVE,
		4,3,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),20,6,26,&CMage::SpellAffectAdd,SPHERE_DESERT));
	m_SpellTable.Add("Heat Shield",new CSpell<CMage>("Heat Shield","&YHeat Shield&n",MAGE_HEAT_SHIELD,CCharacter::AFFECT_HEAT_SHIELD,AREA_SELF | SPELL_DEFENSIVE,
		4,2,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),20,5,21,&CMage::SpellAffectAdd,SPHERE_DESERT));
	//this adds affect blindness so we get max of 8/10 of a minute max blind not as good as clerics
	m_SpellTable.Add("Sand Blast",new CSpell<CMage>("Sand Blast","&YSand Blast&n",MAGE_SAND_BLAST,CCharacter::AFFECT_BLINDNESS,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		4,(CMudTime::PULSES_PER_MUD_HOUR*8/10),1,0,20,5,21,&CMage::SandBlast,SPHERE_DESERT));
   	m_SpellTable.Add("Desert Portal",new CSpell<CMage>("Desert Portal","&YDesert Portal&n",MAGE_DESERT_PORTAL,SPELL_NO_AFFECT,AREA_NONE,
		6,1,MAX_RELOCATE_LAG,0,8,10,46,&CMage::Relocate,SPHERE_DESERT));
	m_SpellTable.Add("Rock to Mud",new CSpell<CMage>("Rock to Mud","&YRock to Mud&n",MAGE_ROCK_TO_MUD,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE,
		4,4,5,0,20,5,21,&CMage::RockToMud,SPHERE_DESERT));
	m_SpellTable.Add("Mirage",new CSpell<CMage>("Mirage","&YMirage&n",MAGE_MIRAGE,SPELL_NO_AFFECT,AREA_NONE,
		8,20,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),20,5,21,&CMage::Mirage,SPHERE_DESERT));
	m_SpellTable.Add("Wall of Stone",new CSpell<CMage>("Wall of Stone","&YWall of Stone&n",MAGE_WALL_OF_STONE,SPELL_NO_AFFECT,AREA_NONE,
		8,0,0,VNUM_WALL_OF_STONE,18,6,26,&CMage::Wall,SPHERE_DESERT));
	m_SpellTable.Add("Vulture Flight",new CSpell<CMage>("Vulture Flight","&YVulture Flight&n",MAGE_VULTURE_FLIGHT,CCharacter::AFFECT_VULTURE_FLIGHT,AREA_TARGET,
		4,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),10,9,41,&CMage::SpellAffectAdd,SPHERE_DESERT));
    m_SpellTable.Add("Conjure Earth Elemental",new CSpell<CMage>("Conjure Earth Elemental","&YConjure Earth Elemental&n",MAGE_CONJURE_EARTH_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		8,100,16,400,23,5,21,&CMage::ConjureElemental,SPHERE_DESERT));
	m_SpellTable.Add("Conjure Grave Elemental",new CSpell<CMage>("Conjure Grave Elemental","&YConjure Grave Elemental&n",MAGE_CONJURE_GRAVE_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		8,100,16,400,23,5,21,&CMage::ConjureElemental,SPHERE_DESERT));
	m_SpellTable.Add("Wall of Sand",new CSpell<CMage>("Wall of Sand","&YWall of Sand&n",MAGE_WALL_OF_SAND,SPELL_NO_AFFECT,AREA_NONE,
		8,0,0,VNUM_WALL_OF_SAND,15,7,31,&CMage::Wall,SPHERE_DESERT));
	m_SpellTable.Add("Wall of Glass",new CSpell<CMage>("Wall of Glass","&YWall of Glass&n",MAGE_WALL_OF_GLASS,SPELL_NO_AFFECT,AREA_NONE,
		8,0,0,VNUM_WALL_OF_GLASS,13,8,36,&CMage::Wall,SPHERE_DESERT));
}

//////////////////////////////
//	INits Tundra sphere spells
void CMage::InitTundraSphereSpells()
{
	// TUNDRA
	m_SpellTable.Add("Duths Snowball Swarm",new CSpell<CMage>("Duths Snowball Swarm","&WDuths Snowball Swarm&n",MAGE_DUTHSSNOWBALLSWARM,SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		4,2,50,75,18,8,36,&CMage::OffensiveSpell,SPHERE_TUNDRA));
	m_SpellTable.Add("Duths Snowball",new CSpell<CMage>("Duths Snowball","&WDuths Snowball&n",MAGE_DUTHSSNOWBALL,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		4,4,10,20,25,4,16,&CMage::OffensiveSpell,SPHERE_TUNDRA));
	m_SpellTable.Add("Duths Snow Storm",new CSpell<CMage>("Duths Snow Storm","&WDuths Snow Storm&n",MAGE_DUTHSSNOWSTORM,SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		4,2,30,10,18,6,26,&CMage::OffensiveSpell,SPHERE_TUNDRA));
	m_SpellTable.Add("Cold Shield",new CSpell<CMage>("Cold Shield","&WCold Shield&n",MAGE_COLD_SHIELD,CCharacter::AFFECT_COLD_SHIELD,AREA_SELF | SPELL_DEFENSIVE,
		4,2,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),20,5,21,&CMage::SpellAffectAdd,SPHERE_TUNDRA));
	m_SpellTable.Add("Airy Portal",new CSpell<CMage>("Airy Portal","&WAiry Portal&n",MAGE_AIRY_PORTAL,SPELL_NO_AFFECT,AREA_NONE,
		6,1,MAX_RELOCATE_LAG,0,8,10,46,&CMage::Relocate,SPHERE_TUNDRA));
	m_SpellTable.Add("Air Shield", new CSpell<CMage>("Air Shield","&WAir Shield&n",MAGE_AIR_SHIELD,CCharacter::AFFECT_AIR_SHIELD,AREA_TARGET | SPELL_DEFENSIVE,
		3,6,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),20,6,26,&CMage::AirShield,SPHERE_TUNDRA));
	m_SpellTable.Add("Air Cushion", new CSpell<CMage>("Air Cushion","&WAir Cusion&n",MAGE_AIR_CUSHION,CCharacter::AFFECT_AIR_CUSHION,AREA_TARGET | SPELL_DEFENSIVE,
		5,10,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),20,6,26,&CMage::SpellAffectAdd,SPHERE_TUNDRA));
	m_SpellTable.Add("Counter Spell", new CSpell<CMage>("Counter Spell","&WCounter Spell&n",MAGE_COUNTER_SPELL,SPELL_NO_AFFECT,AREA_TARGET,
		1,3,25,0,25,5,21,&CMage::CounterSpell,SPHERE_TUNDRA));
	m_SpellTable.Add("Detect Invisibility", new CSpell<CMage>("Detect Invisibility","&WDetect Invisibility&n",MAGE_DETECT_INVISIBILITY,CCharacter::AFFECT_DI,AREA_TARGET | SPELL_DEFENSIVE,
		5,20,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),15,8,36,&CMage::SpellAffectAdd,SPHERE_TUNDRA));
	m_SpellTable.Add("Invisibility", new CSpell<CMage>("Invisibility","&WInvisibility&n",MAGE_INVISIBILITY,CCharacter::AFFECT_INVISIBILITY,AREA_TARGET | SPELL_DEFENSIVE,
		5,20,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),18,7,31,&CMage::SpellAffectAdd,SPHERE_TUNDRA));
	m_SpellTable.Add("Mass Invisibility", new CSpell<CMage>("Mass Invisibility","&WMass Invisibility&n",MAGE_MASS_INVISIBILITY,CCharacter::AFFECT_INVISIBILITY,AREA_ROOM,
		5,20,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),10,9,41,&CMage::MassInvisibility,SPHERE_TUNDRA));
    m_SpellTable.Add("Conjure Air Elemental",new CSpell<CMage>("Conjure Air Elemental","&WConjure Air elemental&n",MAGE_CONJURE_AIR_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		8,100,16,400,23,5,21,&CMage::ConjureElemental,SPHERE_TUNDRA));
	m_SpellTable.Add("Conjure Fog Elemental",new CSpell<CMage>("Conjure Fog Elemental","&WConjure Fog elemental&n",MAGE_CONJURE_FOG_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		8,100,16,400,23,5,21,&CMage::ConjureElemental,SPHERE_TUNDRA));
	m_SpellTable.Add("Wall of Air",new CSpell<CMage>("Wall of Air","&WWall of Air&n",MAGE_WALL_OF_AIR,SPELL_NO_AFFECT,AREA_NONE,
		8,0,0,VNUM_WALL_OF_AIR,15,7,31,&CMage::Wall,SPHERE_TUNDRA));
	m_SpellTable.Add("Wall of Ice",new CSpell<CMage>("Wall of Ice","&WWall of Ice&n",MAGE_WALL_OF_ICE,SPELL_NO_AFFECT,AREA_NONE,
		8,0,0,VNUM_WALL_OF_ICE,10,9,41,&CMage::Wall,SPHERE_TUNDRA));
}

///////////////////////
//	inits forest sphere spells
void CMage::InitForestSphereSpells()
{
	// FOREST
	m_SpellTable.Add("Pine Needles",new CSpell<CMage>("Pine Needles","&GPine Needles&n",MAGE_PINENEEDLES,
		SPELL_NO_AFFECT,AREA_SELF | AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,2,20,20,50,15,7,31,&CMage::OffensiveSpell,SPHERE_FOREST));
	m_SpellTable.Add("Hurricane",new CSpell<CMage>("Hurricane","&GHurricane&n",MAGE_HURRICANE,SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE,
		4,5,100,100,8,10,46,&CMage::OffensiveSpell,SPHERE_FOREST));
	m_SpellTable.Add("Summon Swarm",new CSpell<CMage>("Summon Swarm","&GSummon Swarm&n",MAGE_SUMMON_SWARM,SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		6,4,5,16,8,10,46,&CMage::SummonSwarm,SPHERE_FOREST));
	m_SpellTable.Add("Camouflage",new CSpell<CMage>("Camouflage","&GCamouflage&n",MAGE_CAMOUFLAGE,CCharacter::AFFECT_CAMOUFLAGE,AREA_TARGET,
		5,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),15,8,36,&CMage::SpellAffectAdd,SPHERE_FOREST));
	m_SpellTable.Add("Barkskin",new CSpell<CMage>("Barkskin","&GBarkskin&n",MAGE_BARKSKIN,CCharacter::AFFECT_BARKSKIN,AREA_TARGET | SPELL_DEFENSIVE,
		5,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),20,6,26,&CMage::SpellAffectAdd,SPHERE_FOREST));
	m_SpellTable.Add("Wall of Brambles",new CSpell<CMage>("Wall of Brambles","&GWall of Brambles&n",MAGE_WALL_OF_BRAMBLES,SPELL_NO_AFFECT,AREA_NONE,
		8,0,0,VNUM_WALL_OF_BRAMBLES,18,6,26,&CMage::Wall,SPHERE_FOREST));
	m_SpellTable.Add("Earthenalize", new CSpell<CMage>("Earthenalize","&GEarthenalize&n",MAGE_EARTHENALIZE,CCharacter::AFFECT_EARTHENALIZE,AREA_TARGET,
		8,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),8,10,46,&CMage::SpellAffectAdd,SPHERE_FOREST));
	m_SpellTable.Add("Faerie Flight",new CSpell<CMage>("Faerie Flight","&GFaerie Flight&n",MAGE_FAERIE_FLIGHT,CCharacter::AFFECT_FAERIE_FLIGHT,AREA_TARGET,
		4,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),10,9,41,&CMage::SpellAffectAdd,SPHERE_FOREST));
	m_SpellTable.Add("Forestal Armor",new CSpell<CMage>("Forestal Armor","&GForestal Armor&n",MAGE_FORESTAL_ARMOR,CCharacter::AFFECT_FORESTAL_ARMOR,AREA_SELF | SPELL_DEFENSIVE,
		6,30,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),15,8,36,&CMage::SpellAffectAdd,SPHERE_FOREST));
	m_SpellTable.Add("Haste", new CSpell<CMage>("Haste","&GHaste&n",MAGE_HASTE,CCharacter::AFFECT_HASTE,AREA_TARGET,
		3,4,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),20,5,21,&CMage::SpellAffectAdd,SPHERE_FOREST));
	m_SpellTable.Add("Faerie Fire", new CSpell<CMage>("Faerie Fire","&GFaerie Fire&n",MAGE_FAERIE_FIRE,CCharacter::AFFECT_FAERIE_FIRE,AREA_TARGET | SPELL_OFFENSIVE,
		3,10,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),18,6,28,&CMage::SpellAffectAdd,SPHERE_FOREST));
}

///////////////////
//	Initmountain sphere spells
void CMage::InitMountainSphereSpells()
{
	// MOUNTAIN
	m_SpellTable.Add("Burning Hands",new CSpell<CMage>("Burning Hands","&RBurning Hands&n",MAGE_BURNINGHANDS,
		SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,2,5,10,20,27,4,16,&CMage::OffensiveSpell,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Fire Arrows",new CSpell<CMage>("Fire Arrows","&RFire Arrows&n",MAGE_FIREARROWS,
		SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,3,5,20,20,25,5,21,&CMage::OffensiveSpell,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Immolate",new CSpell<CMage>("Immolate","&RImmolate&n",MAGE_IMMOLATE,
		SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,3,5,50,75,15,7,31,&CMage::OffensiveSpell,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Incinerate",new CSpell<CMage>("Incinerate","&RIncinerate&n",MAGE_INCINERATE,
		SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,4,8,100,100,10,8,36,&CMage::OffensiveSpell,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Hell Fire",new CSpell<CMage>("Hell Fire","&RHell Fire&n",MAGE_HELL_FIRE,
		SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,4,12,100,100,10,8,36,&CMage::OffensiveSpell,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Incendentary Cloud",new CSpell<CMage>("Incendentary Cloud","&RIncendentary Cloud&n",MAGE_INCENDENTARYCLOUD,
		SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,4,5,100,100,10,8,36,&CMage::OffensiveSpell,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Firestorm",new CSpell<CMage>("Firestorm","&RFirestorm&n",MAGE_FIRESTORM,SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		4,5,100,100,8,10,46,&CMage::OffensiveSpell,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Lightning bolt",new CSpell<CMage>("Lightning bolt","&RLightning bolt&n",MAGE_LIGHTNINGBOLT,
		SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,3,10,5,20,27,4,16,&CMage::OffensiveSpell,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Fireball",new CSpell<CMage>("Fireball","&RFireball&n",MAGE_FIREBALL,SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		3,5,10,50,20,6,26,&CMage::OffensiveSpell,SPHERE_MOUNTAIN));
    m_SpellTable.Add("Conjure Fire Elemental",new CSpell<CMage>("Conjure Fire Elemental","&RConjure Fire Elemental&n",MAGE_CONJURE_FIRE_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		8,100,16,400,23,5,21,&CMage::ConjureElemental,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Conjure Pyre Elemental",new CSpell<CMage>("Conjure Pyre Elemental","&RConjure Pyre Elemental&n",MAGE_CONJURE_PYRE_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		8,100,16,400,23,5,21,&CMage::ConjureElemental,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Wall of Flames",new CSpell<CMage>("Wall of Flames","&RWall of Flames&n",MAGE_WALL_OF_FLAMES,SPELL_NO_AFFECT,AREA_NONE,
		8,0,0,VNUM_WALL_OF_FLAMES,18,6,26,&CMage::Wall,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Fire Shield",new CSpell<CMage>("Fire Shield","&RFire Shield&n",MAGE_FIRE_SHIELD,CCharacter::AFFECT_FIRE_SHIELD,AREA_SELF,
		4,2,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),20,5,21,&CMage::SpellAffectAdd,SPHERE_MOUNTAIN));
	m_SpellTable.Add("Delayed Blast Fireball",new CSpell<CMage>("Delayed Blast Fireball","&RDelayed Blast Fireball&n",MAGE_DELAYED_BLAST_FIREBALL,SPELL_NO_AFFECT,AREA_ROOM | SPELL_OFFENSIVE,
		5,5,75,100,10,9,41,&CMage::DelayedBlastFireball,SPHERE_MOUNTAIN));
}

///////////////////////
//	InitSwap sphere spells
void CMage::InitSwapSphereSpells()
{
	// SWAMP
	m_SpellTable.Add("Call Heart",new CSpell<CMage>("Call Heart","&LCall Heart&n",MAGE_CALL_HEART,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		6,10,100,100,8,10,46,&CMage::OffensiveSpell,SPHERE_SWAMP));
	m_SpellTable.Add("Preserve",new CSpell<CMage>("Preserve","&LPreserve&n",MAGE_PRESERVE,SPELL_NO_AFFECT,AREA_NONE,
		10,10,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),23,5,21,&CMage::PreserveCorpse,SPHERE_SWAMP));
	m_SpellTable.Add("Embalm",new CSpell<CMage>("Embalm","&LEmbalm&n",MAGE_EMBALM,SPELL_NO_AFFECT,AREA_NONE,
		6,60,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR>>1),23,5,21,&CMage::PreserveCorpse,SPHERE_SWAMP));
	m_SpellTable.Add("Animate Dead",new CSpell<CMage>("Animate Dead","&LAnimate Dead&n",MAGE_ANIMATE_DEAD,SPELL_NO_AFFECT,AREA_NONE,
		6,10,10,0,25,4,16,&CMage::AnimateDead,SPHERE_SWAMP));
	m_SpellTable.Add("Create Spectre",new CSpell<CMage>("Create Spectre","&LCreate Spectre&n",MAGE_CREATE_SPECTRE,SPELL_NO_AFFECT,AREA_NONE,
		6,0,0,VNUM_UNDEAD_SPECTRE,18,6,26,&CMage::AnimateDead,SPHERE_SWAMP));
	m_SpellTable.Add("Create Ghost",new CSpell<CMage>("Create Ghost","&LCreate Ghost&n",MAGE_CREATE_GHOST,SPELL_NO_AFFECT,AREA_NONE,
		6,0,0,VNUM_UNDEAD_GHOST,15,7,31,&CMage::AnimateDead,SPHERE_SWAMP));
	m_SpellTable.Add("Create Ghoul",new CSpell<CMage>("Create Ghoul","&LCreate Ghoul&n",MAGE_CREATE_GHOUL,SPELL_NO_AFFECT,AREA_NONE,
		6,0,0,VNUM_UNDEAD_GHOUL,13,8,36,&CMage::AnimateDead,SPHERE_SWAMP));
	m_SpellTable.Add("Create Wight",new CSpell<CMage>("Create Wight","&LCreate Wight&n",MAGE_CREATE_WIGHT,SPELL_NO_AFFECT,AREA_NONE,
		6,0,0,VNUM_UNDEAD_WIGHT,10,9,41,&CMage::AnimateDead,SPHERE_SWAMP));
	m_SpellTable.Add("Create Wraith",new CSpell<CMage>("Create Wraith","&LCreate Wraith&n",MAGE_CREATE_WRAITH,SPELL_NO_AFFECT,AREA_NONE,
		6,0,0,VNUM_UNDEAD_WRAITH,8,10,46,&CMage::AnimateDead,SPHERE_SWAMP));
	m_SpellTable.Add("Become Lich",new CSpell<CMage>("Become Lich","&LBecome Lich&n",MAGE_BECOME_LICH,SPELL_NO_AFFECT,AREA_NONE,
		15,0,0,0,200,100,51,&CMage::BecomeLich,SPHERE_SWAMP));
	m_SpellTable.Add("Wall of Bones",new CSpell<CMage>("Wall of Bones","&LWall of Bones&n",MAGE_WALL_OF_BONES,SPELL_NO_AFFECT,AREA_NONE,
		8,0,0,VNUM_WALL_OF_BONES,13,8,36,&CMage::Wall,SPHERE_SWAMP));
	m_SpellTable.Add("Scrye",new CSpell<CMage>("Scrye","&LScrye&n",MAGE_SCRYE,SPELL_NO_AFFECT,AREA_NONE,
		5,0,0,0,13,8,36,&CMage::Scrye,SPHERE_SWAMP));
	m_SpellTable.Add("Awakened Nightmare",new CSpell<CMage>("Awakened Nightmare","&LAwakened Nightmare&n",MAGE_AWAKENED_NIGHTMARE,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		2,5,10,40,27,4,16,&CMage::OffensiveSpell,SPHERE_SWAMP));
}

/////////////////////
//	Init grey sphere spells
void CMage::InitGreySphereSpells()
{
	// GREY
	m_SpellTable.Add("Guks Flicking Finger",new CSpell<CMage>("Guks Flicking Finger","&wGuks Flicking Finger&n",MAGE_GUKSFLICKINGFINGER,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		3,5,50,50,27,5,21,&CMage::OffensiveSpell,SPHERE_GRAY));
	m_SpellTable.Add("Guks Slapping Hand",new CSpell<CMage>("Guks Slapping Hand","&wGuks Slapping Hand&n",MAGE_GUKSSLAPPINGHAND,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		4,5,100,50,25,6,26,&CMage::OffensiveSpell,SPHERE_GRAY));
	m_SpellTable.Add("Guks Wicked Glance",new CSpell<CMage>("Guks Wicked Glance","&wGuks Wicked Glance&n",MAGE_GUKSWICKEDGLANCE,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		1,5,20,10,23,4,16,&CMage::OffensiveSpell,SPHERE_GRAY));
	m_SpellTable.Add("Guks Angry Fist",new CSpell<CMage>("Guks Angry Fist","&wGuks Angry Fist&n",MAGE_GUKSANGRYFIST,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		3,10,100,100,20,7,31,&CMage::OffensiveSpell,SPHERE_GRAY));
	m_SpellTable.Add("Guks Fist Flurry",new CSpell<CMage>("Guks Fist Flurry","&wGuks Fist Flurry&n",MAGE_GUKSFISTFLURRY,SPELL_NO_AFFECT,AREA_TARGET | SPELL_OFFENSIVE | SPELL_DOES_DAM,
		4,12,100,150,10,9,41,&CMage::OffensiveSpell,SPHERE_GRAY));
	m_SpellTable.Add("Guks Magic Portal",new CSpell<CMage>("Guks Magic Portal","&wGuks Magic Portal&n",MAGE_GUKS_MAGIC_PORTAL,SPELL_NO_AFFECT,AREA_NONE,
		8,4,CMudTime::PULSES_PER_MUD_HOUR,(CMudTime::PULSES_PER_MUD_HOUR<<1),8,10,46,&CMage::GuksMagicPortal,SPHERE_GRAY));
	m_SpellTable.Add("Guks Interposing Hand",new CSpell<CMage>("Guks Interposing Hand","&wGuks Interposing Hand&n",MAGE_GUKS_INTERPOSING_HAND,SPELL_NO_AFFECT,AREA_NONE,
		8,0,0,VNUM_WALL_GUKS_HAND,13,8,36,&CMage::Wall,SPHERE_GRAY));
	m_SpellTable.Add("Guks Glowing Globes",new CSpell<CMage>("Guks Glowing Globes","&wGuks Glowing Globes&n",MAGE_GUKS_GLOWING_GLOBES,CCharacter::AFFECT_GUKS_GLOWING_GLOBES,AREA_SELF | SPELL_DEFENSIVE,
		4,3,2,2,18,8,36,&CMage::GuksGlowingGlobes,SPHERE_GRAY));
	m_SpellTable.Add("Create Magic Elemental",new CSpell<CMage>("Create Magic Elemental","&LCreate Magic Elemental&n",MAGE_CREATE_MAGIC_ELEMENTAL,SPELL_NO_AFFECT,AREA_NONE,
		10,0,0,VNUM_MAGIC_ELEMENTAL,8,10,46,&CMage::CreateMagicElemental,SPHERE_GRAY));
	m_SpellTable.Add("Prismatic Shield",new CSpell<CMage>("Prismatic Shield","Prismatic Shield",MAGE_PRISMATIC_SHIELD,CCharacter::AFFECT_STONE_SKIN,AREA_TARGET,
		4,CMudTime::PULSES_PER_MUD_HOUR,5,(CMudTime::PULSES_PER_MUD_HOUR>>1),10,9,41,&CMage::SpellAffectAdd,0));
		
}

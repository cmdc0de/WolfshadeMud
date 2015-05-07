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

CCharacter::sAffectInfo CCharacter::m_AffectInfo[TOTAL_AFFECTS];
const short CCharacter::AFFECT_STONE_SKIN=0;
const short CCharacter::AFFECT_CASTING=1;
const short CCharacter::AFFECT_SNEAKING=2;
const short CCharacter::AFFECT_HIDING=3;
const short CCharacter::AFFECT_COLD_SHIELD=4;
const short CCharacter::AFFECT_MEDITATE=5;
const short CCharacter::AFFECT_HEAT_SHIELD=6;
const short CCharacter::AFFECT_BLINDNESS=7;
const short CCharacter::AFFECT_FISTS_OF_STONE=8;
const short CCharacter::AFFECT_DETECT_MAGIC=9;
const short CCharacter::AFFECT_AUDIBLE_GLAMOUR=10;
const short CCharacter::AFFECT_BIOFEEDBACK=11;
const short CCharacter::AFFECT_INERTIAL_BARRIER=12;
const short CCharacter::AFFECT_TRANSPARENT_FORM=13;
const short CCharacter::AFFECT_AIR_WALK=14;
const short CCharacter::AFFECT_CLEAR_SIGHT=15;
const short CCharacter::AFFECT_CAMOUFLAGE=16;
const short CCharacter::AFFECT_AIR_SHIELD=17;
const short CCharacter::AFFECT_AIR_CUSHION=18;
const short CCharacter::AFFECT_INVISIBLE=19;
const short CCharacter::AFFECT_VULTURE_FLIGHT=20;
const short CCharacter::AFFECT_FLYING=21;
const short CCharacter::AFFECT_DETECT_INVISIBILITY=22;
const short CCharacter::AFFECT_INVISIBILITY=23;
const short CCharacter::AFFECT_MIST_FORM=24;
const short CCharacter::AFFECT_MIST_WALK=25;
const short CCharacter::AFFECT_MIST_SHIELD=26;
const short CCharacter::AFFECT_MIST_BARRIER=27;
const short CCharacter::AFFECT_ARMOR=28;
const short CCharacter::AFFECT_BARKSKIN=29;
const short CCharacter::AFFECT_DEXTERITY=30;
const short CCharacter::AFFECT_STRENGTH=31;
const short CCharacter::AFFECT_AGILITY=32;
const short CCharacter::AFFECT_BATTLE_READY=33;
const short CCharacter::AFFECT_PHYSICAL_ENHANCEMENT=34;
const short CCharacter::AFFECT_MAJOR_PARA=35;
const short CCharacter::AFFECT_EARTHENALIZE=36;
const short CCharacter::AFFECT_FAERIE_FLIGHT=37;
const short CCharacter::AFFECT_FIRE_SHIELD=38;
const short CCharacter::AFFECT_FORESTAL_ARMOR=39;
const short CCharacter::AFFECT_GUKS_GLOWING_GLOBES=40;
const short CCharacter::AFFECT_HASTE=41;
//const short CCharacter::AFFECT_MIRROR_IMAGE=42; //can be reused
const short CCharacter::AFFECT_CHANT=43;
const short CCharacter::AFFECT_CHANT_REGENERATION=44;
const short CCharacter::AFFECT_CHANT_DEFENSE=45;
const short CCharacter::AFFECT_CHANT_HONOR=46;
const short CCharacter::AFFECT_BLOODLUST=47;
const short CCharacter::AFFECT_BLESS=48;
const short CCharacter::AFFECT_CURSE=49;
const short CCharacter::AFFECT_FAERIE_FIRE=50;
const short CCharacter::AFFECT_FUGUE_PLANE=51;
const short CCharacter::AFFECT_BERSERK = 52;
const short CCharacter::AFFECT_DI = 53;
const short CCharacter::AFFECT_INTIMIDATE = 54;
const short CCharacter::AFFECT_AGE_DEATH = 55;
const short CCharacter::AFFECT_ARKANS_BATTLE_CRY = 56;
const short CCharacter::AFFECT_SPIRIT_ARMOR = 57;
const short CCharacter::AFFECT_DIVINE_ARMOR = 58;
const short CCharacter::AFFECT_FLIGHT = 59;
const short CCharacter::AFFECT_VAMPIRIC_TOUCH = 60;
const short CCharacter::AFFECT_CANT_LAY_HANDS = 61;
const short CCharacter::AFFECT_LIGHT = 62;
const short CCharacter::AFFECT_SET_UP_CAMP = 63;
const short CCharacter::AFFECT_NO_BANDAGE_TIME = 64;
const short CCharacter::AFFECT_INNATE_FLY=65;
const short CCharacter::AFFECT_INNATE_FLY_TIRED=66;
const short CCharacter::AFFECT_INNATE_STRENGTH_TIRED=67;
const short CCharacter::AFFECT_INNATE_BITE_TIRED=68;
const short CCharacter::AFFECT_INNATE_FAERIE_FIRE_TIRED=69;
const short CCharacter::AFFECT_INNATE_INVIS_TIRED=70;
const short CCharacter::AFFECT_INNATE_INVIS=71;
const short CCharacter::AFFECT_PSIONIC_LEECH = 72;

CCharacter::sAffectInfo::sAffectInfo
		(CString strName, bool bAcumTime, bool bAcumValue, int nRate, //for round function
			void (CCharacter::*fApply)(short nAffect,bool bRemoving),
			void (CCharacter::*fRound)(),bool bReferenceAffect)
{
	m_strName = strName;
	m_bAcumTime = bAcumTime;
	m_bAcumValue = bAcumValue;
	m_nRateOfAffect = nRate;
	m_pFncApply = fApply;
	m_pFncRound = fRound;
	m_bReferenceAffect = bReferenceAffect;
}

void CCharacter::InitStatics()
{
	if(m_bInit)
		return;
	m_bInit = true;

	////////////////////////////////////////
	//	g++ won't implicity cast a NULL to void (CCharacter::*)()
	//	so we'll cast every one
	// 	it also won't implicitly cast CCharacter::ApplyAffect as a function ptr either
	//	I hope I'm just missing a comlier flag... so we'll just cast every one...ugh
#define CAST1(a) (void (CCharacter::*)(short,bool)) a
#define CAST2(a) (void (CCharacter::*)()) a

	//current reference affects are (12-8-99)
	//affect_invisible affect_flying affect_detect_invisibility
	//reference affects like AFFECT_DETECT_INVISIBILITY, AFFECT_INVISIBLE or AFFECT_FLYING should have a true for refrence affect
	//add time about then in the apply affect function they will just be added to by the multible
	//reference affects SHOULD NOT CALL ANY ApplyAffect Function!
	//The remove affect function will check to see if we are trying to remove one then remove all other affects it references
	
	m_AffectInfo[AFFECT_STONE_SKIN] = sAffectInfo("Stone Skin",true,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_HIDING] = sAffectInfo("Hidden",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_INERTIAL_BARRIER] = sAffectInfo("Inertial Barrier",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_COLD_SHIELD] = sAffectInfo("Cold Shield",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_HEAT_SHIELD] = sAffectInfo("Heat Shield",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_BLINDNESS] = sAffectInfo("Blindness",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_MEDITATE] = sAffectInfo("Meditating",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_FISTS_OF_STONE] = sAffectInfo("Fists of Stone",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_DETECT_MAGIC] = sAffectInfo("Detect Magic",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_AUDIBLE_GLAMOUR] = sAffectInfo("Audible Glamour",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_BIOFEEDBACK] = sAffectInfo("Biofeedback",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_TRANSPARENT_FORM] = sAffectInfo("Transparent Form",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_AIR_WALK] = sAffectInfo("Air Walk",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_CLEAR_SIGHT] = sAffectInfo("Clear Sight",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_CAMOUFLAGE] = sAffectInfo("Camouflage",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_INVISIBLE] = sAffectInfo("Invisible",true,false,0,CAST1(NULL),CAST2(NULL),true);
	m_AffectInfo[AFFECT_AIR_SHIELD] = sAffectInfo("Air Shield",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_AIR_CUSHION] = sAffectInfo("Air Cushion",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_VULTURE_FLIGHT] = sAffectInfo("Vulture Flight",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_FLYING] = sAffectInfo("",true,false,0,CAST1(NULL),CAST2(NULL),true);
	m_AffectInfo[AFFECT_DETECT_INVISIBILITY] = sAffectInfo("",true,false,0,CAST1(NULL),CAST2(NULL),true);
	m_AffectInfo[AFFECT_DI] = sAffectInfo("Detect Invisibility",true,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_INVISIBILITY] = sAffectInfo("Invisibility",true,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_MIST_FORM] = sAffectInfo("Mist Form",true,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_MIST_WALK] = sAffectInfo("Mist Walk",true,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_MIST_SHIELD] = sAffectInfo("Mist Shield",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_MIST_BARRIER] = sAffectInfo("Mist Barrier",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_ARMOR] = sAffectInfo("Armor",true,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_BARKSKIN] = sAffectInfo("Barkskin",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_DEXTERITY] = sAffectInfo("Dexterity",true,true,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_STRENGTH] = sAffectInfo("Strength",true,true,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_AGILITY] = sAffectInfo("Agility",true,true,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_BATTLE_READY] = sAffectInfo("Battle Ready",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_PHYSICAL_ENHANCEMENT] = sAffectInfo("Physical Enhancement",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_MAJOR_PARA] = sAffectInfo("Major Paralaysis",true,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_EARTHENALIZE] = sAffectInfo("Earthenalize",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_FAERIE_FLIGHT] = sAffectInfo("Faerie Flight",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_FIRE_SHIELD] = sAffectInfo("Fire Shield",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_GUKS_GLOWING_GLOBES] = sAffectInfo("Guks Glowing Globes",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_HASTE] = sAffectInfo("Haste",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_BLOODLUST] = sAffectInfo("Bloodlust",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_BLESS] = sAffectInfo("Bless",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_CURSE] = sAffectInfo("Curse",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_FAERIE_FIRE] = sAffectInfo("Faerie Fire",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_FUGUE_PLANE] = sAffectInfo("Spirit",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_BERSERK] = sAffectInfo("Berserking",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_INTIMIDATE] = sAffectInfo("Intimidated",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_AGE_DEATH] = sAffectInfo("Age Death",false,false,CMudTime::PULSES_PER_MUD_MIN,CAST1(NULL),&CCharacter::AgeDeath);
	m_AffectInfo[AFFECT_ARKANS_BATTLE_CRY] = sAffectInfo("Arkans Battle Cry",true,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_SPIRIT_ARMOR] = sAffectInfo("Spirit Armor",true,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_DIVINE_ARMOR] = sAffectInfo("Divine Armor",true,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_FLIGHT] = sAffectInfo("Flight",true,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_VAMPIRIC_TOUCH] = sAffectInfo("Vampiric Touch",true,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_CANT_LAY_HANDS] = sAffectInfo("",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_LIGHT] = sAffectInfo("",true,true,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_SET_UP_CAMP] = sAffectInfo("Setting up camp",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_NO_BANDAGE_TIME] = sAffectInfo("",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_INNATE_FLY] = sAffectInfo("innate fly",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_INNATE_FLY_TIRED] = sAffectInfo("",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_INNATE_STRENGTH_TIRED] = sAffectInfo("",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_INNATE_BITE_TIRED] = sAffectInfo("",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_INNATE_FAERIE_FIRE_TIRED] = sAffectInfo("",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_INNATE_INVIS_TIRED] = sAffectInfo("",false,false,0,CAST1(NULL),CAST2(NULL));
	m_AffectInfo[AFFECT_INNATE_INVIS] = sAffectInfo("innate invis",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_CHANT_REGENERATION] = sAffectInfo("Chanting Regeneration",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_CHANT_DEFENSE] = sAffectInfo("Chanting Defense",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_CHANT_HONOR] = sAffectInfo("Chanting Honor",false,false,0,&CCharacter::ApplyAffect,CAST2(NULL));
	m_AffectInfo[AFFECT_PSIONIC_LEECH] = sAffectInfo("Psionic Leech",true,false,0,CAST1(NULL),CAST2(NULL));
}

////////////////////////////////////
//	Slowly Kill Character
void CCharacter::AgeDeath()
{
	int nHit = DIE(100);
	m_nCurrentHpts -= nHit;
	m_nMaxHpts -= nHit;
	UpDatePosition();
}

//////////////////////////
//	Removes an affect
//	Sends an ending message to the owner
//	calls ending function if one.
//
//	other notes
//The remove affect function will call syncwearaffects...
//we can reset the affect value for imposed limits or we'll get the following
//..AC = -99 we cast spirit armor and change it's affect value from 10 to 1 to impose
//-100 limit...then player removes a piece of equipment that give 11 ac thier ac falls to -91 instead
//of being -100 like it should be.
// to solve this a bool will be added to RemoveAffect that is defaulted to true
//this bool if true will call syncwearaffects ....they only time it will be false is during the call to ccharacter::fadeAffects() or
//a call to removeallaffects so we don't call it for every affect that is removed just once at the end.
/////////////////////////////////////////////////////////
void CCharacter::RemoveAffect(short nAffect,bool bSendMsg)
{
	if(m_AffectInfo[nAffect].IsReferenceAffect())
	{
		m_Affects[nAffect].Remove();
		if(nAffect==AFFECT_DETECT_INVISIBILITY)
		{
			RemoveAffect(AFFECT_DI,bSendMsg);
			RemoveAffect(AFFECT_CLEAR_SIGHT,bSendMsg);
		}
		else if(nAffect==AFFECT_INVISIBLE)
		{
			RemoveAffect(AFFECT_CAMOUFLAGE,bSendMsg);
			RemoveAffect(AFFECT_MIST_FORM,bSendMsg);
			RemoveAffect(AFFECT_INVISIBILITY,bSendMsg);
			RemoveAffect(AFFECT_INNATE_INVIS,bSendMsg);
			RemoveAffect(AFFECT_TRANSPARENT_FORM,bSendMsg);
		}
		else if(nAffect==AFFECT_FLYING)
		{
			RemoveAffect(AFFECT_VULTURE_FLIGHT,bSendMsg);
			RemoveAffect(AFFECT_FAERIE_FLIGHT,bSendMsg);
			RemoveAffect(AFFECT_FLIGHT,bSendMsg);
			RemoveAffect(AFFECT_INNATE_FLY,bSendMsg);
			RemoveAffect(AFFECT_AIR_WALK,bSendMsg);
		}
	}
	else if(!m_Affects[nAffect].IsPermAffect() &&
		m_Affects[nAffect].HasAffect())
	{
		if(m_AffectInfo[nAffect].m_pFncApply)
		{
			(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,true);
		}
		//the apply function may rely on the value in sAffect
		//so we must remove the affect after the function is called
		m_Affects[nAffect].Remove();
		if(bSendMsg)
		{
			m_MsgManager.SendAffectMsg(m_AffectInfo[nAffect].m_strName,this,MSG_END_AFFECT);
		}
	}
}

void CCharacter::ReduceAffect(short nAffect, int nPulses, int nValue, bool bSendMsg) 
{
	if(!m_Affects[nAffect].IsPermAffect() &&
		m_Affects[nAffect].HasAffect())
	{
		int nOldTime = m_Affects[nAffect].m_nTime;
		int nOldValue = m_Affects[nAffect].m_nValue;
		if(m_Affects[nAffect].IsManual())
		{
			if((nOldValue-nValue)<=0)
			{
				RemoveAffect(nAffect,bSendMsg);
			}
			else
			{
				//remove the affect for what we have
				if(m_AffectInfo[nAffect].m_pFncApply)
				{
					(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,true);
				}
				//blank the affect
				m_Affects[nAffect].Remove();
				//reset with new values
				m_Affects[nAffect].Set(sAffect::MANUAL_AFFECT,(nOldValue-nValue));
				//reapply affect with new 
				if(m_AffectInfo[nAffect].m_pFncApply)
				{
					(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,false);
				}
			}
		}
		else
		{
			if((nOldValue-nValue)<0 || (nOldTime-nPulses)<0)
			{
				RemoveAffect(nAffect,bSendMsg);
			}
			else
			{
				//remove the affect for what we have
				if(m_AffectInfo[nAffect].m_pFncApply)
				{
					(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,true);
				}
				//blank the affect
				m_Affects[nAffect].Remove();
				//reset with new values
				m_Affects[nAffect].Set((nOldTime-nPulses),(nOldValue-nValue));
				//reapply affect with new 
				if(m_AffectInfo[nAffect].m_pFncApply)
				{
					(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,false);
				}
			}
		}
	}
}


///////////////////////////
//	Call for all characters in hasCommand()
//	to reduce all affects by 1 pulse
//
void CCharacter::FadeAffects(int nTime)
{
	bool bSync = false;
	for(register int i = 0;i<TOTAL_AFFECTS;i++)
	{
		if(m_Affects[i].HasAffect())
		{
			if(m_Affects[i].IsAffectTime(m_AffectInfo[i].m_nRateOfAffect))
			{
				if(m_AffectInfo[i].m_pFncRound!=NULL)
				{
					(this->*m_AffectInfo[i].m_pFncRound)();
					bSync = true;
				}										
			}
			if(!m_Affects[i].IsPermAffect() && !m_Affects[i].IsManual())
			{
				m_Affects[i].m_nTime-=nTime;
				if(m_Affects[i].m_nTime <= 1)
				{
					RemoveAffect(i,true);
					bSync = true;
				}
			}
		}
	}
	if(bSync)
	{
		SyncWearAffects();
	}
	m_Master.ReduceMastersHold(nTime,this);
}

////////////////////////////
//  Called from Defense function in CCharacter
//  we will put all defensive melee affects and what
//  they do to each character
//  written by: Demetrius Comes
int CCharacter::DoProtectionMelee(CCharacter *pAttacker,int nDam)
{
    if(m_Affects[CCharacter::AFFECT_COLD_SHIELD].HasAffect())
    {
        pAttacker->TakeDamage(this,nDam>>1,true);
        pAttacker->GetRoom()->SendToRoom("%s &Bshivers&n in pain.\r\n",pAttacker);
    }
    if(m_Affects[CCharacter::AFFECT_HEAT_SHIELD].HasAffect())
    {
        pAttacker->TakeDamage(this,(nDam>>2),true);
        pAttacker->GetRoom()->SendToRoom("%s &Rscreams&n in pain from intense &Rh&yea&Rt&n!\r\n",pAttacker);
    }
    if(m_Affects[CCharacter::AFFECT_FIRE_SHIELD].HasAffect())
    {
        pAttacker->TakeDamage(this,nDam>>1,true);
        pAttacker->GetRoom()->SendToRoom("%s &Rscreams&n in pain from &Rburning fire&n!\r\n",pAttacker);
    }
    if(m_Affects[CCharacter::AFFECT_BIOFEEDBACK].HasAffect())
    {
        pAttacker->TakeDamage(this,nDam>>1,true);
        pAttacker->GetRoom()->SendToRoom("&G%s &Gscreams&n in pain from intense bio-toxins&n!\r\n",pAttacker);
    }
    if(m_Affects[CCharacter::AFFECT_STONE_SKIN].HasAffect())
    {
        m_Affects[CCharacter::AFFECT_STONE_SKIN].m_nValue -= nDam;
        if(m_Affects[CCharacter::AFFECT_STONE_SKIN].m_nValue <= 0)
        {
            nDam = abs(m_Affects[CCharacter::AFFECT_STONE_SKIN].m_nValue);
            RemoveAffect(CCharacter::AFFECT_STONE_SKIN);
        }
        else
        {
            nDam = 0;
        }
    }
    if(m_Affects[CCharacter::AFFECT_INERTIAL_BARRIER].HasAffect())
    {
        m_Affects[CCharacter::AFFECT_INERTIAL_BARRIER].m_nValue-=nDam;
        if(m_Affects[CCharacter::AFFECT_INERTIAL_BARRIER].m_nValue<=0)
        {
            nDam = abs(m_Affects[CCharacter::AFFECT_INERTIAL_BARRIER].m_nValue);
            RemoveAffect(CCharacter::AFFECT_INERTIAL_BARRIER);
        }
        else
        {
            nDam = 0;
        }
    }
    if(m_Affects[CCharacter::AFFECT_AIR_SHIELD].HasAffect())
    {
        m_Affects[CCharacter::AFFECT_AIR_SHIELD].m_nValue-=nDam;
        if(m_Affects[CCharacter::AFFECT_AIR_SHIELD].m_nValue<=0)
        {
            nDam = abs(m_Affects[CCharacter::AFFECT_AIR_SHIELD].m_nValue);
            RemoveAffect(CCharacter::AFFECT_AIR_SHIELD);
        }
        else
        {
            nDam = 0;
        }
    }
    if(m_Affects[CCharacter::AFFECT_AIR_CUSHION].HasAffect())
    {
        nDam /= 2;
    }
    if(m_Affects[CCharacter::AFFECT_MIST_SHIELD].HasAffect())
    {
        nDam = (nDam * 3) / 4;
    }
    if(m_Affects[CCharacter::AFFECT_MIST_BARRIER].HasAffect())
    {
        nDam = (nDam * 3) / 4;
    }
    return nDam;
}


////////////////////////////
//  Called from SpellDefense function in CCharacter
//  
//  
//  written by: John Comes 2-2-99
int CCharacter::DoProtectionSpells(CCharacter *pAttacker,int nDam, int nSpellLevel)
{
	CString strToChar;
    if(m_Affects[CCharacter::AFFECT_GUKS_GLOWING_GLOBES].HasAffect())
    {
        pAttacker->GetRoom()->SendToRoom("&ROne of %s's glowing globes flies out and intercepts the spell cast by %s!&n\r\n",this,pAttacker);
		pAttacker->SendToChar("&RA globe around them absorbs your spell!&n\r\n");
		SendToChar("&ROne of your globes absorbs the spell!&n\r\n");
		m_Affects[CCharacter::AFFECT_GUKS_GLOWING_GLOBES].m_nValue--;
		if(m_Affects[CCharacter::AFFECT_GUKS_GLOWING_GLOBES].m_nValue <= 0)
		{
			RemoveAffect(CCharacter::AFFECT_GUKS_GLOWING_GLOBES);
		}
		nDam = 0;
		strToChar.Format("&RYou have %d globes left.&n\r\n",m_Affects[CCharacter::AFFECT_GUKS_GLOWING_GLOBES].m_nValue);
		SendToChar(strToChar);
    }
	if(MakeSavingThrow(CAttributes::SAVE_SPELL))
	{
		//make the saving throw cut dam in half
		nDam>>=1;
	}
	return nDam;
}



//////////////////////
//	Gets enchantment string for 
//	score function
//
void CCharacter::GetEnchantments(CString &str)
{
	for(register int i=0;i<TOTAL_AFFECTS;i++)
	{
		if(m_Affects[i].HasAffect() && !m_AffectInfo[i].m_strName.IsEmpty()
			&& !m_AffectInfo[i].IsReferenceAffect())
		{
			str.Format("%s %s ",(char *)str,(char *) m_AffectInfo[i].m_strName);
			if(IsAffectedBy(CCharacter::AFFECT_DETECT_MAGIC) && m_Affects[i].m_nTime < CMudTime::FADING_TIME && m_Affects[i].m_nTime > CMudTime::FADING_RAPIDLY_TIME)
			{
				str += "(&Wfad&wing&n)";
			}
			if(IsAffectedBy(CCharacter::AFFECT_DETECT_MAGIC) && m_Affects[i].m_nTime < CMudTime::FADING_RAPIDLY_TIME)
			{
				str += "(&Wfa&wdi&Lng rapidly&n)";
			}
			str += "\r\n";
		}
	}
}


// Watch going over max, change value!
//	This function applys the results of different affects
//	to the character
//	DO NOT PUT ANY SEND TO CHARS IN HERE!
//	Note:
//	DO NOT PUT REFERENCE AFFECTS  HERE LOOK AT REMOVE_AFFECT
//	case AFFECT_INVISIBLE: case AFFECT_FLYING:case AFFECT_DETECT_INVISIBILITY:
////////////////////////////////////////////////////////////////
//  other notes
//The remove affect function will call syncwearaffects...
//we can reset the affect value for imposed limits or we'll get the following
//..AC = -99 we cast spirit armor and change it's affect value from 10 to 1 to impose
//-100 limit...then player removes a piece of equipment that give 11 ac thier ac falls to -91 instead
//of being -100 like it should be.
// to solve this a bool will be added to RemoveAffect that is defaulted to true
//this bool if true will call syncwearaffects ....they only time it will be false is during the call to ccharacter::fadeAffects() or
//a call to removeallaffects so we don't call it for every affect that is removed just once at the end.
void CCharacter::ApplyAffect(short nAffect,bool bRemoving)
{
	short nDir = bRemoving ? -1 : 1;
	short nAmount = this->GetAffectValue(nAffect) * nDir;
	switch(nAffect)
	{
	case AFFECT_ARMOR: case AFFECT_BARKSKIN: case AFFECT_SPIRIT_ARMOR:
	case AFFECT_DIVINE_ARMOR: case AFFECT_FORESTAL_ARMOR: case AFFECT_FAERIE_FIRE:
		this->m_nAC=RANGE((this->m_nAC-nAmount),MAX_AC,MIN_AC);
		break;
	case AFFECT_DEXTERITY:
		nAmount = (nAmount+this->m_pAttributes->GetStat(DEX)>100) ? (100-this->m_pAttributes->GetStat(DEX)) : nAmount;
		this->m_pAttributes->AdjustDex(nAmount);
		break;
	case AFFECT_STRENGTH:
		nAmount = (nAmount+this->m_pAttributes->GetStat(STR)>100) ? (100-this->m_pAttributes->GetStat(STR)) : nAmount;
		this->m_pAttributes->AdjustStr(nAmount);
		break;
	case AFFECT_AGILITY:
		nAmount = (nAmount+this->m_pAttributes->GetStat(AGI)>100) ? (100-this->m_pAttributes->GetStat(AGI)) : nAmount;
		this->m_pAttributes->AdjustAgi(nAmount);
		break;
	case AFFECT_BLINDNESS:
		if(bRemoving)
		{
			this->m_nToHit<<=1;
		}
		else
		{
			this->m_nToHit>>=1;
		}
		break;
	case AFFECT_BLESS:
		this->m_nToHit+=nAmount;
		break;
	case AFFECT_CURSE:
		this->m_nToHit-=nAmount;
		break;
	case AFFECT_BATTLE_READY:
		this->m_nToHit+=nAmount;
		this->m_nDamRoll+=nAmount;
		this->m_nAC=RANGE(this->m_nAC - (nAmount * 3),MAX_AC,MIN_AC);
		break;
	case AFFECT_PHYSICAL_ENHANCEMENT:
		{
			int nAmt;
			nAmt = (nAmount+this->m_pAttributes->GetStat(DEX)>100) ? (100-this->m_pAttributes->GetStat(DEX)) : nAmount;
			this->m_pAttributes->AdjustDex(nAmt);
			nAmt = (nAmount+this->m_pAttributes->GetStat(STR)>100) ? (100-this->m_pAttributes->GetStat(STR)) : nAmount;
			this->m_pAttributes->AdjustStr(nAmt);
			nAmt = (nAmount+this->m_pAttributes->GetStat(AGI)>100) ? (100-this->m_pAttributes->GetStat(AGI)) : nAmount;
			this->m_pAttributes->AdjustAgi(nAmt);
		}
		break;
	case AFFECT_CHANT_REGENERATION:
		if(bRemoving) 
			 m_Affects[AFFECT_CHANT].Remove();
		else
			 AddAffect(AFFECT_CHANT,sAffect::MANUAL_AFFECT,0);
		break;
	case AFFECT_CHANT_DEFENSE:
		this->m_nAC=RANGE(this->m_nAC+nAmount,MAX_AC,MIN_AC);
		if(bRemoving) 
			 m_Affects[AFFECT_CHANT].Remove(); 
		else
			AddAffect(AFFECT_CHANT,sAffect::MANUAL_AFFECT,0);
		break;
	case AFFECT_ARKANS_BATTLE_CRY:
		this->m_nToHit += nAmount;
		this->m_nDamRoll += nAmount;
		this->m_nAC = RANGE(this->m_nAC - nAmount,MAX_AC,MIN_AC);
		break;
	case AFFECT_CHANT_HONOR:
		{
			int nAmt;
			nAmt = (nAmount+this->m_pAttributes->GetStat(DEX)>100) ? (100-this->m_pAttributes->GetStat(DEX)) : nAmount;
			this->m_pAttributes->AdjustDex(nAmt);
			nAmt = (nAmount+this->m_pAttributes->GetStat(STR)>100) ? (100-this->m_pAttributes->GetStat(STR)) : nAmount;
			this->m_pAttributes->AdjustStr(nAmt);
			nAmt = (nAmount+this->m_pAttributes->GetStat(AGI)>100) ? (100-this->m_pAttributes->GetStat(AGI)) : nAmount;
			this->m_pAttributes->AdjustAgi(nAmt);
		}
		if(bRemoving)
		{
			RemoveAffect(AFFECT_HASTE);
			m_Affects[AFFECT_CHANT].Remove();
		}
		else
		{
			AddAffect(AFFECT_CHANT,sAffect::MANUAL_AFFECT,0);
		}
		break;
	case AFFECT_FUGUE_PLANE:
		if(bRemoving)
		{
			LeaveFuguePlane();
		}
		break;
	case AFFECT_BERSERK:
		{
			int nAmt = (nAmount+this->m_pAttributes->GetStat(STR)>100) ? (100-this->m_pAttributes->GetStat(STR)) : nAmount;
			this->m_pAttributes->AdjustStr(nAmt);
			this->m_nToHit += nAmount/20;
			this->m_nDamRoll += nAmount/20;
			this->m_nCurrentHpts += nAmount;
			this->m_nMaxHpts += nAmount;
		}
		break;
	case AFFECT_INTIMIDATE:
		this->m_nToHit -= nAmount;
		this->m_nDamRoll -= nAmount;
		break;
	case AFFECT_CAMOUFLAGE: case AFFECT_MIST_FORM: case AFFECT_INVISIBILITY:
	case AFFECT_INNATE_INVIS: case AFFECT_TRANSPARENT_FORM:
		if(bRemoving) 
			ReduceAffect(AFFECT_INVISIBLE,m_Affects[nAffect].m_nTime,0,false);
		else
			//add the time to the reference affect!
			AddAffect(AFFECT_INVISIBLE,m_Affects[nAffect].m_nTime,0);
		break;
	case AFFECT_DI: case AFFECT_CLEAR_SIGHT:
		if(bRemoving) 
			ReduceAffect(AFFECT_DETECT_INVISIBILITY,m_Affects[nAffect].m_nTime,0,false);
		else
			AddAffect(AFFECT_DETECT_INVISIBILITY,m_Affects[nAffect].m_nTime,0);
		break;
	case AFFECT_VULTURE_FLIGHT: case AFFECT_FAERIE_FLIGHT: case AFFECT_FLIGHT:
	case AFFECT_INNATE_FLY: case AFFECT_AIR_WALK: 
		if(bRemoving) 
			ReduceAffect(AFFECT_FLYING,m_Affects[nAffect].m_nTime,0,false);
		else
			AddAffect(AFFECT_FLYING,m_Affects[nAffect].m_nTime,0);	
		break;
	case AFFECT_LIGHT:
		if(bRemoving)
		{
			GetRoom()->ReduceAffect(CRoom::ROOM_AFFECT_LIGHT,m_Affects[AFFECT_LIGHT].m_nValue);
		}
		else
		{
			GetRoom()->AddAffect(CRoom::ROOM_AFFECT_LIGHT,sAffect::MANUAL_AFFECT,m_Affects[AFFECT_LIGHT].m_nValue);
		}
		break;
	case AFFECT_SET_UP_CAMP:
		//if camp time complete and we are removing the affect
		if(bRemoving && m_Affects[AFFECT_SET_UP_CAMP].m_nTime<=1)
		{
			if(!IsLinkDead())
			{
				m_pDescriptor->ChangeState(STATE_RENT);		
			}
			m_lLastRentRoom = GetRoom()->GetVnum();
		}
		break;
	default:
		ErrorLog << "Error out in Apply Affect we were trying to " << (bRemoving ? "remove " : "add ") << "affect #" << nAffect << endl;
		break;
	}
}

//////////////////////////
//	Removes all affects
//	Sends an ending message to the owner
//	calls ending function if one.
//
void CCharacter::RemoveAllAffects(bool bSendMsg)
{
	for(register int i = 0;i < TOTAL_AFFECTS;i++)
	{
		if(IsAffectedBy(i))
		{
			RemoveAffect(i,bSendMsg);
		}
	}
	SyncWearAffects();
}


//////////////////////////////////////
//	Adds an CCharacter Affect to a character
bool CCharacter::AddAffect(short nAffect,int nPulses, int nValue, bool bSendMsg)
{
#ifdef _DEBUG
	if(nAffect > TOTAL_AFFECTS)
		assert(0);
#endif
	//check to see if we have the affect
	bool bHasAffect = m_Affects[nAffect].HasAffect();
	//if we are trying to add hiding and the character is fighting...
	//just return...we'll just come out of hiding the next combat round
	//until the next attack round.
	if(nAffect==AFFECT_HIDING && IsFighting())
	{
		return false;
	}
	// no affect so we just add it
	else if(!bHasAffect)
	{
		if(bSendMsg)
		{
			m_MsgManager.SendAffectMsg(m_AffectInfo[nAffect].m_strName,this,MSG_APPLY_AFFECT);
		}
		m_Affects[nAffect].Set(nPulses,nValue);
		if(m_AffectInfo[nAffect].m_pFncApply!=NULL)
		{
			(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,false);
		}
		return true;
	}
	// if the affect is manual and not timed then can't add the time!
	// b/c we use time variable to MANUAL_AFFECT ...and PERM_AFFECT
	else if(m_Affects[nAffect].IsManual())
	{
		int nOldTime = m_Affects[nAffect].m_nTime;
		int nOldValue= m_Affects[nAffect].m_nValue;
		if(m_AffectInfo[nAffect].m_bAcumValue)
		{
			//remove char affects for this affect
			if(m_AffectInfo[nAffect].m_pFncApply!=NULL)
			{
				(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,true);
			}
			//reset affect struct
			m_Affects[nAffect].Remove();
			//send messages
			if(bSendMsg)
			{
				m_MsgManager.SendAffectMsg(m_AffectInfo[nAffect].m_strName,this,MSG_APPLY_AFFECT);
			}
			//reset with new affects
			m_Affects[nAffect].Set(nOldTime,nOldValue+nValue);
			//reset char affects
			if(m_AffectInfo[nAffect].m_pFncApply!=NULL)
			{
				(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,false);
			}
			return true;
		}
	}
	//other if we already have the affect but it's not a perm affect
	//so we can add to it
	else if(!m_Affects[nAffect].IsPermAffect())
	{
		int nOldTime = m_Affects[nAffect].m_nTime;
		int nOldValue= m_Affects[nAffect].m_nValue;
		if(m_AffectInfo[nAffect].m_bAcumTime && m_AffectInfo[nAffect].m_bAcumValue)
		{
			//remove the affect to char
			if(m_AffectInfo[nAffect].m_pFncApply!=NULL)
			{
				(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,true);
			}
			//remove old values
			m_Affects[nAffect].Remove();
			//send msg
			if(bSendMsg)
			{
				m_MsgManager.SendAffectMsg(m_AffectInfo[nAffect].m_strName,this,MSG_APPLY_AFFECT);
			}
			//put in new values
			m_Affects[nAffect].Set(nOldTime+nPulses,nValue+nOldValue);
			//apply new char affects if any
			if(m_AffectInfo[nAffect].m_pFncApply!=NULL)
			{
				(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,false);
			}
			return true;
		}
		else if(m_AffectInfo[nAffect].m_bAcumTime && !m_AffectInfo[nAffect].m_bAcumValue)
		{
			//remove affect
			m_Affects[nAffect].Remove();
			//send messages
			if(bSendMsg)
                        {
                                m_MsgManager.SendAffectMsg(m_AffectInfo[nAffect].m_strName,this,MSG_APPLY_AFFECT);
                        }
			//reset it with new time and old value
			m_Affects[nAffect].Set(nOldTime+nPulses,nOldValue);
			//only changing time so don't add char affects
			return true;
		}
		else if(!m_AffectInfo[nAffect].m_bAcumTime && m_AffectInfo[nAffect].m_bAcumValue)
		{
			//remove char affects for this affect
			if(m_AffectInfo[nAffect].m_pFncApply!=NULL)
			{
				(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,true);
			}
			//reset affect struct
			m_Affects[nAffect].Remove();
			//send messages
			if(bSendMsg)
			{
				m_MsgManager.SendAffectMsg(m_AffectInfo[nAffect].m_strName,this,MSG_APPLY_AFFECT);
			}
			//reset with new affects
			m_Affects[nAffect].Set(nOldTime,nOldValue+nValue);
			//reset char affects
			if(m_AffectInfo[nAffect].m_pFncApply!=NULL)
			{
				(this->*m_AffectInfo[nAffect].m_pFncApply)(nAffect,false);
			}
			return true;
		}
	}
	return false;
}

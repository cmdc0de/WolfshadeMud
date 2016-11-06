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
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "lich.reference.h"

CLichReference::~CLichReference()
{
	//do nothing
}

CLichReference::CLichReference() : CRacialReference()
{
	SetAll();
	m_nSize = SIZE_MEDIUM;
	m_lStartRoom = GEN_START_ROOM;
	//build our racial based wear positions
	//ONLY HAND POSITIONS CAN HAVE MULTI-BITS
	//SHIELD SHOULD NOT BE WITH PRIMARY HAND
	m_nWearPositions [0]= WEAR_POS_BADGE;
	m_nWearPositions [1]= WEAR_POS_HEAD;
	m_nWearPositions [2]= WEAR_POS_EAR;
	m_nWearPositions [3]= WEAR_POS_EAR;
	m_nWearPositions [4]= WEAR_POS_EYES;
	m_nWearPositions [5]= WEAR_POS_FACE;
	m_nWearPositions [6]= WEAR_POS_NECK;
	m_nWearPositions [7]= WEAR_POS_NECK;
	m_nWearPositions [8]= WEAR_POS_BODY;
	m_nWearPositions [9]= WEAR_POS_ABOUT_BODY;
	m_nWearPositions [10] = WEAR_POS_ON_BACK;
	m_nWearPositions [11]= WEAR_POS_ARMS;
	m_nWearPositions [12]= WEAR_POS_WRIST;
	m_nWearPositions [13]= WEAR_POS_WRIST;
	m_nWearPositions [14]= WEAR_POS_RING;
	m_nWearPositions [15]= WEAR_POS_RING;
	m_nWearPositions [16]= WEAR_POS_HANDS;
	m_nWearPositions [17] = WEAR_POS_WIELD | WEAR_POS_WIELD_TWO_HANDED | WEAR_POS_HOLD;
	m_nWearPositions [18]=  WEAR_POS_SHIELD | WEAR_POS_HOLD | WEAR_POS_DUAL_WIELD;
	m_nWearPositions [19]= WEAR_POS_WAIST;
	m_nWearPositions [20]= WEAR_POS_LEGS;
	m_nWearPositions [21]= WEAR_POS_FEET; //22
	CalcTotalWeaponPositions();
}

void CLichReference::SetCharisma()
{
	m_sCharisma[0] = sCharisma(-10,10,10); //0-9
	m_sCharisma[1] = sCharisma(0,0,0); //10-19
	m_sCharisma[2] = sCharisma(0,0,0); //20-29
	m_sCharisma[3] = sCharisma(0,0,0); //30-39
	m_sCharisma[4] = sCharisma(0,0,0); //40-49
	m_sCharisma[5] = sCharisma(0,0,0); //50-59
	m_sCharisma[6] = sCharisma(0,0,0); //60-69
	m_sCharisma[7] = sCharisma(0,0,0); //70-79
	m_sCharisma[8] = sCharisma(0,0,0); //80-89
	m_sCharisma[9] = sCharisma(10,10,10); //90-99
	m_sCharisma[10] = sCharisma(15,15,15); //100-109
	m_sCharisma[11] = sCharisma(20,20,20); //110-119
	m_sCharisma[12] = sCharisma(22,22,22); //120
}

void CLichReference::SetIntellegence()
{
	m_sIntellegence[0] = sIntellegence(-30,3,-3); //0-9
	m_sIntellegence[1] = sIntellegence(-20,2,-2); //10-19
	m_sIntellegence[2] = sIntellegence(-1,1,-1); //20-29
	m_sIntellegence[3] = sIntellegence(0,0,0); //30-39
	m_sIntellegence[4] = sIntellegence(0,0,0); //40-49
	m_sIntellegence[5] = sIntellegence(0,0,0); //50-59
	m_sIntellegence[6] = sIntellegence(10,-2,2); //60-69
	m_sIntellegence[7] = sIntellegence(20,-3,3); //70-79
	m_sIntellegence[8] = sIntellegence(30,-4,4); //80-89
	m_sIntellegence[9] = sIntellegence(40,-5,5); //90-99
	m_sIntellegence[10] = sIntellegence(50,-6,6); //100-109
	m_sIntellegence[11] = sIntellegence(60,-7,7); //110-119
	m_sIntellegence[12] = sIntellegence(62,-8,8); //120
}

void CLichReference::SetWisdom()
{
	m_sWisdom[0] = sWisdom(-20,3,-3); //0-9
	m_sWisdom[1] = sWisdom(-10,2,-2); //10-19
	m_sWisdom[2] = sWisdom(-5,1,-1); //20-29
	m_sWisdom[3] = sWisdom(0,0,0); //30-39
	m_sWisdom[4] = sWisdom(0,0,0); //40-49
	m_sWisdom[5] = sWisdom(0,0,0); //50-59
	m_sWisdom[6] = sWisdom(10,-1,3); //60-69
	m_sWisdom[7] = sWisdom(20,-2,4); //70-79
	m_sWisdom[8] = sWisdom(30,-3,5); //80-89
	m_sWisdom[9] = sWisdom(40,-4,6); //90-99
	m_sWisdom[10] = sWisdom(45,-5,7); //100-109
	m_sWisdom[11] = sWisdom(50,-6,8); //110-119
	m_sWisdom[12] = sWisdom(52,-7,9); //120
}

void CLichReference::SetPower()
{
	m_sPower[0] = sPower(0,2); //0-9
	m_sPower[1] = sPower(0,3); //10-19
	m_sPower[2] = sPower(5,4); //20-29
	m_sPower[3] = sPower(15,5); //30-39
	m_sPower[4] = sPower(15,5); //40-49
	m_sPower[5] = sPower(15,5); //50-59
	m_sPower[6] = sPower(15,5); //60-69
	m_sPower[7] = sPower(25,6); //70-79
	m_sPower[8] = sPower(35,7); //80-89
	m_sPower[9] = sPower(45,8); //90-99
	m_sPower[10] = sPower(55,9); //100-109
	m_sPower[11] = sPower(60,10); //110-119
	m_sPower[12] = sPower(65,11); //120
}

void CLichReference::SetConstitution()
{
	m_sConstitution[0] = sConstitution(1,0,-20); //0-9
	m_sConstitution[1] = sConstitution(2,0,-10); //10-19
	m_sConstitution[2] = sConstitution(3,1,10); //20-29
	m_sConstitution[3] = sConstitution(4,2,10); //30-39
	m_sConstitution[4] = sConstitution(5,2,10); //40-49
	m_sConstitution[5] = sConstitution(6,2,10); //50-59
	m_sConstitution[6] = sConstitution(7,2,20); //60-69
	m_sConstitution[7] = sConstitution(8,2,30); //70-79
	m_sConstitution[8] = sConstitution(9,3,40); //80-89
	m_sConstitution[9] = sConstitution(10,4,50); //90-99
	m_sConstitution[10] = sConstitution(11,5,60); //100-109
	m_sConstitution[11] = sConstitution(12,6,70); //110-119
	m_sConstitution[12] = sConstitution(13,7,72); //120
}

void CLichReference::SetInnateRace()
{
	m_sInnateRace.m_nInnate_ablities = INNATE_DAY_SIGHT | INNATE_ULTRAVISION;
	m_sInnateRace.m_nMinHeight = 50;
	m_sInnateRace.m_nMaxHeight = 80;
	m_sInnateRace.m_nMinWeight = 100;
	m_sInnateRace.m_nMaxWeight = 150;
	m_sInnateRace.m_nMinAge = 1;
	m_sInnateRace.m_nMaxAge = 1000;
	m_sInnateRace.m_nStartingAlignment = -1000;
	m_sInnateRace.m_nRaceWar = RACE_EVIL;
	//Saves the more negative the number better the save
	//Range for this value should be 100 to -100
	m_sInnateRace.m_pnSaves[CAttributes::SAVE_BREATH] = -20;
	m_sInnateRace.m_pnSaves[CAttributes::SAVE_PARA] = -50;
	m_sInnateRace.m_pnSaves[CAttributes::SAVE_SPELL] = -60;
	m_sInnateRace.m_pnSaves[CAttributes::SAVE_POISON] = -20;
	PopulateInnateString(m_strInnates);
}

void CLichReference::SetStrength()
{
	m_sStrength[0] = sStrength(-3,10,-30); //0-9
	m_sStrength[1] = sStrength(-2,20,-20); //10-19
	m_sStrength[2] = sStrength(-1,30,-10); //20-29
	m_sStrength[3] = sStrength(0,40,0); //30-39
	m_sStrength[4] = sStrength(0,60,0); //40-49
	m_sStrength[5] = sStrength(0,80,0); //50-59
	m_sStrength[6] = sStrength(0,110,0); //60-69
	m_sStrength[7] = sStrength(0,140,0); //70-79
	m_sStrength[8] = sStrength(0,170,0); //80-89
	m_sStrength[9] = sStrength(0,210,0); //90-99
	m_sStrength[10] = sStrength(1,250,5); //90-99
	m_sStrength[11] = sStrength(2,300,10); //110-119
	m_sStrength[12] = sStrength(2,350,15); //120
}

void CLichReference::SetDexterity()
{
	m_sDexterity[0] = sDexterity(0,-20,-30,-30); //0-9
	m_sDexterity[1] = sDexterity(0,-10,-20,-20); //10-19
	m_sDexterity[2] = sDexterity(0,0,-10,-10); //20-29
	m_sDexterity[3] = sDexterity(0,0,0,0); //30-39
	m_sDexterity[4] = sDexterity(0,0,0,0); //40-49
	m_sDexterity[5] = sDexterity(0,0,0,0); //50-59
	m_sDexterity[6] = sDexterity(0,0,0,0); //60-69
	m_sDexterity[7] = sDexterity(10,0,0,0); //70-79
	m_sDexterity[8] = sDexterity(20,0,0,0); //80-89
	m_sDexterity[9] = sDexterity(30,0,0,0); //90-99
	m_sDexterity[10] = sDexterity(40,0,0,0); //90-99
	m_sDexterity[11] = sDexterity(50,0,10,10); //110-119
	m_sDexterity[12] = sDexterity(52,0,20,20); //120
}

void CLichReference::SetAgility()
{
	m_sAgility[0] = sAgility(0,-20,-20,-30,-30); //0-9
	m_sAgility[1] = sAgility(0,-10,-10,-20,-20); //10-19
	m_sAgility[2] = sAgility(1,0,0,-10,-10); //20-29
	m_sAgility[3] = sAgility(2,0,0,0,0); //30-39
	m_sAgility[4] = sAgility(2,0,0,0,0); //40-49
	m_sAgility[5] = sAgility(2,0,0,0,0); //50-59
	m_sAgility[6] = sAgility(2,0,0,0,0); //60-69
	m_sAgility[7] = sAgility(2,0,0,0,0); //70-79
	m_sAgility[8] = sAgility(3,0,0,10,10); //80-89
	m_sAgility[9] = sAgility(4,0,0,20,20); //90-99
	m_sAgility[10] = sAgility(5,0,0,30,30); //100-109
	m_sAgility[11] = sAgility(6,10,10,40,40); //110-119
	m_sAgility[12] = sAgility(7,22,22,42,42); //120
}

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
#include "gnoll.reference.h"

CGnollReference::~CGnollReference() {
  //do nothing
}

CGnollReference::CGnollReference() : CRacialReference() {
  SetAll();
  m_nSize = SIZE_MEDIUM;
  m_lStartRoom = GNOLL_START_ROOM;
  //build our racial based wear positions
  //ONLY HAND POSITIONS CAN HAVE MULTI-BITS
  //SHIELD SHOULD NOT BE WITH PRIMARY HAND
  m_nWearPositions [0] = WEAR_POS_BADGE;
  m_nWearPositions [1] = WEAR_POS_HEAD;
  m_nWearPositions [2] = WEAR_POS_EAR;
  m_nWearPositions [3] = WEAR_POS_EAR;
  m_nWearPositions [4] = WEAR_POS_EYES;
  m_nWearPositions [5] = WEAR_POS_FACE;
  m_nWearPositions [6] = WEAR_POS_NECK;
  m_nWearPositions [7] = WEAR_POS_NECK;
  m_nWearPositions [8] = WEAR_POS_BODY;
  m_nWearPositions [9] = WEAR_POS_ABOUT_BODY;
  m_nWearPositions [10] = WEAR_POS_ON_BACK;
  m_nWearPositions [11] = WEAR_POS_ARMS;
  m_nWearPositions [12] = WEAR_POS_WRIST;
  m_nWearPositions [13] = WEAR_POS_WRIST;
  m_nWearPositions [14] = WEAR_POS_RING;
  m_nWearPositions [15] = WEAR_POS_RING;
  m_nWearPositions [16] = WEAR_POS_HANDS;
  m_nWearPositions [17] = WEAR_POS_WIELD | WEAR_POS_HOLD | WEAR_POS_WIELD_TWO_HANDED;
  m_nWearPositions [18] = WEAR_POS_SHIELD | WEAR_POS_HOLD | WEAR_POS_DUAL_WIELD;
  m_nWearPositions [19] = WEAR_POS_WAIST;
  m_nWearPositions [20] = WEAR_POS_LEGS;
  m_nWearPositions [21] = WEAR_POS_FEET; //22
  CalcTotalWeaponPositions();
}

void CGnollReference::SetCharisma() {
  m_sCharisma[0] = sCharisma(-10, 10, 10); //0-9
  m_sCharisma[1] = sCharisma(0, 0, 0); //10-19
  m_sCharisma[2] = sCharisma(0, 0, 0); //20-29
  m_sCharisma[3] = sCharisma(0, 0, 0); //30-39
  m_sCharisma[4] = sCharisma(0, 0, 0); //40-49
  m_sCharisma[5] = sCharisma(0, 0, 0); //50-59
  m_sCharisma[6] = sCharisma(0, 0, 0); //60-69
  m_sCharisma[7] = sCharisma(0, 0, 0); //70-79
  m_sCharisma[8] = sCharisma(0, 0, 0); //80-89
  m_sCharisma[9] = sCharisma(10, 10, 10); //90-99
  m_sCharisma[10] = sCharisma(20, 20, 20); //100-109
  m_sCharisma[11] = sCharisma(30, 30, 30); //110-119
  m_sCharisma[12] = sCharisma(30, 30, 30); //120
}

void CGnollReference::SetIntellegence() {
  m_sIntellegence[0] = sIntellegence(-30, 3, -3); //0-9
  m_sIntellegence[1] = sIntellegence(-20, 2, -2); //10-19
  m_sIntellegence[2] = sIntellegence(-10, 1, -1); //20-29
  m_sIntellegence[3] = sIntellegence(0, 0, 0); //30-39
  m_sIntellegence[4] = sIntellegence(0, 0, 0); //40-49
  m_sIntellegence[5] = sIntellegence(0, 0, 0); //50-59
  m_sIntellegence[6] = sIntellegence(0, 0, 0); //60-69
  m_sIntellegence[7] = sIntellegence(0, 0, 0); //70-79
  m_sIntellegence[8] = sIntellegence(0, 0, 0); //80-89
  m_sIntellegence[9] = sIntellegence(10, 0, 1); //90-99
  m_sIntellegence[10] = sIntellegence(20, 0, 2); //100-109
  m_sIntellegence[11] = sIntellegence(20, 0, 2); //110-119
  m_sIntellegence[12] = sIntellegence(30, 0, 3); //120
}

void CGnollReference::SetWisdom() {
  m_sWisdom[0] = sWisdom(-30, 3, -3); //0-9
  m_sWisdom[1] = sWisdom(-20, 2, -2); //10-19
  m_sWisdom[2] = sWisdom(-10, 1, -1); //20-29
  m_sWisdom[3] = sWisdom(0, 0, 0); //30-39
  m_sWisdom[4] = sWisdom(0, 0, 0); //40-49
  m_sWisdom[5] = sWisdom(0, 0, 0); //50-59
  m_sWisdom[6] = sWisdom(0, 0, 0); //60-69
  m_sWisdom[7] = sWisdom(0, 0, 0); //70-79
  m_sWisdom[8] = sWisdom(0, 0, 0); //80-89
  m_sWisdom[9] = sWisdom(10, -1, 1); //90-99
  m_sWisdom[10] = sWisdom(20, -2, 2); //100-109
  m_sWisdom[11] = sWisdom(20, -2, 2); //110-119
  m_sWisdom[12] = sWisdom(30, -3, 3); //120
}

void CGnollReference::SetPower() {
  m_sPower[0] = sPower(-30, 0); //0-9
  m_sPower[1] = sPower(-20, 0); //10-19
  m_sPower[2] = sPower(-10, 0); //20-29
  m_sPower[3] = sPower(0, 0); //30-39
  m_sPower[4] = sPower(0, 0); //40-49
  m_sPower[5] = sPower(0, 0); //50-59
  m_sPower[6] = sPower(0, 0); //60-69
  m_sPower[7] = sPower(0, 0); //70-79
  m_sPower[8] = sPower(10, 0); //80-89
  m_sPower[9] = sPower(20, 0); //90-99
  m_sPower[10] = sPower(30, 0); //100-109
  m_sPower[11] = sPower(30, 0); //110-119
  m_sPower[12] = sPower(32, 0); //120
}

void CGnollReference::SetConstitution() {
  m_sConstitution[0] = sConstitution(0, -2, -10); //0-9
  m_sConstitution[1] = sConstitution(0, -1, 0); //10-19
  m_sConstitution[2] = sConstitution(0, 0, 0); //20-29
  m_sConstitution[3] = sConstitution(0, 0, 0); //30-39
  m_sConstitution[4] = sConstitution(0, 0, 0); //40-49
  m_sConstitution[5] = sConstitution(0, 0, 0); //50-59
  m_sConstitution[6] = sConstitution(0, 0, 0); //60-69
  m_sConstitution[7] = sConstitution(2, 1, 0); //70-79
  m_sConstitution[8] = sConstitution(4, 2, 10); //80-89
  m_sConstitution[9] = sConstitution(6, 3, 20); //90-99
  m_sConstitution[10] = sConstitution(8, 4, 30); //100-109
  m_sConstitution[11] = sConstitution(10, 5, 40); //110-119
  m_sConstitution[12] = sConstitution(11, 6, 42); //120
}

void CGnollReference::SetInnateRace() {
  m_sInnateRace.m_nInnate_ablities = INNATE_ULTRAVISION |
	  INNATE_DAY_SIGHT | INNATE_BITE;
  m_sInnateRace.m_nMinHeight = 60;
  m_sInnateRace.m_nMaxHeight = 80;
  m_sInnateRace.m_nMinWeight = 100;
  m_sInnateRace.m_nMaxWeight = 250;
  m_sInnateRace.m_nMinAge = 18;
  m_sInnateRace.m_nMaxAge = 90;
  m_sInnateRace.m_nStartingAlignment = -1000;
  m_sInnateRace.m_nRaceWar = RACE_EVIL;
  //Saves the more negative the number better the save
  //Range for this value should be 100 to -100
  m_sInnateRace.m_pnSaves[CAttributes::SAVE_BREATH] = 0;
  m_sInnateRace.m_pnSaves[CAttributes::SAVE_PARA] = 0;
  m_sInnateRace.m_pnSaves[CAttributes::SAVE_SPELL] = 0;
  m_sInnateRace.m_pnSaves[CAttributes::SAVE_POISON] = 0;
  PopulateInnateString(m_strInnates);
}

void CGnollReference::SetStrength() {
  m_sStrength[0] = sStrength(0, 10, -10); //0-9
  m_sStrength[1] = sStrength(0, 20, 0); //10-19
  m_sStrength[2] = sStrength(0, 40, 0); //20-29
  m_sStrength[3] = sStrength(1, 60, 0); //30-39
  m_sStrength[4] = sStrength(1, 80, 0); //40-49
  m_sStrength[5] = sStrength(1, 100, 0); //50-59
  m_sStrength[6] = sStrength(1, 120, 0); //60-69
  m_sStrength[7] = sStrength(2, 150, 0); //70-79
  m_sStrength[8] = sStrength(3, 180, 10); //80-89
  m_sStrength[9] = sStrength(4, 200, 20); //90-99
  m_sStrength[10] = sStrength(5, 230, 30); //90-99
  m_sStrength[11] = sStrength(5, 300, 30); //110-119
  m_sStrength[12] = sStrength(6, 350, 35); //120
}

void CGnollReference::SetDexterity() {
  m_sDexterity[0] = sDexterity(-10, -10, -20, -20); //0-9
  m_sDexterity[1] = sDexterity(0, 0, -10, -10); //10-19
  m_sDexterity[2] = sDexterity(0, 0, 0, 0); //20-29
  m_sDexterity[3] = sDexterity(0, 0, 0, 0); //30-39
  m_sDexterity[4] = sDexterity(0, 0, 0, 0); //40-49
  m_sDexterity[5] = sDexterity(0, 0, 0, 0); //50-59
  m_sDexterity[6] = sDexterity(0, 0, 0, 0); //60-69
  m_sDexterity[7] = sDexterity(0, 0, 0, 0); //70-79
  m_sDexterity[8] = sDexterity(0, 0, 0, 0); //80-89
  m_sDexterity[9] = sDexterity(10, 10, 10, 10); //90-99
  m_sDexterity[10] = sDexterity(20, 20, 20, 20); //90-99
  m_sDexterity[11] = sDexterity(30, 30, 30, 30); //110-119
  m_sDexterity[12] = sDexterity(40, 40, 40, 40); //120
}

void CGnollReference::SetAgility() {
  m_sAgility[0] = sAgility(0, -10, -10, -20, -20); //0-9
  m_sAgility[1] = sAgility(0, 0, 0, -20, -20); //10-19
  m_sAgility[2] = sAgility(0, 0, 0, 0, 0); //20-29
  m_sAgility[3] = sAgility(0, 0, 0, 0, 0); //30-39
  m_sAgility[4] = sAgility(0, 0, 0, 0, 0); //40-49
  m_sAgility[5] = sAgility(0, 0, 0, 0, 0); //50-59
  m_sAgility[6] = sAgility(0, 0, 0, 0, 0); //60-69
  m_sAgility[7] = sAgility(1, 0, 0, 0, 0); //70-79
  m_sAgility[8] = sAgility(1, 10, 10, 10, 10); //80-89
  m_sAgility[9] = sAgility(2, 20, 20, 20, 20); //90-99
  m_sAgility[10] = sAgility(3, 30, 30, 30, 30); //100-109
  m_sAgility[11] = sAgility(3, 40, 40, 40, 40); //110-119
  m_sAgility[12] = sAgility(4, 45, 45, 45, 45); //120
}
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
/*
 *
 *  Mob implementation cpp
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"

#include <sstream>
#include <ctype.h>
#include "mob.prototype.h"
#include "log.h"

extern CRandom GenNum;
extern CGlobalVariableManagement GVM;

short CMobPrototype::STARTING_MOB_STAT = 70;

//function reads in a mob and stores it.
//we throw any error

void CMobPrototype::ReadMob(CAscii &Infile) {
   CString temp(MAX_FILE_STRING_SIZE);
   int counter = 0;
   char flags[33] = {'\0'};
   char flags1[33] = {'\0'};
   CString Error;

   Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
   if (temp.Compare((char *) "ENDFILE")) {
	if (Infile.rdstate() != std::ios_base::eofbit) {
	   Infile.clear(std::ios_base::eofbit);
	}
	CError Err(NONERROR, "ENDFILE marker hit");
	throw &Err;
   }
   std::string s(temp.cptr(),2048);
   std::istringstream one(s);
   one >> Vnum;
   //don't even read the mob in if we don't have a vnum
   if (!one.good()) {
	CError Err(NON_CRITICAL_ERROR, "Mob has NO VNUM!\r\n");
	throw &Err;
   }
   Infile.ReadFileString(MobAlias, '~');
   if (MobAlias.IsEmpty()) {
	ErrorLog << "Mob has no Aliases!";
   }
   Infile.ReadFileString(MobName, '~');
   Infile.ReadFileString(DefaultPosDesc, '~');
   Infile.ReadFileString(MobDescription, '~');
   Infile.ReadFileString(MobLongDescription, '~');
   Infile.Numeric("%s %s %b", flags, flags1, &Alignment);
   MobFlags = Infile.AsciiConvert(flags);
   //logic check
   if ((MobFlags & CNPC::TRACK) && !(MobFlags & CNPC::MEMORY)) {
	ErrorLog << "Mob #: " << Vnum << " has track bit set but not memory bit...are you sure that's what you want?" << endl;
   }
   m_nProficiencySphere = Infile.AsciiConvert(flags1);
   memset(flags, '\0', 33);
   memset(flags1, '\0', 33);
   Infile.Numeric("%b %b %b", &Race, &ClassType, &Level);
   Infile.Numeric("%b %b %d", &Thaco, &AC, &MinHits);
   Infile.Numeric("%b %b %b", &NumOfDice, &DiceType, &PlusDam);

   Infile.Numeric("%l %d %s %s %b", &Copper, &Experience, flags, flags1, &Sex);
   LoadPos = Infile.AsciiConvert(flags);
   DefaultPos = Infile.AsciiConvert(flags1);

   Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);

   while (temp != (char*) "ENDMOB") {
	Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
   }
}

//zero out everything

CMobPrototype::CMobPrototype() {
   for (int i = 0; i < MAX_NUMBER_OF_STATS; i++)
	Stats[i] = STARTING_MOB_STAT;
   NumInGame = 0;
   m_nProficiencySphere = 0;
}

CMobPrototype::~CMobPrototype() {

}

///////////////////////////////////////
//	VStat
//	Builds String to send to character
//	written by: Demetrius Comes
//	4/27/98
//////////////////////////////////////

static const char *strY = "Y";
static const char *strN = "N";

void CMobPrototype::VStat(CString & ToChar) {
   static CString strFormat(
	   "Name: %s       Race: %s       Class: %s\r\n"
	   "Alignment: %d  Level: %d      Sex:  %s\r\n"
	   "AC: %d/100     Experience: %d Thaco: %d\r\n"
	   "Mob Alias: %s\r\n"
	   "Mob Description: %s \r\n"
	   "Mob Default Desc Position: %s \r\n"
	   "Sentinel:   %s	Scavenger:   %s		Aware: 			%s\r\n"
	   "Aggressive: %s	StayZone:    %s		Flee:  			%s\r\n"
	   "AggroEvil:  %s	AggroGood:   %s		AggroNuetral: 	%s\r\n"
	   "Memory:     %s 	AggroEvilR:  %s		AggroGoodR: 	%s\r\n"
	   "AggroNueR:  %s	AggroAlienR: %s		NoBase: 		%s\r\n"
	   "Track:	    %s	Teacher:     %s		GuardGood: 		%s\r\n"
	   "GuardEvil:  %s	GuardNeutral:%s		GuardNPC:		%s\r\n"
	   "GuardChr:   %s	GuardRace:   %s		GuardGoodRace:  %s\r\n"
	   "GuardEvilR: %s	GuardNueR:   %s		GuardAlienRace: %s\r\n"
	   "PermDI:     %s\r\n"
		);

   ToChar.Format(strFormat.cptr(), MobName.cptr(), GVM.GetRaceName(Race),
	   GVM.GetClassName(ClassType), Alignment, Level, CCharacterAttributes::GetSexName(Sex),
	   AC, Experience, Thaco, MobAlias.cptr(), MobDescription.cptr(),
	   DefaultPosDesc.cptr()
	   ,((MobFlags&(1<<0))!=0?strY:strN)
	   ,((MobFlags&(1<<1))!=0?strY:strN)
	   ,((MobFlags&(1<<2))!=0?strY:strN)
	   ,((MobFlags&(1<<3))!=0?strY:strN)
	   ,((MobFlags&(1<<4))!=0?strY:strN)
	   ,((MobFlags&(1<<5))!=0?strY:strN)
	   ,((MobFlags&(1<<6))!=0?strY:strN)
	   ,((MobFlags&(1<<7))!=0?strY:strN)
	   ,((MobFlags&(1<<8))!=0?strY:strN)
	   ,((MobFlags&(1<<9))!=0?strY:strN)
	   ,((MobFlags&(1<<10))!=0?strY:strN)
	   ,((MobFlags&(1<<11))!=0?strY:strN)
	   ,((MobFlags&(1<<12))!=0?strY:strN)
	   ,((MobFlags&(1<<13))!=0?strY:strN)
	   ,((MobFlags&(1<<14))!=0?strY:strN)
	   ,((MobFlags&(1<<15))!=0?strY:strN)
	   ,((MobFlags&(1<<16))!=0?strY:strN)
	   ,((MobFlags&(1<<17))!=0?strY:strN)
	   ,((MobFlags&(1<<18))!=0?strY:strN)
	   ,((MobFlags&(1<<19))!=0?strY:strN)
	   ,((MobFlags&(1<<20))!=0?strY:strN)
	   ,((MobFlags&(1<<21))!=0?strY:strN)
	   ,((MobFlags&(1<<22))!=0?strY:strN)
	   ,((MobFlags&(1<<23))!=0?strY:strN)
	   ,((MobFlags&(1<<24))!=0?strY:strN)
	   ,((MobFlags&(1<<25))!=0?strY:strN)
	   ,((MobFlags&(1<<26))!=0?strY:strN)
	   ,((MobFlags&(1<<27))!=0?strY:strN)
	   );
}

//////////////////////////////////////
//	Overloaded Constructor
//	for loading

CMobPrototype::CMobPrototype(CMobPrototype * pMobProto) {
   Vnum = pMobProto->Vnum;
   MobAlias = pMobProto->MobAlias;
   MobName = pMobProto->MobName;
   DefaultPosDesc = pMobProto->DefaultPosDesc;
   MobDescription = pMobProto->MobDescription;
   MobLongDescription = pMobProto->MobLongDescription;
   MobFlags = pMobProto->MobFlags;
   Alignment = pMobProto->Alignment;
   ClassType = pMobProto->ClassType;
   Thaco = pMobProto->Thaco;
   Level = pMobProto->Level;
   AC = pMobProto->AC;
   Race = pMobProto->Race;
   Sex = pMobProto->Sex;
   MinHits = pMobProto->MinHits;
   Copper = pMobProto->Copper;
   DefaultPos = pMobProto->DefaultPos;
   LoadPos = pMobProto->LoadPos;
   m_nProficiencySphere = pMobProto->m_nProficiencySphere;
   Experience = pMobProto->Experience;
   NumOfDice = pMobProto->NumOfDice;
   DiceType = pMobProto->DiceType;
   PlusDam = pMobProto->PlusDam;
   NumInGame = 0;
   for (int i = 0; i < MAX_NUMBER_OF_STATS; i++) {
	Stats[i] = pMobProto->Stats[i];
   }
}

/////////////////////////
//	Make a Mob prototype from a character

CMobPrototype::CMobPrototype(CCharacter * pMirror) {
   AC = 100;
   MobName = pMirror->GetName();
   MobAlias = pMirror->GetName();
   MobDescription = "stands here";
   DefaultPosDesc.Format("%s stands here", MobName.cptr());
   Alignment = pMirror->GetAlignment();
   ClassType = pMirror->GetClass();
   Race = pMirror->GetRace();
   Copper = 0;
   DefaultPos = POS_STANDING;
   DiceType = 4;
   NumOfDice = 1;
   PlusDam = 0;
   Experience = 0;
   Level = 0;
   LoadPos = POS_STANDING;
   m_nProficiencySphere = 0;
   MinHits = 0;
   MobFlags = 0;
   NumInGame = 0;
   Sex = pMirror->GetSex();
   for (int i = 0; i < MAX_NUMBER_OF_STATS; i++) {
	Stats[i] = 75;
   }
   Thaco = pMirror->GetThaco();
   Vnum = VNUM_MIRAGE;
}


/////////////////////////
//	Make a Mob prototype from a character

CMobPrototype::CMobPrototype(const sUndeadInfo *p) {
   int i = 0;
   switch (p->m_nUndeadType) {
	case VNUM_UNDEAD_SKELETON:
	   AC = 100;
	   MobName = "&WA Skeleton&n";
	   MobAlias = "skeleton";
	   MobDescription = "&Wstands here with &Rred glowing &Weyes.&n";
	   DefaultPosDesc.Format("%s stands here with &Rred glowing &Weyes&n", MobName.cptr());
	   ClassType = CLASS_WARRIOR;
	   NumOfDice = 2;
	   DiceType = 4;
	   Level = p->m_nLevel;
	   m_nProficiencySphere = 0;
	   MinHits = 10;
	   MobFlags = 0;
	   PlusDam = 0;
	   Sex = NONE;
	   Thaco = 15;
	   break;
	case VNUM_UNDEAD_ZOMBIE:
	   AC = 75;
	   MobName = "&LA Zombie&n";
	   MobAlias = "zombie";
	   MobDescription = "&Lstands here.&n";
	   DefaultPosDesc.Format("%s &Lstands here with its flesh rotting away&n", MobName.cptr());
	   ClassType = CLASS_WARRIOR;
	   NumOfDice = 2;
	   DiceType = 5;
	   Level = p->m_nLevel;
	   m_nProficiencySphere = 0;
	   MinHits = 20;
	   MobFlags = 0;
	   PlusDam = 1;
	   Sex = NONE;
	   Thaco = 15;
	   break;
	case VNUM_UNDEAD_SPECTRE:
	   AC = 50;
	   MobName = "&LA S&wp&Wec&wtr&Le&n";
	   MobAlias = "spectre";
	   MobDescription = "&Lstands here.&n";
	   DefaultPosDesc.Format("%s &Lfloats here&n", MobName.cptr());
	   ClassType = CLASS_ROGUE;
	   DiceType = 4;
	   NumOfDice = 3;
	   Level = p->m_nLevel;
	   m_nProficiencySphere = 0;
	   MinHits = 200;
	   MobFlags = 0;
	   PlusDam = 2;
	   Sex = NONE;
	   Thaco = 10;
	   break;
	case VNUM_UNDEAD_GHOST:
	   AC = 25;
	   MobName = "&WA &wGhost&n";
	   MobAlias = "ghost";
	   MobDescription = "&wstands here.&n";
	   DefaultPosDesc.Format("%s &wfloats here&n", MobName.cptr());
	   ClassType = CLASS_ROGUE;
	   DiceType = 5;
	   NumOfDice = 4;
	   Level = p->m_nLevel;
	   m_nProficiencySphere = 0;
	   MinHits = 300;
	   MobFlags = 0;
	   PlusDam = 4;
	   Sex = NONE;
	   Thaco = 5;
	   break;
	case VNUM_UNDEAD_GHOUL:
	   AC = -50;
	   MobName = "&WA &LGhoul&n";
	   MobAlias = "ghoul";
	   MobDescription = "&Wstands here.&n";
	   DefaultPosDesc.Format("%s &Wfloats here&n", MobName.cptr());
	   ClassType = CLASS_WARRIOR;
	   DiceType = 5;
	   NumOfDice = 5;
	   Level = p->m_nLevel;
	   m_nProficiencySphere = 0;
	   MinHits = 400;
	   MobFlags = 0;
	   PlusDam = 5;
	   Sex = NONE;
	   Thaco = 0;
	   break;
	case VNUM_UNDEAD_WIGHT:
	   AC = -50;
	   MobName = "&WA &rWight&n";
	   MobAlias = "wight";
	   MobDescription = "&rstands here.&n";
	   DefaultPosDesc.Format("%s &rfloats here&n", MobName.cptr());
	   ClassType = CLASS_PSIONICIST;
	   DiceType = 6;
	   NumOfDice = 4;
	   Level = p->m_nLevel;
	   m_nProficiencySphere = 0;
	   MinHits = 400;
	   MobFlags = 0;
	   PlusDam = 1;
	   Sex = NONE;
	   Thaco = 0;
	   break;
	case VNUM_UNDEAD_WRAITH:
	   AC = -90;
	   MobName = "&LA &RWraith&n";
	   MobAlias = "wraith";
	   MobDescription = "&Rstands here.&n";
	   DefaultPosDesc.Format("%s &Lfloats here&n", MobName.cptr());
	   ClassType = CLASS_MAGE;
	   DiceType = 6;
	   NumOfDice = 6;
	   Level = p->m_nLevel;
	   m_nProficiencySphere = CMage::SPHERE_SWAMP;
	   MinHits = 500;
	   MobFlags = 0;
	   PlusDam = 5;
	   Sex = NONE;
	   Thaco = 0;
	   break;
	default:
	   ErrorLog << "Error in MobPrototype constructor trying to create undead" << endl;
	   break;
   }
   Vnum = p->m_nUndeadType;
   Alignment = -1000;
   Race = RACE_UNDEAD;
   Copper = 0;
   Experience = 0;
   LoadPos = POS_STANDING;
   DefaultPos = POS_STANDING;
   NumInGame = 0;
   for (i = 0; i < MAX_NUMBER_OF_STATS; i++)
	Stats[i] = STARTING_MOB_STAT;
}

/////////////////////////
//	Make a Mob prototype from a character

CMobPrototype::CMobPrototype(const sWallInfo *p) {
   CString strDefaultDesc;
   Race = RACE_NONE;
   MinHits = 10;
   switch (p->m_nWallType) {
	case VNUM_WALL_OF_STONE:
	   AC = 100;
	   MobName = "&LA Wall of Stone&n";
	   MobAlias = "stone";
	   MobDescription = "&LThis wall is made of solid stone.&n";
	   strDefaultDesc = "%s &Lstands here to the %s.&n";
	   break;
	case VNUM_WALL_OF_AIR:
	   AC = -100;
	   MobName = "&WA Wall of Air&n";
	   MobAlias = "air";
	   MobDescription = "&WThis wall is made of thick compacted air.&n";
	   strDefaultDesc = "%s &Wstands here to the %s.&n";
	   break;
	case VNUM_WALL_OF_BONES:
	   AC = 50;
	   MobName = "&LA Wall of &WBones&n";
	   MobAlias = "bones";
	   MobDescription = "&LThis wall is made of &Whumaniod bones&L.&n";
	   strDefaultDesc = "%s &Lstands here to the %s.&n";
	   break;
	case VNUM_WALL_OF_BRAMBLES:
	   AC = 50;
	   MobName = "&GA Wall of Brambles&n";
	   MobAlias = "brambles";
	   MobDescription = "&GThis wall is made of thick brambles.&n";
	   strDefaultDesc = "%s &Gstands here to the %s.&n";
	   Race = RACE_TROLL;
	   break;
	case VNUM_WALL_OF_FLAMES:
	   AC = 0;
	   MobName = "&RA Wall of Flames&n";
	   MobAlias = "bones";
	   MobDescription = "&RThis wall is made of hot flames.&n";
	   strDefaultDesc = "%s &Rstands here to the %s.&n";
	   break;
	case VNUM_WALL_OF_GLASS:
	   AC = 100;
	   MobName = "&WA Wall of &CGlass&n";
	   MobAlias = "glass";
	   MobDescription = "&WThis wall is made of solid &Cglass&W.&n";
	   strDefaultDesc = "%s &Wstands here to the %s.&n";
	   MinHits = 1000;
	   break;
	case VNUM_WALL_OF_ICE:
	   AC = 0;
	   MobName = "&WA Wall of &BIce&n";
	   MobAlias = "ice";
	   MobDescription = "&WThis wall is made of &Bsolid ice&W.&n";
	   strDefaultDesc = "%s &Wstands here to the %s.&n";
	   break;
	case VNUM_WALL_OF_SAND:
	   AC = -50;
	   MobName = "&yA Wall of sand&n";
	   MobAlias = "sand";
	   MobDescription = "&yThis wall is made of swirling sand.&n";
	   strDefaultDesc = "%s &ystands here to the %s.&n";
	   break;
	case VNUM_WALL_OF_WATER:
	   AC = 0;
	   MobName = "&BA Wall of water&n";
	   MobAlias = "water";
	   MobDescription = "&BThis wall is made of flowing water.&n";
	   strDefaultDesc = "%s &Bstands here to the %s.&n";
	   break;
	case VNUM_WALL_GUKS_HAND:
	   AC = -100;
	   MobName = "&YA huge interposing hand&n";
	   MobAlias = "hand huge";
	   MobDescription = "&YA huge hand blocks the way here.&n";
	   strDefaultDesc = "%s &Yblocks the way to the %s.&n";
	   break;
	default:
	   ErrorLog << "Unknow type of wall trying to be created" << endl;
	   assert(0);
	   break;
   }
   MobAlias += " wall ";
   MobAlias += CRoom::strDir[p->m_nDirection];
   DefaultPosDesc.Format(strDefaultDesc.cptr(), MobName.cptr(), CRoom::strDir[p->m_nDirection]);
   ClassType = CLASS_WARRIOR;
   DiceType = 2;
   Level = p->m_nLevel;
   m_nProficiencySphere = 0;
   MobFlags = CNPC::SENTINEL;
   PlusDam = 0;
   Sex = NONE;
   Thaco = 20;
   Vnum = p->m_nWallType;
   Alignment = 0;
   Copper = 0;
   Experience = 0;
   LoadPos = POS_STANDING;
   DefaultPos = POS_STANDING;
   NumInGame = 0;
   for (int i = 0; i < MAX_NUMBER_OF_STATS; i++)
	Stats[i] = STARTING_MOB_STAT;
}

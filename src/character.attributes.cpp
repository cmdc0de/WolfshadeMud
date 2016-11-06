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
 * CCharacterAttributes Implementation
 *
 *
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include <cstdio>
#include "save.char.h"
#include "object.save.h"
#include <algorithm>

extern CGlobalVariableManagement GVM;

#define NEWSFILE "files/news.txt"
#define MOTDFILE "files/motd.txt"

//fraginfo statics
short CCharacterAttributes::sFragInfo::ALIEN = 1;
short CCharacterAttributes::sFragInfo::GOOD = 2;
short CCharacterAttributes::sFragInfo::EVIL = 3;
short CCharacterAttributes::sFragInfo::NEUTRAL = 4;

//characterattributes statics
bool CCharacterAttributes::Initialized = false;
std::vector<CCharacterAttributes::sFragInfo> CCharacterAttributes::m_FragList;
CString CCharacterAttributes::NEWS;
CString CCharacterAttributes::MOTD;
CMap <CString, short> CCharacterAttributes::m_SkillNamesMap;
CString CCharacterAttributes::m_SkillNamesArray[MAX_MUD_SKILLS];
const CString CCharacterAttributes::m_strSelf("me self myself");
//player file
CMap<CString, long> CCharacterAttributes::m_PlayerFilePos;
const int CCharacterAttributes::PREFERS_NO_HASSLE = (1 << 0);
const int CCharacterAttributes::PREFERS_TELLS_WHEN_SWITCHED = (1 << 1);
const int CCharacterAttributes::PREFERS_COLOR = (1 << 2);
const int CCharacterAttributes::PREFERS_AUTO_EXIT = (1 << 3);
const int CCharacterAttributes::PREFERS_BRIEF = (1 << 4);
const int CCharacterAttributes::PREFERS_MAP_INSIDE = (1 << 5);
const int CCharacterAttributes::PREFERS_MAP_OUTSIDE = (1 << 6);
const int CCharacterAttributes::PREFERS_TELLS = (1 << 7);
const int CCharacterAttributes::PREFERS_GOD_LIGHT = (1 << 8); //allows god to see in dark
const int CCharacterAttributes::PREFERS_PETITION = (1 << 9);
const int CCharacterAttributes::PREFERS_GOD_CHAT = (1 << 10);
const int CCharacterAttributes::PREFERS_GOD_LOG_DEATHS = (1 << 11);
const int CCharacterAttributes::PREFERS_GOD_LOG_ZONES = (1 << 12);
const int CCharacterAttributes::PREFERS_GOD_SEES_ALL = (1 << 13); //allows god to see all names!
const int CCharacterAttributes::PREFERS_GOD_CONNECTION_LOG = (1 << 14);
const int CCharacterAttributes::PREFERS_GOD_ON_WHO = (1 << 15);
const int CCharacterAttributes::PREFERS_COMPACT = (1 << 16);
const int CCharacterAttributes::PREFERS_OOC = (1 << 17);
const int CCharacterAttributes::PREFERS_EXTRA_RETURNS = (1 << 18);
const int CCharacterAttributes::PREFRES_NO_PAGING = (1 << 19);
const int CCharacterAttributes::PREFERS_GOD_INVIS = (1 << 20);

const char *CCharacterAttributes::Pronouns[3] = {
   "its",
   "his",
   "her"
};

const char *CCharacterAttributes::SexNames[3] = {
   "It",
   "Male",
   "Female"
};

const char *CCharacterAttributes::SizeNames[TOTAL_SIZES] = {
   "*RESERVED*",
   "Tiny",
   "Small",
   "Medium",
   "Large",
   "Huge",
   "Giant"
};


static const char *PLAYER_FILE = "players/players.wolfshade";
FILE *mPlayerFile = 0;

/////////////////////////////////////////////////
//	statics functions

void CCharacterAttributes::InitStatics() {
   if (Initialized) {
	return;
   }
   Initialized = true;
   FILE *testFile = fopen(PLAYER_FILE,"rb");
   if(!testFile) {
	testFile = fopen(PLAYER_FILE,"wb+");
   }
   fclose(testFile);
   mPlayerFile = fopen(PLAYER_FILE, "rb+");
   assert(mPlayerFile);
   InitSkillNames();
   BootPlayers();
   ReadNews();
   ReadMOTD();
}

/////////////////////////
//	Associate skill names with numbers
////////////////////////

bool CCharacterAttributes::ShouldBeImp() {
   fseek(mPlayerFile, 0, SEEK_END);
   return 0 == ftell(mPlayerFile);
}

void CCharacterAttributes::InitSkillNames() {
   m_SkillNamesMap.Add("APPLY POISON", SKILL_APPLY_POISON);
   m_SkillNamesMap.Add("APPRAISE", SKILL_APPRAISE);
   m_SkillNamesMap.Add("ARMOR REPAIR", SKILL_ARMOR_REPAIR);
   m_SkillNamesMap.Add("AWARENESS", SKILL_AWARENESS);
   m_SkillNamesMap.Add("BARE HAND FIGHTING", SKILL_BARE_HAND_FIGHTING);
   m_SkillNamesMap.Add("BATTLE READINESS", SKILL_BATTLE_READINESS);
   m_SkillNamesMap.Add("BEAR HUG", SKILL_BEAR_HUG);
   m_SkillNamesMap.Add("BEG", SKILL_BEG);
   m_SkillNamesMap.Add("BERSERK", SKILL_BERSERK);
   m_SkillNamesMap.Add("BLITHER", SKILL_BLITHER);
   m_SkillNamesMap.Add("BRIBE", SKILL_BRIBE);
   m_SkillNamesMap.Add("CALLED SHOT", SKILL_CALLED_SHOT);
   m_SkillNamesMap.Add("CAMOUFLAGE", SKILL_CAMOUFLAGE);
   m_SkillNamesMap.Add("CAPTURE", SKILL_CAPTURE);
   m_SkillNamesMap.Add("CHANT", SKILL_CHANT);
   m_SkillNamesMap.Add("CHANT DEFENSE", SKILL_CHANT_DEFENSE);
   m_SkillNamesMap.Add("CHANT HONOR", SKILL_CHANT_HONOR);
   m_SkillNamesMap.Add("CHANT REGENERATION", SKILL_CHANT_REGENERATION);
   m_SkillNamesMap.Add("CIRCLE", SKILL_CIRCLE);
   m_SkillNamesMap.Add("CLIMB WALLS", SKILL_CLIMB_WALLS);
   m_SkillNamesMap.Add("CHARM", SKILL_CHARM);
   m_SkillNamesMap.Add("DIRT TOSS", SKILL_DIRT_TOSS);
   m_SkillNamesMap.Add("DISARM", SKILL_DISARM);
   m_SkillNamesMap.Add("DIVERSION", SKILL_DIVERSION);
   m_SkillNamesMap.Add("DOUBLE ATTACK", SKILL_DOUBLE_ATTACK);
   m_SkillNamesMap.Add("DOUBLE BACKSTAB", SKILL_DOUBLE_BACKSTAB);
   m_SkillNamesMap.Add("DRAGON PUNCH", SKILL_DRAGON_PUNCH);
   m_SkillNamesMap.Add("FAR SIGHT", SKILL_FAR_SIGHT);
   m_SkillNamesMap.Add("FAST PREP", SKILL_FAST_PREP);
   m_SkillNamesMap.Add("FORGERY", SKILL_FORGERY);
   m_SkillNamesMap.Add("GAMBLE", SKILL_GAMBLE);
   m_SkillNamesMap.Add("HEADBUTT", SKILL_HEAD_BUTT);
   m_SkillNamesMap.Add("HEROISM", SKILL_HEROISM);
   m_SkillNamesMap.Add("HIT ALL", SKILL_HIT_ALL);
   m_SkillNamesMap.Add("HONE WEAPON", SKILL_HONE_WEAPON);
   m_SkillNamesMap.Add("INSTANT KILL", SKILL_INSTANT_KILL);
   m_SkillNamesMap.Add("INTIMIDATE", SKILL_INTIMIDATE);
   m_SkillNamesMap.Add("JUGGLING", SKILL_JUGGLING);
   m_SkillNamesMap.Add("JUMPING", SKILL_JUMPING);
   m_SkillNamesMap.Add("KNIFE TURN", SKILL_KNIFE_TURN);
   m_SkillNamesMap.Add("KNIGHT CODE", SKILL_KNIGHT_CODE);
   m_SkillNamesMap.Add("LAY HANDS", SKILL_LAY_HANDS);
   m_SkillNamesMap.Add("LISTEN", SKILL_LISTEN);
   m_SkillNamesMap.Add("LOOTING", SKILL_LOOTING);
   m_SkillNamesMap.Add("MEDITATE", SKILL_MEDITATE);
   m_SkillNamesMap.Add("MOUNTED COMBAT", SKILL_MOUNTED_COMBAT);
   m_SkillNamesMap.Add("OBSERVATION", SKILL_OBSERVATION);
   m_SkillNamesMap.Add("PARRY", SKILL_PARRY);
   m_SkillNamesMap.Add("POLYMORPH", SKILL_POLYMORPH);
   m_SkillNamesMap.Add("PRAY", SKILL_PRAY);
   m_SkillNamesMap.Add("PREPARE", SKILL_PREPARE);
   m_SkillNamesMap.Add("QUICK CHANT", SKILL_QUICK_CHANT);
   m_SkillNamesMap.Add("QUICK STRIKE", SKILL_QUICK_STRIKE);
   m_SkillNamesMap.Add("QUIVERING PALM", SKILL_QUIVERING_PALM);
   m_SkillNamesMap.Add("RANGED WEAPONS", SKILL_RANGED_WEAPONS);
   m_SkillNamesMap.Add("REMOVE TRAP", SKILL_REMOVE_TRAP);
   m_SkillNamesMap.Add("RIPOSTE", SKILL_RIPOSTE);
   m_SkillNamesMap.Add("SHIELD RUSH", SKILL_SHIELD_RUSH);
   m_SkillNamesMap.Add("SONG CHARMING", SKILL_SONG_CHARMING);
   m_SkillNamesMap.Add("SONG FLIGHT", SKILL_SONG_FLIGHT);
   m_SkillNamesMap.Add("SONG HEALING", SKILL_SONG_HEALING);
   m_SkillNamesMap.Add("SONG HEROISM", SKILL_SONG_HEROISM);
   m_SkillNamesMap.Add("SONG HURTING", SKILL_SONG_HURTING);
   m_SkillNamesMap.Add("SONG REVELATION", SKILL_SONG_REVELATION);
   m_SkillNamesMap.Add("STEAL", SKILL_STEAL);
   m_SkillNamesMap.Add("SUBTERFUGE", SKILL_SUBTERFUGE);
   m_SkillNamesMap.Add("SUMMON MOUNT", SKILL_SUMMON_MOUNT);
   m_SkillNamesMap.Add("WEAPON FLURRY", SKILL_WEAPON_FLURRY);
   m_SkillNamesMap.Add("THROAT SLIT", SKILL_THROAT_SLIT);
   m_SkillNamesMap.Add("THROW WEAPON", SKILL_THROW_WEAPON);
   m_SkillNamesMap.Add("TRACK", SKILL_TRACK);
   m_SkillNamesMap.Add("TRAP", SKILL_TRAP);
   m_SkillNamesMap.Add("TUMBLING", SKILL_TUMBLING);
   m_SkillNamesMap.Add("VAMPIRIC TOUCH", SKILL_VAMPIRIC_TOUCH);
   m_SkillNamesMap.Add("WEAPON KNOWLEDGE", SKILL_WEAPON_KNOWLEDGE);
   m_SkillNamesMap.Add("WEAPON REPAIR", SKILL_WEAPON_REPAIR);
   m_SkillNamesMap.Add("WRESTLING", SKILL_WRESTLING);
   m_SkillNamesMap.Add("DOOR BASH", SKILL_DOORBASH);
   m_SkillNamesMap.Add("DODGE", SKILL_DODGE);
   m_SkillNamesMap.Add("DUAL WIELD", SKILL_DUAL_WIELD);
   m_SkillNamesMap.Add("TRIPLE ATTACK", SKILL_TRIPLE_ATTACK);
   m_SkillNamesMap.Add("BASH", SKILL_BASH);
   m_SkillNamesMap.Add("BACKSTAB", SKILL_BACKSTAB);
   m_SkillNamesMap.Add("OFFENSE", SKILL_OFFENSE);
   m_SkillNamesMap.Add("LOOT", SKILL_LOOT);
   m_SkillNamesMap.Add("BANDAGE", SKILL_BANDAGE);
   m_SkillNamesMap.Add("BODYSLAM", SKILL_BODYSLAM);
   m_SkillNamesMap.Add("ROAR", SKILL_ROAR);
   m_SkillNamesMap.Add("FIRST AID", SKILL_FIRST_AID);
   m_SkillNamesMap.Add("BERZERK", SKILL_BERZERK);
   m_SkillNamesMap.Add("BOWS", SKILL_BOWS);
   m_SkillNamesMap.Add("MACE CLUB", SKILL_MACE_CLUB);
   m_SkillNamesMap.Add("MOUNT", SKILL_MOUNT);
   m_SkillNamesMap.Add("RESCUE", SKILL_RESCUE);
   m_SkillNamesMap.Add("STAVES", SKILL_STAVES);
   m_SkillNamesMap.Add("SWITCH", SKILL_SWITCH);
   m_SkillNamesMap.Add("TRIP", SKILL_TRIP);
   m_SkillNamesMap.Add("SNEAK", SKILL_SNEAK);
   m_SkillNamesMap.Add("HIDE", SKILL_HIDE);
   m_SkillNamesMap.Add("KICK", SKILL_KICK);
   m_SkillNamesMap.Add("TUNDRA MAGIC", SKILL_TUNDRA_MAGIC);
   m_SkillNamesMap.Add("DESERT MAGIC", SKILL_DESERT_MAGIC);
   m_SkillNamesMap.Add("MOUNTAIN MAGIC", SKILL_MOUNTAIN_MAGIC);
   m_SkillNamesMap.Add("OCEAN MAGIC", SKILL_OCEAN_MAGIC);
   m_SkillNamesMap.Add("FOREST MAGIC", SKILL_FOREST_MAGIC);
   m_SkillNamesMap.Add("SWAMP MAGIC", SKILL_SWAMP_MAGIC);
   m_SkillNamesMap.Add("GREY MAGIC", SKILL_GREY_MAGIC);
   m_SkillNamesMap.Add("CLERIC MAGIC", SKILL_CLERIC_MAGIC);
   m_SkillNamesMap.Add("SHAMAN MAGIC", SKILL_SHAMAN_MAGIC);
   m_SkillNamesMap.Add("FORAGE", SKILL_FORAGE);
   m_SkillNamesMap.Add("BLIND FIGHTING", SKILL_BLIND_FIGHTING);
   POSITION pos = m_SkillNamesMap.GetStartingPosition();
   short nSkill;
   CString str;
   while (pos) {
	nSkill = m_SkillNamesMap.GetNext(str, pos);
	m_SkillNamesArray[nSkill] = str;
   }
}

///////////////////////////////////
//	Initially boots the news and allows gods to force a re-read

bool CCharacterAttributes::ReadNews() {
   NEWS.Empty();
   CAscii NewsFile(NEWSFILE, std::ios_base::in);
   if (NewsFile) {
	NewsFile.ReadFileString(NEWS, EOF);
	NewsFile.close();
	return true;
   } else {
	ErrorLog << "News File doesn't exist" << endl;
	return false;
   }
}

//////////////////////////////////
// initially boots motd file and allows gods to force a re-read

bool CCharacterAttributes::ReadMOTD() {
   CAscii MotdFile(MOTDFILE, std::ios_base::in);
   if (MotdFile) {
	MotdFile.ReadFileString(MOTD, EOF);
	MotdFile.close();
	return true;
   } else {
	ErrorLog << "Motd File doesn't exist" << endl;
	return false;
   }
}

////////////////////////////////////
//	BootPlayers
//	Reads player file adds each player
//	to the hash table by name storing
//	it's file position
//	written by: Demetrius Comes
/////////////////////////////////////

void CCharacterAttributes::BootPlayers() {
   MudLog << (const char *) "Saved Player size is: " << int(sizeof(sSaveChar)) << endl;
   //if no player file return
   if (!mPlayerFile)
	return;

   //remove all deleted characters
   RemoveDeletedChars();

   //Seek begining
   fseek(mPlayerFile, 0, SEEK_END);
   long lSize = ftell(mPlayerFile);

   //is it corrupt after removal of deleted chars?
   if (lSize % sizeof(sSaveChar) && lSize != 0) {
	ErrorLog << "Corrupt player file, after we deleted characters!\r\n" << endl;
	return;
   } else if (lSize != 0) {
	//reset file pos
	fseek(mPlayerFile, 0, SEEK_SET);
	sSaveChar SaveChar;
	long lFilePos = 0;
	int nCounter = 0;
	while (!ferror(mPlayerFile) && (lSize != (lFilePos = ftell(mPlayerFile)))) {
	   assert(1 == fread((char *) &SaveChar, sizeof(sSaveChar), 1, mPlayerFile));
	   if (!m_PlayerFilePos.Lookup(SaveChar.m_strName, lFilePos)) {
		m_PlayerFilePos.Add(SaveChar.m_strName, lFilePos);
		nCounter++;
		CheckFrags(SaveChar);
	   } else {
		ErrorLog << SaveChar.m_strName << " is entered twice in player file!" << endl;
	   }
	}
	MudLog << nCounter << " players in player file" << endl;
   }
}

//////////////////////////////////
//	RemoveDeletedChars
//	every time the mud boots we load all the characters into memory
//	we then truncate the player file
//	and only resave the characters that are in state ALIVE
//	written by:  Demetrius Comes
//	2-2-99

void CCharacterAttributes::RemoveDeletedChars() {
   //seek to the end
   fseek(mPlayerFile, 0, SEEK_END);
   //get file pos for size
   long lSize = ftell(mPlayerFile);

   //is it corrupt
   if (lSize % sizeof(sSaveChar) && lSize != 0) {
	//TODO
	ErrorLog << "Corrupt player file!" << endl;
   } else if (lSize != 0) {
	//reset the file pointer
	fseek(mPlayerFile, 0, SEEK_SET);
	//Clean deleted characters
	long lNumOfPlayers = lSize / sizeof(sSaveChar);
	sSaveChar *pSaveChars = new sSaveChar[lNumOfPlayers];

	int i;
	for (i = 0; i < (int) lNumOfPlayers; i++) {
	   assert(1 == fread((char *) &pSaveChars[i], sizeof(sSaveChar), 1, mPlayerFile));
	}
	fclose(mPlayerFile);
	mPlayerFile = fopen(PLAYER_FILE, "wb+");
	fclose(mPlayerFile);
	//cheap way to truncate the file
	mPlayerFile = fopen(PLAYER_FILE, "rb+");
	assert(mPlayerFile && !ferror(mPlayerFile));
	fseek(mPlayerFile, 0, SEEK_SET);
	int nDeleted = 0;
	for (i = 0; i < (int) lNumOfPlayers; i++) {
	   if (!pSaveChars[i].ShouldBeDeleted()) {
		fwrite((char *) &pSaveChars[i], sizeof(sSaveChar), 1, mPlayerFile);
	   } else {
		nDeleted++;
	   }
	}
	delete [] pSaveChars;
	MudLog << "Deleted : " << nDeleted << " characters from player file." << endl;
	fflush(mPlayerFile);
   }
}

////////////////////////////////////////
//	GetTotal Number of char's saved

int CCharacterAttributes::GetSavePlayerCount() {
   return m_PlayerFilePos.GetCount();
}

//////////////////////////////////////
//	Deletes a character

bool CCharacterAttributes::DeleteChar() {
   long lFilePos;
   if (m_PlayerFilePos.Lookup(m_strName, lFilePos)) {
	sSaveChar SaveChar(this, sSaveChar::STATE_DELETED);
	fseek(mPlayerFile, lFilePos, SEEK_SET);
	fwrite((char *) &SaveChar, sizeof(sSaveChar), 1, mPlayerFile);
	fflush(mPlayerFile);
	m_PlayerFilePos.RemoveKey(m_strName);
	//delete object file
	CString str(sObjectSave::DIR);
	str += m_strName;
	remove(str.cptr());
	//delete spell file if exists
	str.Format("%s%s", SPELL_SAVE_PREFIX, m_strName.cptr());
	remove(str.cptr());
	return true;
   }
   return false;
}

/////////////////////////////////
//  non static functions

/////////////////////////////
//	IsPlayerSaved
//	Static function that checks to see if the
//	current player is saved...if so load all information
//	written by: Demetrius Comes
//	6/30/98
////////////////////////////

bool CCharacterAttributes::IsPlayerSaved() {
   long lPos;

   if (!m_PlayerFilePos.Lookup(m_strName, lPos))
	return false;

   fseek(mPlayerFile, lPos, SEEK_SET);
   sSaveChar SaveChar;
   assert(1 == fread((char *) &SaveChar, sizeof(sSaveChar), 1, mPlayerFile));
   *this = SaveChar;
   return true;
}

////////////////////////////////
//	Overloaded =
//	For save char
//	written by: Demetrius Comes
//	5/22/98
////////////////////////////////

CCharacterAttributes &CCharacterAttributes::operator=(sSaveChar &Char) {
   m_lLastRentRoom = Char.m_lLastRentRoom;
   m_nPracticeSessions = Char.m_nPracticeSessions;
   m_nPreference = Char.m_nPreference;
   m_nWimpy = Char.m_nWimpy;
   m_nByScreenSize = Char.m_nByScreenSize;
   m_nWorshipedGod = Char.m_nWorshipedGod;
   m_nManaPts = Char.m_nManaPts;
   m_nCurrentHpts = Char.m_nCurrentHpts;
   m_nExperience = Char.m_nExperience;
   m_nMaxHpts = Char.m_nMaxHpts;
   m_nMaxManaPts = Char.m_nMaxManaPts;
   m_nMaxMovePts = Char.m_nMaxMovePts;
   m_nMovePts = Char.m_nMovePts;
   m_nPosition = Char.m_nPosition;
   m_strDescription = Char.m_strDescription;
   m_strLongDescription = Char.m_strLongDescription;
   m_strName = Char.m_strName;
   m_strPassword = Char.m_strPassword;
   m_strTitle = Char.m_strTitle;
   m_nLevel = Char.m_nLevel;
   m_nSpellSphere = Char.m_nSpellSphere;
   m_nSkillSphere = Char.m_nSkillSphere;
   m_nAlignment = Char.m_nAlignment;
   m_CashOnHand = Char.m_CashOnHand;
   m_fFrags = Char.m_fFrags;
   m_strGuildName = Char.m_strGuildName;
   m_lPlayerSetHomeTown = Char.m_lPlayerSetHomeTown;
   delete m_pBankCash;
   m_pBankCash = new sMoney(Char.m_BankCash);
   int i;
   for (i = 0; i < MAX_MUD_SKILLS; i++)
	m_Skills[i] = Char.m_Skills[i];
   for (i = 0; i < MAX_SPELLS_FOR_ANY_CLASS; i++)
	m_Spells[i] = Char.m_Spells[i];
   for (i = 0; i < TOTAL_LANGUAGES; i++)
	m_Languages[i] = Char.m_Languages[i];
   delete m_pAttributes;
   m_pAttributes = new CAttributes(Char.m_SavedAttributes);
   memcpy(m_Affects, Char.m_Affects, sizeof(sAffect) * TOTAL_AFFECTS);
   return *this;
}

//////////////////////////
//	FindSkill
//	Given a string gives back
//	the #define skill number
//////////////////////////

short CCharacterAttributes::FindSkill(CString &strSkill) {
   short nSkillNum;
   if (m_SkillNamesMap.Lookup(strSkill, nSkillNum)) {
	return nSkillNum;
   }
   return -1;
}

CString CCharacterAttributes::FindSkill(short nSkill) {
   return m_SkillNamesArray[nSkill];
}

const char*CCharacterAttributes::GetPronoun(short int nSex) {
   if (nSex < 0 || nSex > 2)
	return "Illegal Sex!";
   return Pronouns[nSex];
}

const char*CCharacterAttributes::GetSexName(short int nSex) {
   if (nSex < 0 || nSex > 2)
	return "Illegal Sex!";
   return SexNames[nSex];
}

const char *CCharacterAttributes::GetSizeName(int nSize) {
   if (nSize < 0 || nSize >= TOTAL_SIZES)
	return "Illegal Size!";
   return SizeNames[nSize];
}

///////////////////////////////
//	CCharacterAttributes
//	Populate all variable where they are originial
//	named!
//	written by: Demetrius Comes
//	6/6/98
///////////////////////////////

CCharacterAttributes::CCharacterAttributes(CMobPrototype &mob, CRoom * pPutInRoom) {
   InitStatics();

   short int i; //to run arrays
   //get done at CCharacter level!
   m_nMaxHpts = m_nCurrentHpts = 0;

   m_pAttributes = new CAttributes(mob);
   m_nMovePts = m_nMaxMovePts = m_pAttributes->GetStartingMoves();
   m_nManaPts = m_nMaxManaPts = m_pAttributes->GetStartingMana();
   for (i = 0; i < TOTAL_LANGUAGES; i++) {
	m_Languages[i] = 100; //mob get perfect languages skills
   }
   m_nPracticeSessions = 1; //not read from mob files
   m_nSpellSphere = mob.GetSpellSpheres();
   //pretty sure these should be the same
   m_nSkillSphere = mob.GetSpellSpheres();
   m_nPosition = mob.GetLoadPos();

   m_nPreference = mob.GetMobFlags();
   m_nByScreenSize = 20;
   m_lPlayerSetHomeTown = m_lLastRentRoom = pPutInRoom->GetVnum();
   m_nFuguePlaneTime = 0;
   m_nLevel = 0; //we'll do an advance level to get to mob.Getlevel()
   m_nAlignment = mob.GetAlignment();
   m_nPosition = mob.GetLoadPos();
   m_nNoBareHandDamDice = mob.GetNumOfDice();
   m_nTypeBareHandDamDice = mob.GetDiceType();
   m_nPlusBareHandDamDice = mob.GetPlusDam();
   m_nExperience = 0;
   m_strAlias = mob.GetAlias();
   m_strAlias.Format("%s %s", m_strAlias.cptr(), GVM.GetRaceName(mob.GetRace()));
   m_strName = mob.GetName();
   m_strDescription = mob.GetDesc();
   m_strLongDescription = mob.GetLongDesc();
   m_CashOnHand = mob.GetCash();
   m_fFrags = (float) 0;
   m_nWimpy = 0; //here we set this to zero because we don't have hitpoints
   //to calcuate what they flee at if they are fleeing

   m_pBankCash = NULL; //mobs don't have money in the bank!
   //NULL out eq array
   for (i = 0; i < MAX_MUD_SKILLS; i++)
	m_Skills[i] = 0;
   for (i = 0; i < MAX_SPELLS_FOR_ANY_CLASS; i++)
	m_Spells[i] = 0;
   m_nWorshipedGod = GOD_NONE;

}

///////////////////////////////
//	CCharacterAttributes
//	Populate all variable where they are originial
//	named!
//	written by: Demetrius Comes
//	6/6/98
///////////////////////////////

CCharacterAttributes::CCharacterAttributes(CCharIntermediate *ch) {
   InitStatics();
   short int i; //to run arrays

   m_pAttributes = new CAttributes(ch->m_pAttributes);
   m_nPracticeSessions = ch->m_nPracticeSessions;
   m_nWimpy = ch->m_nWimpy;
   memcpy(m_Affects, ch->m_Affects, sizeof(sAffect) * TOTAL_AFFECTS);
   memcpy(m_Languages, ch->m_Languages, sizeof(skill_type) * TOTAL_LANGUAGES);
   m_nFuguePlaneTime = ch->m_nFuguePlaneTime;
   m_lLastRentRoom = ch->m_lLastRentRoom;
   m_lPlayerSetHomeTown = ch->m_lPlayerSetHomeTown;
   m_nSpellSphere = ch->m_nSpellSphere;
   m_nSkillSphere = ch->m_nSkillSphere;
   m_nPreference = ch->m_nPreference;
   m_nLevel = ch->m_nLevel;
   m_fFrags = ch->m_fFrags;
   m_nAlignment = ch->m_nAlignment;
   m_nPosition = POS_STANDING;
   m_nNoBareHandDamDice = 1;
   m_nTypeBareHandDamDice = 4; //THese should also be race dependent
   m_nPlusBareHandDamDice = 0;
   m_nExperience = ch->m_nExperience;
   m_nMaxHpts = ch->m_nMaxHpts;
   m_nCurrentHpts = ch->m_nCurrentHpts;
   m_nMaxManaPts = ch->m_nMaxManaPts;
   m_nManaPts = ch->m_nManaPts;
   m_nMaxMovePts = ch->m_nMaxMovePts;
   m_nMovePts = ch->m_nMovePts;
   m_strName = ch->m_strName;
   m_strAlias.Format("%s %s", GVM.GetRaceName(m_pAttributes->GetRace()), m_strName.cptr());
   m_strDescription = ch->m_strDescription;
   m_strLongDescription = ch->m_strLongDescription;
   m_strPassword = ch->m_strPassword;
   m_strTitle = ch->m_strTitle;
   m_strPoofIn = ch->m_strPoofIn;
   m_strPoofOut = ch->m_strPoofOut;
   m_strGuildName = ch->m_strGuildName;
   m_nWorshipedGod = ch->m_nWorshipedGod;
   m_nByScreenSize = ch->m_nByScreenSize;
   if (ch->m_pBankCash) {
	m_pBankCash = new sMoney(*(ch->m_pBankCash));
   } else {
	m_pBankCash = new sMoney();
   }

   m_CashOnHand = ch->m_CashOnHand;
   //NULL out eq array
   for (i = 0; i < MAX_MUD_SKILLS; i++)
	m_Skills[i] = ch->m_Skills[i];
   for (i = 0; i < MAX_SPELLS_FOR_ANY_CLASS; i++)
	m_Spells[i] = ch->m_Spells[i];
}

CCharacterAttributes::~CCharacterAttributes() {
   delete m_pAttributes;
   delete m_pBankCash;
}

/////////////////////////////
//	CCharacterAttributes
//	
////////////////////////////

CCharacterAttributes::CCharacterAttributes() {
   InitStatics();
   m_pBankCash = NULL;
   m_lPlayerSetHomeTown = m_lLastRentRoom = NOWHERE;
   m_nFuguePlaneTime = CMudTime::PULSES_PER_REAL_SECOND * 10;
   memset(m_Languages, '\0', sizeof(skill_type) * TOTAL_LANGUAGES);
   m_nAlignment = 0;
   m_nCurrentHpts = 0;
   m_nExperience = 0;
   m_nLevel = 0;
   m_nManaPts = m_nMaxHpts = m_nMaxManaPts = m_nMaxMovePts = 0;
   m_nMovePts = m_nNoBareHandDamDice = m_nPlusBareHandDamDice = 0;
   m_nPosition = POS_STANDING;
   m_nPreference = PREFERS_TELLS | PREFERS_AUTO_EXIT | PREFERS_MAP_OUTSIDE | PREFERS_OOC;
   m_nByScreenSize = 20;
   m_nSpellSphere = 0;
   m_nSkillSphere = 0;
   m_nTypeBareHandDamDice = 0;
   m_nPracticeSessions = 1;
   m_nWimpy = 10;
   m_fFrags = (float) 0;
   m_pAttributes = new CAttributes();
   int i;
   for (i = 0; i < MAX_MUD_SKILLS; i++)
	m_Skills[i] = 0;
   for (i = 0; i < MAX_SPELLS_FOR_ANY_CLASS; i++)
	m_Spells[i] = 0;
   m_nWorshipedGod = GOD_NONE;
}

/////////////////////////////////
//	Overloaded constructor
//	for creating characterattributes from another
//	written by: Demetrius Comes
//	7/4/98
//////////////////////////////

CCharacterAttributes::CCharacterAttributes(CCharacterAttributes * pCh) {
   InitStatics();
   m_pBankCash = new sMoney(*(pCh->m_pBankCash));
   memcpy(m_Affects, pCh->m_Affects, sizeof(sAffect) * TOTAL_AFFECTS);
   memcpy(m_Languages, pCh->m_Languages, sizeof(skill_type) * TOTAL_LANGUAGES);
   m_CashOnHand = pCh->m_CashOnHand;
   m_lLastRentRoom = pCh->m_lLastRentRoom;
   m_lPlayerSetHomeTown = pCh->m_lPlayerSetHomeTown;
   m_nPracticeSessions = pCh->m_nPracticeSessions;
   m_nWimpy = pCh->m_nWimpy;
   m_nByScreenSize = pCh->m_nByScreenSize;
   m_nFuguePlaneTime = pCh->m_nFuguePlaneTime;
   m_nAlignment = pCh->m_nAlignment;
   m_nCurrentHpts = pCh->m_nCurrentHpts;
   m_nExperience = pCh->m_nExperience;
   m_nLevel = pCh->m_nLevel;
   m_fFrags = pCh->m_fFrags;
   m_nManaPts = pCh->m_nManaPts;
   m_nMaxHpts = pCh->m_nMaxHpts;
   m_nMaxManaPts = pCh->m_nMaxManaPts;
   m_nMaxMovePts = pCh->m_nMaxMovePts;
   m_nMovePts = pCh->m_nMovePts;
   m_nNoBareHandDamDice = pCh->m_nNoBareHandDamDice;
   m_nPlusBareHandDamDice = pCh->m_nPlusBareHandDamDice;
   m_nPosition = POS_STANDING;
   m_nPreference = pCh->m_nPreference;
   m_nSpellSphere = pCh->m_nSpellSphere;
   m_nSkillSphere = pCh->m_nSkillSphere;
   m_nTypeBareHandDamDice = pCh->m_nTypeBareHandDamDice;
   m_pAttributes = new CAttributes(pCh->m_pAttributes);
   m_strName = pCh->m_strName;
   m_strAlias = pCh->m_strAlias;
   m_strLongDescription = pCh->m_strLongDescription;
   m_strPassword = pCh->m_strPassword;
   m_strPoofIn = pCh->m_strPoofIn;
   m_strPoofOut = pCh->m_strPoofOut;
   m_strTitle = pCh->m_strTitle;
   m_strGuildName = pCh->m_strGuildName;
   m_nWorshipedGod = pCh->m_nWorshipedGod;
   int i;
   for (i = 0; i < MAX_MUD_SKILLS; i++)
	m_Skills[i] = pCh->m_Skills[i];
   for (i = 0; i < MAX_SPELLS_FOR_ANY_CLASS; i++)
	m_Spells[i] = pCh->m_Spells[i];
}

//////////////////////////////////
//	GetPositionNames
//	Build a string of positions
//	written by: Demetrius Comes
//	8/3/98
/////////////////////////////////

const char *CCharacterAttributes::GetPositionNames(bool bForScore) {
   //if it doesn't change from bug we have a problem
   const char *p = "BUG";
   if (InPosition(POS_SLEEPING)) {
	if (bForScore) {
	   p = "sleeping";
	} else if (InPosition(POS_STANDING)) {
	   p = "stands here sleeping";
	} else if (InPosition(POS_SITTING) || InPosition(POS_RESTING)) {
	   p = "sits here sleeping";
	} else if (InPosition(POS_RECLINING)) {
	   p = "lies here sleeping";
	} else {
	   p = "sleeps here";
	}
   } else {
	if (InPosition(POS_STANDING)) {
	   p = ((bForScore) ? "standing" : "stands here");
	}
	if (InPosition(POS_SITTING) && !InPosition(POS_RESTING)) {
	   p = ((bForScore) ? "sitting" : "sits here");
	}
	if (InPosition(POS_PREP && !InPosition(POS_RESTING))) {
	   p = ((bForScore) ? "preparing" : "sits here");
	}
	if (InPosition(POS_RESTING)) {
	   p = ((bForScore) ? "resting" : "rests here");
	}
	if (InPosition(POS_RECLINING)) {
	   if (InPosition(POS_KOED)) {
		p = ((bForScore) ? "knocked out" : "lies here knocked out.");
	   } else {
		p = ((bForScore) ? "lying down" : "lies here");
	   }
	}
	if (InPosition(POS_STUNNED)) {
	   p = ((bForScore) ? "stunnded" : "is here with a stunned look");
	}
	if (InPosition(POS_INCAP)) {
	   p = ((bForScore) ? "incapacitated" : "is incapacitated");
	}
	if (InPosition(POS_MORTALED)) {
	   p = ((bForScore) ? "mortally wounded" : "is mortally wounded and will die soon");
	}
	if (InPosition(POS_DEAD)) {
	   p = "em your dead ";
	}
   }
   return p;
}

/////////////////////////////////
//	IsSelf
//	Check to see if str is self, me, myself etc
//

bool CCharacterAttributes::IsSelf(CString & strName) {
   return m_strSelf.Find(strName) != -1;
}

/////////////////////////////
//	Returns a wording so
//	characters can't see there nubmers
////////////////////////////

const char *CCharacterAttributes::SkillWording(short nSkilled) {
   if (nSkilled > 90)
	return "master";
   else if (nSkilled <= 90 && nSkilled > 75)
	return "expert";
   else if (nSkilled <= 75 && nSkilled > 55)
	return "above average";
   else if (nSkilled <= 55 && nSkilled > 45)
	return "average";
   else if (nSkilled <= 45 && nSkilled > 30)
	return "below average";
   else if (nSkilled <= 30 && nSkilled > 15)
	return "You know just enough to be dangerous";
   else if (nSkilled <= 15 && nSkilled > 0)
	return "just learning";
   else if (nSkilled == 0)
	return "not learned";
   else
	return "you should never see this";
}


//////////////////////////
//
//  Sets current and max hits
///////////////////////

void CCharacterAttributes::SetHits(int iSet) {
   m_nCurrentHpts = iSet;
   m_nMaxHpts = iSet;
   return;
}

/////////////////////////////////////////////////////
//	Adjust Current Moves
//	Used for spells like Vigorize and Fatigue
//
//	John Comes 3-12-99
/////////////////////////////////////////////////////

void CCharacterAttributes::AdjustCurrentMoves(short nAdj) {
   m_nMovePts = m_nMovePts + nAdj > m_nMaxMovePts ? m_nMaxMovePts : m_nMovePts + nAdj < 0 ? 0 : m_nMovePts + nAdj;
}

/////////////////////////////////////////////////////
//	Adjust Max Moves
//	Used for spells that adjust maximum moves
//
//	John Comes 3-12-99
/////////////////////////////////////////////////////

void CCharacterAttributes::AdjustMaxMoves(short nAdj) {
   m_nMovePts += nAdj;
   m_nMaxMovePts += nAdj;
}

//////////////////////////////////////////////////////
//	if we get a character pointer
//	then we must becomming from the game
//	so we either camped, rented, or died
//	so we sync with saved character

CCharacterAttributes::CCharacterAttributes(CCharacter * pCh) {
   InitStatics();
   //null out everything to be safe
   m_pBankCash = NULL;
   m_lPlayerSetHomeTown = m_lLastRentRoom = NOWHERE;
   m_nFuguePlaneTime = CMudTime::DEFAULT_FUGUE_PLANE_TIME;
   memset(m_Languages, '\0', sizeof(skill_type) * TOTAL_LANGUAGES);
   m_nAlignment = 0;
   m_nCurrentHpts = 0;
   m_nExperience = 0;
   m_nLevel = 0;
   m_nManaPts = m_nMaxHpts = m_nMaxManaPts = m_nMaxMovePts = 0;
   m_nMovePts = m_nNoBareHandDamDice = m_nPlusBareHandDamDice = 0;
   m_nPosition = POS_STANDING;
   m_nPreference = 0;
   m_nByScreenSize = 20;
   m_nSpellSphere = 0;
   m_nSkillSphere = 0;
   m_nTypeBareHandDamDice = 0;
   m_nPracticeSessions = 1;
   m_fFrags = (float) 0;
   m_nWimpy = 10;
   m_pAttributes = new CAttributes();
   int i;
   for (i = 0; i < MAX_MUD_SKILLS; i++) {
	m_Skills[i] = 0;
   }
   for (i = 0; i < MAX_SPELLS_FOR_ANY_CLASS; i++) {
	m_Spells[i] = 0;
   }
   m_nWorshipedGod = GOD_NONE;
   //load from disk
   long lPos;
   if (m_PlayerFilePos.Lookup(pCh->GetName(), lPos)) {
	fseek(mPlayerFile, lPos, SEEK_SET);
	sSaveChar SaveChar;
	assert(1 == fread((char *) &SaveChar, sizeof(sSaveChar), 1, mPlayerFile));
	*this = SaveChar;
   }//if we can only come from the game then
	//if we are not saved assert(0) because we have to be
   else {
	assert(0);
   }
}

/////////////////////////////////////
//	Cleanup
//	Used to insure all statics get cleanuped
//	May or may not be need but we seem to be getting
//	corrupt player files often after crash so we'll
//	try forcing a nice file close.

void CCharacterAttributes::CleanUp() {
   fflush(mPlayerFile);
   fclose(mPlayerFile);
}

///////////////////////////////////
//	Check Frag list

void CCharacterAttributes::CheckFrags(sSaveChar &s) {
   std::vector<CCharacterAttributes::sFragInfo>::iterator
   i = std::find(m_FragList.begin(), m_FragList.end(), s.m_strName);
   //we didn't find it
   if (i == m_FragList.end()) {
	//don't add them if they don't have any frags
	if (s.m_fFrags != (float) 0) {
	   sFragInfo FragInfo(s);
	   m_FragList.insert(m_FragList.end(), FragInfo);
	}
   }//we found them but they are only at 0 frags
   else if (s.m_fFrags == (float) 0) {
	m_FragList.erase(i);
   }//else we found them and they are NOT at 0 frags
   else {
	(*i) = s;
   }
}

void CCharacterAttributes::GetFragList(CString &str) {
   static const char *strFormat =
	   "			&GFRAG LIST&n\r\n\r\n"
	   "		&LEvils     =       %.2f&n\r\n"
	   "		&WGoodies   =       %.2f&n\r\n"
	   "		&wNeutrals  =       %.2f&n\r\n"
	   "		&MAliens    =       %.2f&n\r\n\r\n"
	   "			&Y Top Fraggers&n\r\n\r\n";

   //sort the vector by frags first
   std::sort(m_FragList.begin(), m_FragList.end());

   float fEvil = (float) 0,
	   fGood = (float) 0,
	   fNeutral = (float) 0,
	   fAlien = (float) 0;
   std::vector<CCharacterAttributes::sFragInfo>::iterator i;
   for (i = m_FragList.begin(); i < m_FragList.end(); i++) {
	if ((*i).m_nRaceWar == CCharacterAttributes::sFragInfo::ALIEN)
	   fAlien += (*i).m_fFrags;
	else if ((*i).m_nRaceWar == CCharacterAttributes::sFragInfo::EVIL)
	   fEvil += (*i).m_fFrags;
	else if ((*i).m_nRaceWar == CCharacterAttributes::sFragInfo::GOOD)
	   fGood += (*i).m_fFrags;
	else if ((*i).m_nRaceWar == CCharacterAttributes::sFragInfo::NEUTRAL)
	   fNeutral += (*i).m_fFrags;
	else
	   ErrorLog << "illegal race war type in GetFragList" << endl;
   }
   str.Format(strFormat, fEvil, fGood, fNeutral, fAlien);
   short nCount = 0;
   for (i = m_FragList.begin(); i < m_FragList.end() && nCount < 10; i++) {
	if ((*i).m_fFrags > (float) 0) {
	   nCount++;
	   str.Format("%s  %2d)  %-18s %.2f  [%-17s] (%s)\r\n",
		   str.cptr(),
		   nCount,
		   (*i).m_strName.cptr(),
		   (*i).m_fFrags,
		   GVM.GetColorClass((*i).m_nClass),
		   GVM.GetColorRaceName((*i).m_nRace));
	}
   }
   str += "\r\n			&R Worst Fraggers &n\r\n\r\n";
   std::vector<CCharacterAttributes::sFragInfo>::reverse_iterator j;
   nCount = 0;
   for (j = m_FragList.rbegin(); j < m_FragList.rend() && nCount < 10; j++) {
	if ((*j).m_fFrags < (float) 0) {
	   nCount++;
	   str.Format("%s  %2d)  %-17s %.2f  [%-17s] (%s)\r\n",
		   str.cptr(),
		   nCount,
		   (*j).m_strName.cptr(),
		   (*j).m_fFrags,
		   GVM.GetColorClass((*j).m_nClass),
		   GVM.GetColorRaceName((*j).m_nRace));
	}
   }
}

bool CCharacterAttributes::SaveToFile(bool bAllowNew) {
   long lFilePos = 0;
   assert(ferror(mPlayerFile) == 0);
   bool b = m_PlayerFilePos.Lookup(m_strName, lFilePos);
   if (b || bAllowNew) {
	sSaveChar SaveChar(this);
	CheckFrags(SaveChar);
	if (!b) {
	   fseek(mPlayerFile, 0, SEEK_END);
	   lFilePos = ftell(mPlayerFile);
	   m_PlayerFilePos.Add(m_strName, lFilePos);
	} else {
	   fseek(mPlayerFile, lFilePos, SEEK_SET);
	}
	fwrite((char *) &SaveChar, sizeof(sSaveChar), 1, mPlayerFile);
	fflush(mPlayerFile);
	return true;
   }
   return false;
}

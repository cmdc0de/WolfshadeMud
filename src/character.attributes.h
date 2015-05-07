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
*	CCharacterAttributes
*
*	A simple class to hold most character attributes
*	so that we can derive CCharIntermediate from it as
*	well as CCharacter
*/

#ifndef _CHARACTERATTRIBUTES_H_
#define _CHARACTERATTRIBUTES_H_

#include "linklist.h"
#include "constants.h"
#include "affect.h"
#include "attributes.h"
#include <vector>

class CCharIntermediate; //forward def

class CSkillInfo
{
private:
	int m_nSkillSet; //Profeciency
	byte m_nMax, m_nPct, m_nMin;
public:
	byte GetMax() const {return m_nMax;}
	byte GetPct() const {return m_nPct;}
	byte GetMin() const {return m_nMin;}
	bool CanLearn() const {return m_nMax!=0;}
	bool IsOfSet(int nSet) const {return (m_nSkillSet&nSet)==nSet ? true : false;}
	int GetSet() const {return m_nSkillSet;}
	CSkillInfo(int nSkillSet=0,byte nMax=0, byte nMin=0, byte nPct=0)
	{m_nSkillSet=nSkillSet;m_nMax = MIN(nMax,MAX_SKILL_LEVEL);m_nMin = MIN(nMin,MAX_SKILL_LEVEL);m_nPct = nPct;} //npct can be .1 of a % too
	CSkillInfo &operator=(const CSkillInfo &r) 
	{m_nSkillSet=r.m_nSkillSet;m_nMax = r.m_nMax;m_nMin = r.m_nMin;m_nPct = r.m_nPct;return *this;}
	CSkillInfo (const CSkillInfo &r); //broke
};

class CCharacterAttributes
{
	friend struct sSaveChar;
private:
	struct sFragInfo
	{
	public:
		static short ALIEN;
		static short GOOD;
		static short EVIL;
		static short NEUTRAL;
	public:
		CString m_strName;
		short m_nRace,
			m_nClass;
		float m_fFrags;
		byte m_nRaceWar;
		//actually test the reserve because we want sort in asc order not decending
		bool operator < (const sFragInfo &r) const {return (m_fFrags > r.m_fFrags);}
		bool operator !=(const char *s) {return m_strName!=s;}
		bool operator ==(const char *s) const {return m_strName==s;}
		sFragInfo (const sSaveChar SaveChar)
		{
			m_strName = SaveChar.m_strName;
			m_fFrags = SaveChar.m_fFrags;
			m_nRace = SaveChar.m_SavedAttributes.m_nRace;
			m_nClass = SaveChar.m_SavedAttributes.m_nChClass;
			if(SaveChar.m_SavedAttributes.m_nRace==RACE_ILLITHID ||
				SaveChar.m_SavedAttributes.m_nRace==RACE_THRIKREEN)
			{
				m_nRaceWar = (byte)ALIEN;
			}
			else if(SaveChar.m_nAlignment>GOOD_ALIGN)
			{
				m_nRaceWar = (byte)GOOD;
			}
			else if(SaveChar.m_nAlignment<EVIL_ALIGN)
			{
				m_nRaceWar = (byte)EVIL;
			}
			else
			{
				m_nRaceWar = (byte)NEUTRAL;
			}
		}
	};
private:
	static CMap <CString,short> m_SkillNamesMap;
	static CString m_SkillNamesArray[MAX_MUD_SKILLS];
	static bool Initialized;
private:
	void InitSkillNames();
	void InitStatics();
	void RemoveDeletedChars();
	void BootPlayers();
protected:
	static std::fstream m_PlayerFile;
	static CMap<CString,long> m_PlayerFilePos;
	static CString NEWS;
	static CString MOTD;
	static std::vector<sFragInfo> m_FragList;
public:
	static const int PREFERS_NO_HASSLE;
	static const int PREFERS_TELLS_WHEN_SWITCHED;
	static const int PREFERS_COLOR;
	static const int PREFERS_AUTO_EXIT;
	static const int PREFERS_BRIEF;
	static const int PREFERS_MAP_INSIDE;
	static const int PREFERS_MAP_OUTSIDE;
	static const int PREFERS_TELLS;
	static const int PREFERS_GOD_LIGHT;
	static const int PREFERS_PETITION;
	static const int PREFERS_GOD_CHAT;
	static const int PREFERS_GOD_LOG_DEATHS;
	static const int PREFERS_GOD_LOG_ZONES;
	static const int PREFERS_GOD_SEES_ALL;
	static const int PREFERS_GOD_CONNECTION_LOG;
	static const int PREFERS_GOD_ON_WHO;
	static const int PREFERS_COMPACT;
	static const int PREFERS_OOC;
	static const int PREFERS_EXTRA_RETURNS;
	static const int PREFRES_NO_PAGING;
	static const int PREFERS_GOD_INVIS;
protected:
	CCharacterAttributes(CMobPrototype &mob,CRoom *pPutInRoom);
	CCharacterAttributes(CCharIntermediate *ch);
	CCharacterAttributes(CCharacterAttributes *pCh);
	CCharacterAttributes();
	//statics
protected:
	static const char *SexNames[3];
	static const char *Pronouns[3];
	static const char *SizeNames[TOTAL_SIZES];
	static const CString m_strSelf;
protected:
	int m_nCurrentHpts,
		m_nMaxHpts,
		m_nFuguePlaneTime;

	short int m_nLevel,
		m_nNoBareHandDamDice,		//How many times to roll
		m_nTypeBareHandDamDice,	//What did dice to roll
		m_nPlusBareHandDamDice,	//damage to add to it;
		m_nAlignment, // good, bad, or ugly
		m_nMaxManaPts,
		m_nManaPts,
		m_nMaxMovePts,
		m_nMovePts,
		m_nWorshipedGod,
		m_nByScreenSize,
		m_nPracticeSessions, //number of practice session the player has
		m_nWimpy; //what will they flee at?
		
	room_vnum m_lLastRentRoom,				//duh
		m_lPlayerSetHomeTown;

	float m_fFrags; //how many race war kills

	int m_nExperience,
		m_nPreference, //holds togs for chars and mob flags for mobs
		m_nSpellSphere,
		m_nSkillSphere,
		m_nPosition;	// Character position

	skill_type m_Skills[MAX_MUD_SKILLS];
	skill_type m_Spells[MAX_SPELLS_FOR_ANY_CLASS];
	skill_type m_Languages[TOTAL_LANGUAGES];

	sMoney m_CashOnHand;
	sMoney *m_pBankCash; //So we dont' take up the memory for NPCs
	
	CString m_strDescription,
		m_strLongDescription,
		m_strName,
		m_strPassword,
		m_strAlias,
		m_strTitle,
		m_strGuildName;
		
	CString m_strPoofIn, 
		m_strPoofOut;		

	CAttributes *m_pAttributes;

	CList<CObject *> Inventory;

	sAffect m_Affects[TOTAL_AFFECTS];
	CString CurrentCommand;
protected:
	bool SaveToFile(bool bAllowNew);
	CCharacterAttributes(CCharacter *pCh);
	char *SkillWording(short nSkilled);
	bool IsPlayerSaved();
	CCharacterAttributes &operator=(sSaveChar &Char);
	void CheckFrags(sSaveChar &s);
public:
	static short FindSkill(CString &strSkill);
	static CString FindSkill(short nSkill);
public:
	int GetMaxHits() {return m_nMaxHpts;}
	void GetFragList(CString &str);
	stat_type GetCharisma() {return m_pAttributes->GetStat(CHA);}
	bool IsOfSphere(int nSphere) const {return ((m_nSpellSphere & nSphere)==nSphere) ? true : false;}
	bool HasSkillSet(int nSphere) const {return ((m_nSkillSphere & nSphere)==nSphere) ? true : false;}
	short GetAlignment() const {return m_nAlignment;}
	bool IsEvil() const {return (m_nAlignment < EVIL_ALIGN);}
	bool IsNeutral() const {return ((m_nAlignment >= EVIL_ALIGN) && (m_nAlignment <= GOOD_ALIGN));}
	bool IsGood() const {return (m_nAlignment > GOOD_ALIGN);}
	bool IsAffectedBy(short nAffect) const {return m_Affects[nAffect].HasAffect();}
	long GetLastRentRoom() {return m_lLastRentRoom;}
	CString &GetPassword() {return m_strPassword;}
	bool IsNewbie() const {return m_nLevel==0;}
	short int GetLevel() const {return m_nLevel;}
	CString GetName() const {return m_strName;}
	CString GetAlias() const {return m_strAlias;}
	CString GetDescription() const {return m_strDescription;}
	CString GetLongDescription() const {return m_strLongDescription;}
	short GetRace() const {return m_pAttributes->GetRace();}
	short GetSize() const {return m_pAttributes->GetSize();}
	short GetClass() const {return m_pAttributes->GetClass();}
	short GetGod() const {return m_nWorshipedGod;}
	int GetCurrentHpts() {return m_nCurrentHpts;}
	bool InPosition(int nPos) const {return (m_nPosition & nPos) ? true : false;}
	CString GetTitle() {return m_strTitle;}
	void SetFuguePlane() {m_lLastRentRoom = FUGUE_PLANE;}
	short GetFugueTime() {return m_nFuguePlaneTime;}
	bool IsNewCharacter() {long lPos;return !m_PlayerFilePos.Lookup(m_strName,lPos);}
	bool ShouldBeImp() {m_PlayerFile.seekg(0,std::ios::end);return 0==m_PlayerFile.tellg();}
	void SetFugueTime(short nTime) {m_nFuguePlaneTime = nTime;}
	//do not make Prefers (m_nPreference & nPreference)==nPreference....THAT IS WRONG!
	bool Prefers(int nPreference) const {return (m_nPreference & nPreference) ? true : false;}
	void TogPreference(int nPref) {(m_nPreference & nPref) ? m_nPreference &=~nPref : m_nPreference |= nPref;}
	bool IsRace(short nRace) const {return m_pAttributes->IsRace(nRace);}
public:
	void AdjustCurrentMoves(short nAdj);
	void AdjustMaxMoves(short nAdj);
	const char *GetPositionNames(bool bForScore=true);
	bool DeleteChar();
	void SetMobDesc(CString strSet) {m_strDescription = strSet;}
	void SetHits(int iSet);
public:
	static bool IsSelf(CString &strName);
	static const char*GetPronoun(short int nSex);
	static const char*GetSexName(short int nSex);
	static const char *GetSizeName(int nSize);
	static int GetSavePlayerCount();
public:
	static bool ReadNews();
	static bool ReadMOTD();
	static void CleanUp();
	virtual ~CCharacterAttributes();
};
#endif

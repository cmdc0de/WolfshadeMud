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
* CMobPrototype - Class to hold Mobprototypes
*	Holds all data from mob prototypes...these are used as 
*	'templates' to create new mobs
*/

#ifndef _MOBPROTOTYPE_H_
#define _MOBPROTOTYPE_H_

struct sWallInfo
{
	short m_nWallType,
		m_nLevel,
		m_nDirection;
	int m_nTimeToLive;
	sWallInfo &operator=(const sWallInfo &r)
	{m_nWallType = r.m_nWallType;m_nLevel=r.m_nLevel;m_nDirection=r.m_nDirection;m_nTimeToLive=r.m_nTimeToLive;return *this;}
};
struct sUndeadInfo
{
	short m_nUndeadType,
		m_nLevel;
	sUndeadInfo &operator=(const sUndeadInfo &r)
	{m_nUndeadType = r.m_nUndeadType;m_nLevel = r.m_nLevel;return *this;}
};

class CMobPrototype
{
	//we have a protected constructor so only a friend class
	//could instaniate
	friend class CMobManager;
private:
	static short STARTING_MOB_STAT;
protected:
	CString MobAlias, //alias
		MobName, 
		DefaultPosDesc, 
		MobDescription,
		MobLongDescription;

	short int Alignment,
		ClassType, 
		Thaco, 
		Level, 
		AC,  
		Race,
		Sex,
		NumOfDice, //to roll for damage
		DiceType, //what type of dice
		PlusDam, //+ damage ....example: 3d4+10
		NumInGame;
		
	int MinHits;

	int MobFlags, 
		DefaultPos,
		LoadPos,
		m_nProficiencySphere,
		Experience;

	mob_vnum Vnum;
	long Copper;
	stat_type Stats[MAX_NUMBER_OF_STATS];

protected:
	CMobPrototype(const sWallInfo *pWallInfo);
	CMobPrototype(CCharacter *pMirror);
	CMobPrototype(const sUndeadInfo *pUndeadInfo);
	CMobPrototype(CMobPrototype *pMobProto);
	CMobPrototype();
	void VStat(CString &ToChar);
public:
	int GetMinHits() {return MinHits;}
	~CMobPrototype();

	void ReadMob(CAscii &);
	long GetVnum() {return Vnum;}
	short GetStat (short nStat) const {return (Stats[nStat]);}
	int GetSpellSpheres() const {return m_nProficiencySphere;}
	int GetMobFlags() const {return MobFlags;}
	short int GetAlignment() const {return Alignment;}
	short int GetClassType() const {return ClassType;}
	short int GetRace() const {return Race;}
	short int GetLevel() const {return Level;}
	short int GetThaco() const {return Thaco;}
	short int GetAC() const {return AC;}
	long GetCash() const {return Copper;}
	int GetDefaultPos() {return DefaultPos;}
	int GetExperience() {return Experience;}
	short int GetLoadPos() const {return LoadPos;}
	short int GetSex() const {return Sex;}
	short int GetNumOfDice() const {return NumOfDice;}
	short int GetDiceType() const {return DiceType;}
	short int GetPlusDam() const {return PlusDam;}
	CString &GetName() {return MobName;}
	CString &GetAlias() {return MobAlias;}
	CString &GetDesc() {return MobDescription;}
	CString &GetLongDesc() {return MobLongDescription;}
	CString &GetDefaultPosDesc() {return DefaultPosDesc;}
};

#endif
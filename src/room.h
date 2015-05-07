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
#ifndef _ROOM_H_
#define _ROOM_H_

//#include <fstream.h>
#include "world.h"
#include "linklist.h"
#include "money.h"
#include "list.h"
#include "wolfshade.string.h"

#define TOTAL_ROOM_TYPES 18
#define TOTAL_ROOM_AFFECTS 17
//holds all information for a rooms as well as the functions
//to manipulate the room.
class CRoom
{
	//these are friends because they need to walk the Character and Object
	//link lists
	friend class CCharacter;
	friend class CWorld;
	friend class CZoneManager;
protected:
	//Affect information for rooms
	struct sAffectInfo
	{
		friend class CRoom;
	protected:
		CString m_strAffectName;
		bool m_bAddValue;
		void (CRoom::*m_pAffectFnc)(short nAffect,bool bRemoving);
	public:
		sAffectInfo() {m_bAddValue=false;m_pAffectFnc = NULL;}
		sAffectInfo(CString strAffect,void (CRoom::*AffectFnc)(short,bool),bool bAddValue)
		{
			m_strAffectName = strAffect;
			m_bAddValue = bAddValue;
			m_pAffectFnc = AffectFnc;
		}
		sAffectInfo &operator=(const sAffectInfo &r)
		{
			m_strAffectName = r.m_strAffectName;
			m_pAffectFnc = r.m_pAffectFnc;
			m_bAddValue = r.m_bAddValue;
			return *this;
		}
	};
	//To hold extra description of the room.
	struct sExtraDesc
	{
	public:
		CString KeyWord; //to see description
		CString Description;
		sExtraDesc(){;}
		sExtraDesc(sExtraDesc *r) 
			{KeyWord = r->KeyWord;Description = r->Description;}
	};
	//Contains information specific to each door
	struct sDoor
	{
	public:
		/*Door Types*/
		static const int UNRESTRICTED_DOOR;
		static const int DOOR_OPEN;
		static const int DOOR_CLOSED;
		static const int DOOR_PICKPROFF;
		static const int DOOR_SECRET;
		static const int DOOR_LOCKED;
		static const int DOOR_WALL;
	public:
		sDoor() {VnumKey = -1;ToRoom = -1;DoorType = 0;m_pToRoom = NULL;}
		sDoor(sDoor *pDoor)
		{VnumKey = pDoor->VnumKey;ToRoom = pDoor->ToRoom;DoorType = pDoor->DoorType;m_pToRoom = pDoor->m_pToRoom;}
		CString KeyWord;  //for open or close
		long VnumKey;  //key to open
		long ToRoom;  //what room does it lead to?
		CRoom *m_pToRoom;
		int DoorType;  //bitvector for door type
	public:
		bool IsUnrestricted() {return ((DoorType&UNRESTRICTED_DOOR)==UNRESTRICTED_DOOR && !IsWall());}
		bool IsWall() {return (DoorType&DOOR_WALL)==DOOR_WALL;}
		bool CanWalkThrough() {return ((IsUnrestricted() || (DoorType & DOOR_OPEN) && !IsWall()));}
		bool IsSecret() {return ((DoorType & DOOR_SECRET)==DOOR_SECRET);}
		bool IsOpen() {return ((IsUnrestricted() || (DOOR_OPEN & DoorType) && !IsWall()));}
		bool IsClosed() {return !IsOpen();}
		bool CanBeOpened() {return (((DOOR_SECRET & DoorType) || (DOOR_LOCKED & DoorType) || (DoorType & DOOR_WALL)) ? false : true);}
		bool CanBeClosed() {return (CanBeOpened() && !IsUnrestricted());}
		void Open() {DoorType=DOOR_OPEN;}
		void Close() {DoorType=DOOR_CLOSED;} 
		void SetWall() {DoorType|=DOOR_WALL;}
		void RemoveWall() {DoorType&=~DOOR_WALL;}
		void RemoveDoorType(int nDoorType) {DoorType&=~nDoorType;}
	};
	//static
protected:
	static CString m_RoomColors[TOTAL_ROOM_TYPES];
	static const short m_MovementCost[TOTAL_ROOM_TYPES];
	static bool m_bColorsBuilt;
	static CString m_strBlock;
	static bool m_bInit;
	static void InitStatics();
	static sAffectInfo m_AffectInfo[TOTAL_ROOM_AFFECTS];
protected:
	CRoom();
	CRoom(CRoom *pRoom);
	//Variables
	long m_lVnum;
	bool m_bZoneRoom;
	int m_nZone,
		m_nXpos,
		m_nYpos;

	short m_nRoomType,
		m_nRoomSize;
	
	CString m_strRoomName, 
		m_strDescription;
	
	sDoor * m_Doors[NUM_OF_DOORS];
	
	CLinkList<sExtraDesc> m_ExtraDesc;
	CList<CCharacter *> m_CharactersInRoom;
	CList<CObject *> m_ObjInRoom;

	sMoney m_CashInRoom;

	sAffect m_Affects[TOTAL_ROOM_AFFECTS];
protected:
	virtual CString GetExitString(CCharacter *pLooker);
	void LookCharacters(CString &strLook,CCharacter *pCh = NULL);
	void LookObjects(CString &strLook, CCharacter *pCh);

	//public static
public:
	static const char *strDir[];
	static const char *m_strRoomType[];
	static const char *m_strRoomSize[];
	static const char *m_strRoomBrightness[];
	static void Scan(CCharacter *pCh,CString &str);
	static short StringToDirection(CString &strDir);
public:
	//inlined functions
	bool IsOutdoors() const {return (m_nRoomType!=TYPE_INSIDE && m_nRoomType!=TYPE_UNDERGROUND && m_nRoomType!=TYPE_UNDERWATER && !IsAffectedBy(ROOM_AFFECT_INDOORS));}
	bool IsType(short nType) const {return m_nRoomType==nType;}
	bool IsDoorOpen(short nDir) {return (m_Doors[nDir]!=NULL ? m_Doors[nDir]->IsOpen() : false);}
	int GetZone() {return m_nZone;}
	bool IsZone(int nZone) {return m_nZone==nZone;}
	sMoney &GetCash() {return m_CashInRoom;}
	long GetVnum() {return m_lVnum;}
	void AddCash(long lBaseAmt) {m_CashInRoom = lBaseAmt;}
	short GetRoomType() {return m_nRoomType;}
	const char *GetName() {return m_strRoomName;}
	bool IsWaterRoom() {return (m_nRoomType==TYPE_WATERENTER || m_nRoomType==TYPE_WATERNOENTER);}
	POSITION GetFirstCharacterInRoom() {return m_CharactersInRoom.GetStartPosition();}
	CCharacter *GetNextCharacter(POSITION &pos) {return m_CharactersInRoom.GetNext(pos);}
	bool IsRealRoom() {return m_lVnum < MIN_VIRTUAL_VNUM;}
	bool IsAffectedBy(short nAffect) const {return m_Affects[nAffect].HasAffect();}
	void Add(CObject *pObj,bool bAddToEnd = true); 
	void Remove(CObject *pObj) {m_ObjInRoom.Remove(pObj);}
	bool IsExit(short nDir) {return (nDir>=NUM_OF_DOORS ? false : m_Doors[nDir]!=NULL);}
	bool DoesContain(CCharacter *pCh) {return m_CharactersInRoom.DoesContain(pCh);}
	inline CRoom *GetRoomToThe(short nDir);
	bool AddAffect(short nAffect,int nPulses=sAffect::PERM_AFFECT, int nValue=0);
	inline void RemoveAffect(short nAffect);
	void ReduceAffect(short nAffect,int nValue);
	//non inlined
public:
	short IsKeyWordForDoor(CString str,CString strDir);
	void DoShout(CString &strShout, CLinkList<CRoom> &UsedRooms, CCharacter *pCh, int nDistance);
	void BuildRoomStat(CString &strRoom);
	CObject * FindObjectInRoom(CString strObj,short nNum, const CCharacter *pLooker);
	CCharacter * FindCharacterByAlias(CString &Com,short nNum,CCharacter *pCh);
	void ReadRoom(CAscii &);
	void DeathCry(const CCharacter *pCh);
	void SendToRoom(CString strToRoom,const CCharacter *ch,const CCharacter *pch2 = NULL);
	void SendToRoom(CString strFormat,const CCharacter *pCh,CString strOne,const CCharacter *pCh2);
	void SendToRoom(CString strFormat,const CCharacter *pCh,const CCharacter *pCh2,CString strOne,CString strTwo);
	void SendToRoom(CString strFormat,const CCharacter *pCh,CString strForRoom);
	void SendToRoom(CString strToRoom);
	void SendToRoom(CString strFormat, CCharacter *pCh, CObject *pObj);
	bool IsInRoom(CCharacter *pCh);
	CObject * FindObjectInRoom(long lVnum);
	bool IsPCInRoom();
	CObject *GetMostValuableObject();
	CObject *GetRandomObject();
	int GetRoomBrightness() const;
	void GetExits(CString &str);
	short GetMovementCost();
	CRoom * CanEnter(CCharacter *pCh,CString &strDir, CString &strReverse);
	
public:
	virtual CString DoRoomLook(CCharacter *pLooker);
	virtual bool IsShop() const {return false;}
	virtual bool IsTransportRoom() {return false;}
	virtual void DoList(CString &strList);
	virtual void Remove(CCharacter *pCh);
	virtual void Add(CCharacter *pCh);
	virtual CString GetMap(const CCharacter *pLooker);
	virtual bool CanCampInRoom();
	virtual ~CRoom();
	//static const
public:
	CObject * IsObjectAffectedBy(short nAffect);
	bool OpenSecretDoor();
	CCharacter * IsAnyCharacterAffectedBy(short nAffect);
	void Wall(short nDir,bool bRemoving);
	void PurgeObjects(const CCharacter *pCh);
	short IsDoorKeyWord(CString str,CString strDir);
	short LocateChar(CCharacter *pCh, CLinkList<CRoom> &UsedLL, short nDistance);
	void AddAffectToObjects(short nAffect,int nPulses, int nValue);
	void AddAffectToCharacters(short nAffect,int nPulses, int nValue, bool bSendMsg);
	bool ContainsShip() const;
	void OpenDoor(short nDir, const CCharacter *pCh,bool bOpening);
	//room constants
	static const short TINY;
	static const short SMALL;
	static const short MEDIUM;
	static const short LARGE;
	static const short HUGE;
	static const short GIAGANTIC;
	static const short ENDLESS;
	/*ROOMTYPES*/ //used for colors
	static const short TYPE_INSIDE;
	static const short TYPE_FIELD;
	static const short TYPE_FOREST;
	static const short TYPE_HILLS;
	static const short TYPE_MOUNTAINS;
	static const short TYPE_WATERENTER;
	static const short TYPE_WATERNOENTER;
	static const short TYPE_UNDERWATER;
	static const short TYPE_UNUSED;
	static const short TYPE_DESERT;
	static const short TYPE_UNDERGROUND;
	static const short TYPE_SHOP;
	static const short TYPE_ROAD;
	static const short TYPE_SNOWMOUNTAINS;
	static const short TYPE_TUNDRA;
	static const short TYPE_DARKFOREST;
	static const short TYPE_SWAMP;
	static const short TYPE_BANK;

	/*Room Flags*/
	static const int DARK;
	static const int NOMOB;
	static const int INDOORS;
	static const int PEACEFULL;
	static const int NOSOUND;
	static const int NOTRACK;
	static const int NOMAGIC;
	static const int NO_SCAVENGE;
	static const int GODROOM;
	static const int FASTHEAL;
	static const int TUNNEL;
	static const int INN;
	static const int FLYING;
	static const int PERM_DARK;
	static const int PERM_LIGHT;
	static const int NO_CAMP;

	static const int ROOM_DARK;
	static const int ROOM_LIT;
	static const int ROOM_BRIGHT;
	static const int ROOM_TWILIGHT;

	static const int VIRTUAL_DOOR_NORTH;
	static const int VIRTUAL_DOOR_SOUTH;
	static const int VIRTUAL_DOOR_EAST;
	static const int VIRTUAL_DOOR_WEST;

	static const short ROOM_AFFECT_LIGHT;
	static const short ROOM_AFFECT_DARK;
	static const short ROOM_AFFECT_NOMOB;
	static const short ROOM_AFFECT_PEACEFUL;
	static const short ROOM_AFFECT_NOTRACK;
	static const short ROOM_AFFECT_NO_SOUND;
	static const short ROOM_AFFECT_GOD_ROOM;
	static const short ROOM_AFFECT_FAST_HEAL;
	static const short ROOM_AFFECT_TUNNEL;
	static const short ROOM_AFFECT_INN;
	static const short ROOM_AFFECT_NOMAGIC;
	static const short ROOM_AFFECT_NO_SCAVENGE;
	static const short ROOM_AFFECT_FLYING;
	static const short ROOM_AFFECT_INDOORS;
	static const short ROOM_AFFECT_PERM_DARK;
	static const short ROOM_AFFECT_PERM_LIGHT;
	static const short ROOM_AFFECT_NO_CAMP;
private:
	void SetRoomLoadAffects(int nAffects);
};

inline CRoom *CRoom::GetRoomToThe(short nDir) 
{
#ifdef _DEBUG
	assert(nDir<NUM_OF_DOORS);
#endif
	return ((nDir>=NUM_OF_DOORS || m_Doors[nDir]==NULL) ? NULL : m_Doors[nDir]->m_pToRoom);
}

//////////////////////////
//	Removes an affect
//	calls ending function if one.
//
inline void CRoom::RemoveAffect(short nAffect)
{
	if(m_Affects[nAffect].IsPermAffect())
	{
		return;
	}
	m_Affects[nAffect].Remove();
	if(m_AffectInfo[nAffect].m_pAffectFnc)
	{
		(this->*m_AffectInfo[nAffect].m_pAffectFnc)(nAffect,true);
	}
}
#endif

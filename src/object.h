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
*
*  Cobject header
*
* - written 9/26/97
*/

#ifndef _OBJECT_H_
#define _OBJECT_H_

#define TOTAL_OBJ_AFFECTS 40

#include "object.prototype.h"
#include "linklist.h"
#include "affect.h"
#include "object.save.h"
#include "wolfshade.string.h"

class CCharacter; //forward def
class CRoom;
struct sObjectSave;

class CObject 
{
	//protected constructor so only friend class can create =)
	friend class CObjectManager;
	friend struct sObjectSave;
protected:
	struct sAffectInfo
	{
		friend class CObject;
	protected:  
		CString m_strName,
			m_strAffectRate,
			m_strAffectRateRoom,
			m_strBattleRound,
			m_strBattleRoom,
			m_strEnd,
			m_strEndRoom;
		int m_nRate;
		void (CObject::*m_pFncBattleRound)(short);
		void (CObject::*m_pFncEnd)(short);
		void (CObject::*m_pFncRound)(short);
	public:
		sAffectInfo() {m_pFncBattleRound = m_pFncEnd = m_pFncRound = NULL;m_nRate = 0;}
		sAffectInfo(CString strName, 
			CString strAffectRate,
			CString strAffectRateRoom,
			CString strBattleRound,
			CString strBattleRoom, CString strEnd, 
			CString strEndRoom, 
			void (CObject::*fncRound)(short),
			void (CObject::*fncBattle)(short),
			void (CObject::*fncEnd)(short), 
			int nRate);
		sAffectInfo &operator=(const sAffectInfo &r)
		{
			m_strName=r.m_strName;
			m_strAffectRate = r.m_strAffectRate;
			m_strAffectRateRoom = r.m_strAffectRateRoom;
			m_strBattleRound = r.m_strBattleRound;
			m_strBattleRoom = r.m_strBattleRoom;
			m_strEnd=r.m_strEnd;
			m_strEndRoom=r.m_strEndRoom;
			m_pFncBattleRound=r.m_pFncBattleRound;
			m_pFncEnd = r.m_pFncEnd;
			return *this;
		}
	}; //end CAffectinfo
protected:
	static sAffectInfo m_AffectInfo[TOTAL_OBJ_AFFECTS];
	static bool m_bInit;
	static void InitStatics();
protected:
	sAffect m_Affects[TOTAL_OBJ_AFFECTS];
protected:
	CObject(CObjectPrototype *obj, CCharacter *PersonCarriedby=NULL, CRoom *pInRoom = NULL);
protected:
	CObject(CCharacter *pCh);
	int m_nVnum, 
		m_nVal0, 
		m_nVal1, 
		m_nVal2, 
		m_nVal3,
		m_nVal4,
		m_nWeight,
		m_nCost,
		m_nAffBit, 
		m_nWearBit;

	short m_nState,
		m_nObjType;

	CString m_strObjAlias, 
		m_strObjName, 
		m_strObjDesc, 
		m_strObjSittingDesc;

	//returns whether or not to remove the object

	CCharacter *m_pCarriedby; //ptr to who is carring
	CRoom *m_pInRoom; //ptr to room it's in if not carried
	CLinkList<CObjectDesc> ExtraDesc; 
	CLinkList<sObjectWearAffects> ObjAffects;

protected:
	static const short CAST_PCT_MAGIC_MISSILE;
	static const short DEATHS_MATRIARCH_PCT;
	static const short DIVINE_FATE_PCT;
	static const short SHADOW_ARMOR_TIME;
protected:
	void CastMagicMissile(short nDummy);
	void DeathsMatriarch(short nDummy);
	void DivineFate(short nDummy);
	void DelayedBlastFireball(short nDummy);
	void BlackBox(bool bRemoving,bool bSendMsg);
public:
	static const short STATE_GOOD;
	static const short STATE_DELETE;
	static const short OBJ_AFFECT_MIRAGE;
	static const short OBJ_AFFECT_CAST_MAGIC_MISSILE;
	static const short OBJ_AFFECT_HASTE;
	//Affect for object that gives hide
	static const short OBJ_AFFECT_HIDE;
	static const short OBJ_AFFECT_COLD_SHIELD;
	static const short OBJ_AFFECT_DETECT_INVIS;
	static const short OBJ_AFFECT_DEATHS_MATRIARCH;
	static const short OBJ_AFFECT_DIVINE_FATE;
	static const short OBJ_AFFECT_AIR_SHIELD;
	static const short OBJ_AFFECT_STONE_SKIN;
	static const short OBJ_AFFECT_SHADOW_ARMOR;
	static const short OBJ_AFFECT_DELAYED_BLAST_FIREBALL;
	static const short OBJ_AFFECT_HONE_WEAPON;
	static const short OBJ_AFFECT_MISSED_HONE;
	static const short OBJ_AFFECT_BOX; //Affect for "Little Black Box"
	static const short OBJ_AFFECT_INVISIBLE;
	//Affect for object that is hidden
	static const short OBJ_AFFECT_HIDDEN;
	static const short OBJ_AFFECT_CONTINUAL_LIGHT;
	static const short OBJ_AFFECT_CURRENTLY_WORN;
public:
	//bit vectors
	static const int ITEM_NOT_TAKEABLE;// (1<<0) //a
	static const int ITEM_NEWBIE;// (1<<1)  //b
	static const int ITEM_ANTI_GOOD;// (1<<2)  //c
	static const int ITEM_ANTI_NEUTRAL;// (1<<3) //d
	static const int ITEM_ANTI_EVIL;// (1<<4) //e
	static const int ITEM_ANTI_ANTIPALADIN;// (1<<5) //f
	static const int ITEM_ANTI_CLERIC;// (1<<6)  //g
	static const int ITEM_ANTI_DEFILER;// (1<<7)  //h
	static const int ITEM_ANTI_DRUID;// (1<<8)  //i
	static const int ITEM_ANTI_EMPATH;// (1<<9)  //j
	static const int ITEM_ANTI_MAGE;// (1<<10)  //k
	static const int ITEM_ANTI_MINDFLAYER;// (1<<11) //l
	static const int ITEM_ANTI_PALADIN;// (1<<12)  //m
	static const int ITEM_ANTI_PSIONICIST;// (1<<13) //n
	static const int ITEM_ANTI_RANGER;// (1<<14) //o
	static const int ITEM_ANTI_ROGUE;// (1<<15) //p
	static const int ITEM_ANTI_SHAMAN;// (1<<16) //q
	static const int ITEM_ANTI_WARRIOR;// (1<<17) //r
	static const int ITEM_ANTI_GOOD_RACE;// (1<<18) //s
	static const int ITEM_ANTI_NEUTRAL_RACE;// (1<<19) //t
	static const int ITEM_ANTI_EVIL_RACE;// (1<<20) //u
	static const int ITEM_ANTI_ALIEN_RACE;//(1<<21) //v
public:
	virtual CObject *IsContentsAffectedBy(short nAffect) {return NULL;}
	virtual void WearAffects(bool bRemoving);
	virtual void Open(const CCharacter *pCh,bool bOpen);
	virtual void SaveContents(std::ofstream &ObjFile) {return;}
	virtual bool IsValidObject() {return true;} //TODO make =0; pure virtual
	virtual bool Add(CObject *pObj){return false;}
	virtual CRoom *Add(CCharacter *pCh);
	virtual void Remove(CObject *pObj){;}
	virtual void GetContents(const CCharacter *pLooker, CString &str){;}
	virtual int GetContentsCount() {return (int)0;}
	virtual void LoadContents(std::ifstream &ObjFile, short nCount) {return;}
	virtual void DeleteContents() {return;}
	virtual void Examine(const CCharacter *pLooker,CString &strExamine);
	virtual int GetWeight() const {return m_nWeight;}
	virtual void FadeType(short nAffect);
	virtual CString GetExtraString();
	virtual void FadeAffects(long lTimeToRemove);
	virtual void Loot(CString str, short nHowMany, CCharacter *pCh);
	virtual void Loot(CCharacter *pCh,bool bFromRess = false);
	virtual void Save(std::ofstream &s, int nWornOn, short nPos);
	virtual void DoLook(CString &str);
	virtual void AddCharAffects(bool bRemoving,bool bSendMsg);
	virtual ~CObject();

public:
	CObject(const sObjectSave &ObjSave,CCharacter *pChar,CRoom *pRoom);
	CObject(const CObject &copy); //broken
	bool ByAlias(CString &strItem);
	void RemoveAllAffects();
	void RemoveAffect(short nAffect);
	bool AddAffect(short nAffect,int nPulses=sAffect::PERM_AFFECT, int nValue=0);
	void DoBattle();
	bool CanbeWornBy(const CCharacter *pCh);
public: //inlined
	virtual void Use(CCharacter *pUser);
	void SetDelete() {m_nState = STATE_DELETE;}
	CRoom *GetRoom() {return m_pInRoom;}
	bool IsType(short nType) {return m_nObjType==nType;}
	void Set(CCharacter *ch,CRoom *pRoom) {m_pCarriedby = ch;m_pInRoom = pRoom;}
	const CCharacter *IsBeingCarriedby() {return m_pCarriedby;}
	obj_vnum GetVnum() {return m_nVnum;}
	const char *GetObjSittingDesc() {return m_strObjSittingDesc.cptr();}
	const char *GetObjName() {return m_strObjName.cptr();}
	const char *GetObjName(const CCharacter *pCh);
	CString GetAlias() {return m_strObjAlias;}
	CString GetDesc() {return m_strObjDesc;}
	const char *GetSittingDesc() {return m_strObjSittingDesc.cptr();}
	int GetObjectVal0() {return m_nVal0;}
	unsigned int GetWearBit() const {return m_nWearBit;}
	bool IsWeapon() {return ITEM_TYPE_WEAPON==m_nObjType;}
	//GetDicetype should be weapon only! deal with when we put inheritance sceem together
	int GetDiceType(short nSize) {return (nSize <= SIZE_MEDIUM ? m_nVal1 : m_nVal3);}
	int GetNoOfDice(short nSize) {return (nSize <= SIZE_MEDIUM ? m_nVal0 : m_nVal2);}
	int GetMessageType() {return m_nVal4;}
	int GetCost() {return m_nCost;}
	short GetObjectType() {return m_nObjType;}
	int GetACChange() {return m_nVal0;}
	bool IsAffectBit(UINT nBit) {return (m_nAffBit & nBit) ? true : false;}
	void SetObjectSittingDesc(CString strSet) {m_strObjSittingDesc = strSet;}
	void SetObjectName(CString strSet) {m_strObjName= strSet;}
	void SetObjectDesc(CString strSet) {m_strObjDesc = strSet;}
	bool ShouldBeDeleted() {return m_nState==STATE_DELETE;}
	bool IsAffectedBy(short nAffect) {return m_Affects[nAffect].HasAffect();}
	int GetAffectValue(short nAffect) {return m_Affects[nAffect].m_nValue;}
	CString GetIdentifyString();
};
#endif
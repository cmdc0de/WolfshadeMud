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
*  a member of CGame
*		This is our interpretor... It interprets commands
*	by seeing if the command in the characters command buffer is
*	in the command list then can implement logic then call the polymorphic
*	function or just call polymorphic function, or just use functions defined
*	here. Such as socials will be handled here.
*/

#ifndef _INTERP_H_
#define _INTERP_H_

#include "character.h"
#include "object.manager.h"
#include "hash.h"

class CGame; //forward def

class CInterp
{
private:
	CGame *m_pGame;
	struct sComClass
	{
	public:
		CString CommandName;
		void (CInterp::*CommandFnc)(CCharacter *ptr);
		short int MinLevel;
		int m_nPositionCantBeIn,
			m_nAffectsToRemove;
		inline sComClass(const char *name,void (CInterp::*Comm)(CCharacter *ptr),int minl, int nPositionCantBeIn, int m_nAffectsToRemove=0);
		bool IsGodCommand() {return MinLevel>=LVL_IMMORT;}
	};
	struct sHelp
	{
		long m_lStartPos,
			m_lStopPos;
	};
private:
	CString m_strCommandHelp;
	CString m_strGodHelp;
	void BuildHelp();
	CMap<CString,sComClass *> *m_pCmdTable;
	std::ofstream BugFile;
	std::ifstream m_HelpFile;
	CMap<CString,sHelp *> m_HelpTable;
	void BuildCommands();
private:
	void BuildHelpOnCommand(CString &str,bool bGod);
	static const char *m_strHelpFileName;
	static const char * m_strBasicHelp;
	static const char HELP_FILE_DELIM;

private:
	static const int REMOVE_HIDE;
	static const int REMOVE_MEDITATE;
	static const int REMOVE_INVIS;
	static const int REMOVE_CAMP;
public:
	void RemoveFighter(CCharacter *pFighter) const;
	const CObjectManager *GetObjManager() const;
	void KillChar(CCharacter *ch) const;
	CObject * CreateObj(obj_vnum lObjVnum,CCharacter *pCh) const;
	void CreatePortal(CRoom *pTargetRoom,CRoom *pCasterRoom, short nAffect, int nPulses) const;
	CCharacter *CreateUndead(const sUndeadInfo *pUndeadInfo, CRoom *pRoom) const;
	CCharacter *CreateWall(const sWallInfo *pWallInfo, CRoom *pRoom) const;
	CCharacter *CreateMob(int MobVnum, CRoom *pPutInRoom) const;
	CCharacter *CreateMob(int MobVnum, CRoom *pPutInRoom,int nHp, int nLevel) const;
	void CreateFireball(CRoom *pTargetRoom, int nPulses, int nDam) const;
	CObject *CreateObj(obj_vnum nVnum,CRoom *pRoom) const;
	void RoomStat(CCharacter *pCh);
	void InterpretCommand(CCharacter *ch);
	~CInterp();
	CInterp(CGame *pGame);
	
protected:
	void Practice(CCharacter *ch);
	void Scan(CCharacter *pCh);
	void Create(CCharacter *ch);
	void Vnum(CCharacter *ch);
	void Transport(CCharacter *ch);
	void AdvanceLevel(CCharacter *ch);
	void Vstat(CCharacter *ch);
	void Help(CCharacter *pCh);
	void Disengage(CCharacter *pCh);
	void GroupSay(CCharacter *pCh);
	void Group(CCharacter *pGrouper);
	void Consent(CCharacter *pCh);
	void Follow(CCharacter *pCh);
	void Map(CCharacter *pCh);
	void Restore(CCharacter *pCh);
	void Who(CCharacter *ch);
	void Rent(CCharacter *pCh);
	void Order(CCharacter *pCh);
	void Examine(CCharacter *pCh);
	void Save(CCharacter *pCh);
	void Set(CCharacter *pCh);
	void Echo(CCharacter *pCh);
	void Bug(CCharacter *pCh);
	void Spy(CCharacter *pCh);
	void Shout(CCharacter *pCh);
	void GodLocate(CCharacter *pCh);
	void Initiate(CCharacter *pCh);
	void Read(CCharacter *pCh);
protected: //directly defined
	void Exit(CCharacter *pCh);
	void Wake(CCharacter *pCh);
	void Search(CCharacter *pCh) {bool b;pCh->DoSearch(pCh->GetTarget(1,false,b));}
	void GodChange(CCharacter *pCh) {pCh->GodChange();}
	void Innate(CCharacter *pCh) {pCh->DoInnate();}
	void HomeTown(CCharacter *pCh) {pCh->DoHomeTown();}
	void OOC(CCharacter *pCh) {pCh->DoOOC();}
	void Drag(CCharacter *pCh) {pCh->DoDrag();}
	void Purge(CCharacter *pCh) {pCh->GetRoom()->PurgeObjects(pCh);}
	void Disband(CCharacter *pCh) {pCh->GetGroup().Disband(pCh);}
	void Split(CCharacter *pCh) {pCh->DoSplit();}
	void Sleep(CCharacter *pCh) {pCh->UpDatePosition(POS_SLEEPING);}
	void Track(CCharacter *pCh) {pCh->DoTrack();}
	void News(CCharacter *pCh) {pCh->SendNews();}
	void Flee(CCharacter *pCh){pCh->DoFlee();}
	void Ask(CCharacter *pCh) {pCh->DoAsk(pCh->GetTarget(1,false));}
	void Kill(CCharacter *pCh){pCh->DoKill(this,pCh->GetTarget());}
	void Assist(CCharacter *pCh){pCh->DoAssist(pCh->GetTarget());}
	void Wear(CCharacter *pCh) {pCh->DoWear();}
	void Equipment(CCharacter *pCh) {pCh->DoEquipment();}
	void Attribute(CCharacter *ch) {ch->DoAttributes();}
	void Look(CCharacter *ch){ch->DoLook();}
	void Get(CCharacter *ch){ch->DoGet();}
	void Enter(CCharacter *ch){ch->DoEnter();}
	void Inventory(CCharacter *ch){ch->DoInventory();}
	void Say(CCharacter *ch);
	void Emote(CCharacter *ch){ch->DoEmote();}
	void Tell(CCharacter *ch) {ch->DoTell();}
	void ShutDownWolfshade(CCharacter *ch){ch->ConfirmShutDown();}
	void Skill(CCharacter *ch) {ch->DoSkills();}
	void Wield(CCharacter *ch) {ch->DoWield();}
	void Remove(CCharacter *ch) {ch->DoRemove();}
	void Cast(CCharacter *ch) {ch->DoCast(this,false);}
	void FCast(CCharacter *ch) {ch->DoCast(this,true);}
	void Prepare(CCharacter *ch) {ch->DoPrepare();}
	void Spells(CCharacter *ch) {ch->DoSpells();}
	void Rest(CCharacter *ch) {ch->UpDatePosition(POS_RESTING);}
	void Stand(CCharacter *ch) {ch->UpDatePosition(POS_STANDING);}
	void Sit(CCharacter *ch) {ch->UpDatePosition(POS_SITTING);}
	void Recline(CCharacter *ch) {ch->UpDatePosition(POS_RECLINING);}
	void Score(CCharacter *ch) {ch->DoScore();}
	void BackStab(CCharacter *ch) {ch->DoBackStab(this,ch->GetTarget());}
	void ThroatSlit(CCharacter *ch) {ch->DoThroatSlit(this,ch->GetTarget());}
	void Bash(CCharacter *ch) {ch->DoBash(this,ch->GetTarget());}
	void BodySlam(CCharacter *ch) {ch->DoBodySlam(this,ch->GetTarget());}
	void Kick(CCharacter *ch) {ch->DoKick(this,ch->GetTarget());}
	void Trip(CCharacter *ch) {ch->DoTrip(this,ch->GetTarget());}
	void Sneak(CCharacter *ch) {ch->DoSneak(this);}
	void Hide(CCharacter *ch) {ch->DoHide();}
	void Circle(CCharacter *ch) {ch->DoCircle(this);}
	void Consider(CCharacter *ch) {ch->DoConsider();}
	void Bandage(CCharacter *ch) {ch->DoBandage();}
	void Beg(CCharacter *ch) {ch->DoBeg();}
	void Charm(CCharacter *ch) {ch->DoCharm();}
	void Chant(CCharacter *ch) {ch->DoChant();}
	void List(CCharacter *ch) {ch->DoList();}
	void Report(CCharacter *ch) {ch->DoReport();}
	void Think(CCharacter *ch) {ch->DoThink(this);}
	void Will(CCharacter *ch) {ch->DoWill(this);}
	void Buy(CCharacter *ch) {ch->DoBuy(this);}
	void Sell(CCharacter *ch) {ch->DoSell(this);}
	void Drop(CCharacter *ch) {ch->DoDrop();}
	void Loot(CCharacter *ch) {ch->DoLoot();}
	void Give(CCharacter *ch) {ch->DoGive();}
	void Mediate(CCharacter *ch) {ch->DoMediate();}
	void Forget(CCharacter *ch) {ch->DoForget();}
	void SpellList(CCharacter *ch) {ch->SendSpellList();}
	void SkillList(CCharacter *ch) {ch->SendProficiency();}
	void Glance(CCharacter *ch) {ch->DoGlance();}
	void Experience(CCharacter *ch) {ch->DoExperience();}
	void Identify(CCharacter *pCh) {pCh->DoIdentify();}
	void Title(CCharacter *ch);
	void Rescue(CCharacter *pCh) {pCh->DoRescue(pCh->GetTarget(1,false));}
	void Worship(CCharacter *pCh) {pCh->DoWorship();}
	void FugueTime(CCharacter *pCh) {pCh->FugueTime();}
	void Strike(CCharacter *pCh) {pCh->DoStrike(this,pCh->GetTarget());}
	void Put(CCharacter *pCh) {pCh->DoPut();}
	void HeadButt(CCharacter *pCh) {pCh->DoHeadButt(this,pCh->GetTarget());}
	void ShieldRush(CCharacter *pCh) {pCh->DoShieldRush(this,pCh->GetTarget());}
	void Berserk(CCharacter *pCh) {pCh->DoBerserk();}
	void Flurry(CCharacter *pCh) {pCh->DoWeaponFlurry(this);}
	void DragonPunch(CCharacter *pCh) {pCh->DoDragonPunch(this,pCh->GetTarget());}
	void QuiveringPalm(CCharacter *pCh) {pCh->DoQuiveringPalm(this,pCh->GetTarget());}
	void BearHug(CCharacter *pCh) {pCh->DoBearHug(this,pCh->GetTarget());}
	void Appraise(CCharacter *pCh) {pCh->DoAppraise();}
	void Hone(CCharacter *pCh) {pCh->DoHoneWeapon();}
	void Intimidate(CCharacter *pCh) {pCh->DoIntimidate(this);}
	void HitAll(CCharacter *pCh) {pCh->DoHitAll(this);}
	void Disarm(CCharacter *pCh) {pCh->DoDisarm(this, pCh->GetTarget());}
	void GodKill(CCharacter *pCh) {pCh->DoGodKill(this);}
	void DirtToss(CCharacter *pCh) {pCh->DoDirtToss(pCh->GetTarget());}
	void LayHands(CCharacter *pCh) {pCh->DoLayHands();}
	void Tog(CCharacter *pCh) {pCh->DoTog();}
	void Switch(CCharacter *pCh) {pCh->DoSwitch();}
	void Return(CCharacter *pCh) {pCh->DoReturn();}
	void Time(CCharacter *pCh) {pCh->SendToChar(CMudTime::GetTimeString());}
	void CalledShot(CCharacter *pCh) {pCh->DoCalledShot(this);}
	void Move(CCharacter *pCh) {pCh->DoMove(pCh->CurrentCommand);}
	void Open(CCharacter *pCh) {pCh->DoOpen(true);}
	void Close(CCharacter *pCh) {pCh->DoOpen(false);}
	void Disembark(CCharacter *pCh) {pCh->DoDisembark();}
	void Deposit(CCharacter *pCh) {pCh->GetMoneyFromBank(false);}
	void Withdraw(CCharacter *pCh) {pCh->GetMoneyFromBank(true);}
	void FirstAid(CCharacter *pCh) {pCh->DoFirstAid();}
	void Motd(CCharacter *pCh) {pCh->SendMotd();}
protected:
	void UseObject(CCharacter *pUser);
	void GRelease(CCharacter *pCh);
	void GodFreeze(CCharacter *pCh);
	void Vis(CCharacter *pCh);
	void Refresh(CCharacter *pCh);
	void GodForce(CCharacter *pCh);
	void GodRes(CCharacter *pCh);
	void GodMessage(CCharacter *pCh);
	void FragList(CCharacter *pCh);
	void Sail(CCharacter *pCh);
	void Camp(CCharacter *pCh);
	void Cutlink(CCharacter *pCh);
	void DoGodChat(CCharacter *pCh);
	void DoGoto(CCharacter *pCh);
	void DoPetition(CCharacter *pCh);
	void Show(CCharacter *pCh);
};

inline CInterp::sComClass::sComClass(const char *name,void (CInterp::*Comm)(CCharacter *ptr),int minl, int nPositions, int nAffectsToRemove)
{
	CommandName = name;
	CommandFnc = Comm;
	MinLevel = minl;
	m_nPositionCantBeIn = nPositions;
	m_nAffectsToRemove = nAffectsToRemove;
}
#endif

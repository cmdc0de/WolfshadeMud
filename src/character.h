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
 * CCharacter Class
 *	Here is our base abstract class for all characters.
 *	Variables:
 *		All variables need to define any character
 *	functions:
 *		virtual functions so that we can rewrite them at the 
 *		class type level.  All commands are genericaly
 *		defined here so that we can overwrite for class or race
 *		specific.
 *
 */

#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "mob.prototype.h"
#include "character.attributes.h"
#include "char.intermediate.h"
#include "message.manager.h"
#include "wolfshade.string.h" 
#include "proficiency.h"
#include "list.h"
#include "trackinfo.h"

#define SPELL_NO_AFFECT -1

#define MAX_CASTING_STARS 30;

class CInterp;

class CCharacter : public CCharacterAttributes {
    friend class CNPC;
    friend class CInterp;
    friend class CCleric; // resurrection
    friend CCorpse::CCorpse(CCharacter *pCh);
    friend void CObject::WearAffects(bool bRemoving);
    friend CCharacter * CalcClassPtr(CCharIntermediate *ch);
    friend CCharacter * CalcClassPtr(CMobPrototype *mob, CRoom *pRoom);
private:

    struct sAffectInfo {
        friend class CCharacter;
    protected:
        int m_nRateOfAffect;
        CString m_strName;
        bool m_bAcumTime,
        m_bAcumValue,
        m_bReferenceAffect;
        void (CCharacter::*m_pFncApply)(short nAffect, bool bRemoving);
        void (CCharacter::*m_pFncRound)();
    public:

        sAffectInfo() {
            m_pFncApply = NULL;
            m_pFncRound = NULL;
            m_nRateOfAffect = 0;
            m_bAcumTime = m_bAcumValue = m_bReferenceAffect = false;
        }
        sAffectInfo(CString strName,
                bool bAcumTime,
                bool bAcumValue,
                int nRate, //for round function
                void (CCharacter::*m_pFncApply)(short nAffect, bool bRemoving),
                void (CCharacter::*fRound)(),
                bool bReferenceAffect = false);

        sAffectInfo &operator=(const sAffectInfo &r) {
            m_strName = r.m_strName;
            m_pFncRound = r.m_pFncRound;
            m_pFncApply = r.m_pFncApply;
            m_nRateOfAffect = r.m_nRateOfAffect;
            m_bAcumTime = r.m_bAcumTime;
            m_bAcumValue = r.m_bAcumValue;
            m_bReferenceAffect = r.m_bReferenceAffect;
            return *this;
        }

        bool IsReferenceAffect() {
            return m_bReferenceAffect;
        }
    };

    struct sMaster {
        friend class CCharacter;
        friend class CNPCMage;
        friend class CNPC;
    private:
        static const short MASTERED_OFF;
        int m_nTimeIAmMastered;
        CCharacter *m_pMaster;
        CList<CCharacter*> m_MasterOf;
    protected:

        void Add(CCharacter *pCh) {
            m_MasterOf.Add(pCh);
        }
    public:

        sMaster() {
            m_pMaster = NULL;
            m_nTimeIAmMastered = -1;
        }
        void SetMaster(CCharacter *pMaster, CCharacter *pMasterie, int nTime, bool bMakeFollow = true);

        bool IsMasterOf(CCharacter *pCh) {
            return m_MasterOf.DoesContain(pCh);
        }
        void Remove(CCharacter *pCh);

        bool IsAMaster() {
            return !m_MasterOf.IsEmpty();
        }

        bool HasMaster() const {
            return (m_pMaster != NULL);
        }

        CCharacter *GetMaster() const {
            return m_pMaster;
        }
        void ReduceMastersHold(int nTime, CCharacter *pOwner);
        void CleanUp(CCharacter *pOwner);
    };
    friend struct sMaster;

    struct sFollower {
        CCharacter *m_pCharacterBeingFollowed;
        CLinkList<CCharacter> m_Followers;
        void SetCharToFollow(CCharacter *pCh);

        bool IsFollowing() {
            return m_pCharacterBeingFollowed != NULL;
        }

        sFollower() {
            m_pCharacterBeingFollowed = NULL;
        }

        bool HasFollowers() {
            return !m_Followers.IsEmpty();
        }

        bool HasFollower(CCharacter *pCh) {
            return m_Followers.DoesContain(pCh);
        }
        void Remove(CCharacter *pCh);
        void CleanUp(CCharacter *pOwner);
    };
    friend struct sFollower;

    struct sConsent {
        CCharacter *m_pCharYouConsented;
        CLinkList<CCharacter> m_CharsThatConsentedYou;

        sConsent() {
            m_pCharYouConsented = NULL;
        }

        CCharacter *GetConsented() const {
            return m_pCharYouConsented;
        }
        void CleanUp(CCharacter *pOwner);
    };
    friend struct sConsent;

    struct sGroup {
        friend class CCharacter;
    protected:

        void NewGroup(CCharacter *pHead) {
            m_pGroup = new CList<CCharacter *>();
            m_pGroup->Add(pHead);
        }

        bool IsHead(CCharacter *pCh) {
            return m_pGroup ? m_pGroup->GetFirst() == pCh : false;
        }
        CList<CCharacter *> *m_pGroup; //if null not grouped
        CList<CCharacter *> *NullGroup();
    public:
        sGroup(const sGroup &copy); //broke

        sGroup() {
            m_pGroup = NULL;
        }
        void Remove(CCharacter *pCh);

        void SetGroup(CList<CCharacter *> *pGroup) {
            m_pGroup = pGroup;
        }

        bool IsInGroup(CCharacter *pCh) {
            return (m_pGroup ? m_pGroup->DoesContain(pCh) : false);
        }

        bool IsGrouped() {
            return m_pGroup != NULL;
        }
        void Split(CCharacter *pCh, sMoney MoneyToSplit);

        void RemoveAll() {
            //set character to null so everyone gets the message
            SendToGroup(CString("Your group has been disbanded.\r\n"), NULL);
            delete NullGroup();
        }
        void Disband(CCharacter *pCh);
    public:
        void GroupGainExp(int nExp, CCharacter *pKiller, CCharacter *pDeadChar);
        void BuildGroupString(CString &strGroup);
        void SendToGroup(CString strToChar, CCharacter *pCh);
        void AddToGroup(CCharacter *pGrouper, CCharacter *pCh);
        void GuksMantle(CCharacter *pCaster, short nTime, short nAmount);
        void DuthsWarmth(CCharacter *pCaster, short nAmount);
    }; //end group class
    friend struct CCharacter::sGroup;

    struct sPageInfo {
        CString m_strPage;
        int m_nReturnsSent;

        sPageInfo() {
            m_nReturnsSent = 0;
        }

        void Empty() {
            m_strPage.Empty();
            m_nReturnsSent = 0;
        }
    };
    friend class CTrackInfo;
private:
    void SyncWearAffects();
    void ApplyAffect(short nAffect, bool bRemoving);
    void GetSingleItem(CObject *pObj);
    bool DropSingleItem(CObject *pObj);
    bool PutObjInObj(CObject *pObj, CContainer *pCon);
    void GetObjFromObj(CObject *pObj, CContainer *pCon);
    void GainExp(CCharacter *pDefender, int nDam);
    void AgeDeath();
private:
    void SendToSocket(const CString &str) const;
    long m_lVoidTime;
protected:
    static const char *CastingMsg[];
    static const char * strPageInfo;
    static sAffectInfo m_AffectInfo[TOTAL_AFFECTS];
    static bool m_bInit;
    static short GAME_VAR_AC_WOULD_HIT;
    static short GAME_VAR_DEFENSE_DODGE;
    static short GAME_VAR_DEFENSE_PARRY;
    static short GAME_VAR_DEFENSE_RIPOSTE;
    static short GAME_VAR_SIZE_AC_ADJUST;
    static short DIE_ROLL_FOR_SKILL_UP;
    //load
    static short LOAD_WHAT_LOAD;
    static short LOAD_LIGHT;
    static short LOAD_MODERATE;
    static short LOAD_HEAVY;
    static short LOAD_VERY_HEAVY;
    static short LOAD_IMMOBILIZING;
protected:
    bool m_bMakeCorpseOnDeath; //make corpse upon death?
    //m_pSpyingOn and m_pSwitchedTo Could be variables in
    //a derived class for gods only?  would be more OO
    //but is it worth the work?
    CCharacter *m_pFighting,
    *m_pSwitchedTo, //who are you switched to
    *m_pSpiedby, //who is spying you
    *m_pSpyingOn; //who you are spying on

    short m_nToHit;
    short m_nDamRoll;
    short m_nAC;
    int m_nWeightCarried;
    long m_lLagTime;

    CString LastCommand;

    CRoom *m_pInRoom;

    CEqInfo *m_Equipment;

    sDescriptor *m_pDescriptor;

    sFollower m_Follow;
    sConsent m_Consent;
    sGroup m_Group;
    sMaster m_Master;
    sPageInfo m_PageInfo; //for sending pages to chars
    CTrackInfo *m_pTrackInfo;

public: //statics
    static const CMessageManager m_MsgManager;
protected:
    static void InitStatics();
    //inline publics
public:

    void DoInnate() {
        m_pAttributes->m_pRaceReference->DoInnate(this, CurrentCommand.GetWordAfter(1, true));
    }

    stat_type GetAgility() {
        return m_pAttributes->GetStat(AGI);
    }

    stat_type GetConstitution() {
        return m_pAttributes->GetStat(CON);
    }
    inline const char *GetRaceWarName();

    bool IsInRoom(const CCharacter *pCh) const {
        return (pCh == NULL ? false : pCh->IsDead() ? false : pCh->GetRoom() == this->GetRoom());
    }

    void AdjustAC(short nAdj) {
        m_nAC = (m_nAC + nAdj > 100) ? 100 : (m_nAC + nAdj<-100) ? -100 : m_nAC + nAdj;
    }

    bool ShouldMakeCorpse() {
        return m_bMakeCorpseOnDeath;
    }

    void SetNoCorpse() {
        m_bMakeCorpseOnDeath = false;
    }
    inline void AddCommand(CString str);
    inline short GetLoadPct(const CObject *pBeingDraged = NULL) const;

    bool HasInnate(int nInnate) const {
        return m_pAttributes->m_pRaceReference->HasInnate(nInnate);
    }

    bool HasMaster() const {
        return m_Master.HasMaster();
    }

    bool CommandBufEmpty() {
        return (m_pDescriptor != NULL ? m_pDescriptor->CommandBuf.IsEmpty() : false);
    }

    int CurrentlyMasterOf() const {
        return m_Master.m_MasterOf.GetCount();
    }

    void SetMaster(CCharacter *pCh, int nTime, bool bMakeFollow = true) {
        m_Master.SetMaster(pCh, this, nTime, bMakeFollow);
    }

    CCharacter *GetMaster() {
        return m_Master.GetMaster();
    }

    void LagChar(long lLag) {
        m_lLagTime += lLag;
    }

    bool IsLagged() {
        return m_lLagTime > 0;
    }

    const CCharacter *GetConsented() const {
        return m_Consent.GetConsented();
    }

    sGroup &GetGroup() {
        return m_Group;
    }

    CCharacter *IsFollowing() {
        return m_Follow.m_pCharacterBeingFollowed;
    }
    inline bool CanSeeName(const CCharacter *pCh) const;
    inline bool CanSee(CObject *pObj) const;

    bool CanFight() {
        return (!InPosition(POS_INCAP) && !InPosition(POS_DEAD) && !InPosition(POS_MORTALED) && !InPosition(POS_KOED));
    }

    bool IsDead() const {
        return InPosition(POS_DEAD);
    }

    CRoom *GetRoom() const {
        return m_pInRoom;
    }

    CCharacter *IsFighting() {
        return m_pFighting;
    }
    //this might need to be CDescriptor instead of player descriptor

    void SetDescriptor(sPlayerDescriptor *pDesc) {
        m_pDescriptor = pDesc;
    }
    inline sPlayerDescriptor *KillSocket();
    inline void AssertFighting(CCharacter *pCh);

    bool IsLinkDead() const {
        return m_pDescriptor ? m_pDescriptor->IsLinkDead() : true;
    }

    bool ShouldCutLink() {
        return m_pDescriptor ? m_pDescriptor->IsLinkDead() : false;
    }

    bool ShouldRent() {
        return m_pDescriptor ? m_pDescriptor->ShouldRent() : false;
    }
    inline bool IsPlaying();

    sPlayerDescriptor * GetDescriptor() {
        return (sPlayerDescriptor *) m_pDescriptor;
    }

    short int GetSex() {
        return m_pAttributes->AffectedSex;
    }
    inline void RemovePosition(UINT nRemove);

    bool MakeSavingThrow(short nSave) {
        return m_pAttributes->MakeSavingThrow(nSave);
    }
    inline bool Add(CObject *pObj, bool bAddEnd = true, bool bForceAdd = false);
    inline bool Add(const sMoney Mon, bool bForceAdd = false);
    inline void Remove(CObject *pObj);
    inline void Remove(CObject *pObj, POSITION &pos);

    bool IsBeingSpied() const {
        return m_pSpiedby != NULL;
    }

    bool IsSpying() const {
        return m_pSpyingOn != NULL;
    }
    //non inlined
public:
    bool EnoughExpForNextLevel(bool bLastMortalLevelRestrict = true);
    void PutAllEqInRoom();
    void DoAutoFlee();
    bool MakeSkillSave(short nSkillToSaveAgainst, short Adjustments);
    void LoadObjects();
    bool CanSeeChar(const CCharacter *pCh) const;
    short CanSeeInRoom(const CRoom *pCh) const;
    CObject *RemoveEquipment(int nPos, bool bSendMsg);
    void WearEquipment(CObject *pObj, UINT nWearBit, int nPos, bool bSendMsg);
    bool CheckAge();
    CEqInfo &GetEq(short nPos);
    void SpellDefense(int nDam, CCharacter * pAttacker, short nLevel);
    void ReleaseMemoryForObjects();
    void RemoveObjFromChar(CObject *pObj);
    CString GetRaceOrName(const CCharacter *pCh) const;
    void ConfirmShutDown();
    void EndCurrentCommand();
    void DoSkills();
    void DoOtherState();
    void UpDatePosition();
    void SendPrompt();
    void Wear(CObject *pObj);
    void DoLook();
    CCharacter * GetTarget(int nWordAfter = 1, bool bAllowFight = true);
    CObject * GetTarget(int nWordAfter, bool bInventory, bool &bAll);
    void AddFighters(CCharacter *pTarget, bool bAddMeToo = true);
    void AddAffectString(CString &strLook);
    void Regenerate();

    void SetTitle(CString strTitle) {
        m_strTitle = strTitle;
    }
    int DoProtectionMelee(CCharacter *pAttacker, int nDam);
    int DoProtectionSpells(CCharacter *pAttacker, int nDam, int nSpellLevel);
    void GetEnchantments(CString &str);
    void RemoveAffect(short nAffect, bool bSendMsg = true);
    void ReduceAffect(short nAffect, int nPulses, int nValue, bool bSendMsg);
    //TODO MERGE REMOVEAFFECT AND REDUCEAFFECT

    int GetAffectValue(short nAffect) {
        return m_Affects[nAffect].m_nValue;
    }
    void RemoveAllAffects(bool bSendMsg);
    bool AddAffect(short nAffect, int nPulses = sAffect::PERM_AFFECT, int nValue = 0, bool bSendMsg = true);
    void GetInventoryNames(CString &str, int nNumOfItems);
    //non virtual protected functions
protected:
    //protected constructor so we can create an CCharacter by itself
    //not that you could considering it's an abstract class
    CCharacter(CMobPrototype &mob, CRoom *pPutInRoom);
    CCharacter(CCharIntermediate *ch);
    skill_type GetSkill(short nSkill) const;
    void DoHomeTown();
    void InitSkills();
    void DoTell();
    void DoSay();
    void DoEmote();
    void DoWorship();
    void DoInventory();
    void DoGet();
    void EqList(const CCharacter *pLooker, CString &strEq);
    void DoLoot();
    void DoGive();
    void DoWear();
    void DoWield();
    void DoEquipment();
    void DoReport();
    void DoList();
    void DoScore();
    void DoDrop();
    void DoConsider();
    void DoBandage();
    void DoEnter();
    void DoBodySlam(const CInterp *pInterp, CCharacter *pTarget);
    void DoExperience();
    void DoGlance();
    CObject * FindInInventory(CString &strItem, short nNum);
    const char *GetCondition();
    void DoRemove();
    void DoMove(CString strDirection, CObject *pObjectBeingDragged = NULL);
    void DoSell(const CInterp *pInterp);
    void DoBuy(const CInterp *pInterp);
    void SkillUp(short nSkill);
    void DoPut();
    void DoGodKill(const CInterp *pInterp);
    void DoSwitch();

    bool IsSwitched() const {
        return m_pSwitchedTo != NULL;
    }
    void DoAssist(CCharacter *pTarget);
    void DoKill(const CInterp *pInterp, CCharacter *pTarget);
    bool CanAffordTraining(sMoney &Cost, short nSkill, const CCharacter *pToBeTrained, bool bIsSkill);
    void BuildPracticePriceSkills(const CCharacter *pToBeTaught, CString &str) const;
    void DoAsk(const CCharacter *pTarget);
    void DoFlee();
    bool CanBodySlam(CCharacter *pTarget);
    const char * GetLoadWording();
    void DoOpen(bool bOpening);
    void AdjustAlignment(const CCharacter *pCh);
    void DoSplit();
    void DoTrack();
    void SendNews();
    void DoDisembark();
    void GetMoneyFromBank(bool bWithdraw);
    void DoOOC();
    void DoFirstAid();
    void GodChange();
    void DoSearch(CObject *pObj);
protected: //inlined

    short GetMaxWeaponPositions() {
        return m_pAttributes->m_pRaceReference->GetMaxWeapons();
    }

    short GetWeaponPosition(short n) {
        return m_pAttributes->m_pRaceReference->GetWeaponPosition(n);
    }

    bool IsKillFrag(const CCharacter *pCh) const {
        return (!CanSeeName(pCh) && pCh->GetLevel() >= MIN_FRAG_LEVEL && ((this->GetLevel() - MAX_FRAG_LEVEL_DIFF) <= pCh->GetLevel()));
    }

    void SendMotd() {
        SendToChar(MOTD);
    }
    //virtual functions
protected:

    virtual void DoBreakMastersHold() {
        return;
    } //nothing special for pc's
    virtual void InitializeCharacter();
    virtual void ResetCharVars();

    virtual int GetExtraExp() {
        return 0;
    } //only used for npc
    virtual bool IsGoodEnoughToTeach(const CCharacter *pToBeTaught, short nSkill, bool bSkill) const;
    virtual void DoDrag();
    virtual short int CalcDamage(short nPos, CCharacter *pDefender);
    virtual void DoExchange();
    virtual bool SetSpell(const CCharacter *pCh, CString strSpell, skill_type nValue);
    virtual void DoRescue(CCharacter *pTarget);
    virtual void DoKick(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoBash(const CInterp *pInterp, CCharacter *pTarget);
    virtual void BuildPracticePriceSpells(const CCharacter *pToBeTaught, CString &str) const;
    virtual void DoTog();
    virtual void DoReturn();

    virtual void SetSwitched(CCharacter *pCh) {
        return;
    }
    virtual void DoCharacterLook(CCharacter *ch);
    virtual void SendSpellList();
    virtual void DoForget();
    virtual void DoMediate();
    virtual short ManaCanRegain(bool bTotal = false);
    virtual short GetLearingBonus();
    virtual short GetManaAdjust();
    virtual void DoSpells();
    virtual void DoPrepare();
    virtual short GetMaxHitPointsPerLevel() = 0;
    virtual CProficiency &GetProficiency() = 0;
    virtual const CSkillInfo *GetSkillInfo(short nSkill) = 0;
    virtual short int CalcAC(CCharacter *pAttacker = NULL);
    virtual int GetExp(short nLvl) = 0;
    virtual void SendProficiency() = 0;
    virtual void DoCast(const CInterp *pInterp, bool bForceCast);
    virtual void DoChant();
    virtual short int CalcACWouldHit();
    virtual void DoBackStab(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoThroatSlit(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoTrip(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoSneak(const CInterp *pInterp);
    virtual void DoHide();
    virtual void DoCircle(const CInterp *pInterp);
    virtual void DoBeg();
    virtual void DoCharm();
    virtual void DoAttributes();
    virtual void DoPractice(CString strToPractice, const CCharacter *pTeacher);
    virtual void DoThink(const CInterp *pInterp);
    virtual void DoWill(const CInterp *pInterp);
    virtual int GetExpGap() = 0;
    virtual int GetExpInLevel() = 0;
    virtual void DoIdentify();
    virtual void DoAppraise();
    virtual void DoHoneWeapon();
    virtual void DoStrike(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoHeadButt(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoShieldRush(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoBerserk();
    virtual void DoWeaponFlurry(const CInterp *pInterp);
    virtual void DoDragonPunch(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoQuiveringPalm(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoBearHug(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoIntimidate(const CInterp *pInterp);
    virtual void DoHitAll(const CInterp *pInterp);
    virtual void DoDisarm(const CInterp *pInterp, CCharacter *pTarget);
    virtual void DoDirtToss(CCharacter *pTarget);
    virtual void DoLayHands();
    virtual void DoCalledShot(const CInterp *pInterp);

    virtual bool IsSwitchedTo() {
        return false;
    }

    virtual bool IsTeacher() const {
        return true;
    }
public:
    virtual void CleanUp();
    virtual void SendCharPage(CString str);
    virtual void ThinkerDefense(int nDam, CCharacter *pCaster);
    virtual void FadeAffects(int nTime);
    virtual void TakeDamage(CCharacter *pAttacker, int nDam, bool bFromMeele, bool bGiveExp = true);
    virtual void GiveMana(CCharacter *pAttacker, int nMana);

    virtual bool CheckVoid() {
        return (IsGod() ? ++m_lVoidTime >= GOD_VOID_TIME : ++m_lVoidTime >= CHARACTER_VOID_TIME);
    }

    virtual bool IsGod() const {
        return m_nLevel >= LVL_IMMORT;
    }
    virtual int AdvanceLevel(bool IncreaseLevel, bool bSendMsg = true, bool bIsFollower = false);

    virtual sMoney GetTrainingCosts(const CCharacter *pToBeTrained, short nSkill, bool bIsSkill) const {
        sMoney Mon;
        return Mon;
    } //pc's can figure out what they pay each other
    virtual skill_type CanTeach(const CCharacter *pCharToBeTaught, short nSkillToLearn, bool bIsSkill) const;
    virtual void Save(bool bDidJustDie = false);
    virtual void Defense(short int nACWouldHit, short nPos, CCharacter * pAttacker);
    virtual short GetThaco() = 0;

    virtual UINT GetDefaultPos() {
        assert(0);
        return 0;
    }

    virtual CString GetDefaultPosDesc() {
        assert(0);
        return CString();
    } //only for npcs
    virtual CString GetVitalStats();
    virtual void UpDatePosition(UINT nPosition, bool bSendStr = true);
    virtual long GetVnum();
    virtual bool HasCommand();
    virtual void SendToChar(CString Message, bool bColorize = true) const;
    virtual void SendToChar(const char *Message, bool bColorize = true) const;
    virtual void Attack(bool bOnlyOneAttack = false);
    virtual bool IsNPC() const;
    virtual ~CCharacter();
    virtual bool StatCheck(short int nStat, short nModifier = 0);
    virtual void LeaveFuguePlane();
    virtual void FugueTime();
    //defined in CCaster

    virtual void LoadSpells() {
        return;
    }
    //defined in CNPC

    virtual void AddMemory(CCharacter *pCh) {
        return;
    }

    virtual void DoAI(const CInterp *pInterp) {
        return;
    }

    virtual void RemoveFromMemory(CCharacter *pCh) {
        return;
    }
    virtual void Track();
    //static const
public:
    static const short AFFECT_STONE_SKIN;
    static const short AFFECT_CASTING;
    static const short AFFECT_SNEAKING;
    static const short AFFECT_HIDING;
    static const short AFFECT_COLD_SHIELD;
    static const short AFFECT_MEDITATE;
    static const short AFFECT_HEAT_SHIELD;
    static const short AFFECT_BLINDNESS;
    static const short AFFECT_FISTS_OF_STONE;
    static const short AFFECT_DETECT_MAGIC;
    static const short AFFECT_AUDIBLE_GLAMOUR;
    static const short AFFECT_BIOFEEDBACK;
    static const short AFFECT_INERTIAL_BARRIER;
    static const short AFFECT_TRANSPARENT_FORM;
    static const short AFFECT_AIR_WALK;
    static const short AFFECT_CLEAR_SIGHT;
    static const short AFFECT_CAMOUFLAGE;
    static const short AFFECT_AIR_SHIELD;
    static const short AFFECT_AIR_CUSHION;
    static const short AFFECT_INVISIBLE;
    static const short AFFECT_VULTURE_FLIGHT;
    static const short AFFECT_FLYING;
    static const short AFFECT_DETECT_INVISIBILITY;
    static const short AFFECT_DI;
    static const short AFFECT_INVISIBILITY;
    static const short AFFECT_MIST_FORM;
    static const short AFFECT_MIST_WALK;
    static const short AFFECT_MIST_SHIELD;
    static const short AFFECT_MIST_BARRIER;
    static const short AFFECT_ARMOR;
    static const short AFFECT_BARKSKIN;
    static const short AFFECT_DEXTERITY;
    static const short AFFECT_STRENGTH;
    static const short AFFECT_AGILITY;
    static const short AFFECT_BATTLE_READY;
    static const short AFFECT_PHYSICAL_ENHANCEMENT;
    static const short AFFECT_MAJOR_PARA;
    static const short AFFECT_EARTHENALIZE;
    static const short AFFECT_FAERIE_FLIGHT;
    static const short AFFECT_FIRE_SHIELD;
    static const short AFFECT_FORESTAL_ARMOR;
    static const short AFFECT_GUKS_GLOWING_GLOBES;
    static const short AFFECT_HASTE;
    //removed	static const short AFFECT_MIRROR_IMAGE;
    static const short AFFECT_CHANT;
    static const short AFFECT_CHANT_REGENERATION;
    static const short AFFECT_CHANT_DEFENSE;
    static const short AFFECT_CHANT_HONOR;
    static const short AFFECT_BLOODLUST;
    static const short AFFECT_BLESS;
    static const short AFFECT_CURSE;
    static const short AFFECT_FAERIE_FIRE;
    static const short AFFECT_FUGUE_PLANE;
    static const short AFFECT_BERSERK;
    static const short AFFECT_INTIMIDATE;
    static const short AFFECT_AGE_DEATH;
    static const short AFFECT_ARKANS_BATTLE_CRY;
    static const short AFFECT_SPIRIT_ARMOR;
    static const short AFFECT_DIVINE_ARMOR;
    static const short AFFECT_FLIGHT;
    static const short AFFECT_VAMPIRIC_TOUCH;
    static const short AFFECT_CANT_LAY_HANDS;
    static const short AFFECT_LIGHT; //used to store how much light is comming from player (via objects they where etc)
    static const short AFFECT_SET_UP_CAMP;
    static const short AFFECT_NO_BANDAGE_TIME;
    static const short AFFECT_INNATE_FLY;
    static const short AFFECT_INNATE_FLY_TIRED;
    static const short AFFECT_INNATE_STRENGTH_TIRED;
    static const short AFFECT_INNATE_BITE_TIRED;
    static const short AFFECT_INNATE_FAERIE_FIRE_TIRED;
    static const short AFFECT_INNATE_INVIS_TIRED;
    static const short AFFECT_INNATE_INVIS;
    static const short AFFECT_PSIONIC_LEECH;

    static const short LVLS_PER_PRACTICE_SESSION;
    static const short MIN_SKILL_ABILITY_TO_TEACH;
    static const short PC_TO_PC_TEACHER_PENATLY;
    static const short MIN_SPELL_ABILITY_TO_TEACH;
    static const int PULSES_PER_REGENERATION_TIME;
    static const int PULSES_PER_MOVEMENT_REGENERATION_TIME;
    static const int PULSES_PER_MANA_REGENERATION_TIME;
    static const int LEVELS_PER_DAMAGE_EXP_BONUS;
    static const int LEVELS_PER_KILL_EXP_BONUS;
    static const int LEVELS_PER_GROUP_KILL_EXP_BONUS;
    static const int LEVELS_PER_DAMAGE_EXP_PENATLY;
    static const int LEVELS_PER_KILL_EXP_PENATLY;
    static const int LEVELS_PER_GROUP_KILL_EXP_PENATLY;
    static const int CHECK_AGE_DEATH;
    static const long CHARACTER_VOID_TIME;
    static const long GOD_VOID_TIME;
    static const short CHANCE_TO_SEE;
    static const short SEES_RED_DOTS;
    static const short SEES_ALL;
    static const short SEES_NOTHING;

};

inline const char *CCharacter::GetRaceWarName() {
    if (this->IsNPC()) {
        return "None";
    } else if (m_pAttributes->IsRace(CRacialReference::RACE_ALIEN)) {
        return "Alien";
    } else if (this->IsEvil()) {
        return "Evil";
    } else if (this->IsGood()) {
        return "Good";
    } else {
        return "Neutral";
    }
}

inline short CCharacter::GetLoadPct(const CObject *pBeingDragged) const {
    int nWeight = pBeingDragged == NULL ? m_nWeightCarried : m_nWeightCarried + (pBeingDragged->GetWeight() / 2);
    return ((nWeight * 100) / m_pAttributes->GetMaxWeight());
}

inline void CCharacter::AddCommand(CString str) {
    if (m_pDescriptor != NULL) {
        m_pDescriptor->CommandBuf.Format("%s\r\n%s", str.cptr(), m_pDescriptor->CommandBuf.cptr());
    }
}

inline bool CCharacter::CanSee(CObject *pObj) const {
    if (Prefers(PREFERS_GOD_SEES_ALL) && IsGod()) {
        return true;
    } else {
        if (pObj->IsAffectedBy(CObject::OBJ_AFFECT_HIDDEN)) {
            return false;
        }
        if (pObj->IsAffectedBy(CObject::OBJ_AFFECT_INVISIBLE)
                && !IsAffectedBy(AFFECT_DETECT_INVISIBILITY)) {
            return false;
        }
        if (IsAffectedBy(AFFECT_BLINDNESS)) {
            return false;
        }
    }
    return true;
}

//Can I see his name?
//Can this see pCh->m_strName

inline bool CCharacter::CanSeeName(const CCharacter *pCh) const {
    bool bRetVal = false;
    if (pCh->IsGod()) {
        bRetVal = true;
    } else if (IsNPC() || pCh->IsNPC()) {
        bRetVal = true;
    } else if (this == pCh) {
        bRetVal = true;
    } else if (this->Prefers(PREFERS_GOD_SEES_ALL)) {
        bRetVal = true;
    }        //if both are aliens YES
    else if (m_pAttributes->IsRace(CRacialReference::RACE_ALIEN)
            && pCh->m_pAttributes->IsRace(CRacialReference::RACE_ALIEN)) {
        bRetVal = true;
    }        //if one is alien and one isn't then no
    else if (((!m_pAttributes->IsRace(CRacialReference::RACE_ALIEN)
            && pCh->m_pAttributes->IsRace(CRacialReference::RACE_ALIEN)))
            || (m_pAttributes->IsRace(CRacialReference::RACE_ALIEN)
            && !pCh->m_pAttributes->IsRace(CRacialReference::RACE_ALIEN)))
 {
        bRetVal = false;
    }        //alignment based stuff
    else if ((IsEvil() && pCh->IsEvil()) || (IsNeutral() && pCh->IsNeutral()) || (IsGood() && pCh->IsGood()) || pCh->IsNPC() || IsNPC()) {
        bRetVal = true;
    } else {
        bRetVal = false;
    }
    return bRetVal;
}

inline sPlayerDescriptor *CCharacter::KillSocket() {
    if (IsNPC()) {
        ErrorLog << "killing socket on a mob!\n" << endl;
    }
    sPlayerDescriptor *tmp = ((sPlayerDescriptor *) m_pDescriptor);
    m_pDescriptor = NULL;
    return tmp;
}

inline bool CCharacter::IsPlaying() {
    //if we have a descriptor and we just reconnected (and we just killed intermediate char
    //change state and return true
    if (m_pDescriptor && m_pDescriptor->GetState() == STATE_KILL_INTERMEDIATE) {
        m_pDescriptor->ChangeState(STATE_PLAYING);
        return true;
    }        // if we have a descriptor and we are playing return true
    else if (m_pDescriptor && m_pDescriptor->IsPlaying()) {
        return true;
    }
    //otherwise false!
    return false;
}

inline void CCharacter::AssertFighting(CCharacter *pCh) {
    if (pCh == this)
        return;
    m_pFighting = pCh;
    if (pCh) {
        UpDatePosition(POS_FIGHTING);
    } else {
        m_nPosition &= ~POS_FIGHTING;
    }
}

/////////////////////////////////
//	Remove Stun
//	John Comes 1/28/99
/////////////////////////////////

inline void CCharacter::RemovePosition(UINT nRemove) {
    if ((!(nRemove & POS_STUNNED))
            && (!(nRemove & POS_SLEEPING))
            && (!(nRemove & POS_KOED))) {
        //only use this function to remove positions
        //POS_STUNNED and POS_KOED use updateposition for the rest
        assert(0);
    }
    m_nPosition &= ~nRemove;
    UpDatePosition();
    if (nRemove & POS_STUNNED) {
        SendToChar((const char *)"The world stops spinning.\r\n");
    }
}

/////////////////////////////////////
//	Force add used for resurrection etc
//	we still want to add up the wieght we just don't want
//	to excude anyting from being added to the character

inline bool CCharacter::Add(CObject *pObj, bool bAddEnd, bool bForceAdd) {
    if (m_pAttributes->GetMaxWeight() >= (m_nWeightCarried + pObj->GetWeight())
            || bForceAdd) {
        Inventory.Add(pObj, bAddEnd);
        m_nWeightCarried += pObj->GetWeight();
        pObj->Set(this, NULL);
        return true;
    }
    return false;
}

inline bool CCharacter::Add(const sMoney Mon, bool bForceAdd) {
    if (m_pAttributes->GetMaxWeight() >= (m_nWeightCarried + Mon.GetWeight())
            || bForceAdd) {
        m_CashOnHand += Mon;
        return true;
    }
    return false;
}

inline void CCharacter::Remove(CObject *pObj) {
    Inventory.Remove(pObj);
    m_nWeightCarried -= pObj->GetWeight();
}

inline void CCharacter::Remove(CObject *pObj, POSITION &pos) {
    Inventory.Remove(pObj, pos);
    m_nWeightCarried -= pObj->GetWeight();
}

////////////////////////////////////////////
//	class CEqInfo
//put after CCharacter class so we can declare
//friends

class CEqInfo {
    friend void CCharacter::WearEquipment(CObject * pObj, UINT nWearBit, int nPos, bool bSendMsg);
    friend CObject *CCharacter::RemoveEquipment(int nPos, bool bSendMsg);
private:
    CObject *m_pObj;
    UINT m_nWearBit;
protected:

    CEqInfo(CObject *pObj, UINT nWearBit) {
        m_pObj = pObj;
        m_nWearBit = nWearBit;
    }

    CEqInfo(const CEqInfo &copy) {
        m_pObj = copy.m_pObj;
        m_nWearBit = copy.m_nWearBit;
    }

    void Empty() {
        m_pObj = NULL;
        m_nWearBit = 0;
    }
public:

    CEqInfo() {
        m_pObj = NULL;
        m_nWearBit = 0;
    }

    UINT GetWearBit() {
        return m_nWearBit;
    }

    bool IsWorn(UINT nPos) {
        return (nPos & m_nWearBit) ? true : false;
    }

    CObject *operator->() {
        return m_pObj;
    }

    bool IsEmpty() {
        return m_pObj == NULL;
    }

    CObject * GetObject() {
        return m_pObj;
    }
};
#endif
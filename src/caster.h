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

#ifndef _CASTER_H_
#define _CASTER_H_

#include "spell.h"
#include "spell.save.h"
#include "random.h"
#include <cstdio>
extern CRandom GenNum;

template<class CasterClass>
class CCaster: virtual public CCharacter {
protected:
	bool m_bHasSpelledUp; //so we can check every casting round for a skill up but not get multible skill ups when casting
	CCurrentSpell<CasterClass> m_CurrentSpell;
	CLinkList<CPreparing<CasterClass> > m_SpellsBeingPrepared;
	CLinkList<CPrepared<CasterClass> > m_SpellsPrepared;
protected:

	CCaster(CCharIntermediate *ch) :
			CCharacter(ch) {
		m_SpellsBeingPrepared.SetDeleteData();
		m_SpellsPrepared.SetDeleteData();
		m_CurrentSpell.Empty();
		m_bHasSpelledUp = false;
	}

	CCaster(CMobPrototype &mob, CRoom *pPutInRoom) :
			CCharacter(mob, pPutInRoom) {
		m_SpellsBeingPrepared.SetDeleteData();
		m_SpellsPrepared.SetDeleteData();
		m_CurrentSpell.Empty();
		m_bHasSpelledUp = false;
	}
protected:
	virtual void BuildPracticePriceSpells(const CCharacter *pToBeTaught, CString &str) const;
	virtual const CSpell<CasterClass> *GetSpellInfo(CString strSpell) = 0;
	virtual int CalcSpellDam(const CSpell<CasterClass> * pSpell) = 0;
	virtual const CMap<CString, CSpell<CasterClass> *> *GetSpellTable() const = 0;
	virtual short GetPctSpellUp(const CSpell<CasterClass> *pSpell) = 0;
protected:
	virtual bool SetSpell(const CCharacter *pCh, CString strSpell, skill_type nValue);
	virtual void DoPractice(CString strToPractice, const CCharacter *pTeacher);
	virtual void DoForget();
	virtual void DoPrepare();
	virtual void DoCast(const CInterp *pInterp, bool bForceCast);
	virtual short ManaCanRegain(bool bTotal = false);
	virtual bool LostConcentration();
	virtual void DoSpells();
	virtual ~CCaster();
protected:
	void OffensiveSpell(const CInterp *pInterp, const CSpell<CasterClass> *pSpell);
	void ReducePrepared(const CSpell<CasterClass> * pSpell);
	void RemovePreparing(const CSpell<CasterClass> * pSpell);
	bool CastingTargetInRoom();
	bool IsSpellBeingPrepared(const CSpell<CasterClass> * pSpell);
	bool IsSpellPrepared(const CSpell<CasterClass> * pSpell);
	void PrepareSpell(CString strSpell);
	void AddToPrepList(const CSpell<CasterClass> * pSpell);
	void RePrep(bool bSuppressNothingToPrep = false);
	void GetSpellsPreparedAndPreparing(CString & strSpells);
	void SpellUp(const CSpell<CasterClass> *pSpell);
public:
	virtual void UpDatePosition(UINT nPosition, bool bSendStr = true);
	virtual bool HasCommand();
	virtual void Save(bool bDidJustDie = false);
	virtual void LoadSpells();
};

//////////////////////////////////
//	Spells
//	Send character the spells they know
//	as well as how good they are at that spell
/////////////////////////////////

template<class CasterClass>
void CCaster<CasterClass>::DoSpells() {
	CSpell<CasterClass> *pSpell;
	POSITION pos = GetSpellTable()->GetStartingPosition();
	CString strToChar;

	strToChar += "Spells Known:\r\n";
	while (pos) {
		pSpell = GetSpellTable()->GetNext(pos);
		if (m_Spells[pSpell->GetSpellID()]) {
			strToChar.Format("%s%-30s\t%s\r\n", strToChar.cptr(), pSpell->GetColorizedName().cptr(),
					this->SkillWording(m_Spells[pSpell->GetSpellID()]));
		}
	}
	SendToChar(strToChar);
}

///////////////////////////////
//	Over ridden for casters
//	Allows a god to the effecticy of a spell
///////////////////////////////

template<class CasterClass>
bool CCaster<CasterClass>::SetSpell(const CCharacter *pCh, CString strSpell, skill_type nValue) {
	const CSpell<CasterClass> *pSpell;
	if ((pSpell = this->GetSpellInfo(strSpell)) != NULL) {
		m_Spells[pSpell->GetSpellID()] = nValue > MAX_SKILL_LEVEL ? MAX_SKILL_LEVEL : nValue;
		CString strToChar;
		strToChar.Format("%s makes you feel more comfortable casting %s.\r\n", pCh->GetName().cptr(),
				pSpell->GetColorizedName().cptr());
		SendToChar(strToChar);
		strToChar.Format("You increase %s's skill in casting %s to %d.\r\n", GetName().cptr(),
				pSpell->GetColorizedName().cptr(), nValue);
		m_nSpellSphere |= pSpell->GetSphere();
		pCh->SendToChar(strToChar);
		return true;
	}
	return false;
}

///////////////////////////////
//	Like Skill up but for spells

template<class CasterClass>
void CCaster<CasterClass>::SpellUp(const CSpell<CasterClass> *pSpell) {
	if ((!m_bHasSpelledUp) && m_Spells[pSpell->GetSpellID()] <= MAX_SKILL_LEVEL
			&& (this->GetPctSpellUp(pSpell) >= DIE(101)) && IsAffectedBy(CCharacter::AFFECT_CASTING)) {
		m_bHasSpelledUp = true;
		m_Spells[pSpell->GetSpellID()] += DIE(this->GetLearingBonus());
		CString strToChar;
		strToChar.Format("&cYou feel more comfortable casting %s&n\r\n", pSpell->GetColorizedName().cptr());
		SendToChar(strToChar);
	}
}
//This can not be called from constructor
//we need the use of the pure virtual function,
//GetSpellInfo

template<class CasterClass>
void CCaster<CasterClass>::LoadSpells() {

	CString strFile(SPELL_SAVE_PREFIX);
	strFile += m_strName;
	FILE *SpellFile = fopen(strFile.cptr(), "rb");
	if (!SpellFile) {
		MudLog << "spell file " << strFile.cptr() << "could not be opened" << std::endl;
		return;
	} else if (ferror(SpellFile) != 0) {
		MudLog << "error opening spell file" << strFile.cptr() << std::endl;
		return;
	}
	sSpellSave<CasterClass> SavedSpells;
	const CSpell<CasterClass> *pSpell;
	short i = 0;
	for (i = 0; i < SavedSpells.GetMaxSpellsPrepared() && !SavedSpells.GetPreparedSpell(i).IsEmpty(); i++) {
		if ((pSpell = this->GetSpellInfo(CString(SavedSpells.GetPreparedSpell(i).m_strSpellName)))) {
			m_SpellsPrepared.Add(new CPrepared<CasterClass>(pSpell, SavedSpells.GetPreparedSpell(i).m_nNumber));
		} else {
			CString strError;
			strError.Format("Spell %s has been removed from the game.\r\n",
					SavedSpells.GetPreparedSpell(i).m_strSpellName);
			this->SendToChar(strError);
		}
	}
	for (i = 0; i < SavedSpells.GetMaxSpellsPreparing() && !SavedSpells.GetPreparingSpell(i).IsEmpty(); i++) {
		if ((pSpell = this->GetSpellInfo(CString(SavedSpells.GetPreparingSpell(i).GetName())))) {
			m_SpellsBeingPrepared.Add(
					new CPreparing<CasterClass>(pSpell, SavedSpells.GetPreparingSpell(i).GetTimeLeft(),
							SavedSpells.GetPreparingSpell(i).GetOriginalTime()));
		} else {
			CString strError;
			strError.Format("Spell %s has been removed from the game.\r\n", SavedSpells.GetPreparingSpell(i).GetName());
			this->SendToChar(strError);
		}
	}
	fclose(SpellFile);
}

//Save
//	Save characters spells preparing and prepared
//	then do regular character save
////////////////////

template<class CasterClass>
void CCaster<CasterClass>::Save(bool bDidJustDie) {
	if (!this->IsNPC()) {
		CString strFile(SPELL_SAVE_PREFIX);
		strFile += m_strName;
		FILE *SpellFile = fopen(strFile.cptr(), "wb+");
		if (!SpellFile || (ferror(SpellFile) != 0)) {
			ErrorLog << "Spell file for " << m_strName.cptr() << " won't open on save!" << endl;
		} else {
			if (!bDidJustDie) {
				sSpellSave<CasterClass> SpellsToSave;
				SpellsToSave.AddPrepared(&m_SpellsPrepared);
				SpellsToSave.AddPreparing(&m_SpellsBeingPrepared);
				fwrite((char *) &SpellsToSave, sizeof(sSpellSave<CasterClass> ), 1, SpellFile);
			}
		}
		fflush(SpellFile);
		fclose(SpellFile);
	}
	//do character save
	CCharacter::Save(bDidJustDie);
}

//destructor

template<class CasterClass>
CCaster<CasterClass>::~CCaster() {

}

///////////////////////////
//	LostConcentration()
//	Checks to see if we lose concentration
//	on the spell we are casting.

template<class CasterClass>
bool CCaster<CasterClass>::LostConcentration() {
	if (m_nLevel < (m_CurrentSpell.m_pSpell->GetMinLevel() + 5)) {
		//Do int check and see if concen was lost.
		return !StatCheck(INT);
	}
	return false;
}

///////////////////////////////
//	Overridden HasCommand from CCharacter class
//////////////////////////////

template<class CasterClass>
bool CCaster<CasterClass>::HasCommand() {
	CString strToChar;
	//if they are preping and they are preparing spells
	if ((InPosition(POS_PREP)) && !m_SpellsBeingPrepared.IsEmpty()) {
		CPreparing<CasterClass> *pSpellPreparing = m_SpellsBeingPrepared.GetFirst();
		//if lag time is over
		//if we have a spell to prep and we have the spell is
		//done preping OR we are meditating
		//and we have 1/2 done preping and we make
		//the meditation check
		if (pSpellPreparing
				&& ((--(*pSpellPreparing) <= 0)
						|| (IsAffectedBy(CCharacter::AFFECT_MEDITATE) && pSpellPreparing->TimeToCheckMed()
								&& (DIE(100) <= GetSkill(SKILL_MEDITATE))))) {
			POSITION pos = m_SpellsPrepared.GetStartPosition();
			CPrepared<CasterClass> *pPrepared;
			if (pos) {
				bool bFound = false;
				while (pos) {
					pPrepared = m_SpellsPrepared.GetNext(pos);
					//spell is found so just increase the number preped
					if (pPrepared->GetSpell() == pSpellPreparing->GetSpell()) {
						bFound = true;
						++(*pPrepared);
						pos = NULL;
					}
				}
				//if we didnt find the spell in the linklist
				if (!bFound) {
					m_SpellsPrepared.Add((pPrepared = new CPrepared<CasterClass>(pSpellPreparing)));
				}
			} else {
				m_SpellsPrepared.Add((pPrepared = new CPrepared<CasterClass>(pSpellPreparing)));
			}
			strToChar.Format("You finish preparing %s.\r\n", pSpellPreparing->GetSpell()->GetSpellName().cptr());
			SendToChar(strToChar);

			m_SpellsBeingPrepared.Remove(pSpellPreparing);
			//If we don't need to prep any more spells they are done
			if (m_SpellsBeingPrepared.IsEmpty()) {
				SendToChar("Your preparations are complete.\r\n");
				m_nPosition &= ~POS_PREP;
			}
			SendPrompt();
		} else if (IsAffectedBy(CCharacter::AFFECT_MEDITATE) && pSpellPreparing->TimeToCheckMed()) {
			SkillUp(SKILL_MEDITATE);
		}
	}        //can't be casting and preparing at the same time!
	else if (IsAffectedBy(CCharacter::AFFECT_CASTING)) {
		if (!this->CastingTargetInRoom()) {
			//don't return here so we get the next command
			RemoveAffect(CCharacter::AFFECT_CASTING);
			SendToChar("You target has left the room!\r\n");
			GetRoom()->SendToRoom("&R%s stops chanting abruptly!&n\r\n", this);
			m_lLagTime = 0;
		} else if (!InPosition(POS_STANDING)) {
			SendToChar("You can't do this while not standing!\r\n");
			GetRoom()->SendToRoom("&R%s stops chanting abruptly!&n\r\n", this);
			RemoveAffect(CCharacter::AFFECT_CASTING);
		} else {
			if ((m_lLagTime - 1) && !((m_lLagTime - 1) % (CMudTime::PULSES_PER_CASTING_ROUND))) {
				if (this->LostConcentration()) {
					RemoveAffect(CCharacter::AFFECT_CASTING);
					SendToChar("You lose your concentration.\r\n");
					m_pInRoom->SendToRoom("%s stops chanting.\r\n", this);
					m_lLagTime = 0;
				} else {
					CString strCastMsg;
					short nDie = DIE(100);
					if ((nDie <= GetSkill(SKILL_QUICK_CHANT) && !IsFighting())
							|| (nDie < (GetSkill(SKILL_QUICK_CHANT) >> 2) && IsFighting())) {
						m_lLagTime >>= 1;
					} else {
						SkillUp(SKILL_QUICK_CHANT);
					}
					short nIndex = (short) (m_lLagTime / CMudTime::PULSES_PER_CASTING_ROUND);
					strCastMsg.Format(CastingMsg[nIndex], m_CurrentSpell.m_pSpell->GetSpellName().cptr());
					SendToChar(strCastMsg);
					SpellUp(m_CurrentSpell.m_pSpell);
				}
				SendPrompt();
			}
		}
	}
	//run base has command
	return CCharacter::HasCommand();
}

//////////////////////////
//	Overridden to calculate wether 
//	a mage can regain mana points
//	written by : demetrius Comes
//////////////////////////

template<class CasterClass>
short CCaster<CasterClass>::ManaCanRegain(bool bTotal) {
	short nMana = m_nManaPts;
	if (m_nManaPts >= m_nMaxManaPts) {
		return 0;
	} else {
		POSITION pos = m_SpellsPrepared.GetStartPosition();
		CPrepared<CasterClass> *pPrep;
		while (pos) {
			pPrep = m_SpellsPrepared.GetNext(pos);
			nMana += (pPrep->GetSpell()->GetMana() * (short) (*pPrep));
		}
		pos = m_SpellsBeingPrepared.GetStartPosition();
		CPreparing<CasterClass> *pPreping;
		while (pos) {
			pPreping = m_SpellsBeingPrepared.GetNext(pos);
			nMana += pPreping->GetSpell()->GetMana();
		}
		if (nMana >= m_nMaxManaPts) {
			return 0;
		}        //used for restore
		else if (bTotal) {
			return (m_nMaxManaPts - nMana < 0 ? 0 : m_nMaxManaPts - nMana);
		} else {
			short nManaRegain = m_pAttributes->GetManaAdjustInt();
			//if not mediating or fail skill
			//lose 1/2 mana
			if (!IsAffectedBy(CCharacter::AFFECT_MEDITATE)) {
				nManaRegain /= 2;
				nManaRegain = nManaRegain > 0 ? nManaRegain : 1;
			} else if (IsAffectedBy(CCharacter::AFFECT_MEDITATE) && DIE(100) > GetSkill(SKILL_MEDITATE)) {
				nManaRegain /= 2;
				SkillUp(SKILL_MEDITATE);
			}
			//nMana is mana owned thus far
			//+ the manaregain can not be granter than max
			if ((nMana + nManaRegain) > m_nMaxManaPts) {
				return m_nMaxManaPts - nMana;
			} else {
				return nManaRegain;
			}
		}
	}
}

/////////////////////////////////////////
//	DoCast
//	Find tick ' marks
//	Get spell word and see if in hash table
//	if not return 
//	if it is..dereference function pointer in
//	CSpell class and run function assocated with
//	this spell
//	written by: Demetrius Comes
////////////////////////////////////

template<class CasterClass>
void CCaster<CasterClass>::DoCast(const CInterp *pInterp, bool bForceCast) {
	CString strToRoom = "%s ";
	CString strTemp;
	static const char *pTick = "'";
	//if they aren't casting check all information and insure
	//they can cast the spell
	if (!IsAffectedBy(CCharacter::AFFECT_CASTING)) {
		int first = CurrentCommand.Find(pTick[0]);
		int second = CurrentCommand.Find(pTick[0], 2);

		if (first == -1 || second == -1) {
			SendToChar("Spells must be inclosed in single quotes!\r\n");
			return;
		}

		CString strSpell(CurrentCommand(first + 1, second - 1));

		const CSpell<CasterClass> *pSpell;

		if (!(pSpell = this->GetSpellInfo(strSpell))) {
			SendToChar("Cast what?\r\n");
			return;
		}
		if (!m_Spells[pSpell->GetSpellID()] || pSpell->GetMinLevel() > this->GetLevel()) {
			SendToChar("You don't know how to cast that spell!\r\n");
			return;
		}
		if (!IsSpellPrepared(pSpell) && !bForceCast) {
			SendToChar("You don't have that spell prepared.\r\n");
			return;
		} else if (bForceCast) {
			//take do they have enough to cast the spell?
			if (m_nManaPts - (pSpell->GetMana(false)) < 0) {
				SendToChar("You need more mana!\r\n");
				return;
			}
		}
		//get everything after the ticks and save it to
		//be added to the currentSpell structure
		CString strAfterTicks(CurrentCommand.Right(second + 1));
		//get everything after the ticks and see if we need
		//to pluck off the a '.'
		CString strTarget(CurrentCommand.Right(second + 1));
		strAfterTicks.SkipSpaces();
		strTarget.SkipSpaces();
		CCharacter *pTarget = NULL;

		if ((pSpell->GetAffectedArea() & AREA_TARGET)) {
			CString strTmp(CurrentCommand);
			CurrentCommand = strTarget;
			pTarget = GetTarget(0, true);
			CurrentCommand = strTmp;
			if (!pTarget) {
				SendToChar("At Who?\r\n");
				return;
			}
		}
		if ((pSpell->GetAffectedArea() & AREA_SELF)) {
			pTarget = this;
		}
		//know we know we can cast it so set casting bit
		//and set up all information in current spell struct
		AddAffect(CCharacter::AFFECT_CASTING, sAffect::MANUAL_AFFECT);
		if (DIE(100) <= GetSkill(SKILL_QUICK_CHANT)) {
			m_lLagTime = pSpell->GetLagTime() >> 1;
		} else {
			m_lLagTime = pSpell->GetLagTime();
		}
		m_CurrentSpell.m_pSpell = pSpell;
		m_CurrentSpell.m_strTarget = strAfterTicks;
		m_CurrentSpell.m_pTarget = pTarget;
		if (m_CurrentSpell.m_pSpell->IsOffensive()) {
			if (IsAffectedBy(CCharacter::AFFECT_INVISIBLE)) {
				RemoveAffect(CCharacter::AFFECT_INVISIBLE);
			}
			if ((m_CurrentSpell.m_pSpell->GetAffectedArea() & AREA_TARGET) && m_CurrentSpell.m_pTarget != NULL) {
				if (this->m_Master.IsMasterOf(m_CurrentSpell.m_pTarget)) {
					this->m_Master.Remove(m_CurrentSpell.m_pTarget);
				}
				if (this->m_Follow.HasFollower(m_CurrentSpell.m_pTarget)) {
					this->m_Follow.Remove(m_CurrentSpell.m_pTarget);
				}
			}
		}
		SendToChar(CString("&WYou start chanting...&n\r\n"));
		GetRoom()->SendToRoom(CString("%s &Wstarts casting a spell...&n \r\n"), this);
		m_bHasSpelledUp = false; //set this so we can check for spell ups
	} //they already were casting so we know casting time is up
	  //other wise we wouldn't be in here.
	else {
		if (this->CastingTargetInRoom()) {
			SendToChar(CString("&WYou complete your spell...&n\r\n"));
			GetRoom()->SendToRoom(CString("%s &Wcompletes the spell...&n \r\n"), this);
			strTemp.Format("utters the words '%s'\r\n", m_CurrentSpell.m_pSpell->GetSpellName().cptr());
			strToRoom += strTemp;
			GetRoom()->SendToRoom(strToRoom, this);
			//remove casting Affect
			RemoveAffect(CCharacter::AFFECT_CASTING);

			//cast spell
			m_CurrentSpell.m_pSpell->Cast(pInterp, (CasterClass *) this);
			//if they don't have it prepared take the extra mana away
			if (!IsSpellPrepared(m_CurrentSpell.m_pSpell)) {
				m_nManaPts -= m_CurrentSpell.m_pSpell->GetMana(false);
			} else {
				//add to prep list
				AddToPrepList(m_CurrentSpell.m_pSpell);
				ReducePrepared(m_CurrentSpell.m_pSpell);
			}
		} else {
			SendToChar("At who?\r\n");
		}
		//empty spell struct
		m_CurrentSpell.Empty();
	}
}

/////////////////////////////
//	Updateposition
//	if the caster is preparing and they stand up
//	give em back there ManaPts.
///////////////////////////

template<class CasterClass>
void CCaster<CasterClass>::UpDatePosition(UINT nPosition, bool bSendStr) {
	//if we are preparing and we are trying to switch to any
	//position besides resting or preping and we have spells to prep
	//	give em mana back
	if ((InPosition(POS_PREP)) && (!(nPosition & POS_RESTING)) && (!(nPosition & POS_PREP))
			&& !m_SpellsBeingPrepared.IsEmpty()) {
		CPreparing<CasterClass> *pPreping;
		POSITION pos = m_SpellsBeingPrepared.GetStartPosition();
		while (pos) {
			pPreping = m_SpellsBeingPrepared.GetNext(pos);
			m_nManaPts += pPreping->GetSpell()->GetMana();
			if (m_nManaPts > m_nMaxManaPts) {
				m_nManaPts = m_nMaxManaPts;
				CString str;
				str.Format("%s has gone above max mana!\r\n", m_strName.cptr());
				ErrorLog << str << endl;
			}
			pPreping->ResetTime();
		}
	}
	CCharacter::UpDatePosition(nPosition, bSendStr);
}

/////////////////////////////////////
//	GetSpellsPreparedAndPreparing
//	Builds a string of the spells prepared
//	and the one's being prepared.
//	written by: Demetrius Comes
/////////////////////////////////

template<class CasterClass>
void CCaster<CasterClass>::GetSpellsPreparedAndPreparing(CString & strSpells) {
	//start list of preped spells
	CString strToChar;
	POSITION pos = m_SpellsPrepared.GetStartPosition();
	CPrepared<CasterClass> *pPreped = NULL;
	strToChar = "Prepared spells are:\r\n";
	if (!pos) {
		strToChar += "None!\r\n";
	}
	while (pos) {
		pPreped = m_SpellsPrepared.GetNext(pos);
		if ((short) (*pPreped) > 1) {
			strToChar.Format("%s(%d) %s\r\n", strToChar.cptr(), (short) (*pPreped),
					pPreped->GetSpell()->GetColorizedName().cptr());
		} else {
			strToChar.Format("%s%s\r\n", strToChar.cptr(), pPreped->GetSpell()->GetColorizedName().cptr());
		}
	}
	strToChar += "\r\nCurrently Preparing: \r\n";
	//start list of preparing spells
	pos = m_SpellsBeingPrepared.GetStartPosition();
	CPreparing<CasterClass> *pPreparing = NULL;
	if (!pos) {
		strToChar += "None!\r\n";
	}
	int nTotalTime = 0;
	while (pos) {
		pPreparing = m_SpellsBeingPrepared.GetNext(pos);
		nTotalTime += pPreparing->GetSecondsLeft();
		strToChar.Format("%s%s (%d) (%d)\r\n", strToChar.cptr(), pPreparing->GetSpell()->GetColorizedName().cptr(),
				pPreparing->GetSecondsLeft(), nTotalTime);
	}
	strSpells += strToChar;
}

////////////////////////////////////
//	RePrep
//	this function is called from the 
//	DoPrepare() here we are just checking to 
//	see if they character has enough mana to reprepare
//	the spells they have in their prep list.
//	written by: Demetrius Comes
/////////////////////////////////////

template<class CasterClass>
void CCaster<CasterClass>::RePrep(bool bSupressNothingToPrep) {
	CString strToChar;
	POSITION pos = m_SpellsBeingPrepared.GetStartPosition();
	;
	CPreparing<CasterClass> *pPreparing;
	while (pos) {
		pPreparing = m_SpellsBeingPrepared.GetNext(pos);
		//do they have the mana to prep it?
		if ((m_nManaPts - pPreparing->GetSpell()->GetMana()) >= 0) {
			m_nManaPts -= pPreparing->GetSpell()->GetMana();
		} else {
			//they don't have enough mana so inform them
			strToChar.Format("%sYou don't have enough mana to prepare %s.\r\n", strToChar.cptr(),
					pPreparing->GetSpell()->GetSpellName().cptr());
			m_SpellsBeingPrepared.Remove(pPreparing, pos);
		}
	}
	//send the one's they can't prep to them
	if (m_SpellsBeingPrepared.IsEmpty() && !bSupressNothingToPrep) {
		GetSpellsPreparedAndPreparing(strToChar);
		strToChar += "\r\nYou have no more spells to prepare.\r\n";
	} else {
		GetSpellsPreparedAndPreparing(strToChar);
		strToChar += "You start preparing your spells.\r\n";
		UpDatePosition(POS_PREP);
	}
	SendToChar(strToChar);
}

////////////////////
//	AddToPrepList
//	So that we can change preping times 
//	in one spot 
//	written by: Demetrius Comes

template<class CasterClass>
void CCaster<CasterClass>::AddToPrepList(const CSpell<CasterClass> * pSpell) {
	short nPrepTime = (short) pSpell->GetPrepTime() + m_pAttributes->GetIntPrepTimeAdjust()
			- ((GetLevel() - pSpell->GetMinLevel()) / 2);
	nPrepTime *= CMudTime::PULSES_PER_PREP_ROUND;
	if (nPrepTime < 1) {
		nPrepTime = 1;
	}
	m_SpellsBeingPrepared.Add(new CPreparing<CasterClass>(pSpell, nPrepTime));
	if (InPosition(POS_PREP)) {
		m_nManaPts -= pSpell->GetMana();
	}
}

//////////////////////////
//	PrepareSpells
//	handles if the player says Prep <spell name>
//	

template<class CasterClass>
void CCaster<CasterClass>::PrepareSpell(CString strSpell) {
	CString strToChar;
	CPreparing<CasterClass> *pPrep = NULL;
	const CSpell<CasterClass> *pSpell;
	if (!(pSpell = this->GetSpellInfo(strSpell))) {
		strToChar = "Prep what?\r\n";
	}    //do they have enough mana?
	else if (m_nManaPts - pSpell->GetMana() < 0) {
		strToChar = "You don't have the mana!\r\n";
	} else if (!m_Spells[pSpell->GetSpellID()]) {
		strToChar = "You don't know how to prepared that spell!\r\n";
	} else {
		strToChar.Format("The preparation of %s will cost you %d mana points.\r\n", pSpell->GetColorizedName().cptr(),
				pSpell->GetMana());
		//reduce mana
		UpDatePosition(POS_PREP);
		AddToPrepList(pSpell);
	}
	SendToChar(strToChar);
}

//////////////////////////////////
//	Prepare
//	overriden for mage class
//	Check to see if Spell is prepable
//	if so add it to the LinkList
//	written by: Demetrius Comes
///////////////////////////////

template<class CasterClass>
void CCaster<CasterClass>::DoPrepare() {
	CString strSpell(CurrentCommand.GetWordAfter(1, true));
	CString strToChar;
	//if there is a spell they are trying to prepare
	//and they are resting and preping already just prepare the spell
	if (!strSpell.IsEmpty() && InPosition(POS_RESTING) && InPosition(POS_PREP)) {
		PrepareSpell(strSpell);
	}    //but if they are trying to prepare a spell and are currrently resting
		 //but NOT preparing reprep spells first then prepare the new one
	else if (!strSpell.IsEmpty() && InPosition(POS_RESTING) && !InPosition(POS_PREP)) {
		RePrep(true);
		PrepareSpell(strSpell);
	}        //they are not trying to prepare any new spell just reprep the
			 //ones they have
	else if (strSpell.IsEmpty() && InPosition(POS_RESTING) && !InPosition(POS_FIGHTING) && !InPosition(POS_PREP)) {
		RePrep();
	}        //the are currently preparing but are just checking the time they
			 //have left
	else if (strSpell.IsEmpty() && InPosition(POS_RESTING) && InPosition(POS_PREP)) {
		GetSpellsPreparedAndPreparing(strToChar);
		strToChar += "You continue preparing your spells.\r\n";
		SendToChar(strToChar);
	}        //they are not trying to prepare just seeing what they have prepared
	else {
		GetSpellsPreparedAndPreparing(strToChar);
		SendToChar(strToChar);
	}
}

///////////////////////
//	Take our Target and see if it's 
//	still in the room

template<class CasterClass>
bool CCaster<CasterClass>::CastingTargetInRoom() {
	if (m_CurrentSpell.m_pTarget) {
		return (m_CurrentSpell.m_pTarget->IsDead() ? false : this->IsInRoom(m_CurrentSpell.m_pTarget));
	} else if (m_CurrentSpell.m_pSpell->GetAffectedArea() & AREA_ROOM
			|| m_CurrentSpell.m_pSpell->GetAffectedArea() == 0) {
		return true;
	}
	ErrorLog << "casting bit set when it shouldn't be!" << endl;
	return false;
}

///////////////////////////
//  Removes spell being prepared.
//

template<class CasterClass>
void CCaster<CasterClass>::RemovePreparing(const CSpell<CasterClass> * pSpell) {
	POSITION pos = m_SpellsBeingPrepared.GetStartPosition();
	CPreparing<CasterClass> *pPreping;
	while (pos) {
		pPreping = m_SpellsBeingPrepared.GetNext(pos);
		if (pPreping->GetSpell() == pSpell) {
			m_SpellsBeingPrepared.Remove(pPreping, pos);
			if (InPosition(POS_PREP)) {
				m_nManaPts += pSpell->GetMana();
				m_nManaPts = m_nManaPts > m_nMaxManaPts ? m_nMaxManaPts : m_nManaPts;
				if (m_SpellsBeingPrepared.IsEmpty()) {
					SendToChar("You stop preparing.\r\n");
					m_nPosition &= ~POS_PREP;
				}
			}
			return;
		}
	}
}

///////////////////////////
//	Reduces the number of preped
//	spell
/////////////////////////////

template<class CasterClass>
void CCaster<CasterClass>::ReducePrepared(const CSpell<CasterClass> * pSpell) {
	POSITION pos = m_SpellsPrepared.GetStartPosition();
	CPrepared<CasterClass> *pPrep;
	while (pos) {
		pPrep = m_SpellsPrepared.GetNext(pos);
		if (pSpell == pPrep->GetSpell()) {
			if ((short) (*pPrep) <= 1) {
				m_SpellsPrepared.Remove(pPrep, pos);
			} else {
				pPrep->Reduce();
			}
			m_nManaPts += pSpell->GetMana();
			if (m_nManaPts > m_nMaxManaPts) {
				m_nManaPts = m_nMaxManaPts;
				CString str;
				str.Format("%s has gone above max mana!\r\n", m_strName.cptr());
				ErrorLog << str << endl;
			}
			return;
		}
	}
}

///////////////////////
//	IsSpellPrepared
//	check prepared LL for spell
///////////////////

template<class CasterClass>
bool CCaster<CasterClass>::IsSpellPrepared(const CSpell<CasterClass> * pSpell) {
	POSITION pos = m_SpellsPrepared.GetStartPosition();
	while (pos) {
		if (pSpell == m_SpellsPrepared.GetNext(pos)->GetSpell()) {
			return true;
		}
	}
	return false;
}

///////////////////////
//	IsSpellPrepared
//	check prepared LL for spell
///////////////////

template<class CasterClass>
bool CCaster<CasterClass>::IsSpellBeingPrepared(const CSpell<CasterClass> * pSpell) {
	POSITION pos = m_SpellsBeingPrepared.GetStartPosition();
	while (pos) {
		if (pSpell == m_SpellsBeingPrepared.GetNext(pos)->GetSpell()) {
			return true;
		}
	}
	return false;
}

//////////////////////////////
//	DoForget
//	Check to see if that Spell exists
//	check to see if they have it prepared
//	If both yes reduce prepared.
//	written by: Demetrius Comes
//	12-7-98
/////////////////////////////

template<class CasterClass>
void CCaster<CasterClass>::DoForget() {
	CString strSpell(CurrentCommand.GetWordAfter(1, true));
	const CSpell<CasterClass> *pSpell;
	CString strToChar;
	if (!(pSpell = this->GetSpellInfo(strSpell))) {
		SendToChar("Forget what?\r\n");
	} else {
		if (!IsSpellBeingPrepared(pSpell)) {
			if (!IsSpellPrepared(pSpell)) {
				strToChar = "You can't forget what you don't have!\r\n";
			} else {
				strToChar.Format("You forget %s.\r\n", pSpell->GetSpellName().cptr());
				ReducePrepared(pSpell);
			}
		} else {
			strToChar.Format("You forget %s.\r\n", pSpell->GetSpellName().cptr());
			RemovePreparing(pSpell);
		}
	}
	SendToChar(strToChar);
}

template<class CasterClass>
void CCaster<CasterClass>::DoPractice(CString strToPractice, const CCharacter *pTeacher) {
	const CSpell<CasterClass> *pSpell;
	CString strToChar;
	if ((pSpell = this->GetSpellInfo(strToPractice))) {
		skill_type nPctLearn;
		if (this->GetLevel() < pSpell->GetMinLevel()) {
			SendToChar("You need more levels first!\r\n");
		} else if ((nPctLearn = pTeacher->CanTeach(this, pSpell->GetSpellID(), false)) > 0) {
			if (m_Spells[pSpell->GetSpellID()] == 0) {
				sMoney Cost;
				if (CanAffordTraining(Cost, pSpell->GetSpellID(), pTeacher, false)) {
					strToChar.Format("You learn the spell %s.\r\n", pSpell->GetColorizedName().cptr());
					m_Spells[pSpell->GetSpellID()] = (skill_type) DIE(nPctLearn);
					m_nSpellSphere |= pSpell->GetSphere();

					if (!Cost.IsEmpty()) {
						strToChar.Format("%s%s\r\n", strToChar.cptr(), Cost.GetMoneyString().cptr());
					}
					SendToChar(strToChar);
				} else {
					SendToChar("You can't seem to afford the training.\r\n");
				}
			} else {
				SendToChar("You already know that spell.\r\n");
			}
		}
	} else {
		CCharacter::DoPractice(strToPractice, pTeacher);
	}
}

//////////////////////////////////
//	OffensiveSpell
//	this function runs for all mage offensive spells
//	do checks to see if we can cast in this room etc.
//	next check to see if spell is room affect
//	or target
//	written by: Demetrius Comes

template<class CasterClass>
void CCaster<CasterClass>::OffensiveSpell(const CInterp * pInterp, const CSpell<CasterClass> * pSpell) {
	if (m_pInRoom->IsAffectedBy(CRoom::ROOM_AFFECT_PEACEFUL)) {
		SendToChar("This place is far to peacefull.\r\n");
		return;
	}

	CCharacter *pTarget;
	int nDam;
	if (pSpell->GetAffectedArea() & AREA_ROOM) {
		//get first character in room
		POSITION pos = m_pInRoom->GetFirstCharacterInRoom();
		while (pos) {
			pTarget = m_pInRoom->GetNextCharacter(pos);
			//make sure it's not me
			if (pTarget != this && !m_Group.IsInGroup(pTarget)) {
				nDam = this->CalcSpellDam(pSpell);
				pTarget->SpellDefense(nDam, this, pSpell->GetMinLevel());
				if (pTarget->IsDead()) {
					m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_KILLED, this, pTarget);
					pInterp->KillChar(pTarget);
				} else {
					if (nDam > 0) {
						m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, pTarget);
					}
					AddFighters(pTarget, false);
				}
			}
		}
	}        //we are casting at self or someone else not a room
	else {
		pTarget = m_CurrentSpell.m_pTarget;
		if (!CastingTargetInRoom()) {
			SendToChar("Who?\r\n");
		} else {
			nDam = this->CalcSpellDam(pSpell);
			//spell damage calls take damage
			pTarget->SpellDefense(nDam, this, pSpell->GetMinLevel());

			if (pTarget->IsDead()) {
				m_MsgManager.SpellMsg(m_CurrentSpell.m_pSpell->GetSpellName(), SKILL_KILLED, this, pTarget);
				pInterp->KillChar(pTarget);
			} else {
				if (nDam > 0) {
					m_MsgManager.SpellMsg(pSpell->GetSpellName(), SKILL_WORKED, this, pTarget);
				}
				AddFighters(pTarget, false);
			}
		}
	}
}

template<class CasterClass>
void CCaster<CasterClass>::BuildPracticePriceSpells(const CCharacter *pToBeTaught, CString &str) const {
	if (this->IsTeacher()) {
		sMoney Cost;
		str.Format("%sI can teach you the following spells:\r\n", str.cptr());
		POSITION pos = GetSpellTable()->GetStartingPosition();
		const CSpell<CasterClass> *pSpell;
		while (pos) {
			pSpell = GetSpellTable()->GetNext(pos);
			if (IsGoodEnoughToTeach(pToBeTaught, pSpell->GetSpellID(), false)) {
				Cost = GetTrainingCosts(pToBeTaught, pSpell->GetSpellID(), false);
				str.Format("%s%-30s\t%s\r\n", str.cptr(), pSpell->GetSpellName().cptr(), Cost.GetMoneyString().cptr());
			}
		}
	} else {
		str = "My skills are to value-able for me to teach anyone.\r\n";
	}
}
#endif

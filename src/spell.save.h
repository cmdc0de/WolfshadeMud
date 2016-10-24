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
// SpellSave.h: interface for the sSpellSave struct.
//	Contains the spells the player has prepared when
//	saved...must be a struct...so we can use read and write
//	functions otherwise we over write the v_ptr...DOH!
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPELLSAVE_H__5D0164A3_BFF9_11D2_81AB_00600834E9F3__INCLUDED_)
#define AFX_SPELLSAVE_H__5D0164A3_BFF9_11D2_81AB_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MAX_SPELLS_PREPARED_FOR_SAVE 30
#define MAX_SPELLS_PREPARING_FOR_SAVE 30
#include "spell.h"

template<class CasterClass>
class sSpellSave {
    //saving spells preped and preparing
public:

    struct sPreped {
        short m_nNumber;
        char m_strSpellName[MAX_SPELL_NAME_LENGTH];

        sPreped() {
            m_nNumber = 0;
            memset(m_strSpellName, '\0', (sizeof (char)*MAX_SPELL_NAME_LENGTH));
        }

        bool IsEmpty() const {
            return m_nNumber == 0;
        }
    };

    struct sPreparingSave {
    protected:
        bool m_bEmpty;
        long m_lTimeLeft, m_lOriginalTime;
        char m_strSpellName[MAX_SPELL_NAME_LENGTH];
    public:

        sPreparingSave() {
            m_bEmpty = true;
            m_lTimeLeft = m_lOriginalTime = 0;
            memset(m_strSpellName, '\0', (sizeof (char)*MAX_SPELL_NAME_LENGTH));
        }

        sPreparingSave &operator=(const CPreparing<CasterClass>*pPreparing) {
            m_bEmpty = false;
            m_lOriginalTime = pPreparing->GetOriginalTime();
            m_lTimeLeft = pPreparing->GetPulsesLeft();
            strcpy(m_strSpellName, pPreparing->GetSpell()->GetSpellName().cptr());
            return *this;
        }

        bool IsEmpty() const {
            return m_bEmpty;
        }

        const char *GetName() const {
            return m_strSpellName;
        }

        long GetTimeLeft() const {
            return m_lTimeLeft;
        }

        long GetOriginalTime() const {
            return m_lOriginalTime;
        }
    };
protected:
    sPreped m_PreparedSpells[MAX_SPELLS_PREPARED_FOR_SAVE];
    sPreparingSave m_PreparingSpells[MAX_SPELLS_PREPARING_FOR_SAVE];
public:

    void AddPrepared(const CLinkList<CPrepared<CasterClass> > *pPrepLL) {
        POSITION pos = pPrepLL != NULL ? pPrepLL->GetStartPosition() : NULL;
        short i = 0;
        CPrepared<CasterClass> *pPrep;
        while (pos && i < MAX_SPELLS_PREPARED_FOR_SAVE) {
            pPrep = pPrepLL->GetNext(pos);
            m_PreparedSpells[i].m_nNumber = (short) (*pPrep);
            strcpy(m_PreparedSpells[i].m_strSpellName, pPrep->GetSpell()->GetSpellName().cptr());
            i++;
        }
    }

    void AddPreparing(const CLinkList<CPreparing<CasterClass> >*pPreparingLL) {
        POSITION pos = pPreparingLL != NULL ? pPreparingLL->GetStartPosition() : NULL;
        short i = 0;
        CPreparing<CasterClass> *pPreparing;
        while (pos && i < MAX_SPELLS_PREPARING_FOR_SAVE) {
            pPreparing = pPreparingLL->GetNext(pos);
            m_PreparingSpells[i] = pPreparing;
            i++;
        }
    }
public:
    sSpellSave();

    const sPreped &GetPreparedSpell(short i) {
        return m_PreparedSpells[i];
    }

    const sPreparingSave &GetPreparingSpell(short i) {
        return m_PreparingSpells[i];
    }

    short GetMaxSpellsPrepared() {
        return MAX_SPELLS_PREPARED_FOR_SAVE;
    }

    short GetMaxSpellsPreparing() {
        return MAX_SPELLS_PREPARING_FOR_SAVE;
    }
};

template<class CasterClass>
sSpellSave<CasterClass>::sSpellSave() {

}
#endif // !defined(AFX_SPELLSAVE_H__5D0164A3_BFF9_11D2_81AB_00600834E9F3__INCLUDED_)

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

#ifndef _GLOBAL_FUNCTIONS_H_
#define _GLOBAL_FUNCTIONS_H_

#include "stdinclude.h"
#pragma hdrstop

CCharacter * CalcClassPtr(CMobPrototype *mob, CRoom *room) {
    CCharacter *pCh;
    switch (mob->GetClassType()) {
        case CLASS_WARRIOR:
            pCh = (new CNPCWarrior(*mob, room));
            break;
        case CLASS_ROGUE:
            pCh = (new CNPCRogue(*mob, room));
            break;
        case CLASS_MAGE:
            pCh = (new CNPCMage(*mob, room));
            break;
        case CLASS_PSIONICIST:
            pCh = (new CNPCPsionicist(*mob, room));
            break;
        case CLASS_MINDFLAYER:
            pCh = (new CNPCMindflayer(*mob, room));
            break;
        case CLASS_EMPATH:
            pCh = (new CNPCEmpath(*mob, room));
            break;
        case CLASS_CLERIC:
            pCh = (new CNPCCleric(*mob, room));
            break;
        case CLASS_SHAMAN:
            pCh = (new CNPCShaman(*mob, room));
            break;
        case CLASS_DRUID:
            pCh = (new CNPCDruid(*mob, room));
            break;
        case CLASS_PALADIN:
            pCh = (new CNPCPaladin(*mob, room));
            break;
        case CLASS_ANTI_PALADIN:
            pCh = (new CNPCAntiPaladin(*mob, room));
            break;
        case CLASS_DEFILER:
            pCh = (new CNPCDefiler(*mob, room));
            break;
        case CLASS_RANGER:
            pCh = (new CNPCRanger(*mob, room));
            break;
        default:
            ErrorLog << "default in calcclassptr" << endl;
            return NULL;
            break;
    }
    pCh->InitializeCharacter();
    return pCh;
}

CCharacter * CalcClassPtr(CCharIntermediate *ch) {
    CCharacter *pCh;
    switch (ch->GetClass()) {
        case CLASS_WARRIOR:
            pCh = (new CWarrior(ch));
            break;
        case CLASS_ROGUE:
            pCh = (new CRogue(ch));
            break;
        case CLASS_MAGE:
            pCh = (new CMage(ch));
            break;
        case CLASS_PSIONICIST:
            pCh = (new CPsionicist(ch));
            break;
        case CLASS_MINDFLAYER:
            pCh = (new CMindflayer(ch));
            break;
        case CLASS_EMPATH:
            pCh = (new CEmpath(ch));
            break;
        case CLASS_CLERIC:
            pCh = (new CCleric(ch));
            break;
        case CLASS_SHAMAN:
            pCh = (new CShaman(ch));
            break;
        case CLASS_DRUID:
            pCh = (new CDruid(ch));
            break;
        case CLASS_PALADIN:
            pCh = (new CPaladin(ch));
            break;
        case CLASS_ANTI_PALADIN:
            pCh = (new CAntiPaladin(ch));
            break;
        case CLASS_DEFILER:
            pCh = (new CDefiler(ch));
            break;
        case CLASS_RANGER:
            pCh = (new CRanger(ch));
            break;
        default:
            ErrorLog << "default in CalcClassPtr" << endl;
            return NULL;
            break;
    }
    pCh->InitializeCharacter();
    return pCh;
}

UINT HashKey(CString &s, int nLetters) {
    return HashKey(s.cptr(),nLetters);
}

UINT HashKey(const char *str, int nLetters) {
    UINT nHash = 0;
    int ncount = 0;
    while (*str != '\0' && nLetters != -1 && ncount < nLetters) {
        ncount++;
        nHash += tolower(*str++);
    }
    return nHash;
}

UINT HashKey(long key, int nLetters) {
    return ((UINT) key) << 4;
}

CWolfshadeLog &endl(CWolfshadeLog &log) {
    return log.end();
}

CWolfshadeLog &_endl(CWolfshadeLog &log, CString &str) {
    return log.end(str);
}

WSMMANIP endl(CString &str) {
    return WSMMANIP(_endl, &str);
}

class Stack {
public:
    CObject *m_pObj;
    short m_nNumber;

    Stack(CObject *pObj) {
        m_pObj = pObj;
        m_nNumber = 1;
    }
};

void StackObjects(const CList<CObject *> &Objs, const CCharacter *pLooker, CString &strLook, bool bInInventory) {
    CLinkList<Stack> ObjStack;
    ObjStack.SetDeleteData();
    Stack *pStack;

    POSITION pos = Objs.GetStartPosition();
    bool bFound;
    CObject *pObj;
    while (pos) {
        bFound = false;
        pObj = Objs.GetNext(pos);
        if (pLooker->CanSee(pObj)) {
            POSITION pos2 = ObjStack.GetStartPosition();
            while (pos2) {
                pStack = ObjStack.GetNext(pos2);
                if (pObj->IsType(ITEM_TYPE_CORPSE) && pStack->m_pObj->IsType(ITEM_TYPE_CORPSE)) {
                    if (pObj->GetDesc() == pStack->m_pObj->GetDesc()) {
                        bFound = true;
                    }
                } else if (pStack->m_pObj->GetVnum() == pObj->GetVnum()) {
                    if (pObj->GetDesc() == pStack->m_pObj->GetDesc()) {
                        bFound = true;
                    }
                }
                if (bFound) {
                    pStack->m_nNumber++;
                    break;
                }
            }
            if (!bFound) {
                ObjStack.Add(new Stack(pObj));
            }
        }
    }
    pos = ObjStack.GetStartPosition();
    CString strExtra;
    if (pos) {
        while (pos) {
            pStack = ObjStack.GetNext(pos);
            strExtra = pStack->m_pObj->GetExtraString();
            if (pStack->m_nNumber == 1) {
                if (strExtra.IsEmpty()) {
                    strLook.Format("%s%s\r\n", (char *) strLook,
                            (bInInventory ? pStack->m_pObj->GetObjName() :
                            pStack->m_pObj->GetObjSittingDesc()));
                } else {
                    strExtra.SkipSpaces();
                    strLook.Format("%s%s (%s)\r\n", (char *) strLook,
                            (bInInventory ? pStack->m_pObj->GetObjName() :
                            pStack->m_pObj->GetObjSittingDesc()),
                            (char *) strExtra);
                }
            } else {
                if (strExtra.IsEmpty()) {
                    strLook.Format("%s[%d] %s\r\n", (char *) strLook,
                            pStack->m_nNumber,
                            (bInInventory ? pStack->m_pObj->GetObjName() :
                            pStack->m_pObj->GetObjSittingDesc()));
                } else {
                    strLook.Format("%s[%d] %s (%s)\r\n", (char *) strLook,
                            pStack->m_nNumber,
                            (bInInventory ? pStack->m_pObj->GetObjName() :
                            pStack->m_pObj->GetObjSittingDesc()),
                            (char *) strExtra);
                }
            }
        }
    }
}
#endif
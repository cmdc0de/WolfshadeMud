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
 *	Global handling
 *		This class handles all global variables except log classes
 *
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "linklist.h"

class CGame;
class CInterp;

class CGlobalVariableManagement {
    friend class CGame;
    friend class CInterp;
private:
    static bool m_bCreated;
    static const char *ColorClasses[TOTAL_CLASSES];
    static const char *ColorRaces[TOTAL_RACES];
    static const char *Races[TOTAL_RACES];
    static const char *Classes[TOTAL_CLASSES];
    static const char *Gods[TOTAL_GODS];
    static const char *ColorGods[TOTAL_GODS];
    CList<CCharacter *> m_GodLL;
    CLinkList<CCharacter> PlayerLL;
public:
    void SendToOOC(CString strToOOC, CCharacter *pNoSendChar);
    void SendToOutdoors(CString strMsg);
    void SendToGods(CString strToSend, int nTogs, CCharacter *pCharNotToSendTo);
    void SendToAll(CString strToAll, CCharacter *pCharDontSend);
    CCharacter * FindCharacter(CString strName, CCharacter *pSelf);
    ~CGlobalVariableManagement();
    CGlobalVariableManagement();
    inline void Add(CCharacter *pch);
    inline void Remove(CCharacter *pCh, POSITION pos);

    void AddGod(CCharacter *pCh) {
        m_GodLL.Add(pCh);
    } //only call these if you are not entering or logging from game

    void RemoveGod(CCharacter *pCh) {
        m_GodLL.Remove(pCh);
    }
    inline const char *GetRaceName(int nRace);
    inline const char *GetClassName(int nClass);
    inline const char *GetColorRaceName(int nColRace);
    inline const char *GetColorClass(int nColClass);
    inline const char *GetColorGod(int nColGod);
    inline const char *GetGodName(int nGod);
    inline int GetRaceNum(CString strRace);
};

inline int CGlobalVariableManagement::GetRaceNum(CString strRace) {
    for (int i = 0; i < TOTAL_RACES; i++) {
        if (strRace.Compare(Races[i]))
            return i;
    }
    return -1;
}

inline const char *CGlobalVariableManagement::GetClassName(int nClass) {
    if (nClass < 0 || nClass >= TOTAL_RACES)
        return "Illegal Class Name!";
    return Classes[nClass];
}

inline const char *CGlobalVariableManagement::GetColorClass(int nColClass) {
    if (nColClass < 0 || nColClass >= TOTAL_CLASSES)
        return "Illegal Class Name!";
    return ColorClasses[nColClass];
}

inline const char *CGlobalVariableManagement::GetRaceName(int nRace) {
    if (nRace < 0 || nRace >= TOTAL_RACES)
        return "Illegal Race Name!";
    return Races[nRace];
}

inline const char *CGlobalVariableManagement::GetGodName(int nGod) {
    if (nGod < 0 || nGod >= TOTAL_GODS)
        return "Illegal God Name!";
    return Gods[nGod];
}

inline const char *CGlobalVariableManagement::GetColorRaceName(int nColRace) {
    if (nColRace < 0 || nColRace >= TOTAL_RACES)
        return "Illegal Race Name!";
    return ColorRaces[nColRace];
}

inline const char *CGlobalVariableManagement::GetColorGod(int nColGod) {
    if (nColGod < 0 || nColGod >= TOTAL_GODS)
        return "Illegal God Name!";
    return ColorGods[nColGod];
}

inline void CGlobalVariableManagement::Add(CCharacter *pch) {
    PlayerLL.Add(pch);
    //we no longer will mess with godLL when we add a character
    //we'll use the separate AddGod function
}

inline void CGlobalVariableManagement::Remove(CCharacter *pCh, POSITION pos) {
    if (!pCh->IsNPC() && pCh->IsGod()) {
        RemoveGod(pCh);
    }
    pCh->CleanUp();
    PlayerLL.Remove(pCh, pos);
}
#endif

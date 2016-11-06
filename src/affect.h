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
//////////////////////////////////////////////////////////////
// Affect.h: interface for the CAffect class.
//
//	This gets saved to disk so it should be a struct if we use
//	a class we risk over writing the v_ptr when we use ifstream::read()
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFFECT_H__758FD083_A666_11D2_81AA_00600834E9F3__INCLUDED_)
#define AFX_AFFECT_H__758FD083_A666_11D2_81AA_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

struct sAffect {
public:
    static const int AFFECT_OFF,
    PERM_AFFECT,
    MANUAL_AFFECT;
public:
    int m_nTimeOfAffect,
    m_nTime,
    m_nValue;

    void Set(int nTime, int nValue) {
        m_nTime = nTime;
        m_nValue = nValue;
    }

    bool HasAffect() const {
        return (m_nTime != AFFECT_OFF || m_nTime == PERM_AFFECT || m_nTime == MANUAL_AFFECT);
    }

    bool IsPermAffect() const {
        return m_nTime == PERM_AFFECT;
    }

    bool IsManual() const {
        return m_nTime == MANUAL_AFFECT;
    }

    bool IsAffectTime(int nTimeOfAffect) {
        if (m_nTimeOfAffect == -1) {
            m_nTimeOfAffect = nTimeOfAffect;
        }
        if (--m_nTimeOfAffect < 0) {
            m_nTimeOfAffect = nTimeOfAffect;
            return true;
        }
        return false;
    }

    void Remove() {
        if (!IsPermAffect()) {
            m_nTime = m_nValue = AFFECT_OFF;
            m_nTimeOfAffect = -1;
        }
    }

    sAffect() {
        m_nTimeOfAffect = -1;
        m_nTime = m_nValue = AFFECT_OFF;
    }
    ~sAffect();
};

#endif // !defined(AFX_AFFECT_H__758FD083_A666_11D2_81AA_00600834E9F3__INCLUDED_)

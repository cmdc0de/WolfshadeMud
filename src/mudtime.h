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
// mudtime.h: interface for the CMudTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUDTIME_H__C40F4B93_1F83_11D3_844F_00600834E9F3__INCLUDED_)
#define AFX_MUDTIME_H__C40F4B93_1F83_11D3_844F_00600834E9F3__INCLUDED_

#if _MSC_VER >= 1000
	#pragma once
#endif // _MSC_VER >= 1000

#ifndef _WOLFSHADE_WINDOWS_
#include <sys/resource.h>
#include <sys/time.h>
#else
#include <time.h>
#endif
#include "wolfshade.string.h"

class CMudTime  
{
private:
	struct sSeason
	{
		short m_nMonth,
			m_nDay,
			m_nSunRise,
			m_nSunSet;
		CString m_strSeasonMsg;
		sSeason(short m=0,short d=0,short rise=0, short set=0, const char *str="") 
		{m_nMonth = m;m_nDay = d;m_nSunSet = set;m_nSunRise= rise,m_strSeasonMsg = str;}
		sSeason &operator=(const sSeason &r)
		{m_nMonth=r.m_nMonth;m_nDay=r.m_nDay;m_nSunRise=r.m_nSunRise;m_nSunSet=r.m_nSunSet;m_strSeasonMsg=r.m_strSeasonMsg;return *this;}
	};
	struct sMudTime
	{
		short m_nYear,
			m_nMonth,
			m_nDay,
			m_nHours,
			m_nMinutes,
			m_nSeconds;
		sSeason m_CurrentSeason;
		sMudTime(){m_nYear=m_nMonth=m_nDay=m_nHours=m_nMinutes=m_nSeconds=0;}
		bool IsDayTime()
		{return (m_nHours>=m_CurrentSeason.m_nSunRise && m_nHours<=m_CurrentSeason.m_nSunSet);}
		bool IsBrightPartOfDay()
		{
			if(!IsDayTime())
				return false;
			else
			{
				int nMidDay = (m_CurrentSeason.m_nSunSet + m_CurrentSeason.m_nSunRise)>>1;
				int nHours = m_CurrentSeason.m_nSunSet - m_CurrentSeason.m_nSunRise;
				nHours>>=2;
				if(m_nHours>=(nMidDay-nHours) && m_nHours<=(nMidDay+nHours))
					return true;
				return false;
			}
		}
		bool IsFirstDayOfYear()
		{return (m_nMonth==0 && m_nDay==0 && m_nHours==0 && m_nMinutes==0 && m_nSeconds==0);}
		bool IsLastHourOfDay()
		{return (m_nHours==m_CurrentSeason.m_nSunSet && m_nMinutes==0 && m_nSeconds==0);}
		bool IsFirstHourOfDay() 
		{return (m_nHours==m_CurrentSeason.m_nSunRise && m_nMinutes==0 && m_nSeconds==0);}
		bool IsFirstDayOfSeason(const sSeason &r)
		{return (m_nMonth==r.m_nMonth && m_nDay==r.m_nDay && m_nHours==r.m_nSunRise && m_nMinutes==0 && m_nSeconds==0);}
		bool operator >=(const sSeason &r)
		{return (m_nMonth>r.m_nMonth || (m_nMonth==r.m_nMonth && m_nDay>=r.m_nDay));}
		bool operator < (const sSeason &r)
		{return (m_nMonth<r.m_nMonth || (m_nMonth==r.m_nMonth && m_nDay<r.m_nDay));}
	};
private:
	static const char *m_Months[];
	static const char *m_Days[];
	static const short m_YearPC_Dam_Adj[];
	static const short PC_ADJUST_YEARS;
	static const char *m_strYear[];
public:
	static bool IsDayTime() {return m_CurrentTime.IsDayTime();}
	static bool IsBrightPartOfDay() {return m_CurrentTime.IsBrightPartOfDay();}
public:
	static const int PULSES_PER_SEC;
	//static const int CLOCKS_PER_PULSE	(CLOCKS_PER_SEC/PULSES_PER_SEC) 
	static const int REAL_SECS_PER_MUD_YEAR;
	static const int PULSES_PER_REAL_SECOND;
	static const int PULSES_PER_REAL_MIN;
	static const int PULSES_PER_REAL_HOUR;
	static const int PULSES_PER_CHECK_CONNECTIONS;
	static const int PULSES_PER_DO_VIOLENCE;
	static const int PULSES_PER_DO_AUTO_SAVE;
	static const int PULSES_PER_CHECK_ZONE_RESET;
	static const int PULSES_PER_CASTING_ROUND;
	static const int PULSES_PER_PREP_ROUND;
	static const int PULSES_PER_BATTLE_ROUND;
	static const int ROLL_OVER_PULSES;
	static const int REDUCE_OBJECT_TIMERS;
	static const int PULSES_PER_MUD_MIN;
	static const int PULSES_PER_MUD_HOUR;
	static const int PULSES_PER_CHECK_REMOVE_STUN;
	static const int PULSES_PER_CHECK_REMOVE_KOED;
	static const int PULSES_PER_MUD_DAY;
	static const int PULSES_PER_MUD_WEEK;
	static const int MUD_DAYS_PER_MONTH;
	static const int MUD_MONTHS_PER_YEAR;
	static const int PULSES_PER_MUD_YEAR;
	static const int FADING_TIME;
	static const int FADING_RAPIDLY_TIME;
	static const int DEFAULT_FUGUE_PLANE_TIME;
	static const int PULSES_PER_MUD_MONTH;
	static const int MUD_DAYS_PER_MUD_YEAR;
	static const int USEC_PER_PULSE;
	static bool m_bInit;
	static const sSeason SEASON_FALL;
	static const sSeason SEASON_WINTER;
	static const sSeason SEASON_SPRING;
	static const sSeason SEASON_SUMMER;
	static const short SUNRISE;
	static const short SUNSET;
private:
	static time_t m_dtBootTime;
	static long m_lMissedPulses;
	static struct timeval m_LastTime;
	static struct sMudTime m_CurrentTime;
	static int m_nCurrentPulse;
	static long m_lTotalPulses;
	static long m_lTotalMissed;
public:
	static const int GetCurrentPulse() {return m_nCurrentPulse;}
	static CString GetTimeString();
	CMudTime();
public:
	static CString GetStatInfo();
	static short GetPCYearDamageAdjustment();
	void NextPulse();
	virtual ~CMudTime();
protected:
	void UpdateMudTime(time_t t);
	void ChangeSeason();
	inline struct timeval TimeDiff(struct timeval a, struct timeval b);
#ifdef _WOLFSHADE_WINDOWS_
	inline void gettimeofday(struct timeval *t, struct timezone *dummy);
#endif
};

/* Windows doesn't have gettimeofday, so we'll simulate it. */
#ifdef _WOLFSHADE_WINDOWS_
inline void CMudTime::gettimeofday(struct timeval *t, struct timezone *dummy)
{
  DWORD millisec = GetTickCount();
  t->tv_sec = (int) (millisec / 1000);
  t->tv_usec = (millisec % 1000) * 1000;
}
#endif

inline struct timeval CMudTime::TimeDiff(struct timeval a, struct timeval b)
{
	struct timeval rslt = {0,0};

	if (a.tv_sec == b.tv_sec) 
	{
		if (a.tv_usec > b.tv_usec)
		{
			rslt.tv_sec = 0;
			rslt.tv_usec = a.tv_usec - b.tv_usec;
		}
	}
	else if (a.tv_sec > b.tv_sec)
	{			/* a->tv_sec > b->tv_sec */
		rslt.tv_sec = a.tv_sec - b.tv_sec;
		if (a.tv_usec < b.tv_usec) 
		{
			rslt.tv_usec = a.tv_usec + 1000000 - b.tv_usec;
			rslt.tv_sec--;
		}
		else
		{
			rslt.tv_usec = a.tv_usec - b.tv_usec;
		}
	}
	return rslt;
}
#endif // !defined(AFX_MUDTIME_H__C40F4B93_1F83_11D3_844F_00600834E9F3__INCLUDED_)

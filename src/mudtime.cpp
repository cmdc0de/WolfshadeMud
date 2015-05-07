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
// mudtime.cpp: implementation of the CMudTime class.
//
//////////////////////////////////////////////////////////////////////

#include "stdinclude.h"
#pragma hdrstop
#include "mudtime.h"

#ifndef _WOLFSHADE_WINDOWS
#include <errno.h>
#endif

extern CGlobalVariableManagement GVM;
#define USEC 100000
//////////////////////////////////
//	statics
/////////////////////////////////
const int CMudTime::PULSES_PER_SEC=10;
//const int CMudTime::CLOCKS_PER_PULSE	(CLOCKS_PER_SEC/PULSES_PER_SEC) 
//real time in pulses
const int CMudTime::PULSES_PER_REAL_SECOND=CMudTime::PULSES_PER_SEC;
const int CMudTime::PULSES_PER_REAL_MIN=(CMudTime::PULSES_PER_REAL_SECOND *60);
const int CMudTime::PULSES_PER_REAL_HOUR=(CMudTime::PULSES_PER_REAL_MIN * 60);
//mud time by pulses
const int CMudTime::PULSES_PER_MUD_HOUR=(CMudTime::PULSES_PER_REAL_MIN);
const int CMudTime::PULSES_PER_MUD_MIN=(CMudTime::PULSES_PER_MUD_HOUR/60);
const int CMudTime::PULSES_PER_MUD_DAY=(CMudTime::PULSES_PER_REAL_MIN*24);
const int CMudTime::PULSES_PER_MUD_WEEK=(CMudTime::PULSES_PER_MUD_DAY*7);

const int CMudTime::MUD_DAYS_PER_MONTH=33;
const int CMudTime::PULSES_PER_MUD_MONTH = (CMudTime::PULSES_PER_MUD_DAY*CMudTime::MUD_DAYS_PER_MONTH);
const int CMudTime::MUD_MONTHS_PER_YEAR=12;
const int CMudTime::MUD_DAYS_PER_MUD_YEAR=(CMudTime::MUD_MONTHS_PER_YEAR*CMudTime::MUD_DAYS_PER_MONTH);
const int CMudTime::PULSES_PER_MUD_YEAR = (CMudTime::PULSES_PER_MUD_DAY * CMudTime::MUD_DAYS_PER_MUD_YEAR);

const int CMudTime::REAL_SECS_PER_MUD_YEAR=(PULSES_PER_MUD_YEAR/PULSES_PER_SEC);
//other
const int CMudTime::PULSES_PER_CHECK_CONNECTIONS=(PULSES_PER_REAL_SECOND*2);
const int CMudTime::PULSES_PER_DO_VIOLENCE=(CMudTime::PULSES_PER_REAL_SECOND*2);
const int CMudTime::PULSES_PER_DO_AUTO_SAVE=(CMudTime::PULSES_PER_REAL_MIN * 5);
const int CMudTime::PULSES_PER_CHECK_ZONE_RESET=CMudTime::PULSES_PER_REAL_MIN;
const int CMudTime::PULSES_PER_CASTING_ROUND=CMudTime::PULSES_PER_REAL_SECOND;
const int CMudTime::PULSES_PER_PREP_ROUND=CMudTime::PULSES_PER_REAL_SECOND;
const int CMudTime::PULSES_PER_BATTLE_ROUND=CMudTime::PULSES_PER_DO_VIOLENCE;
const int CMudTime::ROLL_OVER_PULSES=(10*CMudTime::PULSES_PER_REAL_HOUR);
const int CMudTime::REDUCE_OBJECT_TIMERS=(CMudTime::PULSES_PER_REAL_SECOND);
const int CMudTime::PULSES_PER_CHECK_REMOVE_STUN=(CMudTime::PULSES_PER_BATTLE_ROUND * 2);
const int CMudTime::PULSES_PER_CHECK_REMOVE_KOED=(CMudTime::PULSES_PER_MUD_HOUR/2);
const int CMudTime::FADING_TIME=(CMudTime::PULSES_PER_MUD_HOUR*3);
const int CMudTime::FADING_RAPIDLY_TIME=(CMudTime::PULSES_PER_MUD_HOUR);
const int CMudTime::DEFAULT_FUGUE_PLANE_TIME = (CMudTime::PULSES_PER_REAL_SECOND * 10);
const int CMudTime::USEC_PER_PULSE = USEC/CMudTime::PULSES_PER_SEC;
int CMudTime::m_nCurrentPulse = 0;
long CMudTime::m_lTotalPulses=0;
long CMudTime::m_lTotalMissed=0;
bool CMudTime::m_bInit=false;
time_t CMudTime::m_dtBootTime = time(NULL);
long CMudTime::m_lMissedPulses=0;
struct timeval CMudTime::m_LastTime;
struct CMudTime::sMudTime CMudTime::m_CurrentTime;
const CMudTime::sSeason CMudTime::SEASON_FALL(9,15,7,19,"Today is the first day of fall.\r\n");
const CMudTime::sSeason CMudTime::SEASON_WINTER(12,15,9,18,"Today is the first day of winter.\r\n");
const CMudTime::sSeason CMudTime::SEASON_SPRING(3,15,7,19,"Today is the first day of spring.\r\n");
const CMudTime::sSeason CMudTime::SEASON_SUMMER(6,15,6,21,"Today is the first day of summer.\r\n");

const char *CMudTime::m_Months[] = 
{
	"Month 1",
	"Month 2",
	"Month 3",
	"Month 4",
	"Month 5",
	"Month 6",
	"Month 7",
	"Month 8",
	"Month 9",
	"Month 10",
	"Month 11",
	"Month 12"
};

const char *CMudTime::m_Days[] = 
{
	"Day 1",
	"Day 2",
	"Day 3",
	"Day 4",
	"Day 5",
	"Day 6",
	"Day 7",
	"Day 8",
	"Day 9",
	"Day 10",
	"Day 11",
	"Day 12",
	"Day 13",
	"Day 14",
	"Day 15",
	"Day 16",
	"Day 17",
	"Day 18",
	"Day 19",
	"Day 20",
	"Day 21",
	"Day 22",
	"Day 23",
	"Day 24",
	"Day 25",
	"Day 26",
	"Day 27",
	"Day 28",
	"Day 29",
	"Day 30",
	"Day 31",
	"Day 32",
	"Day 33"
};
const short CMudTime::PC_ADJUST_YEARS = 8;
const short CMudTime::m_YearPC_Dam_Adj[] = {25,30,35,40,45,50,60,70};
const char *CMudTime::m_strYear[] = 
{
	"Year of the bunny",
	"Year of the lamb",
	"Year of the snake",
	"Year of the badger",
	"Year of the tiger",
	"Year of the wounded",
	"Year of the massacre",
	"Year of the &rbloody&n",
	"&F&RYEAR OF BLOODLUST&n"
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMudTime::CMudTime()
{
	UpdateMudTime(time(NULL));
	if(m_CurrentTime >= SEASON_FALL &&
		m_CurrentTime < SEASON_WINTER)
	{
		m_CurrentTime.m_CurrentSeason=SEASON_FALL;
	}
	else if(m_CurrentTime >= SEASON_WINTER &&
		m_CurrentTime < SEASON_SPRING)
	{
		m_CurrentTime.m_CurrentSeason=SEASON_WINTER;
	}
	else if(m_CurrentTime >= SEASON_SPRING &&
		m_CurrentTime < SEASON_SUMMER)
	{
		m_CurrentTime.m_CurrentSeason=SEASON_SPRING;
	}
	else
	{
		m_CurrentTime.m_CurrentSeason=SEASON_SUMMER;
	}
}

CMudTime::~CMudTime()
{
	if(m_lTotalPulses>0 || m_nCurrentPulse>0)
	{
		MudLog << "Total Pulses: " << (m_lTotalPulses*ROLL_OVER_PULSES)+m_nCurrentPulse+m_lTotalMissed << endl;
		MudLog << "Missed Pulses: " << m_lTotalMissed << endl;
		MudLog << "%: " << (m_lTotalMissed*100)/((m_lTotalPulses*ROLL_OVER_PULSES)+m_nCurrentPulse+m_lTotalMissed) << endl;
	}
}

//////////////////////////////////////
//	Calculates time from last Pulse
//	see's if we need to sleep, make up for
//  lost pulses, or run keep running
void CMudTime::NextPulse()
{
	if(!m_bInit)
	{
		gettimeofday(&m_LastTime,(struct timezone *)0);
		m_bInit = true;
	}
	struct timeval dtCurrent = {0,0};
	gettimeofday(&dtCurrent,(struct timezone *) 0);
	
	struct timeval dtProcessTime = TimeDiff(dtCurrent,m_LastTime);

	if (dtProcessTime.tv_sec != 0 || dtProcessTime.tv_usec > USEC) 
	{
		m_lMissedPulses += dtProcessTime.tv_sec * PULSES_PER_SEC;
		m_lMissedPulses += dtProcessTime.tv_usec / USEC;
        m_lTotalMissed+=m_lMissedPulses;
		
		if(m_lMissedPulses>100)
		{
			MudLog << "Missed over 100 pulses giving up on catching up!" << endl;
			m_lMissedPulses = 0;
		}
    }

	if(m_lMissedPulses<=0)
	{
		struct timeval timeout = {0,(USEC-(dtProcessTime.tv_usec % USEC))};
		
#ifdef _WOLFSHADE_WINDOWS_
		//windows sleep is in millisecs so /1000
		Sleep(timeout.tv_usec/1000);
#else
		if(select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &timeout) < 0)
		{
			if(errno != EINTR)
			{
				ErrorLog << "Broken sockets while trying to sleep!" << endl;
			}

		}
#endif
		gettimeofday(&m_LastTime,(struct timezone *) 0);
	}
	else
	{
		m_lMissedPulses--;
		m_lMissedPulses=m_lMissedPulses<0 ? 0 : m_lMissedPulses;
		gettimeofday(&m_LastTime,(struct timezone *)0);
		//to next pulse
		//truncate the usec then add USEC to it
		//that should be next pulse
		m_LastTime.tv_usec = m_LastTime.tv_usec - m_LastTime.tv_usec%USEC;
	}
	m_nCurrentPulse++;

	if(m_nCurrentPulse>ROLL_OVER_PULSES)
	{
		m_nCurrentPulse = 0;
		m_lTotalPulses++;
	}
	//only update time 1 per second
	if(!(m_nCurrentPulse%PULSES_PER_REAL_SECOND))
	{
		UpdateMudTime(time(NULL));
	}
}

////////////////////////////////////////////
//	Gets the local time and the time the
//	mud has been up
CString CMudTime::GetTimeString()
{
	static const char* strTime= 
"The current time at the server is:  %.19s\r\n"
"Mud Time:  Year %d, %s of %s, %2d:%2d:%2d\r\n"
"%s\r\n"
"Up Time: Days: %d Hours: %d Minutes: %d Seconds: %d\r\n";
	
	CString str;
	time_t t = time(NULL);
	double dtUpTime = difftime(t,m_dtBootTime);
	int Days = (int)(dtUpTime/(60*60*24));
	dtUpTime = ((int)dtUpTime%(60*60*24));
	int Hours = (int)(dtUpTime/(60*60));
	dtUpTime = ((int)dtUpTime)%(60*60);
	int Min = (int)dtUpTime/60;
	str.Format(strTime,ctime(&t),
		m_CurrentTime.m_nYear,
		m_Days[m_CurrentTime.m_nDay],
		m_Months[m_CurrentTime.m_nMonth],
		m_CurrentTime.m_nHours,
		m_CurrentTime.m_nMinutes,
		m_CurrentTime.m_nSeconds,
		m_strYear[m_CurrentTime.m_nYear%PC_ADJUST_YEARS],
		Days,Hours,Min,((int)dtUpTime)%60);
	return str;
}

//////////////////////////
//	updates the mud time
void CMudTime::UpdateMudTime(time_t t)
{
	int RealSecondsInAMudYear = PULSES_PER_MUD_YEAR/PULSES_PER_SEC;
	m_CurrentTime.m_nYear = t/RealSecondsInAMudYear;
	t = t%RealSecondsInAMudYear;

	int RealSecondsInAMudMonth = PULSES_PER_MUD_MONTH/PULSES_PER_SEC;
	m_CurrentTime.m_nMonth = t/RealSecondsInAMudMonth;
	t = t%RealSecondsInAMudMonth;
	
	int RealSecondsInAMudDay = PULSES_PER_MUD_DAY/PULSES_PER_SEC;
	m_CurrentTime.m_nDay = t/RealSecondsInAMudDay;
	t = t%RealSecondsInAMudDay;

	int RealSecondsInAMudHour = PULSES_PER_MUD_HOUR/PULSES_PER_SEC;
	m_CurrentTime.m_nHours = t/RealSecondsInAMudHour;
	t = t%RealSecondsInAMudHour;

	int RealSecondsInAMudMin = PULSES_PER_MUD_MIN/PULSES_PER_SEC;
	m_CurrentTime.m_nMinutes = t/RealSecondsInAMudMin;
	t = t%RealSecondsInAMudMin;

	m_CurrentTime.m_nSeconds = t/PULSES_PER_SEC;
#ifdef _DEBUG
	assert(m_CurrentTime.m_nMonth<MUD_MONTHS_PER_YEAR);
	assert(m_CurrentTime.m_nDay<MUD_DAYS_PER_MONTH);
#endif
	ChangeSeason();
	if(m_CurrentTime.IsFirstDayOfYear())
	{
		CString str(m_strYear[m_CurrentTime.m_nYear%PC_ADJUST_YEARS]);
		str+="\r\n";
		GVM.SendToAll(str,NULL);
	}
}

/////////////////////////////////////////
//	Checks to see if we should change seasons
//	and sends messages of new seasons, and time of day!
void CMudTime::ChangeSeason()
{
	//we check by hours so if minutes don't = 0 then why check
	if(m_CurrentTime.m_nMinutes==0)
	{
		if(m_CurrentTime.IsFirstDayOfSeason(CMudTime::SEASON_FALL))
		{
			m_CurrentTime.m_CurrentSeason=CMudTime::SEASON_FALL;
			GVM.SendToAll(m_CurrentTime.m_CurrentSeason.m_strSeasonMsg,NULL);
		}
		else if(m_CurrentTime.IsFirstDayOfSeason(CMudTime::SEASON_WINTER))
		{
			m_CurrentTime.m_CurrentSeason=CMudTime::SEASON_WINTER;
			GVM.SendToAll(m_CurrentTime.m_CurrentSeason.m_strSeasonMsg,NULL);
		}
		else if(m_CurrentTime.IsFirstDayOfSeason(CMudTime::SEASON_SPRING))
		{
			m_CurrentTime.m_CurrentSeason=CMudTime::SEASON_SPRING;
			GVM.SendToAll(m_CurrentTime.m_CurrentSeason.m_strSeasonMsg,NULL);
		}
		else if(m_CurrentTime.IsFirstDayOfSeason(CMudTime::SEASON_SUMMER))
		{
			m_CurrentTime.m_CurrentSeason=CMudTime::SEASON_SUMMER;
			GVM.SendToAll(m_CurrentTime.m_CurrentSeason.m_strSeasonMsg,NULL);
		}
		else if(m_CurrentTime.IsFirstHourOfDay())
		{
			GVM.SendToOutdoors("The sun has begun to rise.\r\n");
		}
		else if(m_CurrentTime.IsLastHourOfDay())
		{
			GVM.SendToOutdoors("The sun begins to set.\r\n");
		}
	}
}

////////////////////////////////////
//	returns the PC to PC damange adjustment
//	based on the year
short CMudTime::GetPCYearDamageAdjustment()
{
	return m_YearPC_Dam_Adj[m_CurrentTime.m_nYear%PC_ADJUST_YEARS];
}

////////////////////////////////////
//	GetStatIfno
//	Pass back string of performance so far
//	this is not completely acurrate b/c
//	if no one is logged in they count as missing pulses
//	even though they are not
CString CMudTime::GetStatInfo()
{
	static const char *strStat = 
		"	Missed Pulses	=	%d\r\n"
		"	Total Pulses	=	%d\r\n"
		"	% Missed		=	%d\r\n";
	CString str;
	str.Format(strStat,
		m_lTotalMissed,
		((m_lTotalPulses*ROLL_OVER_PULSES)+m_nCurrentPulse+m_lTotalMissed),
		(m_lTotalMissed*100)/((m_lTotalPulses*ROLL_OVER_PULSES)+m_nCurrentPulse+m_lTotalMissed));
	return str;
}
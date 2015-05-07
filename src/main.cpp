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

#include "stdinclude.h"
#pragma hdrstop
#include "global.race.reference.h"

CRandom GenNum;
CGlobalRaceReferences cGRR;
CGlobalVariableManagement GVM;

main(int agrc, char *argv[])
{
	const char *strUsage = 
		"Wolfshade mud options: \r\n" 
		"-n name server true [default: false] turn on if your network has a name server\r\n"
		"-h this help menu\r\n"
		"-p set port number mud will run on [default: 6969]\r\n"
		"-z set zone index to load [default: zoneindex]\r\n"
		"-o set object index to load [default: objectindex]\r\n"
		"-r set realroom index to load [default: realroomindex]\r\n"
		"-m set mob index to load [default: mobindex]\r\n"
		"-v verion\r\n"
		"report bugs, comments, sugggestions, flames to: \r\n"
		"duth@wolfshade.com or kythis@wolfshade.com";
	try
	{
		CString strOpts;
		register int i;
		for(i=0;i<agrc;i++)
		{
			strOpts.Format("%s %s",
				(char *)strOpts,
				argv[i]);
		}
		if(strOpts.Find("-h")!=-1)
		{
			cout << strUsage << endl;
		}
		else if(strOpts.Find("-v")!=-1)
		{
#ifdef _WOLFSHADE_WINDOWS_
			cout << "1.64.x" << endl;
#else
			cout << VERSION << endl;
#endif
		}
		else
		{
			//CMud Time must be first
			//all timing for the mud is done in CMudTime 
			//so they are not initialized until CMudTime
			//is created.  //Therefore no timing stuff can be in
			//cGRR or GVM!
			CMudTime MudTime;
			CGame Wolfshade(strOpts);
		
			Wolfshade.Heartbeat(MudTime);
		}
	}
	catch(CError *Er)
	{
		ErrorLog << Er->GetMessage() << endl;
		CCharacterAttributes::CleanUp();
		return 1;
	}
	catch(...)
	{
		ErrorLog << "Unknown error caught in main()..." << endl;
		CCharacterAttributes::CleanUp();
		return 1;
	}
	CCharacterAttributes::CleanUp();
	return 0;
}

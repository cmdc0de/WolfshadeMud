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
//  class-less AI functions
//////////////////////////////////////////////////////////////
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "npc.h"

short CNPC::SWAMP_MOHR_STATUS = 0;
CString CNPC::MOHR_TO_ORC = "u\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ns\r\ns\r\ns\r\n";
CString CNPC::MOHR_TO_GNOLL = "u\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\nn\r\nn\r\nn\r\nn\r\nn\r\nn\r\nn\r\nn\r\nn\r\nn\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ne\r\ns\r\ns\r\ns\r\ns\r\ns\r\ns\r\ne\r\ne\r\nn\r\n";
////////////////////////////////////////////////////
//zone ai

////////////////////
//  AI For the Colesium Zone
//  Zone 130
//
//	John Comes 11/20/99
////////////////////

void CNPC::DoColiseumAI() {
   if (!(CMudTime::GetCurrentPulse() % COLISEUM_TIME)) {
	//see if it's the arena master
	if (this->GetVnum() == 13017) {
	   switch (DIE(3)) {
		case 1:
		   this->AddCommand("shout Let the games begin!");
		   break;
		case 2:
		   this->AddCommand("shout Cry 'Havoc' and release the gladiators!");
		   break;
		case 3:
		   this->AddCommand("shout Guards! Open the door and begin the battle!");
		   break;
	   }
	   this->AddCommand("close door n");
	} else if (this->GetVnum() >= 13001 && this->GetVnum() <= 13008) {
	   this->LagChar(CMudTime::PULSES_PER_SEC / 2);
	   switch (DIE(10)) {
		case 1:
		   this->AddCommand("shout Break his legs!");
		   break;
		case 2:
		   this->AddCommand("shout Come on ya pansy!");
		   break;
		case 3:
		   this->AddCommand("shout CRUSH");
		   break;
		case 4:
		   this->AddCommand("shout CRUSH 'EM");
		   break;
		case 5:
		   this->AddCommand("shout Come on, it's just a flesh wound!");
		   break;
		case 6:
		   this->AddCommand("shout Run him through!");
		   break;
		default:
		   this->AddCommand("applaud");
		   break;
	   }
	   this->AddCommand("cheer");
	}	   //nobles
	else if (this->GetVnum() == 13012 || this->GetVnum() == 13013) {
	   this->LagChar(CMudTime::PULSES_PER_SEC / 2);
	   this->AddCommand("cheer");
	}	   //noble people
	else if (this->GetVnum() > 13008 && this->GetVnum() < 13011) {
	   this->LagChar(CMudTime::PULSES_PER_SEC / 2);
	   this->AddCommand("clap");
	}	   //Nervous Pixie
	else if (this->GetVnum() == 13014) {
	   this->LagChar(CMudTime::PULSES_PER_SEC / 2);
	   this->AddCommand("emote wrings his hands in anticipation.");
	}	   //Have random gladiator walk into arena
	else if (this->GetVnum() == (13027 + DIE(5))) {
	   this->AddCommand("north");
	   this->LagChar(CMudTime::PULSES_PER_SEC / 2);
	   this->AddCommand("shout I SHALL TRIUMPH!");
	}
   }
   // mobs say things at random
   if (DIE(1000) <= 4) {
	//vendors shout at random
	if (this->GetVnum() == 13004 || this->GetVnum() == 13016) {
	   this->AddCommand("shout Get your snacks here!  We got peanuts, fried imp livers and pickled orc feet!");
	} else if (this->GetVnum() == 13015) {
	   this->AddCommand("say Oo I haven't had imp livers in forever!");
	}	   //ornery peasant shouts at random
	else if (this->GetVnum() == 13003) {
	   this->AddCommand("shout Blood makes the grass grow, Kill, Kill, Kill!!!");
	} else if (this->GetVnum() == 13028) {
	   this->AddCommand("say Have at you!");
	}
   }
}

void CNPC::DoColiseumFightingAI() {
   if (this->GetVnum() >= 13001 && this->GetVnum() <= 13008) {
	if (DIE(100) <= 3) {
	   this->AddCommand("shout Help! Help! I'm being repressed!");
	}
   }
}

void CNPC::DoWCMilitiaAI() {
   if (DIE(1000) <= 4) {
	switch (this->GetVnum()) {
	   case 13003:
		this->AddCommand("emote works on his backstab.");
		break;
	   case 13005:
		this->AddCommand("emote takes a shot at a target and hits it's mark.");
		this->AddCommand("smile");
		break;
	   case 13006:
		this->AddCommand("emote takes a shot at a target and misses wildly.");
		this->AddCommand("frown");
		break;
	   case 13007:
		this->AddCommand("emote works hard on his next invention, scribbling on a piece of paper.");
		break;
	   case 13009:
		switch (DIE(3)) {
		   case 1:
			this->AddCommand("emote sits and eats quietly.");
			break;
		   case 2:
			this->AddCommand("say Man, working out really builds up the appetite.");
			break;
		   case 3:
			this->AddCommand("say I liked Mom's cooking much better.");
			break;
		}
		break;
	   case 13002:
		this->AddCommand("emote sings... I wanna make love to you woman... I wanna lay you down by the fire..");
		break;
	   default:
		break;
	}
   }
}

void CNPC::DoSwampMohrAI() {
   //if swamp ai is on !=0
   //and vnum is that of a swamp mohrai
   if (SWAMP_MOHR_STATUS != 0
	   && this->GetVnum() >= 14028
	   && this->GetVnum() <= 14031) {
	if (GetVnum() == 14028 && GetRoom()->GetVnum() != 14003) {
	   SWAMP_MOHR_STATUS = 0;
	}
	if (SWAMP_MOHR_STATUS == 1 && this->GetVnum() >= 14028 && this->GetVnum() <= 14031) {
	   this->AddCommand(MOHR_TO_ORC);
	} else if (SWAMP_MOHR_STATUS == 2 && this->GetVnum() >= 14028 && this->GetVnum() <= 14031) {
	   this->TogPreference(SENTINEL);
	} else if (SWAMP_MOHR_STATUS == 3 && this->GetVnum() >= 14028 && this->GetVnum() <= 14031) {
	   this->AddCommand(MOHR_TO_GNOLL);
	}
   }
}

void CNPC::DoSwampMohrBattleAI() {
   if (this->GetVnum() >= 14028 && this->GetVnum() <= 14031) {
	switch (DIE(100)) {
	   case 0:
		this->AddCommand("shout Time to die!");
		break;
	   case 1:
		this->AddCommand("shout Aiyeee!\r\nshout Bonzai!");
		break;
	   case 2:
		this->AddCommand("shout Long live the Knights of Swamp Mohr!");
		break;
	   case 3:
		this->AddCommand("shout Death to you all!");
		break;
	   default:
		break;
	}
   }

}

void CNPC::DoKipuAI() {
   if (DIE(1000) <= 4) {
	switch (this->GetVnum()) {
	   case 200002:
		this->AddCommand("say Why?  Why?  Why did my brother do it?");
		this->AddCommand("cry");
		break;
	   case 200004:
		this->AddCommand("sniff");
		this->AddCommand("say That kid is ripping this family apart.. dabbling in that magic.");
		break;
	   case 200006:
		this->AddCommand("I wouldn't be around here if I were you, that kid is summoning up some creepy stuff.");
		this->AddCommand("cringe");
		break;
	   case 200005:
		this->AddCommand("shout Dramashir, aou madris.\r\nshout Come from the likes of the netherworld creature of my bidding!");
		this->AddCommand("cackle");
		break;
	   default:
		break;
	}
   }
}
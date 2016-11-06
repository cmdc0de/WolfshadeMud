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
 *  CCharIntermediate
 *
 *
 *
 */
#include "random.h"
extern CRandom GenNum;

#include "stdinclude.h"
#include "char.intermediate.h"
#include "random.h"
#include "socket.h"
#ifndef _WOLFSHADE_WINDOWS_
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#endif
#include "affect.h"

extern CGlobalVariableManagement GVM;
extern CGlobalRaceReferences cGRR;
extern CRandom GenNum;

#define NOT_ALLOWED_NAMES "files/namesnotallowed.txt"
#define NUM_OF_BONUS 3
#define TITLESCREEN "files/titlescreen.txt"
#define CTITLESCREEN "files/ansititlescreen.txt"

CMap<CString, bool> CCharIntermediate::NamesNotAllowed;
bool CCharIntermediate::m_bInited = false;
CString CCharIntermediate::m_strRaceList;
const int CCharIntermediate::VOID_TIME = (CMudTime::PULSES_PER_REAL_MIN * 3);
CString CCharIntermediate::TitleScreen;
CString CCharIntermediate::AnsiTitleScreen;

const char *CCharIntermediate::m_strBonus =
        "\r\n"
        "a) Strength     \t b) Agility\r\n"
        "c) Luck         \t d) Intelligence\r\n"
        "e) Wisdom       \t f) Charisma\r\n"
        "g) Constitution \t h) Power:\r\n"
        "i) Dexterity\r\n"
        "Letter: ";

//menu
const char * CCharIntermediate::m_strMenu =
        "\r\n\r\n"
        " &B____________________________________________\r\n"
        "(	&R0.&L  Leave Game&B                       )\r\n"
        "(	&R1.&L  Enter Game&B                       )\r\n"
        "(	&R2.&L  Change Password&B                  )\r\n"
        "(	&R3.&L  Enter Character Description.&B     )\r\n"
        "(	&R4.&L  Delete a Character.&B              )\r\n"
        " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~&n\r\n";

//init our array of function pointers
// I would have like to make this a member variable
// of CCharIntermediate but can't seem to get the syntax right.
// anyone know how?
void (CCharIntermediate::*StateFnc[])() = {
    &CCharIntermediate::StateTitle,
    &CCharIntermediate::StateGetName,
    &CCharIntermediate::StateConfirmName,
    &CCharIntermediate::StateGetPassWord,
    &CCharIntermediate::StateConfirmPassword,
    &CCharIntermediate::StatePickRace,
    &CCharIntermediate::StatePickClass,
    &CCharIntermediate::StatePickSex,
    &CCharIntermediate::StateRollChar,
    &CCharIntermediate::StateBonus,
    &CCharIntermediate::StateMOTD,
    &CCharIntermediate::StateMenu,
    &CCharIntermediate::StateGetSavedPassword,
    &CCharIntermediate::StateKeepChar,
    &CCharIntermediate::ConfirmDeletion,
    &CCharIntermediate::StateChangePassword,
    &CCharIntermediate::StateConfirmPasswordChange

};

const bool CCharIntermediate::Matrix[TOTAL_RACES][TOTAL_CLASSES] = {
    /*NO RACE*/
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_TROLL*/
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_OGRE*/
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_ORC*/
    {0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0},
    /*RACE_GNOLL*/
    {0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0},
    /*RACE_DUERGAR_DWARF*/
    {0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    /*RACE_DROW_ELF*/
    {0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0},
    /*RACE_HALF_DROW_ELF*/
    {0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0},
    /*RACE_GITHYANKI*/
    {0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0},
    /*RACE_DRACONIAN*/
    {0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0},
    /*RACE_IMP*/
    {0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_GNOME*/
    {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    /*RACE_GREY_ELF*/
    {0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0},
    /*RACE_KENDER*/
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_MOUNTAN_DWARF*/
    {0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    /*RACE_BROWNIE*/
    {0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0},
    /*RACE_PIXIE*/
    {0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    /*RACE_CENTAUR*/
    {0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_SAURIAL*/
    {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
    /*RACE_HALF_GREY_ELF*/
    {0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0},
    /*RACE_HUMAN*/
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    /*RACE_THRIKREEN*/
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_MINOTAUR*/
    {0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_HALF_ORC*/
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    /*RACE_BARBARIAN*/
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_KENKU*/
    {0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_MUL*/
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*RACE_ILLITHID*/
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};

/////////////////////////////
// non statics
//create a character intermediate from a description

CCharIntermediate::CCharIntermediate(sPlayerDescriptor *desc)
: CCharacterAttributes() {
    InitStatics();
    m_nBonus = 0;
    m_pDesc = desc;
    m_nTimeSinceLastCmd = 0;
    ChangeState(STATE_TITLE_QUESTION);
}

//////////////////////////////////////////////////////////////////
//Makes a character intermediate from a charintermediate... used to move
//characters to and from q to game
//Only a newly rolled character will get moved to the Que via this
//constructor so we will use it to set up a new character.  Because
//by this time we have the all the data we need to set up a start character
// written 4/6/98
/////////////////////////////////////////////////////////////////

CCharIntermediate::CCharIntermediate(CCharIntermediate *ch)
: CCharacterAttributes(ch) {
    InitStatics();
    m_nBonus = 0;
    m_nTimeSinceLastCmd = 0;
    m_pDesc = ch->m_pDesc;
}

//checks for a command in the command buffer and moves it to the current
//command buffer if need be

bool CCharIntermediate::HasCommand() {
    int returnat;
    if (m_pDesc->IsPlaying()) {
        return false;
    } else if ((returnat = m_pDesc->CommandBuf.Find('\r', '\n')) == -1) {
        m_nTimeSinceLastCmd++;
        if (m_nTimeSinceLastCmd >= VOID_TIME) {
            SendToChar("bye...\r\n");
            ChangeState(STATE_LINK_DEAD);
        }
        return false;
    } else {
        m_nTimeSinceLastCmd = 0;
        if (m_pDesc->IsWritingDescription()) {
            int nNum;
            if (m_pDesc->CommandBuf.GetLength() >= MAX_LONG_STR) {
                SendToChar("Description too long...it will be truncated.\r\n");
                m_strLongDescription = m_pDesc->CommandBuf.Left(MAX_LONG_STR);
                m_pDesc->CommandBuf = m_pDesc->CommandBuf.Right(MAX_LONG_STR);
                SendToChar(m_strMenu);
                ChangeState(STATE_MENU);
            } else if ((nNum = m_pDesc->CommandBuf.Count('^')) > 0) {
                int pos = m_pDesc->CommandBuf.Find('^', nNum);
                if (m_pDesc->CommandBuf[pos + 1] == 'D') {
                    m_strLongDescription = m_pDesc->CommandBuf.Left(returnat);
                    m_pDesc->CommandBuf = m_pDesc->CommandBuf.Right(returnat);
                    SendToChar(m_strMenu);
                    ChangeState(STATE_MENU);
                }
            }
            return false;
        } else {
            CurrentCommand = m_pDesc->CommandBuf.Left(returnat);
            CurrentCommand.SkipSpaces();
            m_pDesc->CommandBuf = m_pDesc->CommandBuf.Right(returnat + 1);
            return true;
        }
    }
}

//hands the player when he/she is at the title screen

void CCharIntermediate::StateTitle() {
    switch (CurrentCommand[0]) {
        case 'n': case 'N':
            m_nPreference &= ~PREFERS_COLOR;
            SendToChar(TitleScreen.cptr());
            break;
        default:
            m_nPreference |= PREFERS_COLOR;
            SendToChar(AnsiTitleScreen.cptr());
            break;
    }
    SendToChar((const char *) "\r\nWhat is Thy Name?");
    ChangeState(STATE_GET_NAME);
}

void CCharIntermediate::SendToChar(const CString &strSend) const {
    CString s(strSend);
    s.Colorize(Prefers(PREFERS_COLOR));
    SendToSocket(s);
}

void CCharIntermediate::SendToChar(const char *strSend) {
    CString s(strSend);
    SendToChar(s);
}

//executes command

void CCharIntermediate::ExecuteState() {
    (this->*StateFnc[m_pDesc->GetState()])();
}

//hands the player when he/she is at get Name prompt

void CCharIntermediate::StateGetName() {
    static const char *strNamesSpeach = "Insure your name is a fantasy, roll playing type name.\r\nIf it isn't please say no and try again.\r\n";
    CString Info;
    m_strName = CurrentCommand;
    if (IsLegalName()) {
        //is player saved?
        if (IsPlayerSaved()) {
            SendToChar("\r\nPassword: ");
            ChangeState(STATE_GET_SAVED_PASSWORD);
            return;
        }
        Info.Format("\r\n%sAre you sure you want to be known as %s? ",
                strNamesSpeach,
                m_strName.cptr());
        SendToChar(Info);
        ChangeState(STATE_CONFIRM_NEW_PLAYER_NAME);
    }
}

//hands the player when he/she is at confirm name prompt for new characters

void CCharIntermediate::StateConfirmName() {
    switch (CurrentCommand[0]) {
        case 'n': case 'N':
            SendToChar("\r\nWhat is it then?\r\n");
            ChangeState(STATE_GET_NAME);
            break;
        case 'y': case 'Y':
            SendToChar("\r\nPassword: ");
            ChangeState(STATE_GET_PASSWORD);
            break;
        default:
            SendToChar("\r\ny or n please! \r\n");
            break;
    }
}

//hands the player when he/she is at get pass word prompt

void CCharIntermediate::StateGetPassWord() {
    m_strPassword = CurrentCommand;
    SendToChar("\r\nRetype password for confirmation: ");
    ChangeState(STATE_CONFIRM_NEW_PASSWORD);
}

//hands the player when he/she is at confirm pass word for new characters

void CCharIntermediate::StateConfirmPassword() {
    if (CurrentCommand == m_strPassword) {
        SendToChar("\r\nEntering new character generation\r\n");
        SendToChar(m_strRaceList);
        ChangeState(STATE_PICK_RACE);
    } else {
        SendToChar("\nPasswords don't match... bye!");
        ChangeState(STATE_LINK_DEAD);
        m_pDesc->CommandBuf = "\r\n";
    }
}

//hands the player when he/she is at race class prompt

void CCharIntermediate::StatePickRace() {
    CString Info;
    short int Num = CurrentCommand.MakeInt();
    if (Num <= 0 || Num >= TOTAL_PLAYER_RACES) {
        SendToChar("That's not a choice!\r\n");
        SendToChar(m_strRaceList);
        return;
    }
    m_pAttributes->SetRace(Num);
    Info.Format("You will be a %s.\r\n\r\n",
            GVM.GetColorRaceName(m_pAttributes->GetRace()));
    m_lPlayerSetHomeTown = m_lLastRentRoom = m_pAttributes->GetHomeTown();
    SendToChar(Info);
    SendClassList(Num);
    ChangeState(STATE_PICK_CLASS);
}

//hands the player when he/she is at race class prompt

void CCharIntermediate::StatePickClass() {
    CString Info;
    short int Num = CurrentCommand.MakeInt();
    if (Num == -1 || Num >= TOTAL_CLASSES || (!Matrix[GetRace()][Num])) {
        SendToChar("That's not a choice!\r\n");
        SendClassList(GetRace());
        return;
    }
    m_pAttributes->SetClass(Num);
    Info.Format("You will be a %s.\r\n\r\n",
            GVM.GetClassName(m_pAttributes->GetClass()));
    SendToChar(Info);
    if (m_pAttributes->GetRace() == RACE_ILLITHID) {
        m_pAttributes->SetSex(NONE);
        m_pAttributes->RollNewStats(Info);
        SendToChar(Info);
        ChangeState(STATE_ROLL_CHAR);
    } else {
        SendToChar("\nAre you male or female (m/f)");
        ChangeState(STATE_PICK_SEX);
    }
}

//hands the player when he/she is at pick sex prompt

void CCharIntermediate::StatePickSex() {
    CString Info;
    switch (CurrentCommand[0]) {
        case 'm':case 'M':
            m_pAttributes->SetSex(MALE);
            m_pAttributes->RollNewStats(Info);
            SendToChar(Info);
            ChangeState(STATE_ROLL_CHAR);
            break;
        case 'f':case 'F':
            m_pAttributes->SetSex(FEMALE);
            m_pAttributes->RollNewStats(Info);
            SendToChar(Info);
            ChangeState(STATE_ROLL_CHAR);
            break;
        default:
            SendToChar("m or f please!\r\n");
            break;
    }
}

//hands the player when he/she is at roll character prompt

void CCharIntermediate::StateRollChar() {
    CString Info;
    switch (CurrentCommand[0]) {
        case 'n': case 'N':
            ChangeState(STATE_BONUS);
            m_nAlignment = m_pAttributes->GetStartingAlignment();
            m_nMovePts = m_nMaxMovePts = m_pAttributes->GetStartingMoves();
            m_nManaPts = m_nMaxManaPts = m_pAttributes->GetStartingMana();
            SendToChar(m_strBonus);
            break;
        default:
            m_pAttributes->RollNewStats(Info);
            SendToChar(Info);
            break;
    }
}

//hands the player when he/she see motd

void CCharIntermediate::StateMOTD() {
    ChangeState(STATE_MENU);
    SendToChar(m_strMenu);
}

//hands the player when he/she is at menu

void CCharIntermediate::StateMenu() {
    CString strToChar;
    switch (CurrentCommand.MakeInt()) {
        case 0:
            SendToChar("Bye\r\n");
            ChangeState(STATE_LINK_DEAD);
            break;
        case 1:
            ChangeState(STATE_PLAYING);
            break;
        case 2:
            SendToChar("Please enter your new password: ");
            ChangeState(STATE_CHANGE_PASSWORD);
            break;
        case 3:
            strToChar.Format("Your current description is:\r\n%s\r\nEnter description type ^D (carot shift d) when done.\r\n",
                    m_strLongDescription.cptr());
            SendToChar(strToChar);
            ChangeState(STATE_ENTER_DESCRIPTION);
            break;
        case 4:
            SendToChar("\r\nAre you sure?\r\n");
            ChangeState(STATE_CONFIRM_DELETE);
            break;
        default:
            SendToChar(m_strMenu);
            break;
    }
}

void CCharIntermediate::ConfirmDeletion() {
    if (CurrentCommand.Compare("yes")) {
        DeleteChar();
        SendToChar("Your character has been deleted.\r\n");
        ChangeState(STATE_LINK_DEAD);
    } else {
        SendToChar("You must type 'yes' to delete your character.\r\n");
        ChangeState(STATE_MENU);
    }
}

CCharIntermediate::~CCharIntermediate() {

}

/////////////////////////////////////
//	Handles if we find a character saved
//	or currently in the game and link dead
//	written by: Demetrius Comes
//	6/5/98
/////////////////////////////////////

void CCharIntermediate::StateGetSavedPassword() {
    CCharacter *pCh;
    //are they currently playing
    if ((pCh = GVM.FindCharacter(m_strName, NULL))
            && !pCh->IsNPC()) {
        if (CurrentCommand == pCh->GetPassword()) {
            if (pCh->IsLinkDead()) {
                pCh->SetDescriptor(m_pDesc);
                pCh->SendToChar("You take over your body again! =)\r\n");
                ChangeState(STATE_KILL_INTERMEDIATE);
            } else {
                SendToChar("That character is already playing.\r\n");
                ChangeState(STATE_LINK_DEAD);
            }
        } else {
            SendToChar("Passwords don't match!\r\n");
            ChangeState(STATE_LINK_DEAD);
        }
    } else {
        //do passwords match saved?
        if (CurrentCommand == m_strPassword) {
            SendToChar(MOTD);
            SendToChar("\r\nReturn");
            ChangeState(STATE_MOTD);
        } else {
            SendToChar("Passwords don't match!\r\n");
            ChangeState(STATE_LINK_DEAD);
        }
    }
}

////////////////////////////////////
//	Overloaded CCharIntermediate
//	used if character rents, camps, or
//  dies. 
///////////////////////////////////

CCharIntermediate::CCharIntermediate(CCharacter * pCh)
: CCharacterAttributes(pCh) {
    InitStatics();
    m_nBonus = 0;
    m_nTimeSinceLastCmd = 0;
    m_pDesc = pCh->GetDescriptor();
}

////////////////////////////
//	Reads in from a file all the
//	names that are not allowed.
//	written by: Demetrius Comes
//	8-31-98
////////////////////////////

void CCharIntermediate::ReadNotAllowedNames() {
    std::ifstream NameFile(NOT_ALLOWED_NAMES, std::ios_base::in);
    CString strName;
    bool bDummy;
    while (NameFile.good()) {
        NameFile >> strName;
        if (!NamesNotAllowed.Lookup(strName, bDummy)) {
            NamesNotAllowed.Add(strName, true);
        } else {
            ErrorLog << "Duplicate names in the names not allowed file: "
                    << strName << endl;
        }
    }
}

///////////////////////////
//	Allows the player todo bonus
//	to attributes
//////////////////////////
#define ATT_RANGE(a,b) (((m_pAttributes->BaseStats[a] + b) <= 100) ? (m_pAttributes->BaseStats[a] + b) : 100)

void CCharIntermediate::StateBonus() {
    short nBonus = DIE(7) + 3;
    switch (tolower(CurrentCommand[0])) {
        case 'a':
            //check for max!
            m_pAttributes->AffectedStats[STR] =
                    m_pAttributes->BaseStats[STR] = ATT_RANGE(STR, nBonus);
            break;
        case 'b':
            m_pAttributes->AffectedStats[AGI] =
                    m_pAttributes->BaseStats[AGI] = ATT_RANGE(AGI, nBonus);
            break;
        case 'c':
            m_pAttributes->AffectedStats[LUCK] =
                    m_pAttributes->BaseStats[LUCK] = ATT_RANGE(LUCK, nBonus);
            break;
        case 'd':
            m_pAttributes->AffectedStats[INT] =
                    m_pAttributes->BaseStats[INT] = ATT_RANGE(INT, nBonus);
            break;
        case 'e':
            m_pAttributes->AffectedStats[WIS] =
                    m_pAttributes->BaseStats[WIS] = ATT_RANGE(WIS, nBonus);
            break;
        case 'f':
            m_pAttributes->AffectedStats[CHA] =
                    m_pAttributes->BaseStats[CHA] = ATT_RANGE(CHA, nBonus);
            break;
        case 'g':
            m_pAttributes->AffectedStats[CON] =
                    m_pAttributes->BaseStats[CON] = ATT_RANGE(CON, nBonus);
            break;
        case 'h':
            m_pAttributes->AffectedStats[POW] =
                    m_pAttributes->BaseStats[POW] = ATT_RANGE(POW, nBonus);
            break;
        case 'i':
            m_pAttributes->AffectedStats[DEX] =
                    m_pAttributes->BaseStats[DEX] = ATT_RANGE(DEX, nBonus);
            break;
        default:
            SendToChar("\r\nThat is not an attribute.\r\n");
            SendToChar(m_strBonus);
            return;
            break;
    }
    CString strToChar;
    m_pAttributes->GetStatMessage(strToChar);
    if (++m_nBonus >= NUM_OF_BONUS) {
        strToChar.Format("\r\nYour final stats are:\r\n%s\r\n\r\nDo you wish to use this character? 'y' ",
                strToChar.cptr());
        SendToChar(strToChar);
        ChangeState(STATE_KEEP_CHAR);
        m_nBonus = 0;
    } else {
        strToChar.Format("\r\nYour stats are now\r\n%s\r\nBonus:\r\n %s",
                strToChar.cptr(), m_strBonus);
        SendToChar(strToChar);
    }
}

void CCharIntermediate::StateKeepChar() {
    switch (tolower(CurrentCommand[0])) {
        case 'n':
            SendToChar("\r\nResetting character...\r\n");
            SendToChar(m_strRaceList);
            ChangeState(STATE_PICK_RACE);
            break;
        default:
            SendToChar("Yeap, it's a keeper! \r\n");
            ChangeState(STATE_MOTD);
            SendToChar(MOTD);
            SendToChar("\r\n PRESS ENTER");
            break;
    }
}

//////////////////////////
//	checks to insure name
//  is legal
///////////////////////

bool CCharIntermediate::IsLegalName() {
    bool bIsLegal = true;
    bool bDummy;
    CString strToChar;
    for (int i = 0; i < m_strName.GetLength(); i++) {
        if (m_strName[i] < 65 || m_strName[i] > 122
                || (m_strName[i] > 90 && m_strName[i] < 97)) {
            strToChar = "\r\nNo funky characters in a name.\r\n";
            bIsLegal = false;
        }
        if (!i) {
            m_strName[i] = toupper(m_strName[i]);
        } else {
            m_strName[i] = tolower(m_strName[i]);
        }
    }
    if (m_strName.GetLength() <= 2) {
        strToChar = "\r\nYou name is not long enough.\r\n";
        bIsLegal = false;
    } else if (m_strName.GetLength() > MAX_NAME_SIZE) {
        strToChar = "\r\nYou name is too long!\r\n";
        bIsLegal = false;
    } else if (NamesNotAllowed.Lookup(m_strName, bDummy)
            || CMobManager::IsMobName(m_strName)) {
        strToChar = "\r\nThat name has been disallowed.\r\n";
        bIsLegal = false;
    } else if (!(m_strName.GetWordAfter(1).IsEmpty())) {
        strToChar = "\r\nA name can only be ONE word!\r\b";
        bIsLegal = false;
    }
    if (!bIsLegal) {
        strToChar += "\r\nWhat is Thy Name?";
    }
    SendToChar(strToChar);
    return bIsLegal;
}

void CCharIntermediate::InitStatics() {
    if (m_bInited) {
        return;
    }
    m_bInited = true;
    ReadNotAllowedNames();
    BootFileStrings();
    MakeRaceList();
}

void CCharIntermediate::MakeRaceList() {
    CString strEvils("&LEvils:&n");
    CString strGoods("&WGoodies: &n");
    CString strAlien("&wAlien: &n");
    CString strNeutral("&wNeutrals: &n");
    for (int i = 1; i < TOTAL_PLAYER_RACES; i++) {
        if (cGRR.GetRaceReference(i)->IsRace(CRacialReference::RACE_EVIL)) {
            strEvils.Format("%s\r\n%3d %-20s", strEvils.cptr(), i, GVM.GetColorRaceName(i));
        } else if (cGRR.GetRaceReference(i)->IsRace(CRacialReference::RACE_GOODIES)) {
            strGoods.Format("%s\r\n%3d %-20s", strGoods.cptr(), i, GVM.GetColorRaceName(i));
        } else if (cGRR.GetRaceReference(i)->IsRace(CRacialReference::RACE_NEUTRAL)) {
            strNeutral.Format("%s\r\n%3d %-20s", strNeutral.cptr(), i, GVM.GetColorRaceName(i));
        } else if (cGRR.GetRaceReference(i)->IsRace(CRacialReference::RACE_ALIEN)) {
            strAlien.Format("%s\r\n%3d %-20s", strAlien.cptr(), i, GVM.GetColorRaceName(i));
        }
    }
    m_strRaceList.Format("%s\r\n%s\r\n%s\r\n%s\r\n",
            strEvils.cptr(), strGoods.cptr(),
            strNeutral.cptr(), strAlien.cptr());
}

void CCharIntermediate::SendClassList(short nRace) {
    CString strClasses = "&WClasses:&n\r\n";
    CString strTemp;
    for (int i = 1; i < TOTAL_CLASSES; i++) {
        if (Matrix[nRace][i]) {
            strTemp.Format("%3d %-20s\r\n", i, GVM.GetColorClass(i));
            strClasses += strTemp;
        }
    }
    SendToChar(strClasses);
}

///////////////////////////////////////////////
//BootFileStrings
//Boots MOTD and title screens
//////////////////////////////////////////////

void CCharIntermediate::BootFileStrings() {
    CAscii Title(CTITLESCREEN, std::ios_base::in);
    if (Title) {
        Title.ReadFileString(AnsiTitleScreen, EOF);
        Title.close();
    } else {
        ErrorLog << "No Ansi Titlescreen loaded!" << endl;
    }
    CAscii Title2(TITLESCREEN, std::ios_base::in);
    if (Title2) {
        Title2.ReadFileString(TitleScreen, EOF);
        Title2.close();
    } else {
        ErrorLog << "No Titlescreen loaded! (Non-Ansi)" << endl;
    }
}

//////////////////////////////
//	Actually calls the write function

void CCharIntermediate::SendToSocket(const CString &str) const {
    if (send(m_pDesc->m_ConnectedSocket, str.cptr(), str.GetLength(), 0) < 0) {
#ifdef _WOLFSHADE_WINDOWS_
        if (WSAGetLastError() != WSAEWOULDBLOCK)
#else
#ifdef EWOULDBLOCK
        if (errno == EWOULDBLOCK)
            errno = EAGAIN;
#endif /* EWOULDBLOCK */
        if (errno != EAGAIN)
#endif //woflshade_windows
        {
            ErrorLog << "Socket " << m_pDesc->m_ConnectedSocket << " bad write for player " << GetName() << endl;
            m_pDesc->ChangeState(STATE_LINK_DEAD);
        }
    }
}

/////////////////////////////////
//	Allows player to change password

void CCharIntermediate::StateChangePassword() {
    m_strPassword = CurrentCommand;
    SendToChar("\r\nRetype password to confirm: ");
    ChangeState(STATE_CHANGE_PASSWORD_CONFIRM);
}

/////////////////////////////////
//	Checks to see if they have typed the same password twice

void CCharIntermediate::StateConfirmPasswordChange() {
    if (m_strPassword == CurrentCommand) {
        SendToChar("\r\nYour password has been changed.\r\n");
        if (!SaveToFile(false)) {
            SendToChar("You must enter the game at least once before chaning your password.\r\n");
        }
    } else {
        SendToChar("The passwords do not match.\r\nYOU PASSWORD HAS NOT BEEN CHANGED!\r\n");
        if (!IsPlayerSaved()) {
            ErrorLog << "CRITICAL ERROR: " << m_strName << " was not in saved player list through they tried to change password.\r\n";
            SendToChar("Error with your player file, please notify a god.\r\n");
            ChangeState(STATE_LINK_DEAD);
            return;
        }
    }
    SendToChar(this->m_strMenu);
    ChangeState(STATE_MENU);
}


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
/**********************************************
 *
 *  CObjectPrototype Implementation
 *
 *   DAC 8/4/97
 *
 ************************************************/
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include <sstream>
#include "object.prototype.h"
#include "log.h"
#include "portal.h"

const char *CObjectPrototype::MaterialNames[TOTAL_MATERIALS] ={
   "*Reserved*",
   "Wood",
   "Steel",
   "Spanish Steel",
   "Amantite",
   "Leather",
   "Gold",
   "Nickle"
};

const char *CObjectPrototype::TypeNames[] ={
   "NONE",
   "FOOD",
   "LIGHT",
   "SCROLL",
   "WAND",
   "STAFF",
   "ARMOR",
   "DRINKCONTAINER",
   "CONTAINER",
   "FOUNTAIN",
   "WEAPON",
   "POTION",
   "PORTAL",
   "reserved",
   "TRANSPORT"
};

const char *CObjectPrototype::AffectNames[] ={
   "NONE",
   "STRADD",
   "STR",
   "AGI",
   "DEX",
   "INT",
   "WIS",
   "CON",
   "CHA",
   "POW",
   "LUCK",
   "CLASS",
   "LEVEL",
   "AGE",
   "WIEGHT",
   "HEIGHT",
   "MANA",
   "HITPOINTS",
   "MOVE",
   "EXP",
   "AC",
   "HITROLL",
   "DAMROLL",
   "SAVE_PARA",
   "SAVE_ROD",
   "SAVE_BREATHE",
   "SAVE_SPELL"
};

const char *CObjectPrototype::GetTypeName(short n) {
   return n >= TOTAL_OBJECT_TYPES ? "illegal type" : TypeNames[n];
}

const char *CObjectPrototype::GetMaterialName(short n) {
   return n >= TOTAL_MATERIALS ? "illegal material" : MaterialNames[n];
}

const char *CObjectPrototype::GetAffectName(short n) {
   return n >= TOTAL_AFFECT_TYPES ? "illegal affect type" : AffectNames[n];
}

/////////////////////////
//	ReadObj
//	Reads in an object from an 
//	ascii file.
////////////////////////

void CObjectPrototype::ReadObj(CAscii &Infile) {
   CString temp(MAX_FILE_STRING_SIZE);
   char flag1[33] = {'\0'};
   char flag2[33] = {'\0'};
   int counter = 0;
   CString ENDFILE("ENDFILE");

   Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
   if (temp == ENDFILE) {
	if (Infile.rdstate() != std::ios_base::eofbit) {
	   Infile.clear(std::ios_base::eofbit);
	}
	CError Err(NONERROR, "End File Marker");
	throw &Err;
   }
   std::string s(temp.cptr(),MAX_FILE_STRING_SIZE);
   std::istringstream one(s);
   one >> Vnum;
   if (!one.good()) {
	CError Err(NON_CRITICAL_ERROR, "Object has no vnum.\r\n");
	throw &Err;
   }
   Infile.ReadFileString(ObjAlias, '~');
   if (ObjAlias.IsEmpty()) {
	ErrorLog << "Object #: " << Vnum << " has no Alias." << endl;
   } else if (ObjAlias.GetLength() > MAX_OBJ_SHORT_STR) {
	ErrorLog << "Object #: " << Vnum << " alias is too long name will be truncated if saved." << endl;
   }
   Infile.ReadFileString(ObjName, '~');
   if (ObjName.GetLength() > MAX_OBJ_SHORT_STR) {
	ErrorLog << "Object #: " << Vnum << " Object Name is too long it will be truncated when saved." << endl;
   }
   Infile.ReadFileString(ObjDesc, '~');
   if (ObjDesc.GetLength() > MAX_OBJ_LONG_STR) {
	ErrorLog << "Object #: " << Vnum << "Object description too long it will be truncated when saved." << endl;
   }
   Infile.ReadFileString(ObjSittingDesc, '~');
   if (ObjSittingDesc.GetLength() > MAX_OBJ_LONG_STR) {
	ErrorLog << "Object #: " << Vnum << "object sitting description too long it will be truncated when saved." << endl;
   }
   Infile.Numeric("%b %s %s", &ObjType, flag1, flag2);
   AffBit = Infile.AsciiConvert(flag1);
   WearBit = Infile.AsciiConvert(flag2);
   switch (ObjType) {
	case ITEM_TYPE_ARMOR:
	   Infile.Numeric("%d", &Val0);
	   break;
	case ITEM_TYPE_PORTAL: //portal time loaded in minutes we need it in pulses
	   Infile.Numeric("%d %d", &Val0, &Val1);
	   if (Val1 != -1) //-1 is forever
	   {
		Val1 = CMudTime::PULSES_PER_REAL_MIN * Val1;
	   }
	   break;
	case ITEM_TYPE_LIGHT:
	   Infile.Numeric("%d %d", &Val0, &Val1);
	   break;
	case ITEM_TYPE_SCROLL:case ITEM_TYPE_STAFF:case ITEM_TYPE_POTION:
	case ITEM_TYPE_WEAPON: case ITEM_TYPE_TRANSPORT:
	   Infile.Numeric("%d %d %d %d %d", &Val0, &Val1, &Val2, &Val3, &Val4);
	   break;
	case ITEM_TYPE_CONTAINER:
	case ITEM_TYPE_FOUNTAIN:
	   Infile.Numeric("%d %s %d", &Val0, flag1, &Val2);
	   Val1 = Infile.AsciiConvert(flag1);
	   break;
	default:
	   ErrorLog << "Illegal item type is obj # : " << Vnum << endl;
	   break;
   }
   Infile.Numeric("%d %d %b", &Weight, &Cost, &Material);
   Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
   sObjectWearAffects *NewAffect;
   static CString ENDOBJ("ENDOBJ");
   static CString EXTRA("EXTRA");
   static CString AFFECTS("AFFECTS");
   
   while (temp != ENDOBJ && Infile.good()) {
	if (temp == EXTRA) {
	   CObjectDesc *ExtraDesc;
	   ObjExtraDesc.Add((ExtraDesc = new CObjectDesc));
	   Infile.ReadFileString(ExtraDesc->KeyWord, '~');
	   Infile.ReadFileString(ExtraDesc->ExtraDesc, '~');
	   Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
	} else if (temp == AFFECTS) {
	   Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
	   while (temp != ENDOBJ) {
		CString strNum;
		ObjAffects.Add((NewAffect = new sObjectWearAffects()));
		strNum = temp.GetWordAfter(0);
		NewAffect->Location = (short) strNum.MakeInt();
		strNum = temp.GetWordAfter(1);
		NewAffect->Affect = (short) strNum.MakeInt();
		Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
	   }
	} else {
	   Infile.getline(temp.ptr(), MAX_FILE_STRING_SIZE);
	}
   }
}

CObjectPrototype::CObjectPrototype() {
   ObjAffects.SetDeleteData();
   ObjExtraDesc.SetDeleteData();
   Val0 = -1;
   Val1 = -1;
   Val2 = -1;
   Val3 = -1;
   Val4 = -1;
}

CObjectPrototype::CObjectPrototype(CObjectPrototype *pObj) {
   ObjAffects.SetDeleteData();
   ObjExtraDesc.SetDeleteData();
   AffBit = pObj->AffBit;
   Cost = pObj->Cost;
   Material = pObj->Material;
   NumberInGame = 0;
   ObjAlias = pObj->ObjAlias;
   ObjDesc = pObj->ObjDesc;
   ObjName = pObj->ObjName;
   ObjSittingDesc = pObj->ObjSittingDesc;
   ObjType = pObj->ObjType;
   Val0 = pObj->Val0;
   Val1 = pObj->Val1;
   Val2 = pObj->Val2;
   Val3 = pObj->Val3;
   Val4 = pObj->Val4;
   Vnum = pObj->Vnum;
   WearBit = pObj->WearBit;
   Weight = pObj->Weight;
   POSITION pos = pObj->ObjAffects.GetStartPosition();
   while (pos) {
	ObjAffects.Add(new sObjectWearAffects(pObj->ObjAffects.GetNext(pos)));
   }
   pos = pObj->ObjExtraDesc.GetStartPosition();
   while (pos) {
	ObjExtraDesc.Add(new CObjectDesc(pObj->ObjExtraDesc.GetNext(pos)));
   }
}

CObjectPrototype::CObjectPrototype(short nAffect, CRoom *pToRoom, int nPulses) {
   ObjAffects.SetDeleteData();
   ObjExtraDesc.SetDeleteData();
   AffBit = CObject::ITEM_NOT_TAKEABLE;
   Cost = 0;
   Material = 0;
   NumberInGame = 0;
   if (nAffect != CObject::OBJ_AFFECT_DELAYED_BLAST_FIREBALL) {
	Val0 = pToRoom->GetVnum();
	Val1 = nPulses;
	if (nAffect == CPortal::WORMHOLE) {
	   ObjAlias = "portal wormhole worm hole";
	   ObjName = "&Lwormhole&n";
	   ObjSittingDesc = "&LA Wormhole is here tearing itself apart&n";
	   ObjType = ITEM_TYPE_PORTAL;
	   Vnum = CPortal::WORMHOLE;
	} else if (nAffect == CPortal::MOONWELL) {
	   ObjAlias = "portal moon well moonwell";
	   ObjName = "&WMoonwell&n";
	   ObjSittingDesc = "&GA &WMoonwell &Gstirs here, bubbling and churning.&n";
	   ObjType = ITEM_TYPE_PORTAL;
	   Vnum = CPortal::MOONWELL;
	} else if (nAffect == CPortal::GUKS_MAGIC_PORTAL) {
	   ObjAlias = "portal guks magic";
	   ObjName = "&Rmagic &Yportal&n";
	   ObjSittingDesc = "&YA Po&Grtal &Bof s&Rwirling &Gcolors &Wis &Mhere.&n";
	   ObjType = ITEM_TYPE_PORTAL;
	   Vnum = CPortal::GUKS_MAGIC_PORTAL;
	}
   } else// if(nAffect == CObject::OBJ_AFFECT_DELAYED_BLAST_FIREBALL)
   {
	ObjAlias = "fireball fire ball";
	ObjName = "&Rfireball&n";
	ObjSittingDesc = "&RA h&Wo&Rt fireball sits here &rburning&R.";
	ObjType = ITEM_TYPE_SPELL_OBJ;
	Vnum = -2;
	Val0 = 0;
	Val1 = 0;
   }
   Val2 = 0;
   Val3 = 0;
   Val4 = 0;
   WearBit = 0;
   Weight = 0;
}

CObjectPrototype::CObjectPrototype(short nType, int nDam) {
   ObjAffects.SetDeleteData();
   ObjExtraDesc.SetDeleteData();
   AffBit = CObject::ITEM_NOT_TAKEABLE;
   Cost = 0;
   Material = 0;
   NumberInGame = 0;
   Vnum = -2;
   ObjAlias = "fireball fire ball";
   ObjName = "&Rfireball&n";
   ObjSittingDesc = "&RA &whot&R fireball sits here &rb&Ru&wr&Wn&wi&Rn&rg&R.";
   ObjType = ITEM_TYPE_SPELL_OBJ;
   Val1 = nDam;
   Val2 = 0;
   Val3 = 0;
   Val4 = 0;
   WearBit = 0;
   Weight = 0;
}

CObjectPrototype::~CObjectPrototype() {

}

void CObjectPrototype::VStat(CString & ToChar) {
   static CString strFormat(
	   "Name: %s       Material: %s\r\n"
	   "Val0: %d  Val1: %d  Val2: %d Val3: %d Val4:%d\r\n"
	   "Weight: %d		Cost: %d\r\n"
	   "Object Type: %s\r\n"
	   "Object Alias: %s\r\n"
	   "Obj Description: %s \r\n"
	   "Affects on Object:\r\n");

   ToChar.Format(strFormat.cptr(), ObjName.cptr(), GetMaterialName(Material),
	   Val0, Val1, Val2, Val3, Val4, Weight, Cost,
	   GetTypeName(ObjType), ObjAlias.cptr(),
	   ObjDesc.cptr());

   sObjectWearAffects *pAff;
   POSITION pos = ObjAffects.GetStartPosition();
   CString str;
   while (pos) {
	pAff = ObjAffects.GetNext(pos);
	str.Format("%s %d\r\n",
		GetAffectName(pAff->Location),
		pAff->Affect);
	ToChar += str;
   }
}

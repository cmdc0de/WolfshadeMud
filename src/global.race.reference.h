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
*  Wrapper for all racial ability classes
*
*
*/
//globalracereference.h
#ifndef _GLOBALRACEREFERENCE_H_
#define _GLOBALRACEREFERENCE_H_

#include "troll.reference.h"
#include "ogre.reference.h"
#include "orc.reference.h"
#include "gnoll.reference.h"
#include "duergar.dwarf.reference.h"
#include "drow.elf.reference.h"
#include "half.drow.elf.reference.h"
#include "githyanki.reference.h"
#include "draconian.reference.h"
#include "imp.reference.h"
#include "human.reference.h"
#include "pixie.reference.h"
#include "brownie.reference.h"
#include "saurial.reference.h"
#include "half.orc.reference.h"
#include "mul.reference.h"
#include "thrikreen.reference.h"
#include "minotaur.reference.h"
#include "kenku.reference.h"
#include "barbarian.reference.h"
#include "gnome.reference.h"
#include "grey.elf.reference.h"
#include "kender.reference.h"
#include "mountain.dwarf.reference.h"
#include "centaur.reference.h"
#include "half.grey.elf.reference.h"
#include "none.reference.h"
#include "undead.reference.h"
#include "lich.reference.h"
#include "illithid.reference.h"
#include "elemental.reference.h"


class CGlobalRaceReferences
{
private:
	const CTrollReference			TrollReference;
	const COgreReference			OgreReference;
	const COrcReference				OrcReference;
	const CGnollReference			GnollReference;
	const CDuergarDwarfReference	DuergarDwarfReference;
	const CDrowElfReference			DrowElfReference;
	const CHalfDrowElfReference		HalfDrowElfReference;
	const CGithyankiReference		GithyankiReference;
	const CDraconianReference		DraconianReference;
	const CImpReference				ImpReference;
	const CHumanReference			HumanReference;
	const CPixieReference			PixieReference;
	const CBrownieReference			BrownieReference;
	const CSaurialReference			SaurialReference;
	const CHalfOrcReference			HalfOrcReference;
	const CMulReference				MulReference;
	const CThriKreenReference		ThriKreenReference;
	const CMinotaurReference		MinotaurReference;
	const CKenkuReference			KenkuReference;
	const CBarbarianReference		BarbarianReference;
	const CGnomeReference			GnomeReference;
	const CGreyElfReference			GreyElfReference;
	const CKenderReference			KenderReference;
	const CMountainDwarfReference	MountainDwarfReference;
	const CCentaurReference			CentaurReference;
	const CHalfGreyElfReference		HalfGreyElfReference;
	const CIllithidReference		IllithidReference;
	const CNoneReference			NoneReference;
	const CUndeadReference			UndeadReference;
	const CLichReference			LichReference;
	const CElementalReference		ElementalReference;
public:
	CGlobalRaceReferences(){;}
	inline const CRacialReference *GetRaceReference(short int nRace);
};

inline const CRacialReference *CGlobalRaceReferences::GetRaceReference(short int nRace)
{
	switch(nRace)
	{
	case RACE_TROLL:
		return &TrollReference;break;
	case RACE_BARBARIAN:
		return &BarbarianReference;break;
	case RACE_OGRE:
		return &OgreReference;break;
	case RACE_ORC:
		return &OrcReference;break;
	case RACE_GNOLL:
		return &GnollReference;break;
	case RACE_DUERGAR_DWARF:
		return &DuergarDwarfReference;break;
	case RACE_DROW_ELF:
		return &DrowElfReference;break;
	case RACE_HALF_DROW_ELF:
		return &HalfDrowElfReference;break;
	case RACE_HALF_ORC:
		return &HalfOrcReference;break;
	case RACE_GITHYANKI:
		return &GithyankiReference;break;
	case RACE_DRACONIAN:
		return &DraconianReference;break;
	case RACE_IMP:
		return &ImpReference;break;
	case RACE_HUMAN:
		return &HumanReference;break;
	case RACE_PIXIE:
		return &PixieReference;break;
	case RACE_BROWNIE:
		return &BrownieReference;break;
	case RACE_SAURIAL:
		return &SaurialReference;break;
	case RACE_MUL:
		return &MulReference;break;
	case RACE_THRIKREEN:
		return &ThriKreenReference;break;
	case RACE_MINOTAUR:
		return &MinotaurReference;break;
	case RACE_KENKU:
		return &KenkuReference;break;
	case RACE_GNOME:
		return &GnomeReference;break;
	case RACE_GRAY_ELF:
		return &GreyElfReference;break;
	case RACE_KENDER:
		return &KenderReference;break;
	case RACE_MOUNTAIN_DWARF:
		return &MountainDwarfReference;break;
	case RACE_CENTAUR:
		return &CentaurReference;break;
	case RACE_HALF_GREY_ELF:
		return &HalfGreyElfReference;break;
	case RACE_ILLITHID:
		return &IllithidReference;break;
	case RACE_NONE:
		return &NoneReference;break;
	case RACE_UNDEAD:
		return &UndeadReference;break;
	case RACE_LICH:
		return &LichReference;break;
	case RACE_ELEMENTAL:
		return &ElementalReference;break;
	default:
		ErrorLog << "Illegal Race in CGlobalRaceReferences\r\n" << endl;
		return NULL;
		break;
	}
}
#endif
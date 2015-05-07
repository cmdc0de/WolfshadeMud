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
#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif
#define FD_SETSIZE 1024
#define ROLL(d,t) GenNum.Roll(d,t)
#define DIE(d) GenNum.Roll(d)

typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned char ubyte;
#ifndef _WOLFSHADE_WINDOWS_
typedef char byte;
#endif

typedef char skill_type;
typedef short stat_type;
typedef int mob_vnum;
typedef int obj_vnum;
typedef long base_money;
typedef long room_vnum;
typedef unsigned int * POSITION;

#define MAX_FILE_LENGTH 255
#define MAX_COMMAND_BUFFER 4096

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define RANGE(value,high,low) (value >= high ? high : value <= low ? low : value)

#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#endif
#ifndef MAKELONG
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#endif
#ifndef LOWORD
#define LOWORD(l)           ((WORD)(l))
#endif
#ifndef HIWORD
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#endif
#ifndef LOBYTE
#define LOBYTE(w)           ((BYTE)(w))
#endif
#ifndef HIBYTE
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
#define UP 4
#define DOWN 5
#define NUM_OF_DOORS 6

#define EXP_PER_MOB_LEVEL 500
#define HITPTS_OF_DEATH -10
#define EXP_LOST_IN_DEATH 20
#define NPC_HIT_POINT_MULTIPLIER 5

#define SPELL_SAVE_PREFIX "players/spellsave/"

/*Special mob Vnums*/
#define VNUM_FIRE_ELEMENTAL 1
#define VNUM_AIR_ELEMENTAL 2
#define VNUM_EARTH_ELEMENTAL 4
#define VNUM_WATER_ELEMENTAL 3
#define VNUM_PYRE_ELEMENTAL 5
#define VNUM_FOG_ELEMENTAL 6
#define VNUM_GRAVE_ELEMENTAL 8
#define VNUM_BLOOD_ELEMENTAL 7
#define VNUM_COAL_ELEMENTAL 30
#define VNUM_ASH_ELEMENTAL 31
#define VNUM_STEAM_ELEMENTAL 32
#define VNUM_PLASMA_ELEMENTAL 33
#define VNUM_MIST_ELEMENTAL 34
#define VNUM_SMOKE_ELEMENTAL 35
#define VNUM_ICE_ELEMENTAL 36
#define VNUM_SLIME_ELEMENTAL 37
#define VNUM_ROCK_ELEMENTAL 38
#define VNUM_TOMB_ELEMENTAL 39
#define VNUM_MUD_ELEMENTAL 40
#define VNUM_GORE_ELEMENTAL 41
#define VNUM_MAGIC_ELEMENTAL 42
// -negative vnums for mobs we don't load from flat files
#define VNUM_UNDEAD_SKELETON -4
#define	VNUM_UNDEAD_ZOMBIE -5
#define VNUM_UNDEAD_SPECTRE -6
#define VNUM_UNDEAD_GHOST -7
#define VNUM_UNDEAD_GHOUL -8
#define VNUM_UNDEAD_WIGHT -9
#define VNUM_UNDEAD_WRAITH -10
#define VNUM_WALL_OF_AIR -11
#define VNUM_WALL_OF_BONES -12
#define VNUM_WALL_OF_BRAMBLES -13
#define VNUM_WALL_OF_FLAMES -14
#define VNUM_WALL_OF_GLASS -15
#define VNUM_WALL_OF_ICE -16
#define VNUM_WALL_OF_SAND -17
#define VNUM_WALL_OF_STONE -18
#define VNUM_WALL_OF_WATER -19
#define VNUM_WALL_GUKS_HAND -20

//SPECIAL OBJ VNUMS
#define VNUM_MIRAGE -4

//special room vnums
#define TRANSPORT_ROOM_VNUM -5
#define FUGUE_PLANE 10101

/*Stats*/
#define STRADD	0
#define STR		1
#define INT		2
#define WIS		3
#define AGI		4
#define CON		5
#define DEX		6
#define CHA		7
#define POW		8
#define LUCK    9
#define MAX_NUMBER_OF_STATS 10
#define MAX_STAT 120

#define NOWHERE -1

#define MIN_AC -100
#define MAX_AC 100
#define TOTAL_SAVES 4
#define AC_NOTCH 5
#define MIN_SPELL_LETTERS 3
#define EVIL_ALIGN -300
#define GOOD_ALIGN 300
#define MIN_FRAG_LEVEL 20
#define MAX_FRAG_LEVEL_DIFF 15
#define TOTAL_LANGUAGES (TOTAL_PLAYER_RACES + 4) //an extra four for the future

//LEVELS
#define LVL_LAST_MORTAL 50
#define LVL_IMMORT	57
#define LVL_LAST_PLAYER 56
#define LVL_IMP 60
#define LVL_LESSER_GOD LVL_IMMORT
#define LVL_GREATER_GOD 59

//sex
#define NONE 0
#define MALE 1
#define FEMALE 2

//sizes
#define NO_SIZE -1
#define SIZE_TINY 1
#define SIZE_SMALL 2
#define SIZE_MEDIUM 3
#define SIZE_LARGE 4
#define SIZE_HUGE 5
#define SIZE_GIANT 6
#define TOTAL_SIZES 7

//EQ
#define MAX_EQ_POS 22 //that any class can wear used to help enforce equality in classes

#define WEAR_POS_HEAD (1<<0)
#define WEAR_POS_HORNS (1<<1)
#define WEAR_POS_EAR (1<<2)
#define WEAR_POS_EYES (1<<3)
#define WEAR_POS_NOSE (1<<4)
#define WEAR_POS_FACE (1<<5)
#define WEAR_POS_NECK (1<<6)
#define WEAR_POS_BODY (1<<7)
#define WEAR_POS_ON_BACK (1<<8)
#define WEAR_POS_ARMS (1<<9)
#define WEAR_POS_WRIST (1<<10)
#define WEAR_POS_RING (1<<11)
#define WEAR_POS_WIELD (1<<12)
#define WEAR_POS_DUAL_WIELD (1<<13)
#define WEAR_POS_WIELD_TWO_HANDED (1<<14)
#define WEAR_POS_SHIELD (1<<15)
#define WEAR_POS_HOLD (1<<16)
#define WEAR_POS_WAIST (1<<17)
#define WEAR_POS_TAIL (1<<18)
#define WEAR_POS_LEGS (1<<19)
#define WEAR_POS_FEET (1<<20)
#define WEAR_POS_HANDS (1<<21)
#define WEAR_POS_BADGE (1<<22)
#define WEAR_POS_WINGS (1<<23)
#define WEAR_POS_ABOUT_BODY (1<<24)



//skills
#define MAX_SKILL_LEVEL 99
#define MAX_MUD_SKILLS 130 //room to grow =)

#define SKILL_BASH		             1
#define SKILL_APPLY_POISON           2
#define SKILL_APPRAISE               3
#define SKILL_ARMOR_REPAIR           4
#define SKILL_AWARENESS              5
#define SKILL_BARE_HAND_FIGHTING     6
#define SKILL_BATTLE_READINESS       7
#define SKILL_BEAR_HUG               8
#define SKILL_BEG                    9
#define SKILL_BERSERK               10
#define SKILL_BLITHER               11
#define SKILL_BRIBE                 12
#define SKILL_CALLED_SHOT           13
#define SKILL_CAMOUFLAGE            14
#define SKILL_CAPTURE               15
#define SKILL_CHANT                 16
#define SKILL_CHANT_DEFENSE         17
#define SKILL_CHANT_HONOR           18
#define SKILL_CHANT_REGENERATION    19
#define SKILL_CIRCLE                20
#define SKILL_CLIMB_WALLS           21
#define SKILL_CHARM                 22
#define SKILL_DIRT_TOSS             23
#define SKILL_DISARM                24
#define SKILL_DIVERSION             25
#define SKILL_DOUBLE_ATTACK         26
#define SKILL_DOUBLE_BACKSTAB       27
#define SKILL_DRAGON_PUNCH          28
#define SKILL_FAR_SIGHT             29
#define SKILL_FAST_PREP             30
#define SKILL_FORGERY               31
#define SKILL_GAMBLE                32
#define SKILL_HEAD_BUTT             33
#define SKILL_HEROISM               34
#define SKILL_HIT_ALL               35
#define SKILL_HONE_WEAPON           36
#define SKILL_INSTANT_KILL			37
#define SKILL_INTIMIDATE            38
#define SKILL_JUGGLING              39
#define SKILL_JUMPING               40
#define SKILL_KNIFE_TURN            41
#define SKILL_KNIGHT_CODE           42
#define SKILL_LAY_HANDS             43
#define SKILL_LISTEN                44
#define SKILL_LOOTING               45
#define SKILL_MEDITATE              46
#define SKILL_MOUNTED_COMBAT        47
#define SKILL_OBSERVATION           48
#define SKILL_PARRY                 49
#define SKILL_POLYMORPH             50
#define SKILL_PRAY                  51
#define SKILL_PREPARE               52
#define SKILL_QUICK_CHANT           53
#define SKILL_QUICK_STRIKE          54
#define SKILL_QUIVERING_PALM        55
#define SKILL_RANGED_WEAPONS        56
#define SKILL_REMOVE_TRAP           57
#define SKILL_RIPOSTE               58
#define SKILL_SHIELD_RUSH           59
#define SKILL_SONG_CHARMING         60
#define SKILL_SONG_FLIGHT           61
#define SKILL_SONG_HEALING          62
#define SKILL_SONG_HEROISM          63
#define SKILL_SONG_HURTING          64
#define SKILL_SONG_REVELATION       65
#define SKILL_STEAL                 66
#define SKILL_SUBTERFUGE            67
#define SKILL_SUMMON_MOUNT          68
#define SKILL_WEAPON_FLURRY         69
#define SKILL_THROAT_SLIT           70
#define SKILL_THROW_WEAPON          71
#define SKILL_TRACK                 72
#define SKILL_TRAP                  73
#define SKILL_TUMBLING              74
#define SKILL_VAMPIRIC_TOUCH        75
#define SKILL_WEAPON_KNOWLEDGE      76
#define SKILL_WEAPON_REPAIR         77
#define SKILL_WRESTLING             78
#define SKILL_DOORBASH				79
#define SKILL_DODGE					80
#define	SKILL_DUAL_WIELD			81
#define SKILL_TRIPLE_ATTACK			82
#define SKILL_BLIND_FIGHTING		83
//#define SKILL_2H_FIGHTING			84 //can be re-used
#define	SKILL_BACKSTAB				85
#define SKILL_OFFENSE				86
#define SKILL_LOOT					87
#define SKILL_BANDAGE				88
#define SKILL_BODYSLAM				89
#define	SKILL_ROAR					90
#define SKILL_FIRST_AID				91
#define SKILL_BERZERK				92
#define SKILL_BOWS					93
//#define SKILL_KNIFE_DAGGERS			94 //can be re-used
#define SKILL_MACE_CLUB				95 //can be re-used
#define SKILL_MOUNT					96
#define SKILL_RESCUE				97
//#define SKILL_SHIELD_USE			98 //can be re-used
#define SKILL_STAVES				99
//#define SKILL_SWORDS				100 //can be re-used
#define SKILL_SWITCH				101
#define SKILL_TRIP					102
#define SKILL_SNEAK					103
#define SKILL_HIDE					104
#define SKILL_KICK					105
#define SKILL_TUNDRA_MAGIC			106
#define SKILL_DESERT_MAGIC			107
#define SKILL_MOUNTAIN_MAGIC		108
#define SKILL_OCEAN_MAGIC			109
#define SKILL_FOREST_MAGIC			110
#define SKILL_SWAMP_MAGIC			111
#define SKILL_GREY_MAGIC			112
//#define SKILL_PUNCH					113 //can be re-used
#define SKILL_FORAGE				114
#define SKILL_CLERIC_MAGIC			115
#define SKILL_SHAMAN_MAGIC			116
#define SKILL_SHADOW				117
#define SKILL_SING					118
#define SKILL_DANCE					119
#define SKILL_INSTRUMENT_PLAYING    120
#define SKILL_PICK_LOCKS			121
#define SKILL_DISGUISE				122


//object affects
#define LOC_NONE	0
#define LOC_STRADD 1
#define LOC_STR	2
#define LOC_AGI 3
#define LOC_DEX	4
#define LOC_INT	5
#define LOC_WIS	6
#define LOC_CON	7
#define LOC_CHA	8
#define LOC_POW 9
#define LOC_LUCK 10
#define LOC_CLASS 11
#define LOC_LEVEL 12
#define LOC_AGE	13
#define LOC_WIEGHT	14
#define LOC_HEIGHT	15
#define LOC_MANA	16
#define LOC_HITPOINTS	17
#define LOC_MOVE	18
#define LOC_EXP	19
#define LOC_AC	20
#define LOC_HITROLL	21
#define LOC_DAMROLL	22
#define LOC_SAVE_PARA	23
#define LOC_SAVE_POISON	24
#define LOC_SAVE_BREATHE	25
#define LOC_SAVE_SPELL	26

#define NORACE 0
#define RACE_TROLL 1
#define RACE_OGRE 2
#define RACE_ORC 3
#define RACE_GNOLL 4
#define RACE_DUERGAR_DWARF 5
#define RACE_DROW_ELF 6
#define RACE_HALF_DROW_ELF 7
#define RACE_GITHYANKI 8
#define RACE_DRACONIAN 9
#define RACE_IMP 10
#define RACE_GNOME 11
#define RACE_GRAY_ELF 12
#define RACE_KENDER 13
#define RACE_MOUNTAIN_DWARF 14
#define RACE_BROWNIE 15
#define RACE_PIXIE 16
#define RACE_CENTAUR 17
#define RACE_SAURIAL 18
#define RACE_HALF_GREY_ELF 19
#define RACE_HUMAN 20
#define RACE_THRIKREEN 21
#define RACE_MINOTAUR 22
#define RACE_HALF_ORC 23
#define RACE_BARBARIAN 24
#define RACE_KENKU 25
#define RACE_MUL 26
#define RACE_ILLITHID 27
#define RACE_NONE 28
#define RACE_UNDEAD 29
#define RACE_LICH 30
#define RACE_ELEMENTAL 31
#define TOTAL_RACES 32 //add in no race
#define NON_PLAYER_RACES 4
#define TOTAL_PLAYER_RACES (TOTAL_RACES-NON_PLAYER_RACES)

//CLASSES
#define NOCLASS 0
#define CLASS_WARRIOR 1
#define CLASS_CLERIC 2
#define CLASS_MAGE 3
#define CLASS_ROGUE 4
#define CLASS_DRUID 5
#define CLASS_RANGER 6
#define CLASS_DEFILER 7
#define CLASS_PALADIN 8
#define CLASS_ANTI_PALADIN 9
#define CLASS_SHAMAN 10
#define CLASS_PSIONICIST 11
#define CLASS_EMPATH 12
#define CLASS_MINDFLAYER 13
#define TOTAL_CLASSES 14 //add in the no class

/*Positions*/
#define POS_DEAD (1<<0)
#define POS_KOED (1<<1)
#define POS_MORTALED (1<<2)
#define POS_INCAP (1<<3)
#define POS_STUNNED (1<<4)
#define POS_RECLINING (1<<5)
#define POS_SLEEPING (1<<6)
#define POS_RESTING (1<<7)
#define POS_SITTING (1<<8)
#define POS_STANDING (1<<9)
#define POS_FIGHTING (1<<10)
#define POS_PREP (1<<11)
#define TOTAL_POSITIONS 12


/*Gods*/
#define GOD_NONE 0
#define	GOD_GUK 1
#define GOD_ARKAN 2
#define GOD_NNGH 3
#define GOD_DUTH 4
#define GOD_ILSENSINE 5
#define TOTAL_GODS 6

//flat file defines
#define MAX_NAME_SIZE 30
#define MAX_PASSWORD 10
#define MAX_LONG_STR 512
#define MAX_STR 90
#define MAX_OBJ_SHORT_STR 90
#define MAX_OBJ_LONG_STR 256
#define MAX_SPELL_NAME_LENGTH 30
#define TOTAL_AFFECTS 100
//
#endif
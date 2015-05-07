

#ifndef STD_HEADER_H_
#define STD_HEADER_H_

#ifdef _WOLFSHADE_WINDOWS_
#pragma warning(disable: 4522)
#pragma warning(disable: 4250)
#pragma warning(disable: 4786)
#include <winsock2.h>
#else
#include "conf.h"
#endif
#include <iostream>
#include <fstream>
#include "mudtime.h"
#include "ascii.stream.h"
#include "constants.h"
#include "log.h"
#include "error.h"
#include "corpse.h"
#include "room.h"
#include "game.h"
#include "npc.classes.h"
#include "rogue.h"
#include "warrior.h"
#include "mage.h"
#include "psionicist.h"
#include "mindflayer.h"
#include "empath.h"
#include "cleric.h"
#include "defiler.h"
#include "ranger.h"
#include "random.h"
#include "shaman.h"
#include "druid.h"
#include "paladin.h"
#include "anti.paladin.h"
#include "global.management.h"
#endif

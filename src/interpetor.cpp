/*
 *
 *  Explain how this really works!
 *
 */
#include "random.h"
extern CRandom GenNum;
#include "stdinclude.h"
#include "interpetor.h"
#include "transport.h"
#include "transportroom.h"

extern CGlobalVariableManagement GVM;
extern CRandom GenNum;
#define BUG_FILE "files/bug.txt"

#define MIN_CMD_LETTERS 1
#define MOST_NOT_ALLOWED_POS (POS_DEAD | POS_KOED | POS_INCAP | POS_SLEEPING | POS_MORTALED)

const int CInterp::REMOVE_HIDE = (1 << 0);
const int CInterp::REMOVE_MEDITATE = (1 << 1);
const int CInterp::REMOVE_INVIS = (1 << 2);
const int CInterp::REMOVE_CAMP = (1 << 3);

const char *CInterp::m_strHelpFileName = "files/help.txt";
const char CInterp::HELP_FILE_DELIM = '#';
const char *CInterp::m_strBasicHelp = "			Basic Help\r\n\r\n"
		"--------------------------------------------------------------------\r\n"
		"help commands - gives you a list of commands. ie help commands\r\n"
		"help <command name> - gives you syntax and what it does.  ie help bash\r\n"
		"help <class name> - gives you help on the PC class type. ie help rogue\r\n"
		"---------------------------------------------------------------------\r\n";

//the last one is the first one found 

void CInterp::BuildCommands() {
	/*POS_DEAD|POS_MORTALED|POS_INCAP|POS_STUNNED|POS_SLEEPING
	 POS_RESTING|POS_SITTING|POS_STANDING|POS_FLYING|POS_FIGHTING*/
	m_pCmdTable->Add("advance",
			new sComClass("advance", &CInterp::AdvanceLevel, LVL_IMMORT, POS_DEAD | POS_SLEEPING | POS_INCAP));
	m_pCmdTable->Add("attribute", new sComClass("wield", &CInterp::Attribute, 0, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("appraise",
			new sComClass("appraise", &CInterp::Appraise, 0,
					POS_DEAD | POS_INCAP | POS_SLEEPING | POS_STUNNED | POS_FIGHTING));
	m_pCmdTable->Add("ask",
			new sComClass("ask", &CInterp::Ask, 0, POS_DEAD | POS_INCAP | POS_SLEEPING | POS_STUNNED | POS_FIGHTING,
					REMOVE_HIDE));
	m_pCmdTable->Add("assist",
			new sComClass("assist", &CInterp::Assist, 0,
					MOST_NOT_ALLOWED_POS | POS_FIGHTING | POS_RESTING | POS_PREP | POS_SITTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("bug", new sComClass("bug", &CInterp::Bug, 0, POS_DEAD));
	m_pCmdTable->Add("buy",
			new sComClass("buy", &CInterp::Buy, 0, POS_DEAD | POS_INCAP | POS_SLEEPING | POS_MORTALED,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("bodyslam",
			new sComClass("bodyslam", &CInterp::BodySlam, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING | POS_FIGHTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("bash",
			new sComClass("bash", &CInterp::Bash, 0, POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("backstab",
			new sComClass("backstab", &CInterp::BackStab, 0,
					POS_DEAD | POS_INCAP | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("bandage",
			new sComClass("bandage", &CInterp::Bandage, 0, MOST_NOT_ALLOWED_POS,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("bearhug",
			new sComClass("bearhug", &CInterp::BearHug, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("berserk",
			new sComClass("berserk", &CInterp::Berserk, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("beg",
			new sComClass("beg", &CInterp::Beg, 0,
					POS_DEAD | POS_INCAP | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("camp",
			new sComClass("camp", &CInterp::Camp, 0, POS_DEAD | POS_INCAP | POS_FIGHTING | POS_SLEEPING | POS_RESTING,
					REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("cutlink", new sComClass("cutlink", &CInterp::Cutlink, LVL_IMMORT, MOST_NOT_ALLOWED_POS));
	m_pCmdTable->Add("close",
			new sComClass("close", &CInterp::Close, 0, MOST_NOT_ALLOWED_POS | POS_FIGHTING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("consent", new sComClass("consent", &CInterp::Consent, 0, MOST_NOT_ALLOWED_POS));
	m_pCmdTable->Add("consider", new sComClass("consider", &CInterp::Consider, 0, MOST_NOT_ALLOWED_POS));
	m_pCmdTable->Add("charm",
			new sComClass("charm", &CInterp::Charm, 0,
					POS_DEAD | POS_INCAP | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("chant",
			new sComClass("chant", &CInterp::Chant, 0, POS_DEAD | POS_INCAP | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("circle",
			new sComClass("circle", &CInterp::Circle, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("create", new sComClass("create", &CInterp::Create, LVL_IMMORT, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("cast",
			new sComClass("cast", &CInterp::Cast, 0, MOST_NOT_ALLOWED_POS | POS_RESTING | POS_SITTING | POS_PREP,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("call",
			new sComClass("call", &CInterp::CalledShot, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("dragonpunch",
			new sComClass("dragonpunch", &CInterp::DragonPunch, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("deposit",
			new sComClass("deposit", &CInterp::Deposit, 0,
					POS_SLEEPING | POS_DEAD | POS_INCAP | POS_MORTALED | POS_PREP | POS_SITTING | POS_RECLINING
							| POS_RESTING, REMOVE_HIDE | REMOVE_CAMP | REMOVE_MEDITATE));
	m_pCmdTable->Add("drop",
			new sComClass("drop", &CInterp::Drop, 0, MOST_NOT_ALLOWED_POS, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("drag",
			new sComClass("drag", &CInterp::Drag, 0,
					POS_DEAD | POS_SLEEPING | POS_INCAP | POS_SITTING | POS_RECLINING | POS_RESTING | POS_PREP
							| POS_MORTALED, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("disarm",
			new sComClass("disarm", &CInterp::Disarm, 0, MOST_NOT_ALLOWED_POS,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("disband",
			new sComClass("disband", &CInterp::Disband, 0, POS_SLEEPING | POS_DEAD | POS_INCAP | POS_MORTALED));
	m_pCmdTable->Add("dirttoss",
			new sComClass("dirttoss", &CInterp::DirtToss, 0, MOST_NOT_ALLOWED_POS,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("disembark",
			new sComClass("disembark", &CInterp::Disembark, 0,
					POS_DEAD | POS_MORTALED | POS_INCAP | POS_FIGHTING | POS_RESTING | POS_SITTING | POS_SLEEPING
							| POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("disengage",
			new sComClass("disengage", &CInterp::Disengage, 0, MOST_NOT_ALLOWED_POS,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("down",
			new sComClass("down", &CInterp::Move, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING
							| POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("echo", new sComClass("echo", &CInterp::Echo, LVL_IMMORT, POS_DEAD));
	m_pCmdTable->Add("experience", new sComClass("experience", &CInterp::Experience, 0, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("exit", new sComClass("exit", &CInterp::Exit, 0, MOST_NOT_ALLOWED_POS));
	m_pCmdTable->Add("examine",
			new sComClass("examine", &CInterp::Examine, 0, MOST_NOT_ALLOWED_POS, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("equipment",
			new sComClass("equipment", &CInterp::Equipment, 0, POS_DEAD | POS_INCAP | POS_SLEEPING));
	m_pCmdTable->Add("emote",
			new sComClass("emote", &CInterp::Emote, 0, POS_DEAD | POS_INCAP | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("enter",
			new sComClass("enter", &CInterp::Enter, 0, POS_DEAD | POS_INCAP | POS_SITTING | POS_FIGHTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("east",
			new sComClass("east", &CInterp::Move, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING
							| POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("first aid",
			new sComClass("first aid", &CInterp::FirstAid, 0, POS_DEAD | POS_SLEEPING | POS_RECLINING | POS_STUNNED,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("follow",
			new sComClass("follow", &CInterp::Follow, 0, MOST_NOT_ALLOWED_POS, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("forget",
			new sComClass("forget", &CInterp::Forget, 0, MOST_NOT_ALLOWED_POS, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("force_cast",
			new sComClass("force_cast", &CInterp::FCast, 0, MOST_NOT_ALLOWED_POS | POS_RESTING | POS_SITTING | POS_PREP,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("fcast",
			new sComClass("fcast", &CInterp::FCast, 0, MOST_NOT_ALLOWED_POS | POS_RESTING | POS_SITTING | POS_PREP,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("fugue",
			new sComClass("fugue", &CInterp::FugueTime, LVL_IMMORT,
					MOST_NOT_ALLOWED_POS | POS_RESTING | POS_SITTING | POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("flurry",
			new sComClass("flurry", &CInterp::Flurry, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("flee",
			new sComClass("flee", &CInterp::Flee, 0, MOST_NOT_ALLOWED_POS | POS_RECLINING | POS_RESTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("fraglist",
			new sComClass("fraglist", &CInterp::FragList, 0,
					POS_DEAD | POS_SLEEPING | POS_KOED | POS_INCAP | POS_MORTALED));
	m_pCmdTable->Add("gfreeze",
			new sComClass("gfreeze", &CInterp::GodFreeze, LVL_GREATER_GOD, POS_DEAD | POS_SLEEPING | POS_KOED));
	m_pCmdTable->Add("grelease",
			new sComClass("grelease", &CInterp::GRelease, LVL_IMMORT, POS_DEAD | POS_SLEEPING | POS_KOED));
	m_pCmdTable->Add("gresurrect", new sComClass("gresurrect", &CInterp::GodRes, LVL_IMMORT, POS_DEAD | POS_SLEEPING));
	m_pCmdTable->Add("glocate", new sComClass("glocate", &CInterp::GodLocate, LVL_IMMORT, POS_DEAD | POS_SLEEPING));
	m_pCmdTable->Add("gforce", new sComClass("gforce", &CInterp::GodForce, LVL_IMMORT, POS_DEAD | POS_SLEEPING));
	m_pCmdTable->Add("gmsg", new sComClass("gmsg", &CInterp::GodMessage, LVL_IMMORT, POS_DEAD | POS_SLEEPING));
	m_pCmdTable->Add("goto",
			new sComClass("goto", &CInterp::DoGoto, LVL_IMMORT, MOST_NOT_ALLOWED_POS,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("godchat", new sComClass("godchat", &CInterp::DoGodChat, LVL_IMMORT, MOST_NOT_ALLOWED_POS));
	m_pCmdTable->Add("gchange", new sComClass("gchange", &CInterp::GodChange, LVL_IMMORT, POS_DEAD | POS_SLEEPING));
	m_pCmdTable->Add("gchat", new sComClass("gchat", &CInterp::DoGodChat, LVL_IMMORT, MOST_NOT_ALLOWED_POS));
	m_pCmdTable->Add("gkill",
			new sComClass("gkill", &CInterp::GodKill, LVL_IMMORT, MOST_NOT_ALLOWED_POS, REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("group-say",
			new sComClass("group-say", &CInterp::GroupSay, 0, MOST_NOT_ALLOWED_POS, REMOVE_MEDITATE));
	m_pCmdTable->Add("gsay", new sComClass("gsay", &CInterp::GroupSay, 0, MOST_NOT_ALLOWED_POS, REMOVE_MEDITATE));
	m_pCmdTable->Add("group", new sComClass("group", &CInterp::Group, 0, MOST_NOT_ALLOWED_POS));
	m_pCmdTable->Add("glance", new sComClass("glance", &CInterp::Glance, 0, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("give",
			new sComClass("give", &CInterp::Give, 0, MOST_NOT_ALLOWED_POS, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("get",
			new sComClass("get", &CInterp::Get, 0, POS_DEAD | POS_INCAP, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("kick",
			new sComClass("kick", &CInterp::Kick, 0, MOST_NOT_ALLOWED_POS | POS_RESTING | POS_SITTING | POS_PREP,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("kill",
			new sComClass("kill", &CInterp::Kill, 0,
					MOST_NOT_ALLOWED_POS | POS_RECLINING | POS_RESTING | POS_SITTING | POS_PREP,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("hide",
			new sComClass("hide", &CInterp::Hide, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_FIGHTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("headbutt",
			new sComClass("headbutt", &CInterp::HeadButt, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING | POS_SITTING | POS_RESTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("hitall",
			new sComClass("hitall", &CInterp::HitAll, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING | POS_SITTING | POS_RESTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("hone",
			new sComClass("hone", &CInterp::Hone, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING | POS_SITTING | POS_RESTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("help", new sComClass("help", &CInterp::Help, 0, POS_DEAD));
	m_pCmdTable->Add("hometown", new sComClass("hometown", &CInterp::HomeTown, 0, POS_DEAD | POS_SLEEPING));
	m_pCmdTable->Add("innate",
			new sComClass("innate", &CInterp::Innate, 0, POS_DEAD | POS_SLEEPING | POS_INCAP | POS_MORTALED | POS_PREP,
					REMOVE_MEDITATE));
	m_pCmdTable->Add("identify", new sComClass("identify", &CInterp::Identify, 0, POS_DEAD | POS_INCAP, REMOVE_CAMP));
	m_pCmdTable->Add("intimidate",
			new sComClass("intimidate", &CInterp::Intimidate, 0,
					MOST_NOT_ALLOWED_POS | POS_RESTING | POS_SITTING | POS_PREP,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("initiate", new sComClass("initiate", &CInterp::Initiate, LVL_IMMORT, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("inventory",
			new sComClass("inventory", &CInterp::Inventory, 0, POS_DEAD | POS_INCAP | POS_SLEEPING));
	m_pCmdTable->Add("layhands",
			new sComClass("layhands", &CInterp::LayHands, 0, POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("list",
			new sComClass("list", &CInterp::List, 0, POS_DEAD | POS_INCAP, REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("loot",
			new sComClass("loot", &CInterp::Loot, 0, MOST_NOT_ALLOWED_POS | POS_FIGHTING | POS_RESTING | POS_RECLINING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("look", new sComClass("look", &CInterp::Look, 0, POS_DEAD | POS_KOED | POS_SLEEPING | POS_INCAP));
	m_pCmdTable->Add("motd", new sComClass("motd", &CInterp::Motd, 0, POS_DEAD | POS_SLEEPING | POS_STUNNED));
	m_pCmdTable->Add("map", new sComClass("map", &CInterp::Map, 0, POS_DEAD | POS_INCAP | POS_MORTALED));
	m_pCmdTable->Add("mediate",
			new sComClass("mediate", &CInterp::Mediate, 0, MOST_NOT_ALLOWED_POS | POS_STANDING | POS_RECLINING,
					REMOVE_HIDE | REMOVE_CAMP));
	m_pCmdTable->Add("news", new sComClass("news", &CInterp::News, 0, POS_DEAD | POS_INCAP | POS_SLEEPING, 0));
	m_pCmdTable->Add("north",
			new sComClass("north", &CInterp::Move, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING
							| POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("open",
			new sComClass("open", &CInterp::Open, 0,
					POS_DEAD | POS_INCAP | POS_FIGHTING | POS_SITTING | POS_SLEEPING | POS_RESTING | POS_PREP,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("ooc", new sComClass("ooc", &CInterp::OOC, 0, POS_SLEEPING | POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("order",
			new sComClass("order", &CInterp::Order, 0, MOST_NOT_ALLOWED_POS, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("purge",
			new sComClass("purge", &CInterp::Purge, LVL_IMMORT, POS_DEAD | POS_INCAP | POS_SLEEPING | POS_KOED));
	m_pCmdTable->Add("petition", new sComClass("petition", &CInterp::DoPetition, 0, MOST_NOT_ALLOWED_POS));
	m_pCmdTable->Add("prepare",
			new sComClass("prepare", &CInterp::Prepare, 0, POS_DEAD | POS_INCAP | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_CAMP));
	m_pCmdTable->Add("practice",
			new sComClass("practice", &CInterp::Practice, 0,
					POS_DEAD | POS_INCAP | POS_SLEEPING | POS_STANDING | POS_FIGHTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("put",
			new sComClass("put", &CInterp::Put, 0, POS_DEAD | POS_INCAP | POS_SLEEPING | POS_FIGHTING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("quiveringpalm",
			new sComClass("quiveringpalm", &CInterp::QuiveringPalm, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("quaff",
			new sComClass("quaff", &CInterp::UseObject, 0, POS_DEAD | POS_SLEEPING | POS_STUNNED | POS_KOED | POS_INCAP,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("refresh", new sComClass("refresh", &CInterp::Refresh, LVL_IMMORT, POS_DEAD | POS_SLEEPING));
	m_pCmdTable->Add("remove",
			new sComClass("remove", &CInterp::Remove, 0, POS_DEAD | POS_INCAP | POS_SLEEPING | POS_RECLINING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("return", new sComClass("return", &CInterp::Return, 0, POS_DEAD));
	m_pCmdTable->Add("restore", new sComClass("restore", &CInterp::Restore, LVL_IMMORT, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("roomstat", new sComClass("roomstat", &CInterp::RoomStat, LVL_IMMORT, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("rent",
			new sComClass("rent", &CInterp::Rent, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING | POS_RESTING | POS_SITTING | POS_FIGHTING
							| POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("report",
			new sComClass("report", &CInterp::Report, 0, POS_DEAD | POS_INCAP | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("rescue",
			new sComClass("rescue", &CInterp::Rescue, 0, POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("rush",
			new sComClass("rush", &CInterp::ShieldRush, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING | POS_FIGHTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("recline",
			new sComClass("recline", &CInterp::Recline, 0, POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("rest",
			new sComClass("rest", &CInterp::Rest, 0, POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("split",
			new sComClass("split", &CInterp::Split, 0, POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("show", new sComClass("show", &CInterp::Show, LVL_IMMORT, MOST_NOT_ALLOWED_POS, 0));
	m_pCmdTable->Add("shout",
			new sComClass("shout", &CInterp::Shout, 0, MOST_NOT_ALLOWED_POS, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("sleep",
			new sComClass("sleep", &CInterp::Sleep, 0, POS_SLEEPING | POS_FIGHTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("scan",
			new sComClass("scan", &CInterp::Scan, 0, MOST_NOT_ALLOWED_POS | POS_FIGHTING | POS_RESTING | POS_PREP));
	m_pCmdTable->Add("set", new sComClass("set", &CInterp::Set, LVL_IMMORT, POS_DEAD));
	m_pCmdTable->Add("switch", new sComClass("switch", &CInterp::Switch, LVL_IMMORT, POS_DEAD));
	m_pCmdTable->Add("sell",
			new sComClass("sell", &CInterp::Sell, 0, POS_DEAD | POS_INCAP | POS_SLEEPING | POS_MORTALED,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("sail",
			new sComClass("sail", &CInterp::Sail, 0, POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING | POS_PREP,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("save", new sComClass("save", &CInterp::Save, 0, POS_DEAD | POS_FIGHTING));
	m_pCmdTable->Add("say",
			new sComClass("say", &CInterp::Say, 0, POS_DEAD | POS_INCAP | POS_SLEEPING, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("'",
			new sComClass("say", &CInterp::Say, 0, POS_DEAD | POS_INCAP | POS_SLEEPING, REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("shutdown",
			new sComClass("shutdown", &CInterp::ShutDownWolfshade, LVL_IMMORT,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING));
	m_pCmdTable->Add("search",
			new sComClass("search", &CInterp::Search, 0,
					POS_DEAD | POS_KOED | POS_SLEEPING | POS_RECLINING | POS_MORTALED | POS_SITTING | POS_RESTING
							| POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("skilllist",
			new sComClass("skilllist", &CInterp::SkillList, 0, POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING));
	m_pCmdTable->Add("skills", new sComClass("skills", &CInterp::Skill, 0, POS_DEAD | POS_INCAP | POS_SLEEPING));
	m_pCmdTable->Add("spy", new sComClass("spy", &CInterp::Spy, LVL_IMMORT, POS_DEAD));
	m_pCmdTable->Add("spelllist",
			new sComClass("spelllist", &CInterp::SpellList, 0, POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING));
	m_pCmdTable->Add("spells", new sComClass("spells", &CInterp::Spells, 0, POS_DEAD | POS_INCAP | POS_SLEEPING));
	m_pCmdTable->Add("sit",
			new sComClass("sit", &CInterp::Sit, 0, POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("strike",
			new sComClass("strike", &CInterp::Strike, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING | POS_SITTING | POS_RESTING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("stand",
			new sComClass("stand", &CInterp::Stand, 0, POS_DEAD | POS_INCAP | POS_MORTALED | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("score", new sComClass("score", &CInterp::Score, 0, POS_DEAD | POS_INCAP | POS_MORTALED));
	m_pCmdTable->Add("sneak",
			new sComClass("sneak", &CInterp::Sneak, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_FIGHTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("south",
			new sComClass("south", &CInterp::Move, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING
							| POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("take",
			new sComClass("take", &CInterp::Get, 0, POS_DEAD | POS_INCAP | POS_SLEEPING | POS_RECLINING,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("time", new sComClass("time", &CInterp::Time, 0, POS_DEAD, 0));
	m_pCmdTable->Add("think",
			new sComClass("think", &CInterp::Think, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_SITTING | POS_STUNNED | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("throat slit",
			new sComClass("throat slit", &CInterp::ThroatSlit, 0,
					POS_DEAD | POS_INCAP | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_MEDITATE | REMOVE_INVIS | REMOVE_CAMP));
	m_pCmdTable->Add("transport",
			new sComClass("transport", &CInterp::Transport, LVL_IMMORT, POS_DEAD | POS_INCAP,
					REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("track",
			new sComClass("track", &CInterp::Track, 0,
					POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING | POS_FIGHTING | POS_PREP,
					REMOVE_HIDE | REMOVE_CAMP | REMOVE_MEDITATE));
	m_pCmdTable->Add("trip",
			new sComClass("trip", &CInterp::Trip, 0, POS_DEAD | POS_INCAP | POS_SITTING | POS_RESTING | POS_SLEEPING,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("title", new sComClass("title", &CInterp::Title, 41, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("tog", new sComClass("tog", &CInterp::Tog, 0, MOST_NOT_ALLOWED_POS));
	m_pCmdTable->Add("tell", new sComClass("tell", &CInterp::Tell, 0, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("use",
			new sComClass("use", &CInterp::UseObject, 0, POS_DEAD | POS_SLEEPING | POS_STUNNED | POS_KOED | POS_INCAP,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("up",
			new sComClass("up", &CInterp::Move, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING
							| POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("withdraw",
			new sComClass("withdraw", &CInterp::Withdraw, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING
							| POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("wake",
			new sComClass("wake", &CInterp::Wake, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_PREP,
					REMOVE_MEDITATE | REMOVE_CAMP | REMOVE_HIDE));
	m_pCmdTable->Add("wear",
			new sComClass("wear", &CInterp::Wear, 0,
					POS_DEAD | POS_INCAP | POS_FIGHTING | POS_SLEEPING | POS_KOED | POS_STUNNED, REMOVE_MEDITATE));
	m_pCmdTable->Add("will",
			new sComClass("will", &CInterp::Will, 0, POS_DEAD | POS_INCAP | POS_MORTALED,
					REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("wield",
			new sComClass("attribute", &CInterp::Wield, 0, POS_DEAD | POS_INCAP | POS_SLEEPING, REMOVE_MEDITATE));
	m_pCmdTable->Add("worship",
			new sComClass("worship", &CInterp::Worship, 0,
					POS_DEAD | POS_INCAP | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING | POS_PREP,
					REMOVE_HIDE | REMOVE_MEDITATE));
	m_pCmdTable->Add("who", new sComClass("who", &CInterp::Who, 0, POS_DEAD | POS_INCAP | POS_FIGHTING | POS_SLEEPING));
	m_pCmdTable->Add("west",
			new sComClass("west", &CInterp::Move, 0,
					POS_DEAD | POS_INCAP | POS_MORTALED | POS_FIGHTING | POS_SITTING | POS_RESTING | POS_SLEEPING
							| POS_PREP, REMOVE_HIDE | REMOVE_MEDITATE | REMOVE_CAMP));
	m_pCmdTable->Add("vnum", new sComClass("vnum", &CInterp::Vnum, LVL_IMMORT, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("vstat", new sComClass("vstat", &CInterp::Vstat, LVL_IMMORT, POS_DEAD | POS_INCAP));
	m_pCmdTable->Add("vis", new sComClass("vis", &CInterp::Vis, 0, POS_DEAD | POS_SLEEPING | POS_INCAP));
}

CInterp::CInterp(CGame *pGame) {
	m_pGame = pGame;
	m_pCmdTable = new CMap<CString, sComClass *>(26, MIN_CMD_LETTERS);
	BuildCommands();
	BuildHelp();
	BugFile.open(BUG_FILE, std::ios_base::out | std::ios_base::app);
}

CInterp::~CInterp() {
	//delete memory for commands
	POSITION pos = m_pCmdTable->GetStartingPosition();
	while (pos) {
		delete m_pCmdTable->GetNext(pos);
	}
	delete m_pCmdTable;
	pos = m_HelpTable.GetStartingPosition();
	while (pos) {
		delete m_HelpTable.GetNext(pos);
	}
	BugFile.close();
}

void CInterp::InterpretCommand(CCharacter *ch) {
	//if they are switched add the command to the
	//npc they are switched to.
	if (ch->IsSwitched()) {
		ch->m_pSwitchedTo->AddCommand(ch->CurrentCommand);
		return;
	}

	CString Com(ch->CurrentCommand.GetWordAfter(0));
	sComClass *pCmd;

	if (!m_pCmdTable->Lookup(Com, pCmd)) {
		const CMsg *pMsg;
		if ((pMsg = ch->m_MsgManager.IsSocial(Com))) {
			ch->RemoveAffect(CCharacter::AFFECT_HIDING);
			CString strTarget(ch->CurrentCommand.GetWordAfter(1, true));
			if (!pMsg->m_pMsg[TARGET_TO_SELF_SOCIAL].IsEmpty() && !strTarget.IsEmpty()) {
				int nDot, nNum = 1;
				if ((nDot = strTarget.Find('.')) != -1) {
					nNum = strTarget.Left(nDot).MakeInt();
					strTarget = strTarget.Right(nDot);
				}
				CCharacter *pTarget = ch->m_pInRoom->FindCharacterByAlias(strTarget, nNum, ch);
				if (!pTarget) {
					ch->SendToChar("Who?\r\n");
				} else if (pTarget == ch) {
					ch->m_MsgManager.SocialMsg(pMsg, ch);
				} else {
					ch->m_MsgManager.SocialMsg(pMsg, ch, pTarget);
				}
			} else {
				ch->m_MsgManager.SocialMsg(pMsg, ch);
			}
		} else {
			ch->SendToChar("Huh?\r\n");
		}
		ch->EndCurrentCommand();
		return;
	}
	//are they high enough level?
	if (ch->GetLevel() < pCmd->MinLevel) {
		ch->SendToChar("Huh?\r\n");
		ch->EndCurrentCommand();
		return;
	}
	//are they in the proper position
	if (ch->InPosition(pCmd->m_nPositionCantBeIn)) {
		if ((ch->InPosition(POS_DEAD)) && (POS_DEAD & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("Em hrm YOUR DEAD\r\n");
		else if (ch->InPosition(POS_KOED) && (POS_KOED & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("Come back to the world of consious thought, and try it.\r\n");
		else if (ch->InPosition(POS_INCAP) && (POS_INCAP & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("You are in a pretty bad shape, unable to do anything!\r\n");
		else if (ch->InPosition(POS_MORTALED) && (POS_MORTALED & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("You are in a pretty bad shape, unable to do anything!\r\n");
		else if (ch->InPosition(POS_STUNNED) && (POS_STUNNED & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("All you can do right now is think about the stars!\r\n");
		else if (ch->InPosition(POS_SLEEPING) && (POS_SLEEPING & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("In your dreams, or what?\r\n");
		else if (ch->InPosition(POS_RESTING) && (POS_RESTING & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("Nah... You feel too relaxed to do that..\r\n");
		else if (ch->InPosition(POS_SITTING) && (POS_SITTING & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("Get off your ass and try it?\r\n");
		else if (ch->InPosition(POS_STANDING) && (POS_STANDING & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("You can't do that standing!\r\n");
		else if (ch->InPosition(POS_RECLINING) && (POS_RECLINING & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("How are you going to do that lying down?\r\n");
		else if (ch->InPosition(POS_FIGHTING) && (POS_FIGHTING & pCmd->m_nPositionCantBeIn))
			ch->SendToChar("Em... You're fighting for your life!\r\n");
		else if (ch->InPosition(POS_PREP) && (POS_PREP & pCmd->m_nPositionCantBeIn)) {
			ch->m_nPosition &= ~POS_PREP;
			ch->SendToChar("You stop preparing your spells.\r\n");
			(this->*pCmd->CommandFnc)(ch);
		}
	} else {
		if ((pCmd->m_nAffectsToRemove & REMOVE_HIDE) && ch->IsAffectedBy(CCharacter::AFFECT_HIDING)) {
			ch->RemoveAffect(CCharacter::AFFECT_HIDING);
		}
		if ((pCmd->m_nAffectsToRemove & REMOVE_MEDITATE) && ch->IsAffectedBy(CCharacter::AFFECT_MEDITATE)) {
			ch->RemoveAffect(CCharacter::AFFECT_MEDITATE);
		}
		if ((pCmd->m_nAffectsToRemove & REMOVE_INVIS)
				&& (ch->IsAffectedBy(CCharacter::AFFECT_INVISIBLE) || ch->IsAffectedBy(CCharacter::AFFECT_CAMOUFLAGE)
						|| ch->IsAffectedBy(CCharacter::AFFECT_TRANSPARENT_FORM))) {
			ch->RemoveAffect(CCharacter::AFFECT_INVISIBLE);
			ch->SendToChar("You snap to visibility.\r\n");
			ch->GetRoom()->SendToRoom("%s snaps into visibility.\r\n", ch);
		}
		if ((pCmd->m_nAffectsToRemove & REMOVE_CAMP) && (ch->IsAffectedBy(CCharacter::AFFECT_SET_UP_CAMP))) {
			ch->RemoveAffect(CCharacter::AFFECT_SET_UP_CAMP, false);
			ch->SendToChar("You abandon your camp grounds.\r\n");
		}
		(this->*pCmd->CommandFnc)(ch);
	}
	if (ch->IsAffectedBy(CCharacter::AFFECT_CASTING)) {
		ch->SendPrompt();
		return;
	}
	ch->EndCurrentCommand();
}

void CInterp::Wake(CCharacter *pCh) {
	CCharacter *pTarget = pCh->GetTarget(1, false);
	CString strToChar;
	if (pTarget != NULL && pTarget != pCh) {
		if (pTarget->InPosition(POS_SLEEPING)) {
			pTarget->RemovePosition(POS_SLEEPING);
			strToChar.Format("%s wakes you up.\r\n", pCh->GetRaceOrName(pTarget).cptr());
			pTarget->SendToChar(strToChar);
			strToChar.Format("You wake up %s.\r\n", pTarget->GetRaceOrName(pCh).cptr());
			pCh->SendToChar(strToChar);
		} else {
			pCh->SendToChar("Who is sleeping?\r\n");
		}
	} else {
		if (pCh->InPosition(POS_SLEEPING)) {
			pCh->RemovePosition(POS_SLEEPING);
			pCh->SendToChar("You wake up.\r\n");
		} else {
			pCh->SendToChar("Your not sleeping!\r\n");
		}
	}
}

//////////////////////////////////////////////////////
//	Vnum
//	Gives all the obj or mob virtual numbers for a given
//	alias
//	written: 4/3/98
///////////////////////////////////////////////////////

void CInterp::Vnum(CCharacter *ch) {
	CString ObjorMob(ch->CurrentCommand.GetWordAfter(1));
	CString strToChar;
	if (ObjorMob.Compare("obj")) {
		m_pGame->m_pObjectManager->GetVnums(strToChar, ch->CurrentCommand.GetWordAfter(2));
		if (strToChar.IsEmpty()) {
			ch->SendToChar("Huh?\r\n");
		} else {
			ch->SendToChar(strToChar);
		}
	} else if (ObjorMob.Compare("mob")) {
		m_pGame->m_pMobManager->GetVnums(strToChar, ch->CurrentCommand.GetWordAfter(2));
		if (strToChar.IsEmpty()) {
			ch->SendToChar("Huh?\r\n");
		} else {
			ch->SendToChar(strToChar);
		}
	} else {
		ch->SendToChar("vnum {obj | mob} alias\r\n");
	}
}

////////////////////////////////////////////////
//Transport
//	Transports a character from one room to another
// written 4/3/98
//////////////////////////////////////////////

void CInterp::Transport(CCharacter * ch) {
	CString strWho(ch->CurrentCommand.GetWordAfter(1));
	CString strToRoom(ch->CurrentCommand.GetWordAfter(2));

	if (strWho.IsEmpty()) {
		ch->SendToChar("Send who?\r\nUsage transport <name> <vnum>\r\n");
	} else if (strToRoom.IsEmpty()) {
		ch->SendToChar("Transport them to where?\r\n Usage transport <name> <vnum>\r\n");
	} else {
		CCharacter *pTarget = GVM.FindCharacter(strWho, ch);
		CRoom *pRealRoom = CWorld::FindRoom(strToRoom.MakeLong());
		if (pRealRoom == NULL && strToRoom.Compare("here")) {
			pRealRoom = ch->GetRoom();
		}

		if (!pTarget) {
			strWho.Format("%s is not currently playing!\r\n", strWho.cptr());
			ch->SendToChar(strWho);
		} else if (!pRealRoom) {
			ch->SendToChar("That room does not exist!\r\n");
		} else {
			CString strToGod, str;
			strToGod.Format("You transport %s to room %u\r\n", pTarget->GetName().cptr(), pRealRoom->GetVnum());
			ch->SendToChar(strToGod);
			pTarget->m_pInRoom->Remove(pTarget);
			if (!pTarget->Prefers(CCharacterAttributes::PREFERS_GOD_INVIS) && pTarget->IsGod()) {
				pTarget->m_pInRoom->SendToRoom("%s looks less solid and disappears!\r\n", pTarget);
			}
			pTarget->SendToChar("POOF!\r\n");
			pTarget->m_pInRoom = pRealRoom;
			pTarget->m_pInRoom->Add(pTarget);
			//Interpetor is friend of CCharacter
			pTarget->SendToChar(pTarget->GetRoom()->DoRoomLook(pTarget));
			if (!pTarget->Prefers(CCharacterAttributes::PREFERS_GOD_INVIS) && pTarget->IsGod()) {
				if (pTarget->m_strPoofIn.IsEmpty()) {
					pTarget->GetRoom()->SendToRoom("%s arrives in a puff of smoke.\r\n", pTarget);
				} else {
					pTarget->GetRoom()->SendToRoom(pTarget->m_strPoofIn, pTarget);
				}
			}
		}
	}
}

/////////////////////////////////////////////
// Create
//	creates a mob or an obj in the room of the
//	creator
//	written by: Demetrius Comes
//	4/12/98
//////////////////////////////////////////////

void CInterp::Create(CCharacter * ch) {
	const char *str = "&WYou make a magical gesture and ...&n\r\n";
	CString strToChar;
	CString strMobOrObj(ch->CurrentCommand.GetWordAfter(1));
	CString strThird(ch->CurrentCommand.GetWordAfter(2));
	int nAmount;

	if ((nAmount = strMobOrObj.MakeInt()) != -1) {
		if (strThird.Compare("plat")) {
			ch->GetRoom()->GetCash().AddPlat(nAmount);
			strToChar.Format("%s %d platinum pieces appear.\r\n", str, nAmount);
		} else if (strThird.Compare("gold")) {
			ch->GetRoom()->GetCash().AddGold(nAmount);
			strToChar.Format("%s %d gold pieces appear.\r\n", str, nAmount);
		} else if (strThird.Compare("silver")) {
			ch->GetRoom()->GetCash().AddSilver(nAmount);
			strToChar.Format("%s %d sliver pieces appear.\r\n", str, nAmount);
		} else if (strThird.Compare("coppper")) {
			ch->GetRoom()->GetCash().AddCopper(nAmount);
			strToChar.Format("%s %d copper pieces appear.\r\n", str, nAmount);
		} else {
			strToChar = "syntax: create <plat gold silver copper> amount\r\n";
		}
	} else if (strMobOrObj.Compare("mob")) {
		CString strVNum(ch->CurrentCommand.GetWordAfter(2));
		long lVnum = strVNum.MakeLong();
		CCharacter *pMob;
		if ((pMob = m_pGame->m_pMobManager->Create(lVnum, ch->GetRoom())) == NULL) {
			strToChar = "Mob could not be created. Do you have the Vnum correct?\r\n";
		} else {
			strToChar.Format("%s %s apprears.\r\n", str, pMob->GetName().cptr());
		}
	} else if (strMobOrObj.Compare("obj")) {
		CString strVNum(ch->CurrentCommand.GetWordAfter(2));
		obj_vnum nVnum = strVNum.MakeInt();
		CObject *pObj;
		if ((pObj = m_pGame->m_pObjectManager->Create(nVnum, ch->GetRoom(), -1)) == NULL) {
			strToChar = "Object could not be created.  Do you have the Vnum correct?\r\n";
		} else {
			strToChar.Format("%s %s appears.\r\n", str, pObj->GetObjName(ch));
		}
	} else {
		strToChar = "syntax:  create {mob | obj} vnum\r\n";
	}
	ch->SendToChar(strToChar);
	ch->m_pInRoom->SendToRoom("%s makes a magical gesture\r\n", ch);
}

/////////////////////////////////////////////////
//	Advances a character levels
//	written by: Demetrius comes
//	4/18/98
//////////////////////////////////////////////////

void CInterp::AdvanceLevel(CCharacter *ch) {
	CString strChar(ch->CurrentCommand.GetWordAfter(1));

	if (strChar.IsEmpty()) {
		ch->SendToChar("syntax: advance <character name> <level to be>\r\n");
		return;
	} else {
		CCharacter *pTarget = GVM.FindCharacter(strChar, ch);

		if (!pTarget) {
			ch->SendToChar("Who????\r\n");
			return;
		}

		CString strNumber(ch->CurrentCommand.GetWordAfter(2));

		int nLevel = strNumber.IsEmpty() ? 0 : strNumber.MakeInt();

		if (nLevel > LVL_GREATER_GOD) {
			ch->SendToChar("There isn't that many levels!\n");
		} else if (nLevel < 1) {
			ch->SendToChar("You can't delevel them that far!\r\n");
		} else {
			CString strToChar;
			if (nLevel == pTarget->m_nLevel) {
				strToChar.Format("%s is already %d.", pTarget->GetName().cptr(), pTarget->m_nLevel);
				ch->SendToChar(strToChar);
			} else if (nLevel < pTarget->m_nLevel) {
				for (int i = pTarget->m_nLevel; i > nLevel; i--) {
					pTarget->AdvanceLevel(false);
				}
				pTarget->m_nExperience = pTarget->GetExp(nLevel);
				strToChar.Format("You lower %s's level to %d.\r\n", pTarget->GetName().cptr(), pTarget->GetLevel());
				ch->SendToChar(strToChar);
			} else {
				for (int i = pTarget->m_nLevel; i < nLevel; i++) {
					pTarget->AdvanceLevel(true);
				}
				pTarget->m_nExperience = pTarget->GetExp(nLevel);
				strToChar.Format("You increase %s's level to %d.\r\n", pTarget->GetName().cptr(), pTarget->GetLevel());
				ch->SendToChar(strToChar);
			}
		}
	}
}

///////////////////////////////////
//	VStat 
//	Gives god information on a mob or obj
//	written by: Demetrius Comes
//	4/27/98
///////////////////////////////////

void CInterp::Vstat(CCharacter * ch) {
	static CString strError("Syntax is vstat {obj | mob} vnum\r\n");
	CString ObjOrMob(ch->CurrentCommand.GetWordAfter(1));
	CString strNum(ch->CurrentCommand.GetWordAfter(2));
	CString ToChar;
	long lVnum;
	static CString MOB("mob");
	static CString OBJ("obj");

	if (ObjOrMob == MOB) {
		if (strNum.IsEmpty() || (lVnum = strNum.MakeLong()) == -1) {
			ch->SendToChar(strError);
			return;
		}
		m_pGame->m_pMobManager->VStat(ToChar, lVnum);
	} else if (ObjOrMob == OBJ) {
		if (strNum.IsEmpty() || (lVnum = strNum.MakeLong()) == -1) {
			ch->SendToChar(strError);
			return;
		}
		m_pGame->m_pObjectManager->VStat(ToChar, lVnum);
	}
	ch->SendToChar(ToChar);
}

////////////////////////////////////////
//  RoomStat
//	Allows a god to check all stats on a current room
//	Message to Room to build string then send it to character
//	written by: Demetrius Comes
//	5/3/98
////////////////////////////////////////

void CInterp::RoomStat(CCharacter * pCh) {
	CString strToChar;
	pCh->m_pInRoom->BuildRoomStat(strToChar);
	pCh->SendToChar(strToChar);
}

////////////////////////////////////
//	CreateObj
//	public function so that MinorCreation will
//	work!
////////////////////////////////////////

CObject *CInterp::CreateObj(obj_vnum nVnum, CRoom *pRoom) const {
	assert(m_pGame);
	assert(m_pGame->m_pObjectManager);

	return m_pGame->m_pObjectManager->Create(nVnum, pRoom, -1);
}

//////////////////////
//	CreateObj
//	Calls ObjectManager::CreateObj
//
//	written by: demetrius Comes

CObject * CInterp::CreateObj(obj_vnum nObjVnum, CCharacter * pCh) const {
	return m_pGame->m_pObjectManager->Create(nObjVnum, pCh);
}

//////////////////////
//	CreatePortal
//	Calls ObjectManager::CreatePortal
//
//	written by: John Comes 1-21-99

void CInterp::CreatePortal(CRoom *pTargetRoom, CRoom *pCasterRoom, short nAffect, int nPulses) const {
	assert(m_pGame);
	assert(m_pGame->m_pObjectManager);

	m_pGame->m_pObjectManager->CreatePortal(pTargetRoom, pCasterRoom, nAffect, nPulses);
}

//////////////////////
//	Create Fireball
//	Calls ObjectManager::CreateFireball
//
//	written by: John Comes 2-11-99

void CInterp::CreateFireball(CRoom *pTargetRoom, int nPulses, int nDam) const {
	assert(m_pGame);
	assert(m_pGame->m_pObjectManager);
	m_pGame->m_pObjectManager->CreateFireball(pTargetRoom, nPulses, nDam);
}

//////////////////////////
//	CreateMob
//	So that spells can create mobs
//	written by: John comes
/////////////////////////

CCharacter * CInterp::CreateMob(int MobVnum, CRoom *pPutInRoom) const {
	assert(m_pGame);
	assert(m_pGame->m_pMobManager);
	return m_pGame->m_pMobManager->Create(MobVnum, pPutInRoom);
}

//////////////////////////
//	CreateMob
//	So that spells can create mobs
//	written by: John comes
/////////////////////////

CCharacter * CInterp::CreateMob(int MobVnum, CRoom *pPutInRoom, int nHp, int nLevel) const {
	assert(m_pGame);
	assert(m_pGame->m_pMobManager);
	return m_pGame->m_pMobManager->Create(MobVnum, pPutInRoom, nHp, nLevel);
}

/////////////////////
//	Create Undead
//	creats an undead
/////////////////////

CCharacter *CInterp::CreateUndead(const sUndeadInfo *pUndeadInfo, CRoom *pRoom) const {
	assert(m_pGame);
	assert(m_pGame->m_pMobManager);
	return m_pGame->m_pMobManager->CreateUndead(pUndeadInfo, pRoom);
}

/////////////////////
//	Create Wall
//	creats a Wall
/////////////////////

CCharacter *CInterp::CreateWall(const sWallInfo *pWallInfo, CRoom *pRoom) const {
	assert(m_pGame);
	assert(m_pGame->m_pMobManager);
	return m_pGame->m_pMobManager->CreateWall(pWallInfo, pRoom);
}

///////////////////////////
//	Removes fighter to fight linklist
/////////////////////////

void CInterp::RemoveFighter(CCharacter * pFighter) const {
	m_pGame->m_FightLL.Remove(pFighter);
}

//////////////////////
//	Restore all or just a character
/////////////////////

void CInterp::Restore(CCharacter * pCh) {
	CString strChar(pCh->CurrentCommand.GetWordAfter(1).cptr());
	CString strToChar;
	strToChar.Format("&BYou are restored by %s.&n\r\n", pCh->GetName().cptr());

	strChar.SkipSpaces();
	CCharacter *pTarget;
	if (strChar.IsEmpty()) {
		pCh->SendToChar("Restore who?\r\n");
	} else if (strChar.Compare("all")) {
		POSITION pos = GVM.PlayerLL.GetStartPosition();
		while (pos) {
			pTarget = GVM.PlayerLL.GetNext(pos);
			if (!pTarget->IsNPC()) {
				pTarget->m_nCurrentHpts = pTarget->m_nMaxHpts;
				pTarget->m_nManaPts += pTarget->ManaCanRegain(true);
				pTarget->m_nMovePts = pTarget->m_nMaxMovePts;
				pTarget->UpDatePosition();
				pTarget->SendToChar(strToChar);
				pTarget->RemoveAffect(CCharacter::AFFECT_BLINDNESS);
			}
		}
	} else {
		pTarget = GVM.FindCharacter(strChar, pCh);

		if (pTarget) {
			pTarget->m_nCurrentHpts = pTarget->m_nMaxHpts;
			pTarget->m_nManaPts += pTarget->ManaCanRegain(true);
			pTarget->m_nMovePts = pTarget->m_nMaxMovePts;
			pTarget->UpDatePosition();
			pTarget->SendToChar(strToChar);
			strToChar.Format("You restore %s.\r\n", pTarget->GetName().cptr());
			pTarget->RemoveAffect(CCharacter::AFFECT_BLINDNESS);
		} else {
			pCh->SendToChar("Restore who?\r\n");
		}
	}
}

///////////////////////////
//	KillChar
//	Calls CGame::KillChar
/////////////////////////

void CInterp::KillChar(CCharacter * ch) const {
	ch->AssertFighting(NULL);
	m_pGame->KillChar(ch);
}

//////////////////////
//	Get Object manager

const CObjectManager *CInterp::GetObjManager() const {
	return m_pGame->m_pObjectManager;
}

///////////////////////////
//	Shows the entire virutal world map
//////////////////////////

void CInterp::Map(CCharacter * pCh) {
	CString strWorld(CWorld::GetMap());
	//it will crash if you set it to colorize on
	//send to char
	pCh->SendToChar(strWorld, false);
}

///////////////////////
//	Allows one char to follow another
//
//////////////////////

void CInterp::Follow(CCharacter * pCh) {
	CCharacter *pTarget = pCh->GetTarget(1, false);
	if (!pTarget) {
		pCh->SendToChar("Follow who?\r\n");
		return;
	}
	//Get the person we are currently following
	CCharacter *pOldFollowie = pCh->m_Follow.m_pCharacterBeingFollowed;
	CString strToChar;
	//if we are going to follow self
	if (pTarget == pCh) {
		//if we are currently following someone
		if (pOldFollowie) {
			//Remove pCh from the person we are following
			pOldFollowie->m_Follow.Remove(pCh);
			pCh->SendToChar("You follow yourself.\r\n");
		} else {
			//we weren't following anyone to begin with!
			pCh->SendToChar("Your not following anyone!\r\n");
		}
	}   //are we trying to follow the same person twice
	else if (pTarget == pOldFollowie) {
		strToChar.Format("You are already following %s.\r\n", pOldFollowie->GetRaceOrName(pCh).cptr());
		pCh->SendToChar(strToChar);
	}   //we are trying to following someone we are not currently
		//following and not ourself
	else {
		//if we were following someont
		if (pOldFollowie) {
			//Remove pCh from the person we are following
			pOldFollowie->m_Follow.m_Followers.Remove(pCh);
		}
		//add pCh to pTargets followers
		pTarget->m_Follow.m_Followers.Add(pCh);
		//Set person being followed to pTarget
		pCh->m_Follow.SetCharToFollow(pTarget);
		//send messages
		strToChar.Format("You start following %s\r\n", pTarget->GetRaceOrName(pCh).cptr());
		pCh->SendToChar(strToChar);
		strToChar.Format("%s starts following you.\r\n", pCh->GetRaceOrName(pTarget).cptr());
		pTarget->SendToChar(strToChar);
		strToChar = "%s starts following %s.\r\n";
		pCh->GetRoom()->SendToRoom(strToChar, pCh, pTarget);
	}
}

//////////////////////////////
//	Consent
//	Find the character they are trying to consent
//	in the global playerLL. If we have a character
//  determine if they are consenting themselves.  If so
//	revoke consent from the former character they were consenting
//	if they are consenting someone else revoke consent from
//	former and give it to new
//	written by: Demetrius Comes
//////////////////////////////

void CInterp::Consent(CCharacter * pCh) {
	CString strTarget(pCh->CurrentCommand.GetWordAfter(1));

	CCharacter *pTarget = GVM.FindCharacter(strTarget, pCh);
	if (!pTarget) {
		pCh->SendToChar("Consent who?\r\n");
	} else if (pTarget->IsNPC()) {
		pCh->SendToChar("You can't consent NPC's!\r\n");
	} else {
		CString strToChar;
		//are they consenting themselves?
		if (pTarget == pCh) {
			//are they already consenting someone
			if (pCh->m_Consent.m_pCharYouConsented) {
				//remove them from the character consent list
				//and set consented pointer to NULL
				pCh->m_Consent.GetConsented()->m_Consent.m_CharsThatConsentedYou.Remove(pCh);
				strToChar.Format("You no longer consent %s.\r\n",
						pCh->m_Consent.m_pCharYouConsented->GetRaceOrName(pCh).cptr());
				pCh->SendToChar(strToChar);
				pCh->m_Consent.m_pCharYouConsented = NULL;
				return;
			}
			pCh->SendToChar("You still consent no-one.\r\n");
		}		//they are consent someone besides themselves
		else {
			//if they are trying to consent the same person
			if (pCh->m_Consent.m_pCharYouConsented == pTarget) {
				strToChar.Format("You have already given %s your consent.\r\n", pTarget->GetRaceOrName(pCh).cptr());
				pCh->SendToChar(strToChar);
				return;
			}		//if they are consenting someone different
					//remove them from formers consent list
			else if (pCh->m_Consent.m_pCharYouConsented) {
				pCh->m_Consent.GetConsented()->m_Consent.m_CharsThatConsentedYou.Remove(pCh);
			}
			//Set consented pointer
			pCh->m_Consent.m_pCharYouConsented = pTarget;
			//add them to consent list
			pTarget->m_Consent.m_CharsThatConsentedYou.Add(pCh);
			strToChar.Format("You now give your consent to %s.\r\n", pTarget->GetRaceOrName(pCh).cptr());
			pCh->SendToChar(strToChar);
			strToChar.Format("You now have %s's consent.\r\n", pCh->GetRaceOrName(pTarget).cptr());
			pTarget->SendToChar(strToChar);
		}
	}
}

/////////////////////////////
//	Group another character
//	The sGroup class handles everything
//	even if they are consented or not
//	written by: Demetrius Comes
//	8-25-98
///////////////////////////

void CInterp::Group(CCharacter * pGrouper) {
	CString strTarget(pGrouper->CurrentCommand.GetWordAfter(1));

	if (strTarget.IsEmpty()) {
		CString strToChar;
		pGrouper->m_Group.BuildGroupString(strToChar);
		pGrouper->SendToChar(strToChar);
		return;
	}

	CCharacter *pGroupie = pGrouper->GetTarget(1, false);
	if (!pGroupie) {
		pGrouper->SendToChar("Group who?\r\n");
	} else {
		pGrouper->m_Group.AddToGroup(pGrouper, pGroupie);
	}
}

/////////////////////////////////
//	GroupSay
//	Send a Message your group
//	written by: Demetrius Comes

void CInterp::GroupSay(CCharacter * pCh) {
	if (pCh->m_Group.IsGrouped()) {
		CString strGroupSay(pCh->CurrentCommand.GetWordAfter(1, true));
		if (strGroupSay.IsEmpty()) {
			pCh->SendToChar(CString("&YWhat do you wish to tell your group?&n\r\n"));
		} else {
			CString strToChar;
			strToChar.Format("&c%s '&C%s&c'&n\r\n", "You group say ", strGroupSay.cptr());
			pCh->SendToChar(strToChar);
			strToChar.Format("&c%s tells the group '&C%s&c'&n\r\n", "%s", strGroupSay.cptr());
			pCh->m_Group.SendToGroup(strToChar, pCh);
		}
	} else {
		pCh->SendToChar("But you are not a member of a group.\r\n");
	}
}

//////////////////////////////
//	Stop Fighting
//	

void CInterp::Disengage(CCharacter * pCh) {
	if (pCh->IsFighting()) {
		pCh->AssertFighting(NULL);
		m_pGame->m_FightLL.Remove(pCh);
		pCh->SendToChar("You stop fighting.\r\n");
		pCh->LagChar((long) ((.75) * CMudTime::PULSES_PER_DO_VIOLENCE));
	} else {
		pCh->SendToChar("You not fighting anyone.\r\n");
	}
}

/////////////////////////////
//	BuildHelp
//	Run the
//	sComClass Map and builds all commands
//	written by: Demetrius Comes
//	8-27-98
//  3-6-99
//	added other help
///////////////////////////

void CInterp::BuildHelp() {
	m_strCommandHelp = "Player commands are:\r\n";
	m_strGodHelp = "God Commands are:\r\n";
	BuildHelpOnCommand(m_strGodHelp, true);
	BuildHelpOnCommand(m_strCommandHelp, false);

	//read and parse help file
	m_HelpFile.open(m_strHelpFileName, std::ios_base::in);
	if (!m_HelpFile && !m_HelpFile.is_open()) {
		ErrorLog<< "There doesn't seem to be a help file.  It is suppose to be located at " << m_strHelpFileName << endl;
	} else {
		MudLog << "booting help system" << endl;
		CString str, tmp;
		char strRead[MAX_LONG_STR];
		long lFilePos;
		sHelp *pHelpInfo = NULL;
		while (m_HelpFile.good()) {
			do {
				lFilePos = m_HelpFile.tellg();
				m_HelpFile >> str;
				str.SkipSpaces();
			}while (str.IsEmpty() && m_HelpFile.good());
			if (m_HelpFile.good()) {
				pHelpInfo = new sHelp();
				pHelpInfo->m_lStartPos = lFilePos;
				do {
					lFilePos = m_HelpFile.tellg();
					m_HelpFile.getline(strRead, MAX_LONG_STR);
					tmp = strRead;
					tmp.SkipSpaces();
				}while (tmp[0] != HELP_FILE_DELIM && m_HelpFile.good());
				pHelpInfo->m_lStopPos = lFilePos;
				m_HelpTable.Add(str, pHelpInfo);
			}
		}
	}
}

////////////////////
//	Help
//	Send To character what they want help on
//	
///////////////////

void CInterp::Help(CCharacter *pCh) {
	CString strHelp(pCh->CurrentCommand.GetWordAfter(1, true));

	if (strHelp.IsEmpty()) {
		pCh->SendToChar(m_strBasicHelp);
	} else if (strHelp.Compare("commands")) {
		if (pCh->IsGod()) {
			CString str;
			str.Format("%s\r\n%s\r\n", m_strGodHelp.cptr(), m_strCommandHelp.cptr());
			pCh->SendCharPage(str);
		} else {
			pCh->SendCharPage(m_strCommandHelp);
		}
	} else {
		CString strToChar;
		char tmp[MAX_LONG_STR] = { '\0' };
		sHelp *pHelpInfo = NULL;
		m_HelpFile.clear();
		if (m_HelpTable.Lookup(strHelp, pHelpInfo)) {
			m_HelpFile.seekg(pHelpInfo->m_lStartPos, std::ios_base::beg);
			do {
				m_HelpFile.getline(tmp, MAX_LONG_STR);
				strToChar.Format("%s%s\r\n", strToChar.cptr(), tmp);
			} while ((m_HelpFile.tellg() < pHelpInfo->m_lStopPos) && m_HelpFile.good());
			strToChar += "\r\n";
			pCh->SendCharPage(strToChar);
		} else {
			strToChar.Format("There is no help on %s.\r\n", strHelp.cptr());
			pCh->SendToChar(strToChar);
		}
	}
}

///////////////////////
//	allows players to report bugs
/////////////////////

void CInterp::Bug(CCharacter * pCh) {
	CString strBug(pCh->CurrentCommand.GetWordAfter(1, true));

	if (strBug.IsEmpty()) {
		pCh->SendToChar("What bug do you want to report.\r\n");
	} else {
		CString strToChar;
		strToChar.Format("The following bug will be reviewed\r\n'%s'\r\n", strBug.cptr());
		pCh->SendToChar(strToChar);
		strToChar.Format("%s reports the following bug %s", pCh->GetName().cptr(), strBug.cptr());
		BugFile << strToChar << endl;
	}
}

/////////////////////////
//	Allows a god to say something
//	to eveyone in the game
/////////////////////////

void CInterp::Echo(CCharacter * pCh) {
	CString strToAll(pCh->CurrentCommand.GetWordAfter(1, true));
	if (strToAll.IsEmpty()) {
		pCh->SendToChar("Don't be shy speak up.\r\n");
	} else {
		CString strToChar;
		strToChar.Format("%s &Bcalls down from the heavens &W'%s'&n\r\n", pCh->GetName().cptr(), strToAll.cptr());
		GVM.SendToAll(strToChar, pCh);
		strToChar.Format("&WYou tell all mortals '&B%s&W'&n\r\n", strToAll.cptr());
		pCh->SendToChar(strToChar);
	}
}

///////////////////////////////////
//	Allow a god to set any attribute
//	of a player
//	set <player> <skill/spell> <value>

void CInterp::Set(CCharacter * pCh) {
	static const char *pTick = "'";
	CString strChar(pCh->CurrentCommand.GetWordAfter(1));
	if (!strChar.IsEmpty()) {
		CCharacter *pTarget = GVM.FindCharacter(strChar, pCh);
		if (!pTarget) {
			pCh->SendToChar("Set who?\r\n");
			return;
		}
		int first = pCh->CurrentCommand.Find(pTick[0]);
		int second = pCh->CurrentCommand.Find(pTick[0], 2);
		if (first == -1 || second == -1) {
			pCh->SendToChar("Skills must be inclosed in single quotes!\r\n");
			return;
		}
		CString strSkillSpell(pCh->CurrentCommand(first + 1, second - 1));
		CString strValue(pCh->CurrentCommand.Right(second + 1));
		strValue.SkipSpaces();
		short nValue = strValue.MakeInt();
		if (nValue != -1) {
			short nSkillSpell = CCharacterAttributes::FindSkill(strSkillSpell);
			if (nSkillSpell != -1) {
				CString strMsg;
				pTarget->m_Skills[nSkillSpell] = (skill_type) (nValue > 100) ? 100 : nValue;
				strMsg.Format("You set %s's skill in %s to %d.\r\n", pTarget->GetName().cptr(),
						CCharacterAttributes::FindSkill(nSkillSpell).cptr(), nValue);
				pCh->SendToChar(strMsg);
				strMsg.Format("You feel your skill in %s change.\r\n",
						CCharacterAttributes::FindSkill(nSkillSpell).cptr());
				pTarget->SendToChar(strMsg);
				return;
			} else if (pTarget->SetSpell(pCh, strSkillSpell, (skill_type) RANGE(nValue, 100, 0))) {
				return;
			}
		}
	}
	pCh->SendToChar("syntax:  set <player name> '<skill/spell>' <value>\r\n");
}

////////////////////////////////////////
//	Save
//	Allows the character to save to disk
///////////////////////////////////////

void CInterp::Save(CCharacter * pCh) {
	if (!pCh->IsNPC()) {
		pCh->Save();
		pCh->SendToChar("Saving...\r\n");
	} else {
		pCh->SendToChar("You can't save an NPC!\r\n");
	}
}

////////////////////////////
//	Examine
//	Sends Examine string to character

void CInterp::Examine(CCharacter * pCh) {
	CString strObj(pCh->CurrentCommand.GetWordAfter(1));

	if (strObj.IsEmpty()) {
		pCh->SendToChar("Examine what?\r\n");
		return;
	}

	int nDot, nNum = 1;
	if ((nDot = strObj.Find('.')) != -1) {
		nNum = strObj.Left(nDot).MakeInt();
		strObj = strObj.Right(nDot);
	}
	CObject *pObj;
	CString strToChar;
	if ((pObj = pCh->GetRoom()->FindObjectInRoom(strObj, nNum, pCh))) {
		pObj->Examine(pCh, strToChar);
		pCh->SendToChar(strToChar);
	} else if ((pObj = pCh->FindInInventory(strObj, nNum))) {
		pObj->Examine(pCh, strToChar);
		pCh->SendToChar(strToChar);
	} else {
		pCh->SendToChar("Examine what?\r\n");
	}
}

///////////////////////////
//	order
//	Allows you to order your followers arrond
// written by: Demetrius Comes
///////////////////////////

void CInterp::Order(CCharacter * pCh) {
	CString strTarget(pCh->CurrentCommand.GetWordAfter(1));
	CCharacter *pTarget;
	CString strCommand, strToRoom, strToChar;
	int nDot, nNum = 1;
	if ((nDot = strTarget.Find('.')) != -1) {
		nNum = strTarget.Left(nDot).MakeInt();
		strTarget = strTarget.Right(nDot);
	}

	if (strTarget.Compare("followers")) {
		strCommand = pCh->CurrentCommand.GetWordAfter(2, true);
		if (pCh->m_Follow.HasFollowers()) {
			POSITION pos = pCh->m_Follow.m_Followers.GetStartPosition();
			while (pos) {
				pTarget = pCh->m_Follow.m_Followers.GetNext(pos);
				if (pCh->m_Master.IsMasterOf(pTarget) && pCh->GetRoom()->DoesContain(pTarget)) {
					strToRoom = "%s gives an order to %s.\r\n";
					pCh->GetRoom()->SendToRoom(strToRoom, pCh, pTarget);
					strToChar.Format("You give an order to %s.\r\n", pTarget->GetRaceOrName(pCh).cptr());
					pCh->SendToChar(strToChar);
					pTarget->AddCommand(strCommand);
				}
			}
		} else {
			pCh->SendToChar("You have no followers.\r\n");
		}
	} else if (strTarget.IsEmpty() || (!(pTarget = pCh->GetRoom()->FindCharacterByAlias(strTarget, nNum, pCh)))) {
		pCh->SendToChar("Order who?\r\n");
	} else if (pCh->m_Master.IsMasterOf(pTarget)) {
		strCommand = pCh->CurrentCommand.GetWordAfter(2, true);
		pTarget->AddCommand(strCommand);
		strToRoom = "%s gives an order to %s.\r\n";
		pCh->GetRoom()->SendToRoom(strToRoom, pCh, pTarget);
		strToChar.Format("You give an order to %s.\r\n", pTarget->GetRaceOrName(pCh).cptr());
		pCh->SendToChar(strToChar);
	} else {
		pCh->SendToChar("You must be thier master!\r\n");
	}
}

//////////////////////////////////
//	DoWho
//	Lists all characters in game
//	Will need to be modified for togs, race war etc
//	John Comes 12-04-98
//	added god string etc 10-29-99
//////////////////////////////////

void CInterp::Who(CCharacter *pToChar) {
	short int nNumOfPlayers = 0;
	int nTotalMobs = 0;
	CCharacter *pCh = NULL;
	CString strGods("&RGods currently online:&n\r\n");
	CString strChars("&WPlayers Currently Online:&n\r\n\r\n");
	POSITION pos = GVM.PlayerLL.GetStartPosition();
	while (pos) {
		pCh = GVM.PlayerLL.GetNext(pos);
		if (!pCh->IsNPC()) {
			if ((pCh->IsGod() && pToChar->IsGod()) //god always see other gods
			|| (pCh->IsGod() && pCh->Prefers(CCharacterAttributes::PREFERS_GOD_ON_WHO))
					|| (!pCh->IsGod() && pToChar->CanSeeName(pCh) && pToChar->CanSeeChar(pCh)
							&& pToChar->CanSeeInRoom(pCh->GetRoom()) != CCharacter::SEES_NOTHING)) {
				if (pCh->IsGod()) {
					strGods.Format("%s[%s] %s %s (%s)\r\n", strGods.cptr(),
							pCh->GetLevel() <= LVL_LESSER_GOD ? "&GLesser God&n" : "&BGreater God&n",
							pCh->GetName().cptr(), pCh->GetTitle().cptr(), GVM.GetColorRaceName(pCh->GetRace()));
				} else {
					if (pToChar->IsGod()) {
						strChars.Format("%s[%2d %-17s] %s %s (%s) (%s)\r\n", strChars.cptr(), pCh->GetLevel(),
								GVM.GetColorClass(pCh->GetClass()), pCh->GetName().cptr(), pCh->GetTitle().cptr(),
								GVM.GetColorRaceName(pCh->GetRace()), pCh->GetRaceWarName());
					} else {
						strChars.Format("%s[%2d %-17s] %s %s (%s)\r\n", strChars.cptr(), pCh->GetLevel(),
								GVM.GetColorClass(pCh->GetClass()), pCh->GetName().cptr(), pCh->GetTitle().cptr(),
								GVM.GetColorRaceName(pCh->GetRace()));
					}
				}
			}
			//we give the total number of players if we can see them
			//or not
			nNumOfPlayers++;
		} else {
			nTotalMobs++;
		}
	}
	strChars.Format("%s&r\r\nTotal number of players: %d&n\r\n", strChars.cptr(), nNumOfPlayers);
	if (pToChar->IsGod()) {
		strChars.Format("%sTotal mobs: %d\r\n", strChars.cptr(), nTotalMobs);
	}
	strGods.Format("%s\r\n%s", strGods.cptr(), strChars.cptr());
	pToChar->SendCharPage(strGods);
	return;
}

///////////////////////////////////
//	Title
//	Sets title of anyone
//

void CInterp::Title(CCharacter * pCh) {
	CString strChar(pCh->CurrentCommand.GetWordAfter(1));
	if (!strChar.IsEmpty()) {
		CCharacter *pTarget = GVM.FindCharacter(strChar, pCh);
		if (!pTarget) {
			pCh->SendToChar("Title who?\r\n");
		} else {
			if (pTarget == pCh || pCh->IsGod()) {
				CString strValue(pCh->CurrentCommand.GetWordAfter(2, true));
				pTarget->SetTitle(strValue);
				if (pTarget == pCh) {
					strValue.Format("Your title has been changed to %s.\r\nDon't be stupid with this.\r\n",
							strValue.cptr());
					pTarget->SendToChar(strValue);
				} else {
					strValue.Format("You change %s's title to %s.\r\n", pTarget->GetName().cptr(), strValue.cptr());
					pCh->SendToChar(strValue);
				}
			} else {
				pCh->SendToChar("You can only set your own title.\r\n");
			}
		}
	}
}

///////////////////////////////////
//	Rent out a character

void CInterp::Rent(CCharacter *pCh) {
	if (!pCh->GetRoom()->IsAffectedBy(CRoom::ROOM_AFFECT_INN)) {
		pCh->SendToChar("You should go to an inn first.\r\n");
	} else if (pCh->IsAffectedBy(CCharacter::AFFECT_BLINDNESS)) {
		pCh->SendToChar("Your blind! You can't see a to find your room.\r\n");
	} else if (pCh->IsAffectedBy(CCharacter::AFFECT_MAJOR_PARA)) {
		pCh->SendToChar("Your paralyzed!\r\n");
	} else {
		pCh->m_pDescriptor->ChangeState(STATE_RENT);
		pCh->m_lLastRentRoom = pCh->GetRoom()->GetVnum();
		pCh->SendToChar("The inn keeper shows you to your room.\r\n");
		CString str;
		str.Format("The inn keeper shows %s to %s room.\r\n", "%s",
				(char *) CCharacterAttributes::GetPronoun(pCh->GetSex()));
		pCh->GetRoom()->SendToRoom(str, pCh);
	}
}

//////////////////////////////////////
//	Scan
//	

void CInterp::Scan(CCharacter *pCh) {
	CString strToChar;
	CRoom::Scan(pCh, strToChar);
	if (strToChar.IsEmpty()) {
		pCh->SendToChar("Nothing.\r\n");
	} else {
		pCh->SendToChar(strToChar);
	}
}

///////////////////////////////////
//	Allow a character to shout
//	Distance based on consituition

void CInterp::Shout(CCharacter * pCh) {
	CString strShout(pCh->CurrentCommand.GetWordAfter(1, true));
	if (strShout.IsEmpty()) {
		pCh->SendToChar("Shout what?\r\n");
	} else {
		CString str;
		str.Format("You shout '%s'\r\n", strShout.cptr());
		pCh->SendToChar(str);
		strShout.Format("%s shouts '%s'\r\n", "%s", strShout.cptr());
		CLinkList<CRoom> RoomLL;
		pCh->GetRoom()->DoShout(strShout, RoomLL, pCh, pCh->m_pAttributes->GetShoutDistance());
	}
}

///////////////////////////////////
//	Do Say
//	If I am affected by AFFECT_AUDIBLE_GLAMOUR
//	do a shout, not a say
//	John Comes 3/4/99
////////////////////////////////////

void CInterp::Say(CCharacter * pCh) {
	if (pCh->IsAffectedBy(CCharacter::AFFECT_AUDIBLE_GLAMOUR)) {
		Shout(pCh);
	} else {
		pCh->DoSay();
	}
}

///////////////////////////////////
//	Allows a god to spy on players
//	

void CInterp::Spy(CCharacter * pCh) {
	CString strCommand(pCh->CurrentCommand.GetWordAfter(1));
	if (strCommand.IsEmpty()) {
		pCh->SendToChar("syntax: spy <PC name> (if you spy the same person twice you stop spying).\r\n");
	} else if (strCommand.Compare("off")) {
		if (!pCh->IsSpying()) {
			pCh->SendToChar("You're not spying on anyone!\r\n");
		} else {
			CString str;
			str.Format("You stop spying on %s.\r\n", pCh->m_pSpyingOn->GetName().cptr());
			pCh->SendToChar(str);
			pCh->m_pSpyingOn->m_pSpiedby = NULL;
			pCh->m_pSpyingOn = NULL;
		}
	} else {
		CCharacter *pTarget = GVM.FindCharacter(strCommand, pCh);
		if (pTarget != NULL && pTarget != pCh) {
			if (!pTarget->IsNPC()) {
				CString strToChar;
				if (!pTarget->IsBeingSpied()) {
					pTarget->m_pSpiedby = pCh;
					pCh->m_pSpyingOn = pTarget;
					strToChar.Format("You will now spy on %s.\r\n", pTarget->GetName().cptr());
					pCh->SendToChar(strToChar);
				} else if (pTarget->m_pSpiedby == pCh) {
					pCh->SendToChar("You stop spying.\r\n");
					pTarget->m_pSpiedby = NULL;
					pCh->m_pSpyingOn = NULL;
				} else {
					strToChar.Format("%s is already being spied by %s.\r\n", pTarget->GetName().cptr(),
							pTarget->m_pSpiedby->GetName().cptr());
					pCh->SendToChar(strToChar);
				}
			} else {
				pCh->SendToChar("You can only spy on pc's you switch to npc's.\r\n");
			}
		} else if (pTarget == pCh) {
			pCh->SendToChar("What are you stupid?\r\n");
		} else {
			pCh->SendToChar("Spy on who?\r\n");
		}
	}
}

//////////////////////////////////////////
//	Parse the practice command so we only
//	send proper information to character class
//	DoPractice

void CInterp::Practice(CCharacter *ch) {
	static const char *strTick = { "'" };
	int first = ch->CurrentCommand.Find(strTick[0]);
	int second = ch->CurrentCommand.Find(strTick[0], 2);

	if (first == -1 || second == -1) {
		ch->SendToChar("syntax practice '<skill or spell>' <Teacher's name>\r\n");
	} else {
		CString strPractice(ch->CurrentCommand(first + 1, second - 1));
		CString strTeacher = ch->CurrentCommand.Right(second);

		int nDot, nNum = 1;
		if ((nDot = strTeacher.Find('.')) != -1) {
			nNum = strTeacher.Left(nDot).MakeInt();
			strTeacher = strTeacher.Right(nDot);
		}
		CCharacter *pTeacher = ch->GetRoom()->FindCharacterByAlias(strTeacher, nNum, ch);
		if (pTeacher == ch) {
			ch->SendToChar("You can't teacher yourself!\r\n");
		} else if (pTeacher == NULL) {
			ch->SendToChar("Ask who to teach you?\r\n");
		} else {
			ch->DoPractice(strPractice, pTeacher);
		}
	}
}

////////////////////////////////////////
//	Exit
//	Shows the exits of the room the player is in

void CInterp::Exit(CCharacter * pCh) {
	CString str;
	pCh->GetRoom()->GetExits(str);
	pCh->SendToChar(str);
}

///////////////////////////////////////
//	Builds a string of commands for help
//	We can only get god commands or only non god commands
//	written by: Demetrius Comes
//	7-8-99
///////////////////////////////////////////

void CInterp::BuildHelpOnCommand(CString & str, bool bGod) {
	POSITION pos = m_pCmdTable->GetStartingPosition();
	sComClass *pCom1, *pCom2, *pCom3;
	short nCounter = 0;
	while (pos) {
		pCom1 = m_pCmdTable->GetNext(pos);
		if ((bGod && pCom1->IsGodCommand()) || (!bGod && !pCom1->IsGodCommand())) {
			if (++nCounter == 3) {
				str.Format("%s%-15s%-15s%-15s\r\n", str.cptr(), pCom3->CommandName.cptr(), pCom2->CommandName.cptr(),
						pCom1->CommandName.cptr());
				nCounter = 0;
			} else if (nCounter == 2) {
				pCom2 = pCom1;
			} else {
				pCom3 = pCom1;
			}
		}
	}
	if (nCounter == 1) {
		str.Format("%s%-15s\r\n", str.cptr(), pCom3->CommandName.cptr());
	} else if (nCounter == 2) {
		str.Format("%s%-15s%-15s\r\n", str.cptr(), pCom2->CommandName.cptr(), pCom3->CommandName.cptr());
	}
}

/////////////////////////////////
//	Allows players to petition gods for
//	questions, etc
//	written: 8-13-99

void CInterp::DoPetition(CCharacter *pCh) {
	CString str(pCh->CurrentCommand.GetWordAfter(1, true));
	if (pCh->IsNPC()) {
		if (pCh->m_Master.HasMaster()) {
			pCh->m_Master.GetMaster()->SendToChar("Petition is for pc's.\r\n");
			CString strToGods;
			strToGods.Format("%s just tried to have thier followers petition: %s.\r\n",
					pCh->m_Master.GetMaster()->GetName().cptr(), str.cptr());
			GVM.SendToGods(strToGods, CCharacterAttributes::PREFERS_PETITION, NULL);
		} else {
			pCh->SendToChar("You can't petition from an npc.\r\n");
		}
		return;
	}
	if (str.IsEmpty()) {
		pCh->SendToChar("Don't you think the gods would get upset if you have nothing to say!\r\n");
	} else if (pCh->IsGod()) {
		pCh->SendToChar("Just talk to your fellow gods\r\n");
	} else {
		CString strToChar;
		strToChar.Format("&r%s petitions: &R'%s'&n\r\n", pCh->GetName().cptr(), str.cptr());
		GVM.SendToGods(strToChar, CCharacterAttributes::PREFERS_PETITION, pCh);
		strToChar.Format("&rYou petition the gods : &R%s&n\r\n", str.cptr());
		pCh->SendToChar(strToChar);
	}
}

////////////////////////////////////////////////
//	Show
//	Give detailed information on Zones, and players
//	and anything else I can think of to add
//	written by: Demetrius Comes
///////////////////////////////////////////////

void CInterp::Show(CCharacter *pCh) {
	CString str(pCh->CurrentCommand.GetWordAfter(1));
	CString strInfo;
	if (str.Compare("zones")) {
		strInfo = m_pGame->m_pZoneManager->GetZoneInformation();
	} else if (str.Compare("players")) {
		strInfo = "Name             State     From\r\n";
		POSITION pos = GVM.PlayerLL.GetStartPosition();
		CCharacter *p;
		while (pos) {
			p = GVM.PlayerLL.GetNext(pos);
			if (!p->IsNPC()) {
				strInfo.Format("%s%-15s%-10s%-40s\r\n", strInfo.cptr(), p->GetName().cptr(),
						p->IsLinkDead() ? "Link Dead" : "playing",
						p->m_pDescriptor == NULL ? "" : ((sPlayerDescriptor *) p->m_pDescriptor)->Host.cptr());
			}
		}
	} else if (str.Compare("stats")) {
		static const char *strFormat = "	Total Players in Player file = %d\r\n%s\r\n";
		strInfo.Format(strFormat, CCharacterAttributes::GetSavePlayerCount(), CMudTime::GetStatInfo().cptr());
	} else {
		strInfo = "useage: show <zones or players or stats>\r\n";
	}
	pCh->SendCharPage(strInfo);
}

///////////////////////////
//	Allows gods to goto characters
//	

void CInterp::DoGoto(CCharacter *pCh) {
	CString strTarget(pCh->CurrentCommand.GetWordAfter(1));
	CCharacter *pTarget = GVM.FindCharacter(strTarget, pCh);
	if (pTarget == NULL) {
		pCh->SendToChar("Goto who?\r\n");
	} else if (pTarget == pCh) {
		pCh->SendToChar("Hrm...your there\r\n");
	} else {
		if (!pCh->Prefers(CCharacterAttributes::PREFERS_GOD_INVIS)) {
			if (pCh->m_strPoofOut.IsEmpty()) {
				pCh->GetRoom()->SendToRoom("%s is gone in a puff of smoke.\r\n", pCh);
			} else {
				pCh->GetRoom()->SendToRoom(pCh->m_strPoofOut);
			}
		}
		//remove from room
		pCh->GetRoom()->Remove(pCh);
		//change room pointer
		pCh->m_pInRoom = pTarget->GetRoom();
		//Add to new room
		pCh->GetRoom()->Add(pCh);
		if (!pCh->Prefers(CCharacterAttributes::PREFERS_GOD_INVIS)) {
			if (pCh->m_strPoofIn.IsEmpty()) {
				pCh->GetRoom()->SendToRoom("%s arrives in a puff of smoke.\r\n", pCh);
			} else {
				pCh->GetRoom()->SendToRoom(pCh->m_strPoofOut);
			}
		}
		pCh->SendToChar(pCh->GetRoom()->DoRoomLook(pCh));
	}

}

///////////////////////////////
//	allow gods to chat to each other

void CInterp::DoGodChat(CCharacter *pCh) {
	CString strToSay(pCh->CurrentCommand.GetWordAfter(1, true));
	if (strToSay.IsEmpty()) {
		pCh->SendToChar("Say what?\r\n");
	} else {
		CString str;
		str.Format("&yYou tell your fellow gods: &Y'%s'&n\r\n", strToSay.cptr());
		pCh->SendToChar(str);
		str.Format("&y%s announces %s fellow gods &Y'%s'&n\r\n", pCh->GetName().cptr(),
				CCharacterAttributes::GetPronoun(pCh->GetSex()), strToSay.cptr());
		GVM.SendToGods(str, CCharacterAttributes::PREFERS_GOD_CHAT, pCh);
	}
}

///////////////////////////////
//	Allows a god to cut a players link 
//  BE NICE with this

void CInterp::Cutlink(CCharacter *pCh) {
	CString str(pCh->CurrentCommand.GetWordAfter(1));
	//can do this to mobs so only pass in one
	CCharacter *pTarget = GVM.FindCharacter(str, pCh);
	if (pTarget == NULL) {
		pCh->SendToChar("Cut who's link?\r\n");
	} else if (pTarget == pCh) {
		pCh->SendToChar("Are you nuts?\r\n");
	} else if (pTarget->IsNPC()) {
		pCh->SendToChar("Em...only players.\r\n");
	} else if (pTarget->IsLinkDead()) {
		pCh->SendToChar("Em... they are already!\r\n");
	} else {
		CString strToChar;
		strToChar.Format("You link has been cut by %s.\r\n", pCh->GetName().cptr());
		pTarget->SendToChar(strToChar);
		pTarget->m_pDescriptor->ChangeState(STATE_LINK_DEAD);
		strToChar.Format("You have cut %s's link.\r\n", pTarget->GetName().cptr());
		pCh->SendToChar(str);
	}
}

//////////////////////////
//	Adds the Camp affect to the character
//	which counts down until they can set up camp
//	and basically rent out of the game

void CInterp::Camp(CCharacter *pCh) {
	if (pCh->GetRoom()->CanCampInRoom()) {
		pCh->AddAffect(CCharacter::AFFECT_SET_UP_CAMP, CMudTime::PULSES_PER_MUD_HOUR);
	} else {
		pCh->SendToChar("You can't camp here!\r\n");
	}
}

////////////////////////////
//	Sail
//	when on a boat give the order to sail in a direction

void CInterp::Sail(CCharacter *pCh) {
	if (pCh->GetRoom()->IsTransportRoom()) {
		CTransport *pTrans = ((CTransportRoom *) pCh->GetRoom())->GetTransport();
		pTrans->DoMove(pCh, pCh->CurrentCommand.GetWordAfter(1));
	} else {
		pCh->SendToChar("You must be on a ship to sail!\r\n");
	}
}

void CInterp::FragList(CCharacter *pCh) {
	CString strList;
	pCh->GetFragList(strList);
	pCh->SendToChar(strList);
}

///////////////////////////////////
//	Allows gods to send messages with no name
//	11-15-99

void CInterp::GodMessage(CCharacter *pCh) {
	CString strMsg(pCh->CurrentCommand.GetWordAfter(1, true));
	if (strMsg.IsEmpty()) {
		pCh->SendToChar("What do you want to tell everyone?\r\n");
	} else {
		strMsg += "\r\n";
		GVM.SendToAll(strMsg, pCh);
		strMsg.Format("You send to the world: %s", strMsg.cptr());
		pCh->SendToChar(strMsg);
	}
}

/////////////////////////////////////
//	Allows a god to resurrect a player
//

void CInterp::GodRes(CCharacter *pCh) {
	bool bAll;
	CObject *pObj = pCh->GetTarget(1, false, bAll);

	if (pObj == NULL) {
		pCh->SendToChar("Ressurect what?\r\n");
	} else if (!pObj->IsType(ITEM_TYPE_CORPSE)) {
		pCh->SendToChar("You can only resurrect corpses!\r\n");
	} else {
		if (((CCorpse *) pObj)->WasPc()) {
			CCharacter *pTarget = GVM.FindCharacter(pObj->GetObjName(), pCh);
			if (pTarget == pCh) {
				pCh->SendToChar("hrm can't resurrect yourself!\r\n");
			} else if (pTarget == NULL) {
				pCh->SendToChar("That Character must be logged on!\r\n");
			} else {
				CString str;
				str.Format("%s has mercy on you and returns your soul to the living.\r\n", pCh->GetName().cptr());
				pTarget->SendToChar(str);
				str.Format("You return %s's soul to %s body.\r\n", pTarget->GetName().cptr(),
						CCharacterAttributes::GetPronoun(pTarget->GetSex()));
				pCh->SendToChar(str);
				//remomove all affects and eq
				pTarget->RemoveAllAffects(false);
				pTarget->PutAllEqInRoom();
				//remove them from the room they are in
				pTarget->GetRoom()->Remove(pTarget);
				//change room
				pTarget->m_pInRoom = pCh->GetRoom();
				//add them to the new room
				pTarget->GetRoom()->Add(pTarget);
				pTarget->m_nCurrentHpts = pTarget->m_nMaxHpts;
				pTarget->m_nManaPts = pTarget->m_nMaxManaPts;
				pTarget->m_nMovePts = pTarget->m_nMaxMovePts;
				pTarget->m_nExperience += ((pTarget->GetExp(pTarget->GetLevel())
						- pTarget->GetExp((pTarget->GetLevel()) - 1)) * (EXP_LOST_IN_DEATH)) / 100;
				if (pTarget->EnoughExpForNextLevel()) {
					pTarget->AdvanceLevel(true, true);
				}

				((CCorpse *) pObj)->Loot(pTarget, true); //true means it will force add all to inventory and not send any messages
				//also saves char

				//set corpse for removal
				pObj->SetDelete();
			}
		} else {
			pCh->SendToChar("You can only resurrect PC's!\r\n");
		}
	}
}

////////////////////////////////////
//	Allows a god to force characters to do something

void CInterp::GodForce(CCharacter *pCh) {
	CString strName(pCh->CurrentCommand.GetWordAfter(1));
	CCharacter *pTarget;
	CString strCmd(pCh->CurrentCommand.GetWordAfter(2, true));
	CString strMsg;
	strMsg.Format("%s forces you to: %s", pCh->GetName().cptr(), strCmd.cptr());
	CString strToGod;
	if (strName.IsEmpty() || strCmd.IsEmpty()) {
		pCh->SendToChar("syntax: gforce <all | player name> <cmd>\r\n");
	} else if (strName.Compare("all")) {
		POSITION pos = GVM.PlayerLL.GetStartPosition();
		while (pos) {
			pTarget = GVM.PlayerLL.GetNext(pos);
			if (!pTarget->IsNPC() && !pTarget->IsLinkDead() && !pTarget->IsGod()) {
				pTarget->SendToChar(strMsg);
				pTarget->AddCommand(strCmd);
			}
		}
		strToGod.Format("You force everyone to: %s.\r\n", strCmd.cptr());
		pCh->SendToChar(strToGod);
	} else {
		pTarget = GVM.FindCharacter(strName, pCh);
		if (pTarget == pCh) {
			pCh->SendToChar("Just do the command yourself.\r\n");
		} else if (pTarget == NULL) {
			pCh->SendToChar("Force who?\r\n");
		} else {
			pTarget->SendToChar(strMsg);
			pTarget->AddCommand(strCmd);
			strToGod.Format("You for %s to: %s.\r\n", pTarget->GetName().cptr(), strCmd.cptr());
		}
	}
}

///////////////////////////////////////
//	Allows a god to re-fresh certain files
//

void CInterp::Refresh(CCharacter *pCh) {
	CString strRe(pCh->CurrentCommand.GetWordAfter(1));
	if (strRe.Compare("news")) {
		if (CCharacterAttributes::ReadNews()) {
			pCh->SendToChar("The news file has been re-freshed.\r\n");
		} else {
			pCh->SendToChar("There was an error reading news file.\r\n");
		}
	} else if (strRe.Compare("motd")) {
		if (CCharacterAttributes::ReadMOTD()) {
			pCh->SendToChar("The motd file has been re-freshed.\r\n");
		} else {
			pCh->SendToChar("There was an error reading motd file.\r\n");
		}
	} else if (strRe.Compare("help")) {
		pCh->SendToChar("ToDo\r\n");
		/*m_strGodHelp.Empty();
		 m_strCommandHelp.Empty();
		 POSITION pos = m_HelpTable.GetStartingPosition();
		 while(pos)
		 {
		 delete m_HelpTable.GetNext(pos);
		 }
		 m_HelpTable.RemoveAll();
		 this->BuildHelp();*/
	} else {
		pCh->SendToChar("syntax: refresh <news | motd | help>\r\n");
	}
}

///////////////////////////////////////
//	god locate
//	allows a god to locate any object in the mud
//  using a generic function that could be used later for a locate spell =)

void CInterp::GodLocate(CCharacter *pCh) {
	CString strObj(pCh->CurrentCommand.GetWordAfter(1));
	if (strObj.IsEmpty()) {
		pCh->SendToChar("Locate what?\r\n");
	} else {
		CString str("You locate the following: \r\n");
		str += m_pGame->m_pObjectManager->LocateObject(strObj, pCh);
		pCh->SendCharPage(str);
	}
}

///////////////////////////
//	if char is invis in vis

void CInterp::Vis(CCharacter *pCh) {
	if (pCh->IsAffectedBy(CCharacter::AFFECT_INVISIBLE)) {
		pCh->RemoveAffect(CCharacter::AFFECT_INVISIBLE);
	} else {
		pCh->SendToChar("You're not invisible now!\r\n");
	}
}

///////////////////////////
//	God freeze allows a god to freeze a character so all they
// can do is use say

void CInterp::GodFreeze(CCharacter *pCh) {
	CCharacter *pTarget = GVM.FindCharacter(pCh->CurrentCommand.GetWordAfter(1), NULL);
	if (pTarget != NULL) {
		pTarget->m_pDescriptor->ChangeState(STATE_FROZEN);
		CString str;
		str.Format(
				"You have frozen %s they will only be able to use the say command.\r\nPlease trans them to judgement room.\r\n",
				pTarget->GetName().cptr());
		pCh->SendToChar(str);
		str.Format(
				"%s has frozen you.\r\nThis command can only be run by a greater god.\r\nYou will be able to 'say'.\r\n",
				pCh->GetName().cptr());
		pTarget->SendToChar(str);
	} else {
		pCh->SendToChar("Freeze who?\r\n");
	}
}

/////////////////////////////
//	Allows a god to release a frozen character

void CInterp::GRelease(CCharacter *pCh) {
	CCharacter *pTarget = GVM.FindCharacter(pCh->CurrentCommand.GetWordAfter(1), NULL);
	if (pTarget != NULL && pTarget->m_pDescriptor->GetState() == STATE_FROZEN) {
		pTarget->m_pDescriptor->ChangeState(STATE_PLAYING);
		CString str;
		pTarget->SendToChar("The gods have released you.  Go in Peace.\r\n");
		str.Format("You release %s.\r\n", pTarget->GetName().cptr());
		pCh->SendToChar(str);
	} else if (pTarget != NULL) {
		pCh->SendToChar("That person is not frozen.\r\n");
	} else {
		pTarget->SendToChar("Release who?\r\n");
	}
}

void CInterp::Initiate(CCharacter *pCh) {
	CString strAI(pCh->CurrentCommand.GetWordAfter(1));
	CString strNumber(pCh->CurrentCommand.GetWordAfter(2));
	int nNumber = strNumber.IsEmpty() ? 0 : strNumber.MakeInt();
	if (strAI.Compare("mohr")) {
		CNPC::SWAMP_MOHR_STATUS = nNumber;
	} else {
		pCh->SendToChar(
				"usage: initiate <zone> <number>\r\nSwamp Mohr Zone:<mohr>: 1: add path to orc town, 2: Turn off Sentinel, 3: add path to gnoll town, 0: Off\r\n");
	}
}

///////////////////////////////////////
//	Will be used for potions, staffs, lock picks etc
//	find the object, send msg, run Use fnc for object

void CInterp::UseObject(CCharacter *pUser) {
	bool bAll;
	CObject *pObj = pUser->GetTarget(1, true, bAll);
	if (pObj != NULL) {
		pObj->Use(pUser);
	} else {
		pUser->SendToChar("You don't seem to have that in your inventory.\r\n");
	}
}

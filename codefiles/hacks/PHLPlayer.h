#pragma once

#include "../PHLMemory.h"

#define GAME_STRUCT_SEARCH_OFFSET 0x1
#define PLAYER_STRUCT_OFFSET 0x1720
#define GAME_STRUCT_OFFSET 0x160
#define MOUSE_X_OFFSET 0xAE4
#define MOUSE_Y_OFFSET 0xAE8

#pragma region player_stats

#define PLAYER_STAT_STRUCT_OFFSET_1 0x44
#define PLAYER_STAT_STRUCT_OFFSET_2 0x904

#define PLAYER_MAX_HP_OFFSET 0x10
#define PLAYER_CUR_HP_OFFSET 0x14

#define PLAYER_MAX_MANA_OFFSET 0x34
#define PLAYER_CUR_MANA_OFFSET 0x38

#define PLAYER_MAX_ES_OFFSET 0x58
#define PLAYER_CUR_ES_OFFSET 0x5C

const enum PlayerStatCode
{
	ARMOR = 0xF4,
	EVASION = 0x11D,
	STRENGTH = 0x243,
	INTELLIGENCE = 0x246,
	DEXTERITY = 0x249
	
};

#pragma endregion

class PHLPlayer : public PHLHack<PHLPlayer>
{
private:
	PHLPlayer ();
public:
	friend class PHLHack<PHLPlayer>;
	void printAddr ();

	// Basic player info
	Addr getPlayerStatAddr ();
	int getCurHealth ();
	int getMaxHealth ();
	int getCurMana ();
	int getMaxMana ();
	int getCurES ();
	int getMaxES ();

	// Cool
	Addr mouseX;
	Addr mouseY;
	Addr baseStruct;
	Addr gameStruct;
	Addr playerStruct;
};
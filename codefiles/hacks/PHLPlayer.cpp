#include "PHLPlayer.h"
#include "../PHLConsole.h"

#define BASE_STRUCT_SEARCH_OFFSET 0xF
#define PLAYER_STRUCT_OFFSET 0x1720
#define GAME_STRUCT_OFFSET 0x160
#define MOUSE_X_OFFSET 0xAE4
#define MOUSE_Y_OFFSET 0xAE8

#pragma region player_stats

#define PLAYER_DEF_OFFSET_1 0x44
#define PLAYER_DEF_OFFSET_2 0x904

#define PLAYER_STAT_OFFSET_1 0x44
#define PLAYER_STAT_OFFSET_2 0x900
#define PLAYER_STAT_OFFSET_3 0x4
#define PLAYER_STAT_OFFSET_4 0x4
#define PLAYER_STAT_OFFSET_5 0x4
#define PLAYER_STAT_OFFSET_6 0x2C

#define PLAYER_MAX_HP_OFFSET 0x10
#define PLAYER_CUR_HP_OFFSET 0x14

#define PLAYER_MAX_MANA_OFFSET 0x34
#define PLAYER_CUR_MANA_OFFSET 0x38

#define PLAYER_MAX_ES_OFFSET 0x58
#define PLAYER_CUR_ES_OFFSET 0x5C

#pragma endregion

PHLPlayer::PHLPlayer ()
{
	baseStruct =
		PHLMemory::Instance ()->findPattern (
			HexPattern ("89 45 ?? B8 ?? ?? ?? ?? "
						"8D 8D ?? ?? ?? ?? E8 ?? "
						"?? ?? ?? C6 45 FC ?? 8B "
						"7D ?? 8D B5 ?? ?? ?? ?? "
						"E8 ?? ?? ?? ?? 8B CE 8A "
						"D8 C6 45 FC ?? E8 ?? ?? "
						"?? ?? 84 DB"));

	baseStruct = PHLMemory::readAddr (baseStruct -
									  BASE_STRUCT_SEARCH_OFFSET);

	gameStruct = PHLMemory::readAddr (baseStruct) +
		GAME_STRUCT_OFFSET;

	playerStruct = gameStruct +
		PLAYER_STRUCT_OFFSET;

	mouseX = gameStruct +
		MOUSE_X_OFFSET;
	mouseY = gameStruct +
		MOUSE_Y_OFFSET;
}

void PHLPlayer::printAddr ()
{
	Addr base = PHLMemory::Instance ()->base;
	PHLConsole::printLog (
		"Base Struct:            %.8X, PathOfExile.exe + %.8X\n"
		"Game Struct:            %.8X, PathOfExile.exe + %.8X\n"
		"Player Struct:          %.8X, PathOfExile.exe + %.8X\n"
		"Mouse X Address:        %.8X, PathOfExile.exe + %.8X\n"
		"Mouse Y Address:        %.8X, PathOfExile.exe + %.8X\n",
		baseStruct, baseStruct - base,
		gameStruct, gameStruct - base,
		playerStruct, playerStruct - base,
		mouseX, mouseX - base,
		mouseY, mouseY - base);
}

int * PHLPlayer::getPlayerStatsPtr () const
{
	Addr playerStatsPtr = PHLMemory::readAddr (playerStruct);

	playerStatsPtr = PHLMemory::readAddr (playerStatsPtr +
										  PLAYER_STAT_OFFSET_1);
	playerStatsPtr = PHLMemory::readAddr (playerStatsPtr +
										  PLAYER_STAT_OFFSET_2);
	playerStatsPtr = PHLMemory::readAddr (playerStatsPtr +
										  PLAYER_STAT_OFFSET_3);
	playerStatsPtr = PHLMemory::readAddr (playerStatsPtr +
										  PLAYER_STAT_OFFSET_4);
	playerStatsPtr = PHLMemory::readAddr (playerStatsPtr +
										  PLAYER_STAT_OFFSET_5);
	playerStatsPtr = PHLMemory::readAddr (playerStatsPtr +
										  PLAYER_STAT_OFFSET_6);

	if (!playerStatsPtr)
	{
		return nullptr;
	}

	return (int *)playerStatsPtr;
}

int PHLPlayer::getPlayerStat (PlayerStatCode code) const
{
	int * playerStatsArr = getPlayerStatsPtr ();

	int index = 0;
	while (playerStatsArr[index] < code)
	{
		index += 2;
	}

	if (playerStatsArr[index] == code)
	{
		return playerStatsArr[index + 1];
	}

	return NULL;
}

Addr PHLPlayer::getPlayerDefenseAddr () const
{
	Addr defAddr = PHLMemory::readAddr (playerStruct);
	defAddr = PHLMemory::readAddr (defAddr +
								   PLAYER_DEF_OFFSET_1);
	defAddr = PHLMemory::readAddr (defAddr +
								   PLAYER_DEF_OFFSET_2);

	if (!defAddr)
	{
		return NULL;
	}

	return defAddr;
}

int PHLPlayer::getPlayerDefense (int offset) const
{
	Addr playerDef = getPlayerDefenseAddr ();
	if (!isAddressValid (playerDef))
	{
		return NULL;
	}

	return PHLMemory::readAddr (playerDef +
								offset);
}

int PHLPlayer::getCurHealth () const
{
	return getPlayerDefense (PLAYER_CUR_HP_OFFSET);
}

int PHLPlayer::getMaxHealth () const
{
	return getPlayerDefense (PLAYER_MAX_HP_OFFSET);
}

int PHLPlayer::getCurMana () const
{
	return getPlayerDefense (PLAYER_CUR_MANA_OFFSET);
}

int PHLPlayer::getMaxMana () const
{
	return getPlayerDefense (PLAYER_MAX_MANA_OFFSET);
}

int PHLPlayer::getCurES () const
{
	return getPlayerDefense (PLAYER_CUR_ES_OFFSET);
}

int PHLPlayer::getMaxES () const
{
	return getPlayerDefense (PLAYER_MAX_ES_OFFSET);
}

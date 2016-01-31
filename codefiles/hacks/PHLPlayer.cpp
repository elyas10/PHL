#include "PHLPlayer.h"

PHLPlayer::PHLPlayer ()
{
	baseStruct = GAME_STRUCT_SEARCH_OFFSET +
		PHLMemory::Instance ()->findPattern (
			HexPattern ("A1 ?? ?? ?? ?? 89 B0 B4 "
						"18 00 00 E8 ?? ?? ?? ?? "
						"89 45 4C"));

	baseStruct = *(Addr*)baseStruct;

	gameStruct = *(Addr*)baseStruct +
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

Addr PHLPlayer::getPlayerStatAddr ()
{
	Addr playerStat = playerStruct;

	if (!isAddressValid (playerStat))
	{
		PHLConsole::printError ("Player Struct Ptr is invalid, bad address or "
								"player stats haven't loaded yet");
		return NULL;
	}

	playerStat = *(Addr*)(playerStat)+
		PLAYER_STAT_STRUCT_OFFSET_1;

	if (!isAddressValid (playerStat))
	{
		PHLConsole::printError ("Player Struct Ptr is invalid, bad address or "
								"player stats haven't loaded yet");
		return NULL;
	}

	return *(Addr*)(playerStat)+
		PLAYER_STAT_STRUCT_OFFSET_2;
}

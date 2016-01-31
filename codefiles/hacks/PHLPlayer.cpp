#include "PHLPlayer.h"

PHLPlayer::PHLPlayer ()
{
	gameStruct = GAME_STRUCT_SEARCH_OFFSET +
		PHLMemory::Instance ()->findPattern (
			HexPattern ({
		0x83, 0xEC, 0x3C, 0x57,
		0x8B, 0x7D, 0x08, 0x8B,
		0x07, 0x6A, 0x00, 0x51,
		0x8B, 0x88, 0xAC, 0x00,
		0x00, 0x00, 0xF3, 0x0F,
		0x11, 0x04, 0x24, 0x6A,
		0x00, 0x6A, 0x07
	}));

	gameStruct = *(Addr*)gameStruct;

	gameStruct = *(Addr*)gameStruct +
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
	PHLConsole::printLog ("Game Struct:            %X, PathOfExile.exe + %X\n"
						  "Player Struct:          %X, PathOfExile.exe + %X\n"
						  "Mouse X Address:        %X, PathOfExile.exe + %X\n"
						  "Mouse Y Address:        %X, PathOfExile.exe + %X\n",
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

	playerStat = *(Addr*)(playerStat) +
		PLAYER_STAT_STRUCT_OFFSET_1;

	if (!isAddressValid (playerStat))
	{
		PHLConsole::printError ("Player Struct Ptr is invalid, bad address or "
								"player stats haven't loaded yet");
		return NULL;
	}

	return *(Addr*)(playerStat) +
		PLAYER_STAT_STRUCT_OFFSET_2;
}

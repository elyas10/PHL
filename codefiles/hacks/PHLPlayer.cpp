#include "PHLPlayer.h"

PHLPlayer::PHLPlayer ()
{
	gameStruct = WIN_GAME_STRUCT_SEARCH_OFFSET +
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
	playerStruct = *(Addr*)gameStruct +
		WIN_GAME_STRUCT_OFFSET;

	mouseX = playerStruct +
		MOUSE_X_OFFSET;
	mouseY = playerStruct +
		MOUSE_Y_OFFSET;
}

void PHLPlayer::printAddr ()
{
	Addr base = PHLMemory::Instance ()->base;
}

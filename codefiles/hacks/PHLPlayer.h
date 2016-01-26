#pragma once

#include "../PHLMemory.h"

#define WIN_GAME_STRUCT_SEARCH_OFFSET 0x32
#define WIN_GAME_STRUCT_OFFSET 0x160
#define MOUSE_X_OFFSET 0xAE4
#define MOUSE_Y_OFFSET 0xAE8

class PHLPlayer : public PHLHack<PHLPlayer>
{
private:
	PHLPlayer ();
public:
	friend class PHLHack<PHLPlayer>;
	void printAddr ();

	// Cool
	Addr mouseX;
	Addr mouseY;
	Addr gameStruct;
	Addr playerStruct;
};
#pragma once

#include "../PHLMemory.h"

const enum PlayerStatCode
{
	ARMOR = 0xF4,
	EVASION = 0x11D,
	STRENGTH = 0x243,
	INTELLIGENCE = 0x246,
	DEXTERITY = 0x249
};



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
#include "PHLMap.h"

#define MAP_HACK_OFFSET_1 0x01;
#define MAP_HACK_OFFSET_2 0x10;
#define MAP_HACK_OFFSET_3 0x1C;
#define MAP_HACK_OFFSET_4 0x28;

#define MAP_HACK_PATCH_VALUE 0xE8;

PHLMap::PHLMap ()
{
	/*
		Search:
			\xD9\x00\x8B\x0C\x24

		Patch:
			0x01 = 0xE8
			0x10 = 0xE8
			0x1C = 0xE8
			0x28 = 0xE8
	*/
	mapHackOff =
		PHLMemory::findPattern (HexPattern ({
		0xD9, 0x00, 0x8B, 0x0C,
		0x24, 0xD9, 0x19, 0x8B
	}));

	CodeCave cc (mapHackOff, { 0xE8 });

	cc.addr = mapHackOff + MAP_HACK_OFFSET_1;
	cc.createCodeCave ();
	cc.addr = mapHackOff + MAP_HACK_OFFSET_2;
	cc.createCodeCave ();
	cc.addr = mapHackOff + MAP_HACK_OFFSET_3;
	cc.createCodeCave ();
	cc.addr = mapHackOff + MAP_HACK_OFFSET_4;
	cc.createCodeCave ();
}

void PHLMap::printAddr ()
{
	Addr base = PHLMemory::Instance ()->base;
	PHLConsole::printLog ("MapHack Offset: %X, PathOfExile.exe + %X\n",
						mapHackOff, mapHackOff - base);
}

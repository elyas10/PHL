#include "PHLBypass.h"
#include "../PHLConsole.h"

PHLBypass * PHLBypass::phlBypass = nullptr;

PHLBypass::PHLBypass ()
{
	setBypassOffsetA ();
	setBypassOffsetB ();
	setBypassOffsetC ();

	activateBypassA ();
	activateBypassB ();
	activateBypassC ();
}

bool PHLBypass::setBypassOffsetA ()
{
	HexPattern hexPattern ({
		0x5F, 0x32, 0xC0, 0x5E,
		0xC3,

		0xE8, 0xC3, 0x13, 0x00,
		0x00,

		0x6A, 0x00, 0x6A, 0x00,
		0x6A, 0x00,

		0x68, 0x00, 0xE8, 0xC4,
		0x00,

		0x6A, 0x00, 0x6A, 0x00
	});

	hexPattern.assignMask ({
		0x01, 0x01, 0x01, 0x01,
		0x01,

		0x00, 0x00, 0x00, 0x00,
		0x00,

		0x01, 0x01, 0x01, 0x01,
		0x01, 0x01,

		0x00, 0x00, 0x00, 0x00,
		0x00,

		0x01, 0x01, 0x01, 0x01
	});

	bypassA = PHLMemory::findPattern (hexPattern);
	bypassA -= 0x02;

	if (!isAddressValid (bypassA))
	{
		return false;
	}
	return true;
}

bool PHLBypass::setBypassOffsetB ()
{
	bypassB =
		PHLMemory::findPattern (HexPattern ({
		0x57, 0x6A, 0x04, 0x68,
		0x00, 0x10, 0x00, 0x00,
		0x8D, 0x44, 0x24, 0x0C,
		0x50, 0x6A, 0x00
	}));
	bypassB -= 0x18;

	if (!isAddressValid (bypassB))
	{
		return false;
	}
	return true;
}

bool PHLBypass::setBypassOffsetC ()
{
	/*
		search: 55 8b ec 83
				e4 f8 83 ec
				08 ?? ?? ??
				?? ?? ?? ??
				53 55 56 57
				0F
		before: 0f 84 cd 00 00 00
		after:  e9 ce 00 00 00 90
	*/
	HexPattern hexPattern ("55 8B EC 83 E4 F8 83 EC "
						   "08 ?? ?? ?? ?? ?? ?? ?? "
						   "53 55 56 57 0F");

	bypassC = PHLMemory::findPattern (hexPattern);

	if (!isAddressValid (bypassC))
	{
		return false;
	}

	bypassC += 0x27;

	return true;
}

PHLBypass * PHLBypass::Instance ()
{
	if (!phlBypass)
	{
		phlBypass = new PHLBypass;
	}
	return phlBypass;
}

void PHLBypass::printAddr ()
{
	Addr base = PHLMemory::Instance ()->base;
	PHLConsole::printLog (
		"Bypass A:               %.8X, PathOfExile + %.8X\n"
		"Bypass B:               %.8X, PathOfExile + %.8X\n"
		"Bypass C:               %.8X, PathOfExile + %.8X\n",
		bypassA, bypassA - base,
		bypassB, bypassB - base,
		bypassC, bypassC - base);
}

bool PHLBypass::activateBypassA ()
{
	// Change jz 5 to nops so it returns
	CodeCave cc =
		CodeCave (bypassA,
		{ 0x90, 0x90 });
	if (cc.createCodeCave ())
	{
		return true;
	}
	return false;
}

bool PHLBypass::activateBypassB ()
{
	/*
		Change jnz to jmp so it always jumps
		25EA07

		To do this we change:
		0F 85 9E 00 00 00
		To:
		E9 9F 00 00 00 90

		It changes from 9E to 9F because we jump
		relatively in bytes, and since we add a nop
		after, we have to jump that extra nop byte
	*/

	Addr entry = bypassB;
	DWORD jumpDist = PHLMemory::readAddr (entry + 0x2) + 0x1;

	BYTE * bytes = (BYTE*)(&jumpDist);

	CodeCave cc = CodeCave (entry,
	{ 0xE9,
	bytes[0], bytes[1],
	bytes[2], bytes[3],
	0x90 });

	if (cc.createCodeCave ())
	{
		return true;
	}
	return false;
}

bool PHLBypass::activateBypassC ()
{
	/*
		before: 0f 84 cd 00 00 00
		after:  e9 ce 00 00 00 90
	*/

	Addr entry = bypassC;
	DWORD jumpDist = PHLMemory::readAddr (entry + 0x2) + 0x1;

	BYTE * bytes = (BYTE*)(&jumpDist);

	CodeCave cc = CodeCave (entry,
	{ 0xE9,
		bytes[0], bytes[1],
		bytes[2], bytes[3],
		0x90 });

	if (cc.createCodeCave ())
	{
		return true;
	}
	return false;
}

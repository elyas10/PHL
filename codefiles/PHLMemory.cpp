#include "PHLMemory.h"
#include "PHLConsole.h"
#include <Psapi.h>

PHLMemory * PHLMemory::phlMem = nullptr;

bool isAddressValid (Addr addr)
{
	if (addr <= 0)
	{
		return false;
	}
	return true;
}

void CodeCave::init ()
{
	this->length = 0;
	this->addr = NULL;
	memset (newOpcodes, 0x90,
			PHL_MAX_ARRAY_SIZE);
	memset (oldOpcodes, 0x90,
			PHL_MAX_ARRAY_SIZE);
}

CodeCave::CodeCave ()
{
	init ();
}

CodeCave::CodeCave (Addr addr,
					HexCode newOp)
{
	init ();
	if (!isAddressValid (addr))
	{
		PHLConsole::printError ("Failed to create code cave variable, "
								"address is invalid");
	}
	this->addr = addr;
	assignNewOpCodes (newOp);
}

bool CodeCave::createCodeCave ()
{
	CodeCave& cc = *this;

	if (!isAddressValid (cc.addr))
	{
		PHLConsole::printError ("Failed to create code cave, "
								"address is invalid");
		return false;
	}

	if (!cc.length)
	{
		PHLConsole::printError ("Failed to create code cave, "
								"length is zero!");
		return false;
	}

	DWORD oldPermission = NULL;
	DWORD newPermission = PAGE_EXECUTE_READWRITE;

	if (!VirtualProtect ((BYTE*)cc.addr, (SIZE_T)cc.length,
						 newPermission, &oldPermission))
	{
		PHLConsole::printError ("Failed to get permission while "
								"attempting to apply code cave");
		return false;
	}

	for (BYTE i = 0x0; i < cc.length; i++)
	{
		cc.oldOpcodes[i] = *(BYTE*)(cc.addr + i);
		*(BYTE*)(cc.addr + i) = cc.newOpcodes[i];
	}

	if (!VirtualProtect ((BYTE*)cc.addr, (SIZE_T)cc.length,
						 oldPermission, &newPermission))
	{
		PHLConsole::printError ("Failed to restore permission "
								"after creating code cave");
		return false;
	}
	return true;
}

void CodeCave::assignNewOpCodes (HexCode newOp)
{
	int index = 0;
	for (BYTE b : newOp)
	{
		newOpcodes[index++] = b;
	}
	length = (BYTE)newOp.size ();
}

void HexPattern::init ()
{
	this->length = 0;
	memset (pattern, 0x90,
			PHL_MAX_ARRAY_SIZE);
	memset (mask, 0x1,
			PHL_MAX_ARRAY_SIZE);
}

HexPattern::HexPattern ()
{
	init ();
}

HexPattern::HexPattern (HexCode val)
{
	init ();
	length = (BYTE)val.size ();
	assignPattern (val);
}

HexPattern::HexPattern (std::string aob)
{
	init ();
	std::vector<char> string;
	std::string::iterator it = aob.begin ();

	// Loop to remove spaces and such
	while (it != aob.end())
	{
		char buffer = *it;
		if (buffer == '?' ||
			buffer == 'x' ||
			buffer == 'X' ||
			(buffer >= 'a' && buffer <= 'f') ||
			(buffer >= 'A' && buffer <= 'F') ||
			(buffer >= '0' && buffer <= '9'))
		{
			string.push_back (buffer);
		}
		it++;
	}

	for (unsigned int i = 0;
	i < string.size (); i++, length++)
	{
		if (string[i] == '?')
		{
			i++;
			if (string[i] != '?')
			{
				PHLConsole::printError ("Invalid string of "
										"bytes given for "
										"the hex pattern!");
			}
			mask[i / 2] = 0x0;
		}
		else
		{
			BYTE buffer;

			std::string charString1 (1, string[i]);
			buffer = std::stoi (charString1, nullptr, 16);
			buffer = buffer << 4;

			i++;
			if (string[i] == '?')
			{
				PHLConsole::printError ("Invalid string of "
										"bytes given for "
										"the hex pattern!");
			}

			std::string charString2 (1, string[i]);
			buffer = buffer | std::stoi (charString2, nullptr, 16);
			pattern[i / 2] = buffer;
		}
	}
}

void HexPattern::assignMask (HexCode val)
{
	int index = 0;
	for (BYTE b : val)
	{
		mask[index++] = b;
	}
}

void HexPattern::assignPattern (HexCode val)
{
	int index = 0;
	length = (BYTE)val.size ();
	for (BYTE b : val)
	{
		pattern[index++] = b;
	}
}

PHLMemory::PHLMemory ()
{
	MODULEINFO modInfo = { 0 };
	HMODULE hModule = GetModuleHandle (NULL);
	if (!hModule)
	{
		PHLConsole::printError ("Failed to find module handle");
	}
	GetModuleInformation (GetCurrentProcess (),
						  hModule, &modInfo, sizeof (MODULEINFO));
	base = (Addr)modInfo.lpBaseOfDll;
	moduleSize = modInfo.SizeOfImage;
}

PHLMemory::~PHLMemory ()
{
	delete phlMem;
}

PHLMemory * PHLMemory::Instance ()
{
	if (!phlMem)
	{
		phlMem = new PHLMemory;
	}
	return phlMem;
}

void PHLMemory::reverseByteOrder (Addr& addr)
{
	if (isAddressValid (addr))
	{
		DWORD four = *(BYTE*)addr;
		DWORD three = *(BYTE*)(addr + 0x1) << 8;
		DWORD two = *(BYTE*)(addr + 0x2) << 16;
		DWORD one = *(BYTE*)(addr + 0x3) << 24;

		addr = 0x0;

		addr |= one | two | three | four;
		return;
	}

	PHLConsole::printError ("Failed to reverse byte order "
							"because address is invalid");

}

DWORD PHLMemory::changeMemory (Addr addr, DWORD value)
{
	if (!isAddressValid (addr))
	{
		PHLConsole::printError ("Failed to change memory "
								"because address is invalid");
		return NULL;
	}

	DWORD oldPermission;
	DWORD newPermission = PAGE_EXECUTE_READWRITE;

	if (!VirtualProtect ((BYTE*)addr, 0x4,
						 newPermission, &oldPermission))
	{
		PHLConsole::printError ("Failed to get permission writing memory");
		return NULL;
	}

	DWORD oldVal = *(DWORD*)(addr);
	*(DWORD*)(addr) = value;

	if (!VirtualProtect ((BYTE*)addr, 0x4,
						 oldPermission, &oldPermission))
	{
		PHLConsole::printError ("Failed to restore permission "
								"after writing memory region");
		return NULL;
	}

	return oldVal;
}

DWORD PHLMemory::readAddr (Addr addr)
{
	if (!isAddressValid (addr))
	{
		PHLConsole::printError ("Failed to read memory "
								"because address is invalid");
		return NULL;
	}

	return *(DWORD*)(addr);
}

void PHLMemory::hookAddr (Addr entryAddr, BYTE patchSize,
						  Addr hookFunc)
{

	if (patchSize < 5)
	{
		PHLConsole::printError ("Failed to hook address "
								"because patch size is less than 5!");
		return;
	}

	if (!isAddressValid (entryAddr) || !hookFunc)
	{
		PHLConsole::printError ("Failed to hook address "
								"because address is invalid");
		return;
	}

	CodeCave cc;
	cc.addr = entryAddr;
	cc.length = patchSize;

	int relJumpDist = (int)hookFunc -
		(int)entryAddr - 5;

	cc.newOpcodes[0] = 0xE9;
	*(Addr*)(cc.newOpcodes + 1) = relJumpDist;

	for (BYTE i = 5; i < patchSize; i++)
	{
		cc.newOpcodes[i] = 0x90;
	}

	cc.createCodeCave ();
}

Addr PHLMemory::findPattern (HexPattern pattern)
{
	bool found;
	DWORD length = (DWORD)pattern.length;
	for (DWORD i = 0; i < Instance()->moduleSize - length; i++)
	{
		found = true;
		for (DWORD j = 0;
		j < length && found; j++)
		{
			found &= !pattern.mask[j] ||
				pattern.pattern[j] ==
				*(BYTE*)(Instance ()->base + i + j);
		}
		if (found)
		{
			return Instance ()->base + i;
		}
	}

	PHLConsole::printError ("Failed to find pattern:");
	for (unsigned short i = 0; i < length - 1; i++)
	{
		if (pattern.mask[i])
		{
			PHLConsole::printLog ("%.2X, ", pattern.pattern[i]);
		}
		else
		{
			PHLConsole::printLog ("??, ", pattern.pattern[i]);
		}
	}
	PHLConsole::printLog ("%.2X !!!", pattern.pattern[length - 1]);

	return NULL;
}

int PHLMemory::findPattern (BYTE * source,
							int sourceLength,
							BYTE * pattern,
							int patternLength)
{
	bool found;
	for (int i = 0;
	i < sourceLength - patternLength;
		i++)
	{
		found = true;
		for (int j = 0;
		j < patternLength && found; j++)
		{
			found &= pattern[j] ==
				source[i + j];
		}
		if (found)
		{
			return i;
		}
	}
	PHLConsole::printError ("Failed to find pattern:");
	for (unsigned short i = 0; i < sourceLength - 1; i++)
	{
		PHLConsole::printLog ("%.2X, ", source[i]);
	}
	PHLConsole::printLog ("%.2X !!!", source[sourceLength - 1]);
	return NULL;
}

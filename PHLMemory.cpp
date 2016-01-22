#include "PHLMemory.h"
#include <io.h>
#include <fcntl.h>

Addr OffsetManager::hWnd;
int OffsetManager::handleCRT;
FILE * OffsetManager::fileHandle;
HANDLE OffsetManager::handleOut;

CodeCave::CodeCave() : addr(NULL),
length(NULL)
{
	memset(newOpcodes, 0x90,
		MAX_ARRAY_SIZE);
	memset(oldOpcodes, 0x90,
		MAX_ARRAY_SIZE);
}

CodeCave::CodeCave(Addr addr,
	OpCode newOp)
{
	CodeCave();
	this->addr = addr;
	length = (BYTE)newOp.size();
	assignNewOpCodes(newOp);
}

void CodeCave::createCodeCave()
{
	CodeCave& cc = *this;

	if (!cc.length || !cc.addr)
	{
		printError(0x1402);
	}

	DWORD oldPermission = NULL;
	DWORD newPermission = PAGE_EXECUTE_READWRITE;

	if (!VirtualProtect((BYTE*)cc.addr, (SIZE_T)cc.length,
		newPermission, &oldPermission))
	{
		printError(0x1402);
	}

	for (BYTE i = 0x0; i < cc.length; i++)
	{
		cc.oldOpcodes[i] = *(BYTE*)(cc.addr + i);
		*(BYTE*)(cc.addr + i) = cc.newOpcodes[i];
	}

	if (!VirtualProtect((BYTE*)cc.addr, (SIZE_T)cc.length,
		oldPermission, &newPermission))
	{
		printError(0x1402);
	}
}

void CodeCave::assignNewOpCodes(OpCode newOp)
{
	int index = 0;
	for (BYTE b : newOp)
	{
		newOpcodes[index++] = b;
	}
}

HexPattern::HexPattern()
{
	memset(mask, 0x1,
		MAX_ARRAY_SIZE);
}

HexPattern::HexPattern(HexCode val)
{
	HexPattern();
	length = (BYTE)val.size();
	assignPattern(val);
}

void HexPattern::assignPattern(HexCode val)
{
	int index = 0;
	length = (BYTE)val.size();
	for (BYTE b : val)
	{
		pattern[index++] = b;
	}
}

void HexPattern::assignMask(HexCode val)
{
	int index = 0;
	for (BYTE b : val)
	{
		mask[index++] = b;
	}
}

OffsetManager::OffsetManager()
{
#ifdef _DEBUG
	// Create a console window
	// taken from:
	// justcheckingonall.wordpress.com/2008/08/29/console-window-win32-app/

	AllocConsole();

	handleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	handleCRT = _open_osfhandle((long)handleOut, _O_TEXT);
	fileHandle = _fdopen(handleCRT, "w");
	setvbuf(fileHandle, NULL, _IONBF, 1);
#endif

#pragma region start_manager
	MODULEINFO modInfo = getModuleInfo();
	base = (Addr)modInfo.lpBaseOfDll;
	moduleSize = modInfo.SizeOfImage;

	printLog("\nStarted...\n\n");

	hWnd = HWND_PATTERN_SEARCH_OFFSET +
		findPattern(HexPattern({
		0x8B, 0x06, 0xF3, 0x0F,
		0x7E, 0x44, 0x24, 0x10,
		0x66, 0x0F, 0xD6, 0x04,
		0x18, 0xF3, 0x0F, 0x7E,
		0x44, 0x24, 0x18, 0x66,
		0x0F, 0xD6, 0x44, 0x18,
		0x08, 0xF3, 0x0F, 0x7E,
		0x44, 0x24, 0x20, 0x66,
		0x0F, 0xD6, 0x44, 0x18,
		0x10, 0x47, 0x83, 0xC3
	}));
	reverseByteOrder(hWnd);
	hWnd = *(Addr*)hWnd;

	bgPatchEntry = BG_PATCH_PATTERN_SEARCH_OFFSET +
		findPattern(HexPattern({
		0x83, 0xEC, 0x68, 0x53,
		0x55, 0x8B, 0xAC, 0x24,
		0x80, 0x00, 0x00, 0x00,
		0x56, 0x33, 0xDB, 0x57,
		0x89, 0x5C, 0x24, 0x24
	}));


	windowStruct = WIN_GAME_STRUCT_SEARCH_OFFSET +
		findPattern(HexPattern({
		0x83, 0xEC, 0x3C, 0x57,
		0x8B, 0x7D, 0x08, 0x8B,
		0x07, 0x6A, 0x00, 0x51,
		0x8B, 0x88, 0xAC, 0x00,
		0x00, 0x00, 0xF3, 0x0F,
		0x11, 0x04, 0x24, 0x6A,
		0x00, 0x6A, 0x07
	}));
	//reverseByteOrder(windowStruct);
	windowStruct = *(Addr*)windowStruct;
	windowStruct = *(Addr*)windowStruct;
	//windowStruct = *(Addr*)(base + 0xA05C38);
	windowGameStruct = windowStruct +
		WIN_GAME_STRUCT_OFFSET;

	mouseHook =
		findPattern(HexPattern({
		0x8B, 0x44, 0x24, 0x18,
		0x8B, 0x4C, 0x24, 0x1C,
		0x89, 0x86, 0xE4, 0x0A,
		0x00, 0x00, 0x89, 0x8E,
		0xE8, 0x0A, 0x00, 0x00
	}));

	mouseX = windowGameStruct +
		MOUSE_X_OFFSET;
	mouseY = windowGameStruct +
		MOUSE_Y_OFFSET;

	inputHandlerFunc =
		findPattern(HexPattern({
		0x55, 0x8B, 0xEC, 0x83,
		0xE4, 0xF8, 0x83, 0xEC,
		0x4C, 0x53, 0x8B, 0x5D,
		0x08, 0x56, 0x57, 0x8B
	}));

	windowHandlerFunc =
		findPattern(HexPattern({
		0x55, 0x8B, 0xEC, 0x83,
		0xE4, 0xF8, 0x64, 0xA1,
		0x00, 0x00, 0x00, 0x00,
		0x6A, 0xFF
	}));

	wndProc =
		findPattern(HexPattern({
		0x55, 0x8B, 0xEC, 0x83,
		0xE4, 0xC0, 0x83, 0xEC,
		0x34, 0x53, 0x56
	}));

	HexPattern bypassHP({
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

	bypassHP.assignMask({
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

	bypass = findPattern(bypassHP);;
	bypass -= 0x02;

	keyStatePtr =
		findPattern(HexPattern({
		0x8B, 0x85, 0x80, 0x18,
		0x00, 0x00, 0x8B, 0x80,
		0x08, 0x01, 0x00, 0x00,
		0x80, 0xB8, 0xE0, 0x07,
		0x00, 0x00, 0x00, 0x74,
		0x05, 0x80, 0x4C, 0x24,
		0x1C, 0x02
	}));

	keyStatePtr -= 0x0E;
	keyStatePtr = readMemory(keyStatePtr);

	decryptStringFunc =
		findPattern(HexPattern({
		0x8B, 0xD1, 0x83, 0xE2,
		0x1F, 0x8A, 0x14, 0x14,
		0x32, 0xD0
	}));

	decryptStringFunc += 0x10;

	encryptedStringRegion =
		findPattern(HexPattern({
		0x12, 0xA2, 0x0D, 0xBA,
		0x64, 0x0F, 0x10, 0x51,
		0x00, 0x00, 0x00, 0x00,
		0xB5, 0x67, 0x34, 0xB1
	}));
	encryptedStringRegion -= 0x10;

	printLog("\nDecrypting Strings...\n");
	printEncryptedStrings();
	printLog("\nGetting offsets...\n");
	printLog("Base Address: %X\n"
		"Window Handle: %X\n"
		"Background Patch: %X (PathOfExile.exe + %X)\n"
		"Window Struct: %X (PathOfExile.exe + %X)\n"
		"Window/Game Struct: %X (PathOfExile.exe + %X)\n"
		"Mouse Hook Entry: %X (PathOfExile.exe + %X)\n"
		"Mouse X Address: %X (PathOfExile.exe + %X)\n"
		"Mouse Y Address: %X (PathOfExile.exe + %X)\n"
		"Input Handler Function: %X (PathOfExile.exe + %X)\n"
		"Window Handler Function: %X (PathOfExile.exe + %X)\n"
		"GetKeyState Function Ptr: %X (PathOfExile.exe + %X)\n"
		"Bypass Code Cave: %X (PathOfExile.exe + %X)\n"
		"Decrypt Function Hook: %X (PathOfExile.exe + %X)\n"
		""
		"\n",
		base, hWnd,
		bgPatchEntry,
		bgPatchEntry - base,
		windowStruct,
		windowStruct - base,
		windowGameStruct,
		windowGameStruct - base,
		mouseHook, mouseHook - base,
		mouseX, mouseX - base,
		mouseY, mouseY - base,
		inputHandlerFunc,
		inputHandlerFunc - base,
		windowHandlerFunc,
		windowHandlerFunc - base,
		keyStatePtr,
		keyStatePtr - base,
		bypass,
		bypass - base,
		decryptStringFunc,
		decryptStringFunc - base);

#pragma endregion start_manager

}

MODULEINFO OffsetManager::getModuleInfo()
{
	MODULEINFO modInfo = { 0 };
	HMODULE hModule = GetModuleHandle(POE_MODULE_NAME);
	if (hModule == 0)
	{
		printError(0x1400);
	}
	GetModuleInformation(GetCurrentProcess(),
		hModule, &modInfo, sizeof(MODULEINFO));
	return modInfo;
}

void OffsetManager::printEncryptedStrings()
{
	BYTE * iter = (BYTE*)encryptedStringRegion;

	BYTE buffer[256];
	int bufferIndex = 0;

	while ((DWORD)iter <
		encryptedStringRegion +
		ENCRYPTED_STRING_REGION_SIZE)
	{
		bufferIndex = 0;
		while (1)
		{
			buffer[bufferIndex] = *(iter++);
			if (bufferIndex)
			{
				if (buffer[bufferIndex] == 0x00 &&
					buffer[bufferIndex - 1] == 0x00)
				{
					while (*iter == 0x00)
					{
						iter++;
					}
					decryptStringA((char*)buffer);
					printLog("%s\n", buffer);
					break;
				}
			}
			bufferIndex++;
		}
	}
}

void OffsetManager::decryptStringA(char * string)
{
	__asm
	{
		mov esi, string;
		sub esp, 0x20;
		mov dword ptr[esp], 0xC35114C0;
		mov dword ptr[esp + 4], 0x883ECE77;
		mov al, 0x6B;
		mov dword ptr[esp + 8], 0x3D7C6B4A;
		mov dword ptr[esp + 12], 0x9B20CB4B;
		mov dword ptr[esp + 16], 0xA797BD8B;
		mov dword ptr[esp + 20], 0x04EFBA0F;
		mov dword ptr[esp + 24], 0x88FE173A;
		mov dword ptr[esp + 28], 0xB1A0186B;
		xor ecx, ecx;

	jump_1:
		mov al, [ecx + esi];
		test al, al;
		jnz jump_2;
		cmp[ecx + esi + 1], al;
		jz jump_3;

	jump_2:
		mov edx, ecx;
		and edx, 0x1F;
		mov dl, byte ptr[esp + edx]
			xor dl, al;
		mov[ecx + esi], dl;
		inc ecx;
		jmp jump_1;

	jump_3:
		mov eax, esi;
		add esp, 0x20;
	}
}

DWORD OffsetManager::changeMemory(Addr addr, DWORD value)
{
	if (!addr)
	{
		printError(0x1402);
	}

	DWORD oldPermission = NULL;
	DWORD newPermission = PAGE_EXECUTE_READWRITE;

	if (!VirtualProtect((BYTE*)addr, 0x4,
		newPermission, &oldPermission))
	{
		printError(0x1402);
	}

	DWORD oldVal = *(DWORD*)(addr);
	*(DWORD*)(addr) = value;

	if (!VirtualProtect((BYTE*)addr, 0x4,
		oldPermission, &oldPermission))
	{
		printError(0x1402);
	}

	return oldVal;
}

DWORD OffsetManager::readMemory(Addr addr)
{
	DWORD value = 0;
	if (!addr)
	{
		printError(0x1402);
	}

	DWORD oldPermission = NULL;
	DWORD newPermission = PAGE_EXECUTE_READWRITE;

	if (!VirtualProtect((BYTE*)addr, 0x4,
		newPermission, &oldPermission))
	{
		printError(0x1402);
	}

	value = *(DWORD*)(addr);

	if (!VirtualProtect((BYTE*)addr, 0x4,
		oldPermission, &newPermission))
	{
		printError(0x1402);
	}

	return value;
}

void OffsetManager::reverseByteOrder(Addr& addr)
{
	DWORD four = *(BYTE*)addr;
	DWORD three = *(BYTE*)(addr + 0x1) << 8;
	DWORD two = *(BYTE*)(addr + 0x2) << 16;
	DWORD one = *(BYTE*)(addr + 0x3) << 24;

	addr = 0x0;

	addr |= one | two | three | four;
}

Addr OffsetManager::getBaseAddr() const
{
	return base;
}

Addr OffsetManager::getWindowHandle() const
{
	return hWnd;
}

Addr OffsetManager::getBGPatchEntry() const
{
	return bgPatchEntry;
}

Addr OffsetManager::getBypassPatchEntry() const
{
	return bypass;
}

Addr OffsetManager::getWindowGameStruct() const
{
	return windowGameStruct;
}

Addr OffsetManager::getWindowStruct() const
{
	return windowStruct;
}

Addr OffsetManager::getInputHandlerFunc() const
{
	return inputHandlerFunc;
}

Addr OffsetManager::getWindowHandlerFunc() const
{
	return windowHandlerFunc;
}

Addr OffsetManager::getMouseHookEntry() const
{
	return mouseHook;
}

Addr OffsetManager::getMouseHookReturn() const
{
	return mouseHook
		+ MOUSE_HOOK_CODE_CAVE_SIZE;
}

Addr OffsetManager::getKeyStatePtr() const
{
	return keyStatePtr;
}

Addr OffsetManager::getDecryptStringEntry() const
{
	return decryptStringFunc;
}

Addr OffsetManager::getDecryptStringHookReturn() const
{
	return decryptStringFunc
		+ DECRYPT_STRING_HOOK_CODE_CAVE_SIZE;
}

Addr OffsetManager::findPattern(HexPattern pattern) const
{
	bool found;
	DWORD length = (DWORD)pattern.length;
	for (DWORD i = 0; i < moduleSize - length; i++)
	{
		found = true;
		for (DWORD j = 0;
		j < length && found; j++)
		{
			found &= !pattern.mask[j] ||
				pattern.pattern[j] ==
				*(BYTE*)(base + i + j);
		}
		if (found)
		{
			return base + i;
		}
	}
	printError(0x1401);
	return NULL;
}

int OffsetManager::findPattern(BYTE * source,
	int sourceLength,
	BYTE * pattern,
	int patternLength) const
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
	return NULL;
}

int OffsetManager::findAllPatterns(HexPattern pattern,
	Addr * result, int resultLength) const
{
	unsigned short resIndex = 0;
	if (!resultLength)
	{
		return 0;
	}
	bool found;
	DWORD length = (DWORD)pattern.length;
	for (DWORD i = 0; i < moduleSize - length; i++)
	{
		found = true;
		for (DWORD j = 0;
		j < length && found; j++)
		{
			found &= !pattern.mask[j] ||
				pattern.pattern[j] ==
				*(BYTE*)(base + i + j);
		}
		if (found)
		{
			*(result + resIndex) =
				base + i;
			resIndex++;
			if (resIndex == resultLength)
			{
				return resIndex;
			}
		}
	}
	return resIndex;
}

void OffsetManager::hookAddr(Addr entryAddr, BYTE patchSize,
	Addr hookFunc)
{
	CodeCave cc;
	cc.addr = entryAddr;
	cc.length = patchSize;


	int relJumpDist = (int)hookFunc -
		(int)entryAddr - 0x05;

	cc.newOpcodes[0] = 0xE9;
	*(Addr*)(cc.newOpcodes + 1) = relJumpDist;

	for (BYTE i = 0x5; i < patchSize; i++)
	{
		cc.newOpcodes[i] = 0x90;
	}

	cc.createCodeCave();
}

Addr OffsetManager::getWndProc() const
{
	return wndProc;
}

void printLog(char * format, ...)
{
#ifdef _DEBUG
	va_list argptr;
	va_start(argptr, format);
	vfprintf(OffsetManager::fileHandle,
		format, argptr);
	va_end(argptr);
#endif
}

void printError(DWORD errorCode)
{
#ifdef _DEBUG
	printLog("\nError Code: %p\n");
#endif
}
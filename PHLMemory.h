#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <initializer_list>

#define POE_MODULE_NAME "PathOfExile.exe"
#define MAX_ARRAY_SIZE 64

#define HWND_PATTERN_SEARCH_OFFSET 0x1FE
#define BG_PATCH_PATTERN_SEARCH_OFFSET 0x74
#define WIN_GAME_STRUCT_SEARCH_OFFSET 0x1B
#define WIN_GAME_STRUCT_OFFSET 0x160
#define MOUSE_X_OFFSET 0xAE4
#define MOUSE_Y_OFFSET 0xAE8

#define MOUSE_HOOK_CODE_CAVE_SIZE 0x8
#define AC_HOOK_CODE_CAVE_SIZE 0x9
#define DECRYPT_STRING_HOOK_CODE_CAVE_SIZE 0x5

typedef DWORD Addr;
typedef std::initializer_list<BYTE> HexCode;
typedef std::initializer_list<BYTE> OpCode;

class CodeCave
{
public:
	CodeCave();
	CodeCave(Addr addr,
		OpCode newOp);

	void createCodeCave();
	void assignNewOpCodes(OpCode newOp);

	Addr addr;
	BYTE newOpcodes[MAX_ARRAY_SIZE];
	BYTE oldOpcodes[MAX_ARRAY_SIZE];
	BYTE length;
};

class HexPattern
{
public:
	HexPattern();
	// Sets mask to non-zero
	HexPattern(HexCode val);

	// Zero is mask,
	// Non-zero is no mask
	void assignMask(HexCode val);
	void assignPattern(HexCode val);

	BYTE length;
	BYTE mask[MAX_ARRAY_SIZE];
	BYTE pattern[MAX_ARRAY_SIZE];
};

class OffsetManager
{
	Addr base;
	DWORD moduleSize;

	// "bg" means background
	Addr bgPatchEntry;

	Addr windowGameStruct;
	Addr windowStruct;
	Addr inputHandlerFunc;
	Addr windowHandlerFunc;
	Addr wndProc;
	Addr mouseX;
	Addr mouseY;
	Addr acHook;
	Addr mouseHook;
	Addr keyStatePtr;
	Addr decryptStringFunc;

	Addr findPattern(HexPattern pattern) const;
	int findAllPatterns(HexPattern pattern,
		Addr * result,
		int resultLength);
	MODULEINFO getModuleInfo();

public:
	OffsetManager();
	Addr getBaseAddr() const;
	Addr getWindowHandle() const;
	Addr getBGPatchEntry() const;
	Addr getWindowGameStruct() const;
	Addr getWindowStruct() const;
	Addr getInputHandlerFunc() const;
	Addr getWindowHandlerFunc() const;
	Addr getWndProc() const;
	Addr getACHookEntry() const;
	Addr getMouseHookEntry() const;
	Addr getMouseHookReturn() const;
	Addr getKeyStatePtr() const;
	Addr getDecryptStringEntry() const;
	Addr getDecryptStringHookReturn() const;

	DWORD changeMemory(Addr addr, DWORD value);
	DWORD readMemory(Addr addr);
	static void reverseByteOrder(Addr& addr);
	void hookAddr(Addr entryAddr, BYTE patchSize,
		Addr hookFunc);

	static Addr hWnd;

};

void printLog(char * format, ...);
void printError(DWORD errorCode);

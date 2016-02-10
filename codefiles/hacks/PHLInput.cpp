#include "PHLInput.h"
#include "PHLPlayer.h"
#include "PHLWindow.h"
#include <tchar.h>
#include "../PHLConsole.h"

#define MOUSE_HOOK_CODE_CAVE_SIZE 0x8
#define POE_INPUT_LAG 6
#define BG_PATCH_PATTERN_SEARCH_OFFSET 0x74

bool PHLInput::isInputLocked = false;

int PHLInput::mouseX;
int PHLInput::mouseY;
bool PHLInput::isSimulated;
Addr PHLInput::mouseHookRetAddr;
Addr PHLInput::decryptStringHookRetAddr;

UINT16 * PHLInput::keyState;

PHLInput::PHLInput ()
{

	keyState = (UINT16 *)malloc (0xFF * sizeof (UINT16));
	for (unsigned short i = 0;
	i < 0xFF; i++)
	{
		keyState[i] = 0x0000;
	}

	setInputHandlerEntry ();
	setBGPatchEntry ();
	setMouseHookEntry ();
	setKeyStatePtr ();

	enableInputInBackground ();
	hookMouse ();
	hookKeyState ();

}

void PHLInput::sendPoEMouseInput (UINT MSG,
								  int mouseX, int mouseY, int errorMargin)
{
	sendPoEInput (mouseX, mouseY, errorMargin,
				  NULL, getMouseLoc (mouseX, mouseY), MSG);
}

PHLInput::~PHLInput ()
{
	// Be careful!
	free (keyState);
}

void PHLInput::lockInput ()
{
	isSimulated = true;
	isInputLocked = true;
}

void PHLInput::unlockInput ()
{
	isSimulated = false;
	isInputLocked = false;
}

void PHLInput::enableKey (BYTE vkCode)
{
	keyState[vkCode] = (UINT16)0xFF81;
}

void PHLInput::disableKey (BYTE vkCode)
{
	keyState[vkCode] = 0;
}

void PHLInput::sendPoEKeyInput (WORD VKCode,
								int mouseX, int mouseY, int errorMargin)
{
	sendPoEInput (mouseX, mouseY, errorMargin,
				  VKCode, 0x100001, 0x100);
}

void PHLInput::sendPoEKeyUpInput (WORD VKCode,
								  int mouseX, int mouseY, int errorMargin)
{
	sendPoEInput (mouseX, mouseY, errorMargin,
				  VKCode, 0x100001, 0x101);
}

void PHLInput::sendPoEInput (int mouseX, int mouseY,
							 int errorMargin, DWORD wParam, DWORD lParam, DWORD message)
{
	isSimulated = true;
	PHLInput::mouseX = mouseX +
		rand () % (errorMargin * 2) - errorMargin;
	PHLInput::mouseY = mouseY +
		rand () % (errorMargin * 2) - errorMargin;

	Sleep (POE_INPUT_LAG);
	callPoEInputHandler (wParam, lParam, message);
	isSimulated = isInputLocked;
}

void PHLInput::callPoEInputHandler (DWORD wParam, DWORD lParam,
									DWORD message) const
{
	// Variables are used in assembly
	// ReSharper disable once CppEntityNeverUsed
	HWND hWnd = PHLWindow::Instance ()->hWnd;
	// ReSharper disable once CppEntityNeverUsed
	Addr gameStruct =
		PHLPlayer::Instance ()->gameStruct;
	// ReSharper disable once CppEntityNeverUsed
	Addr addr = inputHandlerEntry;

	__asm
	{
		push wParam;
		push message;
		push hWnd;
		push gameStruct;
		mov eax, lParam;
		call addr;
	}
}

void PHLInput::setInputHandlerEntry ()
{
	inputHandlerEntry =
		PHLMemory::findPattern (HexPattern (
			"55 8B EC 83 E4 F8 83 EC "
			"?? 53 8B 5D 08 56 57 8B "
			"7D 10"
			));
}

void PHLInput::setBGPatchEntry ()
{
	bgPatchEntry = BG_PATCH_PATTERN_SEARCH_OFFSET +
		PHLMemory::findPattern (HexPattern ({
		0x83, 0xEC, 0x68, 0x53,
		0x55, 0x8B, 0xAC, 0x24,
		0x80, 0x00, 0x00, 0x00,
		0x56, 0x33, 0xDB, 0x57,
		0x89, 0x5C, 0x24, 0x24
	}));
}

void PHLInput::setMouseHookEntry ()
{
	mouseHook =
		PHLMemory::findPattern (HexPattern (
			"8B 44 24 18 8B 4C 24 1C "
			"89 ?? E4 0A 00 00 89 ?? "
			"E8 0A 00 00"
			));
}

void PHLInput::setKeyStatePtr ()
{
	keyStatePtr = 0x2 +
		PHLMemory::findPattern (HexPattern (
			"8B 35 ?? ?? ?? ?? ?? ??"
			"?? ?? ?? ?? 6A 11 FF D6"
			"66 85 C0 ?? ?? ?? ?? ??"
			"?? 8B ?? 0C 8B ?? 04 8B"
			"?? ?? ?? 00 00 8B ?? ??"
			"?? 00 00 83 ?? F7"
			));

	keyStatePtr = PHLMemory::readAddr (keyStatePtr);
}

void PHLInput::enableInputInBackground ()
{
	// Change cmp esi, eax to cmp esi, esi
	// So that the jump after always occurs

	CodeCave (bgPatchEntry,
	{ 0x39, 0xF6 }).createCodeCave ();
}

void PHLInput::hookKeyState ()
{
	Addr funcPtr = (Addr)&hookedKeyState;
	PHLMemory::changeMemory (keyStatePtr,
							 funcPtr);
}

void PHLInput::hookMouse ()
{
	mouseHookRetAddr = mouseHook +
		MOUSE_HOOK_CODE_CAVE_SIZE;
	PHLMemory::hookAddr (mouseHook,
						 MOUSE_HOOK_CODE_CAVE_SIZE,
						 (Addr)&hookedMouse);
}

DWORD PHLInput::getMouseLoc (int x, int y)
{
	POINT p = { x, y };
	ClientToScreen (PHLWindow::Instance ()->hWnd,
					&p);
	DWORD mouseLoc = 0x0;
	mouseLoc |= HIWORD (p.x << 16);
	mouseLoc |= LOWORD (p.y);
	return mouseLoc;
}

__declspec(naked) void PHLInput::hookedMouse ()
{
	__asm
	{
		mov eax, [esp];
		cmp isSimulated, 0;
		je jump_label;
		mov eax, mouseX;
		mov ecx, mouseY;
		mov[esp + 0x18], eax;
		mov[esp + 0x1C], ecx;
	jump_label:;
		mov eax, [esp + 0x18];
		mov ecx, [esp + 0x1C];
		push mouseHookRetAddr;
		ret;
	}
}

SHORT WINAPI PHLInput::hookedKeyState (_In_ int nVirtKey)
{
	if (keyState[nVirtKey]
		|| isSimulated)
	{
		return keyState[nVirtKey];
	}
	return GetKeyState (nVirtKey);
}

void PHLInput::killPoE ()
{
	::_tsystem (_T ("taskkill /F /T /IM PathOfExile.exe"));
	Sleep (10000);
}

void PHLInput::printAddr ()
{
	Addr base = PHLMemory::Instance ()->base;
	PHLConsole::printLog (
		"Input Function:         %.8X, PathOfExile + %.8X\n"
		"BG input patch:         %.8X, PathOfExile + %.8X\n"
		"Mouse Hook:             %.8X, PathOfExile + %.8X\n"
		"Key State Ptr:          %.8X, PathOfExile + %.8X\n",
		inputHandlerEntry, inputHandlerEntry - base,
		bgPatchEntry, bgPatchEntry - base,
		mouseHook, mouseHook - base,
		keyStatePtr, keyStatePtr - base);
}
#include "PHLInput.h"
#include <TlHelp32.h>
#include <tchar.h>

int InputHandler::mouseX;
int InputHandler::mouseY;
bool InputHandler::isSimulated;
Addr InputHandler::mouseHookRetAddr;
Addr InputHandler::decryptStringHookRetAddr;

DWORD InputHandler::perm;
DWORD InputHandler::decryptStringBuff;

UINT16 * InputHandler::keyState;

InputHandler::InputHandler() :
	off(), isInputLocked(false)
{
	keyState = (UINT16 *)malloc(256 * sizeof(UINT16));
	for (int i = 0; i < 256; i++)
	{
		*(keyState + i) = 0;
	}

	hookKeyState();
	enableInputInBackground();

	mouseHookRetAddr = off.getMouseHookReturn();
	off.hookAddr(off.getMouseHookEntry(),
		MOUSE_HOOK_CODE_CAVE_SIZE,
		(Addr)&hookedMouse);

	if (!off.getACHookEntry())
	{
		MessageBox(NULL, "Failed to find anti cheat", "crap", NULL);
		Sleep(5000);
		killPoE();
	}
	
	off.hookAddr(off.getACHookEntry(),
		AC_HOOK_CODE_CAVE_SIZE,
		(Addr)&hookedAC);
		
	
	/*
	decryptStringHookRetAddr = off.getDecryptStringHookReturn();
	off.hookAddr(off.getDecryptStringEntry(),
		DECRYPT_STRING_HOOK_CODE_CAVE_SIZE,
		(Addr)&hookedDecryptString);
		*/
		
}

void InputHandler::sendPoEMouseInput(UINT MSG,
	int mouseX, int mouseY, int errorMargin)
{
	sendPoEInput(mouseX, mouseY, errorMargin,
		NULL, getMouseLoc(mouseX, mouseY), MSG);
}

InputHandler::~InputHandler()
{
	free(keyState);
}

void InputHandler::lockInput()
{
	isSimulated = true;
	isInputLocked = true;
}

void InputHandler::unlockInput()
{
	isSimulated = false;
	isInputLocked = false;
}

void InputHandler::enableKey(BYTE vkCode)
{
	keyState[vkCode] = (UINT16)0xFF81;
}

void InputHandler::disableKey(BYTE vkCode)
{
	keyState[vkCode] = 0;
}

void InputHandler::sendPoEKeyInput(WORD VKCode,
	int mouseX, int mouseY, int errorMargin)
{
	sendPoEInput(mouseX, mouseY, errorMargin,
		VKCode, 0x100001, 0x100);
}

void InputHandler::sendPoEKeyUpInput(WORD VKCode,
	int mouseX, int mouseY, int errorMargin)
{
	sendPoEInput(mouseX, mouseY, errorMargin,
		VKCode, 0x100001, 0x101);
}

void InputHandler::sendPoEInput(int mouseX, int mouseY,
	int errorMargin, DWORD wParam, DWORD lParam, DWORD message)
{
	isSimulated = true;
	InputHandler::mouseX = mouseX +
		rand() % (errorMargin * 2) - errorMargin;
	InputHandler::mouseY = mouseY +
		rand() % (errorMargin * 2) - errorMargin;

	Sleep(POE_INPUT_LAG);
	callPoEInputHandler(wParam, lParam, message);
	isSimulated = isInputLocked;
}

void InputHandler::callPoEInputHandler(DWORD wParam, DWORD lParam,
	DWORD message)
{
	Addr hWnd = off.getWindowHandle();
	Addr windowGameStruct = off.getWindowGameStruct();
	Addr addr = off.getInputHandlerFunc();

	__asm
	{
		push wParam;
		push message;
		push hWnd;
		push windowGameStruct;
		mov eax, lParam;
		call addr;
	}
}

void InputHandler::callPoEWindowHandler(DWORD wParam, DWORD lParam,
	DWORD message)
{
	PoEWindowHandlerFuncPtr func;
	Addr addr = off.getWindowHandlerFunc();
	func = (PoEWindowHandlerFuncPtr)(addr);
	func(off.getWindowStruct(),
		off.getWindowHandle(),
		message, wParam, lParam);
}

void InputHandler::enableInputInBackground()
{
	// Change cmp esi, eax to cmp esi, esi
	// So that the jump after always occurs
	CodeCave(off.getBGPatchEntry(),
	{ 0x39, 0xF6 }).createCodeCave();
}

void InputHandler::hookKeyState()
{
	Addr funcPtr = (Addr)&hookedKeyState;
	off.changeMemory(off.getKeyStatePtr(),
		funcPtr);
}

DWORD InputHandler::getMouseLoc(int x, int y)
{
	POINT p = { x, y };
	ClientToScreen((HWND)
		off.getWindowHandle(), &p);
	DWORD mouseLoc = 0x0;
	mouseLoc |= HIWORD(p.x << 16);
	mouseLoc |= LOWORD(p.y);
	return mouseLoc;
}

__declspec(naked) void InputHandler::hookedMouse()
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

__declspec(naked) void InputHandler::hookedAC()
{
	MessageBox(NULL,
		"\n\nPath Of Exile just started its anti-cheat!"
		"\nWe managed to stop it, "
		"but we are terminating path of exile now!\n\n",
		"Oh crap!", NULL);
	Sleep(4000);
	killPoE();
}

_declspec(naked) void InputHandler::hookedDecryptString()
{
	__asm
	{
		mov decryptStringBuff, esi;
	}

	printLog("%s\n",
		(char*)decryptStringBuff);

	__asm
	{
		mov eax, esi;
		add esp, 0x20;
		push decryptStringHookRetAddr;
		ret;
	}
}

SHORT WINAPI InputHandler::hookedKeyState(_In_ int nVirtKey)
{
	if (keyState[nVirtKey]
		|| isSimulated)
	{
		return keyState[nVirtKey];
	}
	return GetKeyState(nVirtKey);
}

void InputHandler::killPoE()
{
	::_tsystem(_T("taskkill /F /T /IM PathOfExile.exe"));
	Sleep(10000);
}

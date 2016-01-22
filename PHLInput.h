#pragma once

#include "PHLMemory.h"
#include <thread>

#define POE_INPUT_LAG 10

typedef char(__thiscall * PoEWindowHandlerFuncPtr)
(DWORD windowStruct, DWORD hWnd, DWORD message,
	DWORD wparam, DWORD lparam);

typedef char(__thiscall * PoESubWndProcFuncPtr)
(void * theese, DWORD windowStruct, DWORD five);

class InputHandler
{
public:
	InputHandler();
	~InputHandler();
	void sendPoEKeyInput(WORD VKCode,
		int mouseX, int mouseY, int errorMargin);
	void sendPoEKeyUpInput(WORD VKCode,
		int mouseX, int mouseY, int errorMargin);
	void sendPoEMouseInput(UINT MSG,
		int mouseX, int mouseY, int errorMargin);
	void lockInput();
	void unlockInput();
	void enableKey(BYTE vkCode);
	void disableKey(BYTE vkCode);
	static void killPoE();

private:
	void sendPoEInput(int mouseX, int mouseY, int errorMargin,
		DWORD wParam, DWORD lParam, DWORD message);
	void callPoEInputHandler(DWORD wParam, DWORD lParam,
		DWORD message);
	void callPoEWindowHandler(DWORD wParam, DWORD lParam,
		DWORD message);
	void enableInputInBackground();
	void hookKeyState();
	DWORD getMouseLoc(int x, int y);

	static void hookedMouse();
	static void hookedAC();
	static void hookedDecryptString();
	static SHORT WINAPI hookedKeyState(_In_ int nVirtKey);

	OffsetManager off;
	bool isInputLocked;

	static int mouseX;
	static int mouseY;
	static bool isSimulated;
	static Addr mouseHookRetAddr;
	static Addr decryptStringHookRetAddr;

	static DWORD perm;
	static DWORD decryptStringBuff;

	static UINT16 * keyState;

};

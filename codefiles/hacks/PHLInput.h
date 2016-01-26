#pragma once

#include "../PHLMemory.h"

typedef char(__thiscall * PoEWindowHandlerFuncPtr)
(DWORD windowStruct, DWORD hWnd, DWORD message,
	DWORD wparam, DWORD lparam);

class PHLInput : public PHLHack<PHLInput>
{
private:
	PHLInput ();
	~PHLInput ();
	
	void sendPoEInput (int mouseX, int mouseY, int errorMargin,
					   DWORD wParam, DWORD lParam, DWORD message);
	void callPoEInputHandler (DWORD wParam, DWORD lParam,
							  DWORD message);

	// Prepare the hacks
	void setInputHandlerEntry ();
	void setBGPatchEntry ();
	void setMouseHookEntry ();
	void setKeyStatePtr ();

	// Activate hacks
	void enableInputInBackground ();
	void hookKeyState ();
	void hookMouse ();

	// Gets mouse location!
	DWORD getMouseLoc (int x, int y);

	// Hooked functions
	static void hookedMouse ();
	static SHORT WINAPI hookedKeyState (_In_ int nVirtKey);

	// Static variables for hooked functions
	static bool isInputLocked;

	static int mouseX;
	static int mouseY;
	static bool isSimulated;
	static Addr mouseHookRetAddr;
	static Addr decryptStringHookRetAddr;

	static UINT16 * keyState;

	// Offsets
	Addr inputHandlerEntry;
	Addr bgPatchEntry;
	Addr mouseHook;
	Addr keyStatePtr;

public:
	friend class PHLHack<PHLInput>;

	static void killPoE ();

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
	void printAddr ();
};

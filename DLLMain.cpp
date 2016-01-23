#include "PHLInput.h"

DWORD WINAPI clickInputThread(LPVOID param)
{
	InputHandler input;
	int x = 1, y = 1;
	printLog("To left click at a location, type "
		"\"x, y\" then press enter...\n");
	while (x != 0 && y != 0)
	{
		scanLog("%d, %d", &x, &y);
		input.sendPoEMouseInput(0x201, x, y, 5);
	}
	return 0;
}

void init(HINSTANCE dllHandle)
{
	CreateThread(NULL, 2048, clickInputThread,
		NULL, NULL, NULL);
}

BOOL WINAPI DllMain(
	HINSTANCE dllHandle,  // handle to DLL module
	DWORD reason,     // reason for calling function
	LPVOID nothing)     // reserved
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		init(dllHandle);
		break;
	}
	return 1;
}
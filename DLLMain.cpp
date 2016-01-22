#include "PHLInput.h"

void clickInputThread(InputHandler * input)
{
	int x = 1, y = 1;

	while (x != 0 && y != 0)
	{
		scanLog("%d, %d", &x, &y);
		input->sendPoEMouseInput(0x201, x, y, 5);
	}
	
}

void init(HINSTANCE dllHandle)
{

	InputHandler * input = new InputHandler();
	clickInputThread(input);
	//CreateRemoteThread();
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
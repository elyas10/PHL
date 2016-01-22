#include "PHLInput.h"

void init(HINSTANCE dllHandle)
{

	InputHandler input;
	
	input.sendPoEMouseInput(0x200, 470, 350, 5);
	

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
#include "hacks/PHLInput.h"
#include "hacks/PHLBypass.h"
#include "hacks/PHLMap.h"

DWORD WINAPI init (LPVOID)
{
	PHLInput * input = PHLInput::Instance ();
	PHLConsole::printLog ("\nGetting offsets...\n\n");
	input->printAddr ();

	PHLConsole::printLog ("\nActivating map hack...\n\n");
	PHLMap::Instance ()->printAddr ();

	PHLConsole::printLog ("\nActivating bypass...\n\n");
	PHLBypass::Instance ()->printAddr ();
	PHLConsole::printLog ("\nDone\n\n");

	int x = 1, y = 1;
	PHLConsole::printLog ("To left click at a location, type "
						  "\"x, y\" then press enter... (DON'T FORGET THE COMMA)\n");
	while (x != 0 && y != 0)
	{
		PHLConsole::scanLog ("%d, %d", &x, &y);
		input->sendPoEMouseInput (0x200, x, y, 4);
		input->sendPoEMouseInput (0x201, x, y, 4);
	}
	return 0;
}

BOOL WINAPI DllMain(
	HINSTANCE dllHandle,  // handle to DLL module
	DWORD reason,     // reason for calling function
	LPVOID nothing)     // reserved
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread (NULL, 2048, init,
					  NULL, NULL, NULL);
		break;
	}
	return 1;
}
#include "hacks/PHLPlayer.h"
#include "hacks/PHLInput.h"
#include "hacks/PHLBypass.h"
#include "hacks/PHLMap.h"
#include "hacks/PHLD3DRender.h"
#include "PHLConsole.h"

DWORD WINAPI init (LPVOID)
{
	PHLInput * input = PHLInput::Instance ();

	PHLConsole::printLog ("\nGetting player offsets...\n");
	PHLPlayer::Instance ()->printAddr ();

	PHLConsole::printLog ("\nGetting input offsets...\n");
	input->printAddr ();

	PHLConsole::printLog ("\nActivating map hack...\n");
	PHLMap::Instance ()->printAddr ();

	PHLConsole::printLog ("\nActivating bypass...\n");
	PHLBypass::Instance ()->printAddr ();

	PHLConsole::printLog ("\nHooking D3D...\n");
	PHLD3DRender::Instance ()->printAddr();

	PHLConsole::printLog ("\nDone\n\n");


	int x = 1, y = 1;
	PHLConsole::printLog ("To left click at a location, type "
						  "\"x y\" then press enter... (No Comma)\n");

	PHLD3DText hello ("TEST", 300, 300, 0, 255, 0, 0, 255);

	while (x != 0 && y != 0)
	{
		PHLConsole::scanLog (" %d %d", &x, &y);
		input->sendPoEMouseInput (0x200, x, y, 4);
		input->sendPoEMouseInput (0x201, x, y, 4);
	}
	return 0;
}

BOOL WINAPI DllMain (
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
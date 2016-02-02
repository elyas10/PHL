#include "PHLD3DRender.h"

PHLD3DRender::PHLD3DRender ()
{
	// Get device
	// TODO
	d3dDevice = NULL;
}

void PHLD3DRender::printAddr ()
{
	Addr base = PHLMemory::Instance ()->base;
	PHLConsole::printLog ("D3D9 Device:            %.8X, PathOfExile + %.8X\n",
						  d3dDevice, d3dDevice - base);
}

void PHLD3DRender::renderText (int x, int y,
							   DWORD color,
							   const char * text)
{
	// TODO
}
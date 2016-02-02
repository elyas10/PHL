#pragma once

#include "../PHLMemory.h"


class PHLD3DRender : public PHLHack<PHLD3DRender>
{
	PHLD3DRender ();
public:
	friend class PHLHack<PHLD3DRender>;
	void printAddr ();

	void renderText (int x, int y,
					 DWORD color,
					 const char * str);

	Addr d3dDevice;
	
};
#pragma once

#include "../PHLMemory.h"

class PHLWindow : public PHLHack<PHLWindow>
{
private:
	PHLWindow ();
public:
	friend class PHLHack<PHLWindow>;
	void printAddr () override;

	// Window handle
	HWND hWnd;
};
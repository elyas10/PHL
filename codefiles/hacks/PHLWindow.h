#pragma once

#include "../PHLMemory.h"

class PHLWindow : public PHLHack<PHLWindow>
{
private:
	PHLWindow ();
public:
	friend class PHLHack<PHLWindow>;
	void printAddr ();

	// Window handle
	DWORD hWnd;
};
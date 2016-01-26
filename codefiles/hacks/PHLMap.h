#pragma once

#include "../PHLMemory.h"

class PHLMap : public PHLHack<PHLMap>
{
private:
	PHLMap ();
public:
	friend class PHLHack<PHLMap>;
	void printAddr ();

	// Beginning region where we apply map hack
	Addr mapHackOff;
};
#pragma once

#include "../PHLMemory.h"

class PHLBypass
{
private:

	Addr bypassA;
	Addr bypassB;
	Addr bypassC;

	static PHLBypass * phlBypass;

	PHLBypass ();
	~PHLBypass ()
	{};

	bool setBypassOffsetA ();
	bool setBypassOffsetB ();
	bool setBypassOffsetC ();
	bool activateBypassA ();
	bool activateBypassB ();
	bool activateBypassC ();

public:
	static PHLBypass * Instance ();
	PHLBypass (PHLBypass const &) = delete;
	PHLBypass & operator=(PHLBypass const &) = delete;
	void printAddr ();
};
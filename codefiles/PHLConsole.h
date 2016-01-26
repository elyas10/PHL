#pragma once

#include <string>
#include <Windows.h>

// static class
class PHLConsole
{
private:
	// Make a singleton class
	PHLConsole();
	PHLConsole(PHLConsole const & other);
	PHLConsole & operator=(PHLConsole & other);
	
	static bool PHLConsole::mainConsole;

	// Stuff for console window
	static FILE * fileHandleOut;
	static FILE * fileHandleIn;
	static HANDLE handleOut;
	static HANDLE handleIn;

	static void printWarning(std::string);
	static void init ();

public:
	static void printLog(char * format, ...);
	static void scanLog(char * format, ...);
	static void printError(const char * error);
};



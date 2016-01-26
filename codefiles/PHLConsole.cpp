#include "PHLConsole.h"
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <Psapi.h>
#include <stdio.h>

FILE * PHLConsole::fileHandleOut = NULL;
FILE * PHLConsole::fileHandleIn = NULL;
HANDLE PHLConsole::handleOut = NULL;
HANDLE PHLConsole::handleIn = NULL;

bool PHLConsole::mainConsole = false;

PHLConsole::PHLConsole (PHLConsole const & other)
{}

PHLConsole & PHLConsole::operator=(PHLConsole & other)
{
	return other;
}

PHLConsole::PHLConsole ()
{}

void PHLConsole::printWarning (std::string warning)
{
	MessageBox (NULL, warning.c_str (),
				"Warning!", NULL);
}

void PHLConsole::init ()
{
	if (!mainConsole)
	{
		// Create a console window
		// taken from:
		// justcheckingonall.wordpress.com/2008/08/29/console-window-win32-app/

		AllocConsole ();

		int crt = 0;

		handleOut = GetStdHandle (STD_OUTPUT_HANDLE);
		crt = _open_osfhandle ((long)handleOut, _O_TEXT);
		fileHandleOut = _fdopen (crt, "w");
		setvbuf (fileHandleOut, NULL, _IONBF, 1);

		handleIn = GetStdHandle (STD_INPUT_HANDLE);
		crt = _open_osfhandle ((long)handleIn, _O_TEXT);
		fileHandleIn = _fdopen (crt, "r");
		setvbuf (fileHandleIn, NULL, _IONBF, 128);

		mainConsole = true;

		return;
	}
	// printLog("Console already initialized!");
}

void PHLConsole::printLog (char * format, ...)
{
	if (!mainConsole)
	{
		init ();
	}
	va_list argptr;
	va_start (argptr, format);
	vfprintf (PHLConsole::fileHandleOut,
			  format, argptr);
	va_end (argptr);
	return;
}

void PHLConsole::scanLog (char * format, ...)
{
	if (!mainConsole)
	{
		init ();
	}
	va_list argptr;
	va_start (argptr, format);
	vfscanf (PHLConsole::fileHandleIn,
			 format, argptr);
	va_end (argptr);
	return;
}

void PHLConsole::printError (const char * error)
{
	if (!mainConsole)
	{
		init ();
	}
	printLog ("\nError: %s\n", error);
	return;

}
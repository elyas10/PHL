#pragma once

#include <Windows.h>
#include <stdio.h>
#include <initializer_list>
#include <string>
#include <vector>

#define PHL_MAX_ARRAY_SIZE 128
#define HWND_PATTERN_SEARCH_OFFSET 0x1FE

typedef DWORD Addr;

// Checks if address is non-zero and
// greater than the base address
bool isAddressValid (Addr addr);

typedef std::string HexCodeString;
typedef std::initializer_list<BYTE> HexCode;

class CodeCave
{
	void init ();
public:
	CodeCave ();
	// addr - starting location
	// newOp - opcodes to change to
	// Size of change will be determined
	// by size of newOp
	CodeCave (Addr addr,
			  HexCode newOp);

	// Applies the memory change
	// Returns false if fails, true if success
	bool createCodeCave ();
	void assignNewOpCodes (HexCode newOp);

	Addr addr;
	BYTE newOpcodes[PHL_MAX_ARRAY_SIZE];
	BYTE oldOpcodes[PHL_MAX_ARRAY_SIZE];
	BYTE length;
};

class HexPattern
{
	void init ();
public:
	// Null constructor, avoid if can
	HexPattern ();

	// Takes in array of numbers
	// in an initializer list as a constructor
	HexPattern (HexCode val);

	// Takes in a cheat engine like string
	HexPattern (std::string aob);

	// Zero is mask,
	// Non-zero is no mask
	void assignMask (HexCode val);
	// Use when using null constructor
	// Or if you want to change pattern
	void assignPattern (HexCode val);

	BYTE length;
	BYTE mask[PHL_MAX_ARRAY_SIZE];
	BYTE pattern[PHL_MAX_ARRAY_SIZE];
};

// Static class that holds basic info
// Does simple memory operations
class PHLMemory
{
	static PHLMemory * phlMem;

	PHLMemory ();
	~PHLMemory ();
	PHLMemory(PHLMemory const&);

	PHLMemory& operator=(PHLMemory const&);

public:
	// Initializes member variables
	static PHLMemory * Instance ();

	// Base address of process
	Addr base;
	// Size of main module of process
	DWORD moduleSize;

	// Flips byte order for changing little endian
	// to big endian and vice-versa
	static void reverseByteOrder (Addr& addr);

	// Changes the first four bytes of the address
	// to the value
	// Returns the old memory value
	static DWORD changeMemory (Addr addr, DWORD value);

	// Reads first four bytes at address
	// Returns first four bytes at address
	static DWORD readAddr (Addr addr);

	// Uses JMP (E9) to jump to the function
	// Needs at least five bytes to work!
	static void hookAddr (Addr entryAddr, BYTE patchSize,
						  Addr hookFunc);

	// Finds first occurrence of the hex pattern
	// Returns address to the beginning of the pattern found
	// If not found will give an error and return null
	static Addr findPattern (HexPattern pattern);
	// A more generic version of the function above
	static int findPattern (BYTE * source,
							int sourceLength,
							BYTE * pattern,
							int patternLength);
};

/*
	Read:
	en.wikipedia.org/wiki/Curiously_recurring_template_pattern
*/

template <class Derived>
class PHLHack
{
	static Derived * phlClass;
protected:
	PHLHack ()
	{};
public:
	virtual ~PHLHack()
	{
	}

	static Derived * Instance ()
	{
		if (!phlClass)
		{
			phlClass = new Derived;
		}
		return phlClass;
	}
	static void DeleteInstance ()
	{
		delete phlClass;
		phlClass = nullptr;
	}
	virtual void printAddr () = 0;
	PHLHack (PHLHack const &) = delete;
	PHLHack & operator = (PHLHack const &) = delete;
};

template <class T> T * PHLHack<T>::phlClass = nullptr;
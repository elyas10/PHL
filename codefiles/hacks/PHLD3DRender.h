#pragma once

#include "../PHLMemory.h"
#include <d3dx9.h>

struct PHLD3DText
{
	PHLD3DText (std::string text,
		int x, int y, int z,
		BYTE r, BYTE g, BYTE b,
		BYTE alpha);
	~PHLD3DText ();
	std::string text;
	int x;
	int y;
	int z;
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE alpha;
};

template <class T>
class PHLClassArray
{
	int arraySize;
	int elemSize;
	int maxSize;

	void doubleSize ()
	{
		T ** newArray = (T**)malloc (elemSize * maxSize * 2);
		for (int i = 0; i < maxSize; i++)
		{
			newArray[i] = array[i];
		}
		maxSize *= 2;
		free (array);
		array = newArray;
	}

public:
	T ** array;

	PHLClassArray () :
		arraySize (0),
		elemSize (sizeof (T*)),
		maxSize (8)
	{
		array = (T**)malloc (elemSize * maxSize);
	}
	~PHLClassArray ()
	{
		free (array);
	}
	int getSize () const
	{
		return arraySize;
	}
	void removeNullObjects ()
	{
		for (int i = 0; i < arraySize; i++)
		{
			if (!array[i])
			{
				for (int j = i;
				j < arraySize - 1; j++)
				{
					array[j] = array[j + 1];
				}
				array[--arraySize] = nullptr;
			}
		}
	}
	void insert (T * object)
	{
		removeNullObjects ();
		if (arraySize == maxSize)
		{
			doubleSize ();
		}
		array[arraySize++] = object;
	}
	void remove (T * object)
	{
		removeNullObjects ();
		for (int i = 0; i < arraySize; i++)
		{
			if (array[i] == object)
			{
				for (int j = i;
				j < arraySize - 1; j++)
				{
					array[j] = array[j + 1];
				}
				array[--arraySize] = nullptr;
				return;
			}
		}
	}
};

class PHLD3DRender : public PHLHack<PHLD3DRender>
{
	PHLD3DRender ();

	static RECT fontRect;
	static INT fontHeight;
public:
	friend class PHLHack<PHLD3DRender>;
	void printAddr () override;

	static PHLClassArray<PHLD3DText> textArray;
	static LPD3DXFONT font;
	static Addr endSceneRet;
	Addr endSceneHookEntry;

	// Hooked
	static void endSceneHook ();

	LPDIRECT3D9 d3d9Interface;
	static IDirect3DDevice9 * device;

	int xRes, yRes, refreshRate;
};
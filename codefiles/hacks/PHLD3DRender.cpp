#include "PHLD3DRender.h"
#include "../PHLConsole.h"

#define D3D9_DEVICE_SEARCH_OFFSET 0x3
#define END_SCENE_HOOK_HOOK_SIZE 0x6

PHLClassArray<PHLD3DText> PHLD3DRender::textArray;
Addr PHLD3DRender::endSceneRet = NULL;
LPD3DXFONT PHLD3DRender::font = nullptr;
RECT PHLD3DRender::fontRect = { 0 };
INT PHLD3DRender::fontHeight = NULL;

IDirect3DDevice9 * PHLD3DRender::device = nullptr;

PHLD3DText::PHLD3DText (std::string text,
	int x, int y, int z,
	BYTE r, BYTE g, BYTE b,
	BYTE alpha)
{
	this->text = text;
	this->x = x;
	this->y = y;
	this->z = z;
	this->r = r;
	this->g = g;
	this->b = b;
	this->alpha = alpha;
	PHLD3DRender::textArray.insert (this);
}

PHLD3DText::~PHLD3DText ()
{
	PHLD3DRender::textArray.remove (this);
}

PHLD3DRender::PHLD3DRender ()
{
	Addr addy;
	addy = PHLMemory::findPattern (HexPattern (
		"6A 00 68 ?? ?? ?? ?? E8 "
		"?? ?? ?? ?? 83 C4 0C B3 "
		"01")) +
		D3D9_DEVICE_SEARCH_OFFSET;

	addy = PHLMemory::readAddr (addy);

	d3d9Interface = (LPDIRECT3D9)PHLMemory::readAddr (addy);
	device = (IDirect3DDevice9 *)PHLMemory::readAddr (addy + 0x4);

	if (FAILED(D3DXCreateFont (device, 22, 0, FW_NORMAL, 1,
		FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT ("Arial"),
		&font)))
	{
		PHLConsole::printError ("Failed to create d3dx font");
	}

	xRes = (int)PHLMemory::readAddr ((Addr)device + 0x58);
	yRes = (int)PHLMemory::readAddr ((Addr)device + 0x5C);
	refreshRate = (int)PHLMemory::readAddr ((Addr)device + 0x60);

	endSceneHookEntry = PHLMemory::findPattern (HexPattern (
		"8B 82 A8 00 00 00 57 FF "
		"D0 80 3D ?? ?? ?? ?? 00"));
	endSceneRet = endSceneHookEntry +
		END_SCENE_HOOK_HOOK_SIZE;

	PHLMemory::hookAddr (endSceneHookEntry,
		END_SCENE_HOOK_HOOK_SIZE,
		(Addr)&endSceneHook);
}

void PHLD3DRender::printAddr ()
{
	Addr base = PHLMemory::Instance ()->base;
	PHLConsole::printLog (
		"D3D9 Device:            %.8X, PathOfExile + %.8X\n"
		"End Scene Hook Entry:   %.8X, PathOfExile + %.8X\n",
		device, device - base,
		endSceneHookEntry, endSceneHookEntry - base);
}

__declspec(naked) void PHLD3DRender::endSceneHook ()
{
	/*
	if (!fontHeight)
	{
		__asm
		{
			mov fontHeight, edx;
		}
		if (!VirtualProtect ((BYTE*)(fontHeight + 0xA8), 4,
			PAGE_EXECUTE_READWRITE, nullptr))
		{
			PHLConsole::printError ("Failed to get permission while "
				"attempting to gain access to device end scene");
		}
	}
	
	for (int i = 0; i < textArray.getSize (); i++)
	{
		RECT textBox =
		{
			textArray.array[i]->x,
			textArray.array[i]->y,
			1920, 1080
		};
		const char * txt = textArray.array[i]->text.c_str ();
		font->DrawTextA (nullptr, txt, -1,
						 &textBox, NULL,
						 D3DCOLOR_ARGB (textArray.array[i]->alpha,
										textArray.array[i]->r,
										textArray.array[i]->g,
										textArray.array[i]->b));
	}
	*/
	
	
	SetRect (&fontRect, 0, 0, 600, 600);
	fontHeight = font->DrawTextW (0,
		L"Hey there!", -1, &fontRect,
		DT_LEFT | DT_NOCLIP, 0xFFFFFFFF);
		

	__asm
	{
		mov eax, [edx + 0xA8];
		push endSceneRet;
		ret;
	}
}

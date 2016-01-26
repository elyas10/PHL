#pragma once

#include "../PHLMemory.h"

#define DECRYPT_STRING_HOOK_CODE_CAVE_SIZE 0x5
#define ENCRYPTED_STRING_REGION_SIZE 0x31F

// TODO:
// REWORK ENTIRE FILE

class PHLStringDecryptor
{
	Addr decryptStringFunc;
	Addr encryptedStringRegion;
	DWORD encryptedStringRegionSize;

	static DWORD decryptStringBuff;
	static DWORD decryptStringHookRetAddr;

	void decryptStringA (char * string)
	{
		__asm
		{
			mov esi, string;
			sub esp, 0x20;
			mov dword ptr[esp], 0xC35114C0;
			mov dword ptr[esp + 4], 0x883ECE77;
			mov al, 0x6B;
			mov dword ptr[esp + 8], 0x3D7C6B4A;
			mov dword ptr[esp + 12], 0x9B20CB4B;
			mov dword ptr[esp + 16], 0xA797BD8B;
			mov dword ptr[esp + 20], 0x04EFBA0F;
			mov dword ptr[esp + 24], 0x88FE173A;
			mov dword ptr[esp + 28], 0xB1A0186B;
			xor ecx, ecx;

		jump_1:
			mov al, [ecx + esi];
			test al, al;
			jnz jump_2;
			cmp[ecx + esi + 1], al;
			jz jump_3;

		jump_2:
			mov edx, ecx;
			and edx, 0x1F;
			mov dl, byte ptr[esp + edx]
				xor dl, al;
			mov[ecx + esi], dl;
			inc ecx;
			jmp jump_1;

		jump_3:
			mov eax, esi;
			add esp, 0x20;
		}
	}

	void decryptStringB (int key, int length, char * string)
	{
		__asm
		{
			mov esi, string;
			mov edi, length;
			mov edx, key;
			sub esp, 0x20;
			xor ecx, ecx;
			mov dword ptr[esp], 0x88FE173A;
			mov dword ptr[esp + 4], 0xB1A0186B;
			mov dword ptr[esp + 8], 0xA797BD8B;
			mov dword ptr[esp + 12], 0x4EFBA0F;
			mov dword ptr[esp + 16], 0x3D7C6B4A;
			mov dword ptr[esp + 20], 0x9B20CB4B;
			mov dword ptr[esp + 24], 0xC35114C0;
			mov dword ptr[esp + 28], 0x883ECE77;
			test edi, edi;
			jz jump_2;
			movzx edx, dl;
			jmp jump_1;

		jump_1:
			mov eax, edx;
			and eax, 0x1F;
			mov al, [esp + eax];
			xor[ecx + esi], al;
			inc ecx;
			inc edx;
			cmp ecx, edi;
			jb jump_1;

		jump_2:
			mov eax, esi;
			add esp, 0x20;
		}
	}

public:

	__declspec(naked) void hookedDecryptString ()
	{
		__asm
		{
			mov decryptStringBuff, esi;
		}

		PHLConsole::printLog ("%s\n",
							  (char*)decryptStringBuff);

		__asm
		{
			mov eax, esi;
			add esp, 0x20;
			push decryptStringHookRetAddr;
			ret;
		}
	}

	PHLStringDecryptor ()
	{
		decryptStringFunc =
			PHLMemory::findPattern (HexPattern ({
			0x8B, 0xD1, 0x83, 0xE2,
			0x1F, 0x8A, 0x14, 0x14,
			0x32, 0xD0
		}));

		decryptStringFunc += 0x10;

		encryptedStringRegion =
			PHLMemory::findPattern (HexPattern ({
			0x12, 0xA2, 0x0D, 0xBA,
			0x64, 0x0F, 0x10, 0x51,
			0x00, 0x00, 0x00, 0x00,
			0xB5, 0x67, 0x34, 0xB1
		}));

		encryptedStringRegion -= 0x10;
	}

	PHLStringDecryptor (DWORD encryptedStringRegion,
						DWORD encryptedStringRegionSize) :
		encryptedStringRegion (encryptedStringRegion),
		encryptedStringRegionSize (encryptedStringRegionSize)
	{}

	void hook ()
	{
		decryptStringHookRetAddr = decryptStringFunc +
			DECRYPT_STRING_HOOK_CODE_CAVE_SIZE;
		PHLMemory::hookAddr (decryptStringFunc,
							 DECRYPT_STRING_HOOK_CODE_CAVE_SIZE,
							(Addr)&hookedDecryptString);
	}

	void printEncryptedStringA ()
	{
		BYTE * iter = (BYTE*)encryptedStringRegion;

		BYTE buffer[256];
		int bufferIndex = 0;

		while ((DWORD)iter <
			   encryptedStringRegion +
			   encryptedStringRegionSize)
		{
			bufferIndex = 0;
			while (1)
			{
				buffer[bufferIndex] = *(iter++);
				if (bufferIndex)
				{
					if (buffer[bufferIndex] == 0x00 &&
						buffer[bufferIndex - 1] == 0x00)
					{
						while (*iter == 0x00)
						{
							iter++;
						}
						decryptStringA ((char*)buffer);
						PHLConsole::printLog ("%s\n", buffer);
						break;
					}
				}
				bufferIndex++;
			}
		}
	}
};
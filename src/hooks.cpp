#include "hooks.h"
#include "global.h"

#include "utils.h"

#include <Windows.h>
#include <iostream>

uint32_t on_shoot_addr = NULL;
uint32_t on_pos_addr = NULL;
uint32_t on_time_access_addr = NULL;
uint32_t on_map_addr = NULL;

//\x89\x87\x00\x00\x00\x00\xD9\xEE\xD9\x9F\x00\x00\x00\x00
//xx????xxxx????
/*
0657405E    8987 18010000   MOV DWORD PTR [EDI+118],EAX
06574064    D9EE            FLDZ
06574066    D99F 40010000   FSTP DWORD PTR [EDI+140]
*/
void find_in_mono_section(uint32_t &on_shoot, uint32_t &on_time, uint32_t &on_mapchange, bool query_map_only)
{
	on_shoot = NULL;
	on_time = NULL;
	on_mapchange = NULL;

	uint32_t start_addr = (uint32_t)GetModuleHandle(NULL);

	printf("start_addr: 0x%p\n", start_addr);

	for (;;)
	{
		MEMORY_BASIC_INFORMATION mbi;
		ZeroMemory(&mbi, sizeof(mbi));

		if (VirtualQuery((void*)start_addr, &mbi, sizeof(mbi)) == 0)
		{
			start_addr = (uint32_t)GetModuleHandle(NULL);
			continue;
		}

		if (mbi.Protect == PAGE_EXECUTE_READWRITE)
		{
			/*we gotta split this because the on_shoot/on_time mono page will get mapped once you're ingame only and we gotta set the map hook in menu already*/
			if (query_map_only)
			{
				on_mapchange = find_pattern((unsigned char*)mbi.AllocationBase, mbi.RegionSize, (const unsigned char*)"\x8B\x47\x40\x3B\xC6\x0F\x85", "xxxxxxx");

				if (on_mapchange != NULL)
					break;
			}
			else
			{
				if (on_shoot == NULL)
				{
					on_shoot = find_pattern((unsigned char*)mbi.AllocationBase, mbi.RegionSize, (const unsigned char*)"\x7A\x2A\x73\x28\x8B\x87", "xxxxxx");

					/*these two are in the same memory section as on_shoot*/
					if (on_shoot != NULL)
					{
						buddha_mode_opcode = (bool*)(find_pattern((unsigned char*)mbi.AllocationBase, mbi.RegionSize, (const unsigned char*)"\xC6\x87\x4E\x02\x00\x00\x00\xC6\x87\x64\x02\x00\x00\x00", "xxxxxxxxxxxxxx") + 6);

						if (((uint32_t)buddha_mode_opcode - 6) == NULL)
							printf("Failed to init buddha_mode %p\n", buddha_mode_opcode);
					}
				}

				if (on_time == NULL)
					on_time = find_pattern((unsigned char*)mbi.AllocationBase, mbi.RegionSize, (const unsigned char*)"\x8B\x87\x00\x00\x00\x00\x3D\x00\x00\x00\x00\x7E\x16", "xx????x????xx");

				if (on_time != NULL && on_shoot != NULL)
				{
					/*better add offset here so we don't fuck up the NULL addr checks*/
					on_shoot += 4;
					break;
				}
			}
		}

		start_addr += mbi.RegionSize;

		Sleep(3);
	}
}

void __declspec(naked) on_shoot()
{
	__asm
	{
		mov local_ball, edi
		jmp on_shoot_addr
	}
}

void __fastcall store_position(Vector *param)
{
	if (std::find(global_positions.begin(), global_positions.end(), param) == global_positions.end())
		global_positions.push_back(param);
}

void __declspec(naked) on_pos_overwrite()
{
	__asm
	{
		push ecx
		lea ecx, dword ptr[edx + 0x10]
		pushad
		call store_position
		popad
		pop ecx

		jmp on_pos_addr
	}
}

bool hook_text_section()
{
	uint32_t start_addr = (uint32_t)GetModuleHandle(NULL);

	MEMORY_BASIC_INFORMATION mbi;
	ZeroMemory(&mbi, sizeof(mbi));

	/*get size of first section*/
	if (VirtualQuery((void*)start_addr, &mbi, sizeof(mbi)) == 0)
		return false;

	start_addr += mbi.RegionSize;

	/*get size of .text section*/
	if (VirtualQuery((void*)start_addr, &mbi, sizeof(mbi)) == 0)
		return false;

	uint32_t target_addr = find_pattern((unsigned char*)start_addr, mbi.RegionSize, (const unsigned char*)"\xF3\x0F\x11\x5A\x18\x89\x55\xFC", "xxxxxxxx");

	if (target_addr == 0)
		return false;

	on_pos_addr = (uint32_t)make_detour((BYTE*)target_addr, (uint32_t)on_pos_overwrite, 5);

	if (on_pos_addr == 0)
		return false;

	return true;
}

void __declspec(naked) on_time_access()
{
	__asm
	{
		mov hud_info, edi
		jmp on_time_access_addr
	}
}

void __declspec(naked) on_map_change()
{
	__asm
	{
		mov eax, dword ptr[edi + 0x40]
		mov current_map, eax
		jmp on_map_addr
	}
}

bool hook_mono_pages()
{
	uint32_t first_mono = NULL;
	uint32_t second_mono = NULL;
	uint32_t third_mono = NULL;

	find_in_mono_section(first_mono, second_mono, third_mono, true);

	if (third_mono == NULL)
	{
		printf("Failed to find map change hook addr!\n");
		return false;
	}

	if ((on_map_addr = (uint32_t)make_detour((BYTE*)third_mono, (uint32_t)on_map_change, 5)) == NULL)
	{
		printf("Failed to hook on_map!\n");
		return false;
	}

	find_in_mono_section(first_mono, second_mono, third_mono, false);

	if (first_mono == NULL || second_mono == NULL)
	{
		printf("Failed to find on_shoot or on_time hook addr!\n");
		return false;
	}

	if ((on_shoot_addr = (uint32_t)make_detour((BYTE*)first_mono, (uint32_t)on_shoot, 6)) == NULL)
	{
		printf("Failed to hook on_shoot!\n");
		return false;
	}

	if ((on_time_access_addr = (uint32_t)make_detour((BYTE*)second_mono, (uint32_t)on_time_access, 6)) == NULL)
	{
		printf("Failed to hook on_time_access!\n");
		return false;
	}

	printf("Hooked mono pages!\n");

	return true;
}
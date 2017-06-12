#include <Windows.h>
#include <iostream>

#include "utils.h"
#include "hooks.h"
#include "gwyf.h"
#include "features.h"
#include "global.h"

/*
TODO:
glow serverside
in_hole detection
find pattern alle pattern in einem query

menu??? no
*/


void init_thread(HINSTANCE mod)
{
	if (!init_console())
	{
		MessageBox(0, L"Failed to alloc console!", L"Error!", MB_ICONERROR);
		return;
	}

	game_window = FindWindow(NULL, L"Golf With Your Friends");

	if (game_window == NULL)
	{
		printf("Failed to find window!\n");
		return;
	}

	if (!hook_text_section())
	{
		printf("Failed to hook the text section!\n");
		return;
	}

	if (!hook_mono_pages())
	{
		printf("Failed to hook mono pages!\n");
		return;
	}

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main_thread, NULL, 0, NULL);
}


BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)init_thread, hinstDll, 0, NULL);
	}

	return TRUE;
}


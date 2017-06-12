#pragma once

#include <Windows.h>
#include <stdint.h>

extern bool init_console();

extern void *make_detour(PBYTE src, DWORD dst, DWORD len);
extern uint32_t find_pattern(unsigned char* pData, unsigned int length, const unsigned char* pat, const char* msk);
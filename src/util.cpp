#include "utils.h"

#include <Psapi.h>
#include <iostream>

bool init_console()
{
	FILE *con_stream = NULL;

	if (!AttachConsole(ATTACH_PARENT_PROCESS))
	{
		if (!AllocConsole())
			return false;

		SetConsoleTitle(L"Debug");

		if (freopen_s(&con_stream, "CONOUT$", "w", stdout) != 0)
			return false;

		return true;
	}
}

/*ancient code I wrote years ago*/
void *make_detour(PBYTE src, DWORD dst, DWORD len)
{
	PBYTE op_code = new BYTE[len + 5];

	//Allocate memory for origin function call
	PVOID return_mem = VirtualAlloc(0, len + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	//We wanna jump behind the JMP @hooked function
	DWORD rel_addr2 = (DWORD)((DWORD)src - ((DWORD)return_mem + 5));

	//Backup first 5 bytes which will be overwritten
	for (int i = 0; i <= len; i++)
		op_code[i] = *(BYTE*)(src + i);

	//Add a jump
	op_code[len] = 0xE9;
	//Add the relative address
	*(DWORD*)(op_code + len + 1) = rel_addr2;

	//Memcpy to our allocated memory
	memcpy(return_mem, (PVOID)op_code, len + 5);

	//Calc hook relative address
	DWORD rel_addr = (DWORD)(dst - (DWORD)src) - 5;

	//Do regular hooking blabla
	DWORD old_protect, temp;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &old_protect);

	*src = 0xE9;

	*((DWORD *)(src + 1)) = rel_addr;

	for (DWORD x = 0x5; x < len; x++) *(src + x) = 0x90;

	VirtualProtect(src, len, old_protect, &temp);

	delete[] op_code;

	//Return pointer to allocated memory so we can call the origin later
	return return_mem;
}

/*credits: nowayz*/
uint32_t find_pattern(unsigned char* pData, unsigned int length, const unsigned char* pat, const char* msk)
{
	const unsigned char* end = pData + length - strlen(msk);
	int num_masks = ceil((float)strlen(msk) / (float)16);
	int masks[32]; //32*16 = enough masks for 512 bytes
	memset(masks, 0, num_masks * sizeof(int));

	for (int i = 0; i<num_masks; ++i)
		for (int j = strnlen(msk + i * 16, 16) - 1; j >= 0; --j)
			if (msk[i * 16 + j] == 'x')
				masks[i] |= 1 << j;

	__m128i xmm1 = _mm_loadu_si128((const __m128i *) pat);
	__m128i xmm2, xmm3, mask;

	for (; pData != end; _mm_prefetch((const char*)(++pData + 64), _MM_HINT_NTA)) {
		if (pat[0] == pData[0]) {
			xmm2 = _mm_loadu_si128((const __m128i *) pData);
			mask = _mm_cmpeq_epi8(xmm1, xmm2);
			if ((_mm_movemask_epi8(mask)&masks[0]) == masks[0]) {
				for (int i = 1; i<num_masks; ++i) {
					xmm2 = _mm_loadu_si128((const __m128i *) (pData + i * 16));
					xmm3 = _mm_loadu_si128((const __m128i *) (pat + i * 16));
					mask = _mm_cmpeq_epi8(xmm2, xmm3);

					if ((_mm_movemask_epi8(mask)&masks[i]) == masks[i]) {
						if ((i + 1) == num_masks)
							return (DWORD)pData;
					}
					else goto cont;
				}
				return (DWORD)pData;
			}
		}cont:;
	}
	return NULL;
}

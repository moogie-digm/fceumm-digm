#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "l_emu.h"
#include "l_memory.h"

#include "fceu.h"
#include "fceu-types.h"

#include "cart.h"
#include "cheat.h"



extern int CHRram[32];
extern int PRGram[32];


enum {
    SYM_RAM,
    SYM_ROM,
    SYM_WRAM,
    SYM_EXRAM,
    SYM_CHR,
    SYM_CHRRAM,
    SYM_READ,
    SYM_WRITE,
	SYM_CHRSIZE,
	SYM_CHRRAMSIZE,
    SYM_COUNT 	// Automatically tracks the size
};



// global externs needed for lua scripts
static lua_CoreSymbol lua_symbols[] = {
	[SYM_RAM]		= { "ram", NULL },		// 0x800
	[SYM_ROM]		= { "rom", NULL },
	[SYM_WRAM]		= { "wram", NULL },
	[SYM_EXRAM]		= { "exram", NULL },

	[SYM_CHR]		= { "chr", NULL },		// slot 0 = rom or ram
	[SYM_CHRRAM]	= { "chrram", NULL },	// slot 0x10 = ram
	[SYM_CHRSIZE]		= { "chrsize", NULL },
	[SYM_CHRRAMSIZE]	= { "chrramsize", NULL },

	[SYM_READ]		= { "read", NULL },		// memory map
	[SYM_WRITE] 	= { "write", NULL },
};



// Calculate the page indices (Address >> 10)
#define PAGE_5C00 (0x5C00 >> 10) // Page 23
#define PAGE_6000 (0x6000 >> 10) // Page 24
#define PAGE_7000 (0x7000 >> 10) // Page 28



void lua_memory_init() {
    lua_symbols[SYM_RAM].ptr = RAM;
    lua_symbols[SYM_ROM].ptr = PRGptr[0];


	if( MMapPtrs[PAGE_6000] ) {
		lua_symbols[SYM_WRAM].ptr = MMapPtrs[PAGE_6000];  // Normal WRAM mapped at 0x6000
	} 
	else if( MMapPtrs[PAGE_7000] ) {
		lua_symbols[SYM_WRAM].ptr = MMapPtrs[PAGE_7000];	// MMC6 RAM mapped at 0x7000
	} 

	lua_symbols[SYM_EXRAM].ptr = MMapPtrs[PAGE_5C00];	// MMC5 ExRAM mapped at 0x5C00


	lua_symbols[SYM_CHR].ptr = CHRptr[0];	// default chr-rom or chr-ram [25%]
	lua_symbols[SYM_CHRSIZE].ptr = &CHRsize[0];


	if( CHRram[0] ) {
		lua_symbols[SYM_CHRRAM].ptr = CHRptr[0];	// alias mirror
		lua_symbols[SYM_CHRRAMSIZE].ptr = &CHRsize[0];
	}
	else {
		lua_symbols[SYM_CHRRAM].ptr = CHRptr[0x10];		// secondary chip
		lua_symbols[SYM_CHRRAMSIZE].ptr = &CHRsize[0x10];
	}


	lua_symbols[SYM_READ].ptr = ARead;
	lua_symbols[SYM_WRITE].ptr = BWrite;


    lua_append_to_symbol_table(NULL, "raw_memory", lua_symbols);
}

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "l_emu.h"
#include "l_ppu.h"

#include "fceu.h"
#include "cart.h"
#include "ppu.h"



// PPU.c
extern uint8_t PALRAM[0x20], SPRAM[0x100], SPRBUF[0x100];
extern uint8_t vtoggle;
extern uint8_t XOffset;
extern uint32_t TempAddr, RefreshAddr;


/*
uint8_t *vnapage[4];	== nt0, nt1, mmc5 exram, mmc5 fill
MMC5Hack / MMC5HackExNTARAMPtr == bypass
	Line  138: #define MMC5SPRVRAMADR(V)   &MMC5SPRVPage[(V) >> 10][(V)]
	Line  139: #define VRAMADR(V)          &VPage[(V) >> 10][(V)]


cart.h
uint8 *Page[32], *VPage[8]; alongside uint8 **VPageR = VPage;
*/


// global externs needed for lua scripts
static lua_CoreSymbol lua_symbols[] = {
	//{ "scanline", &scanline },	/* it has a -16 pixel bias and will report incorrectly, don't use */
	{ "vtoggle", &vtoggle },
	{ "xofs", &XOffset },
	{ "tempaddr", &TempAddr },
	{ "refreshaddr", &RefreshAddr },

	{ "regs", &PPU },			// 4
	{ "ntaram", &NTARAM },		// 0x800
	{ "palram", &PALRAM },		// 0x20
	{ "spram", &SPRAM },		// 0x100

	{ "ntpage", &vnapage },		// 4
	{ "chrpage", &VPage },		// 8

	{ "getcycle", (void*) PPU_GetCycle },
	{ "getscanline", (void*) PPU_GetScanline },

    { NULL, NULL }
};



void lua_ppu_init() {
    lua_append_to_symbol_table(NULL, "raw_ppu", lua_symbols);
}

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "l_emu.h"

#include "fceu.h"

#include "cart.h"
#include "x6502.h"
#include "x6502struct.h"

#include "l_cpu.h"



lua_cpu_t lua_cpu = {0};
lua_cpu_d_t lua_cpu_d = {0};


extern int X6502_Run_slice(int step);



// global externs needed for lua scripts
static const lua_CoreSymbol lua_symbols[] = {
    { "regs", &X },
	{ "run", (void*) X6502_Run_slice },
	{ "rom_addr", (void*) GetRomAddress },

	{ "cpu_hooks", &lua_cpu },
	{ "cpu_d", &lua_cpu_d },

    { NULL, NULL }
};



void lua_X6502_init() {
    lua_append_to_symbol_table(NULL, "raw_cpu", lua_symbols);
}



void lua_X6502_Run() {
	lua_State *L = g_lua;
	static int cached_run_ref = LUA_NOREF;


    // globals tend to decouple across lua files - cache it instead
    if (cached_run_ref == LUA_NOREF) {
        //lua_getglobal(L, "_G");
		lua_getglobal(L, "emu");

        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, "cpu_run");

            if (lua_isfunction(L, -1)) {
                // Pin it in the registry so the GC can't move it, 
                cached_run_ref = luaL_ref(L, LUA_REGISTRYINDEX); 
            }
			else {
				//const char* error_msg = lua_tostring(L, -1);
				//printf("Lua Error: %s\n", error_msg);
                lua_pop(L, 1); // pop non-function
            }
        }
		else {
			//const char* error_msg = lua_tostring(L, -1);
			//printf("Lua Error: %s\n", error_msg);
		}
        lua_pop(L, 1); // pop _G table
    }


    lua_rawgeti(L, LUA_REGISTRYINDEX, cached_run_ref); // O(1) direct array access

	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		//const char* error_msg = lua_tostring(L, -1);
		//printf("Lua Error: %s\n", error_msg);
		lua_pop(L, 1);

		X6502_Run_slice(0);
	}
}

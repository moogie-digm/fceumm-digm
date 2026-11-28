#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "l_emu.h"

#include "l_cpu.h"
#include "l_memory.h"
#include "l_ppu.h"



static void lua_log(const char *format, ...)
{
   static char buffer[2048]; // Adjust size as needed
   va_list args;
   
   va_start(args, format);
   vsnprintf(buffer, sizeof(buffer), format, args);
   va_end(args);
   
   extern void FCEUD_Message(const char *s);
   FCEUD_Message(buffer);
}



static void lua_warn(const char *format, ...)
{
   static char buffer[2048]; // Adjust size as needed
   va_list args;
   
   va_start(args, format);
   vsnprintf(buffer, sizeof(buffer), format, args);
   va_end(args);
   
   extern void FCEUD_PrintError(const char *c);
   FCEUD_PrintError(buffer);
}



// ffi init functions
typedef void (*InitFunc)(void);


static const InitFunc lua_init_modules[] = {
    lua_memory_init,
    lua_ppu_init,
    lua_X6502_init,
    NULL
};


static void run_lua_inits(void) {
    for (int i = 0; lua_init_modules[i] != NULL; i++) {
        lua_init_modules[i]();
    }
}


// Global Lua State pointer
lua_State *g_lua = NULL;



void lua_append_to_symbol_table(lua_State *L, const char *table_name, const lua_CoreSymbol *symbols) {
	if( L == NULL )
		L = g_lua;


    lua_getglobal(L, table_name);


    // If it doesn't exist, create it
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);       // Pop the nil

        lua_newtable(L);
        lua_pushvalue(L, -1); // Duplicate for setglobal
        lua_setglobal(L, table_name);
    }


    for (int i = 0; symbols[i].name != NULL; i++) {
        lua_pushlightuserdata(L, symbols[i].ptr);
        lua_setfield(L, -2, symbols[i].name);      // Pops pointer, saves to table
    }

    
    // Clean up the stack by popping the table when done (getglobal)
    lua_pop(L, 1);
}



static int lua_panic(lua_State *L) {
    const char *msg = lua_tostring(L, -1);

    // Log the actual crash message so modders know exactly what broke
    printf("FATAL LUA PANIC: %s\n", msg ? msg : "unknown error");
    
    // Instead of crashing, gracefully terminate the process or return an exit code
    exit(1); 

    return 0;
}



void RegisterLua() {
	lua_State *L = luaL_newstate();

	if (!L) {
		lua_warn("Failed to initialize LuaJIT state.\n");
		return;
	}
	
	g_lua = L;


    luaL_openlibs(L);


	// atomic crash handler
	lua_atpanic(L, lua_panic);


	run_lua_inits();



    // modder script
	if (luaL_loadfile(L, "boot.lua") != LUA_OK) {
		printf("Syntax Error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);

		lua_close(L);
		g_lua = NULL;
		return;
	}
	else {
		// Use a strict, protected lua_pcall with an explicit error handler if desired
		// (Passing 0 arguments, LUA_MULTRET results, and 0 for no custom msgh)
		int status = lua_pcall(L, 0, LUA_MULTRET, 0); 
		if (status != LUA_OK) {
			lua_warn("Lua error: %s\n", lua_tostring(L, -1));
			//lua_pop(L, 1);  // close wipes anyway

			lua_close(L);
			g_lua = NULL;
			return;
		}
	}


	lua_log("Lua state initialized successfully.\n");
}

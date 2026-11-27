#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "l_emu.h"
#include "l_state.h"



void lua_load_state()
{
	lua_State *L = g_lua;
	static int cached_run_ref = LUA_NOREF;


    // globals tend to decouple across lua files - cache it instead
    if (cached_run_ref == LUA_NOREF) {
        //lua_getglobal(L, "_G");
		lua_getglobal(L, "emu");

        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, "loadstate_run");

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
	}
}

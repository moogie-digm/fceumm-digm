#ifndef _LUA_EMU_H
#define _LUA_EMU_H


#include "lua.h"
#include "lauxlib.h"


#if defined(__GNUC__) || defined(__clang__)
    #define likely(x)       __builtin_expect(!!(x), 1)
    #define unlikely(x)     __builtin_expect(!!(x), 0)
#else
    #define likely(x)       (x)
    #define unlikely(x)     (x)
#endif



#ifndef FORCE_INLINE
    #if defined(_MSC_VER)
        /* MSVC Specific */
        #define FORCE_INLINE __forceinline
    #elif defined(__clang__) || defined(__GNUC__)
        /* Clang and GCC Specific (using __inline__ for strict ANSI C) */
        #define FORCE_INLINE static __inline__ __attribute__((__always_inline__))
    #else
        /* Fallback for other compilers */
        #define FORCE_INLINE static __inline__
    #endif
#endif



typedef struct {
    const char *name;
    void *ptr;
} lua_CoreSymbol;



extern void RegisterLua();


extern lua_State *g_lua;


extern void lua_append_to_symbol_table(lua_State *L, const char *table_name, const lua_CoreSymbol *symbols);


#endif

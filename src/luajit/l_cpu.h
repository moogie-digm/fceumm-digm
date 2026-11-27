#ifndef LUA_X6502_H
#define LUA_X6502_H



typedef struct _lua_cpu {
	int flags;

	uint8_t hooks[0x10000 / 8];
} lua_cpu_t;



typedef struct _lua_cpu_d {
	uint16_t pc[1024];
	int pc_idx;
} lua_cpu_d_t;


	
void lua_X6502_init();
void lua_X6502_Run();


#define HOOK_PC 1
#define HOOK_READ 2
#define HOOK_WRITE 4

#define CPU_WAIT 0x100		// 1-cycle nop
#define CPU_FAST 0x200		// 0-cycle time
#define CPU_SKIP 0x400		// post-hook resume



extern lua_cpu_t lua_cpu;
extern lua_cpu_d_t lua_cpu_d;



static int lua_hook_break(int addr, int flag) {
    unsigned long index = addr >> 3;
    unsigned char pos = addr & 7;


	// check if hook activated
	int trap = lua_cpu.hooks[index] & (1 << pos);

	lua_cpu.flags |= trap ? flag : 0;


	return trap;
}



FORCE_INLINE int pc_hook_break(int pc) {
	int result = lua_hook_break(pc, HOOK_PC);

	lua_cpu_d.pc[ lua_cpu_d.pc_idx++ ] = pc;
	lua_cpu_d.pc_idx %= 1024;

	return result;
}



FORCE_INLINE int rd_hook_break(int addr) {
	return lua_hook_break(addr, HOOK_READ);
}



FORCE_INLINE int wr_hook_break(int addr) {
	return lua_hook_break(addr, HOOK_WRITE);
}



FORCE_INLINE int pc_wait_loop(int addr) {
	return lua_cpu.flags & CPU_WAIT;
}



FORCE_INLINE int pc_fast_loop(int addr) {
	return lua_cpu.flags & CPU_FAST;
}


#endif

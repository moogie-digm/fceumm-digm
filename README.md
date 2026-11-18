Clone fork of https://github.com/libretro/libretro-fceumm

Targeting HLE LuaJit detour hacking and other novelties; wildly divergent and not expected to merge upstream at all

A blend of realistic hardware hacks, virtual Lua co-processor and stuff not possible on console

Speed is decent even with detours atm, as most internals are ffi exposed to Lua side

Main special feature is pre-pc and post-pc detour hooking ==> bypass code caves hunting

Probably more useful either as rapid prototyping hacks, or ignoring real life friction like space restriction and tight cycle timing

Potential bugfixes to upstream problems will be posted as separate branches, and as discussion tabs

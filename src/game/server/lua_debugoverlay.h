// lua_debugoverlay.h
// Lua exposure for NDebugOverlay

#ifndef LUA_DEBUGOVERLAY_H
#define LUA_DEBUGOVERLAY_H

struct lua_State;

void Lua_RegisterDebugOverlay(lua_State* L);

#endif

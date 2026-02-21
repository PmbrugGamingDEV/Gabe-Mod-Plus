// lua_player.h

#ifndef LUA_PLAYER_H
#define LUA_PLAYER_H

struct lua_State;

// Registers the global "player" table
void Lua_RegisterPlayer(lua_State* L);

#endif
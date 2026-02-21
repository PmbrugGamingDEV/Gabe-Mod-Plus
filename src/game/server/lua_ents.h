#ifndef LUA_ENTS_H
#define LUA_ENTS_H

#ifdef _WIN32
#pragma once
#endif

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class CBaseEntity;

// registration
void Lua_RegisterEnts(lua_State* L);

// helpers
void Lua_PushEntity(lua_State* L, CBaseEntity* ent);
CBaseEntity* Lua_GetEntity(lua_State* L, int index);
CBaseEntity* Lua_CheckEntity(lua_State* L, int index);
int Lua_OpenEntity(lua_State* L);

#endif

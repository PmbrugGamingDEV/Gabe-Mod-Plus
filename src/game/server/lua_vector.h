#pragma once

extern "C" {
#include "lua.h"
}

#include "cbase.h"

void Lua_PushVector(lua_State* L, const Vector& vec);
Vector Lua_CheckVector(lua_State* L, int index);
int Lua_OpenVector(lua_State* L);
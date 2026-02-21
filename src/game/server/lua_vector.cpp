#include "cbase.h"
#include "lua_vector.h"

extern "C" {
#include "lauxlib.h"
}

struct LuaVector
{
    Vector vec;
};

Vector Lua_CheckVector(lua_State* L, int index)
{
    LuaVector* v = (LuaVector*)luaL_checkudata(L, index, "Vector");
    return v->vec;
}

void Lua_PushVector(lua_State* L, const Vector& vec)
{
    LuaVector* v = (LuaVector*)lua_newuserdata(L, sizeof(LuaVector));
    v->vec = vec;

    luaL_getmetatable(L, "Vector");
    lua_setmetatable(L, -2);
}

static int lua_Vector_New(lua_State* L)
{
    Vector v;
    v.x = luaL_checknumber(L, 1);
    v.y = luaL_checknumber(L, 2);
    v.z = luaL_checknumber(L, 3);

    Lua_PushVector(L, v);
    return 1;
}

static int lua_Vector_Index(lua_State* L)
{
    LuaVector* v = (LuaVector*)luaL_checkudata(L, 1, "Vector");
    const char* key = luaL_checkstring(L, 2);

    if (!Q_stricmp(key, "x")) { lua_pushnumber(L, v->vec.x); return 1; }
    if (!Q_stricmp(key, "y")) { lua_pushnumber(L, v->vec.y); return 1; }
    if (!Q_stricmp(key, "z")) { lua_pushnumber(L, v->vec.z); return 1; }

    return 0;
}

static int lua_Vector_NewIndex(lua_State* L)
{
    LuaVector* v = (LuaVector*)luaL_checkudata(L, 1, "Vector");
    const char* key = luaL_checkstring(L, 2);
    float val = luaL_checknumber(L, 3);

    if (!Q_stricmp(key, "x")) v->vec.x = val;
    if (!Q_stricmp(key, "y")) v->vec.y = val;
    if (!Q_stricmp(key, "z")) v->vec.z = val;

    return 0;
}

int Lua_OpenVector(lua_State* L)
{
    luaL_newmetatable(L, "Vector");

    lua_pushcfunction(L, lua_Vector_Index);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lua_Vector_NewIndex);
    lua_setfield(L, -2, "__newindex");

    lua_pop(L, 1);

    lua_pushcfunction(L, lua_Vector_New);
    lua_setglobal(L, "Vector");

    return 1;
}
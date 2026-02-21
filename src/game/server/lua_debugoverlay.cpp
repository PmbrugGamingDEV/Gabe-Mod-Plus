// lua_debugoverlay.cpp

#include "cbase.h"
#include "debugoverlay_shared.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "lua_debugoverlay.h"

#include "tier0/memdbgon.h"


// ------------------------------------------------------------
// Helper: read Vector from 3 consecutive Lua numbers
// ------------------------------------------------------------
static Vector Lua_CheckVector3(lua_State* L, int index)
{
    float x = luaL_checknumber(L, index);
    float y = luaL_checknumber(L, index + 1);
    float z = luaL_checknumber(L, index + 2);
    return Vector(x, y, z);
}


// ============================================================
// Line
// debugoverlay.Line(
//   sx,sy,sz,
//   ex,ey,ez,
//   r,g,b,
//   noDepth,
//   duration)
// ============================================================
static int lua_DebugOverlay_Line(lua_State* L)
{
    Vector start = Lua_CheckVector3(L, 1);
    Vector end = Lua_CheckVector3(L, 4);

    int r = luaL_checkinteger(L, 7);
    int g = luaL_checkinteger(L, 8);
    int b = luaL_checkinteger(L, 9);

    bool noDepth = lua_toboolean(L, 10) != 0;
    float duration = luaL_optnumber(L, 11, 0.0f);

    NDebugOverlay::Line(start, end, r, g, b, noDepth, duration);

    return 0;
}


// ============================================================
// Box
// debugoverlay.Box(
//   ox,oy,oz,
//   minx,miny,minz,
//   maxx,maxy,maxz,
//   r,g,b,
//   noDepth,
//   duration)
// ============================================================
static int lua_DebugOverlay_Box(lua_State* L)
{
    Vector origin = Lua_CheckVector3(L, 1);
    Vector mins = Lua_CheckVector3(L, 4);
    Vector maxs = Lua_CheckVector3(L, 7);

    int r = luaL_checkinteger(L, 10);
    int g = luaL_checkinteger(L, 11);
    int b = luaL_checkinteger(L, 12);

    bool noDepth = lua_toboolean(L, 13) != 0;
    float duration = luaL_optnumber(L, 14, 0.0f);

    NDebugOverlay::Box(origin, mins, maxs, r, g, b, 0, duration);

    return 0;
}


// ============================================================
// Sphere
// debugoverlay.Sphere(
//   x,y,z,
//   radius,
//   r,g,b,
//   noDepth,
//   duration)
// ============================================================
static int lua_DebugOverlay_Sphere(lua_State* L)
{
    Msg("Sphere printed!\n");
    // position (1-3)
    Vector origin(
        luaL_checknumber(L, 1),
        luaL_checknumber(L, 2),
        luaL_checknumber(L, 3)
    );

    // angles (4-6)
    QAngle angles(
        luaL_checknumber(L, 4),
        luaL_checknumber(L, 5),
        luaL_checknumber(L, 6)
    );

    // radius (7)
    float radius = luaL_checknumber(L, 7);

    // color (8-11)
    int r = luaL_checkinteger(L, 8);
    int g = luaL_checkinteger(L, 9);
    int b = luaL_checkinteger(L, 10);
    int a = luaL_checkinteger(L, 11);

    // duration + depth (12-13)
    float duration = luaL_optnumber(L, 12, 0.0f);
    bool noDepth = lua_toboolean(L, 13) != 0;

    NDebugOverlay::Sphere(
        origin,
        angles,
        radius,
        r, g, b, a,
        duration,
        noDepth
    );

    return 0;
}

// ============================================================
// Text
// debugoverlay.Text(
//   x,y,z,
//   "string",
//   duration)
// ============================================================
static int lua_DebugOverlay_Text(lua_State* L)
{
    Vector origin = Lua_CheckVector3(L, 1);
    const char* text = luaL_checkstring(L, 4);

    float duration = luaL_optnumber(L, 5, 0.0f);

    NDebugOverlay::Text(origin, text, false, duration);

    return 0;
}


// ============================================================
// Cross3D
// debugoverlay.Cross(
//   x,y,z,
//   size,
//   r,g,b,
//   noDepth,
//   duration)
// ============================================================
static int lua_DebugOverlay_Cross(lua_State* L)
{
    Vector origin = Lua_CheckVector3(L, 1);
    float size = luaL_checknumber(L, 4);

    int r = luaL_checkinteger(L, 5);
    int g = luaL_checkinteger(L, 6);
    int b = luaL_checkinteger(L, 7);

    bool noDepth = lua_toboolean(L, 8) != 0;
    float duration = luaL_optnumber(L, 9, 0.0f);

    NDebugOverlay::Cross3D(origin, size, r, g, b, noDepth, duration);

    return 0;
}


// ============================================================
// Registration
// ============================================================
void Lua_RegisterDebugOverlay(lua_State* L)
{
    lua_newtable(L);

#define REG(name) \
    lua_pushcfunction(L, lua_DebugOverlay_##name); \
    lua_setfield(L, -2, #name);

    REG(Line)
        REG(Box)
        REG(Sphere)
        REG(Text)
        REG(Cross)

#undef REG

        lua_setglobal(L, "debugoverlay");
}
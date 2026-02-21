#include "cbase.h"
#include "util.h"
#include "ge_gamelua.h"

// memdbgon must be last
#include "tier0/memdbgon.h"


/* ============================================================
   MACROS
   ============================================================ */

#define UTIL_FUNC_VOID(name, block) \
static int lua_Util_##name(lua_State* L) { \
    block \
    return 0; \
}

#define UTIL_FUNC_INT_EXPR(name, expr) \
static int lua_Util_##name(lua_State* L) { \
    lua_pushinteger(L, (expr)); \
    return 1; \
}

#define UTIL_FUNC_BOOL_EXPR(name, expr) \
static int lua_Util_##name(lua_State* L) { \
    lua_pushboolean(L, (expr)); \
    return 1; \
}

#define UTIL_FUNC_INT_BLOCK(name, block) \
static int lua_Util_##name(lua_State* L) { \
    int result = 0; \
    block \
    lua_pushinteger(L, result); \
    return 1; \
}

   /* ============================================================
      CORE ENTITY
      ============================================================ */

UTIL_FUNC_VOID(Remove, {
    CBaseEntity * e = UTIL_EntityByIndex(luaL_checkinteger(L,1));
    if (e) UTIL_Remove(e);
    })

    UTIL_FUNC_VOID(RemoveImmediate, {
        CBaseEntity * e = UTIL_EntityByIndex(luaL_checkinteger(L,1));
        if (e) UTIL_RemoveImmediate(e);
        })

    UTIL_FUNC_BOOL_EXPR(IsValidEntity,
        UTIL_IsValidEntity(UTIL_EntityByIndex(luaL_checkinteger(L, 1)))
    )

    UTIL_FUNC_INT_EXPR(EntityInSolid,
        UTIL_EntityInSolid(UTIL_EntityByIndex(luaL_checkinteger(L, 1)))
    )

    UTIL_FUNC_VOID(SetOrigin, {
        CBaseEntity * e = UTIL_EntityByIndex(luaL_checkinteger(L,1));
        if (!e) return 0;
        UTIL_SetOrigin(e,
            Vector(
                luaL_checknumber(L,2),
                luaL_checknumber(L,3),
                luaL_checknumber(L,4)
            ),
            false);
        })

    UTIL_FUNC_INT_EXPR(DropToFloor,
        UTIL_DropToFloor(
            UTIL_EntityByIndex(luaL_checkinteger(L, 1)),
            MASK_SOLID,
            NULL)
    )

    /* ============================================================
       PLAYER LOOKUPS
       ============================================================ */

    UTIL_FUNC_INT_BLOCK(PlayerByIndex, {
        CBasePlayer * p = UTIL_PlayerByIndex(luaL_checkinteger(L,1));
        result = p ? p->entindex() : 0;
        })

    UTIL_FUNC_INT_BLOCK(PlayerByUserId, {
        CBasePlayer * p = UTIL_PlayerByUserId(luaL_checkinteger(L,1));
        result = p ? p->entindex() : 0;
        })

    UTIL_FUNC_INT_BLOCK(PlayerByName, {
        CBasePlayer * p = UTIL_PlayerByName(luaL_checkstring(L,1));
        result = p ? p->entindex() : 0;
        })

    UTIL_FUNC_INT_EXPR(GetCommandClientIndex,
        UTIL_GetCommandClientIndex()
    )

    /* ============================================================
       VISUAL / HUD
       ============================================================ */

    UTIL_FUNC_VOID(ShowMessageAll,
        UTIL_ShowMessageAll(luaL_checkstring(L, 1));
    )

    UTIL_FUNC_VOID(SayTextAll,
        UTIL_SayTextAll(luaL_checkstring(L, 1), NULL, true);
    )

    UTIL_FUNC_VOID(ScreenShake, {
        UTIL_ScreenShake(
            Vector(
                luaL_checknumber(L,1),
                luaL_checknumber(L,2),
                luaL_checknumber(L,3)
            ),
            luaL_checknumber(L,4),
            luaL_checknumber(L,5),
            luaL_checknumber(L,6),
            luaL_checknumber(L,7),
            SHAKE_START,
            false);
        })

    UTIL_FUNC_VOID(ScreenFadeAll, {
        color32 c;
        c.r = luaL_checkinteger(L,1);
        c.g = luaL_checkinteger(L,2);
        c.b = luaL_checkinteger(L,3);
        c.a = luaL_checkinteger(L,4);

        UTIL_ScreenFadeAll(
            c,
            luaL_checknumber(L,5),
            luaL_checknumber(L,6),
            luaL_checkinteger(L,7)
        );
        })

    UTIL_FUNC_VOID(EmitAmbientSound, {
        UTIL_EmitAmbientSound(
            luaL_checkinteger(L,1),
            Vector(
                luaL_checknumber(L,2),
                luaL_checknumber(L,3),
                luaL_checknumber(L,4)
            ),
            luaL_checkstring(L,5),
            luaL_checknumber(L,6),
            (soundlevel_t)luaL_checkinteger(L,7),
            luaL_checkinteger(L,8),
            luaL_checkinteger(L,9)
        );
        })

    /* ============================================================
       BLOOD / EFFECTS
       ============================================================ */

    UTIL_FUNC_VOID(BloodStream, {
        UTIL_BloodStream(
            Vector(
                luaL_checknumber(L,1),
                luaL_checknumber(L,2),
                luaL_checknumber(L,3)
            ),
            Vector(
                luaL_checknumber(L,4),
                luaL_checknumber(L,5),
                luaL_checknumber(L,6)
            ),
            luaL_checkinteger(L,7),
            luaL_checkinteger(L,8)
        );
        })

    UTIL_FUNC_VOID(BloodSpray, {
        UTIL_BloodSpray(
            Vector(
                luaL_checknumber(L,1),
                luaL_checknumber(L,2),
                luaL_checknumber(L,3)
            ),
            Vector(
                luaL_checknumber(L,4),
                luaL_checknumber(L,5),
                luaL_checknumber(L,6)
            ),
            luaL_checkinteger(L,7),
            luaL_checkinteger(L,8),
            luaL_checkinteger(L,9)
        );
        })

    UTIL_FUNC_VOID(Smoke, {
        UTIL_Smoke(
            Vector(
                luaL_checknumber(L,1),
                luaL_checknumber(L,2),
                luaL_checknumber(L,3)
            ),
            luaL_checknumber(L,4),
            luaL_checknumber(L,5)
        );
        })

    /* ============================================================
       WORLD
       ============================================================ */

    UTIL_FUNC_INT_EXPR(WaterLevel,
        (int)UTIL_WaterLevel(
            Vector(
                luaL_checknumber(L, 1),
                luaL_checknumber(L, 2),
                luaL_checknumber(L, 3)
            ),
            luaL_checknumber(L, 4),
            luaL_checknumber(L, 5)
        )
    )

    UTIL_FUNC_INT_EXPR(FindWaterSurface,
        (int)UTIL_FindWaterSurface(
            Vector(
                luaL_checknumber(L, 1),
                luaL_checknumber(L, 2),
                luaL_checknumber(L, 3)
            ),
            luaL_checknumber(L, 4),
            luaL_checknumber(L, 5)
        )
    )

    UTIL_FUNC_VOID(SetModel, {
        CBaseEntity * e = UTIL_EntityByIndex(luaL_checkinteger(L,1));
        if (e) UTIL_SetModel(e, luaL_checkstring(L,2));
        })

    UTIL_FUNC_VOID(PrecacheOther,
    UTIL_PrecacheOther(luaL_checkstring(L, 1), NULL);
    )

CGameLuaHandle::CGameLuaHandle()
{
	Register();              // <<< CRITICAL
	m_bLuaLoaded = false;
}

CGameLuaHandle::~CGameLuaHandle()
{
}

void CGameLuaHandle::Init()
{
	Msg("[Lua] Game Lua Init\n");
}

void CGameLuaHandle::Shutdown()
{
	Msg("[Lua] Game Lua Shutdown\n");
}

void CGameLuaHandle::RegFunctions()
{
    LuaHandle *handle;
    lua_State* L = handle->GetLua();
    lua_newtable(L);

#define REG_UTIL(name) \
    lua_pushcfunction(L, lua_Util_##name); \
    lua_setfield(L, -2, #name);

    REG_UTIL(Remove)
        REG_UTIL(RemoveImmediate)
        REG_UTIL(IsValidEntity)
        REG_UTIL(EntityInSolid)
        REG_UTIL(SetOrigin)
        REG_UTIL(DropToFloor)
        REG_UTIL(PlayerByIndex)
        REG_UTIL(PlayerByUserId)
        REG_UTIL(PlayerByName)
        REG_UTIL(GetCommandClientIndex)
        REG_UTIL(ShowMessageAll)
        REG_UTIL(SayTextAll)
        REG_UTIL(ScreenShake)
        REG_UTIL(ScreenFadeAll)
        REG_UTIL(EmitAmbientSound)
        REG_UTIL(BloodStream)
        REG_UTIL(BloodSpray)
        REG_UTIL(Smoke)
        REG_UTIL(WaterLevel)
        REG_UTIL(FindWaterSurface)
        REG_UTIL(SetModel)
        REG_UTIL(PrecacheOther)

#undef REG_UTIL

        lua_setglobal(L, "util");
}

void CGameLuaHandle::RegGlobals()
{
	// Per-game globals/constants go here
}

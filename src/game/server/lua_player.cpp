// HL2SB player-related funcs

#include "cbase.h"
#include "player.h"
#include "ge_luamanager.h"
#include "in_buttons.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "tier0/memdbgon.h"

#define PLAYER_FUNC_VOID(name, call) \
static int lua_Player_##name(lua_State* L) { \
    Lua_GetPlayer(L,1)->call; \
    return 0; \
}

#define PLAYER_FUNC_BOOL(name, call) \
static int lua_Player_##name(lua_State* L) { \
    lua_pushboolean(L, Lua_GetPlayer(L,1)->call); \
    return 1; \
}

#define PLAYER_FUNC_INT(name, call) \
static int lua_Player_##name(lua_State* L) { \
    lua_pushinteger(L, Lua_GetPlayer(L,1)->call); \
    return 1; \
}

static CBasePlayer* Lua_GetPlayer(lua_State* L, int index)
{
    int entindex = luaL_checkinteger(L, index);

    CBaseEntity* pEnt = UTIL_PlayerByIndex(entindex);
    if (!pEnt)
        luaL_error(L, "Invalid player index");

    CBasePlayer* pPlayer = ToBasePlayer(pEnt);
    if (!pPlayer)
        luaL_error(L, "Entity is not a player");

    return pPlayer;
}

// ===== CLEAN PLAYER WRAPPERS =====

static int lua_Player_GiveAmmo(lua_State* L)
{
    CBasePlayer* p = Lua_GetPlayer(L, 1);
    int amount = luaL_checkinteger(L, 2);

    if (lua_type(L, 3) == LUA_TSTRING)
        lua_pushinteger(L, p->GiveAmmo(amount, luaL_checkstring(L, 3), lua_toboolean(L, 4) != 0));
    else
        lua_pushinteger(L, p->GiveAmmo(amount, luaL_checkinteger(L, 3), lua_toboolean(L, 4) != 0));

    return 1;
}

static int lua_Player_Jump(lua_State* L)
{
    CBasePlayer* p = Lua_GetPlayer(L, 1);

    p->SetGroundEntity(NULL);
    p->SetAbsVelocity(p->GetAbsVelocity() + Vector(0, 0, 250));
    return 0;
}

static int lua_Player_Duck(lua_State* L)
{
    CBasePlayer* p = Lua_GetPlayer(L, 1);

    p->m_nButtons |= IN_DUCK;

    return 0;
}

static int lua_Player_ForceRespawn(lua_State* L)
{
    Lua_GetPlayer(L, 1)->ForceRespawn();
    return 0;
}

static int lua_Player_IsDead(lua_State* L)
{
    lua_pushboolean(L, Lua_GetPlayer(L, 1)->IsDead());
    return 1;
}

static int lua_Player_GetClientIndex(lua_State* L)
{
    lua_pushinteger(L, Lua_GetPlayer(L, 1)->GetClientIndex());
    return 1;
}

static int lua_Player_SetBodyPitch(lua_State* L)
{
    Lua_GetPlayer(L, 1)->SetBodyPitch(luaL_checknumber(L, 2));
    return 0;
}

static int lua_Player_TakeHealth(lua_State* L)
{
    lua_pushinteger(L,
        Lua_GetPlayer(L, 1)->TakeHealth(
            luaL_checknumber(L, 2),
            luaL_checkinteger(L, 3)));
    return 1;
}

static int lua_Player_SetFlashlightEnabled(lua_State* L)
{
    Lua_GetPlayer(L, 1)->SetFlashlightEnabled(lua_toboolean(L, 2) != 0);
    return 0;
}

static int lua_Player_FlashlightIsOn(lua_State* L)
{
    lua_pushinteger(L, Lua_GetPlayer(L, 1)->FlashlightIsOn());
    return 1;
}

static int lua_Player_RemoveAllItems(lua_State* L)
{
    Lua_GetPlayer(L, 1)->RemoveAllItems(lua_toboolean(L, 2) != 0);
    return 0;
}

static int lua_Player_GiveNamedItem(lua_State* L)
{
    CBaseEntity* ent =
        Lua_GetPlayer(L, 1)->GiveNamedItem(
            luaL_checkstring(L, 2),
            luaL_optinteger(L, 3, 0));

    if (!ent)
        lua_pushnil(L);
    else
        lua_pushinteger(L, ent->entindex());

    return 1;
}

static int lua_Player_HasWeapons(lua_State* L)
{
    lua_pushboolean(L, Lua_GetPlayer(L, 1)->HasWeapons());
    return 1;
}

static int lua_Player_IsOnLadder(lua_State* L)
{
    lua_pushboolean(L, Lua_GetPlayer(L, 1)->IsOnLadder());
    return 1;
}

static int lua_Player_Classify(lua_State* L)
{
    lua_pushinteger(L, Lua_GetPlayer(L, 1)->Classify());
    return 1;
}

static int lua_Player_GetBonusProgress(lua_State* L)
{
    lua_pushinteger(L, Lua_GetPlayer(L, 1)->GetBonusProgress());
    return 1;
}

static int lua_Player_GetBonusChallenge(lua_State* L)
{
    lua_pushinteger(L, Lua_GetPlayer(L, 1)->GetBonusChallenge());
    return 1;
}

static int lua_Player_LockPlayerInPlace(lua_State* L)
{
    Lua_GetPlayer(L, 1)->LockPlayerInPlace();
    return 0;
}

static int lua_Player_UnlockPlayer(lua_State* L)
{
    Lua_GetPlayer(L, 1)->UnlockPlayer();
    return 0;
}

static int lua_Player_CreateViewModel(lua_State* L)
{
    Lua_GetPlayer(L, 1)->CreateViewModel(luaL_optinteger(L, 2, 0));
    return 0;
}

static int lua_Player_DestroyViewModels(lua_State* L)
{
    Lua_GetPlayer(L, 1)->DestroyViewModels();
    return 0;
}

static int lua_Player_HideViewModels(lua_State* L)
{
    Lua_GetPlayer(L, 1)->HideViewModels();
    return 0;
}

static int lua_Player_UpdateTransmitState(lua_State* L)
{
    Lua_GetPlayer(L, 1)->UpdateTransmitState();
    return 0;
}

static int lua_Player_InitHUD(lua_State* L)
{
    Lua_GetPlayer(L, 1)->InitHUD();
    return 0;
}

static int lua_Player_PlayerDeathThink(lua_State* L)
{
    Lua_GetPlayer(L, 1)->PlayerDeathThink();
    return 0;
}

static int lua_Player_DrawDebugGeometryOverlays(lua_State* L)
{
    Lua_GetPlayer(L, 1)->DrawDebugGeometryOverlays();
    return 0;
}

static int lua_Player_ForceSimulation(lua_State* L)
{
    Lua_GetPlayer(L, 1)->ForceSimulation();
    return 0;
}

static int lua_Player_IsNetClient(lua_State* L)
{
    lua_pushboolean(L, Lua_GetPlayer(L, 1)->IsNetClient());
    return 1;
}

static int lua_Player_IsFakeClient(lua_State* L)
{
    lua_pushboolean(L, Lua_GetPlayer(L, 1)->IsFakeClient());
    return 1;
}

static int lua_Player_ShouldFadeOnDeath(lua_State* L)
{
    lua_pushboolean(L, Lua_GetPlayer(L, 1)->ShouldFadeOnDeath());
    return 1;
}

static int lua_Player_GetDelayTicks(lua_State* L)
{
    lua_pushinteger(L, Lua_GetPlayer(L, 1)->GetDelayTicks());
    return 1;
}

static int lua_Player_GetReplayEntity(lua_State* L)
{
    int idx = Lua_GetPlayer(L, 1)->GetReplayEntity();
    lua_pushinteger(L, idx);
    return 1;
}

static int lua_Player_GetVehicleAnalogControlBias(lua_State* L)
{
    lua_pushinteger(L, Lua_GetPlayer(L, 1)->GetVehicleAnalogControlBias());
    return 1;
}

static int lua_Player_EnableControl(lua_State* L)
{
    Lua_GetPlayer(L, 1)->EnableControl(lua_toboolean(L, 2) != 0);
    return 0;
}

static int lua_Player_CheckTrainUpdate(lua_State* L)
{
    Lua_GetPlayer(L, 1)->CheckTrainUpdate();
    return 0;
}

static int lua_Player_InitialSpawn(lua_State* L)
{
    CBasePlayer* p = Lua_GetPlayer(L, 1);
    p->InitialSpawn();
    return 0;
}

static int lua_Player_EquipSuit(lua_State* L)
{
    Lua_GetPlayer(L, 1)->EquipSuit(lua_toboolean(L, 2) != 0);
    return 0;
}

static int lua_Player_RemoveSuit(lua_State* L)
{
    Lua_GetPlayer(L, 1)->RemoveSuit();
    return 0;
}
void Lua_RegisterPlayer(lua_State* L)
{
    lua_newtable(L);

#define REG(name) \
    lua_pushcfunction(L, lua_Player_##name); \
    lua_setfield(L, -2, #name);

    REG(GiveAmmo)
        REG(SetBodyPitch)
        REG(CreateViewModel)
        REG(HideViewModels)
        REG(DestroyViewModels)
        REG(LockPlayerInPlace)
        REG(UnlockPlayer)
        REG(DrawDebugGeometryOverlays)
        REG(UpdateTransmitState)
        REG(ForceRespawn)
        REG(InitHUD)
        REG(PlayerDeathThink)
        REG(Jump)
        REG(Duck)
        REG(ForceSimulation)
        REG(TakeHealth)
        REG(InitialSpawn)
        REG(ShouldFadeOnDeath)
        REG(IsNetClient)
        REG(IsFakeClient)
        REG(GetClientIndex)
        REG(IsDead)
        REG(IsOnLadder)
        REG(SetFlashlightEnabled)
        REG(FlashlightIsOn)
        REG(GetBonusProgress)
        REG(GetBonusChallenge)
        REG(HasWeapons)
        REG(Classify)
        REG(GetDelayTicks)
        REG(GetReplayEntity)
        REG(GetVehicleAnalogControlBias)
        REG(GiveNamedItem)
        REG(RemoveAllItems)

#undef REG

        lua_setglobal(L, "player");
}
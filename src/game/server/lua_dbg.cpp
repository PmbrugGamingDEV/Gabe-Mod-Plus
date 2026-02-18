#include "cbase.h"
#include "lua_dbg.h"
#include "ge_luamanager.h"
#include "fmtstr.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
}

// -------------------- helpers --------------------

static const char* lua_checkmsg(lua_State* L, int idx = 1)
{
	return luaL_checkstring(L, idx);
}

// -------------------- console.Msg --------------------

static int lua_Print(lua_State* L)
{
	int n = lua_gettop(L);

	for (int i = 1; i <= n; i++)
	{
		const char* s = lua_tostring(L, i);
		if (!s)
			s = lua_typename(L, lua_type(L, i));

		Msg("%s", s);

		if (i < n)
			Msg("\t");
	}

	Msg("\n");
	return 0;
}

// =======================
// cmd( "console command" )
// =======================
int luaRunCommand(lua_State* L)
{
	const char* cmd = luaL_checkstring(L, 1);

	if (!cmd || !cmd[0])
		return 0;

	// Run as server command
	engine->ServerCommand(CFmtStr("%s\n", cmd));
	engine->ServerExecute();

	return 0;
}


// -------------------- console.Warning --------------------

static int lua_console_Warning(lua_State* L)
{
	Warning("%s\n", lua_checkmsg(L));
	return 0;
}

// -------------------- console.ConMsg --------------------

static int lua_console_ConMsg(lua_State* L)
{
	ConMsg("%s\n", lua_checkmsg(L));
	return 0;
}

// -------------------- console.ConColorMsg --------------------
static int lua_console_ConColorMsg(lua_State* L)
{
	// Args:
	// 1 = color table {r, g, b [,a]}
	// 2 = string

	luaL_checktype(L, 1, LUA_TTABLE);
	const char* msg = luaL_checkstring(L, 2);

	int r = 255, g = 255, b = 255, a = 255;

	lua_rawgeti(L, 1, 1); r = luaL_optinteger(L, -1, 255); lua_pop(L, 1);
	lua_rawgeti(L, 1, 2); g = luaL_optinteger(L, -1, 255); lua_pop(L, 1);
	lua_rawgeti(L, 1, 3); b = luaL_optinteger(L, -1, 255); lua_pop(L, 1);
	lua_rawgeti(L, 1, 4); a = luaL_optinteger(L, -1, 255); lua_pop(L, 1);

	Color clr(r, g, b, a);
	ConColorMsg(clr, "%s", msg);

	return 0;
}


// -------------------- console.DevMsg --------------------
// console.DevMsg(level, "text")

static int lua_console_DevMsg(lua_State* L)
{
	int level = luaL_checkinteger(L, 1);
	const char* msg = luaL_checkstring(L, 2);
	DevMsg(level, "%s\n", msg);
	return 0;
}

// -------------------- console.Log --------------------

static int lua_console_Log(lua_State* L)
{
	Log("%s\n", lua_checkmsg(L));
	return 0;
}

// -------------------- console.Error (optional) --------------------
// WARNING: this can terminate the game.

static int lua_console_Error(lua_State* L)
{
	Error("FATAL LUA ERROR: %s\n", lua_checkmsg(L));
	return 0; // never reached
}

int lua_console_ClientPrint(lua_State* L)
{
	int idx = luaL_checkinteger(L, 1);
	int msgType = luaL_checkinteger(L, 2);
	const char* msg = luaL_checkstring(L, 3);

	CBasePlayer* pPlayer = UTIL_PlayerByIndex(idx);
	if (!pPlayer)
		return 0;

	ClientPrint(pPlayer, msgType, msg);
	return 0;
}

static int lua_console_ClientPrintAll(lua_State* L)
{
	// Args:
	// 1 = HUD_PRINT*
	// 2 = string

	int printType = luaL_checkinteger(L, 1);
	const char* msg = luaL_checkstring(L, 2);

	UTIL_ClientPrintAll(printType, msg);
	return 0;
}

void ClientActive(edict_t* pEdict)
{
	CBasePlayer* pPlayer = UTIL_PlayerByIndex(ENTINDEX(pEdict));
	if (!pPlayer)
		return;

	lua_State* L = GELua()->GetLua();
	if (!L)
		return;

	lua_getglobal(L, "OnPlayerReady");
	if (lua_isfunction(L, -1))
	{
		lua_pushlightuserdata(L, pPlayer);
		if (lua_pcall(L, 1, 0, 0) != 0)
		{
			Warning("[Lua] OnPlayerReady error: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else
	{
		lua_pop(L, 1);
	}
}

struct LuaConCommand
{
	ConCommand* cmd;
	int luaFuncRef;
};

static CUtlVector<LuaConCommand> g_LuaConCommands;


void LuaConCommandCallback(const CCommand& args)
{
	lua_State* L = GELua()->GetLua();
	if (!L)
		return;

	// Find which command triggered us
	for (int i = 0; i < g_LuaConCommands.Count(); i++)
	{
		if (!Q_stricmp(args.Arg(0), g_LuaConCommands[i].cmd->GetName()))
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, g_LuaConCommands[i].luaFuncRef);

			lua_newtable(L);
			for (int a = 0; a < args.ArgC(); a++)
			{
				lua_pushinteger(L, a + 1);
				lua_pushstring(L, args.Arg(a));
				lua_settable(L, -3);
			}

			if (lua_pcall(L, 1, 0, 0) != 0)
			{
				Warning("[Lua] ConCommand error: %s\n", lua_tostring(L, -1));
				lua_pop(L, 1);
			}
			return;
		}
	}
}

int lua_AddConCommand(lua_State* L)
{
	const char* name = luaL_checkstring(L, 1);
	const char* help = luaL_optstring(L, 2, "");
	luaL_checktype(L, 3, LUA_TFUNCTION);

	lua_pushvalue(L, 3);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);

	ConCommand* cmd = new ConCommand(
		name,
		LuaConCommandCallback,
		help,
		FCVAR_NONE
	);

	LuaConCommand entry;
	entry.cmd = cmd;
	entry.luaFuncRef = ref;

	g_LuaConCommands.AddToTail(entry);

	return 0;
}

int lua_CreateConVar(lua_State* L)
{
	const char* name = luaL_checkstring(L, 1);
	const char* def = luaL_checkstring(L, 2);
	const char* help = luaL_optstring(L, 3, "");
	int flags = luaL_optinteger(L, 4, FCVAR_NONE);

	new ConVar(name, def, flags, help);
	return 0;
}


// -------------------- registration --------------------

void Lua_RegisterConsole(lua_State* L)
{
	static const luaL_Reg console_funcs[] =
	{
		{ "Warning", lua_console_Warning },
		{ "ConMsg",  lua_console_ConMsg },
		{ "DevMsg",  lua_console_DevMsg },
		{ "Log",     lua_console_Log },
		{ "RunCmd",     luaRunCommand },
		{ "ConColorMsg", lua_console_ConColorMsg },
		{ "EngineError",   lua_console_Error }, // WARN: consider gating this
		{ "ClientPrint",    lua_console_ClientPrint },
		{ "ConCommand", lua_AddConCommand },
		{ "ConVar", lua_CreateConVar },
		{ "ClientPrintAll", lua_console_ClientPrintAll },
		{ NULL, NULL }
	};

	luaL_register(L, "console", console_funcs);
	lua_pop(L, 1);
}

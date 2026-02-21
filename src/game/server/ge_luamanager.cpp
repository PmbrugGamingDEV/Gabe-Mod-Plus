///////////// Copyright © 2008 LodleNet. All rights reserved. /////////////
//
//   Project     : Server
//   File        : ge_luamanager.cpp
//
////////////////////////////////////////////////////////////////////////////

#include "cbase.h"
#include "ge_luamanager.h"
#include "filesystem.h"
#include "player.h"

//// EXPOSURE INCLUDES ////
#include "lua_dbg.h" // Console I/O
#include "lua_ents.h" // Entity Functions
#include "lua_player.h" // Player funcs
#include "lua_debugoverlay.h" // DebugOverlay stuff

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// ------------------------------------------------------------
// Lua library helpers
// ------------------------------------------------------------
static void _luaOpenLib(lua_State* L, const char* name, lua_CFunction fn)
{
	lua_pushcfunction(L, fn);
	lua_pushstring(L, name);
	lua_call(L, 1, 0);
}

static void _luaOpenLibs(lua_State* L)
{
	_luaOpenLib(L, "", luaopen_base);
	_luaOpenLib(L, LUA_LOADLIBNAME, luaopen_package);
	_luaOpenLib(L, LUA_TABLIBNAME, luaopen_table);
	_luaOpenLib(L, LUA_STRLIBNAME, luaopen_string);
	_luaOpenLib(L, LUA_MATHLIBNAME, luaopen_math);
#ifdef _DEBUG
	_luaOpenLib(L, LUA_DBLIBNAME, luaopen_debug);
#endif
}

// ------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------
void RegisterLUAFuncs(lua_State* L);
void RegisterLUAGlobals(lua_State* L);

// ------------------------------------------------------------
// Global Lua manager
// ------------------------------------------------------------
CGELUAManager gLuaMng;

CGELUAManager* GELua()
{
	return &gLuaMng;
}

// ------------------------------------------------------------
// CGELUAManager
// ------------------------------------------------------------
CGELUAManager::CGELUAManager()
{
	m_bInit = false;
}

CGELUAManager::~CGELUAManager()
{
}

LuaHandle* CGELUAManager::GetPrimaryHandle()
{
	if (m_vHandles.empty())
		return NULL;
	return m_vHandles[0];
}

lua_State* CGELUAManager::GetLua()
{
	LuaHandle* h = GetPrimaryHandle();
	return h ? h->GetLua() : NULL;
}

void CGELUAManager::RegPublicFunctions(lua_State* L)
{
	RegisterLUAFuncs(L);
}

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

void CGELUAManager::RegPublicGlobals(lua_State* L)
{
	// HUD print constants
	lua_pushinteger(L, HUD_PRINTNOTIFY);
	lua_setglobal(L, "HUD_PRINTNOTIFY");

	lua_pushinteger(L, HUD_PRINTCONSOLE);
	lua_setglobal(L, "HUD_PRINTCONSOLE");

	lua_pushinteger(L, HUD_PRINTTALK);
	lua_setglobal(L, "HUD_PRINTTALK");

	lua_pushinteger(L, HUD_PRINTCENTER);
	lua_setglobal(L, "HUD_PRINTCENTER");

	RegisterLUAGlobals(L);
}

void CGELUAManager::InitDll()
{
	if (m_bInit)
		return;

	for (size_t i = 0; i < m_vHandles.size(); ++i)
	{
		m_vHandles[i]->InitDll();
	}

	m_bInit = true;
}

void CGELUAManager::InitHandles()
{
	for (size_t i = 0; i < m_vHandles.size(); ++i)
	{
		m_vHandles[i]->Init();
	}
}

void CGELUAManager::ShutdownDll()
{
	if (!m_bInit)
		return;

	for (size_t i = 0; i < m_vHandles.size(); ++i)
	{
		m_vHandles[i]->ShutdownDll();
	}

	m_bInit = false;
}

void CGELUAManager::ShutdownHandles()
{
	for (size_t i = 0; i < m_vHandles.size(); ++i)
	{
		m_vHandles[i]->Shutdown();
	}
}

void CGELUAManager::RegisterLuaHandle(LuaHandle* handle)
{
	if (!handle)
		return;

	for (size_t i = 0; i < m_vHandles.size(); ++i)
	{
		if (m_vHandles[i] == handle)
			return;
	}

	m_vHandles.push_back(handle);

	if (m_bInit)
		handle->InitDll();
}

void CGELUAManager::DeRegisterLuaHandle(LuaHandle* handle)
{
	if (!handle)
		return;

	for (size_t i = 0; i < m_vHandles.size(); ++i)
	{
		if (m_vHandles[i] == handle)
		{
			m_vHandles.erase(m_vHandles.begin() + i);
			break;
		}
	}
}

// ------------------------------------------------------------
// LuaHandle
// ------------------------------------------------------------
LuaHandle::LuaHandle()
{
	m_bStarted = false;
	m_bLuaLoaded = false;
	pL = NULL;
}

LuaHandle::~LuaHandle()
{
	GELua()->DeRegisterLuaHandle(this);
}

void LuaHandle::Register()
{
	GELua()->RegisterLuaHandle(this);
}

void LuaHandle::InitDll()
{
	if (m_bStarted)
		return;

	pL = luaL_newstate();
	luaL_openlibs(pL);

	RegFunctions();
	RegGlobals();

	GELua()->RegPublicFunctions(pL);
	Lua_RegisterConsole(pL);
	Lua_RegisterEnts(pL);
	Lua_RegisterPlayer(pL);
	Lua_RegisterDebugOverlay(pL);
	GELua()->RegPublicGlobals(pL);

	FileHandle_t f = filesystem->Open("lua/autorun.lua", "rb", "MOD");
	if (!f)
	{
		Warning("[Lua] autorun.lua not found (lua/autorun.lua)\n");
	}
	else
	{
		int fileSize = filesystem->Size(f);
		char* buffer = (char*)filesystem->AllocOptimalReadBuffer(f, fileSize + 1);

		filesystem->Read(buffer, fileSize, f);
		buffer[fileSize] = '\0';
		filesystem->Close(f);

		if (luaL_loadbuffer(pL, buffer, fileSize, "autorun.lua") != 0)
		{
			Warning("[Lua] autorun load error: %s\n", lua_tostring(pL, -1));
			lua_pop(pL, 1);
		}
		else if (lua_pcall(pL, 0, 0, 0) != 0)
		{
			Warning("[Lua] autorun runtime error: %s\n", lua_tostring(pL, -1));
			lua_pop(pL, 1);
		}

		filesystem->FreeOptimalReadBuffer(buffer);
	}

	m_bLuaLoaded = true;
	Msg("[Lua] Has Initialized....\n");
	m_bStarted = true;
}

void LuaHandle::ShutdownDll()
{
	if (!m_bStarted)
		return;

	Shutdown();
	lua_close(pL);

	m_bStarted = false;
	m_bLuaLoaded = false;
}

void Lua_PushPlayer(lua_State* L, CBasePlayer* pPlayer)
{
	if (!pPlayer)
	{
		lua_pushnil(L);
		return;
	}

	lua_pushinteger(L, pPlayer->entindex());
}

// ------------------------------------------------------------
// Gameplay → Lua bridge
// ------------------------------------------------------------
void CallLua_OnPlayerReady(CBasePlayer* pPlayer)
{
	LuaHandle* lh = GELua()->GetPrimaryHandle();
	if (!lh || !lh->m_bLuaLoaded)
		return;

	lua_State* L = lh->GetLua();

	lua_getglobal(L, "OnPlayerReady");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		return;
	}

	Lua_PushPlayer(L, pPlayer);
	CallLUA(L, 1, 0, 0, "OnPlayerReady");
}

// ------------------------------------------------------------
// Public registration hooks
// ------------------------------------------------------------

int luaGetTime(lua_State* L)
{
	lua_pushnumber(L, gpGlobals->curtime);
	return 1;
}

void RegisterLUAFuncs(lua_State* L)
{
	lua_pushcfunction(L, lua_Print);
	lua_setglobal(L, "print");
	lua_pushcfunction(L, luaGetTime);
	lua_setglobal(L, "getcurtime");
}

void RegisterLUAGlobals(lua_State* L)
{

}

/*
====================================================
 lua_dofile <file>
====================================================
*/
CON_COMMAND(lua_dofile, "Execute a Lua file")
{
	lua_State* L = GELua()->GetLua();
	if (!L)
	{
		Warning("[Lua] No Lua state\n");
		return;
	}

	if (args.ArgC() < 2)
	{
		Msg("Usage: lua_dofile <file>\n");
		return;
	}

	const char* filename = args.Arg(1);

	// Open via Source filesystem
	FileHandle_t fh = filesystem->Open(filename, "r", "MOD");
	if (!fh)
	{
		Warning("[Lua] cannot open %s\n", filename);
		return;
	}

	int size = filesystem->Size(fh);
	char* buffer = new char[size + 1];

	filesystem->Read(buffer, size, fh);
	buffer[size] = '\0';
	filesystem->Close(fh);

	// Load Lua chunk from buffer
	if (luaL_loadbuffer(L, buffer, size, filename) != 0)
	{
		Warning("[Lua] load error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	if (lua_pcall(L, 0, 0, 0) != 0)
	{
		Warning("[Lua] runtime error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	Msg("[Lua] executed %s\n", filename);
}

/*
====================================================
 lua_dostring "<code>"
====================================================
*/
CON_COMMAND(lua_dostring, "Execute inline Lua code")
{
	lua_State* L = GELua()->GetLua();
	if (!L)
		return;

	if (args.ArgC() < 2)
	{
		Msg("Usage: lua_dostring \"code\"\n");
		return;
	}

	if (luaL_loadstring(L, args.Arg(1)) != 0)
	{
		Warning("[Lua] load error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	if (lua_pcall(L, 0, 0, 0) != 0)
	{
		Warning("[Lua] runtime error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}
}

/*
====================================================
 lua_reload
====================================================
*/
CON_COMMAND(lua_reload, "Reload lua/autorun.lua")
{
	lua_State* L = GELua()->GetLua();
	if (!L)
	{
		Warning("[Lua] No Lua state\n");
		return;
	}

	const char* filename = "lua/autorun.lua";

	FileHandle_t fh = filesystem->Open(filename, "r", "MOD");
	if (!fh)
	{
		Warning("[Lua] autorun load error: cannot open %s\n", filename);
		return;
	}

	int size = filesystem->Size(fh);
	char* buffer = (char*)stackalloc(size + 1);

	filesystem->Read(buffer, size, fh);
	buffer[size] = '\0';
	filesystem->Close(fh);

	if (luaL_loadbuffer(L, buffer, size, filename) != 0)
	{
		Warning("[Lua] autorun load error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	if (lua_pcall(L, 0, 0, 0) != 0)
	{
		Warning("[Lua] autorun runtime error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	Msg("[Lua] autorun reloaded\n");
}

/*
====================================================
 lua_status
====================================================
*/
CON_COMMAND(lua_status, "Print Lua VM status")
{
	lua_State* L = GELua()->GetLua();
	if (!L)
	{
		Msg("[Lua] not running\n");
		return;
	}

	Msg("[Lua] VM active | stack top = %d\n", lua_gettop(L));
}

/*
====================================================
 lua_stackdump
====================================================
*/
CON_COMMAND(lua_stackdump, "Dump Lua stack")
{
	lua_State* L = GELua()->GetLua();
	if (!L)
		return;

	int top = lua_gettop(L);
	Msg("[Lua] Stack (%d):\n", top);

	for (int i = 1; i <= top; i++)
	{
		Msg(" %d: %s\n", i, lua_typename(L, lua_type(L, i)));
	}
}

/*
====================================================
 lua_globals
====================================================
*/
CON_COMMAND(lua_globals, "List Lua globals")
{
	lua_State* L = GELua()->GetLua();
	if (!L)
		return;

	lua_getglobal(L, "_G");
	lua_pushnil(L);

	while (lua_next(L, -2))
	{
		const char* key = lua_tostring(L, -2);
		if (key)
			Msg("  %s\n", key);

		lua_pop(L, 1);
	}

	lua_pop(L, 1);
}

/*
====================================================
 lua_gc
====================================================
*/
CON_COMMAND(lua_gc, "Force Lua garbage collection")
{
	lua_State* L = GELua()->GetLua();
	if (!L)
		return;

	lua_gc(L, LUA_GCCOLLECT, 0);
	Msg("[Lua] garbage collection complete\n");
}

/*
====================================================
 lua_call <function>
====================================================
*/
CON_COMMAND(lua_call, "Call a Lua global function")
{
	lua_State* L = GELua()->GetLua();
	if (!L)
		return;

	if (args.ArgC() < 2)
	{
		Msg("Usage: lua_call <function>\n");
		return;
	}

	lua_getglobal(L, args.Arg(1));
	if (!lua_isfunction(L, -1))
	{
		Warning("[Lua] %s is not a function\n", args.Arg(1));
		lua_pop(L, 1);
		return;
	}

	if (lua_pcall(L, 0, 0, 0) != 0)
	{
		Warning("[Lua] error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}
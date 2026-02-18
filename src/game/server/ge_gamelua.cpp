#include "cbase.h"
#include "ge_gamelua.h"

// memdbgon must be last
#include "tier0/memdbgon.h"

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
	// Per-game Lua functions go here
	// (console functions are already registered in LuaHandle::InitDll)
}

void CGameLuaHandle::RegGlobals()
{
	// Per-game globals/constants go here
}

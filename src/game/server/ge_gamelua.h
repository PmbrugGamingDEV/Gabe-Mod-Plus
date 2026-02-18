#pragma once

#include "cbase.h"
#include "ge_luamanager.h"

// Concrete Lua handle for the game
class CGameLuaHandle : public LuaHandle
{
public:
	CGameLuaHandle();
	virtual ~CGameLuaHandle();

	virtual void Init() OVERRIDE;
	virtual void Shutdown() OVERRIDE;

	virtual void RegFunctions() OVERRIDE;
	virtual void RegGlobals() OVERRIDE;
};

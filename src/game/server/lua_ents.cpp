#include "cbase.h"
#include "lua_ents.h"
#include "eventqueue.h"
#include "variant_t.h" 
#include "tier0/memdbgon.h"

#define LUA_ENTITY_MT "Entity"

/*
===============================================================================
 Lua Entity API – Source SDK Base 2007
===============================================================================

This file exposes Source engine entities to Lua with a lightweight userdata
wrapper and provides GMod-style entity control, I/O firing, and lookup helpers.

------------------------------------------------------------------------------
 GLOBAL FUNCTIONS
------------------------------------------------------------------------------

ent_fire( targetName, inputName, [value], [delay] )
	• Fires an entity input by TARGET NAME.
	• Equivalent to Hammer I/O or console ent_fire.
	• value: string passed as variant_t (optional, default "")
	• delay: seconds before firing (optional, default 0)

	Example:
		ent_fire("door1", "Open")
		ent_fire("barrel", "Color", "255 0 0", 0.5)

------------------------------------------------------------

ent_fire_by_id( entIndex, inputName, [value], [delay] )
	• Fires an entity input using a DIRECT ENTITY INDEX.
	• Safer and faster than name-based firing.
	• No name lookup involved.

	Example:
		ent_fire_by_id(42, "Kill")

------------------------------------------------------------

picker()
	• Returns the entity currently under the local player's crosshair.
	• Uses a trace from the player’s eye position.
	• Returns nil if nothing is hit.

	Example:
		local e = picker()
		if e then e:Fire("Kill") end

------------------------------------------------------------------------------
 ents TABLE
------------------------------------------------------------------------------

ents.FindByName( name )
	• Returns a table of entities matching a targetname.
	• Uses gEntList.FindEntityByName.
	• Order is engine iteration order.

	Example:
		for _, e in ipairs(ents.FindByName("barrel")) do
			e:Fire("Ignite")
		end

------------------------------------------------------------

ents.FindByClass( classname )
	• Returns a table of entities matching a classname.
	• Uses gEntList.FindEntityByClassname.

------------------------------------------------------------

ents.GetAll()
	• Returns a table containing ALL valid entities in the world.
	• Includes players, worldspawn, props, NPCs, etc.
	• Use sparingly (can be expensive).

------------------------------------------------------------

ents.GetByIndex( index )
	• Returns the entity with the given entindex.
	• Returns nil if invalid.

------------------------------------------------------------------------------
 Entity USERDATA METHODS
------------------------------------------------------------------------------

Entity:Fire( inputName, [value], [delay] )
	• Fires an input directly on this entity.
	• Uses g_EventQueue internally.
	• activator and caller are set to the entity itself.

	Example:
		ent:Fire("Kill")
		ent:Fire("Color", "0 255 0", 0.2)

------------------------------------------------------------

Entity:IsValid()
	• Returns true if the entity exists and is not marked for deletion.
	• ALWAYS check this before acting on stored entities.

------------------------------------------------------------

Entity:EntIndex()
	• Returns the engine entity index.
	• Useful for networking, debugging, or ent_fire_by_id.

------------------------------------------------------------

Entity:GetClass()
	• Returns the entity classname.

------------------------------------------------------------

Entity:GetName()
	• Returns the entity targetname (may be empty).

------------------------------------------------------------

Entity:GetPos()
	• Returns a table { x, y, z } representing world position.

------------------------------------------------------------

Entity:SetPos( vecTable )
	• Sets world position.
	• vecTable must contain x, y, z fields.

------------------------------------------------------------

Entity:Remove()
	• Safely removes the entity from the world using UTIL_Remove.

------------------------------------------------------------------------------
 NOTES
------------------------------------------------------------------------------

• Lua entity userdata stores a raw CBaseEntity* pointer.
• No automatic lifetime management — ALWAYS use Entity:IsValid().
• All input firing uses Source's event queue (supports delays).
• This API is designed to mirror Source I/O and GMod-style scripting.

===============================================================================
*/


// =======================
// Entity userdata helpers
// =======================

void Lua_PushEntity(lua_State* L, CBaseEntity* ent)
{
	if (!ent)
	{
		lua_pushnil(L);
		return;
	}

	CBaseEntity** udata = (CBaseEntity**)lua_newuserdata(L, sizeof(CBaseEntity*));
	*udata = ent;

	luaL_getmetatable(L, LUA_ENTITY_MT);
	lua_setmetatable(L, -2);
}

CBaseEntity* Lua_GetEntity(lua_State* L, int index)
{
	CBaseEntity** udata = (CBaseEntity**)luaL_checkudata(L, index, LUA_ENTITY_MT);
	return udata ? *udata : NULL;
}

// =======================
// ent_fire
// =======================

int luaEntFire(lua_State* L)
{
	const char* target = luaL_checkstring(L, 1);
	const char* action = luaL_checkstring(L, 2); // ← REQUIRED

	const char* valueStr = "";
	float delay = 0.0f;

	if (lua_gettop(L) >= 3 && lua_isstring(L, 3))
		valueStr = lua_tostring(L, 3);

	if (lua_gettop(L) >= 4 && lua_isnumber(L, 4))
		delay = lua_tonumber(L, 4);

	variant_t value;
	value.SetString(MAKE_STRING(valueStr));

	g_EventQueue.AddEvent(
		target,    // target entity name
		action,   // INPUT NAME (REQUIRED)
		value,
		delay,
		NULL,     // activator
		NULL,     // caller
		0
	);

	return 0;
}

// =======================
// ent_fire_by_id
// =======================
int luaEntFireByID(lua_State* L)
{
	int entIndex = luaL_checkinteger(L, 1);
	const char* action = luaL_checkstring(L, 2);

	const char* valueStr = "";
	float delay = 0.0f;

	if (lua_gettop(L) >= 3 && lua_isstring(L, 3))
		valueStr = lua_tostring(L, 3);

	if (lua_gettop(L) >= 4 && lua_isnumber(L, 4))
		delay = lua_tonumber(L, 4);

	CBaseEntity* ent = UTIL_EntityByIndex(entIndex);
	if (!ent)
		return 0;

	variant_t value;
	value.SetString(MAKE_STRING(valueStr));

	g_EventQueue.AddEvent(
		ent,        // 🔑 entity pointer overload
		action,
		value,
		delay,
		NULL,
		NULL,
		0
	);

	return 0;
}

// =======================
// Entity:Fire()
// =======================

int luaEntity_Fire(lua_State* L)
{
	CBaseEntity* ent = Lua_GetEntity(L, 1);
	const char* action = luaL_checkstring(L, 2);

	const char* valueStr = "";
	float delay = 0.0f;

	if (!ent)
		return 0;

	if (lua_gettop(L) >= 3 && lua_isstring(L, 3))
		valueStr = lua_tostring(L, 3);

	if (lua_gettop(L) >= 4 && lua_isnumber(L, 4))
		delay = lua_tonumber(L, 4);

	variant_t value;
	value.SetString(MAKE_STRING(valueStr));

	g_EventQueue.AddEvent(
		ent,
		action,
		value,
		delay,
		ent,
		ent,
		0
	);

	return 0;
}

// =======================
// ents.FindByName
// =======================

int luaFindByName(lua_State* L)
{
	const char* name = luaL_checkstring(L, 1);

	lua_newtable(L);

	int idx = 1;
	CBaseEntity* ent = NULL;

	while ((ent = gEntList.FindEntityByName(ent, name)) != NULL)
	{
		Lua_PushEntity(L, ent);
		lua_rawseti(L, -2, idx++);
	}

	return 1;
}

// =======================
// ents.FindByClass
// =======================

int luaFindByClass(lua_State* L)
{
	const char* cls = luaL_checkstring(L, 1);

	lua_newtable(L);

	int idx = 1;
	CBaseEntity* ent = NULL;

	while ((ent = gEntList.FindEntityByClassname(ent, cls)) != NULL)
	{
		Lua_PushEntity(L, ent);
		lua_rawseti(L, -2, idx++);
	}

	return 1;
}

int luaEntity_GetPos(lua_State* L)
{
	CBaseEntity* ent = Lua_GetEntity(L, 1);
	Vector v = ent ? ent->GetAbsOrigin() : vec3_origin;

	lua_newtable(L);
	lua_pushnumber(L, v.x); lua_setfield(L, -2, "x");
	lua_pushnumber(L, v.y); lua_setfield(L, -2, "y");
	lua_pushnumber(L, v.z); lua_setfield(L, -2, "z");
	return 1;
}

int luaEntity_SetPos(lua_State* L)
{
	CBaseEntity* ent = Lua_GetEntity(L, 1);
	if (!ent) return 0;

	luaL_checktype(L, 2, LUA_TTABLE);

	Vector v;
	lua_getfield(L, 2, "x"); v.x = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, 2, "y"); v.y = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, 2, "z"); v.z = lua_tonumber(L, -1); lua_pop(L, 1);

	ent->SetAbsOrigin(v);
	return 0;
}

int luaEntity_Remove(lua_State* L)
{
	CBaseEntity* ent = Lua_GetEntity(L, 1);
	if (ent)
		UTIL_Remove(ent);
	return 0;
}

int luaGetPicker(lua_State* L)
{
	CBasePlayer* ply = UTIL_GetLocalPlayer();
	if (!ply)
	{
		lua_pushnil(L);
		return 1;
	}

	trace_t tr;
	Vector start = ply->EyePosition();
	Vector end = start + ply->EyeDirection3D() * 8192;

	UTIL_TraceLine(start, end, MASK_SOLID, ply, COLLISION_GROUP_NONE, &tr);

	Lua_PushEntity(L, tr.m_pEnt);
	return 1;
}

int luaEntity_GetClass(lua_State* L)
{
	CBaseEntity* ent = Lua_GetEntity(L, 1);
	lua_pushstring(L, ent ? ent->GetClassname() : "");
	return 1;
}

int luaEntity_GetName(lua_State* L)
{
	CBaseEntity* ent = Lua_GetEntity(L, 1);
	lua_pushstring(L, ent ? STRING(ent->GetEntityName()) : "");
	return 1;
}

// =======================
// ents.spawn
// =======================
// ents.spawn( classname, model, color )
//
// • classname : entity classname (required)
// • model     : model path (optional, can be "")
// • color     : "r g b [a]" string (optional)
//
// Spawns entity at player's crosshair position
//

int luaEnts_Spawn(lua_State* L)
{
	const char* classname = luaL_checkstring(L, 1);

	const char* model = "";
	const char* color = "";

	if (lua_gettop(L) >= 2 && lua_isstring(L, 2))
		model = lua_tostring(L, 2);

	if (lua_gettop(L) >= 3 && lua_isstring(L, 3))
		color = lua_tostring(L, 3);

	CBasePlayer* ply = UTIL_GetLocalPlayer();
	if (!ply)
	{
		lua_pushnil(L);
		return 1;
	}

	// Trace from player eye
	trace_t tr;
	Vector start = ply->EyePosition();
	Vector end = start + ply->EyeDirection3D() * 8192;

	UTIL_TraceLine(start, end, MASK_SOLID, ply, COLLISION_GROUP_NONE, &tr);

	Vector spawnPos = tr.endpos + tr.plane.normal * 16.0f;
	QAngle spawnAng = ply->EyeAngles();

	// Create entity
	CBaseEntity* ent = CreateEntityByName(classname);
	if (!ent)
	{
		lua_pushnil(L);
		return 1;
	}

	// 🔴 REQUIRED for prop_physics
	if (model && model[0])
	{
		ent->PrecacheModel(model);
		ent->KeyValue("model", model);
	}

	UTIL_SetOrigin(ent, spawnPos);
	ent->SetAbsAngles(spawnAng);

	ent->Spawn();
	ent->Activate();

	// Optional color
	if (color && color[0])
	{
		variant_t v;
		v.SetString(MAKE_STRING(color));
		g_EventQueue.AddEvent(ent, "Color", v, 0.0f, ply, ply, 0);
	}

	Lua_PushEntity(L, ent);
	return 1;
}

int luaEnts_GetAll(lua_State* L)
{
	lua_newtable(L);

	int idx = 1;
	for (int i = 0; i < gEntList.NumberOfEntities(); i++)
	{
		CBaseEntity* ent = gEntList.GetBaseEntity(i);
		if (!ent) continue;

		Lua_PushEntity(L, ent);
		lua_rawseti(L, -2, idx++);
	}

	return 1;
}

int luaEnts_GetByIndex(lua_State* L)
{
	int idx = luaL_checkinteger(L, 1);
	Lua_PushEntity(L, UTIL_EntityByIndex(idx));
	return 1;
}

int luaEntity_EntIndex(lua_State* L)
{
	CBaseEntity* ent = Lua_GetEntity(L, 1);
	lua_pushinteger(L, ent ? ent->entindex() : -1);
	return 1;
}

int luaEntity_IsValid(lua_State* L)
{
	CBaseEntity* ent = Lua_GetEntity(L, 1);
	lua_pushboolean(L, ent && !ent->IsMarkedForDeletion());
	return 1;
}

// =======================
// Registration
// =======================

void Lua_RegisterEnts(lua_State* L)
{
	// =========================================================
	// Entity metatable
	// =========================================================
	luaL_newmetatable(L, LUA_ENTITY_MT);

	// __index = metatable
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	// ---- Entity methods (ent:Method()) ----
	lua_pushcfunction(L, luaEntity_Fire);
	lua_setfield(L, -2, "Fire");

	lua_pushcfunction(L, luaEntity_IsValid);
	lua_setfield(L, -2, "IsValid");

	lua_pushcfunction(L, luaEntity_EntIndex);
	lua_setfield(L, -2, "EntIndex");

	lua_pushcfunction(L, luaEntity_GetClass);
	lua_setfield(L, -2, "GetClass");

	lua_pushcfunction(L, luaEntity_GetName);
	lua_setfield(L, -2, "GetName");

	lua_pushcfunction(L, luaEntity_GetPos);
	lua_setfield(L, -2, "GetPos");

	lua_pushcfunction(L, luaEntity_SetPos);
	lua_setfield(L, -2, "SetPos");

	lua_pushcfunction(L, luaEntity_Remove);
	lua_setfield(L, -2, "Remove");

	lua_pop(L, 1); // pop Entity metatable

	// =========================================================
	// Global functions
	// =========================================================
	lua_pushcfunction(L, luaEntFire);
	lua_setglobal(L, "ent_fire");

	lua_pushcfunction(L, luaEntFireByID);
	lua_setglobal(L, "ent_fire_by_id");

	lua_pushcfunction(L, luaGetPicker);
	lua_setglobal(L, "picker");

	// =========================================================
	// ents table (ents.Function())
	// =========================================================
	lua_newtable(L);

	lua_pushcfunction(L, luaFindByName);
	lua_setfield(L, -2, "FindByName");

	lua_pushcfunction(L, luaFindByClass);
	lua_setfield(L, -2, "FindByClass");

	lua_pushcfunction(L, luaEnts_GetAll);
	lua_setfield(L, -2, "GetAll");

	lua_pushcfunction(L, luaEnts_GetByIndex);
	lua_setfield(L, -2, "GetByIndex");

	lua_pushcfunction(L, luaEnts_Spawn);
	lua_setfield(L, -2, "spawn");

	lua_setglobal(L, "ents");
}

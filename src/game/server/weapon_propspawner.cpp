#include "cbase.h"
#include "player.h"
#include "util.h"
#include "util_shared.h"

#ifndef CLIENT_DLL
#include "spark.h"
#include "ieffects.h"
#endif

static void FixSpawnPosition(CBaseEntity* pEnt, const Vector& hitPos)
{
	if (!pEnt)
		return;

	Vector pos = hitPos;

	if (pEnt->CollisionProp())
	{
		Vector mins = pEnt->CollisionProp()->OBBMins();

		// Correct: shift up by how far bottom is below origin
		pos.z -= mins.z;
	}

	pEnt->SetAbsOrigin(pos);
}

static void SpawnFromCommand(
	CBasePlayer* pPlayer,
	const char* pszSpawnType,
	const char* pszValue)
{
	if (!pPlayer)
		return;

	trace_t tr;
	Vector start = pPlayer->EyePosition();
	Vector end = start + pPlayer->EyeDirection3D() * 1024.0f;

	UTIL_TraceLine(start, end, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr);
	if (!tr.DidHit())
		return;

	Vector hitPos = tr.endpos;

	// --------------------------------------------------
	// PROPS
	// --------------------------------------------------
	if (!Q_stricmp(pszSpawnType, "prop"))
	{
		CBaseEntity* pEnt = CreateEntityByName("prop_physics");

		if (pEnt)
		{
			pEnt->PrecacheModel(pszValue);
			pEnt->SetModel(pszValue);

			DispatchSpawn(pEnt);
		}

		if (!pEnt)
			return;

		// Fix position AFTER model is set
		FixSpawnPosition(pEnt, hitPos);

		DispatchSpawn(pEnt);
		pEnt->Activate();
		return;
	}

	// --------------------------------------------------
	// RAGDOLLS
	// --------------------------------------------------
	if (!Q_stricmp(pszSpawnType, "ragdoll"))
	{
		CBaseEntity* pEnt = CreateEntityByName("prop_ragdoll");
		if (!pEnt)
			return;

		pEnt->PrecacheModel(pszValue);
		pEnt->SetModel(pszValue);

		FixSpawnPosition(pEnt, hitPos);

		DispatchSpawn(pEnt);
		pEnt->Activate();
		return;
	}

	// --------------------------------------------------
	// NPCs
	// --------------------------------------------------
	if (!Q_stricmp(pszSpawnType, "npc"))
	{
		CBaseEntity* pEnt = CreateEntityByName(pszValue);
		if (!pEnt)
		{
			Warning("Unknown NPC classname: %s\n", pszValue);
			return;
		}

		DispatchSpawn(pEnt);

		FixSpawnPosition(pEnt, hitPos);

		pEnt->Activate();
		return;
	}

	// --------------------------------------------------
	// EFFECTS
	// --------------------------------------------------
	if (!Q_stricmp(pszSpawnType, "effect"))
	{
		CBaseAnimating* pCarrier =
			static_cast<CBaseAnimating*>(CreateEntityByName("prop_physics"));

		if (!pCarrier)
			return;

		pCarrier->PrecacheModel("models/props_junk/cinderblock01a.mdl");
		pCarrier->SetModel("models/props_junk/cinderblock01a.mdl");
		pCarrier->SetMoveType(MOVETYPE_VPHYSICS);
		pCarrier->SetSolid(SOLID_VPHYSICS);
		pCarrier->VPhysicsInitNormal(SOLID_VPHYSICS, NULL, false);

		FixSpawnPosition(pCarrier, hitPos);

		DispatchSpawn(pCarrier);
		pCarrier->Activate();

		CBaseAnimating* pEffect =
			static_cast<CBaseAnimating*>(CreateEntityByName("prop_dynamic"));

		if (!pEffect)
		{
			pCarrier->SUB_Remove();
			return;
		}

		pEffect->PrecacheModel(pszValue);
		pEffect->SetModel(pszValue);
		pEffect->SetParent(pCarrier);
		pEffect->SetSolid(SOLID_NONE);
		pEffect->SetLocalOrigin(vec3_origin);

		DispatchSpawn(pEffect);
		pEffect->Activate();
		return;
	}

	// --------------------------------------------------
	// DEBUG ENTITIES
	// --------------------------------------------------
	if (!Q_stricmp(pszSpawnType, "debug"))
	{
		CBaseEntity* pEnt = CreateEntityByName(pszValue);
		if (!pEnt)
		{
			Warning("Unknown debug entity: %s\n", pszValue);
			return;
		}

		DispatchSpawn(pEnt);

		FixSpawnPosition(pEnt, hitPos);

		pEnt->Activate();
		return;
	}

	// --------------------------------------------------
	// WEAPONS
	// --------------------------------------------------
	if (!Q_stricmp(pszSpawnType, "weapon"))
	{
		CBaseEntity* pEnt = CreateEntityByName(pszValue);
		if (!pEnt)
		{
			Warning("[Spawnmenu] Unknown weapon: %s\n", pszValue);
			return;
		}

		DispatchSpawn(pEnt);

		FixSpawnPosition(pEnt, hitPos);

		pEnt->Activate();
		return;
	}

	// --------------------------------------------------
	// COMMANDS
	// --------------------------------------------------
	if (!Q_stricmp(pszSpawnType, "command"))
	{
		if (!pszValue || !pszValue[0])
			return;

		Msg("[SpawnMenu] Executing command: %s\n", pszValue);

		char cmd[512];
		Q_snprintf(cmd, sizeof(cmd), "%s\n", pszValue);

		engine->ClientCommand(pPlayer->edict(), cmd);
		return;
	}

	// fallback effect
	g_pEffects->Sparks(hitPos, 1, 1, 0);
}

CON_COMMAND(gabe_spawn, "gabe_spawn <prop|ragdoll|npc|effect|debug|weapon|command> <value>")
{
	CBasePlayer* pPlayer = UTIL_GetCommandClient();
	if (!pPlayer || args.ArgC() < 3)
	{
		Msg("Usage: gabe_spawn <type> <model|classname>\n");
		return;
	}

	SpawnFromCommand(pPlayer, args[1], args[2]);
}

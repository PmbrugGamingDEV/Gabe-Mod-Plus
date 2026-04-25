#define GABEPLUS_SHARED_H
#ifdef GABEPLUS_SHARED_H
#pragma once

//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared code for Gabe Mod
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "util.h"
#include "engine/IEngineSound.h"
#include "entitylist.h"
#include "vphysics_interface.h"
#include "collisionutils.h"
#include "props.h"
#include "soundent.h"
#include "ai_basenpc.h"
#include "vphysics/constraints.h"
#include "physics.h"
#include "player.h"
#include "game.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

/*

///////////////////////////////////////////////////////////
// ENTITY UTILITIES
///////////////////////////////////////////////////////////

CBaseEntity* Gabe_FindByName(const char* name);
CBaseEntity* Gabe_FindByClass(const char* classname);
CBaseEntity* Gabe_FindClosestEntity(
    const Vector& origin,
    const char* classname,
    float maxDist);

///////////////////////////////////////////////////////////
// DAMAGE UTILITIES
///////////////////////////////////////////////////////////

void Gabe_DealDamage(
    CBaseEntity* victim,
    CBaseEntity* attacker,
    float damage,
    int damageType);

void Gabe_RadiusDamage(
    const Vector& origin,
    CBaseEntity* attacker,
    float damage,
    float radius,
    int damageType);

///////////////////////////////////////////////////////////
// TRACE UTILITIES
///////////////////////////////////////////////////////////

void Gabe_TraceFromPlayer(
    CBasePlayer* player,
    float distance,
    trace_t& tr);

bool Gabe_HasLineOfSight(
    CBaseEntity* from,
    CBaseEntity* to);

bool Gabe_IsInFOV(
    CBaseEntity* viewer,
    CBaseEntity* target,
    float fovDegrees);

///////////////////////////////////////////////////////////
// PHYSICS HELPER
///////////////////////////////////////////////////////////

IPhysicsObject* Gabe_GetPhys(CBaseEntity* ent);

///////////////////////////////////////////////////////////
// CONSTRAINT UTILITIES
///////////////////////////////////////////////////////////

IPhysicsConstraint* Gabe_CreateFixed(
    CBaseEntity* refEnt,
    CBaseEntity* attachedEnt);

IPhysicsConstraint* Gabe_CreateBreakableFixed(
    CBaseEntity* refEnt,
    CBaseEntity* attachedEnt);

IPhysicsConstraint* Gabe_FixedToWorld(
    CBaseEntity* ent);

IPhysicsConstraint* Gabe_CreateBallSocket(
    CBaseEntity* ent1,
    CBaseEntity* ent2,
    const Vector& worldPos);

IPhysicsConstraint* Gabe_CreateRope(
    CBaseEntity* ent1,
    CBaseEntity* ent2,
    const Vector& worldPos1,
    const Vector& worldPos2,
    bool rigid = false);

IPhysicsConstraint* Gabe_CreateSlider(
    CBaseEntity* refEnt,
    CBaseEntity* attachedEnt,
    const Vector& slideAxisWorld,
    float limitMin,
    float limitMax);

void Gabe_SetSliderMotor(
    IPhysicsConstraint* constraint,
    float velocity,
    float maxForce);

IPhysicsConstraint* Gabe_CreateHinge(
    CBaseEntity* ent1,
    CBaseEntity* ent2,
    const Vector& worldPos,
    const Vector& worldAxis);

IPhysicsConstraint* Gabe_CreateLimitedHinge(
    CBaseEntity* ent1,
    CBaseEntity* ent2,
    const Vector& worldPos,
    const Vector& worldAxis,
    float minAngle,
    float maxAngle);

void Gabe_SetBreakable(
    constraint_breakableparams_t& breakParams,
    float forceLimit,
    float torqueLimit);

void Gabe_DestroyConstraint(
    IPhysicsConstraint* constraint);
    */

#endif // GABEPLUS_SHARED_H


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

CBaseEntity* Gabe_FindByName(const char* name)
{
    if (!name)
        return nullptr;

    return gEntList.FindEntityByName(nullptr, name);
}

CBaseEntity* Gabe_FindByClass(const char* classname)
{
    if (!classname)
        return nullptr;

    return gEntList.FindEntityByClassname(nullptr, classname);
}

CBaseEntity* Gabe_FindClosestEntity(const Vector& origin, const char* classname, float maxDist)
{
    CBaseEntity* pEntity = nullptr;
    CBaseEntity* pBest = nullptr;
    float bestDist = maxDist;

    while ((pEntity = gEntList.FindEntityByClassname(pEntity, classname)) != nullptr)
    {
        float dist = origin.DistTo(pEntity->GetAbsOrigin());

        if (dist < bestDist)
        {
            bestDist = dist;
            pBest = pEntity;
        }
    }

    return pBest;
}

void Gabe_DealDamage(
    CBaseEntity* victim,
    CBaseEntity* attacker,
    float damage,
    int damageType)
{
    if (!victim)
        return;

    CTakeDamageInfo info(attacker, attacker, damage, damageType);
    victim->TakeDamage(info);
}

void Gabe_RadiusDamage(
    const Vector& origin,
    CBaseEntity* attacker,
    float damage,
    float radius,
    int damageType)
{
    CTakeDamageInfo info(attacker, attacker, damage, damageType);
    RadiusDamage(info, origin, radius, CLASS_NONE, nullptr);
}

void Gabe_TraceFromPlayer(
    CBasePlayer* player,
    float distance,
    trace_t& tr)
{
    if (!player)
        return;

    Vector start = player->EyePosition();
    Vector forward;
    player->EyeVectors(&forward);

    Vector end = start + forward * distance;

    UTIL_TraceLine(
        start,
        end,
        MASK_SHOT,
        player,
        COLLISION_GROUP_NONE,
        &tr);
}

bool Gabe_HasLineOfSight(CBaseEntity* from, CBaseEntity* to)
{
    if (!from || !to)
        return false;

    trace_t tr;

    UTIL_TraceLine(
        from->EyePosition(),
        to->WorldSpaceCenter(),
        MASK_SOLID,
        from,
        COLLISION_GROUP_NONE,
        &tr);

    return (tr.m_pEnt == to || tr.fraction == 1.0f);
}

bool Gabe_IsInFOV(CBaseEntity* viewer, CBaseEntity* target, float fovDegrees)
{
    if (!viewer || !target)
        return false;

    Vector forward;

    Vector toTarget = (target->WorldSpaceCenter() - viewer->EyePosition());
    toTarget.NormalizeInPlace();

    float dot = DotProduct(forward, toTarget);

    float cosHalfFOV = cosf(DEG2RAD(fovDegrees * 0.5f));

    return dot >= cosHalfFOV;
}

IPhysicsObject* Gabe_GetPhys(CBaseEntity* ent)
{
    if (!ent)
        return nullptr;

    return ent->VPhysicsGetObject();
}

/////////////////////////////////////////////////////////////////////
// CONSTRAINTS
////////////////////////////////////////////////////////////////////

IPhysicsConstraint* Gabe_CreateFixed(
    CBaseEntity* refEnt,
    CBaseEntity* attachedEnt)
{
    if (!refEnt || !attachedEnt)
        return nullptr;

    IPhysicsObject* physRef = refEnt->VPhysicsGetObject();
    IPhysicsObject* physAttached = attachedEnt->VPhysicsGetObject();

    if (!physRef || !physAttached)
        return nullptr;

    constraint_fixedparams_t params;
    params.Defaults();

    params.InitWithCurrentObjectState(physRef, physAttached);

    return physenv->CreateFixedConstraint(
        physRef,
        physAttached,
        nullptr,
        params);
}

IPhysicsConstraint* Gabe_CreateBreakableFixed(
    CBaseEntity* refEnt,
    CBaseEntity* attachedEnt)
{
    if (!refEnt || !attachedEnt)
        return nullptr;

    IPhysicsObject* physRef = refEnt->VPhysicsGetObject();
    IPhysicsObject* physAttached = attachedEnt->VPhysicsGetObject();

    if (!physRef || !physAttached)
        return nullptr;

    constraint_fixedparams_t params;
    params.Defaults();

    params.InitWithCurrentObjectState(physRef, physAttached);

    return physenv->CreateFixedConstraint(
        physRef,
        physAttached,
        nullptr,
        params);
}

IPhysicsConstraint* Gabe_FixedToWorld(CBaseEntity* ent)
{
    if (!ent)
        return nullptr;

    IPhysicsObject* phys = ent->VPhysicsGetObject();
    if (!phys)
        return nullptr;

    constraint_fixedparams_t params;
    params.Defaults();

    params.InitWithCurrentObjectState(
        g_PhysWorldObject,
        phys);

    return physenv->CreateFixedConstraint(
        g_PhysWorldObject,
        phys,
        nullptr,
        params);
}

IPhysicsConstraint* Gabe_CreateBallSocket(
    CBaseEntity* ent1,
    CBaseEntity* ent2,
    const Vector& worldPos)
{
    IPhysicsObject* phys1 = Gabe_GetPhys(ent1);
    IPhysicsObject* phys2 = Gabe_GetPhys(ent2);

    if (!phys1 || !phys2)
        return nullptr;

    constraint_ballsocketparams_t params;
    params.Defaults();

    params.InitWithCurrentObjectState(
        phys1,
        phys2,
        worldPos);

    return physenv->CreateBallsocketConstraint(
        phys1,
        phys2,
        nullptr,
        params);
}

IPhysicsConstraint* Gabe_CreateRope(
    CBaseEntity* ent1,
    CBaseEntity* ent2,
    const Vector& worldPos1,
    const Vector& worldPos2,
    bool rigid = false)
{
    IPhysicsObject* phys1 = Gabe_GetPhys(ent1);
    IPhysicsObject* phys2 = Gabe_GetPhys(ent2);

    if (!phys1 || !phys2)
        return nullptr;

    constraint_lengthparams_t params;
    params.Defaults();

    params.InitWorldspace(
        phys1,
        phys2,
        worldPos1,
        worldPos2,
        rigid);

    return physenv->CreateLengthConstraint(
        phys1,
        phys2,
        nullptr,
        params);
}

IPhysicsConstraint* Gabe_CreateSlider(
    CBaseEntity* refEnt,
    CBaseEntity* attachedEnt,
    const Vector& slideAxisWorld,
    float limitMin,
    float limitMax)
{
    IPhysicsObject* physRef = Gabe_GetPhys(refEnt);
    IPhysicsObject* physAttached = Gabe_GetPhys(attachedEnt);

    if (!physRef || !physAttached)
        return nullptr;

    constraint_slidingparams_t params;
    params.Defaults();

    params.InitWithCurrentObjectState(
        physRef,
        physAttached,
        slideAxisWorld);

    params.limitMin = limitMin;
    params.limitMax = limitMax;

    return physenv->CreateSlidingConstraint(
        physRef,
        physAttached,
        nullptr,
        params);
}

void Gabe_SetSliderMotor(
    IPhysicsConstraint* constraint,
    float velocity,
    float maxForce)
{
    if (!constraint)
        return;

    constraint->SetLinearMotor(velocity, maxForce);
}

IPhysicsConstraint* Gabe_CreateHinge(
    CBaseEntity* ent1,
    CBaseEntity* ent2,
    const Vector& worldPos,
    const Vector& worldAxis)
{
    IPhysicsObject* phys1 = Gabe_GetPhys(ent1);
    IPhysicsObject* phys2 = Gabe_GetPhys(ent2);

    if (!phys1 || !phys2)
        return nullptr;

    constraint_hingeparams_t params;
    params.Defaults();

    params.worldPosition = worldPos;
    params.worldAxisDirection = worldAxis;

    return physenv->CreateHingeConstraint(
        phys1,
        phys2,
        nullptr,
        params);
}

IPhysicsConstraint* Gabe_CreateLimitedHinge(
    CBaseEntity* ent1,
    CBaseEntity* ent2,
    const Vector& worldPos,
    const Vector& worldAxis,
    float minAngle,
    float maxAngle)
{
    IPhysicsObject* phys1 = Gabe_GetPhys(ent1);
    IPhysicsObject* phys2 = Gabe_GetPhys(ent2);

    if (!phys1 || !phys2)
        return nullptr;

    constraint_limitedhingeparams_t params;
    params.Defaults();

    params.worldPosition = worldPos;
    params.worldAxisDirection = worldAxis;

    params.hingeAxis.minRotation = minAngle;
    params.hingeAxis.maxRotation = maxAngle;

    return physenv->CreateHingeConstraint(
        phys1,
        phys2,
        nullptr,
        params);
}

void Gabe_SetBreakable(
    constraint_breakableparams_t& breakParams,
    float forceLimit,
    float torqueLimit)
{
    breakParams.forceLimit = forceLimit;
    breakParams.torqueLimit = torqueLimit;
}

void Gabe_DestroyConstraint(IPhysicsConstraint* constraint)
{
    if (!constraint)
        return;

    physenv->DestroyConstraint(constraint);
}

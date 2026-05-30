// weapon_gabe_physgun.cpp
// Source SDK Base 2007 style physics gun weapon
// Put this file in your server weapon source folder, for example:
// src/game/server/hl2mp/weapon_gabe_physgun.cpp

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "player.h"
#include "in_buttons.h"
#include "physics.h"
#include "vphysics/constraints.h"
#include "vphysics/friction.h"
#include "engine/IEngineSound.h"

#ifndef CLIENT_DLL
#include "ndebugoverlay.h"
#endif

#ifdef CLIENT_DLL
#define CWeaponGabePhysgun C_WeaponGabePhysgun
#endif

class CWeaponGabePhysgun : public CBaseHLCombatWeapon
{
public:
    DECLARE_CLASS(CWeaponGabePhysgun, CBaseHLCombatWeapon);
    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();

    CWeaponGabePhysgun();

    void Precache(void);
    void PrimaryAttack(void);
    void SecondaryAttack(void);
    bool Reload(void);
    void ItemPostFrame(void);
    void DropHeldObject(void);
    void ThrowHeldObject(void);

#ifndef CLIENT_DLL
    void DrawPhysgunBeam(CBasePlayer* pOwner, CBaseEntity* pTarget);
#endif

private:
    CBaseEntity* FindObjectInFront(CBasePlayer* pOwner, trace_t* pTrace);
    void UpdateHeldObject(CBasePlayer* pOwner);
    bool CanPickupObject(CBaseEntity* pEnt);

private:
    CHandle<CBaseEntity> m_hHeldObject;
    IPhysicsObject* m_pHeldPhysics;
    float m_flHoldDistance;
    float m_flNextBeamTime;
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponGabePhysgun, DT_WeaponGabePhysgun)

BEGIN_NETWORK_TABLE(CWeaponGabePhysgun, DT_WeaponGabePhysgun)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponGabePhysgun)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_gabe_physgun, CWeaponGabePhysgun);
PRECACHE_WEAPON_REGISTER(weapon_gabe_physgun);

CWeaponGabePhysgun::CWeaponGabePhysgun()
{
    m_hHeldObject = NULL;
    m_pHeldPhysics = NULL;
    m_flHoldDistance = 96.0f;
    m_flNextBeamTime = 0.0f;
}

void CWeaponGabePhysgun::Precache(void)
{
    BaseClass::Precache();

    PrecacheScriptSound("Weapon_PhysCannon.Pickup");
    PrecacheScriptSound("Weapon_PhysCannon.Drop");
    PrecacheScriptSound("Weapon_PhysCannon.Launch");
}

bool CWeaponGabePhysgun::CanPickupObject(CBaseEntity* pEnt)
{
    if (!pEnt)
        return false;

    if (pEnt->IsPlayer())
        return false;

    if (pEnt->GetMoveType() != MOVETYPE_VPHYSICS)
        return false;

    IPhysicsObject* pPhys = pEnt->VPhysicsGetObject();
    if (!pPhys)
        return false;

    if (!pPhys->IsMoveable())
        return false;

    return true;
}

CBaseEntity* CWeaponGabePhysgun::FindObjectInFront(CBasePlayer* pOwner, trace_t* pTrace)
{
    if (!pOwner)
        return NULL;

    Vector vecStart = pOwner->EyePosition();
    Vector vecForward;
    pOwner->EyeVectors(&vecForward);

    Vector vecEnd = vecStart + vecForward * 512.0f;

    UTIL_TraceLine(vecStart, vecEnd, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, pTrace);

    if (!pTrace->m_pEnt)
        return NULL;

    if (!CanPickupObject(pTrace->m_pEnt))
        return NULL;

    return pTrace->m_pEnt;
}

void CWeaponGabePhysgun::PrimaryAttack(void)
{
    CBasePlayer* pOwner = ToBasePlayer(GetOwner());
    if (!pOwner)
        return;

#ifndef CLIENT_DLL
    if (m_hHeldObject)
    {
        DropHeldObject();
        m_flNextPrimaryAttack = gpGlobals->curtime + 0.2f;
        return;
    }

    trace_t tr;
    CBaseEntity* pEnt = FindObjectInFront(pOwner, &tr);

    if (!pEnt)
    {
        m_flNextPrimaryAttack = gpGlobals->curtime + 0.15f;
        return;
    }

    IPhysicsObject* pPhys = pEnt->VPhysicsGetObject();
    if (!pPhys)
        return;

    m_hHeldObject = pEnt;
    m_pHeldPhysics = pPhys;

    Vector vecDelta = tr.endpos - pOwner->EyePosition();
    m_flHoldDistance = vecDelta.Length();
    m_flHoldDistance = clamp(m_flHoldDistance, 64.0f, 256.0f);

    pPhys->EnableMotion(true);
    pPhys->Wake();
    pPhys->EnableDrag(false);

    EmitSound("Weapon_PhysCannon.Pickup");
#endif

    m_flNextPrimaryAttack = gpGlobals->curtime + 0.2f;
}

void CWeaponGabePhysgun::SecondaryAttack(void)
{
#ifndef CLIENT_DLL
    ThrowHeldObject();
#endif

    m_flNextSecondaryAttack = gpGlobals->curtime + 0.25f;
}

bool CWeaponGabePhysgun::Reload(void)
{
#ifndef CLIENT_DLL
    DropHeldObject();
	return true;
#endif
}

void CWeaponGabePhysgun::ItemPostFrame(void)
{
    CBasePlayer* pOwner = ToBasePlayer(GetOwner());

#ifndef CLIENT_DLL
    if (pOwner && m_hHeldObject)
    {
        UpdateHeldObject(pOwner);

        if (gpGlobals->curtime >= m_flNextBeamTime)
        {
            DrawPhysgunBeam(pOwner, m_hHeldObject);
            m_flNextBeamTime = gpGlobals->curtime + 0.03f;
        }
    }
#endif

    BaseClass::ItemPostFrame();
}

void CWeaponGabePhysgun::UpdateHeldObject(CBasePlayer* pOwner)
{
#ifndef CLIENT_DLL
    if (!pOwner || !m_hHeldObject || !m_pHeldPhysics)
        return;

    if (!CanPickupObject(m_hHeldObject))
    {
        DropHeldObject();
        return;
    }

    Vector vecForward;
    pOwner->EyeVectors(&vecForward);

    Vector vecTarget = pOwner->EyePosition() + vecForward * m_flHoldDistance;
    Vector vecCurrent = m_hHeldObject->WorldSpaceCenter();
    Vector vecToTarget = vecTarget - vecCurrent;

    Vector vecVelocity = vecToTarget * 8.0f;

    float flMaxSpeed = 900.0f;
    float flSpeed = vecVelocity.Length();
    if (flSpeed > flMaxSpeed)
    {
        vecVelocity *= flMaxSpeed / flSpeed;
    }

    AngularImpulse angImpulse(0, 0, 0);
    m_pHeldPhysics->SetVelocity(&vecVelocity, &angImpulse);
    m_pHeldPhysics->Wake();
#endif
}

void CWeaponGabePhysgun::DropHeldObject(void)
{
#ifndef CLIENT_DLL
    if (m_pHeldPhysics)
    {
        m_pHeldPhysics->EnableDrag(true);
        m_pHeldPhysics->Wake();
    }

    if (m_hHeldObject)
    {
        EmitSound("Weapon_PhysCannon.Drop");
    }
#endif

    m_hHeldObject = NULL;
    m_pHeldPhysics = NULL;
}

void CWeaponGabePhysgun::ThrowHeldObject(void)
{
#ifndef CLIENT_DLL
    CBasePlayer* pOwner = ToBasePlayer(GetOwner());

    if (!pOwner || !m_hHeldObject || !m_pHeldPhysics)
        return;

    Vector vecForward;
    pOwner->EyeVectors(&vecForward);

    Vector vecVelocity = vecForward * 1200.0f;
    AngularImpulse angImpulse(200, 200, 200);

    m_pHeldPhysics->SetVelocity(&vecVelocity, &angImpulse);
    m_pHeldPhysics->EnableDrag(true);
    m_pHeldPhysics->Wake();

    EmitSound("Weapon_PhysCannon.Launch");
#endif

    m_hHeldObject = NULL;
    m_pHeldPhysics = NULL;
}

#ifndef CLIENT_DLL
void CWeaponGabePhysgun::DrawPhysgunBeam(CBasePlayer* pOwner, CBaseEntity* pTarget)
{
    if (!pOwner || !pTarget)
        return;

    Vector vecStart = pOwner->Weapon_ShootPosition();
    Vector vecEnd = pTarget->WorldSpaceCenter();

    NDebugOverlay::Line(vecStart, vecEnd, 0, 180, 255, false, 0.035f);
}
#endif

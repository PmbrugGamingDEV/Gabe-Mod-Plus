#include "cbase.h"

// MP5 submachine gun

#if defined( CLIENT_DLL )
#define CWeaponMP5 C_WeaponMP5
#include "c_hl2mp_player.h"
#include "c_weapon__stubs.h"
#else
#include "hl2mp_player.h"
#endif

#include "weapon_hl2mpbasehlmpcombatweapon.h"

class CWeaponMP5 : public CBaseHL2MPCombatWeapon
{
public:
    DECLARE_CLASS(CWeaponMP5, CBaseHL2MPCombatWeapon);
    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();
    DECLARE_ACTTABLE();

    CWeaponMP5();

    virtual void PrimaryAttack();
    virtual void AddViewKick();

private:
    CWeaponMP5(const CWeaponMP5&);
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponMP5, DT_WeaponMP5)

BEGIN_NETWORK_TABLE(CWeaponMP5, DT_WeaponMP5)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponMP5)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_mp5, CWeaponMP5);
PRECACHE_WEAPON_REGISTER(weapon_mp5);

CWeaponMP5::CWeaponMP5()
{
    m_bFiresUnderwater = false;
}

void CWeaponMP5::PrimaryAttack()
{
#ifndef CLIENT_DLL

    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer)
        return;

    if (m_iClip1 <= 0)
    {
        Reload();
        return;
    }

    m_iClip1--;

    WeaponSound(SINGLE);
    pPlayer->DoMuzzleFlash();
    SendWeaponAnim(ACT_VM_PRIMARYATTACK);

    Vector vecSrc = pPlayer->Weapon_ShootPosition();
    Vector vecAiming = pPlayer->GetAutoaimVector(AUTOAIM_SCALE_DEFAULT);

    // Dynamic spread
    float spread = 0.02f;

    if (pPlayer->GetAbsVelocity().Length2D() > 10)
        spread = 0.06f;

    Vector cone = Vector(spread, spread, spread);

    pPlayer->FireBullets(
        1,
        vecSrc,
        vecAiming,
        cone,
        MAX_TRACE_LENGTH,
        m_iPrimaryAmmoType,
        0,
        9   // damage
    );

    AddViewKick();


#endif // CLIENT_DLL

    m_flNextPrimaryAttack = gpGlobals->curtime + 0.07f;
}

void CWeaponMP5::AddViewKick()
{
    CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
    if (!pPlayer)
        return;

    QAngle punch;

    punch.x = random->RandomFloat(-1.2f, -1.8f);
    punch.y = random->RandomFloat(-0.5f, 0.5f);
    punch.z = 0;

    pPlayer->ViewPunch(punch);
}

acttable_t CWeaponMP5::m_acttable[] =
{
    { ACT_HL2MP_IDLE,                    ACT_HL2MP_IDLE_SMG1,                    false },
    { ACT_HL2MP_RUN,                     ACT_HL2MP_RUN_SMG1,                     false },
    { ACT_HL2MP_IDLE_CROUCH,             ACT_HL2MP_IDLE_CROUCH_SMG1,             false },
    { ACT_HL2MP_WALK_CROUCH,             ACT_HL2MP_WALK_CROUCH_SMG1,             false },
    { ACT_HL2MP_GESTURE_RANGE_ATTACK,    ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,    false },
    { ACT_HL2MP_GESTURE_RELOAD,          ACT_HL2MP_GESTURE_RELOAD_SMG1,          false },
    { ACT_HL2MP_JUMP,                    ACT_HL2MP_JUMP_SMG1,                    false },
};

IMPLEMENT_ACTTABLE(CWeaponMP5);

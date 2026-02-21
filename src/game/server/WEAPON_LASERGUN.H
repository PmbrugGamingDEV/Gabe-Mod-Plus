#ifndef WEAPON_LASERGUN_H
#define WEAPON_LASERGUN_H

#include "weapon_hl2mpbasehlmpcombatweapon.h"

class CWeaponLaserGun : public CBaseHL2MPCombatWeapon
{
public:
    DECLARE_CLASS(CWeaponLaserGun, CBaseHL2MPCombatWeapon);
    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();

    CWeaponLaserGun();

    void Precache();
    void PrimaryAttack();

private:

    CNetworkVar(bool, m_bLaserActive);
    CSoundPatch* m_pLaserLoop;

    void FireLaser();
    bool Holster(CBaseCombatWeapon* pSwitchingTo);
    void ItemPostFrame();
};

#endif
#ifndef C_WEAPON_PROPLAUNCHER_H
#define C_WEAPON_PROPLAUNCHER_H

#include "cbase.h"
#include "basecombatweapon_shared.h"

class C_WeaponPropLauncher : public C_BaseCombatWeapon
{
    DECLARE_CLASS(C_WeaponPropLauncher, C_BaseCombatWeapon);
public:
    DECLARE_CLIENTCLASS();
    DECLARE_PREDICTABLE();

    C_WeaponPropLauncher();
    virtual void Precache();
    virtual bool Deploy();
    virtual void PrimaryAttack();
    virtual void SecondaryAttack();

private:
    C_WeaponPropLauncher(const C_WeaponPropLauncher &);
};

#endif // C_WEAPON_PROPLAUNCHER_H

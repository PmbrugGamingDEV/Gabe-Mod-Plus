#include "cbase.h"
#include "c_weapon_proplauncher.h"
#include "prediction.h"

IMPLEMENT_CLIENTCLASS_DT(C_WeaponPropLauncher, DT_WeaponPropLauncher, CWeaponPropLauncher)
END_RECV_TABLE()

BEGIN_PREDICTION_DATA(C_WeaponPropLauncher)
END_PREDICTION_DATA()

C_WeaponPropLauncher::C_WeaponPropLauncher()
{
}

void C_WeaponPropLauncher::Precache()
{
    BaseClass::Precache();
}

bool C_WeaponPropLauncher::Deploy()
{
    return BaseClass::Deploy();
}

void C_WeaponPropLauncher::PrimaryAttack()
{
    // Implement primary attack prediction here if needed
}

void C_WeaponPropLauncher::SecondaryAttack()
{

    // Implement secondary attack prediction here if needed
}

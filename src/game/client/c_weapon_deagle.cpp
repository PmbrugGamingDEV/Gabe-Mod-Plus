//========= Copyright © Valve Corporation ============//
//
// Purpose: Client-side stub for Deagle
//
//===================================================//

#include "cbase.h"
#include "c_basehlcombatweapon.h"

class C_WeaponDeagle : public C_BaseHLCombatWeapon
{
public:
	DECLARE_CLASS(C_WeaponDeagle, C_BaseHLCombatWeapon);
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
};

LINK_ENTITY_TO_CLASS(weapon_deagle, C_WeaponDeagle);

//-----------------------------------------------------------------------------
// Networking
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT(C_WeaponDeagle, DT_WeaponDEagle, CDEagle)
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Prediction
//-----------------------------------------------------------------------------
BEGIN_PREDICTION_DATA(C_WeaponDeagle)
END_PREDICTION_DATA()
#include "cbase.h"
#include "c_weapon__stubs.h"
#include "basehlcombatweapon_shared.h"
#include "c_basehlcombatweapon.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "sdk/weapon_sdkbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// ============================================================
// Gabe Mod+ weapons
// ============================================================

// Rows: 0-5
// Buckets 0+

STUB_WEAPON_CLASS(weapon_hax, WeaponHax, C_BaseHLCombatWeapon);

STUB_WEAPON_CLASS(weapon_multitool_legacy, WeaponLegacyMultiTool, C_BaseCombatWeapon);

STUB_WEAPON_CLASS(weapon_spawnmenu, WeaponSpawnMenu, C_BaseHL2MPCombatWeapon);
STUB_WEAPON_CLASS(weapon_lasergun, WeaponLaserGun, C_BaseHL2MPCombatWeapon);
STUB_WEAPON_CLASS(weapon_multitool, WeaponMultitool, C_BaseHL2MPCombatWeapon);
STUB_WEAPON_CLASS(weapon_stickylauncher, WeaponStickylauncher, C_BaseHLCombatWeapon);

STUB_WEAPON_CLASS(weapon_immolator, WeaponImmolator, C_BaseHLCombatWeapon);
STUB_WEAPON_CLASS(weapon_jetpack, WeaponJetPack, C_BaseHLCombatWeapon);
STUB_WEAPON_CLASS(weapon_eattach, WeaponEAttach, C_BaseHLCombatWeapon);

// Half-Life 1

// Row 0
STUB_WEAPON_CLASS(weapon_crowbar_hl1, WeaponHL1Crowbar, C_BaseHLCombatWeapon); // Spot 0
STUB_WEAPON_CLASS(weapon_egon, WeaponEgon, C_BaseHLCombatWeapon); // Spot 1

// Row 1
STUB_WEAPON_CLASS(weapon_glock, WeaponGlock, C_BaseHLCombatWeapon); // Spot 0

// Row 4
STUB_WEAPON_CLASS(weapon_tripmine, WeaponTripMine, C_BaseHLCombatWeapon); // Spot 1
STUB_WEAPON_CLASS(weapon_snark, WeaponSnark, C_BaseHLCombatWeapon); // Spot 3
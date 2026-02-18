#include "cbase.h"
#include "basecombatweapon_shared.h"
#include "hl2mp_player.h"
#include "player.h"
#include "gamerules.h"
#include "ai_basenpc.h"
#include "explode.h"
#include "entitylist.h"
#include "tier1/KeyValues.h"
#include "filesystem.h"
#include "vstdlib/random.h"
#include "engine/IEngineTrace.h"
#include <explode.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void AdjustMaxAmmoForAllWeapons(int newMaxAmmo)
{
    // Get the local player
    CBasePlayer* pPlayer = UTIL_GetCommandClient();
    if (!pPlayer)
    {
        Msg("Player not found.\n");
        return;
    }

    // Loop through all weapons the player has
    for (int i = 0; i < pPlayer->WeaponCount(); ++i)
    {
        CBaseCombatWeapon* pWeapon = pPlayer->GetWeapon(i);
        if (!pWeapon)
            continue;

        // Adjust the maximum ammo for primary and secondary ammo types
        int primaryAmmoType = pWeapon->GetPrimaryAmmoType();
        int secondaryAmmoType = pWeapon->GetSecondaryAmmoType();

        if (primaryAmmoType != -1)
        {
            pPlayer->SetAmmoCount(newMaxAmmo, primaryAmmoType);
        }

        if (secondaryAmmoType != -1)
        {
            pPlayer->SetAmmoCount(newMaxAmmo, secondaryAmmoType);
        }

        Msg("Max ammo set for %s\n", pWeapon->GetClassname());
    }
}

// Define the console command using ConCommand
CON_COMMAND(gabeplus_maxammo, "Adjusts the max ammo.")
{
    if (args.ArgC() < 2)
    {
        Msg("Usage: gabeplus_maxammo <amount>\n");
        return;
    }

    int newMaxAmmo = atoi(args[1]);  // Convert the argument to an integer
    if (newMaxAmmo < 0)
    {
        Msg("Invalid max ammo amount. Please enter a positive number.\n");
        return;
    }

    AdjustMaxAmmoForAllWeapons(newMaxAmmo);
}

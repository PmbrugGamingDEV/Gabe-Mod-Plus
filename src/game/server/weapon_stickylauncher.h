#pragma once

#include "cbase.h"
#include "basehlcombatweapon.h"

class CStickyPellet;

class CWeaponStickyLauncher : public CBaseHLCombatWeapon
{
public:
	DECLARE_CLASS(CWeaponStickyLauncher, CBaseHLCombatWeapon);
	DECLARE_SERVERCLASS();
	DECLARE_ACTTABLE();

	void PrimaryAttack();
	void SecondaryAttack();
	void Precache();
	void Spawn();

	CUtlVector<CHandle<CStickyPellet>> m_Pellets;
};

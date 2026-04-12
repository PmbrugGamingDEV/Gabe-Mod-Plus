//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: CS Deagle
//
//=============================================================================//

#include "cbase.h"
#include "decals.h"
#include "shake.h"
#include "basehlcombatweapon.h"
#include "player.h"

#define DEAGLE_WEIGHT   7
#define DEAGLE_MAX_CLIP 7

enum deagle_e
{
	DEAGLE_IDLE1 = 0,
	DEAGLE_SHOOT1,
	DEAGLE_SHOOT2,
	DEAGLE_SHOOT_EMPTY,
	DEAGLE_RELOAD,
	DEAGLE_DRAW,
};

class CDEagle : public CBaseHLCombatWeapon
{
public:
	DECLARE_CLASS(CDEagle, CBaseHLCombatWeapon);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CDEagle();

	void Spawn();
	void PrimaryAttack();
	void DEAGLEFire(float flSpread);
	virtual bool Deploy();
	bool Reload();
	void WeaponIdle();

	virtual bool UseDecrement() { return true; };

private:
	float m_flLastFire;

	CDEagle(const CDEagle&);
};

IMPLEMENT_NETWORKCLASS_ALIASED(DEagle, DT_WeaponDEagle)

BEGIN_NETWORK_TABLE(CDEagle, DT_WeaponDEagle)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CDEagle)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_deagle, CDEagle);
PRECACHE_WEAPON_REGISTER(weapon_deagle);


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CDEagle::CDEagle()
{
	m_flLastFire = 0.0f;
}


//-----------------------------------------------------------------------------
// Spawn
//-----------------------------------------------------------------------------
void CDEagle::Spawn()
{
	BaseClass::Spawn();
	m_iClip1 = DEAGLE_MAX_CLIP;
}


//-----------------------------------------------------------------------------
// Deploy
//-----------------------------------------------------------------------------
bool CDEagle::Deploy()
{
	return BaseClass::Deploy();
}


//-----------------------------------------------------------------------------
// Primary Attack
//-----------------------------------------------------------------------------
void CDEagle::PrimaryAttack()
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
		return;

	float flSpread;

	if (!(pPlayer->GetFlags() & FL_ONGROUND))
		flSpread = 0.1f;
	else if (pPlayer->GetAbsVelocity().Length2D() > 5)
		flSpread = 0.05f;
	else if (pPlayer->GetFlags() & FL_DUCKING)
		flSpread = 0.02f;
	else
		flSpread = 0.03f;

	DEAGLEFire(flSpread);
}


//-----------------------------------------------------------------------------
// Fire logic (HL2-style)
//-----------------------------------------------------------------------------
void CDEagle::DEAGLEFire(float flSpread)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
		return;

	if (m_iClip1 <= 0)
	{
		if (m_bFireOnEmpty)
		{
			WeaponSound(EMPTY);
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2f;
		}
		return;
	}

	m_iClip1--;

	pPlayer->DoMuzzleFlash();

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	// Direction with recoil
	QAngle angShoot = pPlayer->EyeAngles() + pPlayer->GetPunchAngle();
	Vector vecDir;
	AngleVectors(angShoot, &vecDir);

	// Spread
	Vector vecSpread(flSpread, flSpread, flSpread);

	FireBulletsInfo_t info;
	info.m_iShots = 1;
	info.m_vecSrc = pPlayer->Weapon_ShootPosition();
	info.m_vecDirShooting = vecDir;
	info.m_vecSpread = vecSpread;
	info.m_flDistance = MAX_TRACE_LENGTH;
	info.m_iAmmoType = m_iPrimaryAmmoType;
	info.m_iTracerFreq = 2;

	pPlayer->FireBullets(info);

	// Fire rate
	m_flNextPrimaryAttack = gpGlobals->curtime + 0.3f;

	// Out of ammo warning
	if (!m_iClip1 && pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);
	}

	SetWeaponIdleTime(gpGlobals->curtime + 1.8f);

	// Recoil
	pPlayer->ViewPunch(QAngle(-2.0f, random->RandomFloat(-1.0f, 1.0f), 0));
}


//-----------------------------------------------------------------------------
// Reload
//-----------------------------------------------------------------------------
bool CDEagle::Reload()
{
	return BaseClass::Reload();
}


//-----------------------------------------------------------------------------
// Idle
//-----------------------------------------------------------------------------
void CDEagle::WeaponIdle()
{
	if (m_flTimeWeaponIdle > gpGlobals->curtime)
		return;

	SetWeaponIdleTime(gpGlobals->curtime + 20);

	if (m_iClip1 != 0)
	{
		SendWeaponAnim(ACT_VM_IDLE);
	}
}
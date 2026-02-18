//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//

#include "cbase.h"
#include "weapon_sdkbase.h"

#if defined( CLIENT_DLL )
#define CWeaponSDKMP5 C_WeaponSDKMP5
#include "c_baseplayer.h"
#else
#include "player.h"
#include "te_firebullets.h"
#endif

class CWeaponSDKMP5 : public CWeaponSDKBase
{
public:
	DECLARE_CLASS(CWeaponSDKMP5, CWeaponSDKBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CWeaponSDKMP5();

	virtual void PrimaryAttack();
	virtual bool Deploy();
	virtual bool Reload();
	virtual void WeaponIdle();

	virtual SDKWeaponID GetWeaponID(void) const { return WEAPON_MP5; }


private:

	CWeaponSDKMP5(const CWeaponSDKMP5&);

	void Fire(float flSpread);
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponSDKMP5, DT_WeaponSDKMP5)

BEGIN_NETWORK_TABLE(CWeaponSDKMP5, DT_WeaponSDKMP5)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponSDKMP5)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(sdk_mp5, CWeaponSDKMP5);
PRECACHE_WEAPON_REGISTER(sdk_mp5);



CWeaponSDKMP5::CWeaponSDKMP5()
{
}

bool CWeaponSDKMP5::Deploy()
{
	CBasePlayer* pPlayer = GetPlayerOwner();

	return BaseClass::Deploy();
}

bool CWeaponSDKMP5::Reload()
{
	CBasePlayer* pPlayer = GetPlayerOwner();

	if (pPlayer->GetAmmoCount(GetPrimaryAmmoType()) <= 0)
		return false;

	int iResult = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);
	if (!iResult)
		return false;

	pPlayer->SetAnimation(PLAYER_RELOAD);

#ifndef CLIENT_DLL
	if ((iResult) && (pPlayer->GetFOV() != pPlayer->GetDefaultFOV()))
	{
		pPlayer->SetFOV(pPlayer, pPlayer->GetDefaultFOV());
	}
#endif

	return true;
}

void CWeaponSDKMP5::PrimaryAttack()
{
    CBasePlayer* pPlayer = GetPlayerOwner();
    if (!pPlayer)
        return;

    if (m_iClip1 <= 0)
    {
        if (m_bFireOnEmpty)
        {
            PlayEmptySound();
            m_flNextPrimaryAttack = gpGlobals->curtime + 0.2f;
        }
        return;
    }

    // Fire rate
    const CSDKWeaponInfo& weaponInfo = GetSDKWpnData();
    float flCycleTime = weaponInfo.m_flCycleTime;

    // Spread
    float flSpread = 0.01f;
    if (!(pPlayer->GetFlags() & FL_ONGROUND))
        flSpread = 0.05f;

    // Animation
    SendWeaponAnim(ACT_VM_PRIMARYATTACK);
    pPlayer->SetAnimation(PLAYER_ATTACK1);

    // Fire bullets (ENGINE HANDLES FX)
    Vector vecSrc = pPlayer->Weapon_ShootPosition();
    Vector vecAiming = pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

    FireBulletsInfo_t info;
    info.m_vecSrc = vecSrc;
    info.m_vecDirShooting = vecAiming;
    info.m_iShots = 1;
    info.m_flDistance = MAX_TRACE_LENGTH;
    info.m_iAmmoType = m_iPrimaryAmmoType;
    info.m_pAttacker = pPlayer;

    pPlayer->FireBullets(info);

    pPlayer->DoMuzzleFlash();

    m_iClip1--;

    m_flNextPrimaryAttack = gpGlobals->curtime + flCycleTime;
    m_flNextSecondaryAttack = gpGlobals->curtime + flCycleTime;

    SetWeaponIdleTime(gpGlobals->curtime + 5.0f);

    if (!m_iClip1 && pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
    {
        pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);
    }
}


void CWeaponSDKMP5::WeaponIdle()
{
	if (m_flTimeWeaponIdle > gpGlobals->curtime)
		return;

	// only idle if the slid isn't back
	if (m_iClip1 != 0)
	{
		SetWeaponIdleTime(gpGlobals->curtime + 5.0f);
		SendWeaponAnim(ACT_VM_IDLE);
	}
}



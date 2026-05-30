//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Cubemap dev weapon with model cycling
//
//=============================================================================//

#include "cbase.h"
#include "basecombatweapon.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponCubemap : public CBaseCombatWeapon
{
public:
	DECLARE_CLASS(CWeaponCubemap, CBaseCombatWeapon);

	void	Precache(void);
	void	Spawn(void);
	void	SecondaryAttack(void);

	bool	HasAnyAmmo(void) { return true; }

	void ItemPostFrame(void);

	DECLARE_SERVERCLASS();

private:
	int m_iModelIndex;
	static const char* m_pszModels[];
};

LINK_ENTITY_TO_CLASS(weapon_cubemap, CWeaponCubemap);

IMPLEMENT_SERVERCLASS_ST(CWeaponCubemap, DT_WeaponCubemap)
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Model list (STARTS WITH ENVBALLS)
//-----------------------------------------------------------------------------
const char* CWeaponCubemap::m_pszModels[] =
{
	"models/shadertest/envballs.mdl",   // * starts here
	"models/props_c17/oildrum001.mdl",
	"models/props_junk/wood_crate001a.mdl",
	"models/props_c17/FurnitureChair001a.mdl",
	"models/props_lab/huladoll.mdl"
};

//-----------------------------------------------------------------------------
// Purpose: Precache assets
//-----------------------------------------------------------------------------
void CWeaponCubemap::Precache(void)
{
	BaseClass::Precache();

	for (int i = 0; i < ARRAYSIZE(m_pszModels); i++)
	{
		PrecacheModel(m_pszModels[i]);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Spawn
//-----------------------------------------------------------------------------
void CWeaponCubemap::Spawn(void)
{
	BaseClass::Spawn();

	m_iModelIndex = 0;

	// Set initial model (envballs)
	SetModel(m_pszModels[m_iModelIndex]);

	// Fix pickup bounds
	UTIL_SetSize(this, Vector(-16, -16, -16), Vector(16, 16, 16));
}

//-----------------------------------------------------------------------------
// Purpose: Right click cycles models
//-----------------------------------------------------------------------------
void CWeaponCubemap::SecondaryAttack(void)
{
	m_iModelIndex++;

	if (m_iModelIndex >= ARRAYSIZE(m_pszModels))
	{
		m_iModelIndex = 0;
	}

	SetModel(m_pszModels[m_iModelIndex]);

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.3f;
}

void CWeaponCubemap::ItemPostFrame(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
		return;

	// Handle right click
	if (pPlayer->m_nButtons & IN_ATTACK2)
	{
		if (m_flNextSecondaryAttack <= gpGlobals->curtime)
		{
			SecondaryAttack();
		}
	}

	BaseClass::ItemPostFrame();
}
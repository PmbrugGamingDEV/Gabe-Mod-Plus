//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef WEAPON_GRENADE_H
#define WEAPON_GRENADE_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_basesdkgrenade.h"


#ifdef CLIENT_DLL
	
	#define CWeaponSDKGrenade C_WeaponSDKGrenade

#endif

//-----------------------------------------------------------------------------
// Fragmentation grenades
//-----------------------------------------------------------------------------
class CWeaponSDKGrenade : public CBaseSDKGrenade
{
public:
	DECLARE_CLASS( CWeaponSDKGrenade, CBaseSDKGrenade );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

	CWeaponSDKGrenade() {}

#ifdef CLIENT_DLL

#else
	DECLARE_DATADESC();

	virtual void EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer, CWeaponSDKBase *pWeapon );
	
#endif

	CWeaponSDKGrenade( const CWeaponSDKGrenade& ) {}
};


#endif // WEAPON_GRENADE_H

#pragma once
#include "cbase.h"
#include "baseanimating.h"

class CGabeEffectCarrier : public CBaseAnimating
{
public:
	DECLARE_CLASS( CGabeEffectCarrier, CBaseAnimating );
	DECLARE_DATADESC();

	void Spawn();
	void Precache();

	void AttachEffectModel( const char *pszModel );
};

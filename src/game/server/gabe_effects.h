#pragma once
#include "cbase.h"
#include "gabe_effect_carrier.h"

class CGabeEffects
{
public:
	static CGabeEffectCarrier *SpawnEffect(
		CBaseEntity *pOwner,
		const Vector &pos,
		const char *pszEffectModel,
		float lifetime = 3.0f
	);
};

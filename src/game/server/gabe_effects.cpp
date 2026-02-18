#include "cbase.h"
#include "gabe_effects.h"
#include "gabe_effect_carrier.h"

CGabeEffectCarrier *CGabeEffects::SpawnEffect(
	CBaseEntity *pOwner,
	const Vector &pos,
	const char *pszEffectModel,
	float lifetime
)
{
	CGabeEffectCarrier *pCarrier =
		static_cast<CGabeEffectCarrier*>( CreateEntityByName( "gabe_effect_carrier" ) );

	if ( !pCarrier )
		return NULL;

	pCarrier->SetAbsOrigin( pos );
	pCarrier->SetOwnerEntity( pOwner );

	DispatchSpawn( pCarrier );
	pCarrier->Activate();

	pCarrier->AttachEffectModel( pszEffectModel );

	if ( lifetime > 0.0f )
	{
		pCarrier->SetThink( &CBaseEntity::SUB_Remove );
		pCarrier->SetNextThink( gpGlobals->curtime + lifetime );
	}

	return pCarrier;
}

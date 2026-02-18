#include "cbase.h"
#include "gabe_effect_carrier.h"

LINK_ENTITY_TO_CLASS( gabe_effect_carrier, CGabeEffectCarrier );

BEGIN_DATADESC( CGabeEffectCarrier )
END_DATADESC()

void CGabeEffectCarrier::Precache()
{
	PrecacheModel( "models/props_junk/watermelon01.mdl" );
}

void CGabeEffectCarrier::Spawn()
{
	Precache();

	SetModel( "models/props_junk/watermelon01.mdl" );

	SetMoveType( MOVETYPE_FLY );
	SetSolid( SOLID_NONE );

	AddEffects( EF_NODRAW ); // invisible carrier
	SetThink( &CBaseEntity::SUB_Remove );
}

void CGabeEffectCarrier::AttachEffectModel( const char *pszModel )
{
	CBaseAnimating *pEffect = (CBaseAnimating*)CreateEntityByName( "prop_dynamic" );
	if ( !pEffect )
		return;

	pEffect->SetModel( pszModel );
	pEffect->SetAbsOrigin( GetAbsOrigin() );
	pEffect->SetParent( this );
	pEffect->SetSolid( SOLID_NONE );
	pEffect->AddEffects( EF_BONEMERGE | EF_PARENT_ANIMATES );

	DispatchSpawn( pEffect );
	pEffect->Activate();
}

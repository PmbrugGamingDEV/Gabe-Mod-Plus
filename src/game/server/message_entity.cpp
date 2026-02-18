#include "cbase.h"
#include "player.h"

// memdbgon must be the last include file
#include "tier0/memdbgon.h"

// Spawnflags
#define SF_MESSAGE_DISABLED 1

class CMessageEntity : public CPointEntity
{
    DECLARE_CLASS( CMessageEntity, CPointEntity );

public:
    void Spawn( void );
    void Think( void );

    void InputEnable( inputdata_t &inputdata );
    void InputDisable( inputdata_t &inputdata );

    DECLARE_DATADESC();

private:
    int      m_radius;
    string_t m_messageText;
    bool     m_bEnabled;
};

LINK_ENTITY_TO_CLASS( point_message, CMessageEntity );

BEGIN_DATADESC( CMessageEntity )

    DEFINE_KEYFIELD( m_radius, FIELD_INTEGER, "radius" ),
    DEFINE_KEYFIELD( m_messageText, FIELD_STRING, "message" ),
    DEFINE_FIELD( m_bEnabled, FIELD_BOOLEAN ),

    DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
    DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Spawn
//-----------------------------------------------------------------------------
void CMessageEntity::Spawn()
{
    BaseClass::Spawn();

    m_bEnabled = !HasSpawnFlags( SF_MESSAGE_DISABLED );
    SetNextThink( gpGlobals->curtime + 0.25f );
}

//-----------------------------------------------------------------------------
// Think (GLOBAL message logic)
//-----------------------------------------------------------------------------
void CMessageEntity::Think()
{
    SetNextThink( gpGlobals->curtime + 0.01f );

    if ( !m_bEnabled )
        return;

    // Check if ANY player is within radius
    bool bShow = false;

    for ( int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
        if ( !pPlayer || !pPlayer->IsAlive() )
            continue;

        float dist = ( pPlayer->EyePosition() - GetAbsOrigin() ).Length();
        if ( dist <= m_radius )
        {
            bShow = true;
            break;
        }
    }

    if ( !bShow )
        return;

    char tempstr[512];
    Q_snprintf( tempstr, sizeof( tempstr ), "%s", STRING( m_messageText ) );

    EntityText( 0, tempstr, 0.25f );
}

//-----------------------------------------------------------------------------
// Inputs
//-----------------------------------------------------------------------------
void CMessageEntity::InputEnable( inputdata_t &inputdata )
{
    m_bEnabled = true;
}

void CMessageEntity::InputDisable( inputdata_t &inputdata )
{
    m_bEnabled = false;
}

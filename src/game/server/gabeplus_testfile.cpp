#include "cbase.h"
#include "baseentity.h"
#include "vphysics_interface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CGabePlusTestFile : public CBaseAnimating
{
public:
    DECLARE_CLASS( CGabePlusTestFile, CBaseAnimating );
    //DECLARE_SERVERCLASS();
    DECLARE_DATADESC();

    CGabePlusTestFile();
    ~CGabePlusTestFile();

    virtual void Spawn() override;
    float        GetMass();
    virtual void Precache() override;
    virtual void Think() override;
};

LINK_ENTITY_TO_CLASS( gabeplus_testfile, CGabePlusTestFile );
//IMPLEMENT_SERVERCLASS_ST( CGabePlusTestFile, DT_GabePlusTestFile )
//END_SEND_TABLE()

BEGIN_DATADESC( CGabePlusTestFile )
END_DATADESC()

CGabePlusTestFile::CGabePlusTestFile()
{
}

CGabePlusTestFile::~CGabePlusTestFile()
{
}

void CGabePlusTestFile::Precache()
{
    PrecacheModel( "models/props_c17/oildrum001.mdl" );
    BaseClass::Precache();
}

void CGabePlusTestFile::Spawn()
{
    BaseClass::Spawn();
    SetModel( "models/props_c17/oildrum001.mdl" );
    SetSolid( SOLID_VPHYSICS );
    SetMoveType( MOVETYPE_VPHYSICS );
    SetThink( &CGabePlusTestFile::Think );
    SetNextThink( gpGlobals->curtime + 0.1f );
}

float CGabePlusTestFile::GetMass()
{
    // Retrieve the physics object associated with this entity
    IPhysicsObject *pPhysics = VPhysicsGetObject();
    if ( pPhysics )
    {
        // Get the mass of the physics object
        float mass = pPhysics->GetMass();
        return mass;
    }
}

float Kg2lbs( float kg )
{
    float lbs = kg * 2.20462f;
    return lbs;
}

void CGabePlusTestFile::Think()
{
    // For testing purposes, we'll just print a message to the console every second
    DevMsg( "CGabePlusTestFile is thinking at time: %f\n", gpGlobals->curtime );
    Msg("Mass: %f\n", Kg2lbs( GetMass() ) );
    SetNextThink( gpGlobals->curtime + 1.0f );
}
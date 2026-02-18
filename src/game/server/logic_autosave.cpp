//===== Copyright (c) 1996-2026 Valve Corporation. =====//
//
// Purpose: Multiplayer stub for logic_autosave.
//          Prevents console warnings when loading HL2 SP maps.
//
//=================================================================//

#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Stub autosave entity for multiplayer.
//-----------------------------------------------------------------------------
class CLogicAutosave : public CBaseEntity
{
public:
    DECLARE_CLASS(CLogicAutosave, CBaseEntity);
    DECLARE_DATADESC();

    void Spawn(void);

    // Common inputs used in SP maps
    void InputEnable(inputdata_t& inputdata);
    void InputDisable(inputdata_t& inputdata);
    void InputSave(inputdata_t& inputdata);
};

LINK_ENTITY_TO_CLASS(logic_autosave, CLogicAutosave);

BEGIN_DATADESC(CLogicAutosave)
DEFINE_INPUTFUNC(FIELD_VOID, "Enable", InputEnable),
DEFINE_INPUTFUNC(FIELD_VOID, "Disable", InputDisable),
DEFINE_INPUTFUNC(FIELD_VOID, "Save", InputSave),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Called when entity spawns
//-----------------------------------------------------------------------------
void CLogicAutosave::Spawn(void)
{
    SetSolid(SOLID_NONE);
    SetMoveType(MOVETYPE_NONE);
    AddEffects(EF_NODRAW);
}

//-----------------------------------------------------------------------------
// Input handlers — intentionally empty
//-----------------------------------------------------------------------------
void CLogicAutosave::InputEnable(inputdata_t& inputdata)
{
}

void CLogicAutosave::InputDisable(inputdata_t& inputdata)
{
}

void CLogicAutosave::InputSave(inputdata_t& inputdata)
{
    Vector pos = GetAbsOrigin();
    ConColorMsg(Color(255, 0, 0, 255),
        "Autosave shut up at (%.2f %.2f %.2f)\n",
        pos.x, pos.y, pos.z);
}

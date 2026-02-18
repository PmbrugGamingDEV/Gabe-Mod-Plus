#include "cbase.h"

class CEdictFiller : public CBaseEntity
{
public:
    DECLARE_CLASS(CEdictFiller, CBaseEntity);
    DECLARE_DATADESC();

    void Spawn(void);
};

LINK_ENTITY_TO_CLASS(edict_filler, CEdictFiller);

BEGIN_DATADESC(CEdictFiller)
END_DATADESC()

void CEdictFiller::Spawn(void)
{
    SetSolid(SOLID_NONE);
    SetMoveType(MOVETYPE_NONE);

    AddEffects(EF_NODRAW);
    AddEFlags(EFL_DONTBLOCKLOS);

    SetTransmitState(FL_EDICT_DONTSEND);

    // No think.
    // No physics.
    // No networking.
}

CON_COMMAND(spawn_fillers, "Spawn edict fillers safely")
{
    if (args.ArgC() < 2)
    {
        Msg("Usage: spawn_fillers <count>\n");
        return;
    }

    int count = atoi(args[1]);

    for (int i = 0; i < count; i++)
    {
        CBaseEntity* pEnt = CreateEntityByName("edict_filler");
        if (!pEnt)
        {
            Msg("Failed to create entity.\n");
            return;
        }

        pEnt->Spawn();
    }

    Msg("Edicts: %d / %d\n", engine->GetEntityCount(), MAX_EDICTS);
}

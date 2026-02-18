//========= GabePlus NPC Inspect Debug ================================
// Inspect NPC stats when player looks at them
// Source SDK Base 2007 / HL2MP compatible
//=====================================================================

#include "cbase.h"
#include "util.h"
#include "ai_baseactor.h"
#include "ndebugoverlay.h"

// memdbgon must be last
#include "tier0/memdbgon.h"

//=====================================================================
// NPC Inspect Entity
//=====================================================================
class CGabeplusNPCInspect : public CBaseEntity
{
public:
    DECLARE_CLASS( CGabeplusNPCInspect, CBaseEntity );
    DECLARE_DATADESC();

    void Spawn();
    void Think();
};

LINK_ENTITY_TO_CLASS( env_gabeplus_npcinspect, CGabeplusNPCInspect );

//=====================================================================
// Datadesc
//=====================================================================
BEGIN_DATADESC( CGabeplusNPCInspect )
    DEFINE_THINKFUNC( Think ),
END_DATADESC()

//=====================================================================
// Spawn
//=====================================================================
void CGabeplusNPCInspect::Spawn()
{
    BaseClass::Spawn();
    SetNextThink( gpGlobals->curtime + 0.05f );
}

void CGabeplusNPCInspect::Think()
{
    for (int i = 1; i <= gpGlobals->maxClients; i++)
    {
        CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
        if (!pPlayer)
            continue;

        // --------------------------------------------------
        // Trace from this player's crosshair
        // --------------------------------------------------
        Vector vecStart = pPlayer->EyePosition();
        Vector vecForward;
        pPlayer->EyeVectors(&vecForward);

        Vector vecEnd = vecStart + vecForward * 4096.0f;

        trace_t tr;
        UTIL_TraceLine(
            vecStart,
            vecEnd,
            MASK_SHOT,
            pPlayer,
            COLLISION_GROUP_NONE,
            &tr
        );

        CAI_BaseNPC* pNPC = dynamic_cast<CAI_BaseNPC*>(tr.m_pEnt);
        if (!pNPC || !pNPC->IsAlive())
            continue; // IMPORTANT: continue, not return

        // --------------------------------------------------
        // Resolve name
        // --------------------------------------------------
        const char* pszName =
            (pNPC->GetEntityName() == NULL_STRING)
            ? pNPC->GetClassname()
            : STRING(pNPC->GetEntityName());

        // --------------------------------------------------
        // Health
        // --------------------------------------------------
        float frac = 1.0f;
        if (pNPC->GetMaxHealth() > 0)
        {
            frac = clamp(
                (float)pNPC->GetHealth() / (float)pNPC->GetMaxHealth(),
                0.0f, 1.0f
            );
        }

        int healthPct = (int)(frac * 100.0f);

        // --------------------------------------------------
        // Build strings
        // --------------------------------------------------
        char lineName[256];
        char lineHealth[256];

        Q_snprintf(lineName, sizeof(lineName), "NAME: %s", pszName);
        Q_snprintf(lineHealth, sizeof(lineHealth), "HEALTH: %d%%", healthPct);

        int r = (int)(255 * (1.0f - frac));
        int g = (int)(255 * frac);

        // --------------------------------------------------
        // Per-player overlay (server replicated)
        // --------------------------------------------------
        NDebugOverlay::ScreenText(
            0.35f, 0.42f,
            "NPC INSPECT",
            255, 255, 120, 255,
            0.1f
        );

        NDebugOverlay::ScreenText(
            0.35f, 0.46f,
            lineName,
            255, 255, 255, 255,
            0.1f
        );

        NDebugOverlay::ScreenText(
            0.35f, 0.50f,
            lineHealth,
            r, g, 50, 255,
            0.1f
        );
    }

    // --------------------------------------------------
    // Schedule ONCE
    // --------------------------------------------------
    SetNextThink(gpGlobals->curtime + 0.05f);
}


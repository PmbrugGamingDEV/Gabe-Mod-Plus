#include "cbase.h"

#if GAME_DLL

#include "achievementmgr.h"
#include "baseachievement.h"
#include "hl2mp_gamerules.h"

CAchievementMgr g_GabeAchievementMgr;

// Achievement ID
#define ACHIEVEMENT_GABEMOD_FIRSTKILL  1

class CAchievementGabeModFirstKill : public CBaseAchievement
{
public:
    void Init()
    {
        SetFlags(ACH_LISTEN_PLAYER_KILL_ENEMY_EVENTS | ACH_SAVE_GLOBAL);
		SetName("1st Frag");
        SetVictimFilter("npc_*"); // Any NPC
        SetGoal(1);
    }

    void ListenForEvents()
    {
        ListenForGameEvent("entity_killed");
    }

    void FireGameEvent(IGameEvent* event)
    {
        if (!event)
            return;

        const char* victim = event->GetString("victim_name");
        if (victim && Q_strnicmp(victim, "npc_", 4) == 0)
        {
            IncrementCount();
        }
    }
};

DECLARE_ACHIEVEMENT(CAchievementGabeModFirstKill,
    ACHIEVEMENT_GABEMOD_FIRSTKILL,
    "GABEMOD_FIRSTKILL",
    5);

#endif

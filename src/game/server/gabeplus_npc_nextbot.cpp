#include "cbase.h"
#include "player.h"
#include "ai_basenpc.h"
#include "nav_mesh.h"
#include "nav_area.h"

#define NEXTBOT_THINK_INTERVAL 0.1f
#define NEXTBOT_MOVE_SPEED 120.0f
#define NEXTBOT_MOVE_TIME 2.5f
#define NEXTBOT_STOP_TIME 1.5f

class CNPC_SimpleNextBot : public CAI_BaseNPC
{
public:
    DECLARE_CLASS(CNPC_SimpleNextBot, CAI_BaseNPC);
    DECLARE_DATADESC();

    void Spawn();
    void Think();

private:
    void PickNewNavGoal();
    void MoveTowardGoal();
    void StopAndSpeak();

    Vector    m_vecGoal;
    float     m_flNextActionTime;
    bool      m_bMoving;
};

LINK_ENTITY_TO_CLASS(npc_nextbot, CNPC_SimpleNextBot);

BEGIN_DATADESC(CNPC_SimpleNextBot)
DEFINE_THINKFUNC(Think),
DEFINE_FIELD(m_vecGoal, FIELD_VECTOR),
DEFINE_FIELD(m_flNextActionTime, FIELD_TIME),
DEFINE_FIELD(m_bMoving, FIELD_BOOLEAN),
END_DATADESC()

void CNPC_SimpleNextBot::Spawn()
{
    Precache();
    SetModel("models/Humans/Group01/male_07.mdl");

    SetHullType(HULL_HUMAN);
    SetHullSizeNormal();

    SetSolid(SOLID_BBOX);
    SetMoveType(MOVETYPE_STEP);
    SetCollisionGroup(COLLISION_GROUP_NPC);

    SetHealth(100);

    m_bMoving = false;
    m_flNextActionTime = gpGlobals->curtime + 1.0f;

    SetThink(&CNPC_SimpleNextBot::Think);
    SetNextThink(gpGlobals->curtime + NEXTBOT_THINK_INTERVAL);
}

void CNPC_SimpleNextBot::Think()
{
    if (gpGlobals->curtime >= m_flNextActionTime)
    {
        if (m_bMoving)
        {
            StopAndSpeak();
        }
        else
        {
            PickNewNavGoal();
        }
    }

    if (m_bMoving)
    {
        MoveTowardGoal();
    }

    SetNextThink(gpGlobals->curtime + NEXTBOT_THINK_INTERVAL);
}

void CNPC_SimpleNextBot::PickNewNavGoal()
{
    if (!TheNavMesh)
        return;

    CNavArea* pArea = TheNavMesh->GetNearestNavArea(GetAbsOrigin());
    if (!pArea)
        return;

    NavDirType dirs[4] = { NORTH, SOUTH, EAST, WEST };

    for (int i = 0; i < 8; i++)
    {
        NavDirType dir = dirs[RandomInt(0, 3)];
        CNavArea* pNext = pArea->GetAdjacentArea(dir, i);

        if (pNext)
        {
            m_vecGoal = pNext->GetCenter();
            m_bMoving = true;
            m_flNextActionTime = gpGlobals->curtime + NEXTBOT_MOVE_TIME;
            return;
        }
    }

    // Fallback: stand still
    m_vecGoal = GetAbsOrigin();
    m_flNextActionTime = gpGlobals->curtime + NEXTBOT_STOP_TIME;
}

void CNPC_SimpleNextBot::MoveTowardGoal()
{
    Vector dir = m_vecGoal - GetAbsOrigin();
    float dist = dir.Length();

    if (dist < 10.0f)
    {
        SetAbsVelocity(vec3_origin);
        return;
    }

    dir.NormalizeInPlace();

    Vector velocity = dir * NEXTBOT_MOVE_SPEED;
    SetAbsVelocity(velocity);

    QAngle ang;
    VectorAngles(velocity, ang);
    SetAbsAngles(ang);
}

void CNPC_SimpleNextBot::StopAndSpeak()
{
    SetAbsVelocity(vec3_origin);
    m_bMoving = false;

    UTIL_ClientPrintAll(HUD_PRINTTALK, "I'm speaking!\n");

    m_flNextActionTime = gpGlobals->curtime + NEXTBOT_STOP_TIME;
}


//================= Gabe Mod v8.1 =================//
//
// Purpose: Advanced NextBot enemy
//
// Features
// - Uses a model instead of a sprite
// - AI state machine
// - Patrol / wander
// - Enemy memory
// - Leap attacks
// - Rage mode
// - Damage reactions
// - Animation control
// - Sound reactions
//
//================================================//

#include "cbase.h"
#include "baseentity.h"
#include "player.h"
#include "gamerules.h"
#include "util.h"
#include "ai_motor.h"
#include "ai_basenpc.h"
#include "soundent.h"
#include "physics.h"
#include "vphysics_interface.h"

#include "tier0/memdbgon.h"

class CGabeNextBot : public CBaseAnimating
{
public:

    DECLARE_CLASS(CGabeNextBot, CBaseAnimating);
    DECLARE_DATADESC();

    void Spawn();
    void Precache();
    void Think();
    void Touch(CBaseEntity* pOther);

    int OnTakeDamage(const CTakeDamageInfo& info);

private:

    enum GabeState
    {
        STATE_IDLE = 0,
        STATE_WANDER,
        STATE_CHASE,
        STATE_ATTACK,
        STATE_RAGE,
        STATE_STUNNED
    };

    void UpdateEnemy();
    void UpdateState();

    void DoIdle();
    void DoWander();
    void DoChase();
    void DoAttack();
    void DoLeap();

    void FaceTarget(Vector pos);
    bool CanSeeEnemy();

    CBasePlayer* m_hEnemy;

    int m_iHealth;
    GabeState m_State;

    Vector m_vecVelocity;
    Vector m_vecWanderDir;

    float m_flNextJump;
    float m_flNextAttack;
    float m_flNextScan;
    float m_flNextWander;
    float m_flEnemyMemory;
};

LINK_ENTITY_TO_CLASS(gabe_nextbot, CGabeNextBot);

BEGIN_DATADESC(CGabeNextBot)

DEFINE_FIELD(m_hEnemy, FIELD_CLASSPTR),
DEFINE_FIELD(m_iHealth, FIELD_INTEGER),

DEFINE_FIELD(m_State, FIELD_INTEGER),

DEFINE_FIELD(m_vecVelocity, FIELD_VECTOR),
DEFINE_FIELD(m_vecWanderDir, FIELD_VECTOR),

DEFINE_FIELD(m_flNextJump, FIELD_TIME),
DEFINE_FIELD(m_flNextAttack, FIELD_TIME),
DEFINE_FIELD(m_flNextScan, FIELD_TIME),
DEFINE_FIELD(m_flNextWander, FIELD_TIME),
DEFINE_FIELD(m_flEnemyMemory, FIELD_TIME),

END_DATADESC()

//------------------------------------------------
// Precache
//------------------------------------------------

void CGabeNextBot::Precache()
{
    PrecacheModel("models/zombie/classic.mdl");

    PrecacheScriptSound("NPC_Zombie.Idle");
    PrecacheScriptSound("NPC_Zombie.Alert");
    PrecacheScriptSound("NPC_Zombie.Attack");
}

//------------------------------------------------
// Spawn
//------------------------------------------------

void CGabeNextBot::Spawn()
{
    Precache();

    SetModel("models/zombie/classic.mdl");

    SetSolid(SOLID_BBOX);
    SetMoveType(MOVETYPE_STEP);

    SetSize(Vector(-16, -16, 0), Vector(16, 16, 72));

    SetGravity(1.0f);

    SetTouch(&CGabeNextBot::Touch);

    SetThink(&CGabeNextBot::Think);
    SetNextThink(gpGlobals->curtime + 0.1f);

    m_iHealth = 300;

    m_State = STATE_IDLE;

    m_flNextJump = gpGlobals->curtime;
    m_flNextAttack = gpGlobals->curtime;
    m_flNextScan = gpGlobals->curtime;

    ResetSequence(LookupSequence("Idle01"));
}

//------------------------------------------------
// Enemy scanning
//------------------------------------------------

void CGabeNextBot::UpdateEnemy()
{
    if (gpGlobals->curtime < m_flNextScan)
        return;

    m_flNextScan = gpGlobals->curtime + 0.3f;

    CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

    if (!pPlayer || !pPlayer->IsAlive())
        return;

    float dist = (pPlayer->GetAbsOrigin() - GetAbsOrigin()).Length();

    if (dist < 2000 && CanSeeEnemy())
    {
        m_hEnemy = pPlayer;
        m_flEnemyMemory = gpGlobals->curtime + 5.0f;
    }

    if (gpGlobals->curtime > m_flEnemyMemory)
        m_hEnemy = NULL;
}

//------------------------------------------------
// Line of sight
//------------------------------------------------

bool CGabeNextBot::CanSeeEnemy()
{
    if (!m_hEnemy)
        return false;

    trace_t tr;

    UTIL_TraceLine(
        WorldSpaceCenter(),
        m_hEnemy->WorldSpaceCenter(),
        MASK_SOLID,
        this,
        COLLISION_GROUP_NONE,
        &tr
    );

    return (tr.m_pEnt == m_hEnemy);
}

//------------------------------------------------
// AI state selection
//------------------------------------------------

void CGabeNextBot::UpdateState()
{
    if (m_iHealth < 100)
    {
        m_State = STATE_RAGE;
        return;
    }

    if (m_hEnemy)
    {
        float dist = (m_hEnemy->GetAbsOrigin() - GetAbsOrigin()).Length();

        if (dist < 80)
            m_State = STATE_ATTACK;
        else
            m_State = STATE_CHASE;
    }
    else
    {
        m_State = STATE_WANDER;
    }
}

//------------------------------------------------
// Idle
//------------------------------------------------

void CGabeNextBot::DoIdle()
{
    ResetSequence(LookupSequence("Idle01"));
}

//------------------------------------------------
// Wander randomly
//------------------------------------------------

void CGabeNextBot::DoWander()
{
    if (gpGlobals->curtime > m_flNextWander)
    {
        m_flNextWander = gpGlobals->curtime + RandomFloat(2, 4);

        m_vecWanderDir.x = RandomFloat(-1, 1);
        m_vecWanderDir.y = RandomFloat(-1, 1);
        m_vecWanderDir.z = 0;

        VectorNormalize(m_vecWanderDir);
    }

    Vector move = m_vecWanderDir * 120 * gpGlobals->frametime;

    SetAbsOrigin(GetAbsOrigin() + move);

    ResetSequence(LookupSequence("walk_all"));
}

//------------------------------------------------
// Chase player
//------------------------------------------------

void CGabeNextBot::DoChase()
{
    if (!m_hEnemy)
        return;

    Vector dir = m_hEnemy->GetAbsOrigin() - GetAbsOrigin();

    dir.z = 0;

    VectorNormalize(dir);

    float speed = 280;

    if (m_State == STATE_RAGE)
        speed = 420;

    Vector move = dir * speed * gpGlobals->frametime;

    SetAbsOrigin(GetAbsOrigin() + move);

    FaceTarget(m_hEnemy->GetAbsOrigin());

    ResetSequence(LookupSequence("run_all"));
}

//------------------------------------------------
// Leap attack
//------------------------------------------------

void CGabeNextBot::DoLeap()
{
    if (gpGlobals->curtime < m_flNextJump)
        return;

    Vector dir = m_hEnemy->GetAbsOrigin() - GetAbsOrigin();

    VectorNormalize(dir);

    Vector vel = dir * 600;

    vel.z = 350;

    SetAbsVelocity(vel);

    m_flNextJump = gpGlobals->curtime + 3.0f;
}

//------------------------------------------------
// Attack
//------------------------------------------------

void CGabeNextBot::DoAttack()
{
    if (gpGlobals->curtime < m_flNextAttack)
        return;

    if (!m_hEnemy)
        return;

    float dist = (m_hEnemy->GetAbsOrigin() - GetAbsOrigin()).Length();

    if (dist > 90)
        return;

    EmitSound("NPC_Zombie.Attack");

    CTakeDamageInfo info(this, this, 45, DMG_SLASH);

    m_hEnemy->TakeDamage(info);

    DoLeap();

    m_flNextAttack = gpGlobals->curtime + 1.0f;
}

//------------------------------------------------
// Face target
//------------------------------------------------

void CGabeNextBot::FaceTarget(Vector pos)
{
    QAngle ang;

    VectorAngles(pos - GetAbsOrigin(), ang);

    SetAbsAngles(QAngle(0, ang.y, 0));
}

//------------------------------------------------
// Damage
//------------------------------------------------

int CGabeNextBot::OnTakeDamage(const CTakeDamageInfo& info)
{
    m_iHealth -= info.GetDamage();

    Vector push = info.GetDamageForce() * 0.02f;

    SetAbsVelocity(GetAbsVelocity() + push);

    if (m_iHealth <= 0)
    {
        EmitSound("NPC_Zombie.Die");
        UTIL_Remove(this);
    }

    return 1;
}

//------------------------------------------------
// Main AI loop
//------------------------------------------------

void CGabeNextBot::Think()
{
    UpdateEnemy();

    UpdateState();

    switch (m_State)
    {
    case STATE_IDLE:
        DoIdle();
        break;

    case STATE_WANDER:
        DoWander();
        break;

    case STATE_CHASE:
        DoChase();
        break;

    case STATE_ATTACK:
        DoAttack();
        break;

    case STATE_RAGE:
        DoChase();
        break;
    }

    SetNextThink(gpGlobals->curtime + 0.01f);
}

//------------------------------------------------
// Touch
//------------------------------------------------

void CGabeNextBot::Touch(CBaseEntity* pOther)
{
    if (pOther && pOther->IsPlayer())
    {
        CTakeDamageInfo info(this, this, 25, DMG_GENERIC);

        pOther->TakeDamage(info);
    }
}
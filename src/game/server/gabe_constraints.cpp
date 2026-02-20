#include "cbase.h"
#include "gabe_constraints.h"
#include "memdbgon.h"

LINK_ENTITY_TO_CLASS(gabe_constraint, CGabeConstraint);

BEGIN_DATADESC(CGabeConstraint)
DEFINE_THINKFUNC(ConstraintThink),
END_DATADESC()

void CGabeConstraint::Spawn()
{
    SetThink(&CGabeConstraint::ConstraintThink);
    SetNextThink(gpGlobals->curtime + TICK_INTERVAL);

    m_flStiffness = 8000.0f;
    m_flDamping = 100.0f;
    m_flBreakForce = 0.0f;
}

void CGabeConstraint::InitWorldAnchor(
    IPhysicsObject* pPhys,
    const Vector& anchor,
    GabeConstraintType_t type,
    float length
)
{
    m_pPhysA = pPhys;
    m_pPhysB = NULL;

    m_vAnchor = anchor;
    m_Type = type;
    m_flLength = length;

    if (pPhys)
        pPhys->GetPosition(NULL, &m_angInitialA);
}

void CGabeConstraint::InitObjectToObject(
    IPhysicsObject* pPhysA,
    IPhysicsObject* pPhysB,
    GabeConstraintType_t type,
    float length
)
{
    m_pPhysA = pPhysA;
    m_pPhysB = pPhysB;
    m_Type = type;
    m_flLength = length;

    if (pPhysA)
        pPhysA->GetPosition(NULL, &m_angInitialA);

    if (pPhysB)
        pPhysB->GetPosition(NULL, &m_angInitialB);
}

void CGabeConstraint::ConstraintThink()
{
    if (!m_pPhysA)
    {
        UTIL_Remove(this);
        return;
    }

    switch (m_Type)
    {
    case GABE_CONSTRAINT_ROPE:
        SolveRope();
        break;

    case GABE_CONSTRAINT_BALLSOCKET:
        SolveBallSocket();
        break;

    case GABE_CONSTRAINT_WELD:
        SolveWeld();
        break;
    }

    SetNextThink(gpGlobals->curtime + TICK_INTERVAL);
}

void CGabeConstraint::SolveRope()
{
    Vector posA;
    m_pPhysA->GetPosition(&posA, NULL);

    Vector anchor = m_pPhysB ?
        [&]() { Vector p; m_pPhysB->GetPosition(&p, NULL); return p; }()
        : m_vAnchor;

    Vector delta = posA - anchor;
    float dist = delta.Length();

    if (dist <= m_flLength)
        return;

    Vector dir = delta / dist;
    float stretch = dist - m_flLength;

    Vector vel;
    AngularImpulse angVel;
    m_pPhysA->GetVelocity(&vel, &angVel);

    Vector force = -dir * (stretch * m_flStiffness) - vel * m_flDamping;

    if (m_flBreakForce > 0.0f && force.Length() > m_flBreakForce)
    {
        UTIL_Remove(this);
        return;
    }

    ApplyForceSafe(m_pPhysA, force);
}

void CGabeConstraint::SolveBallSocket()
{
    Vector posA;

    if (!m_pPhysA || !m_pPhysB)
        return;

    m_pPhysA->GetPosition(&posA, NULL);

    Vector anchor = m_pPhysB ?
        [&]() { Vector p; m_pPhysB->GetPosition(&p, NULL); return p; }()
        : m_vAnchor;

    Vector delta = anchor - posA;

    Vector vel;
    AngularImpulse angVel;
    m_pPhysA->GetVelocity(&vel, &angVel);

    Vector force = delta * m_flStiffness - vel * m_flDamping;

    if (m_flBreakForce > 0.0f && force.Length() > m_flBreakForce)
    {
        UTIL_Remove(this);
        return;
    }

    ApplyForceSafe(m_pPhysA, force);
}

void CGabeConstraint::SolveWeld()
{
    SolveBallSocket();

    QAngle ang;
    m_pPhysA->GetPosition(NULL, &ang);

    AngularImpulse angVel;
    Vector vel;
    m_pPhysA->GetVelocity(&vel, &angVel);

    QAngle error = m_angInitialA - ang;

    AngularImpulse torque;
    torque.x = error.x * 50.0f - angVel.x * 5.0f;
    torque.y = error.y * 50.0f - angVel.y * 5.0f;
    torque.z = error.z * 50.0f - angVel.z * 5.0f;

    m_pPhysA->ApplyTorqueCenter(torque);
}

void CGabeConstraint::ApplyForceSafe(
    IPhysicsObject* pPhys,
    const Vector& force
)
{
    const float maxForce = 20000.0f;

    Vector f = force;

    if (f.Length() > maxForce)
        f = f.NormalizeInPlace() * maxForce;

    pPhys->ApplyForceCenter(f);
}

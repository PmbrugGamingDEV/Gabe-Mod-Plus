#ifndef GABE_CONSTRAINTS_H
#define GABE_CONSTRAINTS_H
#pragma once

#include "cbase.h"
#include "vphysics_interface.h"

enum GabeConstraintType_t
{
    GABE_CONSTRAINT_ROPE = 0,
    GABE_CONSTRAINT_BALLSOCKET,
    GABE_CONSTRAINT_WELD
};

class CGabeConstraint : public CBaseEntity
{
public:
    DECLARE_CLASS(CGabeConstraint, CBaseEntity);
    DECLARE_DATADESC();

    void Spawn();
    void ConstraintThink();

    void InitWorldAnchor(
        IPhysicsObject* pPhys,
        const Vector& anchor,
        GabeConstraintType_t type,
        float length = 0.0f
    );

    void InitObjectToObject(
        IPhysicsObject* pPhysA,
        IPhysicsObject* pPhysB,
        GabeConstraintType_t type,
        float length = 0.0f
    );

    void SetBreakForce(float force) { m_flBreakForce = force; }
    void SetStiffness(float s) { m_flStiffness = s; }
    void SetDamping(float d) { m_flDamping = d; }

private:

    void SolveRope();
    void SolveBallSocket();
    void SolveWeld();

    void ApplyForceSafe(IPhysicsObject* pPhys, const Vector& force);

private:

    IPhysicsObject* m_pPhysA;
    IPhysicsObject* m_pPhysB;

    Vector m_vAnchor;

    GabeConstraintType_t m_Type;

    float m_flLength;
    float m_flStiffness;
    float m_flDamping;
    float m_flBreakForce;

    QAngle m_angInitialA;
    QAngle m_angInitialB;
};

#endif

#include "cbase.h"
#include "physics_prop_ragdoll.h"
#include "vphysics/constraints.h"

class CBaseStaticRagdoll : public CRagdollProp
{
public:
	DECLARE_CLASS(CBaseStaticRagdoll, CRagdollProp);

	virtual void Precache(void)
	{
		BaseClass::Precache();
		PrecacheModel(STRING(GetModelName()));
	}

	virtual void SetupModel(void)
	{
		// Override in child classes
	}

	void Spawn(void)
	{
		SetupModel();
		Precache();

		SetSolid(SOLID_VPHYSICS);
		SetMoveType(MOVETYPE_VPHYSICS);

		// Save initial angles BEFORE spawn resets them
		AngularImpulse angImpulse;
		QAngleToAngularImpulse(GetAbsAngles(), angImpulse);

		BaseClass::Spawn();

		// 🔧 Optional: freeze motion completely
		IPhysicsObject* pPhys = VPhysicsGetObject();
		if (pPhys)
		{
			pPhys->EnableMotion(false);
		}

		// 🔗 Create constraint (locked ragdoll)
		constraint_ragdollparams_t ragdoll;
		ragdoll.Defaults();

		MatrixSetColumn(GetAbsOrigin(), 3, ragdoll.constraintToReference);

		for (int i = 0; i < 3; i++)
		{
			ragdoll.axes[i].minRotation = angImpulse[i];
			ragdoll.axes[i].maxRotation = angImpulse[i];
		}

		physenv->CreateRagdollConstraint(
			g_PhysWorldObject,
			pPhys,
			NULL,
			ragdoll
		);
	}
};

class CGrass : public CBaseStaticRagdoll
{
public:
	void SetupModel(void)
	{
		PrecacheModel("models/props/grass.mdl");
		SetModel("models/props/grass.mdl");
	}
};
LINK_ENTITY_TO_CLASS(prop_grass, CGrass);
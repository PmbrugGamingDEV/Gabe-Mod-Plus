//========= Copyright © Valve Corporation ============//

#include "cbase.h"
#include "simple_physics.h"
#include "mathlib/vmatrix.h"
#include "beamdraw.h"

#include "tier0/memdbgon.h"

class C_Hairball : public C_BaseEntity
{
	DECLARE_CLASS(C_Hairball, C_BaseEntity);

private:

	class CHairballDelegate : public CSimplePhysics::IHelper
	{
	public:
		virtual void GetNodeForces(CSimplePhysics::CNode* pNodes, int iNode, Vector* pAccel)
		{
			pAccel->Init(0, 0, -1500);
		}

		virtual void ApplyConstraints(CSimplePhysics::CNode* pNodes, int nNodes);

		C_Hairball* m_pParent;
	};

public:

	C_Hairball();

	void Init();
	virtual void ClientThink();
	virtual int DrawModel(int flags);

public:

	float m_flSphereRadius;
	int   m_nHairs;
	int   m_nNodesPerHair;
	float m_flSpringDist;

	CUtlVector<CSimplePhysics::CNode> m_Nodes;
	CUtlVector<Vector> m_HairPositions;
	CUtlVector<Vector> m_TransformedHairPositions;

	CHairballDelegate m_Delegate;
	CSimplePhysics m_Physics;

	IMaterial* m_pMaterial;

	// movement
	float m_flSitStillTime;
	Vector m_vMoveDir;

	// spin
	float m_flSpinDuration;
	float m_flCurSpinTime;
	float m_flSpinRateX, m_flSpinRateY;

	bool m_bFirstThink;

	// TEXT MODE
	Vector m_vTextTarget;
	bool   m_bHasTextTarget;
};

//-----------------------------------------------------------------------------
// CONSTRAINTS
//-----------------------------------------------------------------------------
void C_Hairball::CHairballDelegate::ApplyConstraints(CSimplePhysics::CNode* pNodes, int nNodes)
{
	int nSegments = m_pParent->m_nNodesPerHair - 1;
	float flSpringDistSqr = m_pParent->m_flSpringDist * m_pParent->m_flSpringDist;

	for (int iHair = 0; iHair < m_pParent->m_nHairs; iHair++)
	{
		CSimplePhysics::CNode* pBase = &pNodes[iHair * m_pParent->m_nNodesPerHair];

		for (int i = 0; i < nSegments; i++)
		{
			Vector& v1 = pBase[i].m_vPos;
			Vector& v2 = pBase[i + 1].m_vPos;

			Vector vTo = v1 - v2;

			float distSqr = vTo.LengthSqr();
			if (distSqr > flSpringDistSqr)
			{
				float dist = sqrt(distSqr);
				vTo *= 1 - (m_pParent->m_flSpringDist / dist);

				v1 -= vTo * 0.5f;
				v2 += vTo * 0.5f;
			}
		}

		pBase->m_vPos = m_pParent->m_TransformedHairPositions[iHair];
	}
}

//-----------------------------------------------------------------------------
// CONSTRUCTOR
//-----------------------------------------------------------------------------
C_Hairball::C_Hairball()
{
	m_nHairs = 60;
	m_nNodesPerHair = 3;

	float flHairLength = 20;
	m_flSpringDist = flHairLength / (m_nNodesPerHair - 1);

	m_Nodes.SetSize(m_nHairs * m_nNodesPerHair);
	m_HairPositions.SetSize(m_nHairs);
	m_TransformedHairPositions.SetSize(m_nHairs);

	m_flSphereRadius = 20;

	for (int i = 0; i < m_HairPositions.Count(); i++)
	{
		float t = (float)i / (float)m_HairPositions.Count();

		float inc = acos(1 - 2 * t);
		float azi = 2 * M_PI * t * 1.618f;

		float x = sin(inc) * cos(azi);
		float y = sin(inc) * sin(azi);
		float z = cos(inc);

		m_HairPositions[i].Init(x, y, z);
	}

	m_Delegate.m_pParent = this;
	m_Physics.Init(1.0 / 20);

	m_pMaterial = NULL;

	m_bFirstThink = true;

	// TEXT
	m_vTextTarget.Init();
	m_bHasTextTarget = false;
}

//-----------------------------------------------------------------------------
// INIT
//-----------------------------------------------------------------------------
void C_Hairball::Init()
{
	ClientEntityList().AddNonNetworkableEntity(this);
	ClientThinkList()->SetNextClientThink(GetClientHandle(), CLIENT_THINK_ALWAYS);

	AddToLeafSystem(RENDER_GROUP_OPAQUE_ENTITY);

	m_pMaterial = materials->FindMaterial("cable/cable", TEXTURE_GROUP_OTHER);
	m_flSitStillTime = 2;
}

//-----------------------------------------------------------------------------
// THINK
//-----------------------------------------------------------------------------
void C_Hairball::ClientThink()
{
	Assert(!GetMoveParent());

	// TEXT MODE
	if (m_bHasTextTarget)
	{
		Vector to = m_vTextTarget - GetLocalOrigin();

		if (to.Length() > 2.0f)
		{
			SetLocalOrigin(GetLocalOrigin() + to * 3.0f * gpGlobals->frametime);
		}

		QAngle ang = GetLocalAngles();
		ang.y += 90 * gpGlobals->frametime;
		SetLocalAngles(ang);
	}

	// transform hairs
	VMatrix m;
	m.SetupMatrixOrgAngles(GetLocalOrigin(), GetLocalAngles());

	for (int i = 0; i < m_HairPositions.Count(); i++)
	{
		Vector3DMultiplyPosition(m, m_HairPositions[i] * m_flSphereRadius, m_TransformedHairPositions[i]);
	}

	if (m_bFirstThink)
	{
		m_bFirstThink = false;

		for (int i = 0; i < m_HairPositions.Count(); i++)
		{
			for (int j = 0; j < m_nNodesPerHair; j++)
			{
				m_Nodes[i * m_nNodesPerHair + j].Init(m_TransformedHairPositions[i]);
			}
		}
	}

	m_Physics.Simulate(m_Nodes.Base(), m_Nodes.Count(), &m_Delegate, gpGlobals->frametime, 0.98);
}

//-----------------------------------------------------------------------------
// DRAW
//-----------------------------------------------------------------------------
int C_Hairball::DrawModel(int flags)
{
	if (!m_pMaterial)
		return 0;

	CMatRenderContextPtr ctx(g_pMaterialSystem);

	for (int i = 0; i < m_nHairs; i++)
	{
		CSimplePhysics::CNode* pBase = &m_Nodes[i * m_nNodesPerHair];

		CBeamSegDraw beam;
		beam.Start(ctx, m_nNodesPerHair - 1, m_pMaterial);

		for (int j = 0; j < m_nNodesPerHair; j++)
		{
			BeamSeg_t seg;
			seg.m_vPos = pBase[j].m_vPredicted;
			seg.m_vColor.Init(0.2f, 0.6f, 1.0f);
			seg.m_flWidth = 1.5f;
			seg.m_flAlpha = 1;

			beam.NextSeg(&seg);
		}

		beam.End();
	}

	return 1;
}

//////////////////////////////////////////////////////////
// TEXT GENERATION
//////////////////////////////////////////////////////////

void AddPoint(CUtlVector<Vector>& p, int x, int y)
{
	p.AddToTail(Vector(x * 40, 0, -y * 40));
}

void BuildText(CUtlVector<Vector>& p)
{
	// VERY SIMPLE BLOCK LETTERS
	for (int i = 0; i < 5; i++) AddPoint(p, 0, i); // G left
	for (int i = 0; i < 4; i++) AddPoint(p, i, 0);
	for (int i = 0; i < 4; i++) AddPoint(p, i, 4);

	// A
	for (int i = 0; i < 5; i++) AddPoint(p, 6, i);
	for (int i = 0; i < 5; i++) AddPoint(p, 10, i);

	// B
	for (int i = 0; i < 5; i++) AddPoint(p, 12, i);

	// E
	for (int i = 0; i < 5; i++) AddPoint(p, 18, i);

	// M O D (simplified)
	for (int i = 0; i < 5; i++) AddPoint(p, 24, i);
	for (int i = 0; i < 5; i++) AddPoint(p, 28, i);
}

//-----------------------------------------------------------------------------
// SPAWN
//-----------------------------------------------------------------------------
void CreateHairballCallback()
{
	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
		return;

	CUtlVector<Vector> points;
	BuildText(points);

	Vector f, r, u;
	AngleVectors(pPlayer->EyeAngles(), &f, &r, &u);

	Vector origin = pPlayer->GetAbsOrigin() + f * 500 + u * 100;

	for (int i = 0; i < points.Count(); i++)
	{
		C_Hairball* h = new C_Hairball;
		h->Init();

		h->SetLocalOrigin(pPlayer->GetAbsOrigin() + RandomVector(-200, 200));

		h->m_vTextTarget = origin + r * points[i].x + u * points[i].z;
		h->m_bHasTextTarget = true;
	}
}

ConCommand cc_CreateHairball("CreateHairball", CreateHairballCallback, 0, FCVAR_CHEAT);
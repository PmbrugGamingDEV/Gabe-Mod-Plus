#include "cbase.h"
#include "dt_utlvector_recv.h"
#include "c_ai_basenpc.h"
#include "IVRenderView.h"
#include "view_shared.h"
#include "iviewrender.h"
#include "tier0/vprof.h"
#include "debugoverlay_shared.h"
#include "soundinfo.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imesh.h"

#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// NPC class
//-----------------------------------------------------------------------------
class C_NPC_Surface : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_NPC_Surface, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();
	DECLARE_INTERPOLATION();

	C_NPC_Surface();
	virtual ~C_NPC_Surface();

	virtual void GetRenderBounds(Vector& mins, Vector& maxs);
	virtual bool IsTransparent(void) { return true; }
	virtual int DrawModel(int flags);

#define MAX_SURFACE_ELEMENTS 1000

	IMaterial* m_pMaterial;

	CUtlVector<Vector> m_vecSurfacePos;
	CUtlVector<CInterpolatedVar<Vector>> m_iv_vecSurfacePos;

	CUtlVector<float> m_flSurfaceV;
	CUtlVector<CInterpolatedVar<float>> m_iv_flSurfaceV;

	CUtlVector<float> m_flSurfaceR;
	CUtlVector<CInterpolatedVar<float>> m_iv_flSurfaceR;

	int   m_nActiveParticles;
	float m_flRadius;
};

//-----------------------------------------------------------------------------
// Networking
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT(C_NPC_Surface, DT_NPC_Surface, CNPC_Surface)
RecvPropFloat(RECVINFO(m_flRadius)),
RecvPropInt(RECVINFO(m_nActiveParticles)),
RecvPropUtlVector(
	RECVINFO_UTLVECTOR(m_vecSurfacePos),
	MAX_SURFACE_ELEMENTS,
	RecvPropVector(NULL, 0, sizeof(Vector))),
	RecvPropUtlVector(
		RECVINFO_UTLVECTOR(m_flSurfaceV),
		MAX_SURFACE_ELEMENTS,
		RecvPropFloat(NULL, 0, sizeof(float))),
	RecvPropUtlVector(
		RECVINFO_UTLVECTOR(m_flSurfaceR),
		MAX_SURFACE_ELEMENTS,
		RecvPropFloat(NULL, 0, sizeof(float))),
	END_RECV_TABLE()

	//-----------------------------------------------------------------------------
	// Constructor
	//-----------------------------------------------------------------------------
	C_NPC_Surface::C_NPC_Surface()
{
	m_pMaterial = materials->FindMaterial("debug/debugvertexcolor", TEXTURE_GROUP_OTHER);

	m_vecSurfacePos.EnsureCount(MAX_SURFACE_ELEMENTS);
	m_iv_vecSurfacePos.EnsureCount(MAX_SURFACE_ELEMENTS);

	m_flSurfaceV.EnsureCount(MAX_SURFACE_ELEMENTS);
	m_iv_flSurfaceV.EnsureCount(MAX_SURFACE_ELEMENTS);

	m_flSurfaceR.EnsureCount(MAX_SURFACE_ELEMENTS);
	m_iv_flSurfaceR.EnsureCount(MAX_SURFACE_ELEMENTS);

	for (int i = 0; i < MAX_SURFACE_ELEMENTS; i++)
	{
		AddVar(&m_vecSurfacePos[i], &m_iv_vecSurfacePos[i], LATCH_ANIMATION_VAR);
		AddVar(&m_flSurfaceV[i], &m_iv_flSurfaceV[i], LATCH_ANIMATION_VAR);
		AddVar(&m_flSurfaceR[i], &m_iv_flSurfaceR[i], LATCH_ANIMATION_VAR);
	}
}

C_NPC_Surface::~C_NPC_Surface()
{}

//-----------------------------------------------------------------------------
// Bounds
//-----------------------------------------------------------------------------
void C_NPC_Surface::GetRenderBounds(Vector& mins, Vector& maxs)
{
	if (m_nActiveParticles <= 0)
	{
		mins = Vector(-32, -32, -32);
		maxs = Vector(32, 32, 32);
		return;
	}

	mins = m_vecSurfacePos[0];
	maxs = m_vecSurfacePos[0];

	float r = m_flRadius * 2.0f;

	for (int i = 0; i < m_nActiveParticles; i++)
	{
		VectorMin(m_vecSurfacePos[i] - Vector(r, r, r), mins, mins);
		VectorMax(m_vecSurfacePos[i] + Vector(r, r, r), maxs, maxs);
	}

	mins -= GetRenderOrigin();
	maxs -= GetRenderOrigin();
}

//-----------------------------------------------------------------------------
// Simple sphere draw helper
//-----------------------------------------------------------------------------
void DrawDebugSphere(const Vector& pos, float radius)
{
	NDebugOverlay::Sphere(
		pos,
		QAngle(0, 0, 0),
		radius,
		0, 150, 255, 255,
		false,
		0.05f
	);
}

//-----------------------------------------------------------------------------
// Render (FIXED)
//-----------------------------------------------------------------------------
int C_NPC_Surface::DrawModel(int flags)
{
	if (m_nActiveParticles <= 0)
		return 0;

	// simple debug render instead of blobulator
	for (int i = 0; i < m_nActiveParticles; i++)
	{
		float r = (i < m_flSurfaceR.Count()) ? m_flSurfaceR[i] : 1.0f;

		DrawDebugSphere(m_vecSurfacePos[i], r * m_flRadius);
	}

	return 1;
}
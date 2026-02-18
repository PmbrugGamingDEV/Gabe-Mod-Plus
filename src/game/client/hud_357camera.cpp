#include "cbase.h"
#include "hud_357camera.h"
#include "iclientmode.h"
#include "view_shared.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/itexture.h"
#include "materialsystem/imesh.h"
#include "vgui/ISurface.h"

#include "tier0/memdbgon.h"

using namespace vgui;

DECLARE_HUDELEMENT(CHudWeaponCamera);

CHudWeaponCamera::CHudWeaponCamera(const char* pElementName)
    : CHudElement(pElementName), Panel(g_pClientMode->GetViewport(), "HudWeaponCamera")
{
    SetHiddenBits(HIDEHUD_PLAYERDEAD);
    SetPaintBackgroundEnabled(false);
    SetPaintBorderEnabled(false);
}

void CHudWeaponCamera::Init()
{
    m_pMaterial = NULL;
    m_pRenderTarget = NULL;
}

void CHudWeaponCamera::VidInit()
{
    IMaterialSystem* pMatSys = materials;

    m_pRenderTarget = pMatSys->CreateNamedRenderTargetTextureEx(
        "_rt_weaponcam",
        256, 192,
        RT_SIZE_NO_CHANGE,
        IMAGE_FORMAT_RGBA8888,
        MATERIAL_RT_DEPTH_SHARED,
        TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
        CREATERENDERTARGETFLAGS_HDR
    );

    m_pMaterial = pMatSys->FindMaterial("effects/weapon_camera", TEXTURE_GROUP_RENDER_TARGET);
}

bool CHudWeaponCamera::ShouldDraw()
{
    C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
    if (!pPlayer)
        return false;

    C_BaseCombatWeapon* pWeapon = pPlayer->GetActiveWeapon();
    if (!pWeapon)
        return false;

    // Only show when weapon_357camera is active
    return FStrEq(pWeapon->GetClassname(), "weapon_357camera");
}

void CHudWeaponCamera::Paint()
{
    if (!m_pMaterial)
        return;

    int x = 8;
    int y = 8;
    int w = 256;
    int h = 192;

    IMatRenderContext* pRenderContext = materials->GetRenderContext();
    IMesh* pMesh = pRenderContext->GetDynamicMesh(true, NULL, NULL, m_pMaterial);

    CMeshBuilder mesh;
    mesh.Begin(pMesh, MATERIAL_QUADS, 1);

    mesh.Position3f(x, y, 0); mesh.TexCoord2f(0, 0, 0); mesh.Color4ub(255, 255, 255, 255); mesh.AdvanceVertex();
    mesh.Position3f(x + w, y, 0); mesh.TexCoord2f(0, 1, 0); mesh.Color4ub(255, 255, 255, 255); mesh.AdvanceVertex();
    mesh.Position3f(x + w, y + h, 0); mesh.TexCoord2f(0, 1, 1); mesh.Color4ub(255, 255, 255, 255); mesh.AdvanceVertex();
    mesh.Position3f(x, y + h, 0); mesh.TexCoord2f(0, 0, 1); mesh.Color4ub(255, 255, 255, 255); mesh.AdvanceVertex();

    mesh.End();
    pMesh->Draw();
}

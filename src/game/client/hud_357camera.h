#pragma once
#include "cbase.h"
#include "hudelement.h"
#include "vgui_controls/Panel.h"

class CHudWeaponCamera : public CHudElement, public vgui::Panel
{
    DECLARE_CLASS_SIMPLE(CHudWeaponCamera, vgui::Panel);

public:
    CHudWeaponCamera(const char* pElementName);

    virtual void Init();
    virtual void VidInit();
    virtual void Paint();
    virtual bool ShouldDraw();

private:
    IMaterial* m_pMaterial;
    ITexture* m_pRenderTarget;
};

//=========================================================
// Gabe Mod – Minimal HUD Watermark (Top-Right)
//=========================================================

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "vgui/IScheme.h"
#include "vgui/ISurface.h"
#include "vgui_controls/Panel.h"

// memdbgon must be last
#include "tier0/memdbgon.h"

using namespace vgui;

extern IClientMode* g_pClientMode;

//=========================================================

class CHudWatermark : public CHudElement, public Panel
{
public:
    DECLARE_CLASS_SIMPLE(CHudWatermark, Panel);

    CHudWatermark(const char* pElementName);

    virtual void ApplySchemeSettings(IScheme* pScheme);
    virtual bool ShouldDraw(void);
    virtual void Paint(void);

private:
    HFont m_hFont;
};

DECLARE_HUDELEMENT(CHudWatermark);

//=========================================================

CHudWatermark::CHudWatermark(const char* pElementName)
    : CHudElement(pElementName), Panel(NULL, "HudWatermark")
{
    SetParent(g_pClientMode->GetViewport());
    SetHiddenBits(0);
    SetPaintBackgroundEnabled(false);
}

//=========================================================

void CHudWatermark::ApplySchemeSettings(IScheme* pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);

    m_hFont = surface()->CreateFont();
    surface()->SetFontGlyphSet(
        m_hFont,
        "Consolas",
        16,
        400,
        0,
        0,
        0x010 | 0x200
    );

    int w, h;
    surface()->GetScreenSize(w, h);
    SetPos(0, 0);
    SetSize(w, h);
}

//=========================================================

bool CHudWatermark::ShouldDraw(void)
{
    return true;
}

//=========================================================

void CHudWatermark::Paint(void)
{
    const int margin = 8;
    const int padding = 6;

    const wchar_t* line1 = L"Gabe Mod 8.1";
    const wchar_t* line2 = L"sites.google.com/pmbruggaming";

    int sw, sh;
    surface()->GetScreenSize(sw, sh);

    int w1, h1, w2, h2;
    surface()->GetTextSize(m_hFont, line1, w1, h1);
    surface()->GetTextSize(m_hFont, line2, w2, h2);

    int boxW = max(w1, w2) + padding * 2;
    int boxH = h1 + h2 + padding * 2 + 2;

    int boxX = sw - boxW - margin;
    int boxY = margin;

    // Background
    surface()->DrawSetColor(0, 0, 0, 130);
    surface()->DrawFilledRect(boxX, boxY, boxX + boxW, boxY + boxH);

    // Outline
    surface()->DrawSetColor(0, 0, 0, 210);
    surface()->DrawOutlinedRect(boxX, boxY, boxX + boxW, boxY + boxH);

    // Top accent line
    surface()->DrawSetColor(110, 110, 110, 180);
    surface()->DrawFilledRect(boxX, boxY, boxX + boxW, boxY + 1);

    // Text
    int x = boxX + padding;
    int y = boxY + padding;

    surface()->DrawSetTextFont(m_hFont);

    surface()->DrawSetTextColor(230, 230, 230, 210);
    surface()->DrawSetTextPos(x, y);
    surface()->DrawPrintText(line1, wcslen(line1));
    y += h1 + 1;

    surface()->DrawSetTextColor(160, 160, 160, 210);
    surface()->DrawSetTextPos(x, y);
    surface()->DrawPrintText(line2, wcslen(line2));
}

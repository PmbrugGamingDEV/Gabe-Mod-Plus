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
#include "tier0/icommandline.h"
#include "filesystem.h"
#include "tier1/KeyValues.h"
#include "convar.h"
#include "cdll_client_int.h"

// memdbgon must be last
#include "tier0/memdbgon.h"

using namespace vgui;

extern IClientMode* g_pClientMode;

//=========================================================
// ConVars
//=========================================================

ConVar gabeplus_speedrun("gabeplus_speedrun", "0", FCVAR_CLIENTDLL,
    "Shows speedrun timer, for speedrunning purposes");

//=========================================================
// Globals
//=========================================================

static float g_flMapStartTime = 0.0f;
static float g_flPersonalBest = -1.0f;
static bool g_bPBLoaded = false;

//=========================================================
// Personal Best Loading
//=========================================================

void LoadPersonalBest()
{
    if (g_bPBLoaded)
        return;

    g_bPBLoaded = true;

    const char* map = engine->GetLevelName();

    KeyValues* kv = new KeyValues("SpeedrunPB");

    if (kv->LoadFromFile(filesystem, "cfg/gabeplus_speedrun.txt", "MOD"))
    {
        g_flPersonalBest = kv->GetFloat(map, -1.0f);
    }

    kv->deleteThis();
}

//=========================================================
// Personal Best Saving
//=========================================================

void SavePersonalBest(float newTime)
{
    const char* map = engine->GetLevelName();

    KeyValues* kv = new KeyValues("SpeedrunPB");

    kv->LoadFromFile(filesystem, "cfg/gabeplus_speedrun.txt", "MOD");

    kv->SetFloat(map, newTime);

    kv->SaveToFile(filesystem, "cfg/gabeplus_speedrun.txt", "MOD");

    kv->deleteThis();
}


void CheckForLevelFinish()
{
    C_BasePlayer* player = C_BasePlayer::GetLocalPlayer();

    if (!player)
        return;

    trace_t tr;
    UTIL_TraceLine(
        player->EyePosition(),
        player->EyePosition() + Vector(0, 0, -64),
        MASK_ALL,
        player,
        COLLISION_GROUP_NONE,
        &tr
    );

    if (tr.m_pEnt && FClassnameIs(tr.m_pEnt, "trigger_changelevel"))
    {
        float finalTime = gpGlobals->curtime - g_flMapStartTime;

        if (g_flPersonalBest < 0 || finalTime < g_flPersonalBest)
        {
            g_flPersonalBest = finalTime;
            SavePersonalBest(finalTime);
        }
    }
}

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

    g_flMapStartTime = gpGlobals->curtime;

    LoadPersonalBest();
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

    wchar_t timeText[32];
    wchar_t pbText[32];

    bool drawTimer = gabeplus_speedrun.GetBool();

    float elapsed = 0.0f;

    if (drawTimer)
    {
        elapsed = gpGlobals->curtime - g_flMapStartTime;

        int minutes = (int)(elapsed / 60);
        int seconds = (int)elapsed % 60;
        int millis = (int)((elapsed - (int)elapsed) * 1000);

        swprintf_s(timeText, _countof(timeText),
            L"%02d:%02d.%03d", minutes, seconds, millis);

        // PB check
        if (g_flPersonalBest < 0 || elapsed < g_flPersonalBest)
        {
            g_flPersonalBest = elapsed;
            SavePersonalBest(elapsed);
        }

        if (g_flPersonalBest > 0)
        {
            int pbm = (int)(g_flPersonalBest / 60);
            int pbs = (int)g_flPersonalBest % 60;
            int pbms = (int)((g_flPersonalBest - (int)g_flPersonalBest) * 1000);

            swprintf_s(pbText, _countof(pbText),
                L"PB %02d:%02d.%03d", pbm, pbs, pbms);
        }
    }

    int sw, sh;
    surface()->GetScreenSize(sw, sh);

    int w1, h1, w2, h2;
    surface()->GetTextSize(m_hFont, line1, w1, h1);
    surface()->GetTextSize(m_hFont, line2, w2, h2);

    int w3 = 0, h3 = 0;
    int w4 = 0, h4 = 0;

    if (drawTimer)
        surface()->GetTextSize(m_hFont, timeText, w3, h3);

    if (drawTimer && g_flPersonalBest > 0)
        surface()->GetTextSize(m_hFont, pbText, w4, h4);

    int boxW = max(max(max(w1, w2), w3), w4) + padding * 2;

    int boxH = h1 + h2 + padding * 2 + 2;

    if (drawTimer)
        boxH += h3 + 2;

    if (drawTimer && g_flPersonalBest > 0)
        boxH += h4 + 2;

    int boxX = sw - boxW - margin;
    int boxY = margin;

    // Background
    surface()->DrawSetColor(0, 0, 0, 130);
    surface()->DrawFilledRect(boxX, boxY, boxX + boxW, boxY + boxH);

    // Outline
    surface()->DrawSetColor(0, 0, 0, 210);
    surface()->DrawOutlinedRect(boxX, boxY, boxX + boxW, boxY + boxH);

    // Accent line
    surface()->DrawSetColor(110, 110, 110, 180);
    surface()->DrawFilledRect(boxX, boxY, boxX + boxW, boxY + 1);

    int x = boxX + padding;
    int y = boxY + padding;

    surface()->DrawSetTextFont(m_hFont);

    // Title
    surface()->DrawSetTextColor(230, 230, 230, 210);
    surface()->DrawSetTextPos(x, y);
    surface()->DrawPrintText(line1, wcslen(line1));
    y += h1 + 1;

    // Website
    surface()->DrawSetTextColor(160, 160, 160, 210);
    surface()->DrawSetTextPos(x, y);
    surface()->DrawPrintText(line2, wcslen(line2));
    y += h2 + 2;

    if (drawTimer)
    {
        // Timer
        surface()->DrawSetTextColor(255, 210, 120, 210);
        surface()->DrawSetTextPos(x, y);
        surface()->DrawPrintText(timeText, wcslen(timeText));
        y += h3 + 2;

        // PB
        if (g_flPersonalBest > 0)
        {
            surface()->DrawSetTextColor(120, 255, 120, 210);
            surface()->DrawSetTextPos(x, y);
            surface()->DrawPrintText(pbText, wcslen(pbText));
        }
    }

    CheckForLevelFinish();
}
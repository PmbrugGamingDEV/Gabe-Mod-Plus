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
#include "con_nprint.h"

// memdbgon must be last
#include "tier0/memdbgon.h"

using namespace vgui;

extern IClientMode* g_pClientMode;

//=========================================================
// ConVars
//=========================================================

ConVar gabeplus_speedrun("gabeplus_speedrun", "0", FCVAR_CLIENTDLL,
    "Shows speedrun timer, for speedrunning purposes");
ConVar please_dontsteal("please_dontsteal", "0", FCVAR_ARCHIVE, "Hides watermark");
ConVar gabeplus_legacy("gabeplus_legacy", "0", FCVAR_CLIENTDLL,
	"Use legacy console watermark rendering");

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
    if (please_dontsteal.GetBool())
    {
        return false;
    }
    else
    {
        return true;
    }
}

//=========================================================

void CHudWatermark::Paint(void)
{

	// 🔥 LEGACY MODE (console text)
	if (gabeplus_legacy.GetBool())
	{
		con_nprint_s info;
		info.fixed_width_font = false;
		info.color[0] = 1.0f;
		info.color[1] = 0.5f;
		info.color[2] = 0.0f;
		info.time_to_live = 0.1f;

		info.index = 0;
		engine->Con_NXPrintf(&info, "GABE MOD v8.1");

		info.index = 1;
		info.color[0] = 0.0f;
		info.color[1] = 0.8f;
		info.color[2] = 1.0f;
		engine->Con_NXPrintf(&info, "sites.google.com/pmbruggaming");

		return; // 🔥 skip VGUI rendering
	}

    int sw, sh;
    surface()->GetScreenSize(sw, sh);

    const int margin = 14;
    const int padding = 10;

    const wchar_t* title = L"GABE MOD";
    const wchar_t* version = L"v8.1";
    const wchar_t* website = L"sites.google.com/pmbruggaming";

    int wTitle, hTitle;
    int wVersion, hVersion;
    int wSite, hSite;

    surface()->GetTextSize(m_hFont, title, wTitle, hTitle);
    surface()->GetTextSize(m_hFont, version, wVersion, hVersion);
    surface()->GetTextSize(m_hFont, website, wSite, hSite);

    int boxW = max(max(wTitle, wVersion), wSite) + padding * 2;
    int boxH = hTitle + hVersion + hSite + padding * 2 + 8;

    int x = sw - boxW - margin;
    int y = margin;

    // =========================
    // SHADOW (soft)
    // =========================
    surface()->DrawSetColor(0, 0, 0, 100);
    surface()->DrawFilledRect(x + 2, y + 2, x + boxW + 2, y + boxH + 2);

    // =========================
    // BACKGROUND (glass)
    // =========================
    surface()->DrawSetColor(20, 20, 20, 170);
    surface()->DrawFilledRect(x, y, x + boxW, y + boxH);

    // =========================
    // OUTLINE
    // =========================
    surface()->DrawSetColor(0, 0, 0, 220);
    surface()->DrawOutlinedRect(x, y, x + boxW, y + boxH);

    // =========================
    // TOP ACCENT BAR
    // =========================
    surface()->DrawSetColor(255, 140, 0, 220); // orange
    surface()->DrawFilledRect(x, y, x + boxW, y + 3);

    int tx = x + padding;
    int ty = y + padding;

    surface()->DrawSetTextFont(m_hFont);

    // =========================
    // TITLE (bold look)
    // =========================
    surface()->DrawSetTextColor(255, 255, 255, 230);
    surface()->DrawSetTextPos(tx, ty);
    surface()->DrawPrintText(title, wcslen(title));

    ty += hTitle;

    // =========================
    // SUBTITLE (cyan accent)
    // =========================
    surface()->DrawSetTextColor(0, 200, 255, 220);
    surface()->DrawSetTextPos(tx, ty);
    surface()->DrawPrintText(version, wcslen(version));

    ty += hVersion + 4;

    // =========================
    // SEPARATOR
    // =========================
    surface()->DrawSetColor(80, 80, 80, 180);
    surface()->DrawFilledRect(x + padding, ty, x + boxW - padding, ty + 1);

    ty += 4;

    // =========================
    // WEBSITE
    // =========================
    surface()->DrawSetTextColor(160, 160, 160, 220);
    surface()->DrawSetTextPos(tx, ty);
    surface()->DrawPrintText(website, wcslen(website));
}
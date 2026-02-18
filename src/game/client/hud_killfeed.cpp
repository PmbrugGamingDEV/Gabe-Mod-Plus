#include "cbase.h"
#include "hud.h"
#include "hud_macros.h"
#include "hudelement.h"
#include "iclientmode.h"
#include "vgui/ISurface.h"
#include "vgui/ILocalize.h"
#include "hud_killfeed.h"

using namespace vgui;

DECLARE_HUDELEMENT( CHudKillFeed );
DECLARE_HUD_MESSAGE( CHudKillFeed, KillFeed );
DECLARE_HUD_MESSAGE( CHudKillFeed, DamageFeed );

CHudKillFeed::CHudKillFeed(const char *pElementName)
    : CHudElement(pElementName), Panel(NULL, "HudKillFeed")
{
    SetParent(g_pClientMode->GetViewport());
    SetPaintBackgroundEnabled(false);
    SetBounds(0, 0, ScreenWidth(), ScreenHeight());
}

void CHudKillFeed::Init()
{
    HOOK_HUD_MESSAGE( CHudKillFeed, KillFeed );
	HOOK_HUD_MESSAGE( CHudKillFeed, DamageFeed );
    m_Lines.Purge();
}

void CHudKillFeed::Reset()
{
    m_Lines.Purge();
}

void CHudKillFeed::MsgFunc_KillFeed( bf_read &msg )
{
    char attacker[64], victim[64];
    msg.ReadString(attacker, sizeof(attacker));
    msg.ReadString(victim, sizeof(victim));

    char buffer[128];
    Q_snprintf(buffer, sizeof(buffer), "%s killed %s", attacker, victim);

    KillFeedLine line;
    g_pVGuiLocalize->ConvertANSIToUnicode(buffer, line.text, sizeof(line.text));
    line.timeAdded = gpGlobals->curtime;
	line.isDamage = false;

    m_Lines.AddToHead(line);

    // keep only last 5 entries
    if (m_Lines.Count() > 5)
        m_Lines.Remove(m_Lines.Count() - 1);
}

void CHudKillFeed::MsgFunc_DamageFeed( bf_read &msg )
{
    char attacker[64], victim[64], dmgType[32];
    int dmg = 0;

    msg.ReadString( attacker, sizeof(attacker) );
    msg.ReadString( victim, sizeof(victim) );
    dmg = (int)msg.ReadShort();
    msg.ReadString( dmgType, sizeof(dmgType) );

    char buffer[160];
    Q_snprintf( buffer, sizeof(buffer), "%s gave +%d %s to %s", attacker, dmg, dmgType, victim );

    KillFeedLine line;
    g_pVGuiLocalize->ConvertANSIToUnicode( buffer, line.text, sizeof(line.text) );
    line.timeAdded = gpGlobals->curtime;
	line.isDamage = true;

    m_Lines.AddToHead( line );
    if ( m_Lines.Count() > 5 )
        m_Lines.Remove( m_Lines.Count() - 1 );
}

void CHudKillFeed::Paint()
{
    vgui::HFont font = scheme()->GetIScheme(GetScheme())->GetFont("Default", true);
    int y = 25; // top offset

    for (int i = 0; i < m_Lines.Count(); i++)
    {
        // fade out after 5 seconds
        float age = gpGlobals->curtime - m_Lines[i].timeAdded;
        if (age > 5.0f)
        {
            m_Lines.Remove(i);
            i--;
            continue;
        }

        int alpha = RemapValClamped(age, 0.0f, 5.0f, 255.0f, 0.0f);

        surface()->DrawSetTextFont(font);
			if ( m_Lines[i].isDamage )
			{
				surface()->DrawSetTextColor( 255, 128, 0, alpha ); // orange for damage
			}
	else
	{
		surface()->DrawSetTextColor( 255, 0, 0, alpha );   // Red for kills
	}
        surface()->DrawSetTextPos(10, y);
        surface()->DrawPrintText(m_Lines[i].text, wcslen(m_Lines[i].text));

        y += 15;
    }
}

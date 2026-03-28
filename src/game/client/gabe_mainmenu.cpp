#include "cbase.h"
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Label.h>
#include "vgui/iinput.h"
#include "ienginevgui.h"

using namespace vgui;

//------------------------------------------------------------
// Menu item definition
//------------------------------------------------------------
struct GabeMenuItem
{
    const char* label;
    const char* command;
};

//------------------------------------------------------------
// Your menu (from GameMenu.res)
//------------------------------------------------------------
GabeMenuItem g_MenuItems[] =
{
    { "#GameUI_GameMenu_ResumeGame", "ResumeGame" },
    { "#GameUI_GameMenu_Disconnect", "Disconnect" },
    { "#GameUI_GameMenu_PlayerList", "OpenPlayerListDialog" },

    { "", "" },

    { "CREATE NEW GAME", "engine gabeplus_newgame" },
    { "FIND GAMES", "OpenServerBrowser" },
    { "FRIENDS LIST", "engine gabeplus_friends" },
    { "THANKS", "engine gabeplus_thanks" },
    { "TUTORIALS", "engine gabeplus_tutorial" },
    { "VERSION DETAILS", "engine gabeplus_chlog" },
    { "ACHIEVEMENTS", "OpenAchievementsDialog" },
    { "OPTIONS", "OpenOptionsDialog" },
    { "QUIT", "Quit" },
};

//------------------------------------------------------------
// Animation struct
//------------------------------------------------------------
struct MenuButtonAnim
{
    Button* button;
    CUtlString command;
    int targetY;
    float spawnTime;
    float alpha;
};

//------------------------------------------------------------
// Main Menu Panel
//------------------------------------------------------------
class CGabeMainMenu : public Frame
{
    DECLARE_CLASS_SIMPLE(CGabeMainMenu, Frame);

public:
    CGabeMainMenu(VPANEL parent)
        : BaseClass(NULL, "GabeMainMenu")
    {
        SetParent(parent);

        SetTitle("", false);
        SetMoveable(false);
        SetSizeable(false);
        SetCloseButtonVisible(false);
        SetPaintBackgroundEnabled(false);

        SetMouseInputEnabled(true);
        SetKeyBoardInputEnabled(true);

        SetScheme("ClientScheme");

        m_pTitle = new Label(this, "Title", "GABEMOD+");
        m_pTitle->SetContentAlignment(Label::a_center);

        m_flStartTime = gpGlobals->curtime;

        //------------------------------------------------------------
        // Create buttons dynamically
        //------------------------------------------------------------
        int visualIndex = 0;

        for (int i = 0; i < ARRAYSIZE(g_MenuItems); i++)
        {
            if (!g_MenuItems[i].label[0])
                continue;

            AddMenuButton(g_MenuItems[i].label, g_MenuItems[i].command, visualIndex);
            visualIndex++;
        }
    }

    //------------------------------------------------------------
    void AddMenuButton(const char* text, const char* cmd, int index)
    {
        Button* btn = new Button(this, text, text, this, "dynamic");

        btn->SetBgColor(Color(50, 50, 50, 255));
        btn->SetFgColor(Color(255, 255, 255, 255));
        btn->SetAlpha(0);

        MenuButtonAnim anim;
        anim.button = btn;
        anim.command = cmd;
        anim.targetY = 200 + index * 55;
        anim.spawnTime = gpGlobals->curtime + index * 0.12f;
        anim.alpha = 0.0f;

        btn->SetPos(0, anim.targetY + 60);

        m_Buttons.AddToTail(anim);
    }

    //------------------------------------------------------------
    void PerformLayout()
    {
        int w, h;
        surface()->GetScreenSize(w, h);
        SetBounds(0, 0, w, h);

        m_pTitle->SetPos(w / 2 - 200, 80);
        m_pTitle->SetSize(400, 40);

        FOR_EACH_VEC(m_Buttons, i)
        {
            m_Buttons[i].button->SetSize(260, 34);
        }
    }

    //------------------------------------------------------------
    void OnThink()
    {
        BaseClass::OnThink();

        float time = gpGlobals->curtime;

        int w, h;
        GetSize(w, h);

        FOR_EACH_VEC(m_Buttons, i)
        {
            MenuButtonAnim& anim = m_Buttons[i];

            if (time < anim.spawnTime)
                continue;

            int x, y;
            anim.button->GetPos(x, y);

            int targetY = anim.targetY;

            // Smooth slide
            y = y + (targetY - y) * 0.18f;

            anim.button->SetPos(w / 2 - 130, y);

            // Fade in
            anim.alpha += 20;
            if (anim.alpha > 255)
                anim.alpha = 255;

            anim.button->SetAlpha((int)anim.alpha);
        }
    }

    //------------------------------------------------------------
    void OnCommand(const char* cmd)
    {
        if (!Q_stricmp(cmd, "dynamic"))
        {
            VPANEL vpanel = input()->GetFocus();
            Panel* focus = ipanel()->GetPanel(vpanel, "ClientDLL");
            Button* pressed = dynamic_cast<Button*>(focus);

            if (!pressed)
                return;

            FOR_EACH_VEC(m_Buttons, i)
            {
                if (m_Buttons[i].button == pressed)
                {
                    ExecuteCommand(m_Buttons[i].command.String());
                    return;
                }
            }
        }

        BaseClass::OnCommand(cmd);
    }

    //------------------------------------------------------------
    void ExecuteCommand(const char* cmd)
    {
        if (!cmd || !cmd[0])
            return;

        // Handle "engine ..." commands
        if (!Q_strnicmp(cmd, "engine ", 7))
        {
            engine->ClientCmd_Unrestricted(cmd + 7);
            return;
        }

        // Send to GameUI (THIS FIXES YOUR ERROR)
        VPANEL root = enginevgui->GetPanel(PANEL_GAMEUIDLL);

        if (root)
        {
            KeyValues* kv = new KeyValues("Command");
            kv->SetString("command", cmd);

            ivgui()->PostMessage(root, kv, NULL);
        }
    }

private:
    Label* m_pTitle;
    CUtlVector<MenuButtonAnim> m_Buttons;
    float m_flStartTime;
};

//------------------------------------------------------------
// GLOBAL INSTANCE
//------------------------------------------------------------
static CGabeMainMenu* g_pMainMenu = NULL;

//------------------------------------------------------------
// Toggle function
//------------------------------------------------------------
void ToggleGabeMenu()
{
    VPANEL root = enginevgui->GetPanel(PANEL_GAMEUIDLL);
    if (!root)
        return;

    if (!g_pMainMenu)
        g_pMainMenu = new CGabeMainMenu(root);

    g_pMainMenu->SetParent(root);

    bool visible = g_pMainMenu->IsVisible();
    g_pMainMenu->SetVisible(!visible);

    if (!visible)
    {
        g_pMainMenu->MakePopup();
        g_pMainMenu->MoveToFront();
        g_pMainMenu->RequestFocus();
    }
}

//------------------------------------------------------------
// Console command
//------------------------------------------------------------
CON_COMMAND(gabe_menu, "Open Gabe menu")
{
    ToggleGabeMenu();
}
#include "cbase.h"
#ifndef _DEBUG
#include "vgui_controls/Frame.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/Label.h"
#include "vgui/IPanel.h"
#include "ienginevgui.h"
#include "VAwesomium.h"

using namespace vgui;

class CAwesomiumBrowser : public Frame
{
    DECLARE_CLASS_SIMPLE(CAwesomiumBrowser, Frame);

public:

    CAwesomiumBrowser(Panel* parent)
        : BaseClass(parent, "AwesomiumBrowser"),
        m_bLoading(false)
    {
        SetSize(1200, 800);
        SetTitle("Awesomium Web Browser", true);
        SetDeleteSelfOnClose(false);
        MakePopup();
        SetVisible(true);

        // Controls
        m_pBack = new Button(this, "Back", "<", this, "Back");
        m_pForward = new Button(this, "Forward", ">", this, "Forward");
        m_pRefresh = new Button(this, "Refresh", "R", this, "Refresh");
        m_pStop = new Button(this, "Stop", "X", this, "Stop");
        m_pHome = new Button(this, "Home", "Home", this, "Home");

        m_pAddress = new TextEntry(this, "AddressBar");
        m_pAddress->SetText("http://example.com");

        m_pGo = new Button(this, "Go", "Go", this, "Go");

        m_pStatus = new Label(this, "Status", "Ready");
        m_pStatus->SetContentAlignment(Label::a_west);

        // Browser
        m_pBrowser = new VAwesomium(this, "Browser");

        PerformLayout();

        Navigate("http://example.com");
    }

    virtual void PerformLayout() override
    {
        BaseClass::PerformLayout();

        int w, h;
        GetSize(w, h);

        int top = 30;
        int bottom = 25;

        int x = 5;

        m_pBack->SetBounds(x, 5, 30, 20); x += 35;
        m_pForward->SetBounds(x, 5, 30, 20); x += 35;
        m_pRefresh->SetBounds(x, 5, 30, 20); x += 35;
        m_pStop->SetBounds(x, 5, 30, 20); x += 35;
        m_pHome->SetBounds(x, 5, 50, 20); x += 55;

        m_pAddress->SetBounds(x, 5, w - x - 70, 20);
        m_pGo->SetBounds(w - 60, 5, 55, 20);

        m_pBrowser->SetBounds(0, top, w, h - top - bottom);

        m_pStatus->SetBounds(5, h - bottom + 3, w - 10, 20);
    }

    virtual void OnCommand(const char* cmd) override
    {
        if (!Q_stricmp(cmd, "Back"))
            m_pBrowser->GetWebView()->GoBack();
        else if (!Q_stricmp(cmd, "Forward"))
            m_pBrowser->GetWebView()->GoForward();
        else if (!Q_stricmp(cmd, "Refresh"))
            m_pBrowser->GetWebView()->Reload(false);
        else if (!Q_stricmp(cmd, "Stop"))
            m_pBrowser->GetWebView()->Stop();
        else if (!Q_stricmp(cmd, "Home"))
            Navigate("http://example.com");
        else if (!Q_stricmp(cmd, "Go"))
            NavigateFromBar();
        else
            BaseClass::OnCommand(cmd);
    }

    virtual void OnKeyCodePressed(KeyCode code) override
    {
        if (code == KEY_ENTER)
        {
            NavigateFromBar();
        }

        BaseClass::OnKeyCodePressed(code);
    }

private:

    void NavigateFromBar()
    {
        char url[512];
        m_pAddress->GetText(url, sizeof(url));
        Navigate(url);
    }

    void Navigate(const char* url)
    {
        if (!m_pBrowser)
            return;

        char finalURL[512];

        // Auto add http://
        if (!Q_stristr(url, "http://") && !Q_stristr(url, "https://"))
            Q_snprintf(finalURL, sizeof(finalURL), "http://%s", url);
        else
            Q_strncpy(finalURL, url, sizeof(finalURL));

        m_pStatus->SetText("Loading...");
        m_bLoading = true;

        m_pBrowser->OpenURL(finalURL);
    }

private:

    VAwesomium* m_pBrowser;

    Button* m_pBack;
    Button* m_pForward;
    Button* m_pRefresh;
    Button* m_pStop;
    Button* m_pHome;
    Button* m_pGo;

    TextEntry* m_pAddress;
    Label* m_pStatus;

    bool m_bLoading;
};

// ----------------------------------------------------
// Global Instance
// ----------------------------------------------------

static CAwesomiumBrowser* g_pBrowser = NULL;

static Panel* GetRootPanel()
{
    VPANEL root = enginevgui->GetPanel(PANEL_GAMEUIDLL);
    return ipanel()->GetPanel(root, "CLIENTDLL");
}

CON_COMMAND(awesomium_browser, "Open advanced Awesomium browser")
{
    if (!g_pBrowser)
        g_pBrowser = new CAwesomiumBrowser(GetRootPanel());

    g_pBrowser->SetVisible(true);
    g_pBrowser->MakePopup();
    g_pBrowser->MoveToFront();
}

#endif
// gabeplus_friendslist.cpp
// 2007-compatible friends list panel with Steam + fake friend support

#include "cbase.h"

#include <vgui/IVGui.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>

#include <vgui_controls/Frame.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImageList.h>

#include "ienginevgui.h"

#include "steam/steam_api.h"
#include "steam/isteamfriends.h"
#include "steam/isteamutils.h"

using namespace vgui;

//------------------------------------------------------------
// Custom IImage wrapper for Steam avatar
//------------------------------------------------------------
class CSteamAvatarImage : public vgui::IImage
{
public:
    CSteamAvatarImage(int texID, int w, int h)
        : m_iTexture(texID),
        m_iWide(w),
        m_iTall(h),
        m_iX(0),
        m_iY(0),
        m_Color(255, 255, 255, 255)
    {
    }

    virtual void Paint()
    {
        surface()->DrawSetColor(m_Color);
        surface()->DrawSetTexture(m_iTexture);
        surface()->DrawTexturedRect(
            m_iX,
            m_iY,
            m_iX + m_iWide,
            m_iY + m_iTall
        );
    }

    virtual void SetPos(int x, int y) { m_iX = x; m_iY = y; }

    virtual void GetContentSize(int& w, int& h)
    {
        w = m_iWide;
        h = m_iTall;
    }

    virtual void GetSize(int& w, int& h)
    {
        w = m_iWide;
        h = m_iTall;
    }

    virtual void SetSize(int w, int h)
    {
        m_iWide = w;
        m_iTall = h;
    }

    virtual void SetColor(Color col)
    {
        m_Color = col;
    }

private:
    int   m_iTexture;
    int   m_iWide;
    int   m_iTall;
    int   m_iX;
    int   m_iY;
    Color m_Color;
};

//------------------------------------------------------------
// Friends Panel
//------------------------------------------------------------

class CGabePlusFriendsPanel : public Frame
{
    DECLARE_CLASS_SIMPLE(CGabePlusFriendsPanel, Frame);

public:

    CGabePlusFriendsPanel(VPANEL parent)
        : BaseClass(NULL, "GabePlusFriendsPanel")
    {
        SetParent(parent);

        SetTitle("Friends", true);
        SetSize(600, 560);
        SetMoveable(true);
        SetSizeable(false);
        SetCloseButtonVisible(true);
        SetDeleteSelfOnClose(false);

        m_pList = new ListPanel(this, "FriendsList");

        m_pList->AddColumnHeader(
            0, "icon", "", 96,
            ListPanel::COLUMN_IMAGE
        );

        m_pList->AddColumnHeader(
            1, "name", "Friend", 320,
            ListPanel::COLUMN_RESIZEWITHWINDOW
        );

        m_pList->AddColumnHeader(
            2, "status", "Online", 100,
            ListPanel::COLUMN_FIXEDSIZE
        );

        m_pList->SetRowHeight(96);
        m_pList->SetMultiselectEnabled(false);
        m_pList->SetEmptyListText("Steam not available.");

        m_pImageList = new ImageList(false);
        m_pList->SetImageList(m_pImageList, false);

        m_pOverlay = new Button(
            this,
            "OverlayButton",
            "Steam Friends",
            this,
            "OpenOverlay"
        );

        m_pRefresh = new Button(
            this,
            "RefreshButton",
            "Refresh",
            this,
            "Refresh"
        );

        m_pClose = new Button(
            this,
            "CloseButton",
            "Close",
            this,
            "ClosePanel"
        );

        PopulateFriends();
        CenterOnScreen();
    }

    virtual ~CGabePlusFriendsPanel()
    {
        if (m_pImageList)
            delete m_pImageList;
    }

    //--------------------------------------------------------

    struct FakeFriend_t
    {
        CUtlString name;
        bool online;
    };

    CUtlVector<FakeFriend_t> m_FakeFriends;

    void AddFakeFriend(const char* name, bool online)
    {
        FakeFriend_t ff;
        ff.name = name;
        ff.online = online;

        m_FakeFriends.AddToTail(ff);
        PopulateFriends();
    }

    //--------------------------------------------------------

    virtual void PerformLayout()
    {
        BaseClass::PerformLayout();

        int w, h;
        GetSize(w, h);

        const int pad = 12;
        const int btnH = 26;
        const int btnW = 130;
        const int gap = 8;

        int y = h - pad - btnH;

        int x = pad;

        m_pOverlay->SetSize(btnW, btnH);
        m_pOverlay->SetPos(x, y);
        x += btnW + gap;

        m_pRefresh->SetSize(btnW, btnH);
        m_pRefresh->SetPos(x, y);

        m_pClose->SetSize(btnW, btnH);
        m_pClose->SetPos(w - pad - btnW, y);

        m_pList->SetPos(pad, pad + 28);
        m_pList->SetSize(
            w - pad * 2,
            h - (pad * 3) - btnH - 28
        );
    }

    //--------------------------------------------------------

    virtual void OnCommand(const char* cmd)
    {
        if (!Q_stricmp(cmd, "Refresh"))
        {
            PopulateFriends();
            return;
        }

        if (!Q_stricmp(cmd, "OpenOverlay"))
        {
            if (steamapicontext && steamapicontext->SteamFriends())
                steamapicontext->SteamFriends()->ActivateGameOverlay("Friends");
            return;
        }

        if (!Q_stricmp(cmd, "ClosePanel"))
        {
            SetVisible(false);
            return;
        }

        BaseClass::OnCommand(cmd);
    }

private:

    void RebuildImageList()
    {
        if (m_pImageList)
            delete m_pImageList;

        m_pImageList = new ImageList(false);
        m_pList->SetImageList(m_pImageList, false);
    }

    //--------------------------------------------------------

    void PopulateFriends()
    {
        m_pList->DeleteAllItems();
        RebuildImageList();

        // Steam friends
        if (steamapicontext && steamapicontext->SteamFriends())
        {
            ISteamFriends* pFriends = steamapicontext->SteamFriends();
            ISteamUtils* pUtils = steamapicontext->SteamUtils();

            int count = pFriends->GetFriendCount(k_EFriendFlagImmediate);

            for (int i = 0; i < count; ++i)
            {
                CSteamID id =
                    pFriends->GetFriendByIndex(i, k_EFriendFlagImmediate);

                const char* name =
                    pFriends->GetFriendPersonaName(id);

                EPersonaState state =
                    pFriends->GetFriendPersonaState(id);

                bool online =
                    (state == k_EPersonaStateOnline ||
                        state == k_EPersonaStateAway ||
                        state == k_EPersonaStateBusy ||
                        state == k_EPersonaStateSnooze);

                int imageIndex = -1;

                int avatar = pFriends->GetFriendAvatar(id);

                if (avatar != -1 && pUtils)
                {
                    uint32 w = 0, h = 0;

                    if (pUtils->GetImageSize(avatar, &w, &h))
                    {
                        const int imageSize = w * h * 4;
                        unsigned char* rgba = new unsigned char[imageSize];

                        if (pUtils->GetImageRGBA(avatar, rgba, imageSize))
                        {
                            int texID = surface()->CreateNewTextureID(true);

                            surface()->DrawSetTextureRGBA(
                                texID,
                                rgba,
                                w,
                                h,
                                1,
                                true
                            );

                            CSteamAvatarImage* img =
                                new CSteamAvatarImage(texID, w, h);

                            imageIndex = m_pImageList->AddImage(img);

                            if (imageIndex != -1)
                                img->SetSize(88, 88);
                        }

                        delete[] rgba;
                    }
                }

                KeyValues* kv = new KeyValues("item");
                kv->SetInt("icon", imageIndex);
                kv->SetString("name", name);
                kv->SetString("status", online ? "Yes" : "No");

                m_pList->AddItem(kv, 0, false, false);
                kv->deleteThis();
            }
        }

        // Fake injected friends
// Fake injected friends (with random avatars)
        for (int i = 0; i < m_FakeFriends.Count(); ++i)
        {
            const FakeFriend_t& ff = m_FakeFriends[i];

            int imageIndex = -1;

            // Generate 32x32 random color avatar
            const int w = 32;
            const int h = 32;
            const int imageSize = w * h * 4;

            unsigned char* rgba = new unsigned char[imageSize];

            unsigned char r = rand() % 256;
            unsigned char g = rand() % 256;
            unsigned char b = rand() % 256;

            for (int p = 0; p < w * h; ++p)
            {
                rgba[p * 4 + 0] = r;
                rgba[p * 4 + 1] = g;
                rgba[p * 4 + 2] = b;
                rgba[p * 4 + 3] = 255;
            }

            int texID = surface()->CreateNewTextureID(true);

            surface()->DrawSetTextureRGBA(
                texID,
                rgba,
                w,
                h,
                1,
                true
            );

            CSteamAvatarImage* img =
                new CSteamAvatarImage(texID, w, h);

            imageIndex = m_pImageList->AddImage(img);

            if (imageIndex != -1)
                img->SetSize(88, 88);

            delete[] rgba;

            KeyValues* kv = new KeyValues("item");
            kv->SetInt("icon", imageIndex);
            kv->SetString("name", ff.name.String());
            kv->SetString("status", ff.online ? "Yes" : "No");

            m_pList->AddItem(kv, 0, false, false);
            kv->deleteThis();
        }
    }

    //--------------------------------------------------------

    void CenterOnScreen()
    {
        int sw, sh;
        surface()->GetScreenSize(sw, sh);

        int w, h;
        GetSize(w, h);

        SetPos((sw - w) / 2, (sh - h) / 2);
    }

private:
    ListPanel* m_pList;
    ImageList* m_pImageList;

    Button* m_pRefresh;
    Button* m_pOverlay;
    Button* m_pClose;
};

//------------------------------------------------------------
// Console Commands
//------------------------------------------------------------

CGabePlusFriendsPanel* g_pFriends = NULL;

CON_COMMAND_F(
    gabeplus_friends,
    "Open friends list",
    FCVAR_CLIENTDLL
)
{
    VPANEL root = enginevgui->GetPanel(PANEL_GAMEUIDLL);
    if (!root)
        return;

    if (!g_pFriends)
        g_pFriends = new CGabePlusFriendsPanel(root);

    g_pFriends->SetVisible(true);
    g_pFriends->MakePopup();
    g_pFriends->MoveToFront();
    g_pFriends->RequestFocus();
}

CON_COMMAND_F(
    gabeplus_newfriend,
    "Create fake friend: gabeplus_newfriend <name> <0/1>",
    FCVAR_CLIENTDLL
)
{
    if (!g_pFriends)
    {
        Msg("Open the friends panel first.\n");
        return;
    }

    if (args.ArgC() < 2)
    {
        Msg("Usage: gabeplus_newfriend <name> <online 0/1>\n");
        return;
    }

    const char* name = args[1];
    bool online = true;

    if (args.ArgC() >= 3)
        online = (atoi(args[2]) != 0);

    g_pFriends->AddFakeFriend(name, online);

    Msg("Added fake friend: %s\n", name);
}
#include "cbase.h"
#include <vgui_controls/Frame.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Label.h>
#include <filesystem.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>

using namespace vgui;

class CGabeWeaponCreator : public Frame
{
    DECLARE_CLASS_SIMPLE(CGabeWeaponCreator, Frame);

public:

    CGabeWeaponCreator(Panel* parent) : Frame(parent, "WeaponCreator")
    {
        SetSize(520, 500);
        SetTitle("GabeMod Weapon Creator", true);
        SetVisible(true);
        SetDeleteSelfOnClose(false);

        int sw, sh;
        surface()->GetScreenSize(sw, sh);
        SetPos((sw - 520) / 2, (sh - 500) / 2);

        int y = 40;

        m_pClass = CreateField("Class Name:", y); y += 35;
        m_pPrint = CreateField("Print Name:", y); y += 35;
        m_pViewModel = CreateField("View Model:", y); y += 35;
        m_pWorldModel = CreateField("World Model:", y); y += 35;
        m_pDamage = CreateField("Damage:", y); y += 35;
        m_pClip = CreateField("Clip Size:", y); y += 35;
        m_pDelay = CreateField("Fire Delay:", y); y += 45;

        m_pAuto = new CheckButton(this, "AutoCheck", "Automatic");
        m_pAuto->SetPos(20, y);
        y += 45;

        m_pExport = new Button(this, "ExportBtn", "Export Weapon");
        m_pExport->SetPos(20, y);
        m_pExport->SetSize(180, 30);
        m_pExport->SetCommand("export_weapon");
        m_pExport->AddActionSignalTarget(this);
    }

    virtual void OnCommand(const char* command)
    {
        if (!Q_stricmp(command, "export_weapon"))
        {
            ExportWeapon();
            return;
        }

        BaseClass::OnCommand(command);
    }

private:

    TextEntry* m_pClass;
    TextEntry* m_pPrint;
    TextEntry* m_pViewModel;
    TextEntry* m_pWorldModel;
    TextEntry* m_pDamage;
    TextEntry* m_pClip;
    TextEntry* m_pDelay;
    CheckButton* m_pAuto;
    Button* m_pExport;

    TextEntry* CreateField(const char* label, int y)
    {
        Label* lbl = new Label(this, "", label);
        lbl->SetPos(20, y);
        lbl->SetSize(120, 20);

        TextEntry* entry = new TextEntry(this, "");
        entry->SetPos(150, y);
        entry->SetSize(320, 20);
        entry->SetText("");

        return entry;
    }

    void GetTextANSI(TextEntry* entry, char* buffer, int size)
    {
        wchar_t wbuf[512];
        entry->GetText(wbuf, sizeof(wbuf));
        g_pVGuiLocalize->ConvertUnicodeToANSI(wbuf, buffer, size);
    }

    void ExportWeapon()
    {
        char className[128];
        char printName[128];
        char viewModel[256];
        char worldModel[256];
        char damage[32];
        char clip[32];
        char delay[32];

        GetTextANSI(m_pClass, className, sizeof(className));
        GetTextANSI(m_pPrint, printName, sizeof(printName));
        GetTextANSI(m_pViewModel, viewModel, sizeof(viewModel));
        GetTextANSI(m_pWorldModel, worldModel, sizeof(worldModel));
        GetTextANSI(m_pDamage, damage, sizeof(damage));
        GetTextANSI(m_pClip, clip, sizeof(clip));
        GetTextANSI(m_pDelay, delay, sizeof(delay));

        bool automatic = m_pAuto->IsSelected();

        char filename[256];
        Q_snprintf(filename, sizeof(filename), "weapon_%s.cpp", className);

        FileHandle_t file = filesystem->Open(filename, "wb", "MOD");
        if (!file)
        {
            Warning("Failed to create weapon file.\n");
            return;
        }

        char output[8192];

        Q_snprintf(output, sizeof(output),
            "#include \"cbase.h\"\n"
            "#include \"weapon_hl2mpbasehlmpcombatweapon.h\"\n"
            "\n"
            "class CWeapon%s : public CHL2MPBaseHLMPCombatWeapon\n"
            "{\n"
            "public:\n"
            "    DECLARE_CLASS( CWeapon%s, CHL2MPBaseHLMPCombatWeapon );\n"
            "    DECLARE_NETWORKCLASS();\n"
            "    DECLARE_PREDICTABLE();\n"
            "\n"
            "    CWeapon%s() {}\n"
            "\n"
            "    void PrimaryAttack()\n"
            "    {\n"
            "        if ( m_iClip1 <= 0 )\n"
            "            return;\n"
            "\n"
            "        m_iClip1--;\n"
            "\n"
            "        CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );\n"
            "        if ( !pPlayer ) return;\n"
            "\n"
            "        Vector vecSrc = pPlayer->Weapon_ShootPosition();\n"
            "        Vector vecDir = pPlayer->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );\n"
            "\n"
            "        FireBulletsInfo_t info;\n"
            "        info.m_iShots = 1;\n"
            "        info.m_vecSrc = vecSrc;\n"
            "        info.m_vecDirShooting = vecDir;\n"
            "        info.m_flDistance = MAX_TRACE_LENGTH;\n"
            "        info.m_iDamage = %s;\n"
            "\n"
            "        pPlayer->FireBullets( info );\n"
            "\n"
            "        m_flNextPrimaryAttack = gpGlobals->curtime + %s;\n"
            "    }\n"
            "};\n"
            "\n"
            "IMPLEMENT_NETWORKCLASS_ALIASED( Weapon%s, DT_Weapon%s )\n"
            "BEGIN_NETWORK_TABLE( CWeapon%s, DT_Weapon%s )\n"
            "END_NETWORK_TABLE()\n"
            "\n"
            "BEGIN_PREDICTION_DATA( CWeapon%s )\n"
            "END_PREDICTION_DATA()\n"
            "\n"
            "LINK_ENTITY_TO_CLASS( weapon_%s, CWeapon%s );\n"
            "\n"
            "PRECACHE_WEAPON_REGISTER( weapon_%s );\n"
            ,
            className,
            className,
            className,
            damage,
            delay,
            className,
            className,
            className,
            className,
            className,
            className,
            className
        );

        filesystem->Write(output, Q_strlen(output), file);
        filesystem->Close(file);

        Msg("Weapon exported: %s\n", filename);
    }
};

static CGabeWeaponCreator* g_pWeaponCreator = NULL;

CON_COMMAND(gabe_weaponcreator, "Open the Weapon Creator")
{
    if (!g_pWeaponCreator)
        g_pWeaponCreator = new CGabeWeaponCreator(NULL);

    g_pWeaponCreator->SetVisible(true);
    g_pWeaponCreator->MoveToFront();
}

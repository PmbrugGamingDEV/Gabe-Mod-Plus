#include "cbase.h"
#include "vgui_keyvalue.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/ListPanel.h"
#include "vgui_controls/TextEntry.h"
#include "filesystem.h"
#include "ienginevgui.h"

void RecursiveSearch(const char* path, CUtlVector< CUtlString >& results)
{
    char searchPath[512];
    Q_snprintf(searchPath, sizeof(searchPath), "%s/*", path);

    FileFindHandle_t findHandle;
    const char* filename = filesystem->FindFirstEx(searchPath, "MOD", &findHandle);

    if (!filename)
        return;

    while (filename)
    {
        if (!Q_stricmp(filename, ".") || !Q_stricmp(filename, ".."))
        {
            filename = filesystem->FindNext(findHandle);
            continue;
        }

        char fullPath[512];
        Q_snprintf(fullPath, sizeof(fullPath), "%s/%s", path, filename);

        if (filesystem->FindIsDirectory(findHandle))
        {
            RecursiveSearch(fullPath, results);
        }
        else
        {
            const char* ext = Q_strrchr(filename, '.');
            if (ext && (!Q_stricmp(ext, ".txt") || !Q_stricmp(ext, ".res")))
            {
                results.AddToTail(fullPath);
            }
        }

        filename = filesystem->FindNext(findHandle);
    }

    filesystem->FindClose(findHandle);
}

CGBKVEditor::CGBKVEditor(VPANEL parent)
    : BaseClass(NULL, "GBKVEditor")
{
    SetParent(parent);
    SetSize(800, 600);
    SetTitle("KeyValues Editor", true);
    SetVisible(true);
    SetSizeable(false);
    SetMinimizeButtonVisible(true);
    SetCloseButtonVisible(true);

    m_pFileList = new ListPanel(this, "FileList");
    m_pFileList->SetPos(10, 30);
    m_pFileList->SetSize(200, 520);
    m_pFileList->AddColumnHeader(0, "filename", "Filename", 150);

    m_pEditor = new TextEntry(this, "Editor");
    m_pEditor->SetMultiline(true);
    m_pEditor->SetPos(220, 30);
    m_pEditor->SetSize(560, 520);

    Button* OpenBtn = new Button(this, "OpenBtn", "Open", this, "open");
    OpenBtn->SetPos(10, 560);
    Button* SaveBtn = new Button(this, "SaveBtn", "Save", this, "save"); 
    SaveBtn->SetPos(90, 560);
    Button* DeleteBtn = new Button(this, "DeleteBtn", "Delete", this, "delete"); 
    DeleteBtn->SetPos(170, 560);

    RefreshFileList();
}

CGBKVEditor::~CGBKVEditor()
{
}

void CGBKVEditor::RefreshFileList()
{
    m_pFileList->RemoveAll();

    CUtlVector< CUtlString > files;
    RecursiveSearch(".", files);

    for (int i = 0; i < files.Count(); i++)
    {
        KeyValues* kv = new KeyValues("row");
        kv->SetString("filename", files[i]);
        m_pFileList->AddItem(kv, i, false, false);
    }
}

void CGBKVEditor::LoadFile(const char* filename)
{
    FileHandle_t file = filesystem->Open(filename, "r", "MOD");
    if (!file)
        return;

    int size = filesystem->Size(file);
    char* buffer = new char[size + 1];

    filesystem->Read(buffer, size, file);
    buffer[size] = 0;

    m_pEditor->SetText(buffer);

    delete[] buffer;
    filesystem->Close(file);
}

void CGBKVEditor::SaveFile(const char* filename)
{
    FileHandle_t file = filesystem->Open(filename, "w", "MOD");
    if (!file)
        return;

    char buffer[8192];
    m_pEditor->GetText(buffer, sizeof(buffer));

    filesystem->Write(buffer, Q_strlen(buffer), file);
    filesystem->Close(file);

    RefreshFileList();
}

void CGBKVEditor::DeleteFile(const char* filename)
{
    filesystem->RemoveFile(filename, "MOD");
    RefreshFileList();
}

void CGBKVEditor::OnCommand(const char* command)
{
    if (!Q_stricmp(command, "open"))
    {
        int selected = m_pFileList->GetSelectedItem(0);
        if (selected >= 0)
        {
            KeyValues* kv = m_pFileList->GetItem(selected);
            LoadFile(kv->GetString("filename"));
        }
    }
    else if (!Q_stricmp(command, "save"))
    {
        int selected = m_pFileList->GetSelectedItem(0);
        if (selected >= 0)
        {
            KeyValues* kv = m_pFileList->GetItem(selected);
            SaveFile(kv->GetString("filename"));
		}
    }
    else if (!Q_stricmp(command, "delete"))
    {
        int selected = m_pFileList->GetSelectedItem(0);
        if (selected >= 0)
        {
            KeyValues* kv = m_pFileList->GetItem(selected);
            DeleteFile(kv->GetString("filename"));
        }
    }
    else
    {
        BaseClass::OnCommand(command);
    }
}

static CGBKVEditor* g_pKVEditor = NULL;

CON_COMMAND(gabe_kveditor, "Open Gabe KV Editor")
{
    if (!g_pKVEditor)
    {
        VPANEL gameUI = enginevgui->GetPanel(PANEL_GAMEUIDLL);
        g_pKVEditor = new CGBKVEditor(gameUI);
    }

    g_pKVEditor->SetVisible(true);
    g_pKVEditor->MoveToFront();
}

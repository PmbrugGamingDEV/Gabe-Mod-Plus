#ifndef VGUI_GBKVEDITOR_H
#define VGUI_GBKVEDITOR_H

#include "vgui_controls/Frame.h"

using namespace vgui;

class CGBKVEditor : public Frame
{
    DECLARE_CLASS_SIMPLE(CGBKVEditor, Frame);

public:
    CGBKVEditor(VPANEL parent);
    ~CGBKVEditor();

    virtual void OnCommand(const char* command);

    void RefreshFileList();
    void LoadFile(const char* filename);
    void SaveFile(const char* filename);
    void DeleteFile(const char* filename);

private:

    ListPanel* m_pFileList;
    TextEntry* m_pEditor;
};

#endif

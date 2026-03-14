#include "cbase.h"
#include "../common/gameui/igameui.h"
#include "interface.h"

static IGameUI* g_pGameUI = NULL;

void CC_NewGameDialog()
{
    if (g_pGameUI)
    {
        g_pGameUI->ShowNewGameDialog(0);
    }
}

ConCommand show_newgame_dialog("newgamedialog", CC_NewGameDialog);

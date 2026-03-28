#include "cbase.h"
#include "vguicenterprint.h"
#include "tier1/strtools.h"
#include <ctype.h>
#include "c_baseplayer.h"

extern CCenterPrint* internalCenterPrint;

//-----------------------------------------------------------------------------
// hud_message <type> <text>
// hud_message <type> <r> <g> <b> <text>
//
// Types:
// 0 = center
// 1 = console
// 2 = notify (console fallback)
// 3 = talk (chat)
//-----------------------------------------------------------------------------

void CC_HudMessage(const CCommand& args)
{
	if (args.ArgC() < 3)
	{
		Msg("Usage:\n");
		Msg(" gabe_message <type> <text>\n");
		Msg(" gabe_message <type> <r> <g> <b> <text>\n");
		Msg("Types: 0=center, 1=console, 2=notify, 3=talk\n");
		return;
	}

	int type = atoi(args.Arg(1));

	// -----------------------------------------
	// Optional color (center only)
	// -----------------------------------------
	bool bUseColor = false;
	int r = 255, g = 255, b = 255;

	int argIndex = 2;

	if (args.ArgC() > 4 && isdigit(args.Arg(2)[0]))
	{
		r = atoi(args.Arg(2));
		g = atoi(args.Arg(3));
		b = atoi(args.Arg(4));

		bUseColor = true;
		argIndex = 5;
	}

	// -----------------------------------------
	// Build message safely
	// -----------------------------------------
	char msg[512];
	msg[0] = '\0';

	for (int i = argIndex; i < args.ArgC(); i++)
	{
		Q_strncat(msg, args.Arg(i), sizeof(msg), COPY_ALL_CHARACTERS);

		if (i < args.ArgC() - 1)
			Q_strncat(msg, " ", sizeof(msg), COPY_ALL_CHARACTERS);
	}

	// -----------------------------------------
	// Get local player (needed for ClientPrint)
	// -----------------------------------------
	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();

	// -----------------------------------------
	// Output
	// -----------------------------------------
	switch (type)
	{
	case 0: // CENTER
	{
		if (internalCenterPrint)
		{
			if (bUseColor)
				internalCenterPrint->ColorPrint(r, g, b, 255, msg);
			else
				internalCenterPrint->Print(msg);
		}
	}
	break;

	case 1: // CONSOLE
	{
		Msg("%s\n", msg);
	}
	break;

	case 2: // NOTIFY (fallback)
	{
		Msg("[NOTIFY] %s\n", msg);
	}
	break;

	case 3: // TALK (REAL CHAT)
	{
		if (pPlayer)
		{
			ClientPrint(pPlayer, HUD_PRINTTALK, msg);
		}
		else
		{
			Msg("[CHAT] %s\n", msg);
		}
	}
	break;

	default:
	{
		Msg("Invalid type.\n");
	}
	break;
	}
}

//-----------------------------------------------------------------------------
// ConCommand (CLIENT)
//-----------------------------------------------------------------------------
static ConCommand hud_message(
	"gabe_message",
	CC_HudMessage,
	"Client HUD message system\n"
	"Usage:\n"
	" gabe_message <type> <text>\n"
	" gabe_message <type> <r> <g> <b> <text>\n"
	"Types: 0=center, 1=console, 2=notify, 3=talk",
	FCVAR_CLIENTDLL
);
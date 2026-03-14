#include "cbase.h"
#include "tier0/dbg.h"
#include "tier1/strtools.h"
#include "tier1/utlvector.h"
#include "tier1/utlstring.h"
#include "Color.h"

#include "tier0/memdbgon.h"

//---------------------------------------------------------
// Helper: build message string from args
//---------------------------------------------------------

static const char* BuildMsgString(const CCommand& args, int start)
{
	static char buffer[4096];
	buffer[0] = 0;

	for (int i = start; i < args.ArgC(); i++)
	{
		Q_strncat(buffer, args[i], sizeof(buffer), COPY_ALL_CHARACTERS);

		if (i != args.ArgC() - 1)
			Q_strncat(buffer, " ", sizeof(buffer), COPY_ALL_CHARACTERS);
	}

	return buffer;
}


//=========================================================
// BASIC SPEW
//=========================================================

CON_COMMAND(print_msg, "tier0 Msg")
{
	if (args.ArgC() < 2)
	{
		Msg("Usage: print_msg <text>\n");
		return;
	}

	Msg("%s\n", BuildMsgString(args, 1));
}

CON_COMMAND(print_warning, "tier0 Warning")
{
	if (args.ArgC() < 2)
	{
		Msg("Usage: print_warning <text>\n");
		return;
	}

	Warning("%s\n", BuildMsgString(args, 1));
}

CON_COMMAND(print_log, "tier0 Log")
{
	if (args.ArgC() < 2)
	{
		Msg("Usage: print_log <text>\n");
		return;
	}

	Log("%s\n", BuildMsgString(args, 1));
}

CON_COMMAND(print_error, "tier0 Error (fatal)")
{
	if (args.ArgC() < 2)
	{
		Msg("Usage: print_error <text>\n");
		return;
	}

	Error("%s\n", BuildMsgString(args, 1));
}


//=========================================================
// GROUP SPEW
//=========================================================

CON_COMMAND(print_dmsg, "tier0 DMsg <group> <level> <text>")
{
	if (args.ArgC() < 4)
	{
		Msg("Usage: print_dmsg <group> <level> <text>\n");
		return;
	}

	const char* group = args[1];
	int level = atoi(args[2]);

	DMsg(group, level, "%s\n", BuildMsgString(args, 3));
}

CON_COMMAND(print_dwarning, "tier0 DWarning <group> <level> <text>")
{
	if (args.ArgC() < 4)
	{
		Msg("Usage: print_dwarning <group> <level> <text>\n");
		return;
	}

	const char* group = args[1];
	int level = atoi(args[2]);

	DWarning(group, level, "%s\n", BuildMsgString(args, 3));
}

CON_COMMAND(print_dlog, "tier0 DLog <group> <level> <text>")
{
	if (args.ArgC() < 4)
	{
		Msg("Usage: print_dlog <group> <level> <text>\n");
		return;
	}

	const char* group = args[1];
	int level = atoi(args[2]);

	DLog(group, level, "%s\n", BuildMsgString(args, 3));
}


//=========================================================
// DEVELOPER SPEW
//=========================================================

CON_COMMAND(print_devmsg, "tier0 DevMsg <level> <text>")
{
	if (args.ArgC() < 3)
	{
		Msg("Usage: print_devmsg <level> <text>\n");
		return;
	}

	int level = atoi(args[1]);

	DevMsg(level, "%s\n", BuildMsgString(args, 2));
}

CON_COMMAND(print_devwarning, "tier0 DevWarning <level> <text>")
{
	if (args.ArgC() < 3)
	{
		Msg("Usage: print_devwarning <level> <text>\n");
		return;
	}

	int level = atoi(args[1]);

	DevWarning(level, "%s\n", BuildMsgString(args, 2));
}

CON_COMMAND(print_devlog, "tier0 DevLog <level> <text>")
{
	if (args.ArgC() < 3)
	{
		Msg("Usage: print_devlog <level> <text>\n");
		return;
	}

	int level = atoi(args[1]);

	DevLog(level, "%s\n", BuildMsgString(args, 2));
}


//=========================================================
// CONSOLE SPEW
//=========================================================

CON_COMMAND(print_conmsg, "tier0 ConMsg <level> <text>")
{
	if (args.ArgC() < 3)
	{
		Msg("Usage: print_conmsg <level> <text>\n");
		return;
	}

	int level = atoi(args[1]);

	ConMsg(level, "%s\n", BuildMsgString(args, 2));
}

CON_COMMAND(print_conwarning, "tier0 ConWarning <level> <text>")
{
	if (args.ArgC() < 3)
	{
		Msg("Usage: print_conwarning <level> <text>\n");
		return;
	}

	int level = atoi(args[1]);

	ConWarning(level, "%s\n", BuildMsgString(args, 2));
}

CON_COMMAND(print_conlog, "tier0 ConLog <level> <text>")
{
	if (args.ArgC() < 3)
	{
		Msg("Usage: print_conlog <level> <text>\n");
		return;
	}

	int level = atoi(args[1]);

	ConLog(level, "%s\n", BuildMsgString(args, 2));
}


//=========================================================
// CONSOLE COLOR
//=========================================================

CON_COMMAND(print_concolormsg, "tier0 ConColorMsg <r> <g> <b> <text>")
{
	if (args.ArgC() < 5)
	{
		Msg("Usage: print_concolormsg <r> <g> <b> <text>\n");
		return;
	}

	int r = atoi(args[1]);
	int g = atoi(args[2]);
	int b = atoi(args[3]);

	Color clr(r, g, b, 255);

	ConColorMsg(1, clr, "%s\n", BuildMsgString(args, 4));
}


//=========================================================
// NETWORK SPEW
//=========================================================

CON_COMMAND(print_netmsg, "tier0 NetMsg <level> <text>")
{
	if (args.ArgC() < 3)
	{
		Msg("Usage: print_netmsg <level> <text>\n");
		return;
	}

	int level = atoi(args[1]);

	NetMsg(level, "%s\n", BuildMsgString(args, 2));
}

CON_COMMAND(print_netwarning, "tier0 NetWarning <level> <text>")
{
	if (args.ArgC() < 3)
	{
		Msg("Usage: print_netwarning <level> <text>\n");
		return;
	}

	int level = atoi(args[1]);

	NetWarning(level, "%s\n", BuildMsgString(args, 2));
}

CON_COMMAND(print_netlog, "tier0 NetLog <level> <text>")
{
	if (args.ArgC() < 3)
	{
		Msg("Usage: print_netlog <level> <text>\n");
		return;
	}

	int level = atoi(args[1]);

	NetLog(level, "%s\n", BuildMsgString(args, 2));
}
// Made with XBLAH's Modding tool.
// Download it at https://xblah.dev/modding-tool/

"gamemenu"
{
	"0"
	{
		"label"	"Resume Game"
		"command"	"ResumeGame"
		"ingameorder"	"0"
		"onlyingame"	"1"
	}
	"1"
	{
		"label"	"Disconnect"
		"command"	"Disconnect"
		"ingameorder"	"1"
		"onlyingame"	"1"
	}
	"2"
	{
		"label"	"Mute Players"
		"command"	"OpenPlayerListDialog"
		"ingameorder"	"2"
		"onlyingame"	"1"
	}
	"3"
	{
		"label"	""
		"command"	""
		"ingameorder"	"3"
		"onlyingame"	"1"
	}
	"4"
	{
		"label"	"New Game"
		"command"	"engine gabeplus_newgame"
		"ingameorder"	"4"
	}
		"6"
	{
		"label" "Load Game"
		"command" "OpenLoadGameDialog"
		"notmulti" "1"
	}
	"7"
	{
		"label" "Save Game"
		"command" "OpenSaveGameDialog"
		"notmulti" "1"
		"OnlyInGame" "1"
	}
	"5"
	{
		"label"	"Online Servers"
		"command"	"OpenServerBrowser"
		"ingameorder"	"5"
	}
	"5_5"
	{
		"label" "Stress Test"
		"command" "OpenBenchmarkDialog"
		"ingameorder"	"6"
	}
	"6"
	{
		"label"	"Friends"
		"command"	"engine gabeplus_friends"
		"ingameorder"	"7"
	}
	"7"
	{
		"label"	"Toggle Thanks"
		"command"	"engine gabeplus_thanks"
		"ingameorder"	"8"
	}
	"8"
	{
		"label"	"Tutorials"
		"command"	"engine gabeplus_tutorial"
		"ingameorder"	"9"
	}
	"9"
	{
		"label"	"Changelog"
		"command"	"engine gabeplus_chlog"
		"ingameorder"	"10"
	}
	"10"
	{
		"label"	"Achievements"
		"command"	"OpenAchievementsDialog"
		"ingameorder"	"11"
	}
	"11"
	{
		"label"	"Options"
		"command"	"OpenOptionsDialog"
		"ingameorder"	"12"
	}
	"12"
	{
		"label"	"Quit"
		"command"	"Quit"
		"ingameorder"	"13"
	}
}
#pragma once
#include <string>
namespace inject {
	extern bool IsInjected;
	extern bool IsGameOpened;
	extern bool is_game_Downloaded;
	void Download_Game_Hax();
	void Inject_hax();
	bool IsProcessRunning(const std::wstring& processName);
}
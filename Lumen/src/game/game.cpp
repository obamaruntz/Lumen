#include "game.h"
#include <globals.h>
#include <memory/memory.h>

HWND game::get_roblox_window()
{
	if (game::roblox_window && IsWindow(game::roblox_window))
	{
		return game::roblox_window;
	}

	HWND hwnd = FindWindowA(nullptr, "Roblox");
	if (hwnd == nullptr)
	{
		EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			DWORD process_id = 0;
			GetWindowThreadProcessId(hwnd, &process_id);
			if (process_id == memory->m_process_id)
			{
				*reinterpret_cast<HWND*>(lParam) = hwnd;
				return FALSE;
			}
			return TRUE;
		}, reinterpret_cast<LPARAM>(&hwnd));
	}

	if (hwnd)
	{
		game::roblox_window = hwnd;
	}

	return hwnd;
}


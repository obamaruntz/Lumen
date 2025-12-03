#pragma once

namespace rescan
{
	inline bool require_rescan{ false };

	void rescan_game();
	void rescan_process();
}
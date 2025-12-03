#include "rescan.h"

#include <chrono>
#include <thread>

#include <game/game.h>
#include <wallcheck/wallcheck.h>

void rescan::rescan_game()
{
	using namespace std::chrono_literals;

	while (true)
	{
		if (require_rescan)
		{
			game::datamodel = { rbx::c_datamodel::get() };
			game::visualengine = { rbx::c_visualengine::get() };

			wallcheck->cache_workspace();

			require_rescan = false;
		}

		std::string datamodel_name = game::datamodel->get_name();

		if (datamodel_name == "LuaApp" || datamodel_name == "unknown" || datamodel_name == "str_error")
		{
			require_rescan = true;
		}

		std::this_thread::sleep_for(100ms);
	}
}
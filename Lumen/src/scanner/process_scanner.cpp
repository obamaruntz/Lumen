#include "rescan.h"

#include <chrono>
#include <thread>

#include <globals.h>
#include <game/game.h>
#include <logger/logger.h>

void rescan::rescan_process()
{
	using namespace std::chrono_literals;

	while (true)
	{
		DWORD exit_code;

		if (GetExitCodeProcess(memory->m_process_handle, &exit_code))
		{
			if (exit_code != STILL_ACTIVE)
			{
				CloseHandle(memory->m_process_handle);

				std::this_thread::sleep_for(1s);

				while (memory->find_process_id(BINARY_NAME) == 0)
				{
					std::this_thread::sleep_for(100ms);
				}

				if (!memory->attach_to_process(BINARY_NAME))
				{
					ShowWindow(GetConsoleWindow(), SW_SHOW);
					logger->log<ERR>("unable to reattach to the process");
					std::this_thread::sleep_for(5s);
					std::exit(0);
				}

				if (memory->find_module_address(BINARY_NAME) == 0)
				{
					ShowWindow(GetConsoleWindow(), SW_SHOW);
					logger->log<ERR>("unable to rediscover module");
					std::this_thread::sleep_for(5s);
					std::exit(0);
				}

				require_rescan = true;
			}
		}

		std::this_thread::sleep_for(100ms);
	}
}
#include "cache.h"

#include <chrono>
#include <thread>

#include <game/game.h>

void cache::run()
{
	using namespace std::chrono_literals;

	std::vector<cache::entity_t> temp_cache;
	temp_cache.reserve(32);

	while (true)
	{
		temp_cache.clear();

		rbx::c_instance players = game::datamodel->find_first_child_by_class("Players");
		rbx::c_player local_player = memory->read<std::uint64_t>(players.address + Offsets::Player::LocalPlayer);

		game::camera = game::datamodel->get_workspace().find_first_child_by_class("Camera");

		for (rbx::c_player& player : players.get_children<rbx::c_player>())
		{
			rbx::c_model_instance model_instance = player.get_model_instance();
			rbx::c_humanoid humanoid = model_instance.find_first_child("Humanoid");

			entity_t entity
			{
				.instance = player,
				.team = player.get_team(),
				.name = player.get_name(),
				.display_name = player.get_display_name(),
				.health = humanoid.get_health(),
				.max_health = humanoid.get_max_health()
			};

			for (rbx::c_part& part : model_instance.get_children<rbx::c_part>())
			{
				std::string part_name = part.get_name();
				std::string class_name = part.get_class_name();

				if (part_name == "Humanoid")
				{
					entity.humanoid = rbx::c_humanoid(part.address);
				}

				if (part_name == "HumanoidRootPart")
				{
					entity.humanoid_root_part = rbx::c_part(part.address);
				}

				if (class_name.find("Part") != std::string::npos)
				{
					entity.parts[part_name] = part;
				}
			}

			if (player.address == local_player.address)
			{
				cache::local_player = entity;
			}

			temp_cache.push_back(entity);
		}

		{
			std::lock_guard<std::mutex> lock(cache::mtx);
			cache::players = std::move(temp_cache);
		}

		std::this_thread::sleep_for(0.1s);
	}
}
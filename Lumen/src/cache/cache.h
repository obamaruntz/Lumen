#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <unordered_map>

#include <sdk/sdk.h>

namespace cache
{
	inline std::mutex mtx;

	void run();

	struct entity_t final
	{
		rbx::c_instance instance;
		std::uint64_t team;

		std::string name;
		std::string display_name;

		float health;
		float max_health;

		rbx::c_part humanoid_root_part;
		rbx::c_humanoid humanoid;
		std::unordered_map<std::string, rbx::c_part> parts;
	};

	inline entity_t local_player;
	inline std::vector<entity_t> players;
}
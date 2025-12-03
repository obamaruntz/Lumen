#include "esp.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <game/game.h>
#include <cache/cache.h>
#include <sdk/math/math.h>
#include <features/settings.h>
#include "logger/logger.h"
#include <wallcheck/wallcheck.h>

namespace esp
{
	__forceinline void outline(ImVec2& c1, ImVec2& c2, ImU32 col, float rounding = 0.f)
	{
		c1.x = std::round(c1.x); c1.y = std::round(c1.y);
		c2.x = std::round(c2.x); c2.y = std::round(c2.y);

		ImDrawList* draw = ImGui::GetBackgroundDrawList();

		ImRect rect_bb(c1.x, c1.y, c1.x + c2.x, c1.y + c2.y);
		ImVec2 shadow_offset = { cosf(0.f) * 2.f, sinf(0.f) * 2.f };

		draw->AddRect(rect_bb.Min, rect_bb.Max, IM_COL32(0, 0, 0, col >> 24), rounding);
		draw->AddRect({ rect_bb.Min.x - 2.f, rect_bb.Min.y - 2.f }, { rect_bb.Max.x + 2.f, rect_bb.Max.y + 2.f }, IM_COL32(0, 0, 0, col >> 24), rounding);
		draw->AddRect({ rect_bb.Min.x - 1.f, rect_bb.Min.y - 1.f }, { rect_bb.Max.x + 1.f, rect_bb.Max.y + 1.f }, col, rounding);
	}
}

void esp::run()
{
	math::matrix4 view = game::visualengine->get_viewmatrix();
	math::vector2 dims = game::visualengine->get_dimensions();

	static math::vector3 local_corners[8] =
	{
		{ -1, -1, -1 }, { 1, -1, -1 }, { -1, 1, -1 }, { 1, 1, -1 },
		{ -1, -1, 1 }, { 1, -1, 1 }, { -1, 1, 1 }, { 1, 1, 1 }
	};

	ImDrawList* draw = ImGui::GetBackgroundDrawList();
	draw->Flags &= ImDrawListFlags_AntiAliasedLines;

	std::vector<cache::entity_t> players_snapshot;
	{
		std::lock_guard<std::mutex> lock(cache::mtx);
		players_snapshot = cache::players;
	}

	for (auto& entity : players_snapshot)
	{
		if (entity.instance.address == cache::local_player.instance.address)
		{
			continue;
		}

		bool valid = false;
		float left = FLT_MAX, top = FLT_MAX;
		float right = -FLT_MAX, bottom = -FLT_MAX;

		for (auto& pair : entity.parts)
		{
			if (!pair.second.address)
			{
				continue;
			}

			if (pair.second.get_name() == "HumanoidRootPart")
			{
				continue;
			}

			rbx::c_primitive prim = pair.second.get_primitive();
			math::vector3 size = prim.get_size();
			math::vector3 pos = prim.get_position();
			math::matrix3 rot = prim.get_rotation();

			if (size.x == 0.f && size.y == 0.f && size.z == 0.f)
			{
				continue;
			}

			for (math::vector3& lc : local_corners)
			{
				math::vector3 world = pos + rot * math::vector3
				{
					lc.x * size.x * 0.5f,
					lc.y * size.y * 0.5f,
					lc.z * size.z * 0.5f
				};

				math::vector2 out;
				if (game::visualengine->world_to_screen(view, dims, world, out))
				{
					valid = true;
					left = min(left, out.x);
					top = min(top, out.y);
					right = max(right, out.x);
					bottom = max(bottom, out.y);
				}
			}
		}

		if (!valid || left >= right || top >= bottom)
		{
			continue;
		}

		ImVec2 c1(left, top);
		ImVec2 c2(right - left, bottom - top);

		if (settings::visuals::teamcheck)
		{
			if (entity.team == cache::local_player.team)
			{
				continue;
			}
		}

		if (settings::visuals::deadcheck)
		{
			if (entity.health <= 0)
			{
				continue;
			}
		}

		if (settings::visuals::box)
		{
			float transparency = 255.f;

			if (settings::visuals::wallcheck)
			{
				math::vector3 local_position = memory->read<math::vector3>(game::camera + Offsets::Camera::Position);
				math::vector3 target_position = entity.humanoid_root_part.get_primitive().get_position();

				wallcheck->is_visible(local_position, target_position) ? transparency = 255.f : transparency = 50.f;
			}

			esp::outline(c1, c2,
				IM_COL32(
					settings::visuals::colour[0] * 255.f,
					settings::visuals::colour[1] * 255.f,
					settings::visuals::colour[2] * 255.f,
					settings::visuals::colour[3] * transparency,
				)
			);
		}

		if (settings::visuals::debug_wallcheck)
		{
			math::vector3 localpos = cache::local_player.parts["HumanoidRootPart"].get_primitive().get_position();

			for (auto& obb : wallcheck->get_obstacles()) {
				float distance = (obb.center - localpos).length();
				if (distance > settings::visuals::debug_wallcheck_max_length) {
					continue;
				}

				math::vector3 hx = obb.axes[0] * obb.half_size.x;
				math::vector3 hy = obb.axes[1] * obb.half_size.y;
				math::vector3 hz = obb.axes[2] * obb.half_size.z;

				math::vector3 corners[8] = {
					obb.center + hx + hy + hz,
					obb.center - hx + hy + hz,
					obb.center + hx - hy + hz,
					obb.center - hx - hy + hz,
					obb.center + hx + hy - hz,
					obb.center - hx + hy - hz,
					obb.center + hx - hy - hz,
					obb.center - hx - hy - hz
				};

				std::int32_t edges[12][2] = {
					{0,1},{1,3},{3,2},{2,0}, // bottom
					{4,5},{5,7},{7,6},{6,4}, // top
					{0,4},{1,5},{2,6},{3,7}  // sides
				};

				for (int i = 0; i < 12; i++) {
					math::vector2 p1{};
					math::vector2 p2{};

					if (!game::visualengine->world_to_screen(view, dims, corners[edges[i][0]], p1))
					{
						continue;
					}

					if (!game::visualengine->world_to_screen(view, dims, corners[edges[i][1]], p2))
					{
						continue;
					}

					if (p1.x != -1 && p1.y != -1 && p2.x != -1 && p2.y != -1) {
						ImGui::GetForegroundDrawList()->AddLine(
							{ p1.x, p1.y },
							{ p2.x, p2.y },
							IM_COL32(0, 255, 200, 255)
						);
					}
				}
			}
		}
	}
}
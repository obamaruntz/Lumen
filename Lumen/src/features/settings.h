#pragma once
#include <imgui/imgui.h>

namespace settings
{
	namespace aimbot
	{
		inline bool enabled = false;

		inline int target_part = 0;

		inline bool teamcheck = false;
		inline bool deadcheck = false;
		inline bool wallcheck = false;

		namespace a1
		{
			inline float fov = 10.f;
			inline float smooth_x = 10.f;
			inline float smooth_y = 10.f;
		}

		namespace a2
		{
			inline float fov = 10.f;
			inline float smooth_x = 10.f;
			inline float smooth_y = 10.f;
		}

		namespace a3
		{
			inline float fov = 10.f;
			inline float smooth_x = 10.f;
			inline float smooth_y = 10.f;
		}

		namespace a4
		{
			inline float fov = 10.f;
			inline float smooth_x = 10.f;
			inline float smooth_y = 10.f;
		}

		namespace a5
		{
			inline float fov = 10.f;
			inline float smooth_x = 10.f;
			inline float smooth_y = 10.f;
		}

		namespace a6
		{
			inline float fov = 10.f;
			inline float smooth_x = 10.f;
			inline float smooth_y = 10.f;
		}
	}

	namespace visuals
	{
		inline bool debug_wallcheck = false;
		inline float debug_wallcheck_max_length = 75.f;

		inline bool streamproof = true;

		inline bool box = false;
		inline float colour[4] = { 1.f, 1.f, 1.f, 1.f };
		
		inline bool teamcheck = false;
		inline bool deadcheck = false;
		inline bool wallcheck = false;
	}
}
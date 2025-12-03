#pragma once
#include <memory>
#include <windows.h>
#include <sdk/sdk.h>

namespace game
{
	inline std::unique_ptr<rbx::c_datamodel> datamodel{};
	inline std::unique_ptr <rbx::c_visualengine> visualengine{};

	inline std::uint64_t camera{};
	inline HWND roblox_window = nullptr;

	HWND get_roblox_window();
}
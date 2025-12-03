#pragma once
#include <memory>
#include <sdk/sdk.h>

namespace game
{
	inline std::unique_ptr<rbx::c_datamodel> datamodel{};
	inline std::unique_ptr <rbx::c_visualengine> visualengine{};

	inline std::uint64_t camera{};
}
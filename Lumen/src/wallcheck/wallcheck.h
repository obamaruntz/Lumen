#pragma once
#include <vector>

#include "obb.h"
#include <sdk/sdk.h>
#include <sdk/math/math.h>

class c_wallcheck final {
public:
	c_wallcheck() = default;
	~c_wallcheck() = default;

	bool cache_workspace();

	void find_valid_parts(std::vector<rbx::c_instance> instances, std::vector<rbx::c_primitive>& validParts, std::int32_t depth);
	bool is_visible(const math::vector3& origin, const math::vector3& target);

	const std::vector<rbx::c_primitive>& get_parts();
	const std::vector<rbx::obb>& get_obstacles();
private:
	std::vector<rbx::c_primitive> parts;
	std::vector<rbx::obb> obstacles;
};

inline std::unique_ptr<c_wallcheck> wallcheck = std::make_unique<c_wallcheck>();
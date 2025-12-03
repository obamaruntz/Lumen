#include "Wallcheck.h"

#include <iostream>
#include <vector>

#include <game/game.h>

void c_wallcheck::find_valid_parts(std::vector<rbx::c_instance> instances, std::vector<rbx::c_primitive>& valid, std::int32_t depth) {
	for (rbx::c_instance child : instances) {
		std::string className = child.get_class_name();
		std::string name = child.get_name();

		if (
			className == "BasePart" ||
			className == "Part" ||
			className == "MeshPart" ||
			className == "WedgePart" ||
			className == "CornerWedgePart" ||
			className == "Ball" ||
			className == "Cylinder" ||
			className == "UnionOperation" ||
			className == "TrussPart"
			//className == "Terrain" fucky, sometimes games have a terrain covering the whole world which makes the ray never hit
			) {
			rbx::c_primitive prim = child.get_primitive();
			valid.push_back(prim);

			math::vector3 center = prim.get_position();
			math::vector3 size = prim.get_size();
			math::cframe cf = prim.get_cframe();

			if (size.x > 200.f || size.y > 200.f || size.z > 200.f ||
				size.x < 1.f || size.y < 1.f || size.z < 1.f) {
				continue;
			}

			rbx::obb obb(center, size, cf);
			obstacles.push_back(obb);
		}
		if (className == "Folder" || (className == "Model") /*&& depth >= 0*/) {
			find_valid_parts(child.get_children<rbx::c_instance>(), valid, depth + 1);
		}
	}
}

bool c_wallcheck::cache_workspace() {
	std::shared_ptr<rbx::c_instance> workspace = std::make_shared<rbx::c_instance>(game::datamodel->get_workspace().address);
	if (!workspace->address) {
		return false;
	}

	std::vector<rbx::c_instance> children = workspace->get_children<rbx::c_instance>();

	std::vector<rbx::c_primitive> valid;
	find_valid_parts(children, valid, -1);

	if (valid.empty()) {
		return false;
	}

	parts = valid;
	return true;
}

bool c_wallcheck::is_visible(const math::vector3& origin, const math::vector3& target) {
	math::vector3 dir = (target - origin).normalized();
	float distance = (target - origin).length();

	const float max_obb_size = 5.f;

	for (const rbx::obb& box : get_obstacles()) {
		float max_obb_extent = max(max(box.half_size.x, box.half_size.y), box.half_size.z);

		float dist_to_center = (box.center - origin).length();
		if (dist_to_center > distance + max_obb_extent) continue;

		if (box.intersects(origin, dir, distance)) {
			return false;
		}
	}

	return true; // no obstacles
}

////////// don't allow our internal vectors to be mutated //////////
																  //
const std::vector<rbx::c_primitive>& c_wallcheck::get_parts() {   //
	return parts;                                                 //
}                                                                 //
																  //
const std::vector<rbx::obb>& c_wallcheck::get_obstacles() {       //
	return obstacles;                                             //
}                                                                 //
																  //
////////////////////////////////////////////////////////////////////
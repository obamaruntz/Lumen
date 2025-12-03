#include "sdk.h"

#include <memory/memory.h>
#include <game/game.h>

std::string rbx::c_nameable::get_name()
{
	auto name{ memory->read<std::uint64_t>(address + Offsets::Instance::Name) };

	if (name != 0)
	{
		return memory->read_string(name);
	}

	return "unknown";
}

std::string rbx::c_nameable::get_class_name()
{
	auto class_descriptor{ memory->read<std::uint64_t>(address + Offsets::Instance::ClassDescriptor) };
	auto class_name{ memory->read<std::uint64_t>(class_descriptor + Offsets::Instance::ClassName) };

	if (class_name != 0)
	{
		return memory->read_string(class_name);
	}

	return "unknown";
}

std::uint64_t rbx::c_node::find_first_child(std::string_view name)
{
	std::vector<std::uint64_t> children{ get_children() };

	for (rbx::c_nameable child : children)
	{
		if (child.get_name() == name)
		{
			return child.address;
		}
	}

	return {};
}

std::uint64_t rbx::c_node::find_first_child_by_class(std::string_view name)
{
	std::vector<std::uint64_t> children{ get_children() };

	for (rbx::c_nameable child : children)
	{
		if (child.get_class_name() == name)
		{
			return child.address;
		}
	}

	return {};
}

std::vector<std::uint64_t> rbx::c_node::get_children()
{
	auto* self{ static_cast<rbx::c_instance*>(this) };

	static thread_local std::vector<std::uint64_t> container{};
	container.clear();

	auto start{ memory->read<std::uint64_t>(self->address + Offsets::Instance::ChildrenStart) };
	auto end{ memory->read<std::uint64_t>(start + Offsets::Instance::ChildrenEnd) };

	for (
		auto instance{ memory->read<std::uint64_t>(start) };
		instance != end;
		instance += sizeof(std::shared_ptr<void*>)
		)
	{
		container.emplace_back(memory->read<std::uint64_t>(instance));
	}

	return container;
}

std::uint64_t rbx::c_node::get_parent()
{
	auto* base{ static_cast<rbx::c_instance*>(this) };

	return memory->read<std::uint64_t>(base->address + Offsets::Instance::Parent);
}

void rbx::c_node::set_parent(const std::uint64_t& parent)
{
	// TODO
}

rbx::c_model_instance rbx::c_player::get_model_instance()
{
	return { memory->read<std::uint64_t>(address + Offsets::Player::ModelInstance) };
}

std::uint64_t rbx::c_player::get_team()
{
	return { memory->read<std::uint64_t>(address + Offsets::Player::Team) };
}

std::string rbx::c_player::get_display_name()
{
	return memory->read_string(address + Offsets::Player::DisplayName);
}

float rbx::c_humanoid::get_health()
{
	return { memory->read<float>(address + Offsets::Humanoid::Health) };
}

float rbx::c_humanoid::get_max_health()
{
	return { memory->read<float>(address + Offsets::Humanoid::MaxHealth) };
}

float rbx::c_humanoid::get_jump_power()
{
	return { memory->read<float>(address + Offsets::Humanoid::JumpPower) };
}

void rbx::c_humanoid::set_jump_power(const float& jump)
{
	memory->write<float>(address + Offsets::Humanoid::JumpPower, jump);
}

float rbx::c_humanoid::get_walk_speed()
{
	return { memory->read<float>(address + Offsets::Humanoid::Walkspeed) };
}

void rbx::c_humanoid::set_walk_speed(const float& speed)
{
	memory->write<float>(address + Offsets::Humanoid::Walkspeed, speed);
	memory->write<float>(address + Offsets::Humanoid::WalkspeedCheck, speed);
}

float rbx::c_humanoid::get_hip_height()
{
	return { memory->read<float>(address + Offsets::Humanoid::HipHeight) };
}

void rbx::c_humanoid::set_hip_height(const float& height)
{
	memory->write<float>(address + Offsets::Humanoid::HipHeight, height);
}

std::uint8_t rbx::c_humanoid::get_rig_type()
{
	return { memory->read<std::uint8_t>(address + Offsets::Humanoid::RigType) };
}

std::uint16_t rbx::c_humanoid::get_state()
{
	auto humanoid_state{ memory->read<std::uint64_t>(address + Offsets::Humanoid::HumanoidState) };
	return { memory->read<std::uint16_t>(humanoid_state + Offsets::Humanoid::HumanoidStateID) };
}

rbx::c_primitive rbx::c_instance::get_primitive()
{
	return { memory->read<std::uint64_t>(address + Offsets::BasePart::Primitive) };
}

rbx::c_primitive rbx::c_part::get_primitive()
{
	return { memory->read<std::uint64_t>(address + Offsets::BasePart::Primitive) };
}

math::vector3 rbx::c_primitive::get_position()
{
	return { memory->read<math::vector3>(address + Offsets::BasePart::Position) };
}

void rbx::c_primitive::set_position(const math::vector3& position)
{
	memory->write<math::vector3>(address + Offsets::BasePart::Position, position);
}

math::matrix3 rbx::c_primitive::get_rotation()
{
	return { memory->read<math::matrix3>(address + Offsets::BasePart::Rotation) };
}

void rbx::c_primitive::set_rotation(const math::matrix3& rotation)
{
	memory->write<math::matrix3>(address + Offsets::BasePart::Rotation, rotation);
}

math::vector3 rbx::c_primitive::get_size()
{
	return { memory->read<math::vector3>(address + Offsets::BasePart::Size) };
}

void rbx::c_primitive::set_size(const math::vector3& size)
{
	memory->write<math::vector3>(address + Offsets::BasePart::Size, size);
}

math::cframe rbx::c_primitive::get_cframe()
{
	return { memory->read<math::cframe>(address + Offsets::BasePart::Rotation) };
}

rbx::c_workspace rbx::c_datamodel::get_workspace()
{
	return { memory->read<std::uint64_t>(address + Offsets::DataModel::Workspace) };
}

std::uint64_t rbx::c_datamodel::get_game_id()
{
	return memory->read<std::uint64_t>(address + Offsets::DataModel::GameId);
}

std::uint64_t rbx::c_datamodel::get_place_id()
{
	return memory->read<std::uint64_t>(address + Offsets::DataModel::PlaceId);
}

std::uint64_t rbx::c_datamodel::get_creator_id()
{
	return memory->read<std::uint64_t>(address + Offsets::DataModel::CreatorId);
}

std::string rbx::c_datamodel::get_server_ip()
{
	return memory->read_string(address + Offsets::DataModel::ServerIP);
}

math::vector2 rbx::c_visualengine::get_dimensions()
{
	HWND roblox_window = game::get_roblox_window();
	if (roblox_window)
	{
		RECT client_rect{};
		if (GetClientRect(roblox_window, &client_rect))
		{
			return { (float)(client_rect.right - client_rect.left), (float)(client_rect.bottom - client_rect.top) };
		}
	}
	return { (float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN) };
}

math::matrix4 rbx::c_visualengine::get_viewmatrix()
{
	return { memory->read<math::matrix4>(address + Offsets::VisualEngine::ViewMatrix) };
}

bool rbx::c_visualengine::world_to_screen(const math::matrix4& view, const math::vector2& dims, const math::vector3& world, math::vector2& out)
{
	math::vector4 clip = view.multiply({ world.x, world.y, world.z, 1.0f });

	if (clip.w < 0.1f)
	{
		return false;
	}

	clip.x /= clip.w;
	clip.y /= clip.w;

	out.x = (dims.x * 0.5f * clip.x) + (dims.x * 0.5f);
	out.y = -(dims.y * 0.5f * clip.y) + (dims.y * 0.5f);

	HWND roblox_window = game::get_roblox_window();
	if (roblox_window)
	{
		RECT client_rect{};
		POINT client_pos{};
		if (GetClientRect(roblox_window, &client_rect))
		{
			client_pos.x = client_rect.left;
			client_pos.y = client_rect.top;
			ClientToScreen(roblox_window, &client_pos);
			out.x += (float)client_pos.x;
			out.y += (float)client_pos.y;
		}
	}

	return true;
}

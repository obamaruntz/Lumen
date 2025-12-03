#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <memory/memory.h>

#include "offsets/offsets.h"
#include "math/math.h"

namespace rbx
{
	/* forward declarations */
	struct c_addressable;
	struct c_nameable;
	struct c_node;
	struct c_instance;
	struct c_player;
	struct c_model_instance;
	struct c_humanoid;
	struct c_humanoid_root_part;
	struct c_part;
	struct c_primitive;
	struct c_datamodel;
	struct c_workspace;
	struct c_visualengine;

	/* SDK classes */
	struct c_addressable
	{
		std::uint64_t address = 0;
		
		c_addressable() = default;
		c_addressable(std::uint64_t address) : address(address) {}
	};

	struct c_nameable : public c_addressable
	{
		using c_addressable::c_addressable;

		std::string get_name();
		std::string get_class_name();
	};

	struct c_node
	{
		std::uint64_t find_first_child(std::string_view name);
		std::uint64_t find_first_child_by_class(std::string_view name);
			
		template <typename type>
		std::vector<type> get_children();

		std::vector<std::uint64_t> get_children();

		std::uint64_t get_parent();
		void set_parent(const std::uint64_t& parent);
	};

	struct c_instance : public c_nameable, public c_node
	{
		using c_nameable::c_nameable;

		c_primitive get_primitive();
	};

	struct c_player final : public c_instance
	{
		using c_instance::c_instance;

		c_model_instance get_model_instance();
		std::uint64_t get_team();
		std::string get_display_name();
	};

	struct c_model_instance final : public c_addressable, public c_node
	{
		using c_addressable::c_addressable;
	};

	struct c_humanoid final : public c_nameable
	{
		using c_nameable::c_nameable;

		float get_health();
		float get_max_health();

		float get_jump_power();
		void set_jump_power(const float& jump);

		float get_walk_speed();
		void set_walk_speed(const float& speed);

		float get_hip_height();
		void set_hip_height(const float& height);

		std::uint8_t get_rig_type();
		std::uint16_t get_state();
	};

	struct c_humanoid_root_part final : public c_nameable
	{
		using c_nameable::c_nameable;

		// TODO: add getters and setters such as velocity
	};

	struct c_part final : public c_nameable
	{
		using c_nameable::c_nameable;

		c_primitive get_primitive();
	};

	struct c_primitive final : public c_addressable
	{
		using c_addressable::c_addressable;

		math::vector3 get_position();
		void set_position(const math::vector3& position);

		math::matrix3 get_rotation();
		void set_rotation(const math::matrix3& rotation);

		math::vector3 get_size();
		void set_size(const math::vector3& size);

		math::cframe get_cframe();
	};

	struct c_datamodel final : public c_instance
	{
		using c_instance::c_instance;

		static std::unique_ptr<c_datamodel> get()
		{
			auto fake_datamodel{ memory->read<std::uint64_t>(memory->m_base_address + Offsets::FakeDataModel::Pointer) };
			auto real_datamodel{ memory->read<std::uint64_t>(fake_datamodel + Offsets::FakeDataModel::RealDataModel) };
			return std::make_unique<c_datamodel>(real_datamodel);
		}

		c_workspace get_workspace();

		std::uint64_t get_game_id();
		std::uint64_t get_place_id();
		std::uint64_t get_creator_id();

		std::string get_server_ip();
	};

	struct c_workspace final : public c_instance
	{
		using c_instance::c_instance;
	};

	struct c_visualengine final : public c_addressable
	{
		using c_addressable::c_addressable;

		static std::unique_ptr<c_visualengine> get()
		{
			auto visualengine{ memory->read<std::uint64_t>(memory->m_base_address + Offsets::VisualEngine::Pointer) };
			return std::make_unique<c_visualengine>(visualengine);
		}

		math::vector2 get_dimensions();
		math::matrix4 get_viewmatrix();

		/* this function signature is "goyslop" */
		bool world_to_screen(
			const math::matrix4& view,
			const math::vector2& dims,
			const math::vector3& world,
			math::vector2& out
		);
	};
}

template <typename type>
std::vector<type> rbx::c_node::get_children()
{
	auto* self{ static_cast<rbx::c_instance*>(this) };

	static thread_local std::vector<type> container{};
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
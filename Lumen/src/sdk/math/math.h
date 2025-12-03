#pragma once
#include <cmath>
#include <algorithm>
#include <array>

// Stackz was here, you are all fucking skids.

namespace math
{
	struct color3 final
	{
		float r{}, g{}, b{};

		constexpr color3() = default;
		constexpr color3(float r, float g, float b) : r(r), g(g), b(b) {}

		color3 operator+(const color3& c) const { return { r + c.r, g + c.g, b + c.b }; }
		color3 operator-(const color3& c) const { return { r - c.r, g - c.g, b - c.b }; }
		color3 operator*(float s) const { return { r * s, g * s, b * s }; }
		color3 operator/(float s) const { return { r / s, g / s, b / s }; }

		color3 clamp() const { return { std::clamp(r, 0.0f, 1.0f), std::clamp(g, 0.0f, 1.0f), std::clamp(b, 0.0f, 1.0f) }; }
	};

	struct color4 final
	{
		float r{}, g{}, b{}, a{};

		constexpr color4() = default;
		constexpr color4(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
		color4(const color3& c, float a = 1.0f) : r(c.r), g(c.g), b(c.b), a(a) {}

		color4 operator*(float s) const { return { r * s, g * s, b * s, a * s }; }
		color4 operator+(const color4& c) const { return { r + c.r, g + c.g, b + c.b, a + c.a }; }
		color4 clamp() const { return { std::clamp(r, 0.0f, 1.0f), std::clamp(g, 0.0f, 1.0f), std::clamp(b, 0.0f, 1.0f), std::clamp(a, 0.0f, 1.0f) }; }
	};

	struct vector2 final
	{
		float x{}, y{};

		constexpr vector2() = default;
		constexpr vector2(float x, float y) : x(x), y(y) {}

		constexpr vector2 operator+(const vector2& r) const { return { x + r.x, y + r.y }; }
		constexpr vector2 operator-(const vector2& r) const { return { x - r.x, y - r.y }; }
		constexpr vector2 operator*(float s) const { return { x * s, y * s }; }
		constexpr vector2 operator/(float s) const { return { x / s, y / s }; }

		constexpr float dot(const vector2& r) const { return x * r.x + y * r.y; }
		float length() const { return std::sqrt(dot(*this)); }
		vector2 normalized() const { float l = length(); return l == 0 ? *this : vector2{ x / l, y / l }; }
		float distance(const vector2& r) const { return (*this - r).length(); }
	};

	struct vector3 final
	{
		float x{}, y{}, z{};

		constexpr vector3() = default;
		constexpr vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		constexpr vector3 operator+(const vector3& r) const { return { x + r.x, y + r.y, z + r.z }; }
		constexpr vector3 operator-(const vector3& r) const { return { x - r.x, y - r.y, z - r.z }; }
		constexpr vector3 operator*(float s) const { return { x * s, y * s, z * s }; }
		constexpr vector3 operator/(float s) const { return { x / s, y / s, z / s }; }

		constexpr float dot(const vector3& r) const { return x * r.x + y * r.y + z * r.z; }
		constexpr vector3 cross(const vector3& r) const { return { y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x }; }

		float length() const { return std::sqrt(dot(*this)); }
		vector3 normalized() const { float l = length(); return l == 0 ? *this : vector3{ x / l, y / l, z / l }; }
		float distance(const vector3& r) const { return (*this - r).length(); }

		constexpr operator float* () noexcept { return &x; }
		constexpr operator const float* () const noexcept { return &x; }
	};

	struct vector4 final
	{
		float x{}, y{}, z{}, w{};

		constexpr vector4() = default;
		constexpr vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
		constexpr vector4(const vector3& v, float w = 1.0f) : x(v.x), y(v.y), z(v.z), w(w) {}

		constexpr vector4 operator+(const vector4& r) const { return { x + r.x, y + r.y, z + r.z, w + r.w }; }
		constexpr vector4 operator-(const vector4& r) const { return { x - r.x, y - r.y, z - r.z, w - r.w }; }
		constexpr vector4 operator*(float s) const { return { x * s, y * s, z * s, w * s }; }
		constexpr vector4 operator/(float s) const { return { x / s, y / s, z / s, w / s }; }

		constexpr float dot(const vector4& r) const { return x * r.x + y * r.y + z * r.z + w * r.w; }
		float length() const { return std::sqrt(dot(*this)); }
		vector4 normalized() const { float l = length(); return l == 0 ? *this : vector4{ x / l, y / l, z / l, w / l }; }
	};

	struct matrix3 final
	{
		float m[3][3]{};

		static matrix3 identity()
		{
			return { {
				{1,0,0},
				{0,1,0},
				{0,0,1}
			} };
		}

		vector3 multiply(const vector3& v) const
		{
			return {
				m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
				m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
				m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
			};
		}

		vector3 operator*(const vector3& v) const
		{
			return multiply(v);
		}

		vector3 forward() const
		{
			return vector3(-m[0][2], -m[1][2], -m[2][2]);
		}

		vector3 right() const
		{
			return vector3(-m[0][0], -m[1][0], -m[2][0]);
		}

		vector3 up() const
		{
			return vector3(m[0][1], m[1][1], m[2][1]);
		}
	};

	struct matrix4 final
	{
		float m[4][4]{};

		float& operator()(int row, int col) noexcept { return m[row][col]; }
		const float& operator()(int row, int col) const noexcept { return m[row][col]; }

		static matrix4 identity()
		{
			return { {
				{1,0,0,0},
				{0,1,0,0},
				{0,0,1,0},
				{0,0,0,1}
			} };
		}

		vector4 multiply(const vector4& v) const
		{
			return {
				m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
				m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
				m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
				m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
			};
		}
	};

	struct cframe final
	{
		matrix3 rotation;
		vector3 position;

		cframe() : rotation(matrix3::identity()), position(0.0f, 0.0f, 0.0f) {}
		cframe(const matrix3& rot, const vector3& pos) : rotation(rot), position(pos) {}
		cframe(const vector3& pos) : rotation(matrix3::identity()), position(pos) {}

		vector3 transform_point(const vector3& point) const
		{
			return rotation * point + position;
		}

		vector3 inverse_transform_point(const vector3& point) const
		{
			return rotation * (point - position);
		}

		cframe inverse() const
		{
			matrix3 inv_rot = rotation;
			vector3 inv_pos = rotation * (position * -1.0f);
			return cframe(inv_rot, inv_pos);
		}
	};

	inline matrix3 look_at(const vector3& from, const vector3& to)
	{
		vector3 forward = (to - from).normalized();
		
		vector3 up_ref(0, 1, 0);
		if (std::abs(forward.dot(up_ref)) > 0.99f)
		{
			up_ref = vector3(1, 0, 0);
		}
		
		vector3 right = forward.cross(up_ref).normalized();
		vector3 up = right.cross(forward).normalized();

		matrix3 result{};
		result.m[0][0] = -right.x;
		result.m[0][1] = up.x;
		result.m[0][2] = -forward.x;
		result.m[1][0] = -right.y;
		result.m[1][1] = up.y;
		result.m[1][2] = -forward.y;
		result.m[2][0] = -right.z;
		result.m[2][1] = up.z;
		result.m[2][2] = -forward.z;

		return result;
	}
}
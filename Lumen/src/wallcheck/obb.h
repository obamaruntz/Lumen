#pragma once
#include <windows.h>
#include <sdk/math/math.h>

namespace rbx
{
	struct obb final
	{
		math::vector3 center;
		math::vector3 half_size;
		math::vector3 axes[3];

		obb(const math::vector3& center, const math::vector3& size, const math::cframe& cf)
			: center(center), half_size(size * 0.5f)
		{
			axes[0] = math::vector3(cf.rotation.m[0][0], cf.rotation.m[0][1], cf.rotation.m[0][2]);
			axes[1] = math::vector3(cf.rotation.m[1][0], cf.rotation.m[1][1], cf.rotation.m[1][2]);
			axes[2] = math::vector3(cf.rotation.m[2][0], cf.rotation.m[2][1], cf.rotation.m[2][2]);
		}

		bool intersects(const math::vector3& origin, const math::vector3& dir, float max) const
		{
			float tmin = 0.0f;
			float tmax = max;

			bool test = true;
			math::vector3 p = center - origin;

			for (int i = 0; i < 3; ++i)
			{
				float e = axes[i].dot(p);
				float f = axes[i].dot(dir);

				const float EPSILON = 1e-6f;
				if (fabs(f) > EPSILON)
				{
					float t1 = (e + half_size[i]) / f;
					float t2 = (e - half_size[i]) / f;

					if (t1 > t2) std::swap(t1, t2);

					tmin = max(tmin, t1);
					tmax = min(tmax, t2);

					if (tmax < tmin)
					{
						return false;
					}
				}
				else
				{
					if (-e - half_size[i] > 0.0f || -e + half_size[i] < 0.0f)
					{
						return false;
					}
				}
			}

			return true;
		}
	};
}

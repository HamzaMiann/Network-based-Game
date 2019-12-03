#pragma once

#include <stdlib.h>			// For rand()
#include <math.h>
#include <glm/vec3.hpp>
#define PI 3.14159265

namespace Mathf
{

	static void rotate_vector(float angle, glm::vec3 const& center, glm::vec3& point)
	{
		point.x -= center.x;
		point.z -= center.z;

		angle = angle * (PI / 180.f);
		float s = sin(angle);
		float c = cos(angle);

		float xnew = point.x * c - point.z * s;
		float znew = point.z * c + point.x * s;
		point.x = xnew + center.x;
		point.z = znew + center.z;
	}

	static glm::vec3 get_rotated_vector(float angle, glm::vec3 const& center, glm::vec3 const& point)
	{
		angle = angle * (PI / 180.f);
		float s = sin(angle);
		float c = cos(angle);

		float xnew = point.x * c - point.z * s;
		float znew = point.z * c + point.x * s;

		return glm::vec3(
			xnew + center.x,
			point.y,
			znew + center.z
		);
	}

	static glm::vec3 get_direction_vector(glm::vec3 const& from, glm::vec3 const& to)
	{
		return (to - from);
	}

	static glm::vec3 get_reverse_direction_vector(glm::vec3 const& from, glm::vec3 const& to)
	{
		return (from - to);
	}

	static float lerp(float v0, float v1, float t)
	{
		return (1 - t) * v0 + t * v1;
	}
	static glm::vec3 lerp(glm::vec3 v0, glm::vec3 v1, float t)
	{
		return glm::vec3(
			lerp(v0.x, v1.x, t),
			lerp(v0.y, v1.y, t),
			lerp(v0.z, v1.z, t)
		);
	}
	static float clamp(float x, float lowerlimit, float upperlimit)
	{
		if (x < lowerlimit)
			x = lowerlimit;
		if (x > upperlimit)
			x = upperlimit;
		return x;
	}
	static float smoothstep(float edge0, float edge1, float x)
	{
		// Scale, bias and saturate x to 0..1 range
		x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
		// Evaluate polynomial
		return x * x * (3 - 2 * x);
	}

	template <class T>
	static T randInRange(T min, T max)
	{
		double value =
			min + static_cast <double> (rand())
			/ (static_cast <double> (RAND_MAX / (static_cast<double>(max - min))));
		return static_cast<T>(value);
	};

}
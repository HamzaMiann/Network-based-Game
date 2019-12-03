#pragma once

#include <glm/vec3.hpp>

class cAABB
{
public:
	glm::vec3 min;
	float length;

	bool contains(glm::vec3 const& point)
	{
		if (point.x < min.x) return false;
		if (point.y < min.y) return false;
		if (point.z < min.z) return false;

		if (point.x > min.x + length) return false;
		if (point.y > min.y + length) return false;
		if (point.z > min.z + length) return false;

		return true;
	}
};
#pragma once

#include <glm/vec3.hpp>

struct cAABB
{
	glm::vec3 min;
	float length;
	bool contains(glm::vec3 const& pos)
	{
		if (pos.x < min.x) return false;
		if (pos.y < min.y) return false;
		if (pos.z < min.z) return false;

		if (pos.x > min.x + length) return false;
		if (pos.y > min.y + length) return false;
		if (pos.z > min.z + length) return false;

		return true;
	}
};
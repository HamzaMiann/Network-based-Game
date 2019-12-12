#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

template <class T>
static T randInRange(T min, T max)
{
	double value =
		min + static_cast <double> (rand())
		/ (static_cast <double> (RAND_MAX / (static_cast<double>(max - min))));
	return static_cast<T>(value);
};

const glm::vec3 forward(0.f, 0.f, 1.f);

struct Transform
{
	// x is x position
	// y is y ROTATION
	// z is z position
	glm::vec3 pos = glm::vec3(
		randInRange(-20.f, 20.f),
		0.f,
		randInRange(-10.f, 10.f)
	);

	glm::quat Rotation()
	{
		return glm::quat(glm::vec3(0.f, glm::radians(pos.y), 0.f));
	}

	glm::vec3 Forward()
	{
		return Rotation() * forward;
	}

};
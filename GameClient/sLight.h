#pragma once

#include <glm/vec4.hpp>

struct sLight
{
	glm::vec4 position;
	glm::vec4 diffuse;
	glm::vec4 specular;		// rgb = highlight colour, w = power
	glm::vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	glm::vec4 direction;	// Spot, directional lights
	glm::vec4 param1;		// x = lightType, y = inner angle, z = outer angle, w = TBD
							// 0 = pointlight
							// 1 = spot light
							// 2 = directional light
	glm::vec4 param2;		// x = 0 for off, 1 for on
};
#pragma once

#include <vector>
#include <glm/vec3.hpp>

struct sParticle
{
	glm::vec3 pos;
	glm::vec3 vel;
	float scale = 1.f;
	float life_time = -1.f;
};

class cParticleEffect
{
public:
	std::vector<sParticle*> particles;
	glm::vec3 pos;

	glm::vec3 min_vel;
	glm::vec3 max_vel;

	float min_time;
	float max_time;

	float min_scale;
	float max_scale;

	cParticleEffect(unsigned int numParticles);
	virtual ~cParticleEffect();

	void Generate();

	void Step(float delta_time);
};
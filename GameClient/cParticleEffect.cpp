#include "cParticleEffect.h"
#include "Mathf.h"

void _reset_particle(sParticle* particle, cParticleEffect* effect)
{
	particle->pos = effect->pos;
	particle->life_time = Mathf::randInRange(effect->min_time, effect->max_time);
	particle->scale = Mathf::randInRange(effect->min_scale, effect->max_scale);
	particle->vel.x = Mathf::randInRange(effect->min_vel.x, effect->max_vel.x);
	particle->vel.y = Mathf::randInRange(effect->min_vel.y, effect->max_vel.y);
	particle->vel.z = Mathf::randInRange(effect->min_vel.z, effect->max_vel.z);
}

cParticleEffect::cParticleEffect(unsigned int numParticles)
{
	this->pos = glm::vec3(0.f);
	this->min_vel = glm::vec3(0.f);
	this->max_vel = glm::vec3(1.f);
	this->min_time = 1.f;
	this->max_time = 2.f;
	this->min_scale = 0.5f;
	this->min_scale = 1.f;
	for (unsigned int i = 0; i < numParticles; ++i)
	{
		particles.push_back(new sParticle);
	}
}

cParticleEffect::~cParticleEffect()
{
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		delete particles[i];
	}
	particles.clear();
}

void cParticleEffect::Generate()
{
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		sParticle* particle = particles[i];
		_reset_particle(particle, this);
	}
}

void cParticleEffect::Step(float delta_time)
{
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		sParticle* particle = particles[i];
		particle->life_time -= delta_time;
		if (particle->life_time < 0.f)
		{
			_reset_particle(particle, this);
		}
		else
		{
			particle->pos += particle->vel * delta_time;
		}
	}
}

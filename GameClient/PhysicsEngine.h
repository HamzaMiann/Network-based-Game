#pragma once

class Scene;
class cGameObject;
#include "cDebugRenderer.h"
#include "octree.h"

class PhysicsEngine
{
private:
	PhysicsEngine() {}

public:

	static PhysicsEngine* Instance()
	{
		static PhysicsEngine instance;
		return &instance;
	}

	~PhysicsEngine() {}

	const glm::vec3 Gravity = glm::vec3(0.f, -9.8f, 0.f);
	const float drag = 0.7f;
	const float friction = 0.7f;

	cDebugRenderer* renderer;

	octree* tree;

	void IntegrationStep(Scene* scene, float delta_time);
	void CheckCollisions(Scene* scene, float delta_time);
	void GenerateAABB(Scene* scene);
};

void FindClosestPointToMesh(
	Scene& scene, float& closestDistanceSoFar,
	glm::vec3& closestPoint,
	glm::vec3& normalVector,
	cGameObject* const meshObject,
	cGameObject* const pObj);

const sMeshTriangle* FindClosestPointToTriangles(
	std::vector<const sMeshTriangle*> const& triangles,
	float& closestDistanceSoFar,
	glm::vec3& closestPoint,
	glm::vec3& normalVector,
	glm::vec3 point);



int IntersectLineTriangle(glm::vec3 p, glm::vec3 q, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3& r);
#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>

class cGameObject;
class cAABB;

class Renderer
{
private:
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;

	Renderer() {}

public:

	vec3			cameraEye = vec3(0.0f, 0.0f, -10.0);
	vec3			cameraTarget = vec3(0.0f, 0.0f, 0.0f);
	vec3			upVector = vec3(0.0f, 1.0f, 0.0f);

	~Renderer() {}

	static Renderer* Instance()
	{
		static Renderer instance;
		return &instance;
	}

	void SetFloat(std::string name, float value);
	void SetBool(std::string name, bool value);
	void SetVec3(std::string name, vec3 value);
	void SetVec4(std::string name, vec4 value);

	void RenderGameObject(cGameObject* object);
	void RenderAABB(cAABB* object);

};
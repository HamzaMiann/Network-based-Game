#pragma once

#include "iInputHandler.h"
#include "Scene.h"
#include <glm/vec4.hpp>

class cLightController : public iInputHandler
{
private:
	unsigned int index = 0;
	unsigned int property_index = 0;
	glm::vec4* property;
	Scene& _scene;
public:
	cLightController() = delete;
	cLightController(Scene& scene);
	virtual ~cLightController();
	virtual void HandleInput(GLFWwindow* window);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
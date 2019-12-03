#pragma once

#include "iInputHandler.h"
#include "Scene.h"

class cLayoutController : public iInputHandler
{
private:
	unsigned int index = 0;
	Scene& _scene;
public:
	cLayoutController() = delete;
	cLayoutController(Scene& scene);
	virtual ~cLayoutController();
	virtual void HandleInput(GLFWwindow* window);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
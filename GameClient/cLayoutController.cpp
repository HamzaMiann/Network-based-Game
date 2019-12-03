
#include "cLayoutController.h"
#include "cGameObject.h"
#include <iostream>

glm::vec3 original_target_position;

cLayoutController::cLayoutController(Scene& scene) : _scene(scene)
{
	original_target_position = _scene.cameraTarget;
}

cLayoutController::~cLayoutController()
{
	_scene.cameraTarget = original_target_position;
}

void cLayoutController::HandleInput(GLFWwindow * window)
{
	float speed = 10.f;

	if (this->_scene.vecGameObjects.size() == 0) return;

	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		this->_scene.vecGameObjects[index]->pos.z += 0.1f * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		this->_scene.vecGameObjects[index]->pos.z -= 0.1f * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		this->_scene.vecGameObjects[index]->pos.x += 0.1f * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		this->_scene.vecGameObjects[index]->pos.x -= 0.1f * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_T))
	{
		this->_scene.vecGameObjects[index]->pos.y += 0.1f * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_Y))
	{
		this->_scene.vecGameObjects[index]->pos.y -= 0.1f * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_I))
	{
		this->_scene.vecGameObjects[index]->updateOrientation(glm::vec3(0.f, 0.f, 0.1f));
	}
	if (glfwGetKey(window, GLFW_KEY_K))
	{
		this->_scene.vecGameObjects[index]->updateOrientation(glm::vec3(0.f, 0.f, -0.1f));
	}

	if (glfwGetKey(window, GLFW_KEY_J))
	{
		this->_scene.vecGameObjects[index]->updateOrientation(glm::vec3(0.1f, 0.f, 0.f));
	}
	if (glfwGetKey(window, GLFW_KEY_L))
	{
		this->_scene.vecGameObjects[index]->updateOrientation(glm::vec3(-0.1f, 0.f, 0.f));
	}

	if (glfwGetKey(window, GLFW_KEY_U))
	{
		this->_scene.vecGameObjects[index]->updateOrientation(glm::vec3(0.f, 0.1f, 0.f));
	}
	if (glfwGetKey(window, GLFW_KEY_O))
	{
		this->_scene.vecGameObjects[index]->updateOrientation(glm::vec3(0.f, -0.1f, 0.f));
	}

	if (glfwGetKey(window, GLFW_KEY_EQUAL))
	{
		this->_scene.vecGameObjects[index]->scale *= 1.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_MINUS))
	{
		this->_scene.vecGameObjects[index]->scale *= 0.95f;
	}

	_scene.cameraTarget = _scene.vecGameObjects[index]->pos;


}

void cLayoutController::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (this->_scene.vecGameObjects.size() == 0) return;

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		index++;
		if (index >= this->_scene.vecGameObjects.size())
		{
			index = 0;
		}
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		_scene.vecGameObjects[index]->isWireframe = !_scene.vecGameObjects[index]->isWireframe;
		_scene.vecGameObjects[index]->uniformColour = !_scene.vecGameObjects[index]->uniformColour;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
	{
		if (_scene.SaveLayout())
			std::cout << "Layout Saved!" << std::endl;
		else
			std::cout << "Unable to save layout..." << std::endl;
	}
}

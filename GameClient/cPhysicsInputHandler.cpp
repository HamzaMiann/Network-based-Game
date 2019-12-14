
#include "cPhysicsInputHandler.h"
#include "Scene.h"
#include "Mathf.h"
#include "cGameObject.h"
#include "cDebugRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "cLowpassFilter.h"
#include "UDPClient.h"

glm::vec3 originalBallPosition = glm::vec3(0.0f, 6.0f, 0.0f);

const glm::vec3 forward = glm::vec3(0.f, 0.f, 1.f);
float ychange = 0.f, xchange = 0.f, zchange = 0.f;

cPhysicsInputHandler::cPhysicsInputHandler(Scene& scene, GLFWwindow* window) : _scene(scene)
{
	previousX = 0;
	previousY = 0;
	this->_window = window;
	//glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	player = nullptr;
	for (int i = 0; i < scene.vecGameObjects.size(); ++i)
	{
		if (scene.vecGameObjects[i]->tag == "player")
		{
			player = _scene.vecGameObjects[i];
			break;
		}
	}
}

cPhysicsInputHandler::~cPhysicsInputHandler()
{
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void cPhysicsInputHandler::HandleInput(GLFWwindow* window)
{

	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if (previousX == 0 && previousY == 0)
	{
		previousX = x;
		previousY = y;
		return;
	}


	_scene.cameraTarget = glm::vec3(0.f);
	_scene.cameraEye = glm::vec3(0.f, 100.f, 0.f);


	_scene.upVector = glm::vec3(0.f, 0.f, 1.f);// up;

	float speed = 1.f;

	cLowpassFilter* filter = cLowpassFilter::Instance();
	UDPClient* client = UDPClient::Instance();

	char input[6];
	input[0] = 0;
	input[1] = 0;
	input[2] = 0;
	input[3] = 0;
	input[4] = 0;
	input[5] = 0;

	bool should_send = false;

	unsigned int id = client->client_id;
	
	// rotation first
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		input[2] = 1;
		if (id != -1)
		{
			(*client->playerObjects)[id]->updateOrientation(glm::vec3(0.f, -90.f * filter->delta_time() * 2.f, 0.f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		input[3] = 1;
		if (id != -1)
		{
			(*client->playerObjects)[id]->updateOrientation(glm::vec3(0.f, 90.f * filter->delta_time() * 2.f, 0.f));
		}
	}

	if (glfwGetKey(window, GLFW_KEY_W))
	{
		input[0] = 1;
		if (id != -1)
		{
			(*client->playerObjects)[id]->pos += ((*client->playerObjects)[id]->getQOrientation() * forward) * filter->delta_time() * 1.f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		input[1] = 1;
		if (id != -1)
		{
			(*client->playerObjects)[id]->pos += ((*client->playerObjects)[id]->getQOrientation() * -forward) * filter->delta_time() * 1.f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE))
	{
		input[4] = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_R))
	{
		input[5] = 1;
	}
	

	player->pos.y = 50.f;

	client->Send(input, 6);

	//player->updateOrientation(glm::vec3(0.f, xchange * 0.5f, 0.f));

	previousX = x;
	previousY = y;

}


void cPhysicsInputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
	}
}
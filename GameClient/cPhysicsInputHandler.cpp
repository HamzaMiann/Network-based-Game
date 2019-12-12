
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

	//glm::vec3 forward = (player->getQOrientation() * glm::vec4(0.f, 0.f, 1.f, 1.f)) * 5.f;
	//glm::vec3 right = glm::normalize((player->getQOrientation() * glm::quat(glm::vec3(0.f, glm::radians(90.f), 0.f))) * glm::vec4(0.f, 0.f, 1.f, 1.f)) * 5.f;
	//glm::vec3 up = glm::normalize(glm::cross(forward, right));


	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if (previousX == 0 && previousY == 0)
	{
		previousX = x;
		previousY = y;
		return;
	}


	//_scene.cameraTarget = player->pos + forward;
	_scene.cameraTarget = glm::vec3(0.f);
	//_scene.cameraEye = player->pos - forward * 0.7f;
	_scene.cameraEye = glm::vec3(0.f, 100.f, 0.f);

	//glm::vec3 forward = glm::normalize(_scene.cameraTarget - _scene.cameraEye);

	//glm::vec3 up = glm::normalize(glm::cross(forward, right));


	_scene.upVector = glm::vec3(0.f, 0.f, 1.f);// up;

	//_scene.cameraEye += up * 1.f;

	//float deltaX = previousX - x;
	//float deltaY = previousY - y;

	float speed = 1.f;

	/*if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
	{
		speed *= 3.5f;
	}*/

	//xchange = Mathf::lerp(xchange, deltaX, 0.2f);
	//ychange = Mathf::lerp(ychange, deltaY, 0.2f);

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
		//player->AddForce(right * -0.5f);// * filter->delta_time());
		//player->pos.x -= filter->delta_time();
		input[2] = 1;
		//xchange = Mathf::lerp(xchange, -90.f, filter->delta_time() * 90.f);
		if (id != -1)
		{
			(*client->playerObjects)[id]->updateOrientation(glm::vec3(0.f, -90.f * filter->delta_time() * 2.f, 0.f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		//player->AddForce(right * 0.5f);// * filter->delta_time());
		//player->pos.x += filter->delta_time();
		input[3] = 1;
		//xchange = Mathf::lerp(xchange, 90.f, filter->delta_time() * 90.f);
		if (id != -1)
		{
			(*client->playerObjects)[id]->updateOrientation(glm::vec3(0.f, 90.f * filter->delta_time() * 2.f, 0.f));
		}
	}

	if (glfwGetKey(window, GLFW_KEY_W))
	{
		//player->AddForce(forward * 1.f * speed);// *filter->delta_time());
		//player->pos.z += filter->delta_time();
		input[0] = 1;
		if (id != -1)
		{
			(*client->playerObjects)[id]->pos += ((*client->playerObjects)[id]->getQOrientation() * forward) * filter->delta_time() * 1.f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		//player->AddForce(forward * -1.f * speed);// * filter->delta_time());
		//player->pos.z -= filter->delta_time();
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
	
	//if (glfwGetKey(window, GLFW_KEY_Q))
	//{
	//	xchange = 20.f;
	//	//xchange = Mathf::lerp(xchange, 3.f, filter->delta_time() * 5.f);
	//}
	//else if (glfwGetKey(window, GLFW_KEY_E))
	//{
	//	xchange = -20.f;
	//	//xchange = Mathf::lerp(xchange, -3.f, filter->delta_time() * 5.f);
	//}
	//else
	//{
	//	xchange = 0.f;
	//	//xchange = Mathf::lerp(xchange, 0.f, filter->delta_time() * 5.f);
	//}

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
		//_scene.vecGameObjects[1]->velocity += _scene.upVector * 2.f;
		//_scene.pAudioEngine->PlaySound("bounce");
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		//_scene.vecGameObjects[1]->pos = originalBallPosition;
		//_scene.vecGameObjects[1]->velocity = glm::vec3(0.f);
		//_scene.pAudioEngine->PlaySound("respawn");
	}
}

#include "cPhysicsInputHandler.h"
#include "Scene.h"
#include "Mathf.h"
#include "cGameObject.h"
#include "cDebugRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "cLowpassFilter.h"

glm::vec3 originalBallPosition = glm::vec3(0.0f, 6.0f, 0.0f);





float ychange = 0.f, xchange = 0.f, zchange = 0.f;

cPhysicsInputHandler::cPhysicsInputHandler(Scene& scene, GLFWwindow* window) : _scene(scene)
{
	previousX = 0;
	previousY = 0;
	this->_window = window;
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
	
	/*glm::vec3 forwardVector = Mathf::get_direction_vector(_scene.cameraEye, _scene.cameraTarget);
	glm::vec3 backwardsVector = Mathf::get_reverse_direction_vector(_scene.cameraEye, _scene.cameraTarget);
	glm::vec3 rightVector = Mathf::get_rotated_vector(-90.f, glm::vec3(0.f), forwardVector);
	glm::vec3 leftVector = Mathf::get_rotated_vector(90.f, glm::vec3(0.f), forwardVector);*/

	//_scene.upVector = normalize(glm::cross(forwardVector, rightVector));

	//_scene.cameraEye.y += _scene.upVector.y * 3.f;

	glm::vec3 forward = (player->getQOrientation() * glm::vec4(0.f, 0.f, 1.f, 1.f)) * 5.f;
	glm::vec3 right = glm::normalize((player->getQOrientation() * glm::quat(glm::vec3(0.f, glm::radians(90.f), 0.f))) * glm::vec4(0.f, 0.f, 1.f, 1.f)) * 5.f;
	glm::vec3 up = glm::normalize(glm::cross(forward, right));

	/*cDebugRenderer::Instance()->addLine(player->pos,
										player->pos + forward,
										glm::vec3(0.f, 1.f, 0.f), 0.1);

	cDebugRenderer::Instance()->addLine(player->pos,
										player->pos + glm::normalize(glm::cross(forward, right)),
										glm::vec3(0.f, 1.f, 0.f), 0.1);*/


	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if (previousX == 0 && previousY == 0)
	{
		previousX = x;
		previousY = y;
		return;
	}

	//printf("%f, %f\n", x, y);

	_scene.cameraTarget = player->pos + forward;
	_scene.cameraEye = player->pos - forward * 0.7f;
	_scene.upVector = up;

	_scene.cameraEye += up * 1.f;

	float deltaX = previousX - x;
	float deltaY = previousY - y;

	float speed = 1.f;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
	{
		speed *= 3.5f;
	}

	xchange = Mathf::lerp(xchange, deltaX, 0.2f);
	ychange = Mathf::lerp(ychange, deltaY, 0.2f);

	cLowpassFilter* filter = cLowpassFilter::Instance();

	if (glfwGetKey(window, GLFW_KEY_W))
	{
		player->AddForce(forward * 1.f * speed);// *filter->delta_time());
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		player->AddForce(forward * -1.f * speed);// * filter->delta_time());
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		player->AddForce(right * 0.5f);// * filter->delta_time());
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		player->AddForce(right * -0.5f);// * filter->delta_time());
	}
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		zchange = Mathf::lerp(zchange, -3.f, 0.1f);
	}
	else if (glfwGetKey(window, GLFW_KEY_E))
	{
		zchange = Mathf::lerp(zchange, 3.f, 0.1f);
	}
	else
	{
		zchange = Mathf::lerp(zchange, 0.f, 0.1f);
	}

	player->updateOrientation(glm::vec3(-ychange * 0.05f, 0.f, 0.f));
	player->updateOrientation(glm::vec3(0.f, xchange * 0.05f, 0.f));
	player->updateOrientation(glm::vec3(0.f, 0.f, zchange));
	//if (glfwGetKey(window, GLFW_KEY_A))
	//{
	//	//_scene.vecGameObjects[1]->velocity += rightVector * 0.01f;
	//	player->updateOrientation(glm::vec3(0.f, 3.f, 0.f));
	//}
	//if (glfwGetKey(window, GLFW_KEY_D))
	//{
	//	player->updateOrientation(glm::vec3(0.f, -3.f, 0.f));
	//	//_scene.vecGameObjects[1]->velocity += leftVector * 0.01f;
	//}

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
		_scene.vecGameObjects[1]->pos = originalBallPosition;
		//_scene.vecGameObjects[1]->velocity = glm::vec3(0.f);
		//_scene.pAudioEngine->PlaySound("respawn");
	}
}
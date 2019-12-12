#include "GLCommon.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>		// c libs
#include <stdio.h>		// c libs

#include <iostream>
#include <string>

#include "Mathf.h"
#include "PhysicsEngine.h"
#include "Scene.h"
#include "cLightManager.h"
#include "cDebugRenderer.h"
#include "cGameObject.h"
#include "cVAOManager.h"

#include "iInputHandler.h"
#include "cPhysicsInputHandler.h"
#include "cLightController.h"
#include "cLayoutController.h"
#include "cModelLoader.h"
#include "AudioEngine.hpp"
#include "cAudioInputHandler.h"
#include "TextureManager/cBasicTextureManager.h"
#include "cParticleEffect.h"
#include "cLowpassFilter.h"
#include "UDPClient.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

unsigned int input_id = 0;

bool is_paused = false;

Scene* scene;
GLFWwindow* window;
iInputHandler* pInputHandler;

cBasicTextureManager* textureManager;

void DrawObject(cGameObject* objPtr, float ratio, glm::mat4 const& v, glm::mat4 const& p);
void DrawParticle(sParticle* objPtr, float ratio, glm::mat4 const& v, glm::mat4 const& p);

void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, float ratio, glm::mat4 const& v, glm::mat4 const& p);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	/*if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		delete pInputHandler;
		input_id++;
		if (input_id % 3 == 0)
		{
			pInputHandler = new cLayoutController(*scene);
		}
		else if (input_id % 3 == 1)
		{
			pInputHandler = new cLightController(*scene);
		}
		else
		{
			pInputHandler = new cPhysicsInputHandler(*scene, window);
		}
	}*/

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		is_paused = !is_paused;
	}

	if (pInputHandler) pInputHandler->key_callback(window, key, scancode, action, mods);
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void HandleInput(GLFWwindow* window)
{
	glm::vec3 forwardVector = Mathf::get_direction_vector(scene->cameraEye, scene->cameraTarget);
	glm::vec3 backwardsVector = Mathf::get_reverse_direction_vector(scene->cameraEye, scene->cameraTarget);
	glm::vec3 rightVector = Mathf::get_rotated_vector(-90.f, glm::vec3(0.f), forwardVector);
	glm::vec3 leftVector = Mathf::get_rotated_vector(90.f, glm::vec3(0.f), forwardVector);


	// Move the camera (A & D for left and right, along the x axis)
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		Mathf::rotate_vector(-5.f, scene->cameraTarget, scene->cameraEye);
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		Mathf::rotate_vector(5.f, scene->cameraTarget, scene->cameraEye);
	}

	// Move the camera (Q & E for up and down, along the y axis)
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		scene->cameraEye.y -= glm::length(forwardVector) * 0.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		scene->cameraEye.y += glm::length(forwardVector) * 0.05f;
	}

	// Move the camera (W & S for towards and away, along the z axis)
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		scene->cameraEye += forwardVector * 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		scene->cameraEye += backwardsVector * 0.1f;
	}

	if (pInputHandler) pInputHandler->HandleInput(window);
	
}

int main(void)
{

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLEngine", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	// Load scene from file
	cGameObject* pSkyBoxSphere = new cGameObject();

	textureManager = cBasicTextureManager::Instance();
	std::string errorString;
	textureManager->SetBasePath("assets/textures/cubemaps/");
	if (textureManager->CreateCubeTextureFromBMPFiles("space",
													  "SpaceBox_right1_posX.bmp", "SpaceBox_left2_negX.bmp",
													  "SpaceBox_top3_posY.bmp", "SpaceBox_bottom4_negY.bmp",
													  "SpaceBox_front5_posZ.bmp", "SpaceBox_back6_negZ.bmp", true, errorString))
	{
		scene = Scene::LoadFromXML("water.scene.xml");
		pSkyBoxSphere->pos = glm::vec3(0.f);
		pSkyBoxSphere->meshName = "sphere";
		pSkyBoxSphere->shaderName = "basic";
		pSkyBoxSphere->tag = "skybox";
		pSkyBoxSphere->pos = glm::vec3(0.0f, 0.f, 0.0f);
		pSkyBoxSphere->scale = 7000.0f;
		pSkyBoxSphere->texture[0] = "Pizza.bmp";
		pSkyBoxSphere->textureRatio[0] = 1.0f;
		pSkyBoxSphere->inverseMass = 0.0f;
		pSkyBoxSphere->gravityScale = 0.f;
		scene->vecGameObjects.push_back(pSkyBoxSphere);
		std::cout << "Space skybox loaded" << std::endl;
	}
	else
	{
		std::cout << "OH NO! Error while loading cubemap :( " << errorString << std::endl;
		exit(1);
	}

	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing

	PhysicsEngine* phys = PhysicsEngine::Instance();
	phys->GenerateAABB(scene);

	pInputHandler = new cPhysicsInputHandler(*scene, window);


	cDebugRenderer* renderer = cDebugRenderer::Instance();
	renderer->initialize();

#if _DEBUG
	phys->renderer = renderer;
#endif

	


	glm::vec3 min = scene->pModelLoader->min;
	glm::vec3 max = scene->pModelLoader->max;

	cGameObject* ship = scene->vecGameObjects[0];
	sLight* light1 = scene->pLightManager->Lights[0];
	sLight* light2 = scene->pLightManager->Lights[1];

	cParticleEffect pEffect(200);
	pEffect.min_time = .1f;
	pEffect.max_time = 3.f;
	pEffect.min_scale = 0.001f;
	pEffect.max_scale = 0.03f;
	pEffect.pos = glm::vec3(0.f, 20.f, 0.f);
	pEffect.min_vel = glm::vec3(-.3f);
	pEffect.max_vel = glm::vec3(.3f);
	pEffect.Generate();

	cGameObject* sphere = new cGameObject;
	sphere->meshName = "sphere_particle";
	sphere->shaderName = "particle";
	sphere->inverseMass = 0.f;
	sphere->texture[0] = ship->texture[0];
	sphere->textureRatio[0] = 1.f;


	// NETWORK STUFF
	//---------------------------------------------
	//---------------------------------------------
	UDPClient* network_client = UDPClient::Instance();

	network_client->CreateSocket("127.0.0.1", 5150);
	scene->vecGameObjects.erase(scene->vecGameObjects.begin());

	std::vector<cGameObject*> players;
	network_client->playerObjects = &players;
	players.push_back(ship);

	for (int i = 0; i < 3; ++i)
	{
		cGameObject* newPlayer = new cGameObject;
		newPlayer->pos = glm::vec3(0.f, 50.f, 0.f);
		newPlayer->texture[0] = ship->texture[0];
		newPlayer->texture[1] = ship->texture[1];
		newPlayer->textureRatio[0] = ship->textureRatio[0];
		newPlayer->textureRatio[1] = ship->textureRatio[1];
		newPlayer->gravityScale = ship->gravityScale;
		newPlayer->meshName = ship->meshName;
		newPlayer->shaderName = ship->shaderName;
		newPlayer->scale = ship->scale;
		newPlayer->specColour = ship->specColour;
		newPlayer->specIntensity = ship->specIntensity;
		players.push_back(newPlayer);
	}
	//---------------------------------------------
	//---------------------------------------------

	cLowpassFilter* filter = cLowpassFilter::Instance();
	float current_time = (float)glfwGetTime();
	float previous_time = (float)glfwGetTime();
	float delta_time = 0.f;

	while (!glfwWindowShouldClose(window))
	{
		previous_time = current_time;
		current_time = (float)glfwGetTime();

		if (!is_paused)
		{
			delta_time = filter->add_time(current_time - previous_time);
		}
		else
		{
			delta_time = 0.f;
		}

		// Handle key inputs
		HandleInput(window);

		//scene->cameraTarget = ship->pos;

		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;


		glViewport(0, 0, width, height);

		// Clear both the colour buffer (what we see) and the 
		//  depth (or z) buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glEnable(GL_BLEND);      // Enable blend or "alpha" transparency
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		network_client->Update();

		// Update the objects' physics
		phys->CheckCollisions(scene, delta_time);
		phys->IntegrationStep(scene, delta_time);

		// Update particles;
		pEffect.Step(delta_time);
		
		// Update 3D audio engine
		//scene->pAudioEngine->Update3d(scene->cameraEye, scene->cameraTarget, delta_time);

		pSkyBoxSphere->pos = scene->cameraEye;
		// **************************************************
		// **************************************************
		glm::mat4 p, v;

		// Projection matrix
		p = glm::perspective(0.6f,		// FOV
							 ratio,			// Aspect ratio
							 0.1f,			// Near clipping plane
							 100'000.f);		// Far clipping plane

		// View matrix
		v = glm::mat4(1.0f);

		v = glm::lookAt(scene->cameraEye,
						scene->cameraTarget,
						scene->upVector);


		int lastShader = -1;
		
		// Loop to draw everything in the scene
		for (int index = 0; index != scene->vecGameObjects.size(); index++)
		{
			if (index < scene->vecGameObjects.size() - 1)
			{
				glm::vec3 ObjA = scene->vecGameObjects[index]->pos;
				glm::vec3 ObjB = scene->vecGameObjects[index + 1]->pos;

				//			if ( glm::distance( ObjA, ::g_pFlyCamera->eye ) < glm::distance( ObjB, ::g_pFlyCamera->eye ) )
				if (glm::distance2(ObjA, scene->cameraEye) < glm::distance2(ObjB, scene->cameraEye))
				{
					// Out of order, so swap the positions...
					cGameObject* pTemp = scene->vecGameObjects[index];
					scene->vecGameObjects[index] = scene->vecGameObjects[index + 1];
					scene->vecGameObjects[index + 1] = pTemp;
				}
			}

			cGameObject* objPtr = scene->vecGameObjects[index];

			GLint shaderProgID = scene->Shaders[objPtr->shaderName];

			// Only switch shaders if needed
			if (lastShader != shaderProgID)
			{
				glUseProgram(shaderProgID);
				lastShader = shaderProgID;
			}

			// set time
			float time = glfwGetTime();
			glUniform1f(glGetUniformLocation(shaderProgID, "iTime"), time);

			// set resolution
			glUniform2f(glGetUniformLocation(shaderProgID, "iResolution"),
						width,
						height);

			if (objPtr->tag == "water")
			{
				glUniform1i(glGetUniformLocation(shaderProgID, "isWater"),
							true);
			}
			else
			{
				glUniform1i(glGetUniformLocation(shaderProgID, "isWater"),
							false);
			}

			

			DrawObject(objPtr, ratio, v, p);

		}//for (int index...
		// **************************************************
		// **************************************************


		for (int index = 0; index < network_client->numPlayers; ++index)
		{
			if (network_client->players[index].is_alive == 0) continue;

			cGameObject* objPtr = players[index];
			float y;
			network_client->SetPosition(index, objPtr->pos.x, objPtr->pos.z, y, delta_time);
			y = glm::radians(y);
			objPtr->setOrientation(
				glm::slerp(objPtr->getQOrientation(), glm::quat(glm::vec3(0.f, y, 0.f)), delta_time * 10.f)
			);
			objPtr->pos.y = 50.f;


			GLint shaderProgID = scene->Shaders[objPtr->shaderName];

			// Only switch shaders if needed
			if (lastShader != shaderProgID)
			{
				glUseProgram(shaderProgID);
				lastShader = shaderProgID;
			}

			/*if (objPtr->tag == "player")
			{
				glm::mat4 model = objPtr->ModelMatrix();
				light1->position = model * glm::vec4(ship->CollidePoints[0], 1.f);
				light2->position = model * glm::vec4(ship->CollidePoints[1], 1.f);
				pEffect.pos = ((light2->position - light1->position) / 2.f) + light1->position;
			}*/

			DrawObject(objPtr, ratio, v, p);
		}

		glUseProgram(scene->Shaders[sphere->shaderName]);

		// set time
		float time = glfwGetTime();
		glUniform1f(glGetUniformLocation(scene->Shaders[sphere->shaderName], "iTime"), time);

		// set resolution
		glUniform2f(glGetUniformLocation(scene->Shaders[sphere->shaderName], "iResolution"),
					width,
					height);
		for (unsigned int n = 0; n < pEffect.particles.size(); ++n)
		{
			sParticle* particle = pEffect.particles[n];
			sphere->pos = particle->pos;
			sphere->scale = particle->scale;
			DrawObject(sphere, ratio, v, p);
		}

		sphere->scale = 0.3f;
		for (unsigned int index = 0; index < network_client->projectiles.size(); ++index)
		{
			Projectile* projectile = &network_client->projectiles[index];
			if (projectile->state == 1)
			{
				projectile->previousPos = Mathf::lerp(projectile->previousPos, projectile->pos, delta_time * 2.f);
				sphere->pos = projectile->previousPos;
				DrawObject(sphere, ratio, v, p);
			}
		}


		
		//DrawOctree(ship, phys->tree->main_node, bounds, ratio, v, p);

		 // **************************************************
		// **************************************************
		renderer->RenderDebugObjects(v, p, 0.01f);




		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();


	// Delete everything
	delete scene;
	//delete lightManager;

	exit(EXIT_SUCCESS);
}


void SetUpTextureBindingsForObject(
	cGameObject* pCurrentObject,
	GLint shaderProgID)
{

	//// Tie the texture to the texture unit
	//GLuint texSamp0_UL = ::g_pTextureManager->getTextureIDFromName("Pizza.bmp");
	//glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
	//glBindTexture(GL_TEXTURE_2D, texSamp0_UL);	// Texture now assoc with texture unit 0

	// Tie the texture to the texture unit
	GLuint texSamp0_UL = textureManager->getTextureIDFromName(pCurrentObject->texture[0]);
	glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, texSamp0_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp1_UL = ::textureManager->getTextureIDFromName(pCurrentObject->texture[1]);
	glActiveTexture(GL_TEXTURE1);				// Texture Unit 1
	glBindTexture(GL_TEXTURE_2D, texSamp1_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp2_UL = ::textureManager->getTextureIDFromName(pCurrentObject->texture[2]);
	glActiveTexture(GL_TEXTURE2);				// Texture Unit 2
	glBindTexture(GL_TEXTURE_2D, texSamp2_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp3_UL = ::textureManager->getTextureIDFromName(pCurrentObject->texture[3]);
	glActiveTexture(GL_TEXTURE3);				// Texture Unit 3
	glBindTexture(GL_TEXTURE_2D, texSamp3_UL);	// Texture now assoc with texture unit 0

	// Tie the texture units to the samplers in the shader
	GLint textSamp00_UL = glGetUniformLocation(shaderProgID, "textSamp00");
	glUniform1i(textSamp00_UL, 0);	// Texture unit 0

	GLint textSamp01_UL = glGetUniformLocation(shaderProgID, "textSamp01");
	glUniform1i(textSamp01_UL, 1);	// Texture unit 1

	GLint textSamp02_UL = glGetUniformLocation(shaderProgID, "textSamp02");
	glUniform1i(textSamp02_UL, 2);	// Texture unit 2

	GLint textSamp03_UL = glGetUniformLocation(shaderProgID, "textSamp03");
	glUniform1i(textSamp03_UL, 3);	// Texture unit 3


	GLint tex0_ratio_UL = glGetUniformLocation(shaderProgID, "tex_0_3_ratio");
	glUniform4f(tex0_ratio_UL,
				pCurrentObject->textureRatio[0],		// 1.0
				pCurrentObject->textureRatio[1],
				pCurrentObject->textureRatio[2],
				pCurrentObject->textureRatio[3]);



	return;
}


void DrawObject(cGameObject* objPtr, float ratio, glm::mat4 const& v, glm::mat4 const& p)
{
	GLint shaderProgID = scene->Shaders[objPtr->shaderName];

	GLint bIsSkyBox_UL = glGetUniformLocation(shaderProgID, "isSkybox");
	if (objPtr->tag != "skybox")
	{
		glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);
		// Don't draw back facing triangles (default)
		glCullFace(GL_BACK);
		SetUpTextureBindingsForObject(objPtr, shaderProgID);
	}
	else
	{
		// Draw the back facing triangles. 
		// Because we are inside the object, so it will force a draw on the "back" of the sphere 
		glCullFace(GL_FRONT);
		//glCullFace(GL_FRONT_AND_BACK);

		glUniform1f(bIsSkyBox_UL, (float)GL_TRUE);

		GLuint skyBoxTextureID = textureManager->getTextureIDFromName("space");
		glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0

		// Tie the texture units to the samplers in the shader
		GLint skyBoxSampler_UL = glGetUniformLocation(shaderProgID, "skyBox");
		glUniform1i(skyBoxSampler_UL, 26);	// Texture unit 26
	}

	glm::mat4 m, mvp;

	m = glm::mat4(1.0f);



	// ******* TRANSLATION TRANSFORM *********
	glm::mat4 matTrans
		= glm::translate(glm::mat4(1.0f),
						 glm::vec3(objPtr->pos.x,
								   objPtr->pos.y,
								   objPtr->pos.z));
	m = m * matTrans;
	// ******* TRANSLATION TRANSFORM *********



	// ******* ROTATION TRANSFORM *********
	//mat4x4_rotate_Z(m, m, (float) glfwGetTime());
	//glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
	//								objPtr->rotation.z,					// Angle
	//								glm::vec3(0.0f, 0.0f, 1.0f));
	//m = m * rotateZ;

	//glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
	//								objPtr->rotation.y,	//(float)glfwGetTime(),					// Angle
	//								glm::vec3(0.0f, 1.0f, 0.0f));
	//m = m * rotateY;

	//glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
	//								objPtr->rotation.x,	// (float)glfwGetTime(),					// Angle
	//								glm::vec3(1.0f, 0.0f, 0.0f));
	m = m * glm::mat4(objPtr->getQOrientation());
	// ******* ROTATION TRANSFORM *********

	GLint matModelIT_UL = glGetUniformLocation(shaderProgID, "matModelInverTrans");
	glm::mat4 matModelInverseTranspose = glm::inverse(glm::transpose(m));
	glUniformMatrix4fv(matModelIT_UL, 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));


	// ******* SCALE TRANSFORM *********
	/*glm::mat4 scale = glm::scale(glm::mat4(1.0f),
								 glm::vec3(objPtr->scale,
										   objPtr->scale,
										   objPtr->scale));
	m = m * scale;*/
	// ******* SCALE TRANSFORM *********
	

	GLint matModel_UL = glGetUniformLocation(shaderProgID, "matModel");
	GLint matView_UL = glGetUniformLocation(shaderProgID, "matView");
	GLint matProj_UL = glGetUniformLocation(shaderProgID, "matProj");

	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(objPtr->ModelMatrix()));
	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(v));
	glUniformMatrix4fv(matProj_UL, 1, GL_FALSE, glm::value_ptr(p));

	

	GLint eye_loc = glGetUniformLocation(shaderProgID, "eyeLocation");
	glUniform4f(eye_loc,
				scene->cameraEye.x,
				scene->cameraEye.y,
				scene->cameraEye.z,
				1.0f
	);

	GLint diff_loc = glGetUniformLocation(shaderProgID, "diffuseColour");
	glUniform4f(diff_loc,
				objPtr->colour.x,
				objPtr->colour.y,
				objPtr->colour.z,
				objPtr->colour.w
	);

	GLint spec_loc = glGetUniformLocation(shaderProgID, "specularColour");
	glUniform4f(spec_loc,
				objPtr->specColour.x,
				objPtr->specColour.y,
				objPtr->specColour.z,
				objPtr->specIntensity
	);

	GLint isUniform_location = glGetUniformLocation(shaderProgID, "isUniform");
	glUniform1i(isUniform_location, objPtr->uniformColour);


	scene->pLightManager->Set_Light_Data(shaderProgID);



	// This will change the fill mode to something 
	//  GL_FILL is solid 
	//  GL_LINE is "wireframe"
	//glPointSize(15.0f);

	if (objPtr->isWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	sModelDrawInfo drawInfo;
	if (scene->pVAOManager->FindDrawInfoByModelName(objPtr->meshName, drawInfo))
	{
		glBindVertexArray(drawInfo.VAO_ID);
		glDrawElements(GL_TRIANGLES,
					   drawInfo.numberOfIndices,
					   GL_UNSIGNED_INT,
					   0);
		glBindVertexArray(0);
	}
}

void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, float ratio, glm::mat4 const& v, glm::mat4 const& p)
{
	if (node == nullptr) return;

	if (!node->AABB->contains(obj->pos)) return;

	glUniform1i(glGetUniformLocation(scene->Shaders[obj->shaderName], "isWater"),
				false);

	if (!node->has_nodes)
	{
		objPtr->pos = (node->AABB->min + (node->AABB->min + node->AABB->length)) / 2.f;
		objPtr->scale = node->AABB->length / 2.f;

		DrawObject(objPtr, ratio, v, p);
	}

	for (int i = 0; i < 8; ++i)
		DrawOctree(obj, node->nodes[i], objPtr, ratio, v, p);

}

void DrawParticle(sParticle* objPtr, float ratio, glm::mat4 const& v, glm::mat4 const& p)
{

}
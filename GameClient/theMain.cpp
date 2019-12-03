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

#include "cModelLoader.h"		
#include "cVAOManager.h"
#include "cGameObject.h"

#include "cShaderHelper.h"
#include "Mathf.h"
#include "PhysicsEngine.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define vertShaderFile "vertexShader01.glsl"
#define fragShaderFile "fragmentShader01.glsl"

glm::vec3 cameraEye = glm::vec3(0.0, 1.0f, -7.0);
glm::vec3 cameraTarget = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 LightLocation = glm::vec3(0.0f,0.0f,0.0f);
std::vector<cGameObject> vecGameObjects;


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void HandleInput(GLFWwindow* window)
{
	glm::vec3 forwardVector = Mathf::get_direction_vector(cameraEye, cameraTarget);
	glm::vec3 backwardsVector = Mathf::get_reverse_direction_vector(cameraEye, cameraTarget);
	glm::vec3 rightVector = Mathf::get_rotated_vector(-90.f, glm::vec3(0.f), forwardVector);
	glm::vec3 leftVector = Mathf::get_rotated_vector(90.f, glm::vec3(0.f), forwardVector);

	// Move the camera (A & D for left and right, along the x axis)
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		Mathf::rotate_vector(-5.f, cameraTarget, cameraEye);
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		Mathf::rotate_vector(5.f, cameraTarget, cameraEye);
	}

	// Move the camera (Q & E for up and down, along the y axis)
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		cameraEye.y -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		cameraEye.y += 0.1f;
	}

	// Move the camera (W & S for towards and away, along the z axis)
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		cameraEye += forwardVector * 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		cameraEye += backwardsVector * 0.1f;
	}


	if (glfwGetKey(window, GLFW_KEY_J))
	{
		LightLocation.x -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_K))
	{
		LightLocation.x += 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		vecGameObjects[1].velocity += Mathf::get_direction_vector(vecGameObjects[1].positionXYZ,
																  vecGameObjects[1].positionXYZ + forwardVector) * 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		vecGameObjects[1].velocity -= Mathf::get_direction_vector(vecGameObjects[1].positionXYZ,
																  vecGameObjects[1].positionXYZ + forwardVector) * 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		vecGameObjects[1].velocity += rightVector * 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		vecGameObjects[1].velocity += leftVector * 0.01f;
	}
}

int main(void)
{
	GLFWwindow* window;
	GLuint shaderProgID;
	//GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	//GLint mvp_location; /*, vpos_location, vcol_location;*/

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

	try
	{
		shaderProgID = cShaderHelper::BuildShaderProgram(vertShaderFile, fragShaderFile);
	}
	catch (std::exception& err)
	{
		std::cout << "Error: " << err.what() << std::endl;
		return -1;
	}

	cModelLoader* pTheModelLoader = new cModelLoader();	// Heap

	//cMesh bunnyMesh;		// This is stack based
//	if ( ! pTheModelLoader->LoadPlyModel("assets/models/Sky_Pirate_Combined_xyz.ply", bunnyMesh) )
	/*if ( ! pTheModelLoader->LoadPlyModel("assets/models/bun_zipper_res4_XYZ.ply", bunnyMesh) )
	{
		std::cout << "Didn't find the file" << std::endl;
	}*/

	cMesh sphereMesh;
	pTheModelLoader->LoadPlyModel("assets/models/sphere_XYZ.ply", sphereMesh);

	cMesh cubeMesh;
	pTheModelLoader->LoadPlyModel("assets/models/cube_XYZ.ply", cubeMesh);

	

	// Create a VAO Manager...
	// #include "cVAOManager.h"  (at the top of your file)
	cVAOManager* pTheVAOManager = new cVAOManager();

	// Note, the "filename" here is really the "model name" 
	//  that we can look up later (i.e. it doesn't have to be the file name)
	

	sModelDrawInfo drawInfoCube;
	pTheVAOManager->LoadModelIntoVAO("cube",
									 cubeMesh,
									 drawInfoCube,
									 shaderProgID);

	sModelDrawInfo drawInfoSphere;
	pTheVAOManager->LoadModelIntoVAO("sphere",
									 sphereMesh,
									 drawInfoSphere,
									 shaderProgID);

	// At this point, the model is loaded into the GPU


	// Load up my "scene" 


	cGameObject sphere;
	sphere.meshName = "sphere";
	sphere.positionXYZ = glm::vec3(0.0f, 5.0f, 0.0f);
	sphere.rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	sphere.scale = 0.5f;
	sphere.objectColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Set the sphere's initial velocity, etc.
	sphere.velocity = glm::vec3(-0.1f, 0.f, 0.f);
	sphere.acceleration = glm::vec3(0.f);
	sphere.inverseMass = 1.f;

	cGameObject cube;
	cube.meshName = "cube";
	cube.positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	cube.rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	cube.scale = 0.5f;
	cube.objectColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// Set the cube's initial velocity, etc.
	cube.inverseMass = 0.f;	// does not move




	vecGameObjects.push_back(cube);
	vecGameObjects.push_back(sphere);

	//mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);


	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing

	float shipVelocityZ = 0.1f;
	float shipAccelz = 100.f;

	while (!glfwWindowShouldClose(window))
	{
		// Handle key inputs
		HandleInput(window);

		float ratio;
		int width, height;
		glm::mat4 m, p, v, mvp;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		// Projection matrix
		p = glm::perspective(0.6f,		// FOV
							 ratio,			// Aspect ratio
							 0.1f,			// Near clipping plane
							 1000.0f);		// Far clipping plane

		// View matrix
		v = glm::mat4(1.0f);

		v = glm::lookAt(cameraEye,
						cameraTarget,
						upVector);


		glViewport(0, 0, width, height);

		// Clear both the colour buffer (what we see) and the 
		//  depth (or z) buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Update the objects' physics
		PhysicsUpdate(vecGameObjects, 0.1f);

		// **************************************************
		// **************************************************
		// Loop to draw everything in the scene
		for (int index = 0; index != vecGameObjects.size(); index++)
		{

			//         mat4x4_identity(m);
			m = glm::mat4(1.0f);



			// ******* TRANSLATION TRANSFORM *********
			glm::mat4 matTrans 
				= glm::translate(glm::mat4(1.0f),
								 glm::vec3(vecGameObjects[index].positionXYZ.x,
										   vecGameObjects[index].positionXYZ.y,
										   vecGameObjects[index].positionXYZ.z));
			m = m * matTrans;
			// ******* TRANSLATION TRANSFORM *********



			// ******* ROTATION TRANSFORM *********
			//mat4x4_rotate_Z(m, m, (float) glfwGetTime());
			glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
				vecGameObjects[index].rotationXYZ.z,					// Angle 
				glm::vec3(0.0f, 0.0f, 1.0f));
			m = m * rotateZ;

			glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
				vecGameObjects[index].rotationXYZ.y,	//(float)glfwGetTime(),					// Angle 
				glm::vec3(0.0f, 1.0f, 0.0f));
			m = m * rotateY;

			glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
				vecGameObjects[index].rotationXYZ.x,	// (float)glfwGetTime(),					// Angle 
				glm::vec3(1.0f, 0.0f, 0.0f));
			m = m * rotateX;
			// ******* ROTATION TRANSFORM *********



			// ******* SCALE TRANSFORM *********
			glm::mat4 scale = glm::scale(glm::mat4(1.0f),
										 glm::vec3(vecGameObjects[index].scale, 
												   vecGameObjects[index].scale,
												   vecGameObjects[index].scale));
			m = m * scale;
			// ******* SCALE TRANSFORM *********



			//mat4x4_mul(mvp, p, m);
			mvp = p * v * m;

			// Choose which shader to use
			//glUseProgram(program);
			glUseProgram(shaderProgID);


			//glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
			//glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

			//uniform mat4 matModel;		// Model or World 
			//uniform mat4 matView; 		// View or camera
			//uniform mat4 matProj;
			GLint matModel_UL = glGetUniformLocation(shaderProgID, "matModel");
			GLint matView_UL = glGetUniformLocation(shaderProgID, "matView");
			GLint matProj_UL = glGetUniformLocation(shaderProgID, "matProj");
	
			glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(m) );
			glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(v) );
			glUniformMatrix4fv(matProj_UL, 1, GL_FALSE, glm::value_ptr(p) );



			// Find the location of the uniform variable newColour
			GLint newColour_location = glGetUniformLocation(shaderProgID, "newColour");

			glUniform3f(newColour_location, 
						vecGameObjects[index].objectColourRGBA.r, 
						vecGameObjects[index].objectColourRGBA.g,
						vecGameObjects[index].objectColourRGBA.b);

			//uniform float newColourRed;
			//uniform float newColourGreen;
			//uniform float newColourBlue;
			GLint newColourRed_UL = glGetUniformLocation(shaderProgID, "newColourRed");
			GLint newColourGreen_UL = glGetUniformLocation(shaderProgID, "newColourGreen");
			GLint newColourBlue_UL = glGetUniformLocation(shaderProgID, "newColourBlue");

			glUniform1f(newColourRed_UL, vecGameObjects[index].objectColourRGBA.r);
			glUniform1f(newColourGreen_UL, vecGameObjects[index].objectColourRGBA.g);
			glUniform1f(newColourBlue_UL, vecGameObjects[index].objectColourRGBA.b);


			GLint lighPosition_UL = glGetUniformLocation( shaderProgID, "lightPosition");
			glUniform3f(lighPosition_UL, LightLocation.x, 
						LightLocation.y, LightLocation.z );



			// This will change the fill mode to something 
			//  GL_FILL is solid 
			//  GL_LINE is "wireframe"
			//glPointSize(15.0f);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	//		glDrawArrays(GL_TRIANGLES, 0, 2844);
	//		glDrawArrays(GL_TRIANGLES, 0, numberOfVertsOnGPU);

			sModelDrawInfo drawInfo;
			//if (pTheVAOManager->FindDrawInfoByModelName("bunny", drawInfo))
			if (pTheVAOManager->FindDrawInfoByModelName(vecGameObjects[index].meshName, drawInfo))
			{
				glBindVertexArray(drawInfo.VAO_ID);
				glDrawElements(GL_TRIANGLES,
							   drawInfo.numberOfIndices,
							   GL_UNSIGNED_INT,
							   0);			
				glBindVertexArray(0);
			}

		}//for (int index...

		 // **************************************************
		// **************************************************




		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();


	// Delete everything
	delete pTheModelLoader;
//	delete pTheVAOManager;

	// Watch out!!
	// sVertex* pVertices = new sVertex[numberOfVertsOnGPU];
//	delete [] pVertices;		// If it's an array, also use [] bracket

	exit(EXIT_SUCCESS);
}

#pragma once

#include <vector>
#include <string>
#include "GLCommon.h"
#include <glm/vec3.hpp>
#include <map>

class cGameObject;
class cVAOManager;
class cModelLoader;
class AudioEngine;
class cLightManager;

class Scene
{
private:
	typedef			std::vector<cGameObject*>	object_list;
	typedef			glm::vec3					vec3;
public:
	Scene() {}
	~Scene();

	static Scene*	LoadFromXML(std::string filename);

	std::string		SceneName;

	std::map<std::string, unsigned int>	Shaders;

	object_list		vecGameObjects;
	cVAOManager*	pVAOManager;
	cModelLoader*	pModelLoader;
	AudioEngine*	pAudioEngine;
	cLightManager*	pLightManager;

	vec3			cameraEye =		vec3(0.0f, 0.0f, -10.0);
	vec3			cameraTarget =	vec3(0.0f, 0.0f, 0.0f);
	vec3			upVector =		vec3(0.0f, 1.0f, 0.0f);


	bool SaveToFile();
	bool SaveLights();
	bool SaveLayout();
	bool SaveAudio();
};
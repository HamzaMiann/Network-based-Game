#pragma once

#include "iInputHandler.h"
#include <vector>
#include <string>

class AudioEngine;

class cAudioInputHandler : public iInputHandler
{
private:
	AudioEngine* engine;
	int current_sound_id = 0;
	std::vector<std::string> const& groups;
public:
	cAudioInputHandler();
	virtual ~cAudioInputHandler() {}
	virtual void HandleInput(GLFWwindow* window);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

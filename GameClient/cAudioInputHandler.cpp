
#include "cAudioInputHandler.h"
#include "Scene.h"
#include "AudioEngine.hpp"
#include <sstream>

std::map<std::string, FMOD::DSP*>::iterator dsp_it;

static std::string get_sound_details(AudioEngine::SoundGroup* sound_group, std::string friendlyName)
{
	std::ostringstream ss;

	AudioEngine::Sound* sound = sound_group->sounds[0];
	if (!sound) return "";

	ss << "[GROUP: " << sound_group->name << "] ";
	/*unsigned int seconds = sound->get_position() / 1000;
	unsigned int minutes = seconds / 60;
	unsigned int sec = seconds - (minutes * 60);
	unsigned int size = sound->get_size() / 1000;
	unsigned int size_minutes = size / 60;
	unsigned int size_sec = size - (size_minutes * 60);*/
	//ss << "format(" << sound->get_format() << ") ";
	//ss << "type(" << sound->get_type() << ") ";
	//ss << "[" << minutes << ":" << sec << " / " << size_minutes << ":" << size_sec << "] ";
	ss << "Vol(" << sound->get_volume() << ") ";
	//ss << "Fq(" << sound->get_frequency() << ") ";
	ss << "Pitch(" << sound->get_pitch() << ") ";
	ss << "Pan(" << sound->get_pan() << ") ";
	ss << "DSP(" << dsp_it->first << ") ";

	return ss.str();
}

cAudioInputHandler::cAudioInputHandler() : groups(AudioEngine::Instance()->GetGroupNames())
{
	this->engine = AudioEngine::Instance();
	dsp_it = engine->DSPs.begin();
}

void cAudioInputHandler::HandleInput(GLFWwindow* window)
{
	if (groups.size() > 0)
	{
		AudioEngine::Sound* current_sound = engine->GetGroup(groups[current_sound_id])->sounds[0];

		if (glfwGetKey(window, GLFW_KEY_1))
		{
			current_sound->set_volume(current_sound->get_volume() - 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_2))
		{
			current_sound->set_volume(current_sound->get_volume() + 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_3))
		{
			current_sound->set_pitch(current_sound->get_pitch() - 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_4))
		{
			current_sound->set_pitch(current_sound->get_pitch() + 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_5))
		{
			current_sound->set_pan(current_sound->get_pan() - 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_6))
		{
			current_sound->set_pan(current_sound->get_pan() + 0.01f);
		}

		//std::string friendlyName = engine->Get_Name(current_sound_id);
		glfwSetWindowTitle(window, get_sound_details(engine->GetGroup(groups[current_sound_id]), "").c_str());

	}

}


void cAudioInputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (groups.size() > 0)
	{
		int max = groups.size();

		if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		{
			engine->GetGroup(groups[current_sound_id])->remove_DSPs();
			dsp_it = engine->DSPs.begin();

			current_sound_id++;
			if (current_sound_id >= max) current_sound_id = 0;
			
		}

		if (key == GLFW_KEY_N && action == GLFW_PRESS)
		{
			engine->GetGroup(groups[current_sound_id])->toggle_pause();
		}

		if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			dsp_it++;
			if (dsp_it == engine->DSPs.end())
				dsp_it = engine->DSPs.begin();

			engine->GetGroup(groups[current_sound_id])->apply_DSP((dsp_it)->second);
		}

	}

}
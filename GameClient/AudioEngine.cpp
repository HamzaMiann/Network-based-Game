#include "AudioEngine.hpp"
#include "fmod_helper.h"
#include <glm/glm.hpp>
#undef PlaySound

void exit_on_failure(FMOD_RESULT status)
{
	if (status != FMOD_OK)
	{
		std::cout << "exit_on_failure FMOD ERROR: " << status << std::endl;
		exit(1);
	}
}

void AudioEngine::Init()
{
	status = FMOD::System_Create(&system);
	exit_on_failure(status);

	status = system->init(NUM_CHANNELS, FMOD_INIT_NORMAL, 0);
	exit_on_failure(status);

	status = system->set3DSettings(1.0f, 1.0f, 1.0f);
	exit_on_failure(status);

	// LOAD DSPs

	status = system->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &DSPs["reverb"]);
	exit_on_failure(status);

	status = system->createDSPByType(FMOD_DSP_TYPE_ECHO, &DSPs["echo"]);
	exit_on_failure(status);

	status = system->createDSPByType(FMOD_DSP_TYPE_CHORUS, &DSPs["chorus"]);
	exit_on_failure(status);

	status = system->createDSPByType(FMOD_DSP_TYPE_FLANGE, &DSPs["flanger"]);
	exit_on_failure(status);

	status = system->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &DSPs["distortion"]);
	exit_on_failure(status);

	status = system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &DSPs["tremolo"]);
	exit_on_failure(status);

	status = system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &DSPs["high_pass"]);
	exit_on_failure(status);

	status = system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &DSPs["low_pass"]);
	exit_on_failure(status);

}

AudioEngine::~AudioEngine()
{
	// clear all sounds (destructor will handle the memory cleanup)
	for (size_t i = 0; i < Sounds.size(); ++i)
	{
		delete Sounds[i];
	}
	Sounds.clear();
	SoundNames.clear();

	// remove sound groups
	std::map<std::string, SoundGroup*>::iterator it = Groups.begin();
	while (it != Groups.end())
	{
		for (size_t i = 0; i < it->second->sounds.size(); ++i)
		{
			delete it->second->sounds[i];
		}
		it->second->sounds.clear();
		delete it->second;
		it++;
	}
	Groups.clear();

	// clear DSPs
	std::map<std::string, FMOD::DSP*>::iterator dsp_it = DSPs.begin();
	while (dsp_it != DSPs.end())
	{
		dsp_it->second->release();
		dsp_it++;
	}
	DSPs.clear();

	// release system
	if (system)
	{
		// close the system
		status = system->close();
		exit_on_failure(status);

		// release the system
		status = system->release();
		exit_on_failure(status);
	}
}


AUDIO_ID AudioEngine::Create_Sound(std::string filename, std::string friendlyName, bool streamed)
{
	std::string fullPath = "assets\\audio\\" + filename;
	FMOD::Sound* sound = 0;
	FMOD::Channel* channel = 0;
	status = system->createSound(fullPath.c_str(), streamed? FMOD_CREATESTREAM : FMOD_DEFAULT, 0, &sound);
	exit_on_failure(status);

	AudioEngine::Sound* newSound = new AudioEngine::Sound(channel, sound);
	status = system->playSound(newSound->_sound, FMOD_DEFAULT, true, &(newSound->_channel));
	exit_on_failure(status);

	newSound->_paused = true;
	newSound->_streamed = streamed;

	Sounds.push_back(newSound);
	SoundNames.push_back(friendlyName);

	return this->Sounds.size() - 1;
}

AUDIO_ID AudioEngine::Create_Sound_Group(std::string filename, std::string friendlyName, std::string group)
{
	std::string fullPath = "assets\\audio\\" + filename;
	FMOD::Sound* sound = 0;
	FMOD::Channel* channel = 0;
	FMOD::ChannelGroup* channelGroup = 0;
	if (!Groups[group])
	{
		GroupNames.push_back(group);
		Groups[group] = new SoundGroup;
		Groups[group]->name = group;
		status = system->createChannelGroup(group.c_str(), &Groups[group]->channel);
		exit_on_failure(status);
	}
	channelGroup = Groups[group]->channel;
	status = system->createSound(fullPath.c_str(), FMOD_CREATESTREAM, 0, &sound);
	exit_on_failure(status);

	AudioEngine::Sound* newSound = new AudioEngine::Sound(channel, channelGroup, sound);
	status = system->playSound(newSound->_sound, Groups[group]->channel, false, &(newSound->_channel));
	exit_on_failure(status);

	newSound->_paused = true;
	newSound->_streamed = true;

	status = newSound->_channelGroup->setPaused(newSound->_paused);
	exit_on_failure(status);

	Groups[group]->sounds.push_back(newSound);

	return 0;
}


AUDIO_ID AudioEngine::Create_Sound_3d(std::string filename, std::string friendlyName, cGameObject* attach_to)
{
	std::string fullPath = "assets\\audio\\" + filename;
	FMOD::Sound* sound = 0;
	FMOD::Channel* channel = 0;
	status = system->createSound(fullPath.c_str(), FMOD_LOOP_NORMAL | FMOD_3D, 0, &sound);
	exit_on_failure(status);

	AudioEngine::Sound* newSound = new AudioEngine::Sound(channel, sound, attach_to);
	status = system->playSound(newSound->_sound, FMOD_DEFAULT, true, &(newSound->_channel));
	exit_on_failure(status);

	newSound->_paused = true;
	newSound->_streamed = false;

	Sounds.push_back(newSound);
	SoundNames.push_back(friendlyName);

	this->Sounds_3d.push_back(this->Sounds.size() - 1);
	return this->Sounds.size() - 1;
}


void AudioEngine::PlaySound(Sound* sound)
{
	if (sound->_channel)
	{
		sound->reset_position();
		status = sound->_channel->getPaused(&sound->_paused);
		exit_on_failure(status);
		if (sound->_paused)
		{
			status = sound->_channel->setPaused(!sound->_paused);
			exit_on_failure(status);
			sound->_paused = false;
		}
		return;
	}
	if (sound->_channelGroup)
	{
		sound->reset_position();
		status = sound->_channelGroup->getPaused(&sound->_paused);
		exit_on_failure(status);
		if (sound->_paused)
		{
			status = sound->_channelGroup->setPaused(!sound->_paused);
			exit_on_failure(status);
			sound->_paused = false;
		}
		return;
	}
}

void AudioEngine::PlaySound(int sound_id)
{
	AudioEngine::Sound* sound = Sounds[sound_id];
	if (sound->_channel)
	{
		sound->reset_position();
		status = sound->_channel->getPaused(&sound->_paused);
		exit_on_failure(status);
		if (sound->_paused)
		{
			status = sound->_channel->setPaused(!sound->_paused);
			exit_on_failure(status);
			sound->_paused = false;
		}
		return;
	}
	if (sound->_channelGroup)
	{
		sound->reset_position();
		status = sound->_channelGroup->getPaused(&sound->_paused);
		exit_on_failure(status);
		if (sound->_paused)
		{
			status = sound->_channelGroup->setPaused(!sound->_paused);
			exit_on_failure(status);
			sound->_paused = false;
		}
		return;
	}
}

void AudioEngine::PlaySound(std::string friendlyName)
{
	for (int i = 0; i < SoundNames.size(); ++i)
	{
		if (SoundNames[i] == friendlyName)
		{
			AudioEngine::Sound* sound = Sounds[i];
			if (sound->_channel)
			{
				sound->reset_position();
				status = sound->_channel->getPaused(&sound->_paused);
				exit_on_failure(status);
				if (sound->_paused)
				{
					status = sound->_channel->setPaused(!sound->_paused);
					exit_on_failure(status);
					sound->_paused = false;
				}
			}
		}
	}
}

void AudioEngine::PlayGroup(std::string friendlyName)
{
	for (std::map<std::string, SoundGroup*>::iterator i = Groups.begin(); i != Groups.end(); ++i)
	{
		if (i->first == friendlyName && i->second)
		{
			AudioEngine::Sound* sound = i->second->sounds[0];
			if (sound->_channelGroup)
			{
				sound->reset_position();
				status = sound->_channelGroup->getPaused(&sound->_paused);
				exit_on_failure(status);
				if (sound->_paused)
				{
					status = sound->_channelGroup->setPaused(!sound->_paused);
					exit_on_failure(status);
					sound->_paused = false;
				}
				i->second->_paused = sound->_paused;
			}
			break;
		}
	}
}

AudioEngine::Sound* AudioEngine::GetSound(AUDIO_ID sound_id)
{
	if (sound_id < 0 && sound_id >= this->Sounds.size()) return nullptr;
	return this->Sounds[sound_id];
}

AudioEngine::Sound* AudioEngine::GetSound(std::string friendlyName)
{
	for (int i = 0; i < SoundNames.size(); ++i)
	{
		if (SoundNames[i] == friendlyName)
			return Sounds[i];
	}
	return nullptr;
}


void AudioEngine::Update3d(glm::vec3 const& camera, glm::vec3 const& target, float delta_time)
{
	listener.previousLoc = listener.location;
	listener.location = { -camera.x, camera.y, camera.z };
	listener.velocity = {
		(listener.location.x - listener.previousLoc.x) / delta_time,
		(listener.location.y - listener.previousLoc.y) / delta_time,
		(listener.location.z - listener.previousLoc.z) / delta_time
	};
	glm::vec3 forward = glm::normalize(target - camera);
	forward.y = 0.f;
	forward = glm::normalize(forward);
	listener.direction = { forward.x, forward.y, forward.z };
	status = system->set3DListenerAttributes(0, &listener.location, &listener.velocity, &listener.direction, &listener.up);
	exit_on_failure(status);
	for (unsigned int i = 0; i < Sounds_3d.size(); ++i)
	{
		this->Sounds[Sounds_3d[i]]->update_3d();
	}
	status = system->update();
	exit_on_failure(status);
}
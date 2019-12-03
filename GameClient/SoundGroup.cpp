

#include "AudioEngine.hpp"
#include "fmod_helper.h"

void __exit_on_failure(FMOD_RESULT status)
{
	if (status != FMOD_OK)
	{
		std::cout << "__exit_on_failure FMOD ERROR: " << status << std::endl;
		exit(1);
	}
}

void AudioEngine::SoundGroup::toggle_pause()
{
	AudioEngine::Sound* sound = this->sounds[1];
	if (channel)
	{
		FMOD_RESULT status = this->channel->getPaused(&sound->_paused);
		__exit_on_failure(status);
		status = channel->setPaused(!sound->_paused);
		__exit_on_failure(status);
		sound->_paused = !sound->_paused;
		this->_paused = sound->_paused;
	}
}

bool AudioEngine::SoundGroup::is_paused()
{
	return this->_paused;
}

void AudioEngine::SoundGroup::apply_DSP(FMOD::DSP* dsp)
{
	if (dsp && channel)
	{
		FMOD::DSP* _dsp = 0;
		__exit_on_failure(channel->getDSP(0, &_dsp));
		if (_dsp)
		{
			__exit_on_failure(channel->removeDSP(_dsp));
		}
		__exit_on_failure(channel->addDSP(0, dsp));
	}
}

void AudioEngine::SoundGroup::remove_DSPs()
{
	if (channel)
	{
		FMOD::DSP* _dsp = 0;
		__exit_on_failure(channel->getDSP(0, &_dsp));
		if (_dsp)
		{
			__exit_on_failure(channel->removeDSP(_dsp));
		}
	}
}
#include "fmod_helper.h"
#include "AudioEngine.hpp"
#include "cGameObject.h"

void _exit_on_failure(FMOD_RESULT status)
{
	if (status != FMOD_OK)
	{
		std::cout << "_exit_on_failure FMOD ERROR: " << status << std::endl;
		exit(1);
	}
}

AudioEngine::Sound::Sound(FMOD::Channel* _channel_init, FMOD::Sound* _sound_init)
{
	this->_channel = _channel_init;
	this->_sound = _sound_init;
	this->_paused = true;
	set_pitch(this->_pitch);
	set_volume(this->_volume);
	set_pan(this->_pan);
}

AudioEngine::Sound::Sound(FMOD::Channel* _channel_init, FMOD::ChannelGroup* _channel_group_init, FMOD::Sound* _sound_init)
{
	this->_channel = _channel_init;
	this->_channelGroup = _channel_group_init;
	this->_sound = _sound_init;
	this->_paused = true;
	set_pitch(this->_pitch);
	set_volume(this->_volume);
	set_pan(this->_pan);
}

AudioEngine::Sound::Sound(FMOD::Channel* _channel_init, FMOD::Sound* _sound_init, cGameObject* attach_to)
{
	this->_channel = _channel_init;
	this->_sound = _sound_init;
	this->_paused = true;
	set_pitch(this->_pitch);
	set_volume(this->_volume);
	set_pan(this->_pan);

	_details_3d._attached = attach_to;
	_details_3d._pos = { _details_3d._attached->pos.x, _details_3d._attached->pos.y, _details_3d._attached->pos.z };
	_details_3d._velocity = {
		_details_3d._attached->GetVelocity().x,
		_details_3d._attached->GetVelocity().y,
		_details_3d._attached->GetVelocity().z
	};
}

AudioEngine::Sound::~Sound()
{
	if (_sound)
	{
		FMOD_RESULT status = _sound->release();
		_exit_on_failure(status);
	}
}

bool AudioEngine::Sound::update_3d()
{
	if (_details_3d._attached)
	{
		_details_3d._pos = { -_details_3d._attached->pos.x, _details_3d._attached->pos.y, _details_3d._attached->pos.z };
		_details_3d._velocity = {
			_details_3d._attached->GetVelocity().x,
			_details_3d._attached->GetVelocity().y,
			_details_3d._attached->GetVelocity().z };
		if (_channelGroup)
		{
			FMOD_RESULT status = _channelGroup->set3DAttributes(&_details_3d._pos, &_details_3d._velocity);
			_exit_on_failure(status);
			return true;
		}
		if (_channel)
		{
			FMOD_RESULT status = _channel->set3DAttributes(&_details_3d._pos, &_details_3d._velocity);
			_exit_on_failure(status);
			return true;
		}
		
	}

	return false;
}

float AudioEngine::Sound::get_volume()
{
	return _volume;
}
bool AudioEngine::Sound::set_volume(float volume)
{
	if (volume <= 1.0f && volume > 0.f && this->_channelGroup)
	{
		FMOD_RESULT status = _channelGroup->setVolume(volume);
		_exit_on_failure(status);
		_volume = volume;
		return true;
	}
	if (volume <= 1.0f && volume > 0.f && this->_channel)
	{
		FMOD_RESULT status = _channel->setVolume(volume);
		_exit_on_failure(status);
		_volume = volume;
		return true;
	}
	
	return false;
}


float AudioEngine::Sound::get_pitch()
{
	return _pitch;
}
bool AudioEngine::Sound::set_pitch(float pitch)
{
	if (pitch <= 2.0f && pitch > 0.01f && this->_channelGroup)
	{
		FMOD_RESULT status = _channelGroup->setPitch(pitch);
		_exit_on_failure(status);
		_pitch = pitch;
		return true;
	}
	if (pitch <= 2.0f && pitch > 0.01f && this->_channel)
	{
		FMOD_RESULT status = _channel->setPitch(pitch);
		_exit_on_failure(status);
		_pitch = pitch;
		return true;
	}
	
	return false;
}

float AudioEngine::Sound::get_pan()
{
	return _pan;
}
bool AudioEngine::Sound::set_pan(float pan)
{
	if (pan <= 1.0f && pan >= -1.f && this->_channelGroup)
	{
		FMOD_RESULT status = _channelGroup->setPan(pan);
		_exit_on_failure(status);
		_pan = pan;
		return true;
	}
	if (pan <= 1.0f && pan >= -1.f && this->_channel)
	{
		FMOD_RESULT status = _channel->setPan(pan);
		_exit_on_failure(status);
		_pan = pan;
		return true;
	}
	
	return false;
}

float AudioEngine::Sound::get_frequency()
{
	if (this->_channel)
	{
		FMOD_RESULT status = _channel->getFrequency(&_frequency);
		_exit_on_failure(status);
	}
	return _frequency;
}
bool AudioEngine::Sound::set_frequency(float frequency)
{
	if (frequency <= 1.0f && frequency >= -1.f && this->_channel)
	{
		FMOD_RESULT status = _channel->setFrequency(frequency);
		_exit_on_failure(status);
		_frequency = frequency;
		return true;
	}
	return false;
}

unsigned int AudioEngine::Sound::get_position()
{
	unsigned int position = 0.f;
	if (this->_channel)
	{
		FMOD_RESULT status = _channel->getPosition(&position, FMOD_TIMEUNIT_MS);
		_exit_on_failure(status);
	}
	return position;
}

bool AudioEngine::Sound::reset_position()
{
	if (this->_channel)
	{
		FMOD_RESULT status = _channel->setPosition(0, FMOD_TIMEUNIT_MS);
		_exit_on_failure(status);
		return true;
	}
	return false;
}

unsigned int AudioEngine::Sound::get_size()
{
	unsigned int size = 0.f;
	if (this->_sound)
	{
		FMOD_RESULT status = _sound->getLength(&size, FMOD_TIMEUNIT_MS);
		_exit_on_failure(status);
	}
	return size;
}

bool AudioEngine::Sound::is_paused()
{
	if (_channelGroup)
	{
		FMOD_RESULT status = _channelGroup->getPaused(&_paused);
		_exit_on_failure(status);
	}
	else if (_channel)
	{
		FMOD_RESULT status = _channel->getPaused(&_paused);
		_exit_on_failure(status);
	}
	
	return _paused;
}

void AudioEngine::Sound::toggle_pause()
{
	if (_channelGroup)
	{
		FMOD_RESULT status = _channelGroup->setPaused(!_paused);
		_exit_on_failure(status);
		is_paused();
	}
	else if (_channel)
	{
		FMOD_RESULT status = _channel->setPaused(!_paused);
		_exit_on_failure(status);
		is_paused();
	}
}

bool AudioEngine::Sound::is_streamed()
{
	return _streamed;
}

std::string AudioEngine::Sound::get_name()
{
	char name[BUFFER_SIZE] = { '\0' };
	if (_sound)
	{
		FMOD_RESULT status = _sound->getName(name, BUFFER_SIZE);
		_exit_on_failure(status);
	}
	return name;
}

std::string AudioEngine::Sound::get_format()
{
	FMOD_SOUND_TYPE type;
	FMOD_SOUND_FORMAT format;
	int number_of_channels;
	int number_of_bits_per_sample;
	if (_sound)
	{
		FMOD_RESULT status = _sound->getFormat(&type, &format, &number_of_channels, &number_of_bits_per_sample);
		_exit_on_failure(status);
	}
	switch (format)
	{
	case FMOD_SOUND_FORMAT_NONE: return "NONE";
	case FMOD_SOUND_FORMAT_PCM8: return "PCM8";
	case FMOD_SOUND_FORMAT_PCM16: return "PCM16";
	case FMOD_SOUND_FORMAT_PCM24: return "PCM24";
	case FMOD_SOUND_FORMAT_PCM32: return "PCM32";
	case FMOD_SOUND_FORMAT_PCMFLOAT: return "PCMFLOAT";
	case FMOD_SOUND_FORMAT_BITSTREAM: return "BITSTREAM";
	case FMOD_SOUND_FORMAT_MAX: return "MAX";
	case FMOD_SOUND_FORMAT_FORCEINT: return "FORCEINT";
	default:
		return "";
	}
}

std::string AudioEngine::Sound::get_type()
{
	FMOD_SOUND_TYPE type;
	FMOD_SOUND_FORMAT format;
	int number_of_channels;
	int number_of_bits_per_sample;
	if (_sound)
	{
		FMOD_RESULT status = _sound->getFormat(&type, &format, &number_of_channels, &number_of_bits_per_sample);
		_exit_on_failure(status);
	}
	switch (type)
	{
	case FMOD_SOUND_TYPE_UNKNOWN: return "UNKNOWN";
	case FMOD_SOUND_TYPE_AIFF: return "AIFF";
	case FMOD_SOUND_TYPE_ASF: return "ASF";
	case FMOD_SOUND_TYPE_DLS: return "DLS";
	case FMOD_SOUND_TYPE_FLAC: return "FLAC";
	case FMOD_SOUND_TYPE_FSB: return "FSB";
	case FMOD_SOUND_TYPE_IT: return "IT";
	case FMOD_SOUND_TYPE_MIDI: return "MIDI";
	case FMOD_SOUND_TYPE_MOD: return "MOD";
	case FMOD_SOUND_TYPE_MPEG: return "MPEG";
	case FMOD_SOUND_TYPE_OGGVORBIS: return "OGGVORBIS";
	case FMOD_SOUND_TYPE_PLAYLIST: return "PLAYLIST";
	case FMOD_SOUND_TYPE_RAW: return "RAW";
	case FMOD_SOUND_TYPE_S3M: return "S3M";
	case FMOD_SOUND_TYPE_USER: return "USER";
	case FMOD_SOUND_TYPE_WAV: return "WAV";
	case FMOD_SOUND_TYPE_XM: return "XM";
	case FMOD_SOUND_TYPE_XMA: return "XMA";
	case FMOD_SOUND_TYPE_AUDIOQUEUE: return "AUDIOQUEUE";
	case FMOD_SOUND_TYPE_AT9: return "AT9";
	case FMOD_SOUND_TYPE_VORBIS: return "VORBIS";
	case FMOD_SOUND_TYPE_MEDIA_FOUNDATION: return "MEDIA_FOUNDATION";
	case FMOD_SOUND_TYPE_MEDIACODEC: return "MEDIACODEC";
	case FMOD_SOUND_TYPE_FADPCM: return "FADPCM";
	case FMOD_SOUND_TYPE_MAX: return "MAX";
	case FMOD_SOUND_TYPE_FORCEINT: return "FORCEINT";
	default:
		return "";
	}
}

#include "Root/Root.h"
#include <algorithm>

namespace Orin
{
	SoundEvent::SoundEvent(FMOD::Studio::EventInstance* setInstance)
	{
		instance = setInstance;
	}

	bool SoundEvent::Play()
	{
		if (playing)
		{
			Stop();
		}

		playing = true;		

		instance->start();
    
		return true;
	}

	bool SoundEvent::IsPlaying()
	{
		return playing;
	}

	void SoundEvent::Pause(bool pause)
	{
		if (playing && paused != pause)
		{
			paused = pause;
			instance->setPaused(paused);
		}
	}

	void SoundEvent::Stop()
	{
		if (playing)
		{
			instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			playing = false;
		}
	}

	void SoundEvent::SetVolume(float set_volume)
	{
		volume = set_volume;

		if (playing)
		{
			instance->setVolume(volume * root.sounds.masterVolume);
		}
	}

	float SoundEvent::GetVolume()
	{
		return volume;
	}

	bool SoundEvent::Set3DAttributes(Math::Vector3 pos)
	{
		if (instance)
		{
			FMOD_3D_ATTRIBUTES attributes = {0};
			attributes.position = { pos.x, pos.z, pos.y };
			attributes.forward = {0.f, 0.f, 1.f};
			attributes.up = {0.f, 1.f, 0.f};
			attributes.velocity = {0.f, 0.f, 0.f};
			return instance->set3DAttributes(&attributes) == FMOD_OK;
		}
		return false;
	}

	void SoundEvent::Release()
	{
		Stop();

		if (instance)
		{
			instance->release();
			instance = nullptr;
		}		

		delete this;
	}
}
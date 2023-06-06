
#include "Root/Root.h"
#include <algorithm>

namespace Orin
{
	bool SoundEvent::Play(PlayEventType type)
	{
		if (playing)
		{
			Stop();
		}

		playType = type;
		playing = true;

		res->descriptor->createInstance(&instance);

		instance->start();
		//root.sounds.system->playSound(res->sound, 0, true, &channel);

		//channel->setLoopCount(type == PlaySoundType::Looped ? -1 : 0);
		//channel->setVolume(volume);
		//channel->setPaused(false);
    
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

	void SoundEvent::Release()
	{
		Stop();

		if (root.sounds.DecRef(res))
		{
			res->Release();
		}

		auto iterator = eastl::find(root.sounds.soundsEvents.begin(), root.sounds.soundsEvents.end(), this);
		root.sounds.soundsEvents.erase(iterator);

		delete this;
	}
}

#include "Root/Root.h"
#include "SoundInstance.h"

#include <algorithm>

namespace Orin
{
	bool SoundInstance::Play(PlaySoundType type)
	{
		if (playing)
		{
			Stop();
		}

		playType = type;
		playing = true;

		root.sounds.coreSystem->playSound(res->sound, 0, true, &channel);

		channel->setLoopCount(type == PlaySoundType::Looped ? -1 : 0);
		channel->setVolume(volume);
		channel->setPaused(false);
    
		return true;
	}

	bool SoundInstance::IsPlaying()
	{
		return playing;
	}

	void SoundInstance::Pause(bool pause)
	{
		if (playing && paused != pause)
		{
			paused = pause;
			channel->setPaused(paused);
		}
	}

	void SoundInstance::Stop()
	{
		if (playing)
		{
			channel->stop();
			playing = false;
		}
	}

	void SoundInstance::SetVolume(float set_volume)
	{
		volume = set_volume;

		if (playing)
		{
			channel->setVolume(volume * root.sounds.masterVolume);
		}
	}

	float SoundInstance::GetVolume()
	{
		return volume;
	}

	void SoundInstance::Release()
	{
		Stop();

		if (root.sounds.DecRef(res))
		{
			res->Release();
		}

		auto iterator = eastl::find(root.sounds.sounds.begin(), root.sounds.sounds.end(), this);
		root.sounds.sounds.erase(iterator);

		delete this;
	}
}
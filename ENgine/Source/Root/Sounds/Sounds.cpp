#include "Root/Root.h"

namespace Orin
{
	bool Sounds::Init()
	{
		FMOD_RESULT result;

		result = FMOD::System_Create(&system);

		if (result != FMOD_OK)
		{
			return false;
		}

		result = system->init(32, FMOD_INIT_NORMAL, nullptr);

		if (result != FMOD_OK)
		{
			return false;
		}

		return true;
	}

	SoundInstance* Sounds::CreateSound(void* scene, bool streamed, const char* name)
	{
		SoundRes* res = nullptr;

		eastl::string key = name + eastl::string(streamed ? "_str" : "_mem");

		if (soundResRefs.count(key) > 0)
		{
			SoundsResRef& ref = soundResRefs[key];

			ref.count++;
			res = ref.res;
		}
		else
		{
			res = new SoundRes();

			if (!res->Load(name, streamed))
			{
				delete res;
				return nullptr;
			}

			SoundsResRef& ref = soundResRefs[key];

			ref.count = 1;
			ref.res = res;
		}

		SoundInstance* instance = new SoundInstance();
		instance->res = res;

		sounds.push_back(instance);

		return instance;
	}

	void Sounds::DeleteSceneSounds(void* scene)
	{
		for (int i = 0; i < sounds.size(); i++)
		{
			auto* sound = sounds[i];

			if (sound->scene == scene)
			{
				sounds[i]->Release();
				i--;
			}
		}
	}

	void Sounds::Update(float dt)
	{
		system->update();

		for(int i = 0; i < sounds.size(); i++)
		{
			auto* sound = sounds[i];
			if (sound->playing && sound->playType != PlaySoundType::Looped)
			{
				sound->channel->isPlaying(&sound->playing);

				if (!sound->playing && 	sound->playType == PlaySoundType::AutoDelete)
				{
					sound->Release();
					i--;
				}

				continue;
			}
		}
	}

	void Sounds::SetMasterVolume(float volume)
	{
		masterVolume = volume;
	}

	float Sounds::GetMasterVolume()
	{
		return masterVolume;
	}

	void Sounds::ClearAllSounds()
	{
		for (int i = 0; i < sounds.size(); i++)
		{
			sounds[i]->Release();
			i--;
		}
	}

	void Sounds::OnPause()
	{
		for (auto* snd : sounds)
		{
			if (snd->playing)
			{
				snd->Pause(true);
			}
		}
	}

	void Sounds::OnResume()
	{
		for (auto* snd : sounds)
		{
			if (snd->playing)
			{
				snd->Pause(false);
			}
		}
	}

	bool Sounds::DecRef(SoundRes* res)
	{
		typedef eastl::map<eastl::string, SoundsResRef>::iterator it_type;

		for (it_type iterator = soundResRefs.begin(); iterator != soundResRefs.end(); iterator++)
		{
			if (iterator->second.res == res)
			{
				iterator->second.count--;

				if (iterator->second.count == 0)
				{
					soundResRefs.erase(iterator);
					return true;
				}

				return false;
			}
		}

		return true;
	}

	void Sounds::Release()
	{
		ClearAllSounds();

		if (system)
		{
			system->release();
			system = nullptr;
		}
	}
}
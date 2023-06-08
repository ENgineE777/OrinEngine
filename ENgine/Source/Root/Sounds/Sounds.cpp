#include "Root/Root.h"

namespace Orin
{
	bool Sounds::Init()
	{
		/*FMOD_RESULT result;

		result = FMOD::System_Create(&coreSystem);

		if (result != FMOD_OK)
		{
			return false;
		}

		result = coreSystem->init(32, FMOD_INIT_NORMAL, nullptr);

		if (result != FMOD_OK)
		{
			return false;
		}*/

		if (FMOD::Studio::System::create(&system) != FMOD_OK)
		{
			return false;
		}

		system->getCoreSystem(&coreSystem);

		//ERRCHECK(coreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0));

		void* extraDriverData = nullptr;
		if (system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData) != FMOD_OK)
		{
			return false;
		}

		return true;
	}

	//int banksCount = 0;
	//FMOD::Studio::Bank* banks[3];

	bool Sounds::LoadSoundBank(const char* name)
	{
		FMOD::Studio::Bank* masterBank = NULL;

		const char* rootPath = root.GetPath(Root::Path::Assets);
		
		char path[1024];
		StringUtils::Printf(path, 1024, "%s%s", rootPath, name);

		auto result = system->loadBankFile(path, FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

		return result == FMOD_OK;
	}

	bool Sounds::SetListenerAttributes(Math::Vector3 pos)
	{
		if (!system)
		{
			return false;
		}

		FMOD_3D_ATTRIBUTES listenerAttributes;
		listenerAttributes.position = { pos.x, pos.y, pos.z };
		listenerAttributes.forward = {1.f, 0.f, 0.f};
		listenerAttributes.up = {0.f, 1.f, 0.f};
		listenerAttributes.velocity = {0.f, 0.f, 0.f};

		return system->setListenerAttributes(0, &listenerAttributes) == FMOD_OK;
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
		instance->scene = scene;

		sounds.push_back(instance);

		return instance;
	}

	SoundEvent* Sounds::CreateSoundEvent(void* scene, bool streamed, const char* name)
	{
		SoundEventRes* res = nullptr;

		eastl::string key = name + eastl::string(streamed ? "_str" : "_mem");

		if (soundEventResRefs.count(key) > 0)
		{
			SoundsEventResRef& ref = soundEventResRefs[key];

			ref.count++;
			res = ref.res;
		}
		else
		{
			res = new SoundEventRes();

			if (!res->Load(name, streamed))
			{
				delete res;
				return nullptr;
			}

			SoundsEventResRef& ref = soundEventResRefs[key];

			ref.count = 1;
			ref.res = res;
		}

		SoundEvent* instance = new SoundEvent();
		instance->res = res;
		instance->scene = scene;

		soundsEvents.push_back(instance);

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

		for (int i = 0; i < soundsEvents.size(); i++)
		{
			auto* sound = soundsEvents[i];

			if (sound->scene == scene)
			{
				soundsEvents[i]->Release();
				i--;
			}
		}
	}

	void Sounds::Update(float dt)
	{
		if (coreSystem)
		{
			coreSystem->update();
		}

		if (system)
		{
			system->update();
		}

		for (int i = 0; i < sounds.size(); i++)
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

		for (int i = 0; i < soundsEvents.size(); i++)
		{
			auto* sound = soundsEvents[i];
			if (sound->playing && sound->playType != PlayEventType::Looped)
			{
				FMOD_STUDIO_PLAYBACK_STATE state;
				sound->instance->getPlaybackState(&state);

				if (state == FMOD_STUDIO_PLAYBACK_STOPPED && sound->playType == PlayEventType::AutoDelete)
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

		FMOD::ChannelGroup* channelgroup;

		if (coreSystem->getMasterChannelGroup(&channelgroup) == FMOD_OK)
		{
			channelgroup->setVolume(masterVolume);
		}

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

		for (int i = 0; i < soundsEvents.size(); i++)
		{
			soundsEvents[i]->Release();
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

	bool Sounds::DecRef(SoundEventRes* res)
	{
		typedef eastl::map<eastl::string, SoundsEventResRef>::iterator it_type;

		for (it_type iterator = soundEventResRefs.begin(); iterator != soundEventResRefs.end(); iterator++)
		{
			if (iterator->second.res == res)
			{
				iterator->second.count--;

				if (iterator->second.count == 0)
				{
					soundEventResRefs.erase(iterator);
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
			system->unloadAll();

			system->release();
			system = nullptr;
		}
	}
}
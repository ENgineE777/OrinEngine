#include "Root/Root.h"

namespace Orin
{
	bool Sounds::InitFMOD(bool useStudio, bool useLiveUpdate)
	{
		if (coreSystem || system)
		{
			root.Log("Sounds", "FMOD already inited, InitFMOD was called twice");
			return false;
		}

		if (!useStudio)
		{
			root.Log("Sounds", "useStudio = 0, creating core FMOD System");

			if (FMOD::System_Create(&coreSystem) != FMOD_OK)
			{
				return false;
			}

			if (coreSystem->init(32, FMOD_INIT_NORMAL, nullptr) != FMOD_OK)
			{
				return false;
			}
		}
		else
		{
			root.Log("Sounds", "useStudio = 1, creating FMOD Studio System");

			if (FMOD::Studio::System::create(&system) != FMOD_OK)
			{
				return false;
			}

			system->getCoreSystem(&coreSystem);

			void* extraDriverData = nullptr;
			if (system->initialize(1024, (useLiveUpdate ? FMOD_STUDIO_INIT_LIVEUPDATE : FMOD_STUDIO_INIT_NORMAL) | FMOD_STUDIO_INIT_MEMORY_TRACKING, FMOD_INIT_NORMAL | FMOD_INIT_MEMORY_TRACKING, extraDriverData) != FMOD_OK)
			{
				return false;
			}
		}

		return true;
	}

	bool Sounds::Init()
	{		
		return true;
	}

	bool Sounds::LoadSoundBank(const char* name)
	{
		if (!system)
		{
			return false;
		}

		FMOD::Studio::Bank* masterBank = NULL;

		const char* rootPath = root.GetPath(Root::Path::Assets);

		char path[1024];
		StringUtils::Printf(path, 1024, "%s%s", rootPath, name);

		auto result = system->loadBankFile(path, FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

		if (result == FMOD_OK)
		{
			masterBank->loadSampleData();

			return true;
		}

		return false;
	}

	bool Sounds::SetListenerAttributes(Math::Vector3 pos)
	{
		if (!system)
		{
			return false;
		}

		FMOD_3D_ATTRIBUTES listenerAttributes;
		listenerAttributes.position = { pos.x, pos.z, pos.y };
		listenerAttributes.forward = { 0.f, 0.f, 1.f };
		listenerAttributes.up = { 0.f, 1.f, 0.f };
		listenerAttributes.velocity = { 0.f, 0.f, 0.f };

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

	SoundEvent* Sounds::CreateSoundEvent(const char* name)
	{
		FMOD::Studio::EventDescription* descriptor = nullptr;
		
		if (system->getEvent(name, &descriptor) == FMOD_OK)
		{	
			FMOD::Studio::EventInstance* instance = nullptr;
			descriptor->createInstance(&instance);			

			return new SoundEvent(instance);
		}

		return nullptr;
	}

	void Sounds::PlaySoundEvent(const char* name, Math::Vector3* pos)
	{
		FMOD::Studio::EventDescription* descriptor = nullptr;
		if (system->getEvent(name, &descriptor) == FMOD_OK)
		{
			FMOD::Studio::EventInstance* instance = nullptr;
			
			if (descriptor->createInstance(&instance) == FMOD_OK)
			{
				bool is3D = false;
				descriptor->is3D(&is3D);

				if (pos && is3D)
				{
					FMOD_3D_ATTRIBUTES attributes = { 0 };
					attributes.position = { pos->x, pos->z, pos->y };
					attributes.forward = { 0.f, 0.f, 1.f };
					attributes.up = { 0.f, 1.f, 0.f };
					attributes.velocity = { 0.f, 0.f, 0.f };

					instance->set3DAttributes(&attributes);
				}

				instance->start();
				instance->release();
			}
		}
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
		/*FMOD_STUDIO_MEMORY_USAGE memoryusage;
		system->getMemoryUsage(&memoryusage);

		root.render.DebugPrintText(10.0f, ScreenCorner::LeftTop, COLOR_GREEN,  "Exc: %i, inc: %i, sample: %i", memoryusage.exclusive, memoryusage.inclusive, memoryusage.sampledata);*/		

		if (system)
		{
			system->update();
		}
		else
		if (coreSystem)
		{
			coreSystem->update();
		}

		for (int i = 0; i < sounds.size(); i++)
		{
			auto* sound = sounds[i];
			if (sound->playing && sound->playType != PlaySoundType::Looped)
			{
				sound->channel->isPlaying(&sound->playing);

				if (!sound->playing && sound->playType == PlaySoundType::AutoDelete)
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

	float Sounds::GetVCAVolume(const char* path)
	{
		if (!system)
		{
			return 0.0f;
		}

		FMOD::Studio::VCA* vca;
		system->getVCA(path, &vca);

		float volume = 0.0f;

		if (vca)
		{
			vca->getVolume(&volume);
		}

		return volume;
	}	

	void Sounds::SetVCAVolume(const char* path, float volume)
	{
		if (!system)
		{
			return;
		}

		FMOD::Studio::VCA* vca;
		system->getVCA(path, &vca);

		if (vca)
		{
			vca->setVolume(volume);
		}
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
			system->unloadAll();

			system->release();
			system = nullptr;
		}
	}
}
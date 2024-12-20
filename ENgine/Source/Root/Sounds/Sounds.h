
#pragma once

#include "SoundInstance.h"
#include "SoundRes.h"
#include "SoundEvent.h"

namespace Orin
{
	/**
	\ingroup gr_code_services_sound
	*/

	/**
	\brief Sounds

	This class manages sounds. Sounds can be entrielly loaded into memory or can be streamed.
	First type is usefull for SFX. For music better to use streamed version. Wav and Ogg formats
	are supported.

	Example of playing SFX:

	\code
	auto sound_inst = CreateSound("sound01.wav");
	test_inst->Play(SoundBase::Autodelete);
	\endcode

	*/

	class CLASS_DECLSPEC Sounds
	{
		friend class SoundInstance;
		friend class SoundRes;
		friend class SoundEvent;
		friend class SoundEventRes;

		float masterVolume = 1.0f;

		FMOD::Studio::System* system = nullptr;
		FMOD::System* coreSystem = nullptr;

		struct SoundsResRef
		{
			int count;
			SoundRes* res;
		};

		eastl::map<eastl::string, SoundsResRef> soundResRefs;
		eastl::vector<SoundInstance*> sounds;

		bool DecRef(SoundRes* res);

	public:

		bool InitFMOD(bool useStudio, bool useLiveUpdate);

		bool LoadSoundBank(const char* name);

		/**
		\brief Set 3D attributes of a listener

		\param[in] pos Position of a listener
		*/
		bool SetListenerAttributes(Math::Vector3 pos);

		/**
		\brief Create SoundInstance

		\param[in] scene Pointer to a parent scene
		\param[in] streamed Should be audio streamed
		\param[in] name Full path to a file

		\return Pointer to SoundInstance
		*/
		SoundInstance* CreateSound(void* scene, bool streamed, const char* name);

		SoundEvent* CreateSoundEvent(const char* name);

		void PlaySoundEvent(const char* name, Math::Vector3* pos = nullptr);

		void PlaySFX(const char* name);

		/**
		\brief Set master volume

		\param[in] volume Value of a volume 
		*/
		void SetMasterVolume(float volume);

		/**
		\brief Get master volume

		\return Current master volume
		*/
		float GetMasterVolume();

		/**
		\brief Get volume of VCA

		\param[in] path path of vca
		*/

		float GetVCAVolume(const char* path);

		/**
		\brief Set volume of VCA

		\param[in] path path of vca
		\param[in] volume Value of a volume
		*/

		void SetVCAVolume(const char* path, float volume);

	#ifndef DOXYGEN_SKIP
		bool Init();

		void Update(float dt);

		void DeleteSceneSounds(void* scene);

		void ClearAllSounds();

		void OnPause();
		void OnResume();
		void Release();
	#endif
	};
}
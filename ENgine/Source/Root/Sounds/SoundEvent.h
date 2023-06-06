
#pragma once

#include "fmod.hpp"
#include "fmod_studio.hpp"

namespace Orin
{
	enum class CLASS_DECLSPEC PlayEventType
	{
		Once = 0 /*!< Sound should be played only once */,
		Looped /*!< Sound should be played in loop mode */,
		AutoDelete /*!< Play sound once and thet auto delete insctance */,
	};

	class CLASS_DECLSPEC SoundEvent
	{
		friend class Sounds;
		friend class SoundInstance;
		friend class SoundStream;

	public:

		SoundEvent() = default;

		/**
		\brief Start to play a sound

		\param[in] type Type of playing
		*/
		bool Play(PlayEventType type);

		/**
		\brief Check is souns is playing

		*/
		bool IsPlaying();

		/**
		\brief Pause or unpause playing of a sound

		\param[in] pause Define pause ir unpause playing of a sound
		*/
		void Pause(bool pause);

		/**
		\brief Stop playing
		*/
		void Stop();

		/**
		\brief Set a volume

		\param[in] volume Value of a volume
		*/
		void SetVolume(float volume);

		/**
		\brief Get master volume

		\return Current master volume
		*/
		float GetVolume();

		/**
		\brief SoundInstace should released only via this method
		*/
		void Release();

	private:

		class SoundEventRes* res = nullptr;
		FMOD::Studio::EventInstance* instance = nullptr;

		void* scene = nullptr;		

		float volume = 1.0f;
		bool playing = false;
		bool paused = false;

		PlayEventType playType;
	};
}
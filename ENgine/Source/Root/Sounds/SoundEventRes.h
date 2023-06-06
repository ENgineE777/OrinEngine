
#pragma once

#include "Support/Support.h" 
#include "Root/Files/FileInMemory.h"
#include "fmod.hpp"
#include "fmod_studio.hpp"

namespace Orin
{
	class CLASS_DECLSPEC SoundEventRes
	{
		friend class SoundEvent;
		friend class Sounds;

		FMOD::Studio::EventDescription* descriptor = nullptr;

	public:

		bool Load(const char* path, bool streamed);
		void Release();
	};
}
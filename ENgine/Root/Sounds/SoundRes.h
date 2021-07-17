
#pragma once

#include "Support/Support.h" 
#include "Root/Files/FileInMemory.h"
#include "fmod.hpp"

namespace Oak
{
	/**
	\ingroup gr_code_services_sound
	*/

	/**
	\brief SoundRes

	Sound which loads entire sounf file into memory. Good for small SFX.

	*/

	class CLASS_DECLSPEC SoundRes
	{
		friend class SoundInstance;
		friend class Sounds;

		FMOD::Sound* sound = nullptr;
		FMOD_CREATESOUNDEXINFO exinfo;
		FileInMemory buffer;

	public:

		bool Load(const char* path, bool streamed);
		void Release();
	};
}
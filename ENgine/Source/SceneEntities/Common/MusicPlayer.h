
#pragma once

#include "Root/Scenes/SceneEntity.h"

namespace Oak
{
	/**
	\page scene_object_common_MusicPlayer MusicPlayer

	This object plays streamed sound.

	This class ::MusicPlayer is a representation on C++ side.

	This class Script::MusicPlayer is a representation in a script.

	Parameters
	----------

	Name              | Description
	------------------| -------------
	sound_name        | Filename of a sound file
	play_on_start     | Should sound play on start

	*/

	/**
	\ingroup gr_code_scene_objects_common
	*/

	/**
	\brief Representation of a music player

	This object plays streamed sound.

	This class Script::MusicPlayer is a representation in a script.

	*/

	class MusicPlayer : public SceneEntity
	{
	public:
		META_DATA_DECL(MusicPlayer)

		/**
		\brief Filename of a sound file.
		*/

		eastl::string soundName;

		/**
		\brief Pointer to SoundInstance
		*/

		class SoundInstance* stream;

		/**
		\brief Is music faded
		*/
		bool faded = false;

		int fadeMode = 0;
		float fadeTime = 0.0f;
		float curFadeTime = 0.0f;

	#ifndef DOXYGEN_SKIP

		virtual ~MusicPlayer() = default;

		void OnVisiblityChange(bool set) override;

		void Init() override;

		void FadeIn(float time);
		void FadeOut(float time);

		void Work(float dt);

		void Play() override;
		void Release() override;
	#endif
	};
}
#include "MusicPlayer.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, MusicPlayer, "Sound", "MusicPlayer")

	META_DATA_DESC(MusicPlayer)
		BASE_SCENE_ENTITY_PROP(MusicPlayer)
		FILENAME_PROP(MusicPlayer, soundName, "", "Property", "SoundName")
		BOOL_PROP(MusicPlayer, faded, false, "Property", "Faded", " Is music faded")
		FLOAT_PROP(MusicPlayer, volume, 1.0f, "Property", "volume", " Volume of music")
	META_DATA_DESC_END()

	void MusicPlayer::Init()
	{
		Tasks(false)->AddTask(100, this, (Object::Delegate)&MusicPlayer::Work);
	}

	void MusicPlayer::OnVisiblityChange(bool set)
	{
		if (stream && stream->IsPlaying() && !faded)
		{
			stream->Pause(!set);
		}
	}

	void MusicPlayer::Play()
	{
		SceneEntity::Play();

		stream = root.sounds.CreateSound(GetScene(), true, soundName.c_str());

		if (IsVisible() && !faded && stream)
		{
			stream->Play(PlaySoundType::Looped);
			stream->SetVolume(volume);
		}

	}

	void MusicPlayer::FadeIn(float time)
	{
		if (!faded)
		{
			return;
		}

		faded = false;

		fadeMode = 1;
		curFadeTime = 0.0f;
		fadeTime = time;

		if (stream)
		{
			if (!stream->IsPlaying())
			{
				stream->SetVolume(0.0f);
				stream->Play(PlaySoundType::Looped);
			}
			else
			{
				stream->Pause(false);
				stream->SetVolume(volume);
			}
		}
	}

	void MusicPlayer::FadeOut(float time)
	{
		if (faded)
		{
			return;
		}

		fadeMode = -1;
		curFadeTime = 0.0f;
		fadeTime = time;
	}

	void MusicPlayer::Work(float dt)
	{
		if (!stream)
		{
			return;
		}

		if (fadeMode != 0)
		{
			curFadeTime += dt;

			if (curFadeTime > fadeTime)
			{
				curFadeTime = fadeTime;

				if (fadeMode == -1)
				{
					faded = true;
					stream->Pause(true);
				}
				else
				{
					stream->SetVolume(volume);
				}

				fadeMode = 0;
			}
			else
			{
				float k = curFadeTime / fadeTime;

				if (fadeMode == -1)
				{
					k = 1.0f - k;
				}

				stream->SetVolume(k * volume);
			}
		}
	}

	void MusicPlayer::Release()
	{
		RELEASE(stream);

		SceneEntity::Release();
	}
}
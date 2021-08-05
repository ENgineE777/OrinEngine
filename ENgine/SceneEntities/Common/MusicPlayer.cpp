#include "MusicPlayer.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, MusicPlayer, "MusicPlayer")

	META_DATA_DESC(MusicPlayer)
		BASE_SCENE_ENTITY_PROP(MusicPlayer)
		FILENAME_PROP(MusicPlayer, soundName, "", "Property", "SoundName")
		BOOL_PROP(MusicPlayer, faded, false, "Property", "Faded", " Is music faded")
	META_DATA_DESC_END()

	void MusicPlayer::Init()
	{
		Tasks(false)->AddTask(100, this, (Object::Delegate)&MusicPlayer::Work);
	}

	void MusicPlayer::SetVisible(bool visible)
	{
		if (stream && stream->IsPlaying() && !faded)
		{
			stream->Pause(!visible);
		}

		SceneEntity::SetVisible(visible);
	}

	void MusicPlayer::Play()
	{
		SceneEntity::Play();

		stream = root.sounds.CreateSound(GetScene(), true, soundName.c_str());

		if (IsVisible() && !faded)
		{
			stream->Play(PlaySoundType::Looped);
		}

	}

	void MusicPlayer::FadeIn(float time)
	{
		faded = false;

		fadeMode = 1;
		curFadeTime = 0.0f;
		fadeTime = time;

		if (!stream->IsPlaying())
		{
			stream->SetVolume(0.0f);
			stream->Play(PlaySoundType::Looped);
		}
		else
		{
			stream->Pause(false);
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
					stream->SetVolume(1.0f);
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

				stream->SetVolume(k);
			}
		}
	}

	void MusicPlayer::Release()
	{
		RELEASE(stream);

		SceneEntity::Release();
	}
}
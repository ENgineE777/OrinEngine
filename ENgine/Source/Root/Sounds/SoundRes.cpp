
#include "Root/Root.h"
#include "SoundRes.h"

namespace Oak
{
	bool SoundRes::Load(const char* path, bool streamed)
	{
		FMOD_RESULT result;

		if (!buffer.Load(path))
		{
			return false;
		}

		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.length = buffer.GetSize();
		
		if (streamed)
		{
			result = root.sounds.system->createStream((const char*)buffer.GetPtr(), FMOD_OPENMEMORY | FMOD_LOOP_NORMAL | FMOD_2D, &exinfo, &sound);
		}
		else
		{
			result = root.sounds.system->createSound((const char*)buffer.GetPtr(), FMOD_OPENMEMORY | FMOD_LOOP_NORMAL | FMOD_2D, &exinfo, &sound);
		}

		return result == FMOD_OK;
	}

	void SoundRes::Release()
	{
		if (sound)
		{
			sound->release();
		}

		delete this;
	}
}
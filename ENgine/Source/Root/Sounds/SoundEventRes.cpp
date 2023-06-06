
#include "Root/Root.h"

namespace Orin
{
	bool SoundEventRes::Load(const char* path, bool streamed)
	{
		auto result = root.sounds.system->getEvent(path, &descriptor);
		
		if (descriptor && streamed)
		{
			descriptor->loadSampleData();
		}
		
		return result == FMOD_OK;
	}

	void SoundEventRes::Release()
	{
		delete this;
	}
}
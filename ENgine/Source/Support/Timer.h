#pragma once

#include "Defines.h"

/**
\ingroup gr_code_common
*/

namespace Oak::Timer
{
	CLASS_DECLSPEC float CountDeltaTime();
	CLASS_DECLSPEC float GetDeltaTime();
	CLASS_DECLSPEC int GetFPS();
	CLASS_DECLSPEC const char* GetTimeStamp();
}

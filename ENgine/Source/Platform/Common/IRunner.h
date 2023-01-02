#pragma once

#include "Support/Defines.h"
#include "Support/Delegate.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/**
\ingroup gr_code_platform_common
*/

namespace Orin
{
	class CLASS_DECLSPEC IRunner : public Object
	{
	public:

		virtual bool Init(HWND hwnd) = 0;
		virtual void Update() = 0;
		virtual void OnResize(int width, int height) = 0;

		virtual void Release() = 0;
	};

	CLASS_DECLSPEC IRunner* GetRunner();
}

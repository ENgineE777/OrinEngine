#pragma once

#include "Support/Defines.h"
#include "Support/Delegate.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/**
\ingroup gr_code_editor
*/

namespace Orin
{
	class CLASS_DECLSPEC IEditor : public Object
	{
	public:

		virtual bool Init(HWND hwnd, const char* startProject) = 0;
		virtual bool Update() = 0;
		virtual bool ProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
		virtual void OnResize(int width, int height) = 0;

		virtual void Release() = 0;
	};

	CLASS_DECLSPEC IEditor* GetEditor();
}

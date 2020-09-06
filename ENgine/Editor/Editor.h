#pragma once

#include "Root/Root.h"
#include "FreeCamera.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Oak
{
	class Editor : Object
	{
		HWND hwnd;
		TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;
		FreeCamera freeCamera;

	public:

		bool Init(HWND hwnd);
		void Update();
		void Render(float dt);
		void OnMouseMove(int msx, int msy);
	};
}

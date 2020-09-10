
#pragma once

#include "Root/Controls/Controls.h"
#include "Root/Files/Files.h"
#include "Root/Fonts/Fonts.h"
#include "Root/Memory/MemoryManager.h"
#include "Root/Render/Render.h"
#include "Root/TaskExecutor/TaskExecutor.h"

namespace Oak
{
	class Root
	{
		#ifdef PLATFORM_WIN
		char  logsDir[1024];
		float dt = 0.0f;
		#endif

	public:

		Controls controls;
		Files files;
		Fonts fonts;
		MemoryManager memory;
		Render render;
		TaskExecutor taskExecutor;

		Root();
		~Root() = default;
		bool Init(void* renderData);

		void Log(const char* name, const char* text, ...);

		void Update();
		void CountDeltaTime();
		float GetDeltaTime();
		int GetFPS();

		void Release();
	};

	extern Root root;
}

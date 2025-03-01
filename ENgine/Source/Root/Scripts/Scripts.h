#pragma once

#ifdef PLATFORM_WIN
#include "Windows.h"
#endif

#include "Support/Support.h"

namespace Orin
{
	class Scripts
	{
		#ifdef PLATFORM_WINDOWS
		HMODULE Module = nullptr;
		uint64_t LastWrite = 0;
		int pingPong = 0;
		bool allowDynamicReload = false;

#ifdef _DEBUG
		eastl::string configName = "Debug";
#else
		eastl::string configName = "Release";
#endif

		bool CheckGamePlayDll();
		bool LoadGamePlayDll(const char* path);
		#endif

	public:

		#ifdef PLATFORM_WINDOWS
		bool CompileProjectCode(bool forceCompile);
		void SetAllowDynamicReload(bool val);
		#endif

		void Update();
		bool Start();
		void Stop();
	};
}

#include "Root.h"
#include "Support/Timer.h"
#include "Support/StringUtils.h"
#include "Support/Sprite.h"

#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "stb_sprintf.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#ifdef ORIN_EDITOR
#include "Editor/Editor.h"
#include <filesystem>
#endif

namespace Orin
{
	Root root;

	IRoot* GetRoot()
	{
		return &root;
	}

	Root::Root()
	{
		logsDir[0] = 0;
	}

	bool Root::Init(void* renderData)
	{
		srand((unsigned int)time(nullptr));

		#ifdef PLATFORM_WINDOWS
		crashHandler.Init();

		char curDir[1024];
		GetCurrentDirectoryA(1024, curDir);
		StringUtils::Printf(logsDir, 1024, "%s/Logs", curDir);

		root.files.DeleteFolder(logsDir);
		CreateDirectoryA(logsDir, nullptr);

		time_t t = time(0);

		#endif

		if (!files.Init())
		{
			return false;
		}

		if (!controls.Init("ENgine/Controls/hardware_win", true))
		{
			return false;
		}

		if (!controls.LoadAliases("ENgine/Controls/engine_aliases"))
		{
			OAK_ALERT("controls.LoadAliases(\"ENgine/Controls/engine_aliases\") failed");

			return false;
		}

		if (!render.Init("DX11", renderData))
		{
			return false;
		}

		if (!fonts.Init())
		{
			return false;
		}

		Sprite::Init();

		particles.Init();

		sounds.Init();

		assets.Init();

		physics.Init();

		scenes.Init();

		StringUtils::Init();

		Timer::CountDeltaTime();

		return true;
	}

	void Root::Log(const char* name, const char* text, ...)
	{
		static char buffer[4096];

		int timeStampLen = (int)strlen(Timer::GetTimeStamp());
		memcpy(buffer, Timer::GetTimeStamp(), timeStampLen);
		buffer[timeStampLen];

		va_list args;
		va_start(args, text);

		stbsp_vsnprintf(&buffer[timeStampLen], sizeof(buffer) - 4 - timeStampLen, text, args);

		va_end(args);

		#ifdef PLATFORM_WINDOWS
		OutputDebugStringA(buffer);
		OutputDebugStringA("\n");

		char path[1024];
		StringUtils::Printf(path, 1024, "%s/%s.txt", logsDir, name);

#ifdef ORIN_EDITOR
		if (redirectLog)
		{
			editor.CaptureLog(name, buffer);
		}
#endif

		FILE* f = nullptr;
		fopen_s(&f, path, "a");

		if (f)
		{
			fwrite(buffer, strlen(buffer), 1, f);
			fwrite("\n", strlen("\n"), 1, f);
			fclose(f);
		}
		#endif
	}

	void Root::CountDeltaTime()
	{
		Timer::CountDeltaTime();
		dt = Timer::GetDeltaTime() * dtScale;

		if (controls.DebugKeyPressed("KEY_LSHIFT", AliasAction::Pressed) ||
			controls.DebugKeyPressed("KEY_RSHIFT", AliasAction::Pressed))
		{
			if (controls.DebugKeyPressed("KEY_MINUS", AliasAction::JustPressed))
			{
				dtScale *= 0.5f;
			}
			else
			if (controls.DebugKeyPressed("KEY_EQUALS", AliasAction::JustPressed))
			{
				dtScale *= 2.0f;
			}
			else
			if (controls.DebugKeyPressed("KEY_0", AliasAction::JustPressed))
			{
				dtScale = 1.0f;
			}
		}
	}

	void Root::Update()
	{
		Sprite::Update();

		assets.Update();

		controls.Update(dt);

		scenes.Execute(dt);

		physics.DeleteObjects();

		scripts.Update();

		physics.Update(dt);
		physics.Fetch();

		render.Execute(dt);

		sounds.Update(dt);		
	}

	float Root::GetDeltaTime()
	{
		return dt;
	}

	int Root::GetFPS()
	{
		return Timer::GetFPS();
	}

	void Root::PreparePhysScene()
	{
		if (pscene == nullptr)
		{
			pscene = root.physics.CreateScene();
		}
	}

	void Root::ClearPhysScene()
	{
		if (pscene)
		{
			root.physics.DestroyScene(pscene);
			pscene = nullptr;
		}
	}

	void Root::SetRootPath(const char* setRootPath)
	{
		#ifdef PLATFORM_WINDOWS
		rootPath = eastl::string(setRootPath);
		rootPathAssets = eastl::string(setRootPath) + "Assets/";
		rootPathBinaries = eastl::string(setRootPath) + "Binaries/Win64/";
		rootPathVcProj = eastl::string(setRootPath) + "VcProj/";

		char searchPath[256];
		StringUtils::Printf(searchPath, 256, "DebugInfo;%s", rootPathBinaries.c_str());

		crashHandler.SetSearchPath(searchPath);

		#endif
	}

	const char* Root::GetPath(Path path)
	{
		#ifdef PLATFORM_WINDOWS
		switch (path)
		{
			case Path::Project:
				return rootPath.c_str();
			break;
			case Path::Assets:
				return rootPathAssets.c_str();
			break;
			case Path::Binaries:
				return rootPathBinaries.c_str();
			break;
			case Path::VcProj:
				return rootPathVcProj.c_str();
			break;
		}
		#endif

		return nullptr;
	}

	void Root::Release()
	{
		assets.Release();

		Sprite::Release();

		scenes.Release();
		fonts.Release();
		render.Release();
		controls.Release();
		sounds.Release();

		redirectLog = false;

		memory.LogMemory();

		crashHandler.Release();
	}
}

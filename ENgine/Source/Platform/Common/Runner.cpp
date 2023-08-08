#include "Runner.h"
#include "Support/Perforce.h"

namespace Orin
{
	IRunner* GetRunner()
	{
		static Runner runner;

		return &runner;
	}

	bool Runner::Init(HWND setHwnd)
	{
		hwnd = setHwnd;

		root.sounds.SetInitializePrams(true, false);

		if (!root.Init(nullptr))
		{
			return false;
		}

		root.render.AddExecutedLevelPool(-1000);

		renderTaskPool = root.render.AddTaskPool(_FL_);
		renderTaskPool->AddTask(-1000, this, (Object::Delegate) & Runner::Render);

		RECT desktopRect;
		HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktopRect);

		if (!root.render.GetDevice()->SetBackBuffer(0, desktopRect.right, desktopRect.bottom, &hwnd))
		{
			return false;
		}

		//root.render.GetDevice()->SetFullscreenState(true);

		char currDir[512];
		GetCurrentDirectoryA(512, currDir);

		root.SetRootPath(StringUtils::PrintTemp("%s/project/", currDir));

		root.localization.LoadLocale();

		root.assets.LoadAssets();

		if (!root.scripts.Start())
		{
			return false;
		}

		JsonReader reader;

		if (reader.ParseFile("perforce.cl"))
		{
			int cl = -1;
			reader.Read("CL", cl);

			Perforce::SetCurrentCL(cl);
		}

		root.PreparePhysScene();
		root.scenes.LoadProject("project/project.pra");

		return true;
	}

	void Runner::Update()
	{
		root.controls.SetFocused(hwnd == GetActiveWindow());

		root.CountDeltaTime();

		//root.render.DebugPrintText(5.0f, ScreenCorner::RightTop, COLOR_WHITE, "%i", root.GetFPS());

		POINT corner = { 0, 0};

		ClientToScreen(hwnd, (LPPOINT)&corner.x);

		POINT mousePos; 
		GetCursorPos(&mousePos);

		mousePos.x -= corner.x;
		mousePos.y -= corner.y;

		root.controls.OverrideMousePos((int)Math::Clamp((float)mousePos.x, 0.0f, (float)root.render.GetDevice()->GetWidth()), (int)Math::Clamp((float)mousePos.y, 0.0f, (float)root.render.GetDevice()->GetHeight()));

		root.Update();
	}

	void Runner::Render(float dt)
	{
		root.render.GetDevice()->Clear(true, COLOR_BLACK, true, 1.0f);
		
		root.render.ExecutePool(-10, dt);
		root.render.ExecutePool(0, dt);
		root.render.ExecutePool(1, dt);
		root.render.ExecutePool(2, dt);
		root.render.ExecutePool(3, dt);
		root.render.ExecutePool(4, dt);
		root.render.ExecutePool(5, dt);
		root.render.ExecutePool(6, dt);
		root.render.ExecutePool(7, dt);
		root.render.ExecutePool(8, dt);
		root.render.ExecutePool(9, dt);
		root.render.ExecutePool(10, dt);
		root.render.ExecutePool(11, dt);
		root.render.ExecutePool(12, dt);

		root.render.ExecutePool(100, dt);
		root.render.ExecutePool(150, dt);
		root.render.ExecutePool(199, dt);
		root.render.ExecutePool(1000, dt);
		root.render.GetDevice()->Present();
	}

	void Runner::OnResize(int width, int height)
	{
		RECT rect;
		GetClientRect(hwnd, (LPRECT)&rect);

		root.render.GetDevice()->SetBackBuffer(0, rect.right, rect.bottom, &hwnd);
	}

	bool Runner::IsQuitRequested()
	{
		return root.IsQuitRequested();
	}

	void Runner::Release()
	{
		root.scenes.UnloadAll();

		root.scripts.Stop();

		root.ClearPhysScene();

		root.render.DelTaskPool(renderTaskPool);
		root.Release();
	}
}

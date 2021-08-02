#include "Runner.h"

namespace Oak
{
	IRunner* GetRunner()
	{
		static Runner runner;

		return &runner;
	}

	bool Runner::Init(HWND setHwnd)
	{
		hwnd = setHwnd;

		if (!root.Init(nullptr))
		{
			return false;
		}

		root.render.AddExecutedLevelPool(1);

		renderTaskPool = root.render.AddTaskPool(_FL_);
		renderTaskPool->AddTask(1, this, (Object::Delegate) & Runner::Render);

		RECT rect;
		GetClientRect(hwnd, (LPRECT)&rect);

		if (!root.render.GetDevice()->SetBackBuffer(0, rect.right, rect.bottom, &hwnd))
		{
			return false;
		}

		root.SetRootPath("project/");

		if (!root.scripts.Start())
		{
			return false;
		}

		root.PreparePhysScene();
		root.scenes.LoadProject("project/project.pra");

		return true;
	}

	void Runner::Update()
	{
		root.CountDeltaTime();

		POINT corner = { 0, 0};

		ClientToScreen(hwnd, (LPPOINT)&corner.x);

		POINT mousePos; 
		GetCursorPos(&mousePos);

		mousePos.x -= corner.x;
		mousePos.y -= corner.y;

		Oak::root.controls.OverrideMousePos(Math::Clamp(mousePos.x, 0, root.render.GetDevice()->GetWidth()), Math::Clamp(mousePos.y, 0, root.render.GetDevice()->GetHeight()));

		root.Update();
	}

	void Runner::Render(float dt)
	{
		root.render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

		root.render.ExecutePool(0, dt);
		root.render.ExecutePool(10, dt);
		root.render.ExecutePool(100, dt);
		root.render.ExecutePool(199, dt);
		root.render.ExecutePool(1000, dt);
		root.render.GetDevice()->Present();
	}

	void Runner::OnResize(int width, int height)
	{
		//if (d3dDevice != nullptr)
		{
			//CleanupRenderTarget();
			//swapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, 0);
			//CreateRenderTarget();
		}
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

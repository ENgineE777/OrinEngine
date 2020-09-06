#include "Editor.h"

namespace Oak
{
	void Editor::Init(HWND setHwnd)
	{
		hwnd = setHwnd;
		root.Init();

		root.render.AddExecutedLevelPool(1);

		renderTaskPool = root.render.AddTaskPool();
		renderTaskPool->AddTask(1, this, (Object::Delegate) & Editor::Render);

		root.render.GetDevice()->SetVideoMode(800, 600, &hwnd);

		freeCamera.Init();
	}

	void Editor::Update()
	{
		root.CountDeltaTime();

		root.render.DebugPrintText(5.0f, ScreenCorner::RightTop, COLOR_WHITE, "%i", root.GetFPS());

		for (int i = 0; i <= 20; i++)
		{
			float pos = (float)i - 10.0f;

			root.render.DebugLine(Math::Vector3(pos, 0.0f, -10.0f), COLOR_WHITE, Math::Vector3(pos, 0.0f, 10.0f), COLOR_WHITE);
			root.render.DebugLine(Math::Vector3(-10.0f, 0.0f, pos), COLOR_WHITE, Math::Vector3(10.0f, 0.0f, pos), COLOR_WHITE);
		}

		float dt = root.GetDeltaTime();

		root.controls.SetFocused(GetActiveWindow() == hwnd);

		freeCamera.Update(dt);

		root.Update();
	}

	void Editor::Render(float dt)
	{
		root.render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);
		root.render.ExecutePool(199, dt);
		root.render.ExecutePool(1000, dt);
		root.render.GetDevice()->Present();
	}

	void Editor::OnMouseMove(int msx, int msy)
	{
		root.controls.OverrideMousePos(msx, msy);
	}
}

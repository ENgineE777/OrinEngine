#include "Editor.h"

namespace Oak
{
    void Editor::Init(HWND hwnd)
    {
        root.Init();

        root.render.AddExecutedLevelPool(1);

        renderTaskPool = root.render.AddTaskPool();
        renderTaskPool->AddTask(1, this, (Object::Delegate) & Editor::Render);

        root.render.GetDevice()->SetVideoMode(800, 600, &hwnd);
    }

    void Editor::Update()
    {
        root.CountDeltaTime();

        root.render.DebugPrintText(5.0f, ScreenCorner::RightTop, COLOR_WHITE, "%i", root.GetFPS());

        root.Update();
    }

    void Editor::Render(float dt)
    {
        root.render.GetDevice()->Clear(true, COLOR_BLUE, true, 1.0f);
        root.render.ExecutePool(199, dt);
        root.render.ExecutePool(1000, dt);
        root.render.GetDevice()->Present();
    }
}

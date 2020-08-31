#pragma once

#include "Root/Root.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Oak
{
    class Editor : Object
    {
        TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;

    public:

        void Init(HWND hwnd);
        void Update();
        void Render(float dt);
    };
}

#pragma once

#include "Support/Defines.h"

namespace Orin
{
    class CLASS_DECLSPEC CrashHandler
    {
    public:
        bool Init();
        void Release();

        bool LoadDebugInfo(const char *name);
        bool UnloadDebugInfo(const char *name);

        void SetSearchPath(const char *path);
    };
}
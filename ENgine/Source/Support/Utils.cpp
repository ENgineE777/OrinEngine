#include "Utils.h"
#include <fstream>
#include <string>
#include "windows.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

namespace Orin::Utils
{
    bool Execute(const char* command, eastl::vector<eastl::string>& result)
    {
        result.clear();

        /*std::array<char, 128> buffer;
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command, "r"), _pclose);

        if (!pipe)
        {
            return false;
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result.push_back(buffer.data());
        }

        return true;*/

        HANDLE stdOutHandles[2];

        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = true;
        saAttr.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&stdOutHandles[0], &stdOutHandles[1], &saAttr, 0))
        {
            return false;
        }

        PROCESS_INFORMATION pInfo;
        ZeroMemory(&pInfo, sizeof(PROCESS_INFORMATION));

        STARTUPINFOA startInfo;
        ZeroMemory(&startInfo, sizeof(STARTUPINFOA));
        startInfo.cb = sizeof(STARTUPINFOA);
        startInfo.hStdOutput = stdOutHandles[1];
        startInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        startInfo.wShowWindow = SW_HIDE;

        if (!CreateProcessA(NULL, (char*)command, NULL, NULL, TRUE, 0, NULL, NULL, &startInfo, &pInfo))
        {
            CloseHandle(stdOutHandles[0]);
            CloseHandle(stdOutHandles[1]);
            return FALSE;
        }

        WaitForSingleObject(pInfo.hProcess, INFINITE);
        TerminateProcess(pInfo.hProcess, 0);

        CloseHandle(stdOutHandles[0]);
        CloseHandle(stdOutHandles[1]);

        char buffer[2048];
        DWORD readBufferSize;
        BOOL res;
        if ((res = ReadFile(stdOutHandles[0], buffer, 2048, &readBufferSize, NULL)))
        {
            buffer[readBufferSize] = '\0';

            eastl::string line;

            for (int i = 0; i < readBufferSize; i++)
            {
                char symbol = buffer[i];

                if (symbol == '\n')
                {
                    result.emplace_back(line);
                    line.clear();
                }
                else
                {
                    line += symbol;
                }
            }

            if (!line.empty())
            {
                result.emplace_back(line);
            }
        }

        CloseHandle(pInfo.hProcess);
        CloseHandle(pInfo.hThread);

        return res;
	}
}

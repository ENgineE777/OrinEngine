#include "Utils.h"
#include <fstream>
#include <string>
#include "windows.h"

namespace Oak::Utils
{
    bool Execute(const char* command, eastl::vector<eastl::string>& result)
    {
        result.clear();

        HANDLE childStdRead = nullptr;
        HANDLE childStdWrite = nullptr;

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        SECURITY_ATTRIBUTES saAttr;

        ZeroMemory(&saAttr, sizeof(saAttr));
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&childStdRead, &childStdWrite, &saAttr, 0))
        {
            return false;
        }

        if (!SetHandleInformation(childStdRead, HANDLE_FLAG_INHERIT, 0))
        {
            return false;
        }

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdError = childStdWrite;
        si.hStdOutput = childStdWrite;
        si.hStdInput = childStdRead;
        si.dwFlags |= STARTF_USESTDHANDLES;

        ZeroMemory(&pi, sizeof(pi));

        std::string commandLine = command;

        if (CreateProcessA(nullptr, (char*)commandLine.c_str(), nullptr, nullptr, true, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
        {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            constexpr int BUFSIZE = 64;
            DWORD dwRead;
            char chBuf[BUFSIZE];
            bool bSuccess = true;
            eastl::string line;

            while (bSuccess)
            {
                bSuccess = ReadFile(childStdRead, chBuf, BUFSIZE, &dwRead, nullptr);

                for (int i = 0; i < dwRead; i++)
                {
                    char symbol = chBuf[i];

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

                if (!bSuccess || dwRead < BUFSIZE) break;
            }

            if (!line.empty())
            {
                result.emplace_back(line);
            }
        }

        CloseHandle(childStdRead);
        CloseHandle(childStdWrite);

		return true;
	}
}

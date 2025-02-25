
#include "CrashHandler.h"
#include "IRoot.h"

#ifndef PLATFORM_WINDOWS
#error Crash handler supported only on Windows
#endif

#include "EASTL/string.h"

#include <windows.h>
#include <DbgHelp.h>
#include <tlhelp32.h>
#include <Psapi.h>

namespace Orin
{
    static const char *exceptionTypeToString(uint32_t code)
    {
        switch (code)
        {
        case EXCEPTION_ACCESS_VIOLATION:
            return "access violation";
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            return "misaligned data access";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            return "out of bounds access";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            return "floating point /0";
        case EXCEPTION_FLT_INVALID_OPERATION:
            return "floating point error";
        case EXCEPTION_FLT_OVERFLOW:
            return "floating point overflow";
        case EXCEPTION_FLT_STACK_CHECK:
            return "floating point stack over/underflow";
        case EXCEPTION_FLT_UNDERFLOW:
            return "floating point underflow";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            return "integer /0";
        case EXCEPTION_INT_OVERFLOW:
            return "integer overflow";
        case EXCEPTION_PRIV_INSTRUCTION:
            return "priv instruction";
        case EXCEPTION_IN_PAGE_ERROR:
            return "page is inaccessible";
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            return "illegal opcode";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            return "attempt to continue after noncontinuable exception";
        case EXCEPTION_STACK_OVERFLOW:
            return "stack overflow";
        case EXCEPTION_INVALID_DISPOSITION:
            return "invalid disposition";
        case EXCEPTION_INVALID_HANDLE:
            return "invalid handle";
        case STATUS_FLOAT_MULTIPLE_TRAPS:
            return "float trap";
        case STATUS_FLOAT_MULTIPLE_FAULTS:
            return "float faults";
        default:
            return "*unknown type*";
        }
    }

    static eastl::string PrintSymbol(const void *addr)
	{
		eastl::string res;

		HANDLE ph = GetCurrentProcess();
		
		struct Sym: public IMAGEHLP_SYMBOL { char nm[256]; };
		IMAGEHLP_MODULE mod; 
		Sym sym;
		ULONG_PTR ofs;
		unsigned long col=0;
		IMAGEHLP_LINE line;
		bool symOk = false;
		bool lineOk = false;

		sym.SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL);
		sym.MaxNameLength = sizeof(sym.nm);
		mod.SizeOfStruct  = sizeof(IMAGEHLP_MODULE);
		line.SizeOfStruct = sizeof(line);

		if (SymGetSymFromAddr(ph, (uintptr_t)addr, &ofs, &sym))
		{
			symOk = true;
		}

		res.append(StringUtils::PrintTemp("  %-8llX", (int64_t)addr));

		if (SymGetModuleInfo(ph, (uintptr_t)addr, &mod))
		{
			res.append(StringUtils::PrintTemp(" %s!", mod.ModuleName));
		}

		if (SymGetLineFromAddr(ph, (uintptr_t)addr, &col, &line))
		{
			lineOk = true;
		}

		if (symOk)
		{
			char n[256];
			if (!UnDecorateSymbolName(sym.Name, n, 256, UNDNAME_NO_MEMBER_TYPE|UNDNAME_NO_ACCESS_SPECIFIERS|UNDNAME_NO_ALLOCATION_MODEL|UNDNAME_NO_MS_KEYWORDS))
			{
				if (!SymUnDName(&sym, n, 128))
				{
					strcpy_s(n, sym.Name);
				}
			}

			if (lineOk)
			{
				res.append(StringUtils::PrintTemp(" %s  +%u/%u\n    %s(%d)  +%d", n, ofs, sym.Size, line.FileName, line.LineNumber, col));
			}
			else
			{
				res.append(StringUtils::PrintTemp(" %s  +%u/%u", n, ofs, sym.Size));
			}
		}

		if (lineOk && !symOk)
		{
			res.append(StringUtils::PrintTemp("    File=%s\n in Line=%d,Col=%d", line.FileName, line.LineNumber, col));
		}
		else if (!lineOk && !symOk) 
		{
			res.append(StringUtils::PrintTemp(" ?" ));
		}

		res.append(StringUtils::PrintTemp("\n" ));

		return res;
	}

	static eastl::string PrintCallStack(CONTEXT &context)
	{
		eastl::string callstack;

		if (context.Rip == 0 && context.Rsp != 0)
		{
			context.Rip = (ULONG64)(*(PULONG64)context.Rsp);
			context.Rsp += 8;
		}

		for (int frameIter = 0; context.Rip; ++frameIter)
		{
			ULONG64 imageBase = 0;
			if (PRUNTIME_FUNCTION runtimeFunction = (PRUNTIME_FUNCTION)RtlLookupFunctionEntry(context.Rip, &imageBase, NULL))
			{
				VOID* handlerData = NULL;
				ULONG64 framePointers[2] = { 0, 0 };

				RtlVirtualUnwind(UNW_FLAG_NHANDLER, imageBase, context.Rip, runtimeFunction, &context, &handlerData, framePointers, NULL);
			}
			else
			{
				break;
			}

			if (!context.Rip)
			{
				break;
			}

			callstack.append(StringUtils::PrintTemp("#%d: %s", frameIter, PrintSymbol((void*)(uintptr_t)context.Rip).c_str()));
		}
		return callstack;
	}
	
	// Crash handler function
	static LONG WINAPI CrashHandlerImpl(EXCEPTION_POINTERS* exceptionInfo)
	{
        CONTEXT ctx;
        RtlCaptureContext(&ctx);

		// Display a message box with information about the exception
		eastl::string message{StringUtils::PrintTemp(
		                      "An unhandled exception occurred:\n\nCode: 0x%x (%s)\nAddress: %p\n\n",
			                  exceptionInfo->ExceptionRecord->ExceptionCode,
                              exceptionTypeToString(exceptionInfo->ExceptionRecord->ExceptionCode),
							  exceptionInfo->ExceptionRecord->ExceptionAddress)};

        message += PrintCallStack(ctx);

		GetRoot()->Log("Crash", message.c_str());

		MessageBoxA(nullptr, message.c_str(), "Crash",  MB_OK | MB_ICONERROR);

		// Terminate the process
		TerminateProcess(GetCurrentProcess(), 1);

		return EXCEPTION_CONTINUE_SEARCH;
	}

    static bool FindModuleAddr(const char *moduleName, uintptr_t &baseAddr, uint32_t &moduleSize)
	{
		HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
		MODULEENTRY32 me32;

		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
		if (hModuleSnap == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		me32.dwSize = sizeof(MODULEENTRY32);

		if (!Module32First(hModuleSnap, &me32))
		{
			CloseHandle(hModuleSnap);
			return false;
		}

		do
		{
			if (::strcmp((const char*)me32.szExePath, moduleName) == 0 || ::strcmp((const char*)me32.szModule, moduleName) == 0)
			{
				CloseHandle(hModuleSnap);
				baseAddr = (uintptr_t)me32.modBaseAddr;
				moduleSize = me32.modBaseSize;
				return true;
			}
		}
		while (Module32Next(hModuleSnap, &me32));

		CloseHandle(hModuleSnap);
		return false;
	}

	static bool LoadModule(const char *moduleName, uintptr_t baseAddr = 0, uint32_t moduleSize = 0)
	{
		if (baseAddr == 0)
		{
			FindModuleAddr(moduleName, baseAddr, moduleSize);
		}

		return SymLoadModule(GetCurrentProcess(), NULL, moduleName, NULL, baseAddr, moduleSize);
	}

    bool CrashHandler::Init()
    {
        if (!SymInitialize(GetCurrentProcess(), "DebugInfo", FALSE))
        {
            return false;
        }

        SymSetOptions(SYMOPT_DEFERRED_LOADS);

		char exe_path[MAX_PATH];
		GetModuleFileNameA(GetModuleHandleA(NULL), exe_path, _countof(exe_path));
		LoadModule(exe_path);

		#ifdef _DEBUG
		const char *OrinDll = "Orin_Debug.dll";
		#else
		const char *OrinDll = "Orin.dll";
		#endif

		static const char* const knownModules[] = { OrinDll, "ntdll.dll", "kernelbase.dll", "kernel32.dll", "user32.dll" };
		for (int i = 0; i < _countof(knownModules); ++i)
		{
			MODULEINFO mi;
			HMODULE hmod = GetModuleHandleA(knownModules[i]);
			if (hmod && GetModuleInformation(GetCurrentProcess(), hmod, &mi, sizeof(mi)))
			{
				LoadModule(knownModules[i], (uintptr_t)mi.lpBaseOfDll, mi.SizeOfImage);
			}
		}

		SetUnhandledExceptionFilter(CrashHandlerImpl);

        return true;
    }

    void CrashHandler::Release()
    {
		SymCleanup(GetCurrentProcess());
    }

    bool CrashHandler::LoadDebugInfo(const char *path)
    {
        char moduleName[256];
		StringUtils::GetFileName(path, moduleName);

        MODULEINFO mi;
		HMODULE hmod = GetModuleHandleA(moduleName);
		if (hmod && GetModuleInformation(GetCurrentProcess(), hmod, &mi, sizeof(mi)))
		{
			return LoadModule(moduleName, (uintptr_t)mi.lpBaseOfDll, mi.SizeOfImage);
		}
		return false;
    }

    bool CrashHandler::UnloadDebugInfo(const char *name)
    {
        uintptr_t baseAddr = 0;
        uint32_t moduleSize = 0;
        FindModuleAddr(name, baseAddr, moduleSize);

		if (baseAddr != 0)
		{
			SymUnloadModule(::GetCurrentProcess(), baseAddr);
			return true;
		}

		return false;
    }

    void CrashHandler::SetSearchPath(const char *path)
    {
		SymSetSearchPath(GetCurrentProcess(), path);
    }
}
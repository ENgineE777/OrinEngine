
#include "ThreadExecutor.h"

namespace Orin
{
	CriticalSection::CriticalSection()
	{
	#ifdef PLATFORM_WIN
		InitializeCriticalSection(&critSection);
	#endif
	}

	CriticalSection::~CriticalSection()
	{
	#ifdef PLATFORM_WIN
		DeleteCriticalSection(&critSection);
	#endif
	}

	void CriticalSection::Enter()
	{
	#ifdef PLATFORM_WIN
		EnterCriticalSection(&critSection);
	#endif
	}

	void CriticalSection::UnLock()
	{
	#ifdef PLATFORM_WIN
		LeaveCriticalSection(&critSection);
	#endif
	}

	void ThreadExecutor::Execute(ThreadCaller* caller, ThreadCaller::Delegate call)
	{
		this->caller = caller;
		this->call = call;

		if (state != State::Idle)
		{
			return;
		}

	#ifdef PLATFORM_WIN
		thread = CreateThread (0, 256*1024, Entry, this, CREATE_SUSPENDED, NULL);
		SetThreadPriority(thread, THREAD_PRIORITY_NORMAL);

		state = State::Working;
		ResumeThread(thread);
	#endif
	}

	bool ThreadExecutor::IsExecuting()
	{
		return (state != State::Idle);
	}

	void ThreadExecutor::Terminate()
	{
		if (state != State::Working)
		{
			return;
		}

		state = State::Finishing;

		while (state == State::Finishing)
		{
			Sleep(20);
		}
	}

	void ThreadExecutor::Sleep(int mili_sec)
	{
	#ifdef PLATFORM_WIN
		::Sleep(mili_sec);
	#endif
	}

	#ifdef PLATFORM_WIN
	DWORD WINAPI ThreadExecutor::Entry(void* arg)
	{
		ThreadExecutor* executor = (ThreadExecutor*)arg;
		(executor->caller->*executor->call)();
		executor->state = State::Idle;

		return 0;
	}
	#endif
}
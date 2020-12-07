
#pragma once

#ifdef PLATFORM_WIN
#include <windows.h>
#endif

/**
\ingroup gr_code_common
*/

namespace Oak
{
	/**
	\brief Wrapper around mutex

	This class wraps mutex and allows to work with mutex via platform independent inteface.

	*/

	class CriticalSection
	{
	public:
		CriticalSection();
		~CriticalSection();

		/**
		\brief Lock mutex
		*/

		void Enter();

		/**
		\brief Unlock mutex
		*/

		void UnLock();
	#ifndef DOXYGEN_SKIP
	private:
	#ifdef PLATFORM_WIN
		CRITICAL_SECTION critSection;
	#endif
	#endif
	};

	class ThreadCaller
	{
	public:
		typedef void (ThreadCaller::*Delegate)();

		virtual ~ThreadCaller() {};
	};

	/**
	\brief Wrapper around threads

	This class implements work with thread via platform independent inteface.

	*/

	class ThreadExecutor
	{
	#ifndef DOXYGEN_SKIP
		enum class State
		{
			Idle,
			Working,
			Finishing
		};

		State state = State::Idle;

	#ifdef PLATFORM_WIN
		HANDLE thread = 0;

		static DWORD WINAPI Entry(void* arg);
	#endif

	#endif

	public:

		/**
		\brief Start execution of a thread
		*/
		void Execute(ThreadCaller* caller, ThreadCaller::Delegate call);

		/**
		\brief Check if thread is executing
		*/
		bool IsExecuting();

		/**
		\brief Stop execution of a thread
		*/
		void Terminate();

		static void Sleep(int mili_sec);

	#ifndef DOXYGEN_SKIP
	private:
		ThreadCaller* caller;
		ThreadCaller::Delegate call;
	#endif
	};
}
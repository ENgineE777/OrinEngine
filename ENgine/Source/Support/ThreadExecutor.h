
#pragma once

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#include "Defines.h"

namespace Orin
{
	/**
	\ingroup gr_code_common
	*/

	/**
	\brief Wrapper around mutex

	This class wraps mutex and allows to work with mutex via platform independent inteface.

	*/

	class CLASS_DECLSPEC CriticalSection
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
	#ifdef PLATFORM_WINDOWS
		CRITICAL_SECTION critSection;
	#endif
	#endif
	};

	class CLASS_DECLSPEC ThreadCaller
	{
	public:
		typedef void (ThreadCaller::*Delegate)();

		virtual ~ThreadCaller() {};
	};

	/**
	\brief Wrapper around threads

	This class implements work with thread via platform independent inteface.

	*/

	class CLASS_DECLSPEC ThreadExecutor
	{
	#ifndef DOXYGEN_SKIP
		enum class State
		{
			Idle,
			Working,
			Finishing
		};

		State state = State::Idle;

	#ifdef PLATFORM_WINDOWS
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
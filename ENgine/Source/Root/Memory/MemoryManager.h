
#pragma once

namespace Orin
{
	/**
	\ingroup gr_code_root_memory
	*/

	/**
	\brief FileInMemory

	This a memory manager whihc is keeping track of all allocated memory

	*/

	class MemoryManager
	{
		#ifndef DOXYGEN_SKIP
		struct Allocation
		{
			const char* file = nullptr;
			int line = 0;
			size_t size = 0;
			Allocation* next = nullptr;
			Allocation* prev = nullptr;
		};

		constexpr static int allocationSize = sizeof(Allocation);
		Allocation* head = nullptr;
		Allocation* tail = nullptr;

		size_t trackedUsedMemory = 0;
		size_t untrackedUsedMemory = 0;
		#endif

	public:

		/**
		\brief Requesst allocation of a memory

		\param[in] size Size of memory
		\param[in] file Name of a file from which allocation was requested
		\param[in] line Number of a line from which allocation was requested


		\return pointer to allocated memory
		*/
		void* Alloc(size_t size, const char* file, int line);

		/**
		\brief Requesst allocation of a memory

		\param[in] size Size of memory
		\param[in] alignment Aligment of an adress
		\param[in] file Name of a file from which allocation was requested
		\param[in] line Number of a line from which allocation was requested


		\return pointer to allocated memory
		*/
		void* AllignedAlloc(size_t size, size_t alignment, const char* file, int line);

		/**
		\brief Deallocate memory

		\param[in] ptr Pointer to previosly allocated memory

		\return pointer to allocated memory
		*/
		void Free(void* ptr);

		void LogMemory();

	private:
		#ifndef DOXYGEN_SKIP
		void FillAllocation(Allocation* allocation, size_t size, const char* file, int line);
		#endif
	};
}

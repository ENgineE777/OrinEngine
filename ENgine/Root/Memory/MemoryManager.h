
#pragma once

namespace Oak
{
	class MemoryManager
	{
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

	public:

		void* Alloc(size_t size, const char* file, int line);
		void* AllignedAlloc(size_t size, size_t alignment, const char* file, int line);
		void Free(void* ptr);

		void LogMemory();

	private:
		void FillAllocation(Allocation* allocation, size_t size, const char* file, int line);
	};
}

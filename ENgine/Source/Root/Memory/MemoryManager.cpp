
#include "Root/Root.h"
#include <jemalloc/jemalloc.h>

void* operator new (size_t size, const char* file, int line)
{
	return Oak::root.memory.Alloc(size, file, line);
}

void* operator new[](size_t size, const char* file, int line)
{
	return Oak::root.memory.Alloc(size, file, line);
}

void operator delete(void* ptr, const char* file, int line)
{
	je_free(ptr);
}

void operator delete[](void* ptr, const char* file, int line)
{
	je_free(ptr);
}

void* operator new(size_t size)
{
	return Oak::root.memory.Alloc(size, _FL_);
}

void* operator new[](size_t size)
{
	return Oak::root.memory.Alloc(size, _FL_);
}

void operator delete(void* ptr)
{
	Oak::root.memory.Free(ptr);
}

void operator delete[](void* ptr)
{
	Oak::root.memory.Free(ptr);
}

void operator delete(void* ptr, size_t)
{
	Oak::root.memory.Free(ptr);
}

void operator delete[](void* ptr, size_t)
{
	Oak::root.memory.Free(ptr);
}

namespace Oak
{
	void* MemoryManager::Alloc(size_t size, const char* file, int line)
	{
		char8_t* ptr = (char8_t*)je_malloc(size + allocationSize);

		FillAllocation((Allocation*)ptr, size, file, line);

		return ptr + allocationSize;
	}

	void* MemoryManager::AllignedAlloc(size_t size, size_t alignment, const char* file, int line)
	{
		char8_t* ptr = (char8_t*)je_aligned_alloc(alignment, size + allocationSize);

		FillAllocation((Allocation*)ptr, size, file, line);

		return ptr + allocationSize;
	}

	void MemoryManager::FillAllocation(Allocation* allocation, size_t size, const char* file, int line)
	{
		allocation->file = file;
		allocation->line = line;
		allocation->size = size;
		allocation->prev = nullptr;
		allocation->next = nullptr;

		if (file)
		{
			trackedUsedMemory += size;
		}
		else
		{
			untrackedUsedMemory += size;
		}

		if (head == nullptr)
		{
			head = allocation;
			tail = allocation;
		}
		else
		{
			tail->next = allocation;
			allocation->prev = tail;

			tail = allocation;
		}
	}

	void MemoryManager::Free(void* p)
	{
		if (p == nullptr)
		{
			return;
		}

		uint8_t* ptr = (uint8_t*)p;
		ptr-= allocationSize;

		Allocation* allocation = (Allocation*)ptr;

		if (allocation->file)
		{
			trackedUsedMemory -= allocation->size;
		}
		else
		{
			untrackedUsedMemory -= allocation->size;
		}

		Allocation* prev = allocation->prev;
		Allocation* next = allocation->next;

		if (prev != nullptr && next != nullptr)
		{
			prev->next = next;
			next->prev = prev;
		}
		else
		if (prev != nullptr && next == nullptr)
		{
			prev->next = nullptr;
			tail = prev;
		}
		else
		if (prev == nullptr && next != nullptr)
		{
			next->prev = nullptr;
			head = next;
		}
		else
		{
			head = nullptr;
			tail = nullptr;
		}

		je_free(ptr);
	}

	void MemoryManager::LogMemory()
	{
		Allocation* allocation = head;

		float trackedMB = (float)trackedUsedMemory / (1024 * 1024);
		float untrackedMB = (float)untrackedUsedMemory / (1024 * 1024);

		root.Log("Memory", "Total used memory: %4.3f MB (tracked %4.3f MB / untracked %4.3f MB)", (trackedMB + untrackedMB), trackedMB, untrackedMB);

		while (allocation)
		{
			if (allocation->file)
			{
				root.Log("Memory", "Allocated %i from %s, %i", allocation->size, allocation->file, allocation->line);
			}

			allocation = allocation->next;
		}
	}
}
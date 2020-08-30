
#pragma once

#include <stdint.h>
#include <string.h>

namespace Oak
{
	class FileInMemory
	{
		#ifndef DOXYGEN_SKIP
		uint8_t* buffer = nullptr;
		int32_t size = 0;
		uint8_t* ptr;
		#endif

	public:

		FileInMemory() = default;
		~FileInMemory();

		bool Load(const char* name);

		uint8_t* GetData();
		int GetSize();

		void Read(void* dest, int len);
		void Skip(int len);

		void* GetPtr();

	protected:
		void Release();
	};
}

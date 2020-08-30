
#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h> 

namespace Oak
{
	class File
	{
		uint8_t* data_ptr;
		uint8_t* ptr;

		FILE* file;
	
		uint32_t size;

	public:

		enum class ModeType
		{
			read = 0,
			write = 1,
			writeText = 2,
			append = 3,
			appendText = 4
		};

		enum class SeekType
		{
			seekSet = 0,
			seekCur = 1,
			seekEnd = 2
		};

		File() = default;
		~File();

		bool Load(const char* name, File::ModeType mode);

		int GetSize();

		void Write(const void* data, int size);
		void Print(const char *format, ...);

		uint32_t Read(void* data, int size);
		void Read(char* str);
		uint32_t Seek(uint32_t offset, File::SeekType seek);
		uint32_t GetCurrentPos();

	protected:
		void  Release();
	};
}




#pragma once

#include <stdio.h>

namespace Oak
{
	class Files
	{
		friend class File;
		friend class FileInMemory;
		friend class JsonWriter;

	public:

		void Init();

		#ifdef PLATFORM_WIN
		bool IsFileExist(const char* name);
		void CreateFolder(const char* path);
		void DeleteFolder(const char* path);
		void CpyFolder(const char* path, const char* destPath);
		bool CpyFile(const char* srcPath, const char* destPath);
		#endif

	private:

		FILE* FileOpen(const char* path, const char* mode);
	};
}

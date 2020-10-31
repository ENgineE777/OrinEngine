
#pragma once

#include <stdio.h>
#include "JsonReader.h"
#include "JsonWriter.h"

namespace Oak
{
	/**
	\ingroup gr_code_root_files
	*/

	/**
	\brief Files

	This service allows to open files. It is neccesery to work via this service with files because
	pathes are diferent on each supported platform.

	*/

	class Files
	{
		friend class File;
		friend class FileInMemory;
		friend class JsonWriter;

	public:

		#ifndef DOXYGEN_SKIP
		bool Init();
		#endif

		#ifdef PLATFORM_WIN
		/**
		\brief Check if file exist

		\param[in] name Full path of a file

		\return True will be returned if file exist. Otherwise it returns false.
		*/
		bool IsFileExist(const char* name);

		/**
		\brief Create a folder

		\param[in] name Full path of a folder

		*/
		void CreateFolder(const char* path);

		/**
		\brief Delete a folder

		\param[in] name Full path of a folder

		*/
		void DeleteFolder(const char* path);

		/**
		\brief Create a folder

		\param[in] name Full path of a folder
		\param[in] name Full path to a destination

		*/
		void CpyFolder(const char* path, const char* destPath);

		/**
		\brief Copy a file

		\param[in] name Full path of a file
		\param[in] name Full path to a destination 

		\return True will be returned if file was copied. Otherwise it returns false.

		*/
		bool CpyFile(const char* srcPath, const char* destPath);
		#endif

	private:
		#ifndef DOXYGEN_SKIP
		FILE* FileOpen(const char* path, const char* mode);
		#endif
	};
}

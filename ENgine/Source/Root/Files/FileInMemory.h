
#pragma once

#include <stdint.h>
#include <string.h>
#include "Support/Defines.h"

namespace Orin
{
	/**
	\ingroup gr_code_root_files
	*/

	/**
	\brief FileInMemory

	This class helpfull for loading whole file straight into memory.
	Just call Load and whole file will be loaded. After that it is
	to read data by small chunks via call FileInMemory::Read

	*/

	class CLASS_DECLSPEC FileInMemory
	{
		#ifndef DOXYGEN_SKIP
		uint8_t* buffer = nullptr;
		int32_t size = 0;
		uint8_t* ptr = nullptr;
		#endif

	public:

		FileInMemory() = default;
		~FileInMemory();

		/**
		\brief Load whole file into memory

		\param[in] name Full path of a file

		\return True will be returned if file seccefully loaded. Otherwise it returns false.
		*/
		bool Load(const char* name);

		/**
		\brief Get pointer to internal buffer which stores loaded file

		\return pointer to internal buffer
		*/
		uint8_t* GetData();

		/**
		\brief Get size of a loaded file

		\return size of a loaded file
		*/
		int GetSize();

		/**
		\brief Copy chunk of data and move position of a reading for amount of size of chunk

		\param[in] dest pointer to destination
		\param[in] len size of chunk which will be copied
		*/
		void Read(void* dest, int len);

		/**
		\brief Move position of a reading

		\param[in] len Amount of moving of position of a reading
		*/
		void Skip(int len);

		/**
		\brief Get pointer to current position of reading
		*/
		uint8_t* GetPtr();

#ifndef DOXYGEN_SKIP
	protected:
		void Release();
#endif
	};
}

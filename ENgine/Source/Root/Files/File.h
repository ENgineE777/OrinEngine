
#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h> 

namespace Oak
{
	/**
	\ingroup gr_code_root_files
	*/

	/**
	\brief File

	This class is a wrapper around FILE

	*/

	class CLASS_DECLSPEC File
	{
		#ifndef DOXYGEN_SKIP
		uint8_t* data_ptr = nullptr;
		uint8_t* ptr = nullptr;

		FILE* file = nullptr;
	
		uint32_t size;
		#endif

	public:

		enum class CLASS_DECLSPEC ModeType
		{
			Read = 0 /*!< Read access mode */,
			Write = 1 /*!< Write access mode */,
			WriteText = 2 /*!< Write text access mode */,
			Append = 3 /*!< Append to the end of a file */,
			AppendText = 4 /*!< Append to the end of a text file */
		};

		enum class CLASS_DECLSPEC SeekType
		{
			SeekSet = 0 /*!< Beginning of file */,
			SeekCur = 1 /*!< Current position of the file pointer */,
			SeekEnd = 2 /*!< End of file */
		};

		File() = default;
		~File();

		/**
		\brief Open file

		\param[in] name Full path of a file
		\param[in] mode File access mode

		\return True will be returned if file successfully opened. Otherwise it returns false.
		*/

		bool Open(const char* name, File::ModeType mode);

		/**
		\brief Get size of an opened file

		\return size of an opened file
		*/

		int GetSize();

		/**
		\brief Write a binary data

		\param[in] data Pointer to a data
		\param[in] size Size in bytes of a data
		*/

		void Write(const void* data, int size);

		/**
		\brief Write formated null terminated string

		\param[in] format pf a string
		*/

		void Print(const char *format, ...);

		/**
		\brief Read a binary data

		\param[in] data Pointer to a data
		\param[in] size Size in bytes of a data

		\return Size of readed data
		*/

		uint32_t Read(void* data, int size);

		/**
		\brief Read a null terminated string

		\param[in] str Pointer to a null terminated string
		*/

		void Read(char* str);

		/**
		\brief Sets the position indicator associated with the stream to a new position.

		\param[in] offset Offset from beginig of a file
		\param[in] seek Seek type

		\return Size of readed data
		*/

		uint32_t Seek(uint32_t offset, File::SeekType seek);

		/**
		\brief Returns the current value of the position indicator of the stream.

		\return On success, the current value of the position indicator is returned. On failure, -1L is returned, and errno is set to a system-specific positive value.
		*/
		uint32_t GetCurrentPos();

		#ifndef DOXYGEN_SKIP
	protected:
		void  Release();
		#endif
	};
}



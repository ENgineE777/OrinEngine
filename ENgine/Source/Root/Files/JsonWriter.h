
#pragma once

#include <stdio.h>
#include "Support/Support.h"
#include <stdint.h>
#include <EASTL/string.h>

namespace Orin
{
	/**
	\ingroup gr_code_root_files
	*/

	/**
	\brief JsonWriter

	This is a helper class for reading kson file

	*/

	class CLASS_DECLSPEC JsonWriter
	{
		FILE* file = nullptr;
		int offset = 0;
		int depth = 0;
		bool block_started[32];

	public:

		JsonWriter();
		~JsonWriter();

		/**
		\brief Start writing a json file

		\param[in] name Full path of a file

		\return True will be returned if file was successfully opened for writing. Otherwise it returns false.
		*/

		bool Start(const char* name);

		void StartArray(const char* name);
		void FinishArray();

		void StartBlock(const char* name);
		void FinishBlock();

		void Write(const char* name, eastl::string& val);
		void Write(const char* name, const char* val);
		void Write(const char* name, bool val);
		void Write(const char* name, float val);
		void Write(const char* name, uint16_t& val);
		void Write(const char* name, int val);
		void Write(const char* name, uint32_t val);
		void Write(const char* name, int64_t val);
		void Write(const char* name, Math::Vector2& val);
		void Write(const char* name, Math::Vector3& val);
		void Write(const char* name, Math::Vector4& val);
		void Write(const char* name, Math::Matrix& val);
		void Write(const char* name, Color& val);
		void Write(const char* name, Math::Quaternion& val);

	protected:

		#ifndef DOXYGEN_SKIP

		void Close();
		void AddComma();
		void WriteFormatedStr(bool addComa, const char* str, ...);
		void WriteStr(const char* str);

		#endif
	};
}




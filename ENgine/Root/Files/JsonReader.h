
#pragma once

#include "json.h"
#include "Support/Support.h"
#include <EASTL/string.h>
#include "FileInMemory.h"

namespace Oak
{
	/**
	\ingroup gr_code_root_files
	*/

	/**
	\brief JSONReader

	This is a helper class for reading kson file

	*/

	class JsonReader
	{
		#ifndef DOXYGEN_SKIP
		block_allocator allocator;
		json_value* root = nullptr;

		FileInMemory file;

		int curDepth = 0;
		json_value* nodes[32];
		json_value* curNode = nullptr;
		#endif

	public:

		JsonReader();
		~JsonReader();

		/**
		\brief Parse json file

		\param[in] name Full path of a file

		\return True will be returned if file successfully opened and parsed. Otherwise it returns false.
		*/
		bool Parse(const char* name);

		bool EnterBlock(const char* name);
		void LeaveBlock();

		bool IsString(const char* name);
		bool Read(const char* name, char* val, int valLen);
		bool Read(const char* name, eastl::string& val);
		bool Read(const char* name, bool& val);
		bool Read(const char* name, float& val);
		bool Read(const char* name, uint16_t& val);
		bool Read(const char* name, int& val);
		bool Read(const char* name, uint32_t& val);
		bool Read(const char* name, int64_t& val);
		bool Read(const char* name, Math::Vector2& val);
		bool Read(const char* name, Math::Vector3& val);
		bool Read(const char* name, Math::Matrix& val);
		bool Read(const char* name, Color& val);
		bool Read(const char* name, Math::Quaternion& val);

	protected:

		#ifndef DOXYGEN_SKIP
		json_value* FindValue(const char* name);
		#endif
	};
}




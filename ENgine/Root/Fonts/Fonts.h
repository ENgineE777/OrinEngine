#pragma once

#include "FontRef.h"
#include <eastl/map.h>

namespace Oak
{
	class Fonts
	{
		friend class FontRes;
		eastl::map<eastl::string, FontRes*> fonts;

		struct FontVertex
		{
			Math::Vector3 pos;
			Math::Vector2 uv;
		};

		ProgramRef fntProg;
		DataBufferRef vbuffer;
		VertexDeclRef vdecl;

	public:

		FontRef LoadFont(const char* file_name, bool is_bold, bool is_italic, int height, const char* file, int line);

		bool Init();
		void Release();
	};
}
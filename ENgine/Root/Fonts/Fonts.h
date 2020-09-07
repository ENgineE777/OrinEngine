#pragma once

#include "Font.h"
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

		Program* fntProg = nullptr;
		DataBuffer* vbuffer = nullptr;
		VertexDecl* vdecl = nullptr;

	public:

		Font* LoadFont(const char* file_name, bool is_bold, bool is_italic, int height);

		bool Init();
		void Release();

	protected:
		void DeleteRes(FontRes* res);
	};
}
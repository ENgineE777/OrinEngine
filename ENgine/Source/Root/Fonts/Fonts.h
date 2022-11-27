#pragma once

#include "FontRef.h"
#include <eastl/map.h>

namespace Oak
{
	/**
	\ingroup gr_code_root_font
	*/

	/**
	\brief Fonts

	This is the manager of fonts. System supports TrueType fonts.

	*/

	class Fonts
	{
		#ifndef DOXYGEN_SKIP
		friend class FontRes;
		eastl::map<eastl::string, FontRes*> fonts;

		struct FontVertex
		{
			Math::Vector3 pos;
			Math::Vector2 uv;
		};

		RenderTechniqueRef fntProg;
		DataBufferRef vbuffer;
		VertexDeclRef vdecl;
		#endif

	public:

		/**
		\brief Load a font

		\param[in] file_name Full path to a file with TrueType font
		\param[in] is_bold Should a font has bold style
		\param[in] is_italic Should a font has italic style
		\param[in] height Hieght of font iin pixels
		\param[in] file Name of a file from which ctreation was requested
		\param[in] line Number of a line from which ctreation was requested

		\return Instance of a font. Nullptr will returned if it is impossible to load a font.
		*/
		FontRef LoadFont(const char* file_name, bool is_bold, bool is_italic, int height, const char* file, int line);

		#ifndef DOXYGEN_SKIP
		bool Init();
		void Release();
		#endif
	};
}
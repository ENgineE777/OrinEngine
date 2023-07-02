#pragma once

#include "FontRes.h"
#include "Support/PointerRef.h"

namespace Orin
{
	/**
	\ingroup gr_code_root_font
	*/

	/**
	\brief Font

	This is a inctance of font. Instance allow to draw text with selected font.

	*/

	class CLASS_DECLSPEC FontRef : public PointerRef<FontRes>
	{
		friend class Fonts;

	public: 

		#ifndef DOXYGEN_SKIP
		FontRef() = default;

		FontRef(FontRes* setPtr, const char* file, int line);
		#endif

		/**
			\brief Gather a info line breaks in a particula text

			\param[out] line_breaks Array of postions of line breakes
			\param[in] text Context of a text
			\param[in] width Maximum width of line
		*/
		float GetLineBreak(eastl::vector<FontRes::LineBreak>& line_breaks, const char* text, int width);

		/**
		\brief Print a text with precalculated postions of line breaks

			\param[out] line_breaks Array of postions of line breakes
			\param[in] transform Transformation of a position
			\param[in] font_scale Scale of a font during rendering of a text
			\param[in] color Color of a font during rendering of a text
			\param[in] text Context of a text

		*/
		void Print(eastl::vector<FontRes::LineBreak>& line_breaks, Math::Matrix& transform, float font_scale, Color color, const char* text);

		/**
		\brief Print a text

			\param[in] transform Transformation of a position
			\param[in] font_scale Scale of a font during rendering of a text
			\param[in] color Color of a font during rendering of a text
			\param[in] text Context of a text

		*/
		void Print(Math::Matrix& transform, float font_scale, Color color,  const char* text);
	};
}

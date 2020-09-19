#pragma once

#include "FontRes.h"
#include "Support/PointerRef.h"

namespace Oak
{
	class FontRef : public PointerRef<FontRes>
	{
		friend class Fonts;

	public: 

		FontRef() = default;

		FontRef(FontRes* setPtr, const char* file, int line);

		float GetLineBreak(eastl::vector<FontRes::LineBreak>& line_breaks, const char* text, int width);

		void Print(eastl::vector<FontRes::LineBreak>& line_breaks, Math::Matrix& transform, float font_scale, Color color, const char* text);

		void Print(Math::Matrix& transform, float font_scale, Color color,  const char* text);
	};
}

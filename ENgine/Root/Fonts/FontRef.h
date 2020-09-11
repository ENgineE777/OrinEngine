#pragma once

#include "FontRes.h"

namespace Oak
{
	class FontRef
	{
		friend class Fonts;

		FontRes* res;

		const char* file = nullptr;
		int line = 0;
		FontRef* flMarker = nullptr;

	public: 

		FontRef() = default;
		FontRef(const FontRef& ref);
		~FontRef();

		FontRef& operator=(const FontRef& ref);

		float GetLineBreak(eastl::vector<FontRes::LineBreak>& line_breaks, const char* text, int width);

		void Print(eastl::vector<FontRes::LineBreak>& line_breaks, Math::Matrix& transform, float font_scale, Color color, const char* text);

		void Print(Math::Matrix& transform, float font_scale, Color color,  const char* text);

	protected:
		void ReleaseRef();
	};
}

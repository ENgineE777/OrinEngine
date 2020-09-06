#pragma once

#include "FontRes.h"

namespace Oak
{
	class Font
	{
		FontRes* res;

	public: 

		Font(FontRes* res);

		float GetLineBreak(eastl::vector<FontRes::LineBreak>& line_breaks, const char* text, int width);

		void Print(eastl::vector<FontRes::LineBreak>& line_breaks, Math::Matrix& transform, float font_scale, Color color, const char* text);

		void Print(Math::Matrix& transform, float font_scale, Color color,  const char* text);

		virtual void Release();
	};
}

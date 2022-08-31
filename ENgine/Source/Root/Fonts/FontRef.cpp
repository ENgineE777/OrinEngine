#include "FontRef.h"

namespace Oak
{
	FontRef::FontRef(FontRes* setPtr, const char* file, int line) : PointerRef<FontRes>(setPtr, file, line)
	{

	}

	float FontRef::GetLineBreak(eastl::vector<FontRes::LineBreak>& line_breaks, const char* text, int width)
	{
		return Get() ? Get()->GetLineBreak(line_breaks, text, width) : 0.0f;
	}

	void FontRef::Print(eastl::vector<FontRes::LineBreak>& line_breaks, Math::Matrix& transform, float font_scale, Color color, const char* text)
	{
		if (!text[0] || Get() == nullptr) return;

		Get()->Print(line_breaks, transform, font_scale, color, text);
	}

	void FontRef::Print(Math::Matrix& transform, float font_scale, Color color, const char* text)
	{
		if (!text[0] || Get() == nullptr) return;

		eastl::vector<FontRes::LineBreak> lines;
		lines.push_back(FontRes::LineBreak());
		Get()->Print(lines, transform, font_scale, color, text);
	}
}
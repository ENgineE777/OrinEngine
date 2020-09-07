#include "Font.h"

namespace Oak
{
	Font::Font(FontRes* set_res)
	{
		res = set_res;
	}

	float Font::GetLineBreak(eastl::vector<FontRes::LineBreak>& line_breaks, const char* text, int width)
	{
		return res->GetLineBreak(line_breaks, text, width);
	}

	void Font::Print(eastl::vector<FontRes::LineBreak>& line_breaks, Math::Matrix& transform, float font_scale, Color color, const char* text)
	{
		if (!text[0]) return;

		res->Print(line_breaks, transform, font_scale, color, text);
	}

	void Font::Print(Math::Matrix& transform, float font_scale, Color color, const char* text)
	{
		if (!text[0]) return;

		eastl::vector<FontRes::LineBreak> lines;
		lines.push_back(FontRes::LineBreak());
		res->Print(lines, transform, font_scale, color, text);
	}

	void Font::Release()
	{
		RELEASE(res)
		delete this;
	}
}
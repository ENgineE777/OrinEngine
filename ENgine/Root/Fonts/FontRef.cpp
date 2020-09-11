#include "FontRef.h"

namespace Oak
{
	FontRef::FontRef(const FontRef& ref)
	{
		res = ref.res;

		if (res)
		{
			res->refCounter++;
		}

		file = ref.file;
		line = ref.line;
		flMarker = new(file, line) FontRef();
	}

	FontRef::~FontRef()
	{
		ReleaseRef();
	}

	float FontRef::GetLineBreak(eastl::vector<FontRes::LineBreak>& line_breaks, const char* text, int width)
	{
		return res ? res->GetLineBreak(line_breaks, text, width) : 0.0f;
	}

	void FontRef::Print(eastl::vector<FontRes::LineBreak>& line_breaks, Math::Matrix& transform, float font_scale, Color color, const char* text)
	{
		if (!text[0] || res == nullptr) return;

		res->Print(line_breaks, transform, font_scale, color, text);
	}

	void FontRef::Print(Math::Matrix& transform, float font_scale, Color color, const char* text)
	{
		if (!text[0] || res == nullptr) return;

		eastl::vector<FontRes::LineBreak> lines;
		lines.push_back(FontRes::LineBreak());
		res->Print(lines, transform, font_scale, color, text);
	}

	FontRef& FontRef::operator=(const FontRef& ref)
	{
		res = ref.res;

		if (res)
		{
			res->refCounter++;
		}

		file = ref.file;
		line = ref.line;
		flMarker = new(file, line) FontRef();

		return *this;
	}

	void FontRef::ReleaseRef()
	{
		DELETE_PTR(flMarker)

		if (res)
		{
			res->refCounter--;

			if (res->refCounter == 0)
			{
				res->Release();
			}

			res = nullptr;
		}
	}
}
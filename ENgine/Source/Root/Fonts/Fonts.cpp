#include "Fonts.h"
#include "Root/Root.h"

namespace Orin
{
	class FontProgram : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "font_vs.shd"; };
		virtual const char* GetPsName() { return "font_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetDepthTest(false);
		}
	};

	bool Fonts::Init()
	{
		fntProg = root.render.GetRenderTechnique<FontProgram>(_FL_);

		vbuffer = root.render.GetDevice()->CreateBuffer(6 * 1000, sizeof(Fonts::FontVertex), _FL_);

		VertexDecl::ElemDesc desc[] = { { ElementType::Float3, ElementSemantic::Position, 0 }, { ElementType::Float2, ElementSemantic::Texcoord, 0 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(2, desc, _FL_);

		return true;
	}

	FontRef Fonts::LoadFont(const char* file_name, bool is_bold, bool is_italic, int height, const char* file, int line)
	{
		if (!file_name[0]) return FontRef();

		if (height > 100)
		{
			height = 100;
		}

		char nm[512];
		char font_path[128];
		font_path[0] = 0;

		FontRes* res = nullptr;

		const char* prefix = "";

		if (is_bold && !is_italic)
		{
			prefix = "_b";
		}
		else
		if (!is_bold && is_italic)
		{
			prefix = "_i";
		}
		else
		if (is_bold && is_italic)
		{
			prefix = "_bi";
		}

		StringUtils::Printf(font_path, 128, "%s%s", file_name, prefix);
		StringUtils::Printf(nm, 512, "%s%i%s", font_path, height, prefix);

		if (fonts.count(nm) > 0)
		{
			res = fonts[nm];
		}

		if (!res)
		{
			res = NEW FontRes(nm, font_path, height);

			if (!res->Load())
			{
				res->Release();

				return FontRef();
			}

			fonts[nm] = res;
		}

		return FontRef(res, file, line);
	}

	void Fonts::Release()
	{
		vbuffer.ReleaseRef();
		vdecl.ReleaseRef();
		fntProg.ReleaseRef();
	}
}


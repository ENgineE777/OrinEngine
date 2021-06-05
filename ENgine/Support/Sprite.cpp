
#include "Sprite.h"
#include "Root/Root.h"

namespace Oak::Sprite
{
	class QuadProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "sprite_vs.shd"; };
		virtual const char* GetPsName() { return "sprite_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	CLASSREGEX(Program, QuadProgram, QuadProgram, "QuadProgram")
	CLASSREGEX_END(Program, QuadProgram)

	ProgramRef quadPrg;
	VertexDeclRef vdecl;
	DataBufferRef buffer;

	float pixelsPerUnit = 50.0f;
	float pixelsPerUnitInvert = 1.0f / pixelsPerUnit;
	float pixelsHeight = 1080.0f;

	void Init()
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float2, ElementSemantic::Position, 0 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(1, desc, _FL_);

		int stride = sizeof(Math::Vector2);
		buffer = root.render.GetDevice()->CreateBuffer(4, stride, _FL_);

		Math::Vector2* v = (Math::Vector2*)buffer->Lock();

		v[0] = Math::Vector2(0.0f, 1.0f);
		v[1] = Math::Vector2(1.0f, 1.0f);
		v[2] = Math::Vector2(0.0f, 0.0f);
		v[3] = Math::Vector2(1.0f, 0.0f);

		buffer->Unlock();

		quadPrg = root.render.GetProgram("QuadProgram", _FL_);
	}

	void Draw(Texture* texture, Color clr, Math::Matrix trans, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 uv, Math::Vector2 duv)
	{
		root.render.GetDevice()->SetVertexBuffer(0, buffer);
		root.render.GetDevice()->SetVertexDecl(vdecl);

		root.render.GetDevice()->SetProgram(quadPrg);

		Device::Viewport viewport;
		root.render.GetDevice()->GetViewport(viewport);

		Math::Vector4 params[3];
		params[0] = Math::Vector4(pos.x, pos.y, size.x, size.y);
		params[1] = Math::Vector4(uv.x, uv.y, duv.x, duv.y);
		params[2] = Math::Vector4((float)root.render.GetDevice()->GetWidth(), (float)root.render.GetDevice()->GetHeight(), 0.5f, pixelsPerUnitInvert);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::WrldViewProj, view_proj);

		trans.Pos() *= pixelsPerUnitInvert;

		quadPrg->SetVector(ShaderType::Vertex, "desc", &params[0], 3);
		quadPrg->SetMatrix(ShaderType::Vertex, "trans", &trans, 1);
		quadPrg->SetMatrix(ShaderType::Vertex, "view_proj", &view_proj, 1);
		quadPrg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&clr.r, 1);
		quadPrg->SetTexture(ShaderType::Pixel, "diffuseMap", texture ? texture : root.render.GetWhiteTexture());

		root.render.GetDevice()->Draw(PrimitiveTopology::TriangleStrip, 0, 2);
	}

	void Release()
	{
		vdecl.ReleaseRef();
		buffer.ReleaseRef();
		quadPrg.ReleaseRef();
	}
}




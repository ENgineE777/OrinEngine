
#include "Sprite.h"
#include "Root/Root.h"

namespace Oak
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

	class QuadProgramNoZ : public QuadProgram
	{
	public:

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	CLASSREGEX(Program, QuadProgram, QuadProgram, "QuadProgram")
	CLASSREGEX_END(Program, QuadProgram)

	CLASSREGEX(Program, QuadProgramNoZ, QuadProgramNoZ, "QuadProgramNoZ")
	CLASSREGEX_END(Program, QuadProgramNoZ)

	VertexDeclRef Sprite::_vdecl;
	DataBufferRef Sprite::_buffer;

	float Sprite::_pixelsPerUnit = 50.0f;
	float Sprite::_pixelsPerUnitInvert = 1.0f / _pixelsPerUnit;
	float Sprite::_pixelsHeight = 1080.0f;
	Math::Vector2 Sprite::camPos;

	ProgramRef Sprite::quadPrg;
	ProgramRef Sprite::quadPrgNoZ;

	void Sprite::SetData(float pixelsHeight, float pixelsPerUnit)
	{
		_pixelsHeight = pixelsHeight;
		_pixelsPerUnit = pixelsPerUnit;
		_pixelsPerUnitInvert = 1.0f / pixelsPerUnit;
	}

	void Sprite::Init()
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float2, ElementSemantic::Position, 0 } };
		_vdecl = root.render.GetDevice()->CreateVertexDecl(1, desc, _FL_);

		int stride = sizeof(Math::Vector2);
		_buffer = root.render.GetDevice()->CreateBuffer(4, stride, _FL_);

		Math::Vector2* v = (Math::Vector2*)_buffer->Lock();

		v[0] = Math::Vector2(0.0f, 1.0f);
		v[1] = Math::Vector2(1.0f, 1.0f);
		v[2] = Math::Vector2(0.0f, 0.0f);
		v[3] = Math::Vector2(1.0f, 0.0f);

		_buffer->Unlock();

		quadPrg = root.render.GetProgram("QuadProgram", _FL_);
		quadPrgNoZ = root.render.GetProgram("QuadProgramNoZ", _FL_);
	}

	void Sprite::Draw(Texture* texture, Color clr, Math::Matrix trans, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 uv, Math::Vector2 duv, ProgramRef prg)
	{
		root.render.GetDevice()->SetVertexBuffer(0, _buffer);
		root.render.GetDevice()->SetVertexDecl(_vdecl);

		root.render.GetDevice()->SetProgram(prg);

		Device::Viewport viewport;
		root.render.GetDevice()->GetViewport(viewport);

		Math::Vector4 params[3];
		params[0] = Math::Vector4(pos.x, pos.y, size.x, size.y);
		params[1] = Math::Vector4(uv.x, uv.y, duv.x, duv.y);
		params[2] = Math::Vector4((float)root.render.GetDevice()->GetWidth(), (float)root.render.GetDevice()->GetHeight(), 0.5f, _pixelsPerUnitInvert);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::WrldViewProj, view_proj);

		trans.Pos() *= _pixelsPerUnitInvert;

		prg->SetVector(ShaderType::Vertex, "desc", &params[0], 3);
		prg->SetMatrix(ShaderType::Vertex, "trans", &trans, 1);
		prg->SetMatrix(ShaderType::Vertex, "view_proj", &view_proj, 1);
		prg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&clr.r, 1);
		prg->SetTexture(ShaderType::Pixel, "diffuseMap", texture ? texture : root.render.GetWhiteTexture());

		root.render.GetDevice()->Draw(PrimitiveTopology::TriangleStrip, 0, 2);
	}

	void Sprite::DebugLine(const Math::Vector3& from, const Math::Vector3& to, const Color& color)
	{
		root.render.DebugLine(ToUnits(from), color, ToUnits(to), color, false);
	}

	void Sprite::DebugSphere(const Math::Vector3& pos, float radius, const Color& color)
	{
		root.render.DebugSphere(ToUnits(pos), color, ToUnits(radius), true /* full_shade */);
	}

	void Sprite::DebugRect(const Math::Vector2& p1, const Math::Vector2& p2, Color color)
	{
		DebugLine(Math::Vector2(p1.x, p1.y), Math::Vector2(p2.x, p1.y), color);
		DebugLine(Math::Vector2(p2.x, p1.y), Math::Vector2(p2.x, p2.y), color);
		DebugLine(Math::Vector2(p2.x, p2.y), Math::Vector2(p1.x, p2.y), color);
		DebugLine(Math::Vector2(p1.x, p2.y), Math::Vector2(p1.x, p1.y), color);
	}

	void Sprite::Release()
	{
		_vdecl.ReleaseRef();
		_buffer.ReleaseRef();
		quadPrg.ReleaseRef();
		quadPrgNoZ.ReleaseRef();
	}
}




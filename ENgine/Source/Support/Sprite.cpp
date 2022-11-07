
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

	class PolygonProgram : public Program
	{
	public:
		virtual const char* GetVsName() { return "polygon_vs.shd"; };
		virtual const char* GetPsName() { return "polygon_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
			root.render.GetDevice()->SetDepthWriting(false);
		};
	};

	class PolygonProgramNoZ : public PolygonProgram
	{
	public:

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
			root.render.GetDevice()->SetDepthWriting(false);
		};
	};

	CLASSREGEX(Program, QuadProgram, QuadProgram, "QuadProgram")
	CLASSREGEX_END(Program, QuadProgram)

	CLASSREGEX(Program, QuadProgramNoZ, QuadProgramNoZ, "QuadProgramNoZ")
	CLASSREGEX_END(Program, QuadProgramNoZ)

	CLASSREGEX(Program, PolygonProgram, PolygonProgram, "PolygonProgram")
	CLASSREGEX_END(Program, PolygonProgram)

	CLASSREGEX(Program, PolygonProgramNoZ, PolygonProgramNoZ, "PolygonProgramNoZ")
	CLASSREGEX_END(Program, PolygonProgramNoZ)

	VertexDeclRef Sprite::_vdecl;
	DataBufferRef Sprite::_quadBuffer;
	DataBufferRef Sprite::_polygonBuffer;

	float Sprite::_pixelsPerUnit = 50.0f;
	float Sprite::_pixelsPerUnitInvert = 1.0f / _pixelsPerUnit;
	float Sprite::_pixelsHeight = 1080.0f;
	Math::Vector2 Sprite::_camPos;

	ProgramRef Sprite::quadPrg;
	ProgramRef Sprite::quadPrgNoZ;

	ProgramRef Sprite::polygonPrg;
	ProgramRef Sprite::polygonPrgNoZ;

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
		_quadBuffer = root.render.GetDevice()->CreateBuffer(4, stride, _FL_);

		Math::Vector2* v = (Math::Vector2*)_quadBuffer->Lock();

		v[0] = Math::Vector2(0.0f, 1.0f);
		v[1] = Math::Vector2(1.0f, 1.0f);
		v[2] = Math::Vector2(0.0f, 0.0f);
		v[3] = Math::Vector2(1.0f, 0.0f);

		_quadBuffer->Unlock();

		_polygonBuffer = root.render.GetDevice()->CreateBuffer(128, stride, _FL_);

		quadPrg = root.render.GetProgram("QuadProgram", _FL_);
		quadPrgNoZ = root.render.GetProgram("QuadProgramNoZ", _FL_);

		polygonPrg = root.render.GetProgram("PolygonProgram", _FL_);
		polygonPrgNoZ = root.render.GetProgram("PolygonProgramNoZ", _FL_);
	}

	void Sprite::Draw(Texture* texture, Color clr, Math::Matrix trans, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 uv, Math::Vector2 duv, ProgramRef prg)
	{
		root.render.GetDevice()->SetVertexBuffer(0, _quadBuffer);
		root.render.GetDevice()->SetVertexDecl(_vdecl);

		root.render.GetDevice()->SetProgram(prg);

		Math::Vector4 params[2];
		params[0] = Math::Vector4(pos.x, pos.y, size.x, size.y) * _pixelsPerUnitInvert;
		params[1] = Math::Vector4(uv.x, uv.y, duv.x, duv.y);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::WrldViewProj, view_proj);

		prg->SetVector(ShaderType::Vertex, "desc", &params[0], 2);
		prg->SetMatrix(ShaderType::Vertex, "trans", &trans, 1);
		prg->SetMatrix(ShaderType::Vertex, "view_proj", &view_proj, 1);
		prg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&clr.r, 1);
		prg->SetTexture(ShaderType::Pixel, "diffuseMap", texture ? texture : root.render.GetWhiteTexture());

		root.render.GetDevice()->Draw(PrimitiveTopology::TriangleStrip, 0, 2);
	}

	void Sprite::DrawConvexPolygon(Math::Vector2* points, int pointsCount, Math::Matrix trans, Color clr, ProgramRef prg)
	{
		root.render.GetDevice()->SetVertexBuffer(0, _polygonBuffer);
		root.render.GetDevice()->SetVertexDecl(_vdecl);

		root.render.GetDevice()->SetProgram(prg);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::WrldViewProj, view_proj);

		Math::Vector2* v = (Math::Vector2*)_polygonBuffer->Lock();

		int triangleCount = pointsCount - 2;
		int index = 0;

		for (int i = 0; i < triangleCount; i++)
		{
			v[index++] = points[0] * _pixelsPerUnitInvert;
			v[index++] = points[i + 1] * _pixelsPerUnitInvert;
			v[index++] = points[i + 2] * _pixelsPerUnitInvert;
		}

		_polygonBuffer->Unlock();

		prg->SetMatrix(ShaderType::Vertex, "trans", &trans, 1);
		prg->SetMatrix(ShaderType::Vertex, "view_proj", &view_proj, 1);
		prg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&clr.r, 1);
		prg->SetTexture(ShaderType::Pixel, "diffuseMap", root.render.GetWhiteTexture());

		root.render.GetDevice()->Draw(PrimitiveTopology::TrianglesList, 0, pointsCount - 2);
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
		_quadBuffer.ReleaseRef();
		_polygonBuffer.ReleaseRef();
		quadPrg.ReleaseRef();
		quadPrgNoZ.ReleaseRef();
		polygonPrg.ReleaseRef();
		polygonPrg.ReleaseRef();
	}
}




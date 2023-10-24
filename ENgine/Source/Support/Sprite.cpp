
#include "Sprite.h"
#include "Root/Root.h"
#include "stb_sprintf.h"

namespace Orin
{
	class QuadRenderTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "sprite_vs.shd"; };
		virtual const char* GetPsName() { return "sprite_discard_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);			
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	class QuadRenderLightenTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "sprite_vs.shd"; };
		virtual const char* GetPsName() { return "sprite_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetBlendFunc(BlendArg::ArgSrcAlpha, BlendArg::ArgOne);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	class QuadRenderNoDepthWriteTechnique : public QuadRenderTechnique
	{
	public:
		virtual const char* GetPsName() { return "sprite_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	class QuadRenderNoZTechnique : public QuadRenderTechnique
	{
	public:
		virtual const char* GetPsName() { return "sprite_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};


	class QuadRenderShdNoZTechnique : public QuadRenderNoZTechnique
	{
	public:
		virtual const char* GetVsName() { return "sprite_vs_shd.shd"; };
		virtual const char* GetPsName() { return "sprite_ps_shd.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetBlendFunc(BlendArg::ArgSrcAlpha, BlendArg::ArgOne);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	static RenderTechniqueRef quadPrgShdNoZ;
	class PolygonRenderTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "polygon_vs.shd"; };
		virtual const char* GetPsName() { return "sprite_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
			root.render.GetDevice()->SetDepthWriting(false);
		};
	};

	class PolygonRenderNoZTechnique : public PolygonRenderTechnique
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

	VertexDeclRef Sprite::_vdecl;
	DataBufferRef Sprite::_quadBuffer;

	VertexDeclRef Sprite::_polygonVdecl;
	DataBufferRef Sprite::_polygonBuffer;

	float Sprite::_pixelsPerUnit = 50.0f;
	float Sprite::_pixelsPerUnitInvert = 1.0f / _pixelsPerUnit;
	float Sprite::_pixelsHeight = 1080.0f;
	Math::Vector2 Sprite::_camPos;
	float Sprite::_zoom = 1.0f;
	Math::Vector2 Sprite::_halfScreenSize;

	RenderTechniqueRef Sprite::quadPrg;
	RenderTechniqueRef Sprite::quadLightenPrg;
	RenderTechniqueRef Sprite::quadPrgNoZ;
	RenderTechniqueRef Sprite::quadPrgNoDepthWrite;
	RenderTechniqueRef Sprite::quadPrgShdNoZ;

	RenderTechniqueRef Sprite::polygonPrg;
	RenderTechniqueRef Sprite::polygonPrgNoZ;

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

		_quadBuffer = root.render.GetDevice()->CreateBuffer(4, sizeof(Math::Vector2), _FL_);

		Math::Vector2* v = (Math::Vector2*)_quadBuffer->Lock();

		v[0] = Math::Vector2(0.0f, 1.0f);
		v[1] = Math::Vector2(1.0f, 1.0f);
		v[2] = Math::Vector2(0.0f, 0.0f);
		v[3] = Math::Vector2(1.0f, 0.0f);

		_quadBuffer->Unlock();

		VertexDecl::ElemDesc polygonDesc[] = { { ElementType::Float2, ElementSemantic::Position, 0 }, { ElementType::Float2, ElementSemantic::Texcoord, 0 } };
		_polygonVdecl = root.render.GetDevice()->CreateVertexDecl(2, polygonDesc, _FL_);

		_polygonBuffer = root.render.GetDevice()->CreateBuffer(128, sizeof(PolygonVertex), _FL_);

		quadPrg = root.render.GetRenderTechnique<QuadRenderTechnique>(_FL_);
		quadLightenPrg = root.render.GetRenderTechnique<QuadRenderLightenTechnique>(_FL_);
		quadPrgNoDepthWrite = root.render.GetRenderTechnique<QuadRenderNoDepthWriteTechnique>(_FL_);
		quadPrgNoZ = root.render.GetRenderTechnique<QuadRenderNoZTechnique>(_FL_);
		quadPrgShdNoZ = root.render.GetRenderTechnique<QuadRenderShdNoZTechnique>(_FL_);
		polygonPrg = root.render.GetRenderTechnique<PolygonRenderTechnique> (_FL_);
		polygonPrgNoZ = root.render.GetRenderTechnique<PolygonRenderNoZTechnique>(_FL_);
	}

	void Sprite::Update()
	{
		_halfScreenSize.x = Sprite::GetPixelsHeight() / root.render.GetDevice()->GetAspect() * 0.5f;
		_halfScreenSize.y = Sprite::GetPixelsHeight() * 0.5f;
	}

	void Sprite::Draw(Texture* texture, Color clr, Math::Matrix trans, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 uv, Math::Vector2 duv, RenderTechniqueRef prg)
	{
		root.render.GetDevice()->SetVertexBuffer(0, _quadBuffer);
		root.render.GetDevice()->SetVertexDecl(_vdecl);

		root.render.GetDevice()->SetRenderTechnique(prg);

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

	void Sprite::DrawConvexPolygon(Texture* texture, PolygonVertex* points, int pointsCount, Math::Matrix trans, Color clr, RenderTechniqueRef prg)
	{
		root.render.GetDevice()->SetVertexBuffer(0, _polygonBuffer);
		root.render.GetDevice()->SetVertexDecl(_polygonVdecl);

		root.render.GetDevice()->SetRenderTechnique(prg);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::WrldViewProj, view_proj);

		PolygonVertex* v = (PolygonVertex*)_polygonBuffer->Lock();

		int triangleCount = pointsCount - 2;
		int index = 0;

		for (int i = 0; i < triangleCount; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				v[index] = points[j == 0 ? 0 : i + j];
				v[index].pos *= _pixelsPerUnitInvert;
				index++;
			}
		}

		_polygonBuffer->Unlock();

		prg->SetMatrix(ShaderType::Vertex, "trans", &trans, 1);
		prg->SetMatrix(ShaderType::Vertex, "view_proj", &view_proj, 1);
		prg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&clr.r, 1);
		prg->SetTexture(ShaderType::Pixel, "diffuseMap", texture ? texture : root.render.GetWhiteTexture());

		root.render.GetDevice()->Draw(PrimitiveTopology::TrianglesList, 0, pointsCount - 2);
	}

	bool Sprite::IsPointVisibile(Math::Vector2 point)
	{
		auto halfScreenSize = GetHalfScreenSize();

		if (_camPos.x - halfScreenSize.x < point.x && point.x < _camPos.x + halfScreenSize.x &&
			_camPos.y - halfScreenSize.y < point.y && point.y < _camPos.y + halfScreenSize.y)
		{
			return true;
		}

		return false;
	}

	bool Sprite::IsRectVisibile(Math::Vector2 p1, Math::Vector2 p2)
	{
		auto halfScreenSize = GetHalfScreenSize();

		return Math::IsRectsOverlap(p1, p2, _camPos - halfScreenSize, _camPos + halfScreenSize);
	}

	bool Sprite::IsTrinagleVisibile(Math::Vector2 p1, Math::Vector2 p2, Math::Vector2 p3)
	{
		auto halfScreenSize = GetHalfScreenSize();

		if (Math::IntersectTriangleTriangle(p1, p2, p3,
											_camPos + Math::Vector2(-halfScreenSize.x,  halfScreenSize.y),
											_camPos + Math::Vector2( halfScreenSize.x,  halfScreenSize.y),
											_camPos + Math::Vector2( halfScreenSize.x, -halfScreenSize.y)) ||
			Math::IntersectTriangleTriangle(p1, p2, p3,
											_camPos + Math::Vector2(-halfScreenSize.x,  halfScreenSize.y),
											_camPos + Math::Vector2( halfScreenSize.x, -halfScreenSize.y),
											_camPos + Math::Vector2(-halfScreenSize.x, -halfScreenSize.y)))
		{
			return true;
		}

		return false;
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

	void Sprite::DebugBox(Math::Matrix m, Math::Vector3 size, Color color)
	{
		Math::Vector3 halfSize = size * 0.5f;
		Math::Vector3 leftTop = m * (halfSize * Math::Vector3{ -1.f,  1.f, 0.f });
		Math::Vector3 bottomRight = m * (halfSize * Math::Vector3{ 1.f, -1.f, 0.f });

		DebugLine(leftTop, leftTop + m.MulNormal(Math::Vector3{ size.x, 0.f, 0.f }), color);
		DebugLine(leftTop + m.MulNormal(Math::Vector3{ size.x, 0.f, 0.f }), bottomRight, color);
		DebugLine(bottomRight, leftTop + m.MulNormal(Math::Vector3{ 0.f, -size.y, 0.f }), color);
		DebugLine(leftTop + m.MulNormal(Math::Vector3{ 0.f, -size.y, 0.f }), leftTop, color);
	}

	void Sprite::DebugText(const Math::Vector2 pos, Color color, const char* text, ...)
	{
		char buffer[256];
		va_list args;
		va_start(args, text);
		stbsp_vsnprintf(buffer, 256, text, args);
		va_end(args);

		float scale = root.render.GetDevice()->GetHeight() / _pixelsHeight;

		auto screenPos = (_camPos - pos) * scale * _zoom;
		
		screenPos.x = _halfScreenSize.x * scale - screenPos.x;
		screenPos.y = 2.0f * _halfScreenSize.y * scale - (_halfScreenSize.y * scale - screenPos.y);

		root.render.DebugPrintText({ screenPos.x, screenPos.y }, ScreenCorner::LeftTop, color, buffer);
	}

	void Sprite::DebugText(int line, const char* text, ...)
	{
		char buffer[256];
		va_list args;
		va_start(args, text);
		stbsp_vsnprintf(buffer, 256, text, args);
		va_end(args);

		root.render.DebugPrintText({ 10.f, 10.0f + 20.f * float(line) }, ScreenCorner::LeftTop, COLOR_GREEN, buffer);
	}

	void Sprite::DebugTextBottom(int line, const char* text, ...)
	{
		char buffer[256];
		va_list args;
		va_start(args, text);
		stbsp_vsnprintf(buffer, 256, text, args);
		va_end(args);

		root.render.DebugPrintText({ 10.f, 10.0f + 20.f * float(line) }, ScreenCorner::LeftBottom, COLOR_GREEN, buffer);
	}

	void Sprite::Release()
	{ 
		_vdecl.ReleaseRef();
		_quadBuffer.ReleaseRef();
		_polygonVdecl.ReleaseRef();
		_polygonBuffer.ReleaseRef();
		quadPrg.ReleaseRef();
		quadPrgNoZ.ReleaseRef();
		polygonPrg.ReleaseRef();
		polygonPrgNoZ.ReleaseRef();
	}
}




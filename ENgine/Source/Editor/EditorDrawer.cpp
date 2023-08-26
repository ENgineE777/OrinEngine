#include "EditorDrawer.h"
#include "Editor.h"
#include "Root/Root.h"

namespace Orin
{
	EditorDrawer editorDrawer;

	class TriangleSimplest : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "triangle_simplest_vs.shd"; };
		virtual const char* GetPsName() { return "triangle_simplest_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
		};
	};

	void EditorDrawer::Init()
	{
		anchornTex = root.render.LoadTexture("ENgine\\editor\\gizmo_anch.png", _FL_);
		anchornTex->SetFilters(TextureFilter::Point, TextureFilter::Point);

		centerTex = root.render.LoadTexture("ENgine\\editor\\gizmo_center.png", _FL_);
		centerTex->SetFilters(TextureFilter::Point, TextureFilter::Point);

		nodeTex = root.render.LoadTexture("ENgine\\editor\\node.png", _FL_);
		nodeTex->SetFilters(TextureFilter::Point, TextureFilter::Point);

		arrowTex = root.render.LoadTexture("ENgine\\editor\\arrow.png", _FL_);
		arrowTex->SetFilters(TextureFilter::Point, TextureFilter::Point);

		checkerTex = root.render.LoadTexture("ENgine/editor/checker.png", _FL_);
		checkerTex->SetFilters(TextureFilter::Point, TextureFilter::Point);

		lightBulbTex = root.render.LoadTexture("ENgine/editor/LightBulp.png", _FL_);
		lightBulbTex->SetFilters(TextureFilter::Point, TextureFilter::Point);

		directionalTex = root.render.LoadTexture("ENgine/editor/LightBulpDir.png", _FL_);
		directionalTex->SetFilters(TextureFilter::Point, TextureFilter::Point);

		iconsTex = root.render.LoadTexture("ENgine/editor/icons.png", _FL_);
		iconsTex->SetFilters(TextureFilter::Point, TextureFilter::Point);

		projectIconTex = root.render.LoadTexture("ENgine/editor/ProjectIcon.png", _FL_);

		font = root.fonts.LoadFont("ENgine\\DroidSans.ttf", false, false, 12, _FL_);

		VertexDecl::ElemDesc desc[] = { { ElementType::Float3, ElementSemantic::Position, 0 },{ ElementType::Float2, ElementSemantic::Texcoord, 0 } };
		skyBoxVdecl = root.render.GetDevice()->CreateVertexDecl(2, desc, _FL_);

		skyBoxVbuffer = root.render.GetDevice()->CreateBuffer(8, sizeof(VertexSkyBox), _FL_);

		VertexSkyBox* vertices = (VertexSkyBox*)skyBoxVbuffer->Lock();

		float  size = 25.0f;

		VertexSkyBox base_vertices[] =
		{
			{ Math::Vector3(-size, size, -size), Math::Vector2(0.0f, 0.0f) },
			{ Math::Vector3(size, size, -size), Math::Vector2(1.0f, 0.0f) },
			{ Math::Vector3(-size, -size, -size), Math::Vector2(0.0f, 1.0f) },
			{ Math::Vector3(size, -size, -size), Math::Vector2(1.0f, 1.0f) },
			{ Math::Vector3(-size, size, size), Math::Vector2(1.0f, 0.0f) },
			{ Math::Vector3(size, size, size), Math::Vector2(0.0f, 0.0f) },
			{ Math::Vector3(-size, -size, size), Math::Vector2(1.0f, 1.0f) },
			{ Math::Vector3(size, -size, size), Math::Vector2(0.0f, 1.0f) }
		};

		memcpy(vertices, base_vertices, 8 * 5 * sizeof(float));

		skyBoxVbuffer->Unlock();

		skyBoxIbuffer = root.render.GetDevice()->CreateBuffer(12 * 3, sizeof(int), _FL_);

		int* indices = (int*)skyBoxIbuffer->Lock();

		int bace_indices[] =
		{
			0, 1, 2, 2, 1, 3,
			4, 0, 6, 6, 0, 2,
			7, 5, 6, 6, 5, 4,
			3, 1, 7, 7, 1, 5,
			4, 5, 0, 0, 5, 1,
			3, 7, 2, 2, 7, 6
		};

		memcpy(indices, bace_indices, 36 * sizeof(int));

		skyBoxIbuffer->Unlock();

		skyBoxPrg = root.render.GetRenderTechnique<TriangleSimplest>(_FL_);

		skyBoxTexture = root.render.LoadTexture("ENgine/Editor/skybox.jpg", _FL_);
		skyBoxTexture->SetAdress(TextureAddress::Clamp);
	}

	void EditorDrawer::SetCameraMatrices(Math::Vector2 pos, float pixelHeight, float aspect)
	{
		float dist = Sprite::ToUnits(pixelHeight) * 0.5f / tanf(22.5f * Math::Radian);

		pos = Sprite::ToUnits(pos);

		Math::Matrix view;
		view.BuildView(Math::Vector3(pos.x, pos.y, -dist),
						Math::Vector3(pos.x, pos.y, -dist + 1.0f), Math::Vector3(0, 1, 0));
		root.render.SetTransform(TransformStage::View, view);

		Math::Matrix proj;
		proj.BuildProjection(45.0f * Math::Radian, aspect, 1.0f, 1000.0f);

		root.render.SetTransform(TransformStage::Projection, proj);
	}

	void EditorDrawer::DrawCheckerBoard(Math::Vector2 camPos, Math::Vector2 viewportSize, float camZoom)
	{
		Math::Matrix mat;

		Sprite::Draw(checkerTex, COLOR_WHITE, mat,
					Math::Vector2(camPos.x - viewportSize.x * 0.5f / camZoom, camPos.y + viewportSize.y * 0.5f / camZoom), viewportSize / camZoom,
					Math::Vector2(((int)(camPos.x) % 42) / 42.0f, 1.0f - ((int)(camPos.y) % 42) / 42.0f),
					Math::Vector2(viewportSize.x / camZoom / 42.0f, viewportSize.y / camZoom / 42.0f), Sprite::quadPrgNoZ);
	}

	void EditorDrawer::DrawWindowBorder()
	{
		Color color(1.0, 0.65f, 0.0f, 1.0f);

		for (float i = 0; i < 3.0f; i += 1.0f)
		{
			root.render.DebugLine2D(Math::Vector2(0.5f, 0.5f + i), color, Math::Vector2((float)root.render.GetDevice()->GetWidth(), 0.5f + i), color);
			root.render.DebugLine2D(Math::Vector2(0.5f, (float)root.render.GetDevice()->GetHeight() - 0.5f - i), color, Math::Vector2((float)root.render.GetDevice()->GetWidth(), (float)root.render.GetDevice()->GetHeight() - 0.5f - i), color);
			root.render.DebugLine2D(Math::Vector2(0.5f + i, 0.5f), color, Math::Vector2(0.5f + i, (float)root.render.GetDevice()->GetHeight()), color);
			root.render.DebugLine2D(Math::Vector2((float)root.render.GetDevice()->GetWidth() - i - 0.5f, 0.5f), color, Math::Vector2((float)root.render.GetDevice()->GetWidth() - i - 0.5f, (float)root.render.GetDevice()->GetHeight()), color);
		}
	}

	void EditorDrawer::DrawSprite(Texture* tex, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 offset, float rotate, Color color)
	{
		Math::Matrix mat;
		mat.RotateZ(-rotate);
		mat.Pos() = Math::Vector3(pos.x, pos.y, 0.01f);
		mat.Pos() *= Sprite::ToUnits(1.0f);

		Sprite::Draw(tex, color, mat, offset, size, 0.0f, 1.0f, Sprite::quadPrgNoZ);
	}

	void EditorDrawer::PrintText(Math::Vector2 pos, float fontScale, Color color, const char* text)
	{
		Math::Vector3 pos3d = Sprite::ToUnits(Math::Vector3(pos));

		pos3d = root.render.TransformToScreen(pos3d, 2);

		Math::Matrix mat;
		mat.Scale(fontScale);
		mat.Pos().x = pos3d.x;
		mat.Pos().y = pos3d.y;

		if (pos3d.x + 250.0f < 0.0f || (float)root.render.GetDevice()->GetWidth() < pos3d.x ||
			pos3d.y + 15.0f < 0.0f || (float)root.render.GetDevice()->GetHeight() < pos3d.y)
		{
			return;
		}

		char str[64];
		StringUtils::Copy(str, 64, text);

		if ((int)strlen(str) > 15)
		{
			memcpy(&str[15], "...", 4);
		}

		font.Print(mat, 1.0f, color, str);
	}

	void EditorDrawer::DrawLine(Math::Vector2 from, Math::Vector2 to, Color color)
	{
		Math::Vector2 dir = to - from;
		Math::Vector2 size(dir.Length(), 2.0f);

		Math::Matrix mat;
		mat.RotateZ(-atan2f(dir.y / size.x, dir.x / size.x));
		mat.Pos() = Math::Vector3(from.x, from.y, 0.01f);
		mat.Pos() *= Sprite::ToUnits(1.0f);

		Sprite::Draw(nullptr, color, mat, 0.0f, size, 0.0f, 1.0f, Sprite::quadPrgNoZ);
	}

	void EditorDrawer::DrawCurve(Math::Vector2 from, Math::Vector2 to, Color color)
	{
		Math::Vector2 last = from;

		float offset = fminf((from - to).Length() * 0.5f, 100.0f);
		Math::Vector2 p2 = Math::Vector2(from.x + offset, from.y);
		Math::Vector2 p3 = Math::Vector2(to.x - offset, to.y);

		uint32_t segments = 20;
		float t_step = 1.0f / (float)segments;

		for (uint32_t i = 1; i <= segments; i++)
		{
			float t = t_step * (float)i;
			float u = 1.0f - t;
			float w1 = u * u*u;
			float w2 = 3 * u*u*t;
			float w3 = 3 * u*t*t;
			float w4 = t * t *t;
			Math::Vector2 pos = w1 * from + w2 * p2 + w3 * p3 + w4 * to;

			DrawLine(last, pos, color);

			last = pos;
		}
	}

	void EditorDrawer::DrawSkyBox()
	{
		root.render.GetDevice()->SetRenderTechnique(skyBoxPrg);

		root.render.GetDevice()->SetVertexDecl(skyBoxVdecl);
		root.render.GetDevice()->SetVertexBuffer(0, skyBoxVbuffer);
		root.render.GetDevice()->SetIndexBuffer(skyBoxIbuffer);

		Math::Matrix view_prev;
		root.render.GetTransform(TransformStage::View, view_prev);

		Math::Matrix view;
		view.BuildView(0.0f, Math::Vector3(cosf(editor.freeCamera.angles.x), sinf(editor.freeCamera.angles.y), sinf(editor.freeCamera.angles.x)), Math::Vector3(0, 1, 0));
		root.render.SetTransform(TransformStage::View, view);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::WrldViewProj, view_proj);

		root.render.SetTransform(TransformStage::View, view_prev);

		skyBoxPrg->SetMatrix(ShaderType::Vertex, "view_proj", &view_proj, 1);
		skyBoxPrg->SetTexture(ShaderType::Pixel, "diffuseMap", skyBoxTexture);

		root.render.GetDevice()->DrawIndexed(PrimitiveTopology::TrianglesList, 0, 0, 12);
	}

	void EditorDrawer::Release()
	{
		font.ReleaseRef();
		anchornTex.ReleaseRef();
		centerTex.ReleaseRef();
		nodeTex.ReleaseRef();
		arrowTex.ReleaseRef();
		checkerTex.ReleaseRef();
		projectIconTex.ReleaseRef();
		lightBulbTex.ReleaseRef();
		iconsTex.ReleaseRef();

		skyBoxPrg.ReleaseRef();
		skyBoxVbuffer.ReleaseRef();
		skyBoxVdecl.ReleaseRef();
		skyBoxIbuffer.ReleaseRef();
		skyBoxTexture.ReleaseRef();
	}
}
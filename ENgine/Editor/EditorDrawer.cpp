#include "EditorDrawer.h"
#include "Root/Root.h"

namespace Oak
{
	EditorDrawer editorDrawer;

	void EditorDrawer::Init()
	{
		anchorn = root.render.LoadTexture("ENgine\\editor\\gizmo_anch.png", _FL_);
		anchorn->SetFilters(TextureFilter::Point, TextureFilter::Point);

		center = root.render.LoadTexture("ENgine\\editor\\gizmo_center.png", _FL_);
		center->SetFilters(TextureFilter::Point, TextureFilter::Point);

		node_tex = root.render.LoadTexture("ENgine\\editor\\node.png", _FL_);
		node_tex->SetFilters(TextureFilter::Point, TextureFilter::Point);

		arrow_tex = root.render.LoadTexture("ENgine\\editor\\arrow.png", _FL_);
		arrow_tex->SetFilters(TextureFilter::Point, TextureFilter::Point);

		checker_texture = root.render.LoadTexture("ENgine/editor/checker.png", _FL_);
		checker_texture->SetFilters(TextureFilter::Point, TextureFilter::Point);

		font = root.fonts.LoadFont("ENgine\\OpenSans-Regular.ttf", false, false, 11, _FL_);
	}

	void EditorDrawer::DrawSprite(Texture* tex, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 offset, float rotate, Color color)
	{
		Math::Matrix mat;
		mat.RotateZ(rotate);

		mat.Pos() = Math::Vector3(pos.x, pos.y, 0.01f);

		if (pos.x + size.x < 0 || root.render.GetDevice()->GetWidth() < pos.x ||
			pos.y + size.y < 0 || root.render.GetDevice()->GetHeight() < pos.y)
		{
			return;
		}

		Sprite::Draw(tex, color, mat, offset, size, 0.0f, 1.0f, false);
	}

	void EditorDrawer::PrintText(Math::Vector2 pos, Color color, const char* text)
	{
		if (pos.x + 250 < 0 || root.render.GetDevice()->GetWidth() < pos.x ||
			pos.y + 15 < 0 || root.render.GetDevice()->GetHeight() < pos.y)
		{
			return;
		}

		Math::Matrix mat;
		mat.Pos().x = pos.x;
		mat.Pos().y = pos.y;

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

		Math::Vector2 center = from + dir * 0.5f;

		Math::Matrix mat;
		mat.RotateZ(atan2f(dir.y / size.x, dir.x / size.x));
		mat.Pos() = Math::Vector3(center.x, center.y, 0.01f);

		Sprite::Draw(nullptr, color, mat, -0.5f * size, size, 0.0f, 1.0f, false);
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

	void EditorDrawer::Release()
	{
		font.ReleaseRef();
		anchorn.ReleaseRef();
		center.ReleaseRef();
		node_tex.ReleaseRef();
		arrow_tex.ReleaseRef();
		checker_texture.ReleaseRef();

	}
}
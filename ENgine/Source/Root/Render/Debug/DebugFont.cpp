#include "DebugFont.h"
#include "Root/Root.h"

namespace Oak
{
	DebugFont::DebugFont()
	{
		texts.reserve(32);
	}

	DebugFont::~DebugFont()
	{
	}

	bool DebugFont::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
	{
		debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugFont::Draw);

		font = root.fonts.LoadFont("ENgine/helvetica", false, false, font_size, _FL_);

		return true;
	}

	void DebugFont::AddText(eastl::vector<Text>& texts, Math::Vector3 pos, ScreenCorner corner, float dist, Color color, const char* text)
	{
		if (texts.size()>1000) return;

		texts.push_back(Text());

		Text* txt = &texts[texts.size() - 1];

		txt->pos = pos;
		txt->corner = corner;
		txt->dist = 0.0f;
		txt->color = color;
		StringUtils::Copy(txt->text, 256, text);
	}

	void DebugFont::AddText(Math::Vector2 pos, ScreenCorner corner, Color color, const char* text)
	{
		AddText(texts, Math::Vector3(pos.x, pos.y, 0.0f), corner, -1.0f, color, text);
	}

	void DebugFont::AddText(Math::Vector3 pos, float dist, Color color, const char* text)
	{
		AddText(textsIn3d, pos, ScreenCorner::LeftTop, dist, color, text);
	}

	void DebugFont::Draw(float dt)
	{
		Math::Matrix mat;

		for (int i=0; i < textsIn3d.size(); i++)
		{
			Text* txt = &textsIn3d[i];

			Math::Vector3 pos = root.render.TransformToScreen(txt->pos, 1);

			if (pos.z>0 && pos.z<txt->dist)
			{
				mat.Pos() = Math::Vector3(pos.x * root.render.GetDevice()->GetWidth(), pos.y * root.render.GetDevice()->GetHeight(), 0);
				font.Print(mat, 1.0f, txt->color, txt->text);
			}
		}

		textsIn3d.clear();

		eastl::vector<FontRes::LineBreak> line_breaks;
		mat.Pos().z = 0.0f;

		Math::Vector2 screen = Math::Vector2((float)root.render.GetDevice()->GetWidth(), (float)root.render.GetDevice()->GetHeight());

		for (int i=0;i<texts.size();i++)
		{
			Text* txt = &texts[i];

			font.GetLineBreak(line_breaks, txt->text, 1000);

			mat.Pos().x = (txt->corner == ScreenCorner::LeftTop || txt->corner == ScreenCorner::LeftBottom) ? (txt->pos.x + 1) : (screen.x - txt->pos.x - line_breaks[0].width);
			mat.Pos().y = (txt->corner == ScreenCorner::LeftTop || txt->corner == ScreenCorner::RightTop) ? (txt->pos.y + 1) : (screen.y - txt->pos.y - font_size);
			font.Print(mat, 1.0f, COLOR_BLACK, txt->text);

			mat.Pos() -= 1.0f;
			font.Print(mat, 1.0f, txt->color, txt->text);
		}

		texts.clear();
	}

	void DebugFont::Release()
	{
		delete this;
	}
}
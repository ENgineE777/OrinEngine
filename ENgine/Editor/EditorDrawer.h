
#pragma once

#include "Support/Support.h"
#include "Support/Sprite.h"
#include "Root/Fonts/Fonts.h"

/**
\ingroup gr_code_editor
*/

namespace Oak
{
	class EditorDrawer
	{
		Transform trans;
		Sprite::FrameState state;

	public:

		FontRef font;
		TextureRef anchorn;
		TextureRef center;
		TextureRef node_tex;
		TextureRef arrow_tex;
		TextureRef checker_texture;

		void Init();
		void DrawSprite(Texture* tex, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 offset, float rotate, Color color);
		void PrintText(Math::Vector2 pos, Color color, const char* text);
		void DrawLine(Math::Vector2 from, Math::Vector2 to, Color color);
		void DrawCurve(Math::Vector2 from, Math::Vector2 to, Color color);

		void Release();
	};

	extern EditorDrawer editorDrawer;
}

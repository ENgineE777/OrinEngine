
#pragma once

#include "Support/Support.h"
#include "Support/Sprite.h"
#include "Root/Fonts/Fonts.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/**
\ingroup gr_code_editor
*/

namespace Oak
{
	class EditorDrawer
	{
		Transform trans;

		struct VertexSkyBox
		{
			Math::Vector3 pos;
			Math::Vector2 uv;
		};

		ProgramRef skyBoxPrg;
		DataBufferRef skyBoxVbuffer;
		VertexDeclRef skyBoxVdecl;
		DataBufferRef skyBoxIbuffer;
		TextureRef skyBoxTexture;

	public:

		HWND hwnd;
		FontRef font;
		TextureRef anchornTex;
		TextureRef centerTex;
		TextureRef nodeTex;
		TextureRef arrowTex;
		TextureRef checkerTex;
		TextureRef projectIconTex;

		void Init();

		void DrawCheckerBoard(Math::Vector2 camPos, Math::Vector2 viewportSize, float camZoom);
		void SetCameraMatrices(Math::Vector2 pos, float pixelHeight, float aspect);
		void DrawWindowBorder();
		void DrawSprite(Texture* tex, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 offset, float rotate, Color color);
		void PrintText(Math::Vector2 pos, float fontScale, Color color, const char* text);
		void DrawLine(Math::Vector2 from, Math::Vector2 to, Color color);
		void DrawCurve(Math::Vector2 from, Math::Vector2 to, Color color);
		void DrawSkyBox();

		void Release();
	};

	extern EditorDrawer editorDrawer;
}

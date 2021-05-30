#pragma once

#ifdef OAK_EDITOR

#include "Support/Sprite.h"

namespace Oak
{
	class SpriteWindow
	{
		enum Drag
		{
			DragNone,
			DragRects,
			DragField
		};

		Math::Vector2 spritePos;
		Math::Vector2 spriteSize;
		Math::Vector2 spriteOffsetX;
		Math::Vector2 spriteOffsetY;

		Math::Vector2 points[16];
		Math::Vector2 deltaMouse = 0.0f;

		Drag drag = DragNone;
		int rectWidth;
		int rectHeight;
		int selRow = -1;
		int selCol = -1;
		Math::Vector2 prevMs;

		bool borderDrawed = false;
		int numFrames = 1;
		eastl::vector<Math::Vector2> frames;

		bool needSetSize = true;
		Math::Vector2 camPos;
		float camZoom = 1.0f;

		bool viewportCaptured = false;
		bool vireportHowered = false;

		Math::Vector2 lastViewportSize = Math::Vector2(800.f, 600.0f);

	public:

		bool opened = false;
		int curFrame = 0;
		bool showAnim = true;

		static Sprite::Data* sprite;
		static SpriteWindow* instance;

		static void StartEdit(Sprite::Data* ed_sprite);
		static void StopEdit();

		void FillPoints(int index, int stride, float val, bool vert);
		void ResizeSpriteRect();
		void UpdateSpriteRect();
		void SetImage(const char* img);
		void Prepare();
		void SetCurFrame(int frame);

		void SelectRect();
		void FillRects();
		void MoveRects(Math::Vector2 delta);
		void UpdateAnimRect();
		void UpdateSavedPos();
		void ActualPixels();
		void FitImage();

		void ImGui();
		void DrawViewport(Math::Vector2 viewportSize);

		void OnMouseMove(Math::Vector2 ms);

		void OnLeftMouseDown();
		void OnLeftMouseUp();

		void OnMiddleMouseDown();
		void OnMiddleMouseUp();
	};
}

#endif
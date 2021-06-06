#pragma once

#ifdef OAK_EDITOR

#include "Root/Assets/AssetTexture.h"

namespace Oak
{
	class SpriteWindow
	{
		enum class Drag
		{
			DragNone,
			DragRects,
			DragField,
			DragNewSlice
		};

		Math::Vector2 points[16];
		Math::Vector2 deltaMouse = 0.0f;

		Drag drag = Drag::DragNone;
		int rectWidth;
		int rectHeight;
		int selRow = -1;
		int selCol = -1;
		int selSlice = -1;
		Math::Vector2 prevMs;

		bool borderDrawed = false;
		int numFrames = 1;

		bool needSetSize = true;
		Math::Vector2 camPos;
		float camZoom = 1.0f;

		bool viewportCaptured = false;
		bool vireportHowered = false;
		bool imageFocused = false;
		bool inDragAndDrop = false;

		AssetTextureRef textureRef;

		Math::Vector2 rectStart;
		Math::Vector2 rectEnd;
		Math::Vector2 lastViewportSize = Math::Vector2(800.f, 600.0f);

		void DrawRect(Math::Vector2 p1, Math::Vector2 p2, Color color);

		void Text(const char* name);
		bool InputFloat(float* value, const char* prefix);

	public:

		bool opened = false;
		int curFrame = 0;
		bool showAnim = true;

		static AssetTexture* texture;
		static SpriteWindow* instance;

		static void StartEdit(AssetTexture* setTexture);

		void Prepare();

		void FillPoints(int index, int stride, float val);
		void FillRects();

		void FitImage();

		void MoveRects(Math::Vector2 delta);

		void UpdateSlice();

		void ImGui();
		void DrawViewport(Math::Vector2 viewportSize);

		void OnMouseMove(Math::Vector2 ms, bool drag);

		void OnLeftMouseDown();
		void OnLeftMouseUp();
	};
}

#endif
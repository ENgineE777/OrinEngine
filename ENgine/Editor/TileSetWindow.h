#pragma once

#ifdef OAK_EDITOR

#include "Root/Assets/AssetTileSet.h"
#include "eastl/queue.h"

namespace Oak
{
	class TileSetWindow
	{
		enum class Drag
		{
			DragNone,
			DragField
		};

		Drag drag = Drag::DragNone;
		bool needSetSize = true;

		Math::Vector2 prevMs;

		Math::Vector2 lastViewportSize = Math::Vector2(800.f, 600.0f);

		Math::Vector2 camPos;
		float camZoom = 1.0f;

		bool viewportCaptured = false;
		bool vireportHowered = false;
		bool imageFocused = false;

	public:

		bool opened = false;

		static AssetTileSet* tileSet;
		static TileSetWindow* instance;

		static void StartEdit(AssetTileSet* setTileSet);

		void Prepare();

		void ImGui();
		void DrawViewport(Math::Vector2 viewportSize);

		void ShowToolbar();
		void ShowImage();

		void OnMouseMove(Math::Vector2 ms, bool drag);

		void OnLeftMouseDown();
		void OnLeftMouseUp();
	};
}

#endif
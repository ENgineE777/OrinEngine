#pragma once

#ifdef OAK_EDITOR

#include "Root/Assets/AssetTileSet.h"
#include "eastl/queue.h"

namespace Oak
{
	class TileSetWindow
	{
		bool needSetSize = true;

	public:

		bool opened = false;

		static AssetTileSet* tileSet;
		static TileSetWindow* instance;

		static void StartEdit(AssetTileSet* setTileSet);

		void Prepare();

		void ImGui();
		void DrawViewport(Math::Vector2 viewportSize);

		void OnMouseMove(Math::Vector2 ms, bool drag);

		void OnLeftMouseDown();
		void OnLeftMouseUp();
	};
}

#endif
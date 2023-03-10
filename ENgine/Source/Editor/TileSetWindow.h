#pragma once

#ifdef ORIN_EDITOR

#include "Root/Assets/AssetTileSet.h"
#include "Root/Assets/AssetTexture.h"
#include "eastl/queue.h"

namespace Orin
{
	class TileSetWindow
	{
		enum class Drag
		{
			DragNone,
			DragDrop,
			DragField,
			DragTile
		};

		Drag drag = Drag::DragNone;

		int dragX = 0;
		int dragY = 0;

		bool needSetSize = true;

		Math::Vector2 prevMs;

		Math::Vector2 lastViewportSize = Math::Vector2(800.f, 600.0f);

		bool viewportCaptured = false;
		bool vireportHowered = false;
		bool imageFocused = false;

		void MouseToCell(int& x, int& y);
		int FindTileIndex(int x, int y);
		void DrawCell(int x, int y);
		void TryAddSlice(AssetTextureRef& texture, Math::Vector2 offset, int sliseIndex);

	public:

		bool opened = false;

		static AssetTileSet* tileSet;
		static TileSetWindow* instance;

		static void StartEdit(AssetTileSet* setTileSet);
		static void StopEdit();

		void Prepare();

		void ImGui();
		void DrawViewport(Math::Vector2 viewportSize);

		void ShowImage();
		void ShowTileProperties();

		void OnMouseMove(Math::Vector2 ms, bool drag);

		void OnLeftMouseDown();
		void OnLeftMouseUp();

		static void Release();
	};
}

#endif
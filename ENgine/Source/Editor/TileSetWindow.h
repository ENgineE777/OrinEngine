#pragma once

#ifdef ORIN_EDITOR

#include "Root/Assets/AssetTileSet.h"
#include "Root/Assets/AssetTexture.h"
#include "eastl/queue.h"

namespace Orin
{
	class TileSetWindow
	{
		enum class Mode
		{
			None,
			DragTile,
			DragField,
			TileSlected,
			TilesSelection,
			TilesSelected,
			TilesMove
		};

		Mode mode = Mode::None;

		Math::Vector2 drag;

		bool needSetSize = true;

		Math::Vector2 prevMs;
		Math::Vector2 lastViewportSize = Math::Vector2(800.f, 600.0f);

		Math::Vector2 rectStart;
		Math::Vector2 rectEnd;		

		bool viewportCaptured = false;
		bool vireportHowered = false;
		bool imageFocused = false;

		Math::Vector2 MouseToCell(Math::Vector2 viewportSize);
		int FindTileIndex(Math::Vector2 pos);
		void DrawCell(Math::Vector2 pos);
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
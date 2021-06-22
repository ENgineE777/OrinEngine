#pragma once

#ifdef OAK_EDITOR

#include "Root/Assets/AssetTexture.h"
#include "eastl/queue.h"

namespace Oak
{
	class SpriteWindow
	{
		enum class Drag
		{
			DragNone,
			DragSelectRect,
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

		float labelSize = 45;
		float inputSize = 120;

		int typeAutoSlice = 0;
		int AutoSliceRows = 1;
		int AutoSliceCols = 1;
		int AutoSliceCellSizeX = 32;
		int AutoSliceCellSizeY = 32;
		int AutoSliceMinSizeX = 16;
		int AutoSliceMinSizeY = 16;

		void DrawRect(Math::Vector2 p1, Math::Vector2 p2, Color color);

		int selAnim = -1;
		int selAnimFrame = -1;
		int animFrameToPaste = -1;

		int curAnimPlayFrame = -1;
		float curAnimPlayTime = 0.0f;

		eastl::vector<int> selectedSlices;

		bool animFrameDragChecked = false;
		int dragFrameIndex = -1;

		void Text(const char* name);
		bool InputFloat(float* value, const char* prefix);
		bool InputInt(int* value, const char* prefix, bool needClamp);
		bool InputInt(float* value, const char* prefix, bool needClamp);
		bool InputString(eastl::string& value, const char* name);

		int posXMin, posXMax;
		int posYMin, posYMax;

		void AddToQueue(int posX, int posY, eastl::queue<Math::Vector2>& nodes, int width, int height, uint8_t* data, uint8_t* visited);
		void TextureCrawler(int posX, int posY, int width, int height, uint8_t* data, uint8_t* visited);
		
		void ShowImageInfo();
		void ShowAutoSlicing();
		void ShowSlices();
		void ShowAnimations();
		void ShowImage();

		void DrawImage(int sliceScaleIndex, int slice, float size, Math::Vector2 offset, int index);
		bool AnimFrameDrag(int index);
		void FitImage();

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
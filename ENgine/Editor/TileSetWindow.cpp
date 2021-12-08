
#ifdef OAK_EDITOR

#include "TileSetWindow.h"
#include "Root/Root.h"

#include "Editor/EditorDrawer.h"

#include "imgui.h"

#include "stb_image.h"

#include "imgui_internal.h"

#include "eastl/sort.h"

#include "Support/ImGuiHelper.h"

namespace Oak
{
	AssetTileSet* TileSetWindow::tileSet;
	TileSetWindow* TileSetWindow::instance = nullptr;

	void ShowTileSetWindow(AssetTileSet* tileSet)
	{
		TileSetWindow::StartEdit(tileSet);
	}

	void TileSetWindow::StartEdit(AssetTileSet* setTileSet)
	{
		if (!instance)
		{
			instance = new TileSetWindow();
		}

		tileSet = setTileSet;
		instance->Prepare();
		instance->opened = true;
	}

	void TileSetWindow::Prepare()
	{
	}

	void TileSetWindow::ImGui()
	{
		if (!opened)
		{
			tileSet = nullptr;
			return;
		}

		ImGui::Begin("TileSet Editor", &opened, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

		if (needSetSize)
		{
			auto size = ImGui::GetWindowSize();

			if (size.x <= 32 && size.y <= 32)
			{
				ImGui::SetWindowSize(ImVec2(900.0f, 700.0f));
			}

			needSetSize = false;
		}

		ImGuiID dockspaceID = ImGui::GetID("SpriteEditor");

		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		if (!ImGui::DockBuilderGetNode(dockspaceID))
		{
			ImGui::DockBuilderRemoveNode(dockspaceID);
			ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);

			ImGuiID dock_main_id = dockspaceID;
			ImGuiID dock_top_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.1f, nullptr, &dock_main_id);
			ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.9f, nullptr, &dock_main_id);

			ImGui::DockBuilderDockWindow("ToolbarTileSet", dock_top_id);
			ImGui::DockBuilderDockWindow("ImageTileSet", dock_bottom_id);

			ImGui::DockBuilderFinish(dock_main_id);

			ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_top_id);
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

			node = ImGui::DockBuilderGetNode(dock_bottom_id);
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
		}

		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);

		ImGui::End();

		ShowToolbar();
		ShowImage();
	}

	void TileSetWindow::DrawViewport(Math::Vector2 viewportSize)
	{
		lastViewportSize = viewportSize;

		root.render.GetDevice()->SetBackBuffer(1, (int)viewportSize.x, (int)viewportSize.y, &editorDrawer.hwnd);

		root.render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

		editorDrawer.SetCameraMatrices(camPos, viewportSize.y / camZoom, viewportSize.y / viewportSize.x);

		Color color = COLOR_WHITE;

		{
			Math::Vector2 step = { (float)tileSet->sizeX, (float)tileSet->sizeY };

			float minStep = 16.0f;

			Math::Vector2 pos = camPos;

			pos.x = step.x * (int)(pos.x / step.x);
			pos.y = step.y * (int)(pos.y / step.y);

			pos *= Sprite::pixelsPerUnitInvert;
			step *= Sprite::pixelsPerUnitInvert;

			Color color = COLOR_WHITE_A(0.75f);

			int numCellsY = 30;
			int numCellsX = 45;

			numCellsX = (int)(viewportSize.y * 0.5f * Sprite::pixelsPerUnitInvert / root.render.GetDevice()->GetAspect() / camZoom / step.y + 2);
			numCellsY = (int)(viewportSize.y * 0.5f * Sprite::pixelsPerUnitInvert / camZoom / step.y + 2);

			for (int i = -numCellsY; i <= numCellsY; i++)
			{
				root.render.DebugLine(Math::Vector3(-numCellsX * step.x + pos.x, i * step.y + pos.y, 0.0f), color, Math::Vector3(numCellsX * step.x + pos.x, i * step.y + pos.y, 0.0f), color, false);
			}

			for (int i = -numCellsX; i <= numCellsX; i++)
			{
				root.render.DebugLine(Math::Vector3(i * step.x + pos.x, -numCellsY * step.y + pos.y, 0.0f), color, Math::Vector3(i * step.x + pos.x, numCellsY * step.y + pos.y, 0.0f), color, false);
			}
		}

		if (imageFocused)
		{
			editorDrawer.DrawWindowBorder();
		}

		root.render.ExecutePool(1000, 0.0f);

		root.render.GetDevice()->Present();
	}

	void TileSetWindow::ShowToolbar()
	{
		ImGui::Begin("ToolbarTileSet");

		ImGui::Dummy({20.0f, 20.0f});
		ImGui::End();
	}

	void TileSetWindow::ShowImage()
	{
		ImGui::Begin("ImageTileSet");

		ImGuiIO& io = ImGui::GetIO();

		ImVec2 size = ImGui::GetContentRegionAvail();

		ImVec2 viewportPos = ImVec2(io.MousePos.x - ImGui::GetCursorScreenPos().x, io.MousePos.y - ImGui::GetCursorScreenPos().y);

		if (size.y < 0)	size.y = -size.y;

		DrawViewport(Math::Vector2(size.x, size.y));

		ImGui::Image(Oak::root.render.GetDevice()->GetBackBuffer(), size);

		imageFocused = ImGui::IsWindowFocused();

		if (imageFocused)
		{
			camZoom = Math::Clamp(camZoom + io.MouseWheel * 0.5f, 0.4f, 3.0f);
		}

		vireportHowered = ImGui::IsItemHovered();

		if (vireportHowered && ImGui::IsMouseClicked(0))
		{
			OnLeftMouseDown();
			viewportCaptured = true;
		}
		else
		if (vireportHowered && ImGui::IsMouseClicked(2))
		{
			drag = Drag::DragField;
			viewportCaptured = true;
		}

		ImVec2 del = ImGui::GetMouseDragDelta(0);
		OnMouseMove(Math::Vector2((float)viewportPos.x, (float)viewportPos.y),
					fabsf(del.x) > 7.0f && fabsf(del.y) > 7.0f && ImGui::IsWindowFocused() &&
					viewportPos.x > 0 && viewportPos.x < size.x&& viewportPos.y > 0 && viewportPos.y < size.x);

		if (viewportCaptured && ImGui::IsMouseReleased(0))
		{
			OnLeftMouseUp();
			viewportCaptured = false;
		}
		else
		if (viewportCaptured && ImGui::IsMouseReleased(2))
		{
			drag = Drag::DragNone;
			viewportCaptured = false;
		}

		ImGui::End();
	}

	void TileSetWindow::OnMouseMove(Math::Vector2 ms, bool dragMouse)
	{
		Math::Vector2 delta(prevMs.x - ms.x, prevMs.y - ms.y);

		if (drag == Drag::DragField)
		{
			camPos += Math::Vector2(delta.x, -delta.y) / camZoom;
		}
		
		prevMs = ms;
	}

	void TileSetWindow::OnLeftMouseDown()
	{
	}

	void TileSetWindow::OnLeftMouseUp()
	{
		drag = Drag::DragNone;
	}
}

#endif
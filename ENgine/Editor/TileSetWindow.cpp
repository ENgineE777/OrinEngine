
#ifdef OAK_EDITOR

#include "TileSetWindow.h"
#include "SpriteWindow.h"

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

		if (tileSet)
		{
			tileSet->SaveMetaData();
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
			if (tileSet)
			{
				tileSet->SaveMetaData();
			}

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
			//ImGuiID dock_top_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.01f, nullptr, &dock_main_id);
			ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 1.0f, nullptr, &dock_main_id);

			//ImGui::DockBuilderDockWindow("ToolbarTileSet", dock_top_id);
			ImGui::DockBuilderDockWindow("ImageTileSet", dock_bottom_id);

			ImGui::DockBuilderFinish(dock_main_id);

			ImGuiDockNode* node;

			//ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_top_id);
			//node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

			node = ImGui::DockBuilderGetNode(dock_bottom_id);
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
		}

		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);

		ImGui::End();

		//ShowToolbar();
		ShowImage();
	}

	void TileSetWindow::DrawViewport(Math::Vector2 viewportSize)
	{
		lastViewportSize = viewportSize;

		root.render.GetDevice()->SetBackBuffer(2, (int)viewportSize.x, (int)viewportSize.y, &editorDrawer.hwnd);

		root.render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

		if (tileSet)
		{
			editorDrawer.SetCameraMatrices(tileSet->camPos, viewportSize.y / tileSet->camZoom, viewportSize.y / viewportSize.x);

			Color color = COLOR_WHITE;

			{
				Math::Vector2 step = { (float)tileSet->sizeX, (float)tileSet->sizeY };

				float minStep = 16.0f;

				Math::Vector2 pos = tileSet->camPos;

				pos.x = step.x * (int)(pos.x / step.x);
				pos.y = step.y * (int)(pos.y / step.y);

				pos *= Sprite::pixelsPerUnitInvert;
				step *= Sprite::pixelsPerUnitInvert;

				Color color = COLOR_WHITE_A(0.5f);

				int numCellsY = 30;
				int numCellsX = 45;

				numCellsX = (int)(viewportSize.y * 0.5f * Sprite::pixelsPerUnitInvert / root.render.GetDevice()->GetAspect() / tileSet->camZoom / step.y + 2);
				numCellsY = (int)(viewportSize.y * 0.5f * Sprite::pixelsPerUnitInvert / tileSet->camZoom / step.y + 2);

				for (int i = -numCellsY; i <= numCellsY; i++)
				{
					root.render.DebugLine(Math::Vector3(-numCellsX * step.x + pos.x, i * step.y + pos.y, 0.0f), color, Math::Vector3(numCellsX * step.x + pos.x, i * step.y + pos.y, 0.0f), color, false);
				}

				for (int i = -numCellsX; i <= numCellsX; i++)
				{
					root.render.DebugLine(Math::Vector3(i * step.x + pos.x, -numCellsY * step.y + pos.y, 0.0f), color, Math::Vector3(i * step.x + pos.x, numCellsY * step.y + pos.y, 0.0f), color, false);
				}
			}

			Transform transform;
			transform.offset.x = 0.0f;
			transform.offset.y = 0.0f;

			Math::Matrix mat;

			for (auto tile : tileSet->tiles)
			{
				Math::Vector2 size = tile.texture.GetSize();
				transform.size = Math::Vector3(size.x, size.y, 0.0f);

				mat.Pos() = mat.Vx() * (float)tile.x * transform.size.x + mat.Vy() * (float)tile.y * transform.size.y;

				transform.global = mat;
				tile.texture.Draw(&transform, COLOR_WHITE, root.GetDeltaTime());
			}

			if (tileSet->selTile != -1)
			{
				auto& tile = tileSet->tiles[tileSet->selTile];
				DrawCell(tile.x, tile.y);
			}

			if (drag == Drag::DragTile || drag == Drag::DragDrop)
			{
				DrawCell(dragX, dragY);
			}
		}

		if (imageFocused)
		{
			editorDrawer.DrawWindowBorder();
		}

		root.render.ExecutePool(1000, 0.0f);

		root.render.GetDevice()->Present();
	}

	void TileSetWindow::DrawCell(int x, int y)
	{
		Math::Vector2 step = { (float)tileSet->sizeX, (float)tileSet->sizeY };
		step *= Sprite::pixelsPerUnitInvert;

		Math::Matrix mat;
		Math::Vector3 p1 = mat.Vx() * (float)x * step.x + mat.Vy() * (float)y * step.y;
		Math::Vector3 p2 = mat.Vx() * (float)(x + 1) * step.x + mat.Vy() * (float)(y - 1) * step.y;

		Color color = COLOR_WHITE;

		root.render.DebugLine(p1, color, Math::Vector3(p2.x, p1.y, 0.0f), color, false);
		root.render.DebugLine(Math::Vector3(p2.x, p1.y, 0.0f), color, p2, color, false);
		root.render.DebugLine(p2, color, Math::Vector3(p1.x, p2.y, 0.0f), color, false);
		root.render.DebugLine(Math::Vector3(p1.x, p2.y, 0.0f), color, p1, color, false);
	}

	void TileSetWindow::ShowToolbar()
	{
		ImGui::Begin("ToolbarTileSet");
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

		if (imageFocused)
		{
			if (io.KeysDown[VK_DELETE])
			{
				if (tileSet->selTile != -1)
				{
					tileSet->tiles.erase(tileSet->tiles.begin() + tileSet->selTile);
					tileSet->selTile = -1;
				}
			}
		}

		ImGuiPayload& payload = ImGui::GetCurrentContext()->DragDropPayload;

		if (ImGui::BeginDragDropTarget())
		{
			Math::Vector2 step = { (float)tileSet->sizeX, (float)tileSet->sizeY };
			step *= Sprite::pixelsPerUnitInvert;

			Math::Matrix mat;
			Math::Vector3 pos = mat.Vx() * (float)dragX * step.x + mat.Vy() * (float)dragY * step.y;

			if (ImGui::AcceptDragDropPayload("_ASSET_TEX", ImGuiDragDropFlags_AcceptNoDrawDefaultRect) && tileSet)
			{
				AssetTextureRef texture = *reinterpret_cast<AssetTextureRef**>(payload.Data)[0];

				if (texture.sliceIndex == -1 && texture.animIndex == -1)
				{
					for (int i = 0; i < texture.Get()->slices.size(); i++)
					{
						AssetTextureRef subSlice = texture;
						subSlice.sliceIndex = i;

						auto& slice = texture.Get()->slices[i];

						int x = dragX + (int)(slice.pos.x / (float)tileSet->sizeX);
						int y = dragY - (int)(slice.pos.y / (float)tileSet->sizeY);

						if (FindTileIndex(x, y) == -1)
						{
							tileSet->tiles.push_back({ x, y, subSlice });
						}
					}

					tileSet->SaveMetaData();
				}
				else
				if (FindTileIndex(dragX, dragY) == -1)
				{
					tileSet->tiles.push_back({ dragX, dragY, texture });
					tileSet->SaveMetaData();
				}

				drag = Drag::DragNone;
			}
			else
			if (ImGui::AcceptDragDropPayload("_ANIM_FRAMES", ImGuiDragDropFlags_AcceptNoDrawDefaultRect) && tileSet)
			{
				AssetTextureRef texture = *reinterpret_cast<AssetTextureRef**>(payload.Data)[0];

				Math::Vector2 pos = texture.GetSize();

				for (auto sliceIndex : SpriteWindow::instance->selectedSlices)
				{
					auto& slice = texture.Get()->slices[sliceIndex];

					if (pos.x > slice.pos.x)
					{
						pos.x = slice.pos.x;
					}

					if (pos.y > slice.pos.y)
					{
						pos.y = slice.pos.y;
					}
				}

				for (auto sliceIndex : SpriteWindow::instance->selectedSlices)
				{
					AssetTextureRef subSlice = texture;
					subSlice.sliceIndex = sliceIndex;

					auto& slice = subSlice.Get()->slices[sliceIndex];

					int x = dragX + (int)((slice.pos.x - pos.x) / (float)tileSet->sizeX);
					int y = dragY - (int)((slice.pos.y - pos.y) / (float)tileSet->sizeY);

					if (FindTileIndex(x, y) == -1)
					{
						tileSet->tiles.push_back({ x, y, subSlice });
					}
				}

				tileSet->SaveMetaData();

				drag = Drag::DragNone;
			}
			else
			{
				drag = Drag::DragDrop;
			}

			ImGui::EndDragDropTarget();
		}
		else
		{
			if (drag == Drag::DragDrop)
			{
				drag = Drag::DragNone;
			}
		}

		imageFocused = ImGui::IsWindowFocused();

		if (imageFocused)
		{
			tileSet->camZoom = Math::Clamp(tileSet->camZoom + io.MouseWheel * 0.5f, 0.4f, 3.0f);
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
			tileSet->camPos += Math::Vector2(delta.x, -delta.y) / tileSet->camZoom;
		}
		
		prevMs = ms;

		MouseToCell(dragX, dragY);
	}

	void TileSetWindow::OnLeftMouseDown()
	{
		if (tileSet)
		{
			tileSet->selTile = FindTileIndex(dragX, dragY);

			if (tileSet->selTile != -1)
			{
				drag = Drag::DragTile;
			}
		}
	}

	void TileSetWindow::OnLeftMouseUp()
	{
		if (tileSet->selTile != -1)
		{
			if (FindTileIndex(dragX, dragY) == -1)
			{
				auto& tile = tileSet->tiles[tileSet->selTile];

				tile.x = dragX;
				tile.y = dragY;
			}
		}

		drag = Drag::DragNone;
	}

	void TileSetWindow::MouseToCell(int& x, int& y)
	{
		Math::Vector3 mouseOrigin;
		Math::Vector3 mouseDirection;

		Math::GetMouseRay(prevMs, mouseOrigin, mouseDirection);

		Math::Vector3 pos;
		Math::IntersectPlaneRay(0.0f, { 0.0f, 0.0f, 1.0f }, mouseOrigin, mouseDirection, pos);

		if (pos.x < 0.0f)
		{
			pos.x -= ((float)tileSet->sizeX * Sprite::pixelsPerUnitInvert);
		}

		x = (int)(pos.x / ((float)tileSet->sizeX * Sprite::pixelsPerUnitInvert));

		if (pos.y > 0.0f)
		{
			pos.y += ((float)tileSet->sizeX * Sprite::pixelsPerUnitInvert);
		}

		y = (int)(pos.y / ((float)tileSet->sizeY * Sprite::pixelsPerUnitInvert));
	}

	int TileSetWindow::FindTileIndex(int x, int y)
	{
		for (int i = 0; i < tileSet->tiles.size(); i++)
		{
			auto& tile = tileSet->tiles[i];

			if (tile.x == x && tile.y == y)
			{
				return i;
			}
		}

		return -1;
	}
}

#endif
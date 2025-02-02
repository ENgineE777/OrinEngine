
#ifdef ORIN_EDITOR

#include "TileSetWindow.h"
#include "SpriteWindow.h"

#include "Root/Root.h"

#include "Editor/Editor.h"
#include "Editor/EditorDrawer.h"

#include "imgui.h"

#include "stb_image.h"

#include "imgui_internal.h"

#include "eastl/sort.h"

#include "Support/ImGuiHelper.h"

namespace Orin
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

		if (instance->opened && tileSet == setTileSet)
		{
			return;
		}

		if (tileSet)
		{
			tileSet->Save();
		}

		tileSet = setTileSet;
		instance->Prepare();
		instance->opened = true;
	}

	void TileSetWindow::StopEdit()
	{
		tileSet = nullptr;

		if (instance && instance->opened)
		{
			instance->Prepare();
			instance->opened = false;
		}
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
				tileSet->Save();
			}

			tileSet = nullptr;
			return;
		}

		ImGui::Begin("TileSet Editor", &opened, ImGuiWindowFlags_NoCollapse);

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
			ImGuiID dock_up_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.8f, nullptr, &dock_main_id);
			ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.2f, nullptr, &dock_main_id);
			
			ImGui::DockBuilderDockWindow("ImageTileSet", dock_up_id);
			ImGui::DockBuilderDockWindow("TileProperties", dock_down_id);

			ImGui::DockBuilderFinish(dock_main_id);

			ImGuiDockNode* node;

			node = ImGui::DockBuilderGetNode(dock_main_id);
			node->LocalFlags |= ImGuiDockNodeFlags_NoDockingOverOther;

			node = ImGui::DockBuilderGetNode(dock_up_id);
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

			node = ImGui::DockBuilderGetNode(dock_down_id);
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
		}

		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);

		ImGui::End();
		
		ShowImage();
		ShowTileProperties();
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

				pos *= Sprite::ToUnits(1.0f);
				step *= Sprite::ToUnits(1.0f);

				Color color = COLOR_WHITE_A(0.5f);

				int numCellsY = 30;
				int numCellsX = 45;

				numCellsX = (int)(Sprite::ToUnits(viewportSize.y * 0.5f) / root.render.GetDevice()->GetAspect() / tileSet->camZoom / step.y + 2);
				numCellsY = (int)(Sprite::ToUnits(viewportSize.y * 0.5f) / tileSet->camZoom / step.y + 2);

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
			transform.objectType = ObjectType::Object2D;
			transform.offset.x = 0.5f;
			transform.offset.y = 0.5f;

			Math::Matrix mat;

			for (auto tile : tileSet->tiles)
			{
				Math::Vector2 size = tile.texture.GetSize();
				transform.size = Math::Vector3(size.x, size.y, 0.0f);
				transform.rotation = (float)tile.rotation;
				transform.scale = Math::Vector3(tile.flipH ? -1.0f : 1.0f, tile.flipV ? -1.0f : 1.0f, 1.0f);
				transform.position = mat.Vx() * (float)tile.x * transform.size.x + mat.Vy() * (float)tile.y * transform.size.y + Math::Vector3(0.5f, -0.5f, 0.0f) * transform.size;

				tile.texture.Draw(&transform, COLOR_WHITE, root.GetDeltaTime());
			}

			for (auto index : tileSet->selTiles)
			{
				auto& tile = tileSet->tiles[index];
				DrawCell({ (float)tile.x, (float)tile.y });
			}

			if (mode == Mode::TileSlected || mode == Mode::DragTile)
			{
				DrawCell(drag);
			}
		}

		if (mode == Mode::TilesSelection)
		{
			root.render.DebugLine2D({ rectStart.x, rectStart.y }, COLOR_GREEN, { rectEnd.x, rectStart.y }, COLOR_GREEN);
			root.render.DebugLine2D({ rectStart.x, rectEnd.y }, COLOR_GREEN, { rectEnd.x, rectEnd.y }, COLOR_GREEN);
			root.render.DebugLine2D({ rectStart.x, rectStart.y }, COLOR_GREEN, { rectStart.x, rectEnd.y }, COLOR_GREEN);
			root.render.DebugLine2D({ rectEnd.x, rectStart.y }, COLOR_GREEN, { rectEnd.x, rectEnd.y }, COLOR_GREEN);
		}

		if (imageFocused)
		{
			editorDrawer.DrawWindowBorder();
		}

		root.render.ExecutePool(1000, 0.0f);

		root.render.GetDevice()->Present();
	}

	void TileSetWindow::DrawCell(Math::Vector2 pos)
	{
		Math::Vector2 step = { (float)tileSet->sizeX, (float)tileSet->sizeY };
		step *= Sprite::ToUnits(1.0f);

		Math::Matrix mat;
		Math::Vector3 p1 = mat.Vx() * pos.x * step.x + mat.Vy() * pos.y * step.y;
		Math::Vector3 p2 = mat.Vx() * (pos.x + 1.0f) * step.x + mat.Vy() * (pos.y - 1.0f) * step.y;

		Color color = COLOR_YELLOW;

		root.render.DebugLine(p1, color, Math::Vector3(p2.x, p1.y, 0.0f), color, false);
		root.render.DebugLine(Math::Vector3(p2.x, p1.y, 0.0f), color, p2, color, false);
		root.render.DebugLine(p2, color, Math::Vector3(p1.x, p2.y, 0.0f), color, false);
		root.render.DebugLine(Math::Vector3(p1.x, p2.y, 0.0f), color, p1, color, false);

		p1 += Sprite::ToUnits(Math::Vector3( 1.0f, 1.0f, 0.0f )) / tileSet->camZoom;
		p2 -= Sprite::ToUnits(Math::Vector3( 1.0f, 1.0f, 0.0f )) / tileSet->camZoom;

		root.render.DebugLine(p1, color, Math::Vector3(p2.x, p1.y, 0.0f), color, false);
		root.render.DebugLine(Math::Vector3(p2.x, p1.y, 0.0f), color, p2, color, false);
		root.render.DebugLine(p2, color, Math::Vector3(p1.x, p2.y, 0.0f), color, false);
		root.render.DebugLine(Math::Vector3(p1.x, p2.y, 0.0f), color, p1, color, false);
	}

	void TileSetWindow::TryAddSlice(AssetTextureRef& texture, Math::Vector2 offset, int sliseIndex)
	{
		AssetTextureRef subSlice = texture;
		subSlice.sliceIndex = sliseIndex;

		auto& slice = texture.Get()->slices[sliseIndex];
		Math::Vector2 pos = { drag.x + (int)((slice.pos.x - offset.x) / (float)tileSet->sizeX), drag.y - (int)((slice.pos.y - offset.y) / (float)tileSet->sizeY) };

		if (FindTileIndex(pos) == -1)
		{
			AssetTileSet::Tile tile;
			tile.x = (int)pos.x;
			tile.y = (int)pos.y;
			tile.texture = texture;
			tile.texture.sliceIndex = sliseIndex;

			tileSet->tiles.push_back(tile);
		}
	}

	void TileSetWindow::ShowImage()
	{
		ImGui::Begin("ImageTileSet");

		ImGuiIO& io = ImGui::GetIO();

		ImVec2 size = ImGui::GetContentRegionAvail();

		ImVec2 viewportPos = ImVec2(io.MousePos.x - ImGui::GetCursorScreenPos().x, io.MousePos.y - ImGui::GetCursorScreenPos().y);

		if (size.y < 0)	size.y = -size.y;

		DrawViewport(Math::Vector2(size.x, size.y));

		ImGui::Image(root.render.GetDevice()->GetBackBuffer(), size);

		if (imageFocused)
		{
			if (io.KeysDown[VK_DELETE])
			{
				for (auto index : tileSet->selTiles)
				{
					tileSet->tiles.erase(tileSet->tiles.begin() + index);
				}

				tileSet->selTiles.clear();

				tileSet->Save();
			}
		}

		ImGuiPayload& payload = ImGui::GetCurrentContext()->DragDropPayload;

		if (ImGui::BeginDragDropTarget())
		{
			Math::Vector2 step = { (float)tileSet->sizeX, (float)tileSet->sizeY };
			step *= Sprite::ToUnits(1.0f);

			Math::Matrix mat;
			Math::Vector3 pos = mat.Vx() * drag.x * step.x + mat.Vy() * drag.y * step.y;

			if (ImGui::AcceptDragDropPayload("_ASSET_TEX", ImGuiDragDropFlags_AcceptNoDrawDefaultRect) && tileSet)
			{
				AssetTextureRef texture = *reinterpret_cast<AssetTextureRef**>(payload.Data)[0];

				if (texture.sliceIndex == -1 && texture.animIndex == -1)
				{
					Math::Vector2 offset = texture.GetSize();

					for (auto slice : texture.Get()->slices)
					{
						offset.Min(slice.pos);
					}

					for (int i = 0; i < texture.Get()->slices.size(); i++)
					{
						TryAddSlice(texture, offset, i);
					}

					tileSet->Save();
				}
				else
				if (FindTileIndex(drag) == -1)
				{
					AssetTileSet::Tile tile;
					tile.x = (int)drag.x;
					tile.y = (int)drag.y;
					tile.texture = texture;

					tileSet->tiles.push_back(tile);
					tileSet->Save();
				}

				mode = Mode::None;
			}
			else
			if (ImGui::AcceptDragDropPayload("_ANIM_FRAMES", ImGuiDragDropFlags_AcceptNoDrawDefaultRect) && tileSet)
			{
				AssetTextureRef texture = *reinterpret_cast<AssetTextureRef**>(payload.Data)[0];

				Math::Vector2 offset = texture.GetSize();

				for (auto sliceIndex : SpriteWindow::instance->selectedSlices)
				{
					auto& slice = texture.Get()->slices[sliceIndex];

					offset.Min(slice.pos);
				}

				for (auto sliceIndex : SpriteWindow::instance->selectedSlices)
				{
					TryAddSlice(texture, offset, sliceIndex);
				}

				tileSet->Save();

				mode = Mode::None;
			}
			else
			{
				mode = Mode::DragTile;
			}

			ImGui::EndDragDropTarget();
		}
		else
		{
			if (mode == Mode::DragTile)
			{
				mode = Mode::None;
			}
		}

		imageFocused = editor.AllowGrabFocused() && ImGuiHelper::IsFocused();

		if (imageFocused)
		{
			tileSet->camZoom = Math::Clamp(tileSet->camZoom + io.MouseWheel * 0.5f, 0.4f, 3.0f);
			editor.Unfocus();
		}

		vireportHowered = ImGui::IsItemHovered();

		if (imageFocused && vireportHowered && ImGui::IsMouseClicked(0))
		{
			OnLeftMouseDown();
			viewportCaptured = true;
			editor.DisallowMainFocus(true);
		}

		if (imageFocused && vireportHowered && mode == Mode::None && ImGui::IsMouseClicked(1))
		{
			mode = Mode::DragField;
			viewportCaptured = true;
			editor.DisallowMainFocus(true);
		}

		ImVec2 del = ImGui::GetMouseDragDelta(0);
		OnMouseMove(Math::Vector2((float)viewportPos.x, (float)viewportPos.y),
					fabsf(del.x) > 7.0f && fabsf(del.y) > 7.0f && ImGui::IsWindowFocused() &&
					viewportPos.x > 0 && viewportPos.x < size.x&& viewportPos.y > 0 && viewportPos.y < size.x);

		if (viewportCaptured)
		{
			if (ImGui::IsMouseReleased(0))
			{
				OnLeftMouseUp();
				viewportCaptured = false;
				editor.DisallowMainFocus(false);
			}
			else
			if (ImGui::IsMouseReleased(1))
			{
				mode = Mode::None;
				viewportCaptured = false;
				editor.DisallowMainFocus(false);
			}
		}

		ImGui::End();
	}

	void TileSetWindow::ShowTileProperties()
	{
		ImGui::Begin("TileProperties");

		ImGui::Columns(2);

		if (tileSet)
		{
			if (tileSet->selTiles.size() == 1)
			{
				auto* tile = &tileSet->tiles[tileSet->selTiles[0]];
				auto* metaData = tile->GetMetaData();

				metaData->Prepare(tile);
				metaData->ImGuiWidgets(nullptr);

				if (metaData->IsValueWasChanged())
				{
					tileSet->Save();
				}
			}
		}

		ImGui::Columns(1);

		ImGui::End();
	}

	void TileSetWindow::OnMouseMove(Math::Vector2 ms, bool dragMouse)
	{
		Math::Vector2 delta(prevMs.x - ms.x, prevMs.y - ms.y);

		if (mode == Mode::DragField)
		{
			tileSet->camPos += Math::Vector2(delta.x, -delta.y) / tileSet->camZoom;
		}
		
		if (mode == Mode::TilesSelection)
		{
			rectEnd = ms;
		}

		prevMs = ms;

		drag = MouseToCell(prevMs);
	}

	void TileSetWindow::OnLeftMouseDown()
	{
		if (tileSet)
		{
			if (mode == Mode::TilesSelected)
			{
			}
			else
			if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_X)) || ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Z)))
			{
				mode = Mode::TilesSelection;

				if (!ImGui::IsKeyPressed(ImGuiKey_LeftShift))
				{
					tileSet->selTiles.clear();
				}

				rectStart = prevMs;
			}
			else
			{
				tileSet->selTiles.clear();

				int selTile = FindTileIndex(drag);

				if (selTile != -1)
				{
					mode = Mode::TileSlected;
					tileSet->selTiles.push_back(selTile);
				}
			}
		}		
	}

	void TileSetWindow::OnLeftMouseUp()
	{
		if (mode == Mode::TilesSelection)
		{
			auto pos1 = MouseToCell(rectStart);
			auto pos2 = MouseToCell(rectEnd);

			int startX = (int)fmin(pos1.x, pos2.x);
			int endX = (int)fmax(pos1.x, pos2.x);

			int startY = (int)fmin(pos1.y, pos2.y);
			int endY = (int)fmax(pos1.y, pos2.y);

			for (int y = startY; y <= endY; y++)
				for (int x = startX; x <= endX; x++)
				{
					for (int i = 0; i < tileSet->tiles.size(); i++)
					{
						if (tileSet->tiles[i].x == x && tileSet->tiles[i].y == y)
						{
							bool canAdd = true;

							for (auto index : tileSet->selTiles)
							{
								if (index == i)
								{
									canAdd = false;
									break;
								}
							}
							
							if (canAdd)
							{
								tileSet->selTiles.push_back(i);
							}

							break;
						}
					}
				}
		}
		else
		if (tileSet->selTiles.size() == 1)
		{
			if (FindTileIndex(drag) == -1)
			{
				auto& tile = tileSet->tiles[tileSet->selTiles[0]];

				tile.x = (int)drag.x;
				tile.y = (int)drag.y;

				tileSet->Save();
			}
		}

		mode = Mode::None;
	}

	Math::Vector2 TileSetWindow::MouseToCell(Math::Vector2 msPos)
	{
		Math::Vector3 mouseOrigin;
		Math::Vector3 mouseDirection;

		Math::GetMouseRay(msPos, mouseOrigin, mouseDirection);

		Math::Vector3 pos;
		Math::IntersectPlaneRay(0.0f, { 0.0f, 0.0f, 1.0f }, mouseOrigin, mouseDirection, pos);

		if (pos.x < 0.0f)
		{
			pos.x -= Sprite::ToUnits((float)tileSet->sizeX);
		}

		if (pos.y > 0.0f)
		{
			pos.y += Sprite::ToUnits((float)tileSet->sizeX);
		}

		int x = (int)(pos.x / Sprite::ToUnits((float)tileSet->sizeX));
		int y = (int)(pos.y / Sprite::ToUnits((float)tileSet->sizeY));

		return { (float)x, (float)y };
	}

	int TileSetWindow::FindTileIndex(Math::Vector2 pos)
	{
		int x = (int)pos.x;
		int y = (int)pos.y;

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

	void TileSetWindow::Release()
	{
		DELETE_PTR(instance);

		tileSet = nullptr;
	}
}

#endif
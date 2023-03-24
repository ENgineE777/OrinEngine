
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

		if (instance)
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
			ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.8f, nullptr, &dock_main_id);
			ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
			
			ImGui::DockBuilderDockWindow("ImageTileSet", dock_left_id);
			ImGui::DockBuilderDockWindow("TileProperties", dock_right_id);

			ImGui::DockBuilderFinish(dock_main_id);

			ImGuiDockNode* node;

			node = ImGui::DockBuilderGetNode(dock_left_id);
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
				transform.position = mat.Vx() * (float)tile.x * transform.size.x + mat.Vy() * (float)tile.y * transform.size.y + Math::Vector3( 0.5f, -0.5f, 0.0f ) * transform.size;

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
		step *= Sprite::ToUnits(1.0f);

		Math::Matrix mat;
		Math::Vector3 p1 = mat.Vx() * (float)x * step.x + mat.Vy() * (float)y * step.y;
		Math::Vector3 p2 = mat.Vx() * (float)(x + 1) * step.x + mat.Vy() * (float)(y - 1) * step.y;

		Color color = COLOR_WHITE;

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

		int x = dragX + (int)((slice.pos.x - offset.x) / (float)tileSet->sizeX);
		int y = dragY - (int)((slice.pos.y - offset.y) / (float)tileSet->sizeY);

		if (FindTileIndex(x, y) == -1)
		{
			AssetTileSet::Tile tile;
			tile.x = x;
			tile.y = y;
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
			step *= Sprite::ToUnits(1.0f);

			Math::Matrix mat;
			Math::Vector3 pos = mat.Vx() * (float)dragX * step.x + mat.Vy() * (float)dragY * step.y;

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
				if (FindTileIndex(dragX, dragY) == -1)
				{
					AssetTileSet::Tile tile;
					tile.x = dragX;
					tile.y = dragY;
					tile.texture = texture;

					tileSet->tiles.push_back(tile);
					tileSet->Save();
				}

				drag = Drag::DragNone;
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

		if (imageFocused && vireportHowered && ImGui::IsMouseClicked(0))
		{
			OnLeftMouseDown();
			viewportCaptured = true;
		}		

		root.controls.SetFocused(GetFocus() == editor.hwnd && ImGui::IsWindowFocused());

		if (viewportCaptured && imageFocused && vireportHowered && root.controls.GetAliasState(editor.freeCamera.alias_move2d_active, AliasAction::Pressed))
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

	void TileSetWindow::ShowTileProperties()
	{
		ImGui::Begin("TileProperties");

		ImGui::Columns(2);

		if (tileSet)
		{
			if (tileSet->selTile != -1)
			{
				auto* tile = &tileSet->tiles[tileSet->selTile];
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
			pos.x -= Sprite::ToUnits((float)tileSet->sizeX);
		}

		x = (int)(pos.x / Sprite::ToUnits((float)tileSet->sizeX));

		if (pos.y > 0.0f)
		{
			pos.y += Sprite::ToUnits((float)tileSet->sizeX);
		}

		y = (int)(pos.y / Sprite::ToUnits((float)tileSet->sizeY));
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

	void TileSetWindow::Release()
	{
		DELETE_PTR(instance);

		tileSet = nullptr;
	}
}

#endif
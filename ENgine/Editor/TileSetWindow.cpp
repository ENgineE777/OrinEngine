
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
			//texture = nullptr;
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

		/*ImGuiID dockspaceID = ImGui::GetID("SpriteEditor");

		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		if (!ImGui::DockBuilderGetNode(dockspaceID))
		{
			ImGui::DockBuilderRemoveNode(dockspaceID);
			ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);

			ImGuiID dock_main_id = dockspaceID;
			ImGuiID dock_top_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.6f, nullptr, &dock_main_id);
			ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

			ImGui::DockBuilderDockWindow("Image Info", dock_bottom_id);
			ImGui::DockBuilderDockWindow("Auto slicing", dock_bottom_id);
			ImGui::DockBuilderDockWindow("Selected Slices", dock_bottom_id);
			ImGui::DockBuilderDockWindow("Animations", dock_bottom_id);
			ImGui::DockBuilderDockWindow("Image", dock_top_id);

			ImGui::DockBuilderFinish(dock_main_id);

			ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_top_id);
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
		}

		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);*/

		ImGui::End();

		/*ShowImageInfo();
		ShowAutoSlicing();
		ShowSlices();
		ShowAnimations();
		ShowImage();*/
	}

	void TileSetWindow::DrawViewport(Math::Vector2 viewportSize)
	{
		/*lastViewportSize = viewportSize;

		root.render.GetDevice()->SetBackBuffer(1, (int)viewportSize.x, (int)viewportSize.y, &editorDrawer.hwnd);

		root.render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

		editorDrawer.SetCameraMatrices(camPos, viewportSize.y / camZoom, viewportSize.y / viewportSize.x);
	
		editorDrawer.DrawCheckerBoard(camPos, viewportSize, camZoom);

		Color color = COLOR_WHITE;

		Math::Matrix spriteTrans;
		spriteTrans.Pos().y = texture->size.y;

		Sprite::Draw(nullptr, COLOR_GRAY_A(0.65f), spriteTrans, 0.0f, texture->size, 0.0f, 1.0f, Sprite::quadPrgNoZ);
		Sprite::Draw(texture->texture, COLOR_WHITE, spriteTrans, 0.0f, texture->size, 0.0f, 1.0f, Sprite::quadPrgNoZ);

		for (int i = 0; i < texture->slices.size(); i++)
		{
			if (selSlice == i)
			{
				continue;
			}

			auto& slice = texture->slices[i];
			Math::Vector2 pos(slice.pos.x, texture->size.y - slice.pos.y);
			DrawRect(pos, pos + Math::Vector2(slice.size.x, -slice.size.y), color);
		}

		if (drag == Drag::DragNewSlice)
		{
			DrawRect(rectStart, rectEnd, color);
		}

		if (selSlice != -1)
		{
			for (int i = 0; i < rectHeight; i++)
				for (int j = 0; j<rectWidth; j++)
				{
					int index = rectWidth * i + j;

					color = COLOR_GREEN;

					if (j < rectWidth - 1 && i < rectHeight - 1)
					{
						root.render.DebugLine((Math::Vector3(points[index + rectWidth].x, points[index + rectWidth].y, 0.0f)) * Sprite::pixelsPerUnitInvert, color,
											  (Math::Vector3(points[index].x, points[index].y, 0.0f)) * Sprite::pixelsPerUnitInvert, color, false);

						root.render.DebugLine((Math::Vector3(points[index].x, points[index].y, 0.0f)) * Sprite::pixelsPerUnitInvert, color,
												(Math::Vector3(points[index + 1].x, points[index + 1].y, 0.0f)) * Sprite::pixelsPerUnitInvert, color, false);

						if (i == rectHeight - 2 || j == rectWidth - 2)
						{
							root.render.DebugLine((Math::Vector3(points[index + 1].x, points[index + 1].y, 0.0f)) * Sprite::pixelsPerUnitInvert, color,
													(Math::Vector3(points[index + rectWidth + 1].x, points[index + rectWidth + 1].y, 0.0f)) * Sprite::pixelsPerUnitInvert, color, false);

							root.render.DebugLine((Math::Vector3(points[index + rectWidth + 1].x, points[index + rectWidth + 1].y, 0.0f)) * Sprite::pixelsPerUnitInvert, color,
													(Math::Vector3(points[index + rectWidth].x, points[index + rectWidth].y, 0.0f)) * Sprite::pixelsPerUnitInvert, color, false);
						}
					}

					color = COLOR_WHITE;

					if (selRow == i && selCol == j)
					{
						color.Set(1.0, 0.9f, 0.0f, 1.0f);
					}

					Math::Vector3 pos = root.render.TransformToScreen((Math::Vector3(points[index].x, points[index].y, 0.0f)) * Sprite::pixelsPerUnitInvert, 2);
					root.render.DebugSprite(editorDrawer.anchornTex, Math::Vector2(pos.x - 4, pos.y - 4), Math::Vector2(8.0f), color);
				}
		}
		else
		{
			for (int i = 0; i < selectedSlices.size(); i++)
			{
				auto& slice = texture->slices[selectedSlices[i]];

				Math::Vector2 pos(slice.pos.x, texture->size.y - slice.pos.y);
				DrawRect(pos, pos + Math::Vector2(slice.size.x, -slice.size.y), COLOR_GREEN);
			}
		}

		if (imageFocused)
		{
			editorDrawer.DrawWindowBorder();
		}

		root.render.ExecutePool(1000, 0.0f);

		root.render.GetDevice()->Present();*/
	}

	void TileSetWindow::OnMouseMove(Math::Vector2 ms, bool dragMouse)
	{
		/*Math::Vector2 delta(prevMs.x - ms.x, prevMs.y - ms.y);

		if (drag == Drag::DragField)
		{
			camPos += Math::Vector2(delta.x, -delta.y) / camZoom;

			camPos.x = Math::Clamp(camPos.x, 0.0f, texture->size.x);
			camPos.y = Math::Clamp(camPos.y, 0.0f, texture->size.y);
		}
		else
		if (drag == Drag::DragRects)
		{
			MoveRects(Math::Vector2(delta.x, delta.y) / camZoom);
		}

		prevMs = ms;

		if (!inDragAndDrop && dragMouse && drag == Drag::DragNone)
		{
			drag = Drag::DragNewSlice;
		}

		if (drag == Drag::DragNewSlice)
		{
			rectEnd = camPos + Math::Vector2(prevMs.x - lastViewportSize.x * 0.5f, -prevMs.y + lastViewportSize.y * 0.5f) / camZoom;
		}*/
	}

	void TileSetWindow::OnLeftMouseDown()
	{
		/*if (inDragAndDrop || root.controls.DebugKeyPressed("KEY_LCONTROL", AliasAction::Pressed, true))
		{
			return;
		}

		Math::Vector2 ps(prevMs.x, -prevMs.y);

		rectStart = camPos + Math::Vector2(prevMs.x - lastViewportSize.x * 0.5f, -prevMs.y + lastViewportSize.y * 0.5f) / camZoom;

		int prevSelSlice = selSlice;
		selSlice = -1;

		bool multiSelected = root.controls.DebugKeyPressed("KEY_LSHIFT", AliasAction::Pressed, true);

		float bufferZone = 3.0f;
		for (int i = 0; i < texture->slices.size(); i++)
		{
			auto& slice = texture->slices[i];

			if (slice.pos.x - bufferZone < rectStart.x && rectStart.x < slice.pos.x + slice.size.x + bufferZone &&
				texture->size.y - slice.pos.y - slice.size.y - bufferZone < rectStart.y && rectStart.y < texture->size.y - slice.pos.y + bufferZone)
			{
				if (multiSelected)
				{
					bool add = false;

					if (prevSelSlice != -1)
					{
						selectedSlices.push_back(prevSelSlice);

						selSlice = -1;
					}

					bool alreadyAdded = false;

					for (int j = 0; j < selectedSlices.size(); j++)
					{
						if (selectedSlices[j] == i)
						{
							alreadyAdded = true;

							selectedSlices.erase(selectedSlices.begin() + j);

							break;
						}
					}

					if (!alreadyAdded)
					{
						selectedSlices.push_back(i);

						selSlice = -1;

						break;
					}

					break;
				}
				else
				{
					selSlice = i;

					FillRects();
				}
			}
		}

		if (!multiSelected && selSlice == -1)
		{
			selectedSlices.clear();
		}

		if (selSlice != -1)
		{
			if (selectedSlices.size() > 0)
			{
				for (int j = 0; j < selectedSlices.size(); j++)
				{
					if (selectedSlices[j] == selSlice)
					{
						selSlice = -1;
						break;
					}
				}

				if (selSlice == -1)
				{
					drag = Drag::DragRects;
				}
				else
				{
					selectedSlices.clear();
					drag = Drag::DragSelectRect;
				}
			}
			else
			if (prevSelSlice != selSlice)
			{
				drag = Drag::DragSelectRect;
			}
			else
			{
				drag = Drag::DragRects;

				selCol = -1;
				selRow = -1;

				for (int i = 0; i < rectHeight; i++)
					for (int j = 0; j < rectWidth; j++)
					{
						Math::Vector2 point = points[rectWidth * i + j];

						if (point.x - 7 < rectStart.x && rectStart.x < point.x + 7 &&
							point.y - 7 < rectStart.y && rectStart.y < point.y + 7)
						{
							selRow = i;
							selCol = j;
						}
					}
			}
		}*/
	}

	void TileSetWindow::OnLeftMouseUp()
	{
		/*if (drag == Drag::DragNewSlice)
		{
			AssetTexture::Slice slice;
			slice.pos = Math::Vector2(fminf(rectStart.x, rectEnd.x), texture->size.y - fmaxf(rectStart.y, rectEnd.y));
			slice.size = Math::Vector2(fmaxf(rectStart.x, rectEnd.x), fmaxf(rectStart.y, rectEnd.y)) - Math::Vector2(fminf(rectStart.x, rectEnd.x), fminf(rectStart.y, rectEnd.y));

			slice.name = StringUtils::PrintTemp("Slice%i", texture->slices.size());

			selSlice = (int)texture->slices.size();
			texture->slices.push_back(slice);

			FillRects();
		}
		else
		if (drag == Drag::DragRects)
		{
			if (selSlice == -1)
			{
				for (int j = 0; j < selectedSlices.size(); j++)
				{
					auto& slice = texture->slices[selectedSlices[j]];
					slice.pos.x = floorf(slice.pos.x);
					slice.pos.y = floorf(slice.pos.y);
				}
			}
			else
			{
				FillRects();
			}
		}


		selCol = -1;
		selRow = -1;
		drag = Drag::DragNone;
		inDragAndDrop = false;*/
	}
}

#endif

#ifdef OAK_EDITOR

#include "SpriteWindow.h"
#include "Root/Root.h"

#include "Editor/EditorDrawer.h"

#include "imgui.h"

namespace Oak
{
	AssetTexture* SpriteWindow::texture;
	SpriteWindow* SpriteWindow::instance = nullptr;

	void ShowSpriteWindow(AssetTexture* texture)
	{
		SpriteWindow::StartEdit(texture);
	}

	void SpriteWindow::StartEdit(AssetTexture* setTexture)
	{
		if (!instance)
		{
			instance = new SpriteWindow();
		}

		texture = setTexture;
		instance->Prepare();
		instance->opened = true;
	}

	void SpriteWindow::Prepare()
	{
		textureRef = AssetTextureRef(texture, _FL_);

		camZoom = lastViewportSize.y / texture->size.y;

		camPos = texture->size * 0.5f;

		if ((float)texture->size.x * camZoom > lastViewportSize.x)
		{
			camZoom = lastViewportSize.x / texture->size.x;
		}

		deltaMouse = 0.0f;
		camZoom = 1.0f;
	}

	void SpriteWindow::FillPoints(int index, int stride, float val)
	{
		auto& slice = texture->slices[selSlice];

		points[index + stride * 0] = Math::Vector2(val, slice.pos.y + texture->size.y);
		points[index + stride * 1] = Math::Vector2(val, slice.pos.y - slice.upLeftOffset.y + texture->size.y);
		points[index + stride * 2] = Math::Vector2(val, slice.pos.y - slice.size.y + slice.downRightOffset.y + texture->size.y);
		points[index + stride * 3] = Math::Vector2(val, slice.pos.y - slice.size.y + texture->size.y);
	}

	void SpriteWindow::FillRects()
	{
		auto& slice = texture->slices[selSlice];

		if (slice.isNineSliced)
		{
			rectWidth = 4;
			rectHeight = 4;

			FillPoints(0, 4, slice.pos.x);
			FillPoints(1, 4, slice.pos.x + slice.upLeftOffset.x);
			FillPoints(2, 4, slice.pos.x + slice.size.x - slice.downRightOffset.x);
			FillPoints(3, 4, slice.pos.x + slice.size.x);
		}
		else
		{
			rectWidth = 2;
			rectHeight = 2;

			points[0] = Math::Vector2(slice.pos.x, slice.pos.y + texture->size.y);
			points[1] = Math::Vector2(slice.pos.x + slice.size.x, slice.pos.y + texture->size.y);
			points[2] = Math::Vector2(slice.pos.x, slice.pos.y - slice.size.y + texture->size.y);
			points[3] = Math::Vector2(slice.pos.x + slice.size.x, slice.pos.y - slice.size.y + texture->size.y);
		}
	}

	void SpriteWindow::UpdateSlice()
	{
		auto& slice = texture->slices[selSlice];

		slice.pos = points[0];
		slice.pos.y -= texture->size.y;

		slice.size = points[rectHeight * rectWidth - 1] - points[0];
		slice.size.y = -slice.size.y;

		if (rectWidth > 2)
		{
			slice.upLeftOffset.x = points[1].x - points[0].x;
			slice.downRightOffset.x = points[3].x - points[2].x;
		}

		if (rectHeight > 2)
		{
			slice.upLeftOffset.y = points[0].y - points[rectWidth].y;
			slice.downRightOffset.y = points[2 * rectWidth].y - points[3 * rectWidth].y;
		}

		texture->SaveMetaData();
	}

	void SpriteWindow::ImGui()
	{
		if (!opened)
		{
			texture = nullptr;
			return;
		}

		ImGui::Begin("Sprite Editor", &opened, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus);

		if (needSetSize)
		{
			ImGui::SetWindowSize(ImVec2(800.0f, 600.0f));
			needSetSize = false;
		}

		ImGui::BeginGroup();

		ImGui::Dummy(ImVec2(120, 3));

		if (selSlice != -1)
		{
			auto& slice = texture->slices[selSlice];
			
			bool changed = false;

			ImGui::Text("Name");
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(40.0f - ImGui::CalcTextSize("Name").x, 1.0f));
			ImGui::SameLine();

			struct Funcs
			{
				static int ResizeCallback(ImGuiInputTextCallbackData* data)
				{
					if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
					{
						eastl::string* str = (eastl::string*)data->UserData;
						str->resize(data->BufSize + 1);
						data->Buf = str->begin();
					}
					return 0;
				}
			};

			ImGui::SetNextItemWidth(60.0f);
			ImGui::InputText("###SliceName", slice.name.begin(), (size_t)slice.name.size() + 1, ImGuiInputTextFlags_CallbackResize, Funcs::ResizeCallback, (void*)&slice.name);

			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(40.0f - ImGui::CalcTextSize("X").x, 1.0f));
			ImGui::SameLine();

			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::InputFloat("###SliceX", &slice.pos.x)) changed = true;

			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(40.0f - ImGui::CalcTextSize("Y").x, 1.0f));
			ImGui::SameLine();

			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::InputFloat("###SliceY", &slice.pos.y)) changed = true;

			ImGui::Text("Width");
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(40.0f - ImGui::CalcTextSize("Width").x, 1.0f));
			ImGui::SameLine();

			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::InputFloat("###SliceWidth", &slice.size.x)) changed = true;

			ImGui::Text("Heigth");
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(40.0f - ImGui::CalcTextSize("Heigth").x, 1.0f));
			ImGui::SameLine();

			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::InputFloat("###SliceHeight", &slice.size.y)) changed = true;

			ImGui::Text("Is 9-slice");
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(40.0f - ImGui::CalcTextSize("Is 9-slice").x, 1.0f));
			ImGui::SameLine();

			if (ImGui::Checkbox("###Is9Slice", &slice.isNineSliced)) changed = true;

			if (changed)
			{
				FillRects();
			}
		}

		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginChild("ImageView");

		ImGuiIO& io = ImGui::GetIO();

		if (selSlice != -1 && imageFocused)
		{
			if (io.KeysDown[VK_DELETE])
			{
				texture->slices.erase(texture->slices.begin() + selSlice);
				selSlice = -1;
			}
			else
			{
				Math::Vector2 delta;

				if (ImGui::IsKeyPressed(VK_UP))
				{
					delta.y = 1.0f;
				}

				if (ImGui::IsKeyPressed(VK_DOWN))
				{
					delta.y = -1.0f;
				}

				if (ImGui::IsKeyPressed(VK_LEFT))
				{
					delta.x = 1.0f;
				}

				if (ImGui::IsKeyPressed(VK_RIGHT))
				{
					delta.x = -1.0f;
				}

				MoveRects(delta);
			}
		}

		ImVec2 size = ImGui::GetContentRegionAvail();

		ImVec2 viewportPos = ImVec2(io.MousePos.x - ImGui::GetCursorScreenPos().x, io.MousePos.y - ImGui::GetCursorScreenPos().y);

		DrawViewport(Math::Vector2(size.x, size.y));

		ImGui::Image(Oak::root.render.GetDevice()->GetBackBuffer(), size);

		if (root.controls.DebugKeyPressed("KEY_LCONTROL", AliasAction::Pressed, true) && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			textureRef.sliceIndex = selSlice;
			Assets::AssetRef* ptr = (Assets::AssetRef*)&textureRef;
			ImGui::SetDragDropPayload("_ASSET_TEX", &ptr, sizeof(Assets::AssetRef*));
			ImGui::EndDragDropSource();

			inDragAndDrop = true;
		}

		imageFocused = ImGui::IsWindowFocused();

		camZoom = Math::Clamp(camZoom + io.MouseWheel * 0.5f, 0.4f, 3.0f);

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
		OnMouseMove(Math::Vector2((float)viewportPos.x,(float)viewportPos.y),
					fabsf(del.x) > 7.0f && fabsf(del.y) > 7.0f && ImGui::IsWindowFocused() &&
					viewportPos.x > 0 && viewportPos.x < size.x && viewportPos.y > 0 && viewportPos.y < size.x);

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

		ImGui::EndChild();

		ImGui::End();
	}

	void SpriteWindow::DrawRect(Math::Vector2 p1, Math::Vector2 p2, Color color)
	{
		root.render.DebugLine(Math::Vector3(p1.x, p1.y, 0.0f) * Sprite::pixelsPerUnitInvert, color,
			Math::Vector3(p2.x, p1.y, 0.0f) * Sprite::pixelsPerUnitInvert, color, false);

		root.render.DebugLine(Math::Vector3(p2.x, p1.y, 0.0f) * Sprite::pixelsPerUnitInvert, color,
			Math::Vector3(p2.x, p2.y, 0.0f) * Sprite::pixelsPerUnitInvert, color, false);

		root.render.DebugLine(Math::Vector3(p2.x, p2.y, 0.0f) * Sprite::pixelsPerUnitInvert, color,
			Math::Vector3(p1.x, p2.y, 0.0f) * Sprite::pixelsPerUnitInvert, color, false);

		root.render.DebugLine(Math::Vector3(p1.x, p2.y, 0.0f) * Sprite::pixelsPerUnitInvert, color,
			Math::Vector3(p1.x, p1.y, 0.0f) * Sprite::pixelsPerUnitInvert, color, false);
	}

	void SpriteWindow::DrawViewport(Math::Vector2 viewportSize)
	{
		lastViewportSize = viewportSize;

		root.render.GetDevice()->SetBackBuffer(1, (int)viewportSize.x, (int)viewportSize.y, &editorDrawer.hwnd);

		root.render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);

		float dist = (viewportSize.y * 0.5f * Sprite::pixelsPerUnitInvert) / (tanf(22.5f * Math::Radian * camZoom));
		Math::Matrix view;
		view.BuildView(Math::Vector3(camPos.x * Sprite::pixelsPerUnitInvert, camPos.y * Sprite::pixelsPerUnitInvert, -dist), Math::Vector3(camPos.x * Sprite::pixelsPerUnitInvert, camPos.y * Sprite::pixelsPerUnitInvert, -dist + 1.0f), Math::Vector3(0, 1, 0));
		root.render.SetTransform(TransformStage::View, view);

		Math::Matrix proj;
		proj.BuildProjection(45.0f * Math::Radian, viewportSize.y / viewportSize.x, 1.0f, 1000.0f);
		root.render.SetTransform(TransformStage::Projection, proj);

		Transform trans;
	
		Math::Matrix spriteTrans;
		spriteTrans.Pos().z = 0.01f;

		Sprite::Draw(editorDrawer.checkerTex, COLOR_WHITE, spriteTrans,
					Math::Vector2(camPos.x - viewportSize.x * 0.5f / camZoom, camPos.y - viewportSize.y * 0.5f / camZoom) , viewportSize / camZoom,
					Math::Vector2(((int)(camPos.x) % 42) / 42.0f, 1.0f - ((int)(camPos.y) % 42) / 42.0f),
					Math::Vector2(viewportSize.x / camZoom / 42.0f, viewportSize.y / camZoom / 42.0f));

		float wd = texture->texture ? texture->size.x * 1.1f : 512.0f;
		float ht = texture->texture ? texture->size.y * 1.1f : 512.0f;

		Color color = COLOR_WHITE;

		spriteTrans.Pos().z = 0.005f;
		Sprite::Draw(nullptr, COLOR_GRAY_A(0.65f), spriteTrans, 0.0f, texture->size, 0.0f, 1.0f);

		spriteTrans.Pos().z = 0.0f;
		Sprite::Draw(texture->texture, COLOR_WHITE, spriteTrans, 0.0f, texture->size, 0.0f, 1.0f);

		for (int i = 0; i < texture->slices.size(); i++)
		{
			if (selSlice == i)
			{
				continue;
			}

			auto& slice = texture->slices[i];
			Math::Vector2 pos = slice.pos;
			pos.y += texture->size.y;
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

		if (imageFocused)
		{
			Color color(1.0, 0.65f, 0.0f, 1.0f);

			for (float i = 0; i < 3.0f; i += 1.0f)
			{
				root.render.DebugLine2D(Math::Vector2(0.5f, 0.5f + i), color, Math::Vector2((float)root.render.GetDevice()->GetWidth(), 0.5f + i), color);
				root.render.DebugLine2D(Math::Vector2(0.5f, (float)root.render.GetDevice()->GetHeight() - 0.5f - i), color, Math::Vector2((float)root.render.GetDevice()->GetWidth(), (float)root.render.GetDevice()->GetHeight() - 0.5f - i), color);
				root.render.DebugLine2D(Math::Vector2(0.5f + i, 0.5f), color, Math::Vector2(0.5f + i, (float)root.render.GetDevice()->GetHeight()), color);
				root.render.DebugLine2D(Math::Vector2((float)root.render.GetDevice()->GetWidth() - i - 0.5f, 0.5f), color, Math::Vector2((float)root.render.GetDevice()->GetWidth() - i - 0.5f, (float)root.render.GetDevice()->GetHeight()), color);
			}

			borderDrawed = true;
		}

		root.render.ExecutePool(1000, 0.0f);

		root.render.GetDevice()->Present();
	}

	void SpriteWindow::OnMouseMove(Math::Vector2 ms, bool dragMouse)
	{
		deltaMouse.x += (prevMs.x - ms.x);
		deltaMouse.y += (prevMs.y - ms.y);

		Math::Vector2 delta(0.0f);

		if (fabs(deltaMouse.x) > 1.0f)
		{
			delta.x = floorf(deltaMouse.x);
			deltaMouse.x = 0.0f;
		}

		if (fabs(deltaMouse.y) > 1.0f)
		{
			delta.y = floorf(deltaMouse.y);
			deltaMouse.y = 0.0f;
		}

		if (drag == Drag::DragField)
		{
			camPos += Math::Vector2(delta.x, -delta.y) * camZoom;

			camPos.x = Math::Clamp(camPos.x, 0.0f, texture->size.x);
			camPos.y = Math::Clamp(camPos.y, 0.0f, texture->size.y);
		}
		else
		if (drag == Drag::DragRects)
		{
			MoveRects(Math::Vector2(delta.x, delta.y) * camZoom);
		}

		prevMs = ms;

		if (!inDragAndDrop && dragMouse && drag == Drag::DragNone)
		{
			drag = Drag::DragNewSlice;
		}

		if (drag == Drag::DragNewSlice)
		{
			rectEnd = camPos + Math::Vector2(prevMs.x - lastViewportSize.x * 0.5f, -prevMs.y + lastViewportSize.y * 0.5f) / camZoom;
		}
	}

	void SpriteWindow::OnLeftMouseDown()
	{
		if (inDragAndDrop || root.controls.DebugKeyPressed("KEY_LCONTROL", AliasAction::Pressed, true))
		{
			return;
		}

		Math::Vector2 ps(prevMs.x, -prevMs.y);

		rectStart = camPos + Math::Vector2(prevMs.x - lastViewportSize.x * 0.5f, -prevMs.y + lastViewportSize.y * 0.5f) / camZoom;

		selSlice = -1;

		float bufferZone = 3.0f;
		for(int i = 0; i < texture->slices.size(); i++)
		{
			auto& slice = texture->slices[i];

			if (slice.pos.x - bufferZone < rectStart.x && rectStart.x < slice.pos.x + slice.size.x + bufferZone &&
				slice.pos.y - slice.size.y - bufferZone + texture->size.y < rectStart.y && rectStart.y < slice.pos.y + bufferZone + texture->size.y)
			{
				selSlice = i;

				FillRects();
			}
		}

		if (selSlice != -1)
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
	}

	void SpriteWindow::OnLeftMouseUp()
	{
		if (drag == Drag::DragNewSlice)
		{
			AssetTexture::Slice slice;
			slice.pos = Math::Vector2(fminf(rectStart.x, rectEnd.x), fmaxf(rectStart.y, rectEnd.y));
			slice.pos.y -= texture->size.y;
			slice.size = Math::Vector2(fmaxf(rectStart.x, rectEnd.x), fmaxf(rectStart.y, rectEnd.y)) - Math::Vector2(fminf(rectStart.x, rectEnd.x), fminf(rectStart.y, rectEnd.y));

			texture->slices.push_back(slice);
		}

		selCol = -1;
		selRow = -1;
		drag = Drag::DragNone;
		inDragAndDrop = false;
	}

	void SpriteWindow::MoveRects(Math::Vector2 delta)
	{
		if (selRow == -1)
		{
			for (int i = 0; i<rectHeight; i++)
				for (int j = 0; j<rectWidth; j++)
				{
					points[rectWidth * i + j] -= Math::Vector2(delta.x, -delta.y);
				}
		}
		else
		{
			int index = rectWidth * selRow + selCol;

			int border = 3;

			if (selCol > 0 && selRow == 0)
			{
				if (points[index - 1].x + border > points[index].x - delta.x)
				{
					delta.x = points[index].x - points[index - 1].x - border;
				}
			}

			if (selCol == 0 && selRow > 0)
			{
				if (points[index - rectWidth].y - border < points[index].y + delta.y)
				{
					delta.y = points[index - rectWidth].y - border - points[index].y;
				}
			}

			if (selCol > 0 && selRow > 0)
			{
				if (points[index - rectWidth - 1].x + border > points[index].x - delta.x)
				{
					delta.x = points[index].x - points[index - rectWidth - 1].x - border;
				}

				if (points[index - rectWidth - 1].y - border < points[index].y + delta.y)
				{
					delta.y = points[index - rectWidth - 1].y - border - points[index].y;
				}
			}

			if (selCol < rectWidth - 1 && selRow == rectHeight - 1)
			{
				if (points[index + 1].x - border < points[index].x - delta.x)
				{
					delta.x = points[index].x - points[index + 1].x + border;
				}
			}

			if (selCol == rectWidth - 1 && selRow < rectHeight - 1)
			{
				if (points[index + 1].y + border > points[index].y + delta.y)
				{
					delta.y = points[index + 1].y + border - points[index].y;
				}
			}

			if (selCol < rectWidth - 1 && selRow < rectHeight - 1)
			{
				if (points[index + rectWidth + 1].x - border < points[index].x - delta.x)
				{
					delta.x = points[index].x - points[index + rectWidth + 1].x + border;
				}

				if (points[index + rectWidth + 1].y + border > points[index].y + delta.y)
				{
					delta.y = points[index + rectWidth + 1].y + border - points[index].y;
				}
			}

			for (int i = 0; i<rectHeight; i++)
			{
				points[rectWidth * i + selCol] -= Math::Vector2(delta.x, 0.0f);
			}

			for (int j = 0; j<rectWidth; j++)
			{
				points[rectWidth * selRow + j] += Math::Vector2(0.0f, delta.y);
			}
		}

		UpdateSlice();
	}
}

#endif
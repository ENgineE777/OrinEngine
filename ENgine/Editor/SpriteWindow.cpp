
#ifdef OAK_EDITOR

#include "SpriteWindow.h"
#include "Root/Root.h"

#include "Editor/EditorDrawer.h"

#include "imgui.h"

namespace Oak
{
	Sprite::Data* SpriteWindow::sprite = nullptr;
	SpriteWindow* SpriteWindow::instance = nullptr;

	void ShowSpriteWindow(Sprite::Data* data)
	{
		SpriteWindow::StartEdit(data);
	}

	void SpriteWindow::StartEdit(Sprite::Data* ed_sprite)
	{
		if (!instance)
		{
			instance = new SpriteWindow();
		}

		sprite = ed_sprite;
		instance->Prepare();
		instance->opened = true;
	}

	void SpriteWindow::FillPoints(int index, int stride, float val, bool vert)
	{
		if (vert)
		{
			points[index + stride * 0] = Math::Vector2(val, spritePos.y);
			points[index + stride * 1] = Math::Vector2(val, spritePos.y - spriteOffsetY.x);
			points[index + stride * 2] = Math::Vector2(val, spritePos.y - spriteSize.y + spriteOffsetY.y);
			points[index + stride * 3] = Math::Vector2(val, spritePos.y - spriteSize.y);
		}
		else
		{
			points[index + 0] = Math::Vector2(spritePos.x, val);
			points[index + 1] = Math::Vector2(spritePos.x + spriteOffsetX.x, val);
			points[index + 2] = Math::Vector2(spritePos.x + spriteSize.x - spriteOffsetX.y, val);
			points[index + 3] = Math::Vector2(spritePos.x + spriteSize.x, val);
		}
	}

	void SpriteWindow::SetImage(const char* img)
	{
		sprite->texName = img;
		sprite->LoadTexture();

		//load_image->SetText(img);

		if (!sprite->texture)
		{
			sprite->width = 32;
			sprite->height = 32;

			spritePos = Math::Vector2(0, (float)sprite->height);
			spriteSize = Math::Vector2((float)sprite->width, (float)sprite->height);
			spriteOffsetX = 10.0f;
			spriteOffsetY = 10.0f;

			if (sprite->rects.size() == 0)
			{
				ResizeSpriteRect();
			}

			FillRects();
			UpdateAnimRect();
			UpdateSavedPos();
			UpdateSpriteRect();
			FitImage();
		}

		//char str[128];
		//StringUtils::Printf(str, 128, "Image Size: %i x %i", sprite->width, sprite->height);
		//image_size_label->SetText(str);
	}

	void SpriteWindow::ResizeSpriteRect()
	{
		int count = 1;

		if (sprite->type == Sprite::Type::ThreeSlicesHorz || sprite->type == Sprite::Type::ThreeSlicesVert)
		{
			count = 3;
		}

		if (sprite->type == Sprite::Type::NineSlices)
		{
			count = 9;
		}

		if (sprite->type == Sprite::Type::Frames)
		{
			count = (int)frames.size() / 4;
		}

		sprite->rects.resize(count);
	}

	void SpriteWindow::UpdateSpriteRect()
	{
		int index = 0;

		Math::Vector2 image_size((float)sprite->width, (float)sprite->height);

		if (sprite->type == Sprite::Type::Frames)
		{
			for (int i = 0; i<sprite->rects.size(); i++)
			{
				Sprite::Frame& rect = sprite->rects[i];

				rect.pos = frames[i * 4 + 0];
				rect.size = frames[i * 4 + 3] - frames[i * 4 + 0];
				rect.size.y = -rect.size.y;

				rect.uv = rect.pos / image_size;
				rect.duv = rect.size / image_size;
			}

			return;
		}

		if (sprite->rects.size() == 0)
		{
			return;
		}

		for (int i = 0; i < rectHeight - 1; i++)
			for (int j = 0; j < rectWidth - 1; j++)
			{
				int indx = rectWidth * i + j;
				Sprite::Frame& rect = sprite->rects[index];

				rect.pos = points[indx];
				rect.size = points[indx + 1 + rectWidth] - points[indx];
				rect.size.y = -rect.size.y;

				rect.uv = rect.pos / image_size;
				rect.duv = rect.size / image_size;

				index++;
			}
	}

	void SpriteWindow::Prepare()
	{
		if (sprite->type == Sprite::Type::Frames)
		{
			numFrames = (int)sprite->rects.size();
		}
		else
		{
			numFrames = 1;
		}

		curFrame = 0;
		frames.resize(numFrames * 4);

		if (sprite->rects.size() == 0)
		{
			ResizeSpriteRect();
		}

		SetImage(sprite->texName.c_str());

		SelectRect();

		if (sprite->texture)
		{
			int index = 0;

			if (sprite->type == Sprite::Type::Frames)
			{
				for (int i = 0; i<sprite->rects.size(); i++)
				{
					Sprite::Frame& rect = sprite->rects[i];

					frames[i * 4 + 0] = rect.pos;
					frames[i * 4 + 1] = Math::Vector2(rect.pos.x + rect.size.x, rect.pos.y);
					frames[i * 4 + 2] = Math::Vector2(rect.pos.x, rect.pos.y - rect.size.y);
					frames[i * 4 + 3] = Math::Vector2(rect.pos.x + rect.size.x, rect.pos.y - rect.size.y);
				}
			}

			for (int i = 0; i < rectHeight - 1; i++)
				for (int j = 0; j < rectWidth - 1; j++)
				{
					int indx = rectWidth * i + j;
					Sprite::Frame& rect = sprite->rects[index];

					points[indx] = rect.pos;

					if (j == rectWidth - 2)
					{
						points[indx + 1] = Math::Vector2(rect.pos.x + rect.size.x, rect.pos.y);
					}

					if (i == rectHeight - 2)
					{
						points[indx + rectWidth] = Math::Vector2(rect.pos.x, rect.pos.y - rect.size.y);
					}

					if (j == rectWidth - 2 && i == rectHeight - 2)
					{
						points[indx + rectWidth + 1] = Math::Vector2(rect.pos.x + rect.size.x, rect.pos.y - rect.size.y);
					}

					index++;
				}

			UpdateSavedPos();
		}

		FitImage();
	}

	void SpriteWindow::SetCurFrame(int frame)
	{
		curFrame = frame;

		for (int j = 0; j < 4; j++)
		{
			points[j] = frames[curFrame * 4 + j];
		}

		//cur_frame_ebox->SetText(cur_frame);
		//cur_frame_time_ebox->SetText(sprite->rects[cur_frame].frame_time);
		//pivot_x_ebox->SetText((int)sprite->rects[cur_frame].offset.x);
		//pivot_y_ebox->SetText((int)sprite->rects[cur_frame].offset.y);
		UpdateSavedPos();
	}

	void SpriteWindow::SelectRect()
	{
		rectWidth = 2;
		rectHeight = 2;

		if (sprite->type == Sprite::Type::ThreeSlicesVert)
		{
			rectHeight = 4;
		}
		else
		if (sprite->type == Sprite::Type::ThreeSlicesHorz)
		{
			rectWidth = 4;
		}
		else
		if (sprite->type == Sprite::Type::NineSlices)
		{
			rectWidth = 4;
			rectHeight = 4;
		}
	}

	void SpriteWindow::FillRects()
	{
		switch (sprite->type)
		{
			case Sprite::Type::Image:
			case Sprite::Type::Frames:
			{
				points[0] = Math::Vector2(spritePos.x, spritePos.y);
				points[1] = Math::Vector2(spritePos.x + spriteSize.x, spritePos.y);
				points[2] = Math::Vector2(spritePos.x, spritePos.y - spriteSize.y);
				points[3] = Math::Vector2(spritePos.x + spriteSize.x, spritePos.y - spriteSize.y);

				break;
			}
			case Sprite::Type::ThreeSlicesVert:
			{
				FillPoints(0, 2, spritePos.x, true);
				FillPoints(1, 2, (float)spritePos.x + spriteSize.x, true);

				break;
			}
			case Sprite::Type::ThreeSlicesHorz:
			{
				FillPoints(0, 4, spritePos.y, false);
				FillPoints(4, 4, spritePos.y - spriteSize.y, false);

				break;
			}
			case Sprite::Type::NineSlices:
			{
				FillPoints(0, 4, spritePos.x, true);
				FillPoints(1, 4, spritePos.x + spriteOffsetX.x, true);
				FillPoints(2, 4, spritePos.x + spriteSize.x - spriteOffsetY.y, true);
				FillPoints(3, 4, spritePos.x + spriteSize.x, true);

				break;
			}
		}
	}

	void SpriteWindow::ActualPixels()
	{
		camZoom = 1.0f;
		camPos = 0.0f;
		deltaMouse = 0.0f;
	}

	void SpriteWindow::FitImage()
	{
		camZoom = lastViewportSize.y / sprite->height;

		camPos = 0.0f;

		if ((float)sprite->width * camZoom > lastViewportSize.x)
		{
			camZoom = lastViewportSize.x / sprite->width;
		}

		deltaMouse = 0.0f;
	}

	void SpriteWindow::UpdateAnimRect()
	{
		for (int j = 0; j < 4; j++)
		{
			frames[curFrame * 4 + j] = points[j];
		}
	}

	void SpriteWindow::UpdateSavedPos()
	{
		spritePos = points[0];
		spriteSize = points[rectHeight * rectWidth - 1] - points[0];
		spriteSize.y = -spriteSize.y;

		if (rectWidth > 2)
		{
			spriteOffsetX.x = points[1].x - points[0].x;
			spriteOffsetX.y = points[3].x - points[2].x;
		}

		if (rectHeight > 2)
		{
			spriteOffsetY.x = points[0].y - points[rectWidth].y;
			spriteOffsetY.y = points[2 * rectWidth].y - points[3 * rectWidth].y;
		}

		//prop_x_ebox->SetText((int)spritePos.x);
		//prop_y_ebox->SetText((int)(sprite->height - sprite_pos.y));

		//prop_w_ebox->SetText((int)sprite_size.x);
		//prop_h_ebox->SetText((int)sprite_size.y);
	}

	void SpriteWindow::ImGui()
	{
		if (!opened)
		{
			sprite = nullptr;
			return;
		}

		ImGui::Begin("Sprite Editor", &opened, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus);

		if (needSetSize)
		{
			ImGui::SetWindowSize(ImVec2(800.0f, 600.0f));
			needSetSize = false;
		}

		ImGuiIO& io = ImGui::GetIO();

		ImVec2 size = ImGui::GetContentRegionAvail();

		DrawViewport(Math::Vector2(size.x, size.y));

		ImGui::Image(Oak::root.render.GetDevice()->GetBackBuffer(), size);

		ImVec2 viewportPos = ImVec2(io.MousePos.x - ImGui::GetCursorScreenPos().x, io.MousePos.y - ImGui::GetCursorScreenPos().y);

		camZoom = Math::Clamp(camZoom + io.MouseWheel * 0.5f, 0.4f, 3.0f);

		vireportHowered = ImGui::IsItemHovered();

		if (vireportHowered && ImGui::IsMouseDown(0))
		{
			OnLeftMouseDown();
			viewportCaptured = false;
		}
		else
		if (vireportHowered && ImGui::IsMouseDown(2))
		{
			OnMiddleMouseDown();
			viewportCaptured = true;
		}

		OnMouseMove(Math::Vector2((float)viewportPos.x, (float)viewportPos.y));

		if (viewportCaptured && ImGui::IsMouseReleased(0))
		{
			OnLeftMouseUp();
			viewportCaptured = false;
		}
		else
		if (viewportCaptured && ImGui::IsMouseReleased(2))
		{
			OnMiddleMouseUp();
			viewportCaptured = false;
		}

		ImGui::End();
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
		Sprite::FrameState state;
	
		Math::Matrix spriteTrans;
		spriteTrans.Pos().z = 0.01f;

		Sprite::Draw(editorDrawer.checkerTex, COLOR_WHITE, spriteTrans,
					Math::Vector2(camPos.x - viewportSize.x * 0.5f / camZoom, camPos.y - viewportSize.y * 0.5f / camZoom) , viewportSize / camZoom,
					Math::Vector2(((int)(camPos.x) % 42) / 42.0f, 1.0f - ((int)(camPos.y) % 42) / 42.0f),
					Math::Vector2(viewportSize.x / camZoom / 42.0f, viewportSize.y / camZoom / 42.0f), false);

		float wd = sprite->texture ? sprite->width * 1.1f : 512.0f;
		float ht = sprite->texture ? sprite->height * 1.1f : 512.0f;

		Color color = COLOR_WHITE;
		//root.render.DebugLine2D(Math::Vector2(pixel_density, (-ht) * pixel_density), color, Math::Vector2(pixel_density, (ht) * pixel_density), color);
		//root.render.DebugLine2D(Math::Vector2((wd) * pixel_density, pixel_density), color, Math::Vector2(( wd) * pixel_density, pixel_density), color);

		float curWidth = (float)sprite->width;
		float curHeigt = (float)sprite->height;

		spriteTrans.Pos().z = 0.005f;
		Sprite::Draw(nullptr, COLOR_WHITE_A(0.5f), spriteTrans, Math::Vector2(-curWidth * 0.5f, -curHeigt * 0.5f), Math::Vector2((float)curWidth, (float)curHeigt), 0.0f, 1.0f, false);

		spriteTrans.Pos().z = 0.0f;
		Sprite::Draw(sprite->texture, COLOR_WHITE, spriteTrans, Math::Vector2(-curWidth * 0.5f, -curHeigt * 0.5f), Math::Vector2((float)curWidth, (float)curHeigt), 0.0f, 1.0f, false);

		if (sprite->type == Sprite::Type::Frames)
		{
			color = COLOR_WHITE;

			for (int i = 0; i< numFrames; i++)
			{
				root.render.DebugLine2D(Math::Vector2((frames[i * 4 + 0].x), (-frames[i * 4 + 0].y)), color,
										Math::Vector2((frames[i * 4 + 1].x), (-frames[i * 4 + 1].y)), color);

				root.render.DebugLine2D(Math::Vector2((frames[i * 4 + 1].x), (-frames[i * 4 + 1].y)), color,
										Math::Vector2((frames[i * 4 + 3].x), (-frames[i * 4 + 3].y)), color);

				root.render.DebugLine2D(Math::Vector2((frames[i * 4 + 3].x), (-frames[i * 4 + 3].y)), color,
										Math::Vector2((frames[i * 4 + 2].x), (-frames[i * 4 + 2].y)), color);

				root.render.DebugLine2D(Math::Vector2((frames[i * 4 + 2].x), (-frames[i * 4 + 2].y)), color,
										Math::Vector2((frames[i * 4 + 0].x), (-frames[i * 4 + 0].y)), color);
			}

			color = COLOR_GREEN;

			for (int i = 0; i< numFrames - 1; i++)
			{
				root.render.DebugLine2D(Math::Vector2((frames[i * 4 + 0].x + (frames[i * 4 + 1].x - frames[i * 4 + 0].x) * 0.5f),
												(-frames[i * 4 + 1].y - (frames[i * 4 + 2].y - frames[i * 4 + 1].y) * 0.5f)), color,
												Math::Vector2((frames[(i + 1) * 4 + 0].x + (frames[(i + 1) * 4 + 1].x - frames[(i + 1) * 4 + 0].x) * 0.5f),
												(-frames[(i + 1) * 4 + 1].y - (frames[(i + 1) * 4 + 2].y - frames[(i + 1) * 4 + 1].y) * 0.5f)), color);
			}
		}

		for (int i = 0; i < rectHeight; i++)
			for (int j = 0; j<rectWidth; j++)
			{
				int index = rectWidth * i + j;

				color = COLOR_WHITE;

				if (j < rectWidth - 1 && i < rectHeight - 1)
				{
					root.render.DebugLine2D(Math::Vector2((points[index + rectWidth].x), (-points[index + rectWidth].y)), color,
											Math::Vector2((points[index].x), (-points[index].y)), color);

					root.render.DebugLine2D(Math::Vector2((points[index].x), (-points[index].y)), color,
											Math::Vector2((points[index + 1].x), (-points[index + 1].y)), color);

					if (i == rectHeight - 2 || j == rectWidth - 2)
					{
						root.render.DebugLine2D(Math::Vector2((points[index + 1].x), (-points[index + 1].y)), color,
												Math::Vector2((points[index + rectWidth + 1].x), (-points[index + rectWidth + 1].y)), color);

						root.render.DebugLine2D(Math::Vector2((points[index + rectWidth + 1].x), (-points[index + rectWidth + 1].y)), color,
												Math::Vector2((points[index + rectWidth].x), (-points[index + rectWidth].y)), color);
					}
				}

				if (selRow == i && selCol == j)
				{
					color.Set(1.0, 0.9f, 0.0f, 1.0f);
				}

				root.render.DebugSprite(editorDrawer.anchornTex, Math::Vector2((points[index].x) - 4, (-points[index].y) - 4), Math::Vector2(8.0f), color);
			}

		if (sprite->type == Sprite::Type::Frames)
		{
			color.Set(1.0, 0.9f, 0.0f, 1.0f);

			float pivot_x = (frames[curFrame * 4 + 0].x + frames[curFrame * 4 + 1].x) * 0.5f;
			float pivot_y = frames[curFrame * 4 + 2].y;

			root.render.DebugLine2D(Math::Vector2((pivot_x), (-pivot_y)), color,
									Math::Vector2((pivot_x), (-pivot_y + sprite->rects[curFrame].offset.y)), color);

			root.render.DebugLine2D(Math::Vector2((pivot_x - sprite->rects[curFrame].offset.x), (-pivot_y + sprite->rects[curFrame].offset.y)), color,
									Math::Vector2((pivot_x), (-pivot_y + sprite->rects[curFrame].offset.y)), color);

			root.render.DebugSprite(editorDrawer.centerTex, Math::Vector2((pivot_x) - 4, (-pivot_y) - 4), Math::Vector2(8.0f), color);
			root.render.DebugSprite(editorDrawer.centerTex, Math::Vector2((pivot_x - sprite->rects[curFrame].offset.x) - 4, (-pivot_y + sprite->rects[curFrame].offset.y) - 4), Math::Vector2(8.0f), color);
		}

		if (ImGui::IsWindowFocused())
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

	void SpriteWindow::OnMouseMove(Math::Vector2 ms)
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

		if (drag == DragField)
		{
			camPos += Math::Vector2(delta.x, -delta.y) * camZoom;

			camPos.x = Math::Clamp(camPos.x, -sprite->width * 0.5f, sprite->width * 0.5f);
			camPos.y = Math::Clamp(camPos.y, -sprite->height * 0.5f, sprite->height * 0.5f);
		}
		else
		if (drag == DragRects)
		{
			MoveRects(delta);
		}
		else
		{
			Math::Vector2 ps(ms.x, -ms.y);

			selCol = -1;
			selRow = -1;

			for (int i = 0; i < rectHeight; i++)
				for (int j = 0; j < rectWidth; j++)
				{
					Math::Vector2 point = points[rectWidth * i + j];

					if (point.x - 7 < ps.x && ps.x < point.x + 7 &&
						point.y - 7 < ps.y && ps.y < point.y + 7)
					{
						selRow = i;
						selCol = j;
					}
				}
		}

		prevMs = ms;
	}


	void SpriteWindow::OnLeftMouseDown()
	{
		Math::Vector2 ps(prevMs.x, -prevMs.y);

		if (sprite->type == Sprite::Type::Frames)
		{
			Math::Vector2 point = frames[curFrame * 4];
			Math::Vector2 point2 = frames[curFrame * 4 + 3];

			int sel_frame = -1;

			if (point.x < ps.x && ps.x < point2.x &&
				point2.y < ps.y && ps.y < point.y)
			{
				sel_frame = curFrame;
			}

			if (sel_frame == -1)
			{
				for (int i = 0; i < numFrames; i++)
				{
					Math::Vector2 point = frames[i * 4];
					Math::Vector2 point2 = frames[i * 4 + 3];

					if (point.x < ps.x && ps.x < point2.x &&
						point2.y < ps.y && ps.y < point.y)
					{
						sel_frame = i;
					}
				}

				if (sel_frame != -1)
				{
					SetCurFrame(sel_frame);
				}
			}
		}

		drag = DragRects;
	}

	void SpriteWindow::OnLeftMouseUp()
	{
		/*if (sender == load_image)
		{
			const char* file_name = EUI::OpenOpenDialog(wnd->GetNative(), "Image files", "*");

			if (file_name)
			{
				string name;
				core.files.MakePathRelative(name, file_name);

				SetImage(name.c_str());
				FitImage();
			}
		}

		if (sender == del_image)
		{
			SetImage("");
		}

		if (sender == btn_zoom_in)
		{
			MakeZoom(0.5f);
		}

		if (sender == btn_zoom_out)
		{
			if (pixel_density > 1.0f)
			{
				MakeZoom(-0.5f);
			}
		}

		if (sender == btn_act_pixels)
		{
			ActualPixels();
		}

		if (sender == btn_fit)
		{
			FitImage();
		}

		if (sender == btn_del_frame && num_frames > 1)
		{
			for (int i = cur_frame + 1; i < num_frames; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					frames[(i - 1) * 4 + j] = frames[i * 4 + j];
				}
			}

			num_frames--;
			frames.resize(num_frames * 4);
			ResizeSpriteRect();

			SetCurFrame((int)fmin(cur_frame, num_frames - 1));
			num_frame_ebox->SetText(num_frames);

			UpdateSpriteRect();
		}

		if (sender == show_anim_box)
		{
			show_anim = !show_anim;
		}*/

		//if (sender == img_wgt)
		{
			selCol = -1;
			selRow = -1;
			drag = DragNone;
		}
	}

	void SpriteWindow::OnMiddleMouseDown()
	{
		drag = DragField;
	}

	void SpriteWindow::OnMiddleMouseUp()
	{
		drag = DragNone;
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

		UpdateSavedPos();
		UpdateAnimRect();
		UpdateSpriteRect();
	}

	/*void SpriteWindow::SetColorToLabel()
	{
		int clr[3];
		clr[0] = (int)(sprite->color.r * 255.0f);
		clr[1] = (int)(sprite->color.g * 255.0f);
		clr[2] = (int)(sprite->color.b * 255.0f);

		prop_color_edlabel->SetBackgroundColor(true, clr);
	}

	void SpriteWindow::OnComboBoxSelChange(EUIComboBox* sender, int index)
	{
		if (sender == texture_mode)
		{
			sprite->mode = (Texture::TextureAddress)texture_mode->GetCurStringIndex();
		}

		if (sender == texture_filter)
		{
			sprite->filter = (Texture::FilterType)texture_filter->GetCurStringIndex();
		}

		if (sender == cb_type)
		{
			sprite->type = (Sprite::Type)cb_type->GetCurStringIndex();
			ShowFrameWidgets();
			SelectRect();
			ResizeSpriteRect();
			FillRects();
			UpdateAnimRect();
			UpdateSpriteRect();
		}
	}

	void SpriteWindow::OnEditBoxStopEditing(EUIEditBox* sender)
	{
		float delta_x = 0.0f;
		float delta_y = 0.0f;

		if (sender == prop_x_ebox)
		{
			delta_x = sprite_pos.x - prop_x_ebox->GetAsInt();
		}

		if (sender == prop_y_ebox)
		{
			delta_y = (sprite->height - prop_y_ebox->GetAsInt()) - sprite_pos.y;
		}

		if (fabs(delta_x) > 0.0f || fabs(delta_y) > 0.0f)
		{
			for (int i = 0; i < rect_height; i++)
				for (int j = 0; j < rect_width; j++)
				{
					points[rect_width * i + j] -= Vector2(delta_x, -delta_y);
				}

			UpdateSavedPos();
			UpdateAnimRect();
			UpdateSpriteRect();
		}

		if (sender == prop_w_ebox || sender == prop_h_ebox)
		{
			if (sender == prop_w_ebox)
			{
				sprite_size.x = prop_w_ebox->GetAsFloat();
			}

			if (sender == prop_h_ebox)
			{
				sprite_size.y = prop_h_ebox->GetAsFloat();
			}

			FillRects();
			UpdateAnimRect();
			UpdateSpriteRect();
		}

		if (sender == cur_frame_ebox)
		{
			SetCurFrame((int)fmin(atoi(cur_frame_ebox->GetText()), num_frames - 1));
		}

		if (sender == num_frame_ebox)
		{
			int prev_num_frames = num_frames;
			num_frames = (int)fmax(atoi(num_frame_ebox->GetText()), 1);
			num_frame_ebox->SetText(num_frames);

			frames.resize(num_frames * 4);
			ResizeSpriteRect();

			if (prev_num_frames < num_frames)
			{
				for (int i = prev_num_frames + 1; i < num_frames; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						frames[i * 4 + j] = frames[(prev_num_frames - 1) * 4 + j];
					}
				}
			}

			if (cur_frame >= num_frames)
			{
				SetCurFrame(num_frames - 1);
			}

			UpdateSpriteRect();
		}

		if (sender == frame_time_ebox)
		{
			sprite->frame_time = fmax(0.025f, frame_time_ebox->GetAsFloat());
		}

		if (sender == cur_frame_time_ebox)
		{
			sprite->rects[cur_frame].frame_time = cur_frame_time_ebox->GetAsFloat();
		}

		if (sender == pivot_x_ebox)
		{
			sprite->rects[cur_frame].offset.x = pivot_x_ebox->GetAsFloat();
		}

		if (sender == pivot_y_ebox)
		{
			sprite->rects[cur_frame].offset.y = pivot_y_ebox->GetAsFloat();
		}
	}

	void SpriteWindow::OnLeftDoubliClick(EUIWidget* sender, int mx, int my)
	{
		if (sender == prop_color_edlabel && EUI::OpenColorDialog(wnd->GetNative(), &sprite->color.r))
		{
			SetColorToLabel();
		}
	}

	void SpriteWindow::OnMouseWheel(EUIWidget* sender, int delta)
	{
		if (sender == img_wgt)
		{
			MakeZoom(-(float)delta * 0.025f);
		}
	}

	void SpriteWindow::OnKey(EUIWidget* sender, int key)
	{
		Vector2 delta = 0.0f;

		if (!border_drawed)
		{
			return;
		}

		switch (key)
		{
			case VK_UP : delta.y = 0.5f; break;
			case VK_DOWN: delta.y = -0.5f; break;
			case VK_LEFT: delta.x = 0.5f; break;
			case VK_RIGHT: delta.x = -0.5f; break;
			case 'I':
			{
				if (cur_frame != -1 && num_frames > 1)
				{
					num_frames--;
					num_frame_ebox->SetText(num_frames);
					frames.erase(frames.begin() + cur_frame * 4, frames.begin() + (cur_frame + 1) * 4);
					ResizeSpriteRect();

					if (cur_frame >= num_frames)
					{
						cur_frame = num_frames - 1;
					}

					SetCurFrame(cur_frame);

					UpdateSpriteRect();
				}
				break;
			}
			case 'O':
			{
				if (cur_frame != -1)
				{
					num_frames++;
					num_frame_ebox->SetText(num_frames);
					frames.insert(frames.begin() + (cur_frame) * 4, 4, Vector2());
					ResizeSpriteRect();

					for (int j = 0; j < 4; j++)
					{
						frames[cur_frame * 4 + j] = frames[(cur_frame + 1) * 4 + j];
					}

					SetCurFrame(cur_frame);

					UpdateSpriteRect();
				}

				break;
			}
			case 'P':
			{
				if (cur_frame != -1)
				{
					num_frames++;
					num_frame_ebox->SetText(num_frames);
					frames.insert(frames.begin() + (cur_frame + 1) * 4, 4, Vector2());
					ResizeSpriteRect();

					for (int j = 0; j < 4; j++)
					{
						frames[(cur_frame + 1) * 4 + j] = frames[cur_frame * 4 + j];
					}

					SetCurFrame(cur_frame + 1);

					UpdateSpriteRect();
				}

				break;
			}
		}

		if (fabs(delta.x) > 0.0f || fabs(delta.y) > 0.0f)
		{
			MoveRects(delta);
		}
	}*/
}

#endif
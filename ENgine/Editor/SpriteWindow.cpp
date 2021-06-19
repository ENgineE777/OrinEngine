
#ifdef OAK_EDITOR

#include "SpriteWindow.h"
#include "Root/Root.h"

#include "Editor/EditorDrawer.h"

#include "imgui.h"

#include "stb_image.h"

#include "imgui_internal.h"

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
		selSlice = -1;
		selAnim = (texture && texture->animations.size() > 0) ? 0 : -1;

		curAnimPlaySlice = -1;

		selAnimSlice = -1;
		animSliceToPaste = -1;

		textureRef = AssetTextureRef(texture, _FL_);

		camZoom = lastViewportSize.y / texture->size.y;

		camPos = texture->size * 0.5f;

		if ((float)texture->size.x * camZoom > lastViewportSize.x)
		{
			camZoom = lastViewportSize.x / texture->size.x;
		}

		deltaMouse = 0.0f;
	}

	void SpriteWindow::FillPoints(int index, int stride, float val)
	{
		auto& slice = texture->slices[selSlice];

		points[index + stride * 0] = Math::Vector2(val, texture->size.y - slice.pos.y);
		points[index + stride * 1] = Math::Vector2(val, texture->size.y - slice.pos.y - slice.upLeftOffset.y);
		points[index + stride * 2] = Math::Vector2(val, texture->size.y - slice.pos.y - slice.size.y + slice.downRightOffset.y);
		points[index + stride * 3] = Math::Vector2(val, texture->size.y - slice.pos.y - slice.size.y);
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

			points[0] = Math::Vector2(slice.pos.x, texture->size.y - slice.pos.y);
			points[1] = Math::Vector2(slice.pos.x + slice.size.x, texture->size.y - slice.pos.y);
			points[2] = Math::Vector2(slice.pos.x, texture->size.y - slice.pos.y - slice.size.y);
			points[3] = Math::Vector2(slice.pos.x + slice.size.x, texture->size.y - slice.pos.y - slice.size.y);
		}
	}

	void SpriteWindow::UpdateSlice()
	{
		auto& slice = texture->slices[selSlice];

		slice.pos = Math::Vector2(points[0].x, texture->size.y - points[0].y);
		slice.pos.x = floorf(slice.pos.x);
		slice.pos.y = floorf(slice.pos.y);

		slice.size = points[rectHeight * rectWidth - 1] - points[0];
		slice.size.x = floorf(slice.size.x);
		slice.size.y = -floorf(slice.size.y);

		if (rectWidth > 2)
		{
			slice.upLeftOffset.x = points[1].x - points[0].x;
			slice.downRightOffset.x = points[3].x - points[2].x;

			slice.upLeftOffset.x = floorf(slice.upLeftOffset.x);
			slice.upLeftOffset.x = floorf(slice.upLeftOffset.x);
		}

		if (rectHeight > 2)
		{
			slice.upLeftOffset.y = points[0].y - points[rectWidth].y;
			slice.downRightOffset.y = points[2 * rectWidth].y - points[3 * rectWidth].y;

			slice.upLeftOffset.y = floorf(slice.upLeftOffset.y);
			slice.upLeftOffset.y = floorf(slice.upLeftOffset.y);
		}

		texture->SaveMetaData();
	}

	void SpriteWindow::Text(const char* name)
	{
		ImGui::Text(name);
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(labelSize - ImGui::CalcTextSize(name).x, 1.0f));
		ImGui::SameLine();
	}

	bool SpriteWindow::InputFloat(float* value, const char* name)
	{
		Text(name);

		ImGui::SetNextItemWidth(inputSize);
		return ImGui::InputFloat(StringUtils::PrintTemp("###Slice%s", name), value);
	}

	bool SpriteWindow::InputInt(int* value, const char* name, bool needClamp)
	{
		Text(name);

		ImGui::SetNextItemWidth(inputSize);
		if (ImGui::InputInt(StringUtils::PrintTemp("###Slice%s", name), value))
		{
			if (needClamp && *value < 1) *value = 1;
			return true;
		}

		return false;
	}

	bool SpriteWindow::InputInt(float* value, const char* name, bool needClamp)
	{
		Text(name);

		int intValue = (int)(*value);

		ImGui::SetNextItemWidth(inputSize);
		if (ImGui::InputInt(StringUtils::PrintTemp("###Slice%s", name), &intValue))
		{
			if (needClamp && intValue < 1) intValue = 1;
			*value = (float)intValue;

			return true;
		}

		return false;
	}

	bool SpriteWindow::InputString(eastl::string& value, const char* name)
	{
		Text(name);

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

		ImGui::SetNextItemWidth(inputSize);
		return ImGui::InputText("###SliceName", value.begin(), (size_t)value.size() + 1, ImGuiInputTextFlags_CallbackResize, Funcs::ResizeCallback, (void*)&value);
	}


	void SpriteWindow::AddToQueue(int posX, int posY, eastl::queue<Math::Vector2>& nodes, int width, int height, uint8_t* data, uint8_t* visited)
	{
		if (posY + 1 < height)
		{
			if (posX > 0 && !visited[(posY + 1) * width + posX - 1] && data[((posY + 1) * width + posX - 1) * 4 + 3] == 255)
			{
				posXMin = (int)fminf((float)posXMin, (float)posX - 1);
				posYMax = (int)fmaxf((float)posYMax, (float)posY + 1.0f);
				nodes.push(Math::Vector2((float)posX - 1, (float)posY + 1.0f));
			}

			if (!visited[(posY + 1) * width + posX] && data[((posY + 1) * width + posX) * 4 + 3] == 255)
			{
				posYMax = (int)fmaxf((float)posYMax, (float)posY + 1.0f);
				nodes.push(Math::Vector2((float)posX, (float)posY + 1.0f));
			}

			if (posX + 1 < width && !visited[(posY + 1) * width + posX + 1] && data[((posY + 1) * width + posX + 1) * 4 + 3] == 255)
			{
				posXMax = (int)fmaxf((float)posXMax, (float)posX + 1);
				posYMax = (int)fmaxf((float)posYMax, (float)posY + 1.0f);
				nodes.push(Math::Vector2((float)posX + 1, (float)posY + 1.0f));
			}
		}

		if (posY - 1 >= 0)
		{
			if (posX > 0 && !visited[(posY - 1) * width + posX - 1] && data[((posY - 1) * width + posX - 1) * 4 + 3] == 255)
			{
				posXMin = (int)fminf((float)posXMin, (float)posX - 1);
				posYMin = (int)fminf((float)posYMin, (float)posY - 1.0f);
				nodes.push(Math::Vector2((float)posX - 1, (float)posY - 1.0f));
			}

			if (!visited[(posY - 1) * width + posX] && data[((posY - 1) * width + posX) * 4 + 3] == 255)
			{
				posYMin = (int)fminf((float)posYMin, (float)posY - 1.0f);
				nodes.push(Math::Vector2((float)posX, (float)posY - 1.0f));
			}

			if (posX + 1 < width && !visited[(posY - 1) * width + posX + 1] && data[((posY - 1) * width + posX + 1) * 4 + 3] == 255)
			{
				posXMax = (int)fmaxf((float)posXMax, (float)posX + 1);
				posYMin = (int)fminf((float)posYMin, (float)posY - 1.0f);
				nodes.push(Math::Vector2((float)posX + 1, (float)posY - 1.0f));
			}
		}
	}

	void SpriteWindow::TextureCrawler(int posX, int posY, int width, int height, uint8_t* data, uint8_t* visited)
	{
		eastl::queue<Math::Vector2> nodes;
		nodes.push(Math::Vector2((float)posX, (float)posY));

		while (nodes.size())
		{
			auto current = nodes.front();
			nodes.pop();

			for (int i = (int)current.x; i < width; i++)
			{
				if (visited[(int)current.y * width + i] || data[((int)current.y * width + i) * 4 + 3] != 255)
				{
					break;
				}

				visited[(int)current.y * width + i] = 1;

				posXMax = (int)fmaxf((float)posXMax, (float)i);

				AddToQueue(i, (int)current.y, nodes, width, height, data, visited);
			}

			for (int i = (int)current.x - 1; i >= 0; i--)
			{
				if (visited[(int)current.y * width + i] || data[((int)current.y * width + i) * 4 + 3] != 255)
				{
					break;
				}

				posXMin = (int)fminf((float)posXMin, (float)i);

				AddToQueue(i, (int)current.y, nodes, width, height, data, visited);
			}
		}
	}

	void SpriteWindow::ShowImageInfo()
	{
		ImGui::Begin("Image Info", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImGui::Text(StringUtils::PrintTemp("Image name: %s", texture->GetName()));
		ImGui::Text(StringUtils::PrintTemp("Image Size: %i x %i", (int)texture->size.x, (int)texture->size.y));

		ImGui::End();
	}

	void SpriteWindow::ShowAutoSlicing()
	{
		ImGui::Begin("Auto slicing");

		const char* types = "Set cells count\0Set cells size\0Auto\0";
		Text("Slicing Type");
		ImGui::SetNextItemWidth(inputSize);
		ImGui::Combo("###SlicingType", &typeAutoSlice, types);

		if (typeAutoSlice == 0)
		{
			InputInt(&AutoSliceCols, "Num Cols", true);
			InputInt(&AutoSliceRows, "Num Rows", true);
		}

		if (typeAutoSlice == 1)
		{
			InputInt(&AutoSliceCellSizeX, "CellX", true);
			InputInt(&AutoSliceCellSizeY, "CellY", true);
		}

		if (typeAutoSlice == 2)
		{
			InputInt(&AutoSliceMinSizeX, "MinSizeX", true);
			InputInt(&AutoSliceMinSizeY, "MinSizeY", true);
		}

		if (ImGui::Button("Slice###SliceSlice"))
		{
			texture->slices.clear();
			selSlice = -1;

			float stepX = (typeAutoSlice == 0) ? (texture->size.x / AutoSliceRows) : AutoSliceCellSizeX;
			float stepY = (typeAutoSlice == 0) ? (texture->size.y / AutoSliceCols) : AutoSliceCellSizeY;

			if (typeAutoSlice == 0 || typeAutoSlice == 1)
			{
				float posY = 0;

				int index = 0;

				while (posY < texture->size.y)
				{
					float posX = 0;

					while (posX < texture->size.x)
					{
						AssetTexture::Slice slice;
						slice.pos = Math::Vector2(posX, posY);
						slice.size = Math::Vector2(stepX, stepY);

						slice.name = StringUtils::PrintTemp("Slice%i", index);

						texture->slices.push_back(slice);

						index++;

						posX += stepX;
					}

					posY += stepY;
				}
			}
			else
			if (typeAutoSlice == 2)
			{
				FileInMemory buffer;

				if (buffer.Load(texture->GetPath().c_str()))
				{
					uint8_t* ptr = buffer.GetData();

					int bytes;
					int width;
					int height;
					uint8_t* data = stbi_load_from_memory(ptr, buffer.GetSize(), &width, &height, &bytes, STBI_rgb_alpha);

					uint8_t* visited = (uint8_t*)malloc(width * height);
					memset(visited, 0, width * height);

					int index = 0;

					for (int j = 0; j < height; j++)
					{
						for (int i = 0; i < width; i++)
						{
							if (!visited[(j * width + i)] && data[(j * width + i) * 4 + 3] == 255)
							{
								posXMin = posXMax = i;
								posYMin = posYMax = j;

								TextureCrawler(i, j, width, height, data, visited);

								if (posXMax - posXMin + 1 >= AutoSliceMinSizeX && posYMax - posYMin + 1 >= AutoSliceMinSizeY)
								{
									AssetTexture::Slice slice;
									slice.pos = Math::Vector2((float)posXMin, (float)posYMin);
									slice.size = Math::Vector2((float)posXMax - (float)posXMin + 1, (float)posYMax - (float)posYMin + 1);

									slice.name = StringUtils::PrintTemp("Slice%i", index);

									texture->slices.push_back(slice);

									index++;
								}
							}
						}
					}

					free(data);
					free(visited);

					texture->SaveMetaData();
				}
			}
		}

		ImGui::End();
	}

	void SpriteWindow::ShowSlices()
	{
		ImGui::Begin("Slices");

		if (selSlice != -1)
		{
			auto& slice = texture->slices[selSlice];

			bool changed = false;

			if (InputString(slice.name, "Name")) changed = true;

			if (InputInt(&slice.pos.x, "X", false)) changed = true;
			if (InputInt(&slice.pos.y, "Y", false)) changed = true;

			if (InputInt(&slice.size.x, "Width", true)) changed = true;
			if (InputInt(&slice.size.y, "Heigth", true)) changed = true;

			Text("Is 9-slice");

			if (ImGui::Checkbox("###Is9Slice", &slice.isNineSliced)) changed = true;

			if (changed)
			{
				FillRects();
				texture->SaveMetaData();
			}
		}

		ImGui::End();
	}

	void SpriteWindow::ShowAnimations()
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("Animations");

		ImGui::BeginChild("AnimationsList", ImVec2(120, 0), true);

		ImGui::SetNextItemWidth(300);

		bool changed = false;

		if (ImGui::Button("Add###AnimAdd"))
		{
			auto& anim = texture->animations.push_back();
			anim.name = "Anim";

			selAnim = (int)texture->animations.size() - 1;
			selAnimSlice = -1;
			animSliceToPaste = -1;

			changed = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Del###AnimDel"))
		{
			if (selAnim != -1)
			{
				texture->animations.erase(texture->animations.begin() + selAnim);
				selAnim = -1;
				selAnimSlice = -1;
				animSliceToPaste = -1;

				changed = true;
			}
		}

		ImGui::BeginChild("AnimationsListItems", ImVec2(0, 0), true);

		for (int i = 0; i < texture->animations.size(); i++)
		{
			auto& anim = texture->animations[i];

			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			if (i == selAnim)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			ImGui::TreeNodeEx(&anim, node_flags, anim.name.c_str());

			if (ImGui::IsItemClicked())
			{
				selAnim = i;
				selAnimSlice = -1;
				animSliceToPaste = -1;
			}
		}

		ImGui::EndChild();

		ImGui::EndChild();

		ImGui::SameLine();

		if (selAnim != -1)
		{
			ImGui::BeginChild("AnimationSelAnim", ImVec2(200, 0), true);

			auto& anim = texture->animations[selAnim];

			if (InputString(anim.name, "Name")) changed = true;
			if (InputInt(&anim.fps, "FPS", true)) changed = true;

			if (anim.frames.size() > 0)
			{
				if (curAnimPlaySlice == -1)
				{
					curAnimPlaySlice = 0;
				}

				if (anim.frames.size() > 1)
				{
					curAnimPlayTime += root.GetDeltaTime();

					int count = (int)(curAnimPlayTime * (float)anim.fps);
					curAnimPlayTime -= (float)count / (float)anim.fps;

					curAnimPlaySlice = (curAnimPlaySlice + count) % (int)anim.frames.size();
				}

				float sz = 180.0f;

				DrawImage(anim.frames[0].slice, anim.frames[curAnimPlaySlice].slice, sz, anim.frames[curAnimPlaySlice].offset, -1);

				ImGui::Image(nullptr, ImVec2(sz, sz), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					textureRef.sliceIndex = -1;
					textureRef.animIndex = selAnim;
					AssetTextureRef* ptr = &textureRef;
					ImGui::SetDragDropPayload("_ASSET_TEX", &ptr, sizeof(AssetTextureRef*));
					ImGui::EndDragDropSource();

					inDragAndDrop = true;
				}
			}
			else
			{
				curAnimPlaySlice = -1;
			}

			ImGui::EndChild();

			ImGui::SameLine();

			auto size = ImGui::GetContentRegionAvail();

			ImGui::BeginChild("AnimFrames", ImVec2(size.x - 200, 0), true);

			if (anim.frames.size() == 0)
			{
				Text("No farmes");
			}
			else
			{
				auto size = ImGui::GetContentRegionAvail();

				float pos = 0.0f;
				float sz = 60.0f;

				for (int i = 0; i < anim.frames.size(); i++)
				{
					DrawImage(anim.frames[0].slice, anim.frames[i].slice, sz, 0.0f, i);

					ImGui::Image(nullptr, ImVec2(sz, sz), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), selAnimSlice == i ? ImVec4(1, 1, 0, 1) : ImVec4(1, 1, 1, 1));

					if (ImGui::IsItemHovered() && (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
					{
						selAnimSlice = i;
						selSlice = anim.frames[selAnimSlice].slice;
						FillRects();
					}

					if (selAnimSlice == i && ImGui::BeginPopupContextItem("FrameContext"))
					{
						if (ImGui::MenuItem("Duplicate"))
						{
							anim.frames.insert(anim.frames.begin() + i + 1, anim.frames[i]);
							selAnimSlice = i + 1;
							animSliceToPaste = -1;
						}

						if (ImGui::MenuItem("Copy"))
						{
							animSliceToPaste = i;
						}

						if (animSliceToPaste != -1 && ImGui::MenuItem("Paste"))
						{
							anim.frames.insert(anim.frames.begin() + i + 1, anim.frames[animSliceToPaste]);
							selAnimSlice = i + 1;
						}

						if (ImGui::MenuItem("Delete"))
						{
							anim.frames.erase(anim.frames.begin() + selAnimSlice);

							if (selSlice == selAnimSlice)
							{
								selSlice = -1;
							}

							//changed = true;

							selAnimSlice = -1;
							animSliceToPaste = -1;
							i--;
						}

						ImGui::EndPopup();
					}

					pos += sz + 10.0f;

					if (pos > size.x - sz - 10.0f)
					{
						pos = 0.0f;
					}
					else
					{
						ImGui::SameLine();
					}
				}
			}

			ImGui::EndChild();

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ASSET_TEX", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

				if (payload)
				{
					AssetTextureRef* assetRef = reinterpret_cast<AssetTextureRef**>(payload->Data)[0];

					if (assetRef->sliceIndex != -1)
					{
						AssetTexture::Frame frame;
						frame.slice = assetRef->sliceIndex;

						anim.frames.push_back(frame);
						changed = true;
					}
				}
			}

			ImGui::SameLine();

			ImGui::BeginChild("SelAnimFrame", ImVec2(200, 0), true);

			if (selAnimSlice != -1)
			{
				auto& frame = anim.frames[selAnimSlice];
				if (InputFloat(&frame.frameLength, "Length")) changed = true;
				if (InputInt(&frame.offset.x, "OffsetX", false)) changed = true;
				if (InputInt(&frame.offset.y, "OffsetY", false)) changed = true;

				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				float sz = 180.0f;

				if (selAnimSlice > 0)
				{
					DrawImage(anim.frames[0].slice, anim.frames[selAnimSlice - 1].slice, sz, anim.frames[selAnimSlice - 1].offset, -1);
				}

				DrawImage(anim.frames[0].slice, frame.slice, sz, frame.offset, -1);
				ImGui::Image(nullptr, ImVec2(sz, sz), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
				
				if (ImGui::Button("Delete###SelAnimFrameDel", ImVec2(180, 0)))
				{
					anim.frames.erase(anim.frames.begin() + selAnimSlice);
					selAnimSlice = -1;
					animSliceToPaste = -1;
				}
			}

			ImGui::EndChild();
		}

		if (changed) texture->SaveMetaData();

		ImGui::End();
	}

	void SpriteWindow::ShowImage()
	{
		ImGui::Begin("Image");

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

		if (size.y < 0)	size.y = -size.y;

		DrawViewport(Math::Vector2(size.x, size.y));

		ImGui::Image(Oak::root.render.GetDevice()->GetBackBuffer(), size);

		if (root.controls.DebugKeyPressed("KEY_LCONTROL", AliasAction::Pressed, true) && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			textureRef.sliceIndex = selSlice;
			textureRef.animIndex = -1;
			AssetTextureRef* ptr = &textureRef;
			ImGui::SetDragDropPayload("_ASSET_TEX", &ptr, sizeof(AssetTextureRef*));
			ImGui::EndDragDropSource();

			inDragAndDrop = true;
		}

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

	void SpriteWindow::DrawImage(int sliceScaleIndex, int sliceIndex, float size, Math::Vector2 offset, int index)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		auto& sliceScale = texture->slices[sliceScaleIndex];
		float k = sliceScale.size.x / sliceScale.size.y;

		auto& slice = texture->slices[sliceIndex];
		Math::Vector2 uv = Math::Vector2(slice.pos.x, slice.pos.y) / texture->size;
		Math::Vector2 duv = slice.size / texture->size;

		ImVec2 p = ImGui::GetCursorScreenPos();
	
		ImVec2 sz = k > 1.0f ? ImVec2(size, size / k) : ImVec2(size * k, size);

		float scale = slice.size.x / sz.x;

		ImVec2 pos = ImVec2(p.x + (size - sz.x) * 0.5f + offset.x * scale, p.y + (size - sz.y) * 0.5f + offset.y * scale);

		drawList->AddImage(texture->GetTexture()->GetNativeResource(), pos, ImVec2(pos.x + sz.x, pos.y + sz.y), ImVec2(uv.x, uv.y), ImVec2(uv.x + duv.x, uv.y + duv.y));

		if (index != -1)
		{
			drawList->AddText(ImVec2(p.x + 4, p.y + 1), IM_COL32_WHITE, StringUtils::PrintTemp("%i", index));
		}
	}

	void SpriteWindow::ImGui()
	{
		if (!opened)
		{
			texture = nullptr;
			return;
		}

		ImGui::Begin("Sprite Editor", &opened, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

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
			ImGuiID dock_top_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.6f, nullptr, &dock_main_id);
			ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

			ImGui::DockBuilderDockWindow("Image Info", dock_bottom_id);
			ImGui::DockBuilderDockWindow("Auto slicing", dock_bottom_id);
			ImGui::DockBuilderDockWindow("Slices", dock_bottom_id);
			ImGui::DockBuilderDockWindow("Animations", dock_bottom_id);
			ImGui::DockBuilderDockWindow("Image", dock_top_id);

			ImGui::DockBuilderFinish(dock_main_id);

			ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_top_id);
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
		}

		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);

		ImGui::End();

		ShowImageInfo();
		ShowAutoSlicing();
		ShowSlices();
		ShowAnimations();
		ShowImage();
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

		float dist = (viewportSize.y * 0.5f * Sprite::pixelsPerUnitInvert) / (tanf(22.5f * Math::Radian) * camZoom);
		Math::Matrix view;
		view.BuildView(Math::Vector3(camPos.x * Sprite::pixelsPerUnitInvert, camPos.y * Sprite::pixelsPerUnitInvert, -dist), Math::Vector3(camPos.x * Sprite::pixelsPerUnitInvert, camPos.y * Sprite::pixelsPerUnitInvert, -dist + 1.0f), Math::Vector3(0, 1, 0));
		root.render.SetTransform(TransformStage::View, view);

		Math::Matrix proj;
		proj.BuildProjection(45.0f * Math::Radian, viewportSize.y / viewportSize.x, 1.0f, 1000.0f);
		root.render.SetTransform(TransformStage::Projection, proj);

		Transform trans;
	
		Math::Matrix spriteTrans;
		spriteTrans.Pos().z = 0.0f;

		Sprite::Draw(editorDrawer.checkerTex, COLOR_WHITE, spriteTrans,
					Math::Vector2(camPos.x - viewportSize.x * 0.5f / camZoom, camPos.y + viewportSize.y * 0.5f / camZoom), viewportSize / camZoom,
					Math::Vector2(((int)(camPos.x) % 42) / 42.0f, 1.0f - ((int)(camPos.y) % 42) / 42.0f),
					Math::Vector2(viewportSize.x / camZoom / 42.0f, viewportSize.y / camZoom / 42.0f));

		Color color = COLOR_WHITE;

		spriteTrans.Pos().y = texture->size.y;
		spriteTrans.Pos().z = -0.0025f;
		Sprite::Draw(nullptr, COLOR_GRAY_A(0.65f), spriteTrans, 0.0f, texture->size, 0.0f, 1.0f);

		spriteTrans.Pos().z = -0.005f;
		Sprite::Draw(texture->texture, COLOR_WHITE, spriteTrans, 0.0f, texture->size, 0.0f, 1.0f);

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

		int prevSelSlice = selSlice;
		selSlice = -1;

		float bufferZone = 3.0f;
		for(int i = 0; i < texture->slices.size(); i++)
		{
			auto& slice = texture->slices[i];

			if (slice.pos.x - bufferZone < rectStart.x && rectStart.x < slice.pos.x + slice.size.x + bufferZone &&
				texture->size.y - slice.pos.y - slice.size.y - bufferZone < rectStart.y && rectStart.y < texture->size.y - slice.pos.y + bufferZone)
			{
				selSlice = i;

				FillRects();
			}
		}

		if (selSlice != -1)
		{
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
		}
	}

	void SpriteWindow::OnLeftMouseUp()
	{
		if (drag == Drag::DragNewSlice)
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
			FillRects();
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
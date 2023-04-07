#include "Editor.h"

#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "EditorDrawer.h"
#include "SpriteWindow.h"
#include "TileSetWindow.h"
#include <filesystem>

#include "commdlg.h"

#include "shellapi.h"
#include "Support/ImGuiHelper.h"
#include "Support/Perforce.h"

#include <filesystem>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Orin
{
	Editor editor;

	IEditor* GetEditor()
	{
		return &editor;
	}

	void Editor::ToolButton::Execute()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		
		ImGui::PushID(name.c_str());

		if (icon == -1)
		{
			bool needPopStyle = false;

			if (state && state())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
				needPopStyle = true;
			}			

			if (ImGui::Button(name.c_str(), ImVec2(25.0f, 25.0f)))
			{
				callback();
			}

			if (needPopStyle)
			{
				ImGui::PopStyleColor(1);
			}
		}
		else
		{
			ImVec4 color = style.Colors[ImGuiCol_Button];

			if (state && state())
			{
				color = style.Colors[ImGuiCol_ButtonActive];
			}

			int y = (int)((float)icon / 10.0f);
			int x = icon - y * 10;
			float du = 24.0f / 240.0f;
			float dv = 24.0f / 72.0f;

			ImVec2 size = ImVec2(24.0f, 24.0f);
			ImVec2 uv0 = ImVec2(x * du, y * dv);
			ImVec2 uv1 = ImVec2((x + 1) * du, (y + 1) * dv);
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			if (ImGui::ImageButton(editorDrawer.iconsTex->GetNative(), size, uv0, uv1, 0, hovered ? style.Colors[ImGuiCol_ButtonHovered] : color, tint_col) ||
				root.controls.DebugKeyPressed(hotKey.c_str()))
			{
				callback();
			}
		}

		ImGui::PopID();

		hovered = ImGui::IsItemHovered();		

		ImGui::SameLine();		
	}

	CLASS_DECLSPEC IEditor* GetEditor();
	const char* Editor::OpenFileDialog(const char* extName, const char* ext, bool open)
	{
		char curDir[512];
		GetCurrentDirectoryA(512, curDir);

		char curDirDialog[512];
		StringUtils::Copy(curDirDialog, 512, curDir);

		OPENFILENAMEA ofn;

		static char fileName[512];

		char filter[512];
		StringUtils::Copy(filter, 512, extName);

		int index = (int)strlen(filter);

		filter[index + 1] = '*';
		filter[index + 2] = '.';

		if (ext)
		{
			StringUtils::Copy(&filter[index + 3], 5, ext);
		}
		else
		{
			StringUtils::Copy(&filter[index + 3], 5, "*");
		}

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = editor.hwnd;
		ofn.lpstrFile = fileName;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = 512;
		ofn.lpstrFilter = filter;
		ofn.lpstrDefExt = ext;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = curDirDialog;
		ofn.Flags = OFN_PATHMUSTEXIST;

		if (open)
		{
			ofn.Flags |= OFN_FILEMUSTEXIST;
		}

		bool res = false;

		if (open)
		{
			res = GetOpenFileNameA(&ofn) ? true : false;
		}
		else
		{
			res = GetSaveFileNameA(&ofn) ? true : false;
		}

		SetCurrentDirectoryA(curDir);

		if (res)
		{
			return fileName;
		}

		return nullptr;
	}

	bool Editor::Init(HWND setHwnd, const char* startProject)
	{
		ImGui_ImplWin32_EnableDpiAwareness();

		hwnd = setHwnd;

		if (!CreateDeviceD3D())
		{
			CleanupDeviceD3D();
			return false;
		}

		if (!root.Init(d3dDevice))
		{
			return false;
		}

		root.scripts.SetAllowDynamicReload(true);

		root.render.AddExecutedLevelPool(-1000);

		renderTaskPool = root.render.AddTaskPool(_FL_);
		renderTaskPool->AddTask(-1000, this, (Object::Delegate)&Editor::Render);

		if (!root.render.GetDevice()->SetBackBuffer(0, 800, 600, &hwnd))
		{
			return false;
		}

		freeCamera.Init();

		editorDrawer.Init();
		editorDrawer.hwnd = hwnd;

		SetupImGUI();

		LoadSettings();

		JsonReader reader;

		if (reader.ParseFile("projects"))
		{
			while (reader.EnterBlock("projects"))
			{
				ProjectEntry entry;
				reader.Read("name", entry.name);
				reader.Read("path", entry.path);

				LoadProjectIcon(entry);

				projects.push_back(entry);

				reader.LeaveBlock();
			}
		}

		if (startProject)
		{
			projectToLoad = startProject;
		}

		toolButtons.resize(Button::ButtonsCount);

		{
			auto& button = toolButtons[Button::Play];
			button.icon = 0;
			button.name = "Play";
			button.state = [this]() -> bool { return projectRunning; };
			button.callback = [this]() { if (!projectRunning) StartProject(); else StopProject(); };
		}

		{
			auto& button = toolButtons[Button::Build];
			button.icon = 7;
			button.name = "Build";
			button.callback = [this]() { root.scripts.CompileProjectCode(true); };
		}

		{
			auto& button = toolButtons[Button::Mode2D];
			button.icon = -1;
			button.name = "2D";
			button.state = [this]() -> bool { return freeCamera.mode2D; };
			button.callback = [this]() { freeCamera.mode2D = !freeCamera.mode2D; };
		}

		{
			auto& button = toolButtons[Button::Select];
			button.icon = 1;
			button.name = "Select";
			button.state = [this]() -> bool { return editMode == EditMode::Select; };
			button.callback = [this]() { editMode = EditMode::Select; gizmo.mode = TransformMode::None; };
			button.hotKey = "KEY_Q";
		}
		
		{
			auto& button = toolButtons[Button::Drag];
			button.icon = 2;
			button.name = "Drag";
			button.state = [this]() -> bool { return editMode == EditMode::DragFiled; };
			button.callback = [this]() { editMode = EditMode::DragFiled; gizmo.mode = TransformMode::None; };
			button.hotKey = "KEY_W";
		}

		{
			auto& button = toolButtons[Button::Move];
			button.icon = 3;
			button.name = "Move";
			button.state = [this]() -> bool { return editMode == EditMode::Move; };
			button.callback = [this]() { editMode = EditMode::Move; gizmo.mode = TransformMode::Move; };
			button.hotKey = "KEY_E";
		}

		{
			auto& button = toolButtons[Button::Rotate];
			button.icon = 4;
			button.name = "Rotate";
			button.state = [this]() -> bool { return editMode == EditMode::Rotate; };
			button.callback = [this]() { editMode = EditMode::Rotate; gizmo.mode = TransformMode::Rotate; };
			button.hotKey = "KEY_R";
		}

		{
			auto& button = toolButtons[Button::Scale];
			button.icon = 5;
			button.name = "Scale";
			button.state = [this]() -> bool { return editMode == EditMode::Scale; };
			button.callback = [this]() { editMode = EditMode::Scale; gizmo.mode = TransformMode::Scale; };
			button.hotKey = "KEY_T";
		}

		{
			auto& button = toolButtons[Button::Rectangle];
			button.icon = 6;
			button.name = "Rectangle";
			button.state = [this]() -> bool { return editMode == EditMode::Rectangle; };
			button.callback = [this]() { editMode = EditMode::Rectangle; gizmo.mode = TransformMode::Rectangle; };
			button.hotKey = "KEY_Y";
		}

		{
			auto& button = toolButtons[Button::Align];
			button.icon = 8;
			button.name = "Align";
			button.state = [this]() -> bool { return gizmo.useAlignGrid; };
			button.callback = [this]() { gizmo.useAlignGrid = !gizmo.useAlignGrid; };
			button.hotKey = "KEY_O";
		}

		{
			auto& button = toolButtons[Button::AlignX];
			button.icon = -1;
			button.name = "X";
			button.state = [this]() -> bool { return gizmo.alignGridFlag & Axis::X; };
			button.callback = [this]() { gizmo.alignGridFlag = (Axis)(gizmo.alignGridFlag ^ Axis::X); };
		}

		{
			auto& button = toolButtons[Button::AlignY];
			button.icon = -1;
			button.name = "Y";
			button.state = [this]() -> bool { return gizmo.alignGridFlag & Axis::Y; };
			button.callback = [this]() { gizmo.alignGridFlag = (Axis)(gizmo.alignGridFlag ^ Axis::Y); };
		}

		{
			auto& button = toolButtons[Button::AlignZ];
			button.icon = -1;
			button.name = "Z";
			button.state = [this]() -> bool { return gizmo.alignGridFlag & Axis::Z; };
			button.callback = [this]() { gizmo.alignGridFlag = (Axis)(gizmo.alignGridFlag ^ Axis::Z); };
		}

		{
			auto& button = toolButtons[Button::AlignOffset];
			button.icon = 9;
			button.name = "AlignOffset";
			button.state = [this]() -> bool { return gizmo.useAlignGridOffset; };
			button.callback = [this]() { gizmo.useAlignGridOffset = !gizmo.useAlignGridOffset; };
			button.hotKey = "KEY_P";
		}

		{
			auto& button = toolButtons[Button::AlignOffsetX];
			button.icon = -1;
			button.name = "X ";
			button.state = [this]() -> bool { return gizmo.alignOffsetGridFlag & Axis::X; };
			button.callback = [this]() { gizmo.alignOffsetGridFlag = (Axis)(gizmo.alignOffsetGridFlag ^ Axis::X); };
		}

		{
			auto& button = toolButtons[Button::AlignOffsetY];
			button.icon = -1;
			button.name = "Y ";
			button.state = [this]() -> bool { return gizmo.alignOffsetGridFlag & Axis::Y; };
			button.callback = [this]() { gizmo.alignOffsetGridFlag = (Axis)(gizmo.alignOffsetGridFlag ^ Axis::Y); };
		}

		{
			auto& button = toolButtons[Button::AlignOffsetZ];
			button.icon = -1;
			button.name = "Z ";
			button.state = [this]() -> bool { return gizmo.alignOffsetGridFlag & Axis::Z; };
			button.callback = [this]() { gizmo.alignOffsetGridFlag = (Axis)(gizmo.alignOffsetGridFlag ^ Axis::Z); };
		}

		return true;
	}

	void Editor::LoadProjectIcon(ProjectEntry& entry)
	{
		JsonReader reader;

		if (reader.ParseFile(entry.path.c_str()))
		{
			eastl::string iconPath;
			reader.Read("iconPath", iconPath);

			char projectPath[1024];
			StringUtils::GetPath(entry.path.c_str(), projectPath);
			StringUtils::FixSlashes(projectPath);

			entry.icon = root.render.LoadTexture((projectPath + eastl::string("/Assets/") + iconPath).c_str(), _FL_);
		}
	}

	void Editor::SaveProjectsList()
	{
		JsonWriter writer;

		if (writer.Start("projects"))
		{
			writer.StartArray("projects");

			for (auto& entry : projects)
			{
				writer.StartBlock(nullptr);

				writer.Write("name", entry.name);
				writer.Write("path", entry.path);

				writer.FinishBlock();
			}

			writer.FinishArray();
		}
	}

	void Editor::LoadSettings()
	{
		JsonReader reader;

		if (reader.ParseFile(StringUtils::PrintTemp("%s/ENgine/editor_settings", std::filesystem::current_path().string().c_str())))
		{
			reader.Read("themeName", selectedThemeName);

			if (reader.EnterBlock("FreeCamera"))
			{
				reader.Read("rotationSensivity", freeCamera.rotationSensivity);
				reader.Read("moveSpeed", freeCamera.moveSpeed);
				reader.Read("moveFastSpeed", freeCamera.moveFastSpeed);
				reader.Read("moveAcceleration", freeCamera.moveAcceleration);
				reader.Read("moveDeacceleration", freeCamera.moveDeacceleration);
				reader.Read("zoom2Dsensivity", freeCamera.zoom2Dsensivity);

				reader.LeaveBlock();
			}
		}

		ApplySelecetedTheme();
	}

	void Editor::SaveSettings()
	{
		JsonWriter writer;

		if (writer.Start(StringUtils::PrintTemp("%s/ENgine/editor_settings", std::filesystem::current_path().string().c_str())))
		{
			writer.Write("themeName", selectedThemeName);

			writer.StartBlock("FreeCamera");

			writer.Write("rotationSensivity", freeCamera.rotationSensivity);
			writer.Write("moveSpeed", freeCamera.moveSpeed);
			writer.Write("moveFastSpeed", freeCamera.moveFastSpeed);
			writer.Write("moveAcceleration", freeCamera.moveAcceleration);
			writer.Write("moveDeacceleration", freeCamera.moveDeacceleration);
			writer.Write("zoom2Dsensivity", freeCamera.zoom2Dsensivity);

			writer.FinishBlock();
		}
	}

	bool Editor::InSelectMode()
	{
		return editMode == EditMode::Select;
	}

	void Editor::SetupImGUI()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.MouseDrawCursor = true;

		io.ConfigWindowsMoveFromTitleBarOnly = true;

		ImGui::StyleColorsDark();

		ImGuiHelper::GetAllStyles(themes);

		ImGuiIO& guiIO = ImGui::GetIO();
		ImFont* font = io.Fonts->AddFontFromFileTTF("ENgine/DroidSans.ttf", 16);
		guiIO.FontDefault = font;

		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowPadding.x = 2;
		style.WindowPadding.y = 2;

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(d3dDevice, d3dDeviceContext);
	}

	void Editor::ApplySelecetedTheme()
	{
		selectedTheme = -1;

		for (int index = 0; index < themes.size(); index++)
		{
			if (StringUtils::IsEqual(selectedThemeName.c_str(), themes[index].c_str()))
			{
				selectedTheme = index;
				break;
			}
		}

		if (selectedTheme == -1)
		{
			selectedTheme = 0;
			selectedThemeName = themes[selectedTheme];
		}

		ImGuiHelper::LoadStyle(selectedThemeName.c_str());
	}

	void Editor::AddAction(IEditorAction* action)
	{
		curAction++;

		for (int i = curAction; i < actions.size(); i++)
		{
			actions[i]->Release();
		}

		actions.erase(actions.begin() + curAction, actions.end());

		actions.push_back(action);
	}

	void Editor::DeleteActionsFromHistory(void* owner)
	{
		for (int i = 0; i < actions.size(); i++)
		{
			if (actions[i]->owner == owner)
			{
				if (curAction >= i)
				{
					actions[i]->Release();
					actions.erase(actions.begin() + i);
					curAction--;
				}

				i--;
			}
		}
	}

	void Editor::Save()
	{
		if (selectedEditAsset)
		{
			selectedEditAsset->Save();
		}

		project.Save();
	}

	void Editor::RedoAction()
	{
		if (curAction < (int)actions.size() - 1)
		{
			curAction++;
			actions[curAction]->Apply();
		}
	}

	void Editor::UndoAction()
	{
		if (curAction >= 0)
		{
			actions[curAction]->Undo();
			curAction--;
		}
	}

	void Editor::ShowEditorSettings()
	{
		if (!showEditorSettings)
		{
			return;
		}

		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::Begin("Editor setting", &showEditorSettings, ImGuiWindowFlags_NoDocking);

		if (needSetSizeEdSet)
		{
			auto size = ImGui::GetWindowSize();

			if (size.x <= 32 && size.y <= 32)
			{
				ImGui::SetWindowSize(ImVec2(900.0f, 700.0f));
			}

			needSetSizeEdSet = false;
		}

		bool changed = false;

		bool is_open = ImGui::CollapsingHeader("Free camera###EditorSettingsFreeCamera", ImGuiTreeNodeFlags_DefaultOpen);

		if (is_open)
		{
			changed |= ImGui::SliderFloat("Rotation Sensivity", &freeCamera.rotationSensivity, 1.0f, 10.0f, "%.0f");
			changed |= ImGui::SliderFloat("Move Speed", &freeCamera.moveSpeed, 30.0f, 150.0f, "%.0f");
			changed |= ImGui::SliderFloat("Move Fast Speed", &freeCamera.moveFastSpeed, 200.0f, 500.0f, "%.0f");
			changed |= ImGui::SliderFloat("Move Accel", &freeCamera.moveAcceleration, 50.0f, 400.0f, "%.0f");
			changed |= ImGui::SliderFloat("Move Deaccel", &freeCamera.moveDeacceleration, 50.0f, 400.0f, "%.0f");
			changed |= ImGui::SliderFloat("Zoom2D Sensivity", &freeCamera.zoom2Dsensivity, 1.0f, 10.0f, "%.0f");
		}

		is_open = ImGui::CollapsingHeader("Color theme###EditorSettingsColorTheme", ImGuiTreeNodeFlags_DefaultOpen);

		if (is_open)
		{
			ImGui::Dummy(ImVec2(0.0f, 3.0f));
			ImGui::Dummy(ImVec2(3.0f, 3.0f));
			ImGui::SameLine();

			if (ImGuiHelper::InputCombobox("###SelectThemeName", selectedTheme, themes, themeList))
			{
				selectedThemeName = themes[selectedTheme];
				ApplySelecetedTheme();
				changed = true;
			}

			ImGui::Dummy(ImVec2(3.0f, 3.0f));
			ImGui::SameLine();

			is_open = ImGui::CollapsingHeader("Theme settings###EditorSettingsThemeSettings", ImGuiTreeNodeFlags_None);

			if (is_open)
			{
				ImGuiHelper::VerticalHorizontalPadding();

				ImGuiHelper::InputString("###ThemeName", selectedThemeName);
				ImGui::SameLine();

				if (ImGui::Button("Save"))
				{
					ImGuiHelper::SaveStyle(selectedThemeName.c_str());

					themeList.clear();
					ImGuiHelper::GetAllStyles(themes);

					ApplySelecetedTheme();
					changed = true;
				}

				ImGuiHelper::HorizontalPadding();

				static ImGuiTextFilter filter;
				filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

				static ImGuiColorEditFlags alpha_flags = 0;

				ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);

				for (int i = 0; i < ImGuiCol_COUNT; i++)
				{
					const char* name = ImGui::GetStyleColorName(i);
					if (!filter.PassFilter(name)) continue;

					ImGui::PushID(i);

					ImGuiHelper::HorizontalPadding();

					ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
					ImGui::TextUnformatted(name);
					ImGui::PopID();
				}

				ImGui::EndChild();
			}
		}

		if (changed)
		{
			SaveSettings();
		}

		ImGui::End();
	}

	bool Editor::AllowGrabFocused()
	{
		return viewportCaptured == ViewportCature::None;
	}	

	void Editor::DisallowMainFocus(bool disallow)
	{
		disallowMainFocus = disallow;
	}
	
	void Editor::Unfocus()
	{
		viewportFocused = false;
	}

	void Editor::ShowViewport()
	{
		if (openedAssets.size() == 0 || projectRunning)
		{
			if (project.runningInFullscreen)
			{
				ImGui::Begin("Game###GameFullscreen", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking);

				ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

				RECT desktopRect;
				HWND hDesktop = GetDesktopWindow();
				GetWindowRect(hDesktop, &desktopRect);

				ImGui::SetWindowSize(ImVec2((float)desktopRect.right, (float)desktopRect.bottom));
			}
			else
			{
				ImGui::Begin("Game###Game0", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			}

			ImGuiIO& io = ImGui::GetIO();

			ImVec2 viewportPos = ImVec2(io.MousePos.x - ImGui::GetCursorScreenPos().x, io.MousePos.y - ImGui::GetCursorScreenPos().y);

			root.controls.OverrideMousePos((int)viewportPos.x, (int)viewportPos.y);

			ImVec2 size = ImGui::GetContentRegionAvail();
			root.render.GetDevice()->SetBackBuffer(0, (int)size.x, (int)size.y, &hwnd);

			UpdateOrin();

			ImGui::Image(root.render.GetDevice()->GetBackBuffer(), size);

			ImGui::End();

			if (root.controls.DebugKeyPressed("KEY_F6") || root.IsQuitRequested())
			{
				root.SetQuitRequest(false);

				StopProject();
			}

			if (root.controls.DebugKeyPressed("KEY_F11"))
			{
				project.runningInFullscreen = !project.runningInFullscreen;
			}

			return;
		}

		if (openedAssets.size() == 0)
		{
			ImGui::Begin("Viewport###Game0", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::End();

			return;
		}

		int index2Delete = -1;
		bool needReselect = false;

		for (int i = 0; i < openedAssets.size(); i++)
		{
			auto& asset = openedAssets[i];

			if (!asset.Get())
			{
				openedAssets.erase(openedAssets.begin() + i);
				i--;

				continue;
			}

			bool opened = true;

			bool selected = asset.Get() == selectedEditAsset.Get();

			if (forceImGUISetViewportFocusCount > 0 && selected)
			{
				ImGui::SetNextWindowFocus();
			}

			ImGui::Begin(StringUtils::PrintTemp("%s%s###Game%i", asset->GetName().c_str(), asset->ContainsUnsavedChanges() ? "*" : "", i),
						openedAssets.size() > 1 ? &opened : nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			if (forceImGUISetViewportFocusCount == 0 && ImGui::IsWindowFocused() && !selected)
			{
				SelectEditAsset(asset);
			}

			if (selected)
			{
				ImGuiIO& io = ImGui::GetIO();

				ImVec2 viewportPos = ImVec2(io.MousePos.x - ImGui::GetCursorScreenPos().x, io.MousePos.y - ImGui::GetCursorScreenPos().y);

				root.controls.OverrideMousePos((int)viewportPos.x, (int)viewportPos.y);

				ImVec2 size = ImGui::GetContentRegionAvail();
				root.render.GetDevice()->SetBackBuffer(0, (int)size.x, (int)size.y, &hwnd);

				UpdateOrin();

				ImGui::Image(root.render.GetDevice()->GetBackBuffer(), size);

				vireportHowered = ImGui::IsItemHovered();

				viewportFocused = !disallowMainFocus && ImGuiHelper::IsFocused();
				root.controls.SetFocused(viewportFocused);

				if (vireportHowered)
				{
					if (projectRunning && project.hideCursor)
					{
						ImGui::SetMouseCursor(ImGuiMouseCursor_None);
					}

					ImGui::SetWindowFocus();

					if (viewportFocused)
					{
						if (ImGui::IsMouseClicked(0))
						{
							if (selectedEditAsset)
							{
								selectedEditAsset->OnLeftMouseDown();
							}

							gizmo.OnLeftMouseDown();

							viewportCaptured = ViewportCature::LeftButton;
						}
						else
						if (ImGui::IsMouseClicked(1))
						{
							if (selectedEditAsset)
							{
								selectedEditAsset->OnRightMouseDown();
							}

							viewportCaptured = ViewportCature::RightButton;
						}
						else
						if (ImGui::IsMouseClicked(2))
						{
							if (selectedEditAsset)
							{
								selectedEditAsset->OnMiddleMouseDown();
							}

							viewportCaptured = ViewportCature::MiddleButton;
						}
					}
				}

				gizmo.OnMouseMove(Math::Vector2((float)viewportPos.x, (float)viewportPos.y));

				if (selectedEditAsset)
				{
					selectedEditAsset->ImGuiViewport(viewportFocused);
					selectedEditAsset->OnMouseMove(Math::Vector2((float)viewportPos.x, (float)viewportPos.y));
				}

				if (viewportCaptured == ViewportCature::LeftButton && ImGui::IsMouseReleased(0))
				{
					gizmo.OnLeftMouseUp();

					if (selectedEditAsset)
					{
						selectedEditAsset->OnLeftMouseUp();
					}

					viewportCaptured = ViewportCature::None;
				}
				else
				if (viewportCaptured == ViewportCature::RightButton && ImGui::IsMouseReleased(1))
				{
					if (selectedEditAsset)
					{
						selectedEditAsset->OnRightMouseUp();
					}

					viewportCaptured = ViewportCature::None;
				}
				else
				if (viewportCaptured == ViewportCature::MiddleButton && ImGui::IsMouseReleased(2))
				{
					if (selectedEditAsset)
					{
						selectedEditAsset->OnMiddleMouseUp();
					}

					viewportCaptured = ViewportCature::None;
				}
			}

			ImGui::End();

			if (!opened)
			{
				index2Delete = i;
				needReselect = selected;
			}
		}

		if (index2Delete != -1)
		{
			openedAssets.erase(openedAssets.begin() + index2Delete);

			if (index2Delete >= openedAssets.size())
			{
				index2Delete--;
			}

			SelectEditAsset(openedAssets[index2Delete]);
		}

		if (forceImGUISetViewportFocusCount > 0)
		{
			forceImGUISetViewportFocusCount--;
		}
	}

	void Editor::ShowCreateAssetDialog()
	{
		if (assetDialog == AssetDialog::Inactive)
		{
			return;
		}

		bool open = true;

		if (need2openAssetPopup)
		{
			need2openAssetPopup = false;
			ImGui::OpenPopup("Enter name");
		}

		ImGui::SetNextWindowSize(ImVec2(300.0f, 130.0f));

		if (ImGui::BeginPopupModal("Enter name", &open, ImGuiWindowFlags_NoResize))
		{
			ImGui::Dummy(ImVec2(3.0f, 3.0f));

			ImGui::Dummy(ImVec2(3.0f, 3.0f));
			ImGui::SameLine();

			ImGui::Text("Name");
			ImGui::SameLine();

			ImGuiHelper::InputString("##NewName", createAssetName);

			if (assetDialog == AssetDialog::Prefab && !duplicateAsset)
			{
				ImGui::Dummy(ImVec2(3.0f, 3.0f));
				ImGui::SameLine();

				ImGui::Text("Type");
				ImGui::SameLine();

				ImGuiHelper::InputCombobox("##PrefabType", createPrefabTypesIndex, createPrefabTypesList, createPrefabTypesListFlat);
			}

			ImGui::Dummy(ImVec2(3.0f, 3.0f));

			ImGui::Dummy(ImVec2(90.0f, 7.0f));
			ImGui::SameLine();

			if (ImGui::Button("Ok", ImVec2(50.0f, 0.0f)))
			{
				eastl::string path = createAssetName;

				if (selectedFolder != nullptr)
				{
					path = selectedFolder->fullName + createAssetName;
				}
				else
				if (selectedAssetHolder != nullptr)
				{
					char assetPath[512];
					assetPath[0] = 0;
					StringUtils::GetPath(selectedAssetHolder->fullName.c_str(), assetPath);

					path = assetPath + createAssetName;
				}

				path = root.GetPath(Root::Path::Assets) + path;

				open = false;

				const char* AssetsExt[] =
				{
					nullptr,
					nullptr,
					"scn",
					"ang",
					"tls",
					"prefab",
					"spl"
				};

				if (std::filesystem::exists(path.c_str()))
				{
					MESSAGE_BOX("Can't create", "Already exists");
				}
				else
				{
					if (assetDialog == AssetDialog::Folder)
					{
						std::filesystem::create_directories(path.c_str());
					}
					else
					{
						eastl::string metaPath = StringUtils::PrintTemp("%s.%s.meta", path.c_str(), AssetsExt[(int)assetDialog]);

						{
							File asset;
							asset.Open(StringUtils::PrintTemp("%s.%s.meta", path.c_str(), AssetsExt[(int)assetDialog]), File::ModeType::WriteText);
						}

						path = StringUtils::PrintTemp("%s.%s", path.c_str(), AssetsExt[(int)assetDialog]);

						{
							File asset;
							asset.Open(path.c_str(), File::ModeType::WriteText);
						}

						if (assetDialog == AssetDialog::Prefab || assetDialog == AssetDialog::Scene)
						{
							AssetScene* scene = assetDialog == AssetDialog::Prefab ? new AssetPrefab() : new AssetScene();
							scene->Init();
							scene->SetPath(path.c_str());
							scene->uid = root.scenes.GenerateUID();

							if (duplicateAsset)
							{
								AssetScene* srcScene = selectedAssetHolder->GetAsset<AssetScene>();
								scene->Copy(srcScene);
							}
							else
							if (scene->IsPrefab())
							{
								dynamic_cast<AssetPrefab*>(scene)->SetRootEntityType(createPrefabTypesList[createPrefabTypesIndex].c_str());
							}

							scene->Save();

							delete scene;

							project.Save();
						}

						Perforce::AddToDepot(path.c_str());
						Perforce::AddToDepot(metaPath.c_str());
					}
				}
					
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (open == false)
		{
			assetDialog = AssetDialog::Inactive;
		}
	}

	void Editor::ShowAbout()
	{
		if (!showAbout)
		{
			return;
		}

		ImGui::Begin("About", &showAbout, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize(ImVec2(300.0f, 200.0f));
		ImGui::Text("Orin is awesome!!!");

		if (ImGui::Button("Ok", ImVec2(40, 30)))
		{
			showAbout = false;
		}

		ImGui::End();
	}

	void Editor::ShowProjectSettings()
	{
		if (!showProjectSettings)
		{
			return;
		}

		ImGui::Begin("Project Settings", &showProjectSettings, ImGuiWindowFlags_NoDocking);

		if (needSetSizePrjSet)
		{
			auto size = ImGui::GetWindowSize();

			if (size.x <= 32 && size.y <= 32)
			{
				ImGui::SetWindowSize(ImVec2(900.0f, 700.0f));
			}

			needSetSizePrjSet = false;
		}

		ImGui::Columns(1);

		bool is_open = ImGui::CollapsingHeader("Windows###ProjectSettingsWindows", ImGuiTreeNodeFlags_DefaultOpen);

		if (is_open)
		{
			ImGui::Columns(2);

			ImGui::Text("Window title");
			ImGui::NextColumn();

			ImGuiHelper::InputString("###Windowtitle", project.projectName);

			ImGui::NextColumn();

			ImGui::Text("Icon");
			ImGui::NextColumn();

			if (ImGui::Button(StringUtils::PrintTemp("%s###ProjectSettingsIconWin", project.iconPath[0] ? project.iconPath.c_str() : "File not set"), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
			{
				const char* fileName = OpenFileDialog("Icon texture", nullptr, true);

				if (fileName)
				{
					char relativeName[512];
					StringUtils::GetCropPath(root.GetPath(Root::Path::Assets), fileName, relativeName, 512);

					project.iconPath = relativeName;
					project.icon = root.render.LoadTexture(project.iconPath.c_str(), _FL_);
				}
			}

			ImGui::Image(project.icon.Get() ? project.icon->GetNativeResource() : nullptr, ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

			ImGui::NextColumn();

			ImGui::Text("IconSmall");
			ImGui::NextColumn();

			if (ImGui::Button(StringUtils::PrintTemp("%s###ProjectSettingsIconSmallWin", project.iconSmallPath[0] ? project.iconSmallPath.c_str() : "File not set"), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
			{
				const char* fileName = OpenFileDialog("Icon texture", nullptr, true);

				if (fileName)
				{
					char relativeName[512];
					StringUtils::GetCropPath(root.GetPath(Root::Path::Assets), fileName, relativeName, 512);

					project.iconSmallPath = relativeName;
					project.iconSmall = root.render.LoadTexture(project.iconSmallPath.c_str(), _FL_);
				}
			}

			ImGui::Image(project.iconSmall.Get() ? project.iconSmall->GetNativeResource() : nullptr, ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

			ImGui::NextColumn();

			ImGui::Columns(1);
		}

		is_open = ImGui::CollapsingHeader("General###ProjectSettingsGeneral", ImGuiTreeNodeFlags_DefaultOpen);

		if (is_open)
		{
			ImGui::Checkbox("Hide Cursor", &project.hideCursor);
		}

		is_open = ImGui::CollapsingHeader("2D###ProjectSettings2D", ImGuiTreeNodeFlags_DefaultOpen);

		if (is_open)
		{
			ImGui::Columns(2);

			ImGui::Text("Pixels height");
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(-1);

			float pixelsHeight = Sprite::GetPixelsHeight();
			ImGui::InputFloat("###ProjectSettings2DPH", &pixelsHeight);

			ImGui::NextColumn();

			ImGui::Text("Pixels per unit");
			ImGui::NextColumn();

			ImGui::SetNextItemWidth(-1);

			float pixelsPerUnit = Sprite::ToPixels(1.0f);

			if (ImGui::InputFloat("###ProjectSettings2DPPU", &pixelsPerUnit))
			{
				if (pixelsPerUnit < 1.0f)
				{
					pixelsPerUnit = 1.0f;
				}
			}

			Sprite::SetData(pixelsHeight, pixelsPerUnit);

			ImGui::NextColumn();

			ImGui::Columns(1);
		}

		is_open = ImGui::CollapsingHeader("Export###ProjectSettingsExport", ImGuiTreeNodeFlags_DefaultOpen);

		if (is_open)
		{
			ImGui::Columns(2);

			ImGui::Text("Export Directory");
			ImGui::NextColumn();

			if (ImGui::Button(StringUtils::PrintTemp("%s###ProjectSettingsExportDir", project.exportDir[0] ? project.exportDir.c_str() : "File not set"), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
			{
				project.SelectExportDir();
			}

			ImGui::NextColumn();

			ImGui::Columns(1);
		}

		is_open = ImGui::CollapsingHeader("P4###ProjectSettingsP4", ImGuiTreeNodeFlags_DefaultOpen);

		if (is_open)
		{
			ImGui::Columns(2);

			ImGui::Text("URL");
			ImGui::NextColumn();

			bool changed = false;

			changed |= ImGuiHelper::InputString("###P4URL", project.p4URL);
			ImGui::NextColumn();

			ImGui::Text("Workspace");
			ImGui::NextColumn();

			changed |= ImGuiHelper::InputString("###P4Workspace", project.p4Workspace);
			ImGui::NextColumn();

			ImGui::Text("User");
			ImGui::NextColumn();

			changed |= ImGuiHelper::InputString("###P4User", project.p4User);
			ImGui::NextColumn();

			if (changed)
			{
				Perforce::SetConfig(project.p4URL.c_str(), project.p4Workspace.c_str(), project.p4User.c_str());
			}

			ImGui::Text("Status");
			ImGui::NextColumn();

			if (ImGui::Button("Check Connection"))
			{	
				if (Perforce::CheckConnection())
				{
					MESSAGE_BOX("Connection succeed", "Request to p4 was successfull");
				}
				else
				{
					MESSAGE_BOX("Connection error", "Failed connect to P4 server. Please check your P4 settings");
				}
			}

			ImGui::NextColumn();

			ImGui::Columns(1);
		}

		ImGui::End();
	}

	void Editor::ShowSelectProject()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("Projects", nullptr, window_flags);

		ImGui::Dummy(ImVec2(0.0f, 3.0f));
		ImGui::Dummy(ImVec2(3.0f, 3.0f));
		ImGui::SameLine();

		const char* projectToAdd = nullptr;

		if (ImGui::Button("Add", ImVec2(40.0f, 25.0f)))
		{
			const char* fileName = OpenFileDialog("Project file", "pra", true);

			if (fileName)
			{
				StringUtils::FixSlashes((char*)fileName);
				projectToAdd = fileName;

				for (auto& entry : projects)
				{
					if (StringUtils::IsEqual(entry.path.c_str(), fileName))
					{
						projectToAdd = nullptr;
						MESSAGE_BOX("Can't add a project", "Project wasalready added");
					}
				}
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("New", ImVec2(40.0f, 25.0f)))
		{
			const char* fileName = OpenFileDialog("Project file", "pra", false);

			if (fileName)
			{
				char path[512];
				StringUtils::GetPath(fileName, path);

				bool empty = std::filesystem::directory_iterator(path) == std::filesystem::directory_iterator();
				
				if (!empty)
				{
					MESSAGE_BOX("Can't create a project", "Folder is not empty");
				}
				else
				{
					root.files.CreateFolder(StringUtils::PrintTemp("%sAssets/tst", path));
					projectToAdd = fileName;
					projectToLoad = fileName;
				}
			}
		}

		if (projectToAdd)
		{
			ProjectEntry entry;
			char name[256];
			StringUtils::GetFileName(projectToAdd, name);
			StringUtils::RemoveExtension(name);
			entry.name = name;
			entry.path = projectToAdd;

			LoadProjectIcon(entry);

			projects.push_back(entry);

			SaveProjectsList();

			if (projectToLoad)
			{
				project.Save(projectToLoad);
			}
		}

		ImVec2 scrolling_child_size = ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 7 + 30);
		ImGui::BeginChild("ProjectsList");

		ImGuiContext* context = ImGui::GetCurrentContext();
		ImGuiWindow* window = context->CurrentWindow;

		for (int i = 0; i < projects.size(); i++)
		{
			auto& entry = projects[i];

			ImGui::Dummy(ImVec2(0.0f, 3.0f));

			ImVec2 p = ImGui::GetCursorScreenPos();
			window->DrawList->AddRectFilled(ImVec2(p.x + 5, p.y), ImVec2(p.x + ImGui::GetContentRegionAvail().x - 6, p.y + 82) /*- ImVec2(10.0f, 10.0f)*/, IM_COL32(80, 80, 80,255));

			ImGui::Dummy(ImVec2(0.0f, 3.0f));
			ImGui::Dummy(ImVec2(3.0f, 3.0f));
			ImGui::SameLine();

			ImGui::PushID(StringUtils::PrintTemp("%s%i", entry.path.c_str(), i));

			ImGui::BeginGroup();
			ImGui::Image(entry.icon.Get() ? entry.icon->GetNativeResource() : editorDrawer.projectIconTex->GetNativeResource(), ImVec2(70, 70));
			ImGui::EndGroup();

			ImGui::SameLine();

			ImGui::BeginGroup();
			ImGui::Text(entry.name.c_str());
			ImGui::Text(entry.path.c_str());

			if (std::filesystem::exists(entry.path.c_str()))
			{
				if (ImGui::Button("Load"))
				{
					projectToLoad = entry.path.c_str();
				}
			}
			else
			{
				ImGui::Text("[Project was deleted]");
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				projects.erase(projects.begin() + i);

				SaveProjectsList();

				i--;
			}

			ImGui::EndGroup();

			ImGui::PopID();

			ImGui::Dummy(ImVec2(0.0f, 3.0f));
		}
		
		ImGui::EndChild();

		ImGui::End();
	}

	void Editor::LoadProject(const char* projectPath)
	{
		char curDir[1024];
		GetCurrentDirectoryA(1024, curDir);

		eastl::string projectFullName;
		if (projectPath[1] == '.')
		{
			std::filesystem::path path = StringUtils::PrintTemp("%s/%s", curDir, projectPath);
			projectFullName = std::filesystem::canonical(path).u8string().c_str();
		}
		else
		{
			projectFullName = projectPath;
		}


		if (project.Load(projectFullName.c_str()))
		{
			hackMaximaize = 10;
		}

		projectToLoad = nullptr;
	}

	void Editor::MaximizeEditorWindow()
	{
		if (hackMaximaize > 0)
		{
			hackMaximaize--;

			if (hackMaximaize == 0)
			{
				hackMaximaize = -1;
				ShowWindow(hwnd, SW_MAXIMIZE);
			}
		}		
	}

	void Editor::SelectEditAsset(AssetRef asset)
	{
		if (asset && !asset->IsEditable())
		{
			return;
		}

		selectedAsset = asset;

		if (selectedEditAsset)
		{
			selectedEditAsset->EnableEditing(false);
		}

		selectedEditAsset = AssetRef(asset);

		forceImGUISetViewportFocusCount = 2;

		if (selectedEditAsset)
		{
			auto iter = eastl::find_if(openedAssets.begin(), openedAssets.end(), [this](const AssetRef& asset) { return asset && StringUtils::IsEqual(asset->GetPath().c_str(), selectedEditAsset->GetPath().c_str()); });

			if (iter == openedAssets.end())
			{
				openedAssets.push_back(selectedEditAsset);
			}

			selectedEditAsset->EnableEditing(true);
		}
	}

	bool Editor::CreateDeviceD3D()
	{
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION,
		                                  &sd, &swapChain, &d3dDevice, &featureLevel, &d3dDeviceContext) != S_OK)
		{
			return false;
		}

		CreateRenderTarget();

		return true;
	}

	void Editor::CleanupDeviceD3D()
	{
		CleanupRenderTarget();

		RELEASE(swapChain)
		RELEASE(d3dDeviceContext)
		RELEASE(d3dDevice)
	}

	void Editor::CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
		pBackBuffer->Release();
	}

	void Editor::CleanupRenderTarget()
	{
		RELEASE(mainRenderTargetView)
	}

	void Editor::AssetsTreePopup(bool contextItem)
	{
		if (assetsTreePopup)
		{
			return;
		}

		if ((contextItem && ImGui::BeginPopupContextItem("CreateEntity")) ||
			(!contextItem && ImGui::BeginPopupContextWindow("CreateEntity")))
		{
			assetsTreePopup = true;

			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Folder"))
				{
					assetDialog = AssetDialog::Folder;
				}

				if (ImGui::MenuItem("Scene"))
				{
					assetDialog = AssetDialog::Scene;
				}

				if (ImGui::MenuItem("Anim Graph 2D"))
				{
					assetDialog = AssetDialog::AnimGraph2D;
				}

				if (ImGui::MenuItem("Sprites Layer"))
				{
					assetDialog = AssetDialog::SpritesLayer;
				}

				if (ImGui::MenuItem("TileSet"))
				{
					assetDialog = AssetDialog::TileSet;
				}

				if (ImGui::MenuItem("Prefab"))
				{
					createPrefabTypesIndex = 0;
					
					createPrefabTypesList.clear();

					auto& decls = ClassFactorySceneEntity::Decls();

					for (auto& decl : decls)
					{
						createPrefabTypesList.push_back(decl->GetShortName());
					}

					assetDialog = AssetDialog::Prefab;
				}

				if (assetDialog != AssetDialog::Inactive)
				{
					need2openAssetPopup = true;
					createAssetName = "";
					duplicateAsset = false;
				}

				ImGui::EndMenu();
			}

			if (selectedAssetHolder)
			{
				AssetScene* scene = selectedAssetHolder->GetAsset<AssetScene>();

				if (scene)
				{
					if (!scene->IsPrefab() && ImGui::MenuItem("Mark as start scne") && !scene->IsPrefab())
					{
						project.SetStartScene(scene->GetPath());
					}

					if (ImGui::MenuItem("Duplicate"))
					{
						need2openAssetPopup = true;
						assetDialog = scene->IsPrefab() ? AssetDialog::Prefab : AssetDialog::Scene;
						createAssetName = "";
						duplicateAsset = true;
					}
				}
			}

			/*if (ImGui::MenuItem("Rename"))
			{
			}*/

			/*if (ImGui::MenuItem("Delete"))
			{
				//auto* scene = project.selectedScene;
				//project.SelectScene(nullptr);
				//project.DeleteScene(scene);
			}*/

			ImGui::EndPopup();
		}
	}

	void Editor::CaptureLog(const char* name, const char* text)
	{
		LogCategory* category = nullptr;

		if (logCategories.count(name) > 0)
		{
			category = logCategories[name];
		}
		else
		{
			category = NEW LogCategory();

			logCategories[name] = category;
		}

		category->logs.push_front();
		category->logs.front() = text;

		if (category->logs.size() <= 128)
		{
			category->logsPtr.push_back();
		}

		int index = 0;

		for (auto& item : category->logs)
		{
			category->logsPtr[index] = item.c_str();
			index++;
		}
	}

	void Editor::AssetsFolder(Assets::Folder* folder)
	{
		for (auto* item : folder->folders)
		{
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

			if (item == selectedFolder)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}

			bool open = ImGui::TreeNodeEx(item, nodeFlags, item->name.c_str());

			if (ImGui::IsItemHovered() && (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
			{
				selectedFolder = item;
				selectedAssetHolder = nullptr;
				selectedAsset = AssetRef();
			}

			AssetsTreePopup(false);

			if (open)
			{
				AssetsFolder(item);

				ImGui::TreePop();
			}
		}

		for (auto* item : folder->assets)
		{
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

			if (item == selectedAssetHolder)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}

			eastl::string name = item->name;

			if (item->GetAsset<AssetScene>() && project.IsStartScene(item->name.c_str()))
			{
				name += "(Main)";
			}

			bool open = ImGui::TreeNodeEx(item, nodeFlags, name.c_str());

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				selectedFolder = nullptr;
				selectedAssetHolder = item;

				Save();

				SelectEditAsset(item->GetAssetRef<AssetRef>());
			}

			if (ImGui::IsItemHovered() && (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
			{
				selectedFolder = nullptr;
				selectedAssetHolder = item;
				selectedAsset = item->GetAssetRef<AssetRef>();
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				if (StringUtils::IsEqual(item->GetAssetType(), "AssetTexture"))
				{
					draggedTextureAsset = item->GetAssetRef<AssetTextureRef>();
					AssetTextureRef* ptr = (AssetTextureRef*)&draggedTextureAsset;
					ImGui::SetDragDropPayload("_ASSET_TEX", &ptr, sizeof(AssetTextureRef*));
				}
				else
				if (StringUtils::IsEqual(item->GetAssetType(), "AssetAnimGraph2D"))
				{
					draggedAssetAnimGraph2D = item->GetAssetRef<AssetAnimGraph2DRef>();
					AssetAnimGraph2DRef* ptr = (AssetAnimGraph2DRef*)&draggedAssetAnimGraph2D;
					ImGui::SetDragDropPayload("_ASSET_ANIM_GRAPH_2D", &ptr, sizeof(AssetAnimGraph2DRef*));
				}
				else
				if (StringUtils::IsEqual(item->GetAssetType(), "AssetTileSet"))
				{
					draggedAssetTileSet = item->GetAssetRef<AssetTileSetRef>();
					AssetTileSetRef* ptr = (AssetTileSetRef*)&draggedAssetTileSet;
					ImGui::SetDragDropPayload("_ASSET_TILE_SET", &ptr, sizeof(AssetTileSetRef*));
				}
				else
				if (StringUtils::IsEqual(item->GetAssetType(), "AssetSpritesLayer"))
				{
					draggeSpriteLayerSet = item->GetAssetRef<AssetSpritesLayerRef>();
					AssetSpritesLayerRef* ptr = (AssetSpritesLayerRef*)&draggeSpriteLayerSet;
					ImGui::SetDragDropPayload("_ASSET_SPRITES_LAYER", &ptr, sizeof(AssetSpritesLayerRef*));
				}
				else
				if (StringUtils::IsEqual(item->GetAssetType(), "AssetPrefab"))
				{
					ImGui::SetDragDropPayload("_ASSET_PREFAB", &item, sizeof(item));
				}


				ImGui::EndDragDropSource();
			}

			if (open)
			{
				ImGui::TreePop();
			}
		}
	}

	void Editor::StartProject()
	{
		if (!project.CanRun())
		{
			return;
		}

		if (!root.scripts.Start())
		{
			return;
		}

		if (project.projectFullName.size() == 0)
		{
			const char* fileName = OpenFileDialog("Project file", "pra", false);

			if (fileName)
			{
				project.projectFullName = fileName;
			}
			else
			{
				return;
			}
		}

		root.scripts.SetAllowDynamicReload(false);

		if (selectedEditAsset)
		{
			selectedEditAsset->EnableEditing(false);
		}

		Save();

		root.PreparePhysScene();
		root.scenes.LoadProject(project.projectFullName.c_str());

		projectRunning = true;
	}

	void Editor::StopProject()
	{
		if (!projectRunning)
		{
			return;
		}

		root.scenes.UnloadAll();

		root.scripts.Stop();

		root.ClearPhysScene();

		if (selectedEditAsset)
		{
			selectedEditAsset->EnableEditing(true);
			forceImGUISetViewportFocusCount = 2;
		}

		root.scripts.SetAllowDynamicReload(true);

		projectRunning = false;
	}

	bool Editor::ShowEditor()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspaceID = ImGui::GetID("DockSpace");

		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		if (!ImGui::DockBuilderGetNode(dockspaceID))
		{
			ImGui::DockBuilderRemoveNode(dockspaceID);
			ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);

			ImGuiID dock_main_id = dockspaceID;
			ImGuiID dock_top_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.05f, nullptr, &dock_main_id);
			ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
			ImGuiID dock_right_up_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Up, 0.2f, nullptr, &dock_right_id);
			ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.2f, nullptr, &dock_right_id);
			ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.2f, nullptr, &dock_main_id);
			ImGuiID dock_right_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.3f, nullptr, &dock_main_id);

			ImGui::DockBuilderDockWindow("Toolbar", dock_top_id);

			for (int i = 0; i < 100; i++)
			{
				ImGui::DockBuilderDockWindow(StringUtils::PrintTemp("###Game%i", i), dock_main_id);
			}

			ImGui::DockBuilderDockWindow("TileSet Editor", dock_right_right_id);

			ImGui::DockBuilderDockWindow("###Scene", dock_right_up_id);
			ImGui::DockBuilderDockWindow("Layers", dock_right_up_id);
			ImGui::DockBuilderDockWindow("Properties", dock_right_down_id);
			ImGui::DockBuilderDockWindow("Assets browser", dock_bottom_id); 
			ImGui::DockBuilderDockWindow("Console", dock_bottom_id);

			ImGui::DockBuilderFinish(dock_main_id);

			ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_top_id);
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
			node->LocalFlags |= ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoResizeY | ImGuiDockNodeFlags_NoResizeX;
		}

		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{				
				if (ImGui::MenuItem("Save"))
				{
					Save();
				}

				if (ImGui::Button("Save All"))
				{
					for (int i = 0; i < openedAssets.size(); i++)
					{
						auto& asset = openedAssets[i];

						if (asset.Get())
						{
							asset->Save();
						}
					}
					
					Save();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					return false;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Editor settings") && !showEditorSettings)
				{
					showEditorSettings = true;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Settings") && !showProjectSettings)
				{
					showProjectSettings = true;
				}

				if (ImGui::MenuItem("Export"))
				{
					project.Export();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Docs"))
				{
					char curDir[512];
					GetCurrentDirectoryA(512, curDir);

					ShellExecuteA(NULL, "open", StringUtils::PrintTemp("%s/Docs/html/index.html", curDir), NULL, NULL, SW_SHOWNORMAL);
				}

				if (ImGui::MenuItem("About") && !showAbout)
				{
					showAbout = true;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		//ImGui::ShowDemoWindow();

		projectTreePopup = false;
		assetsTreePopup = false;

		{
			ImGui::Begin("Toolbar");

			ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.29f, 0.29f, 0.29f, 1.0f));

			ImGui::Dummy(ImVec2(3.0f, 3.0f));

			ImGui::Dummy(ImVec2(3.0f, 3.0f));
			ImGui::SameLine();

			toolButtons[Button::Play].Execute();

			if (!projectRunning && root.controls.DebugKeyPressed("KEY_F7"))
			{
				StartProject();
			}

			toolButtons[Button::Build].Execute();			

			ImGui::SameLine();

			toolButtons[Button::Mode2D].Execute();

			if (freeCamera.mode2D)
			{
				ImGuiStyle& style = ImGui::GetStyle();

				int y = 1;
				int x = 0;
				float du = 24.0f / 240.0f;
				float dv = 24.0f / 72.0f;

				ImVec2 size = ImVec2(24.0f, 24.0f);
				ImVec2 uv0 = ImVec2(x * du, y * dv);
				ImVec2 uv1 = ImVec2((x + 1) * du, (y + 1) * dv);
				ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

				ImGui::PushID("Zoom");
				ImGui::Image(editorDrawer.iconsTex->GetNative(), size, uv0, uv1, tint_col, style.Colors[ImGuiCol_FrameBg]);
				ImGui::PopID();
				

				ImGui::SameLine();

				ImGui::SetNextItemWidth(60.0f);
				if (ImGui::InputFloat("##Zoom2D", &freeCamera.zoom2D))
				{
					freeCamera.ClampZoom2D();
				}
				ImGui::SameLine();
			}

			ImGui::Separator();
			ImGui::SameLine();

			ImGui::Separator();
			ImGui::SameLine();

			{
				toolButtons[Button::Select].Execute();
				toolButtons[Button::Drag].Execute();
				toolButtons[Button::Move].Execute();
				toolButtons[Button::Rotate].Execute();
				toolButtons[Button::Scale].Execute();
				toolButtons[Button::Rectangle].Execute();

				if (!freeCamera.mode2D && ImGui::Button(gizmo.useLocalSpace ? "Local" : "Global", ImVec2(50.0f, 25.0f)))
				{
					gizmo.useLocalSpace = !gizmo.useLocalSpace;
				}

				ImGui::SameLine();

				ImGui::Separator();
				ImGui::SameLine();

				auto* transform = gizmo.transform;

				/*if (ImGui::Button("To Object", ImVec2(75.0f, 25.0f)) && transform)
				{
					auto pos = transform->GetGlobal().Pos();
					freeCamera.pos = pos - Math::Vector3(cosf(freeCamera.angles.x), sinf(freeCamera.angles.y), sinf(freeCamera.angles.x)) * 5.0f;
				}

				ImGui::SameLine();

				if (ImGui::Button("To Camera", ImVec2(75.0f, 25.0f)) && transform)
				{
					auto pos = freeCamera.pos;
					transform->position = pos + Math::Vector3(cosf(freeCamera.angles.x), sinf(freeCamera.angles.y), sinf(freeCamera.angles.x)) * 5.0f;
				}

				ImGui::SameLine();*/

				ImGui::SameLine();

				ImGui::Separator();
				ImGui::SameLine();
			}

			{
				toolButtons[Button::Align].Execute();

				toolButtons[Button::AlignX].Execute();

				ImGui::SetNextItemWidth(80.0f);
				int value = (int)gizmo.alignGrid.x;
				ImGui::InputInt("##SnapXID", &value);
				if (value < 2) value = 2;
				gizmo.alignGrid.x = (float)value;
				ImGui::SameLine();

				toolButtons[Button::AlignY].Execute();

				ImGui::SetNextItemWidth(80.0f);
				value = (int)gizmo.alignGrid.y;
				ImGui::InputInt("##SnapYID", &value, ImGuiInputTextFlags_CharsDecimal);
				if (value < 2) value = 2;
				gizmo.alignGrid.y = (float)value;
				ImGui::SameLine();

				toolButtons[Button::AlignZ].Execute();

				ImGui::SetNextItemWidth(80.0f);
				value = (int)gizmo.alignGrid.z;
				ImGui::InputInt("##SnapZID", &value, ImGuiInputTextFlags_CharsDecimal);
				if (value < 2) value = 2;
				gizmo.alignGrid.z = (float)value;
				ImGui::SameLine();
				ImGui::Separator();
				ImGui::SameLine();

				{
					toolButtons[Button::AlignOffset].Execute();

					toolButtons[Button::AlignOffsetX].Execute();					

					ImGui::SetNextItemWidth(80.0f);
					value = (int)gizmo.alignGridOffset.x;
					ImGui::InputInt("##SnapOffsetXID", &value);
					if (value < 0) value = 0;
					gizmo.alignGridOffset.x = (float)value;
					ImGui::SameLine();

					toolButtons[Button::AlignOffsetY].Execute();

					ImGui::SetNextItemWidth(80.0f);
					value = (int)gizmo.alignGridOffset.y;
					ImGui::InputInt("##SnapOffsetYID", &value, ImGuiInputTextFlags_CharsDecimal);
					if (value < 0) value = 0;
					gizmo.alignGridOffset.y = (float)value;
					ImGui::SameLine();

					toolButtons[Button::AlignOffsetZ].Execute();

					ImGui::SetNextItemWidth(80.0f);
					value = (int)gizmo.alignGridOffset.z;
					ImGui::InputInt("##SnapOffsetZID", &value, ImGuiInputTextFlags_CharsDecimal);
					if (value < 0) value = 0;
					gizmo.alignGridOffset.z = (float)value;
					ImGui::SameLine();
				}
			}

			ImGui::PopStyleColor(1);

			ImGui::End();
		}

		{
			ImGui::Begin("Layers");

			ImGuiHelper::InputString("###LayerName", layerName);
			ImGui::SameLine();

			if (ImGui::Button("Add"))
			{
				auto iter = eastl::find_if(project.layers.begin(), project.layers.end(), [this](const Project::Layer& layer) { return StringUtils::IsEqual(layer.name.c_str(), this->layerName.c_str()); });

				if (iter == project.layers.end() && !layerName.empty())
				{
					Project::Layer layer;
					layer.name = layerName;

					project.layers.push_back(layer);
					selLayer = (int)project.layers.size() - 1;
				}
			}
			
			ImGui::SameLine();
			if (ImGui::Button("Ren") && selLayer != -1)
			{
				auto iter = eastl::find_if(project.layers.begin(), project.layers.end(), [this](const Project::Layer& layer) { return StringUtils::IsEqual(layer.name.c_str(), this->layerName.c_str()); });

				if (iter == project.layers.end())
				{
					project.layers[selLayer].name = layerName;
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Del"))
			{
				if (selLayer != -1)
				{
					project.layers.erase(project.layers.begin() + selLayer);
					selLayer = -1;
				}
			}

			ImGui::BeginChild("LayersList");

			ImGui::Columns(2, "Layers");

			if (layerStateNames.empty())
			{
				layerStateNames.push_back("Normal");
				layerStateNames.push_back("Unselectable");
				layerStateNames.push_back("Invisible");
			}

			for (int i = 0; i < project.layers.size(); i++)
			{
				auto& layer = project.layers[i];

				char label[32];
				sprintf(label, "%s###%04d", layer.name.c_str(), i);				

				if (ImGui::Selectable(label, selLayer == i, ImGuiSelectableFlags_None))
				{
					selLayer = i;
					layerName = layer.name;
				}

				ImGui::NextColumn();

				sprintf(label, "###%07d", i);

				int index = layer.state;

				if (ImGuiHelper::InputCombobox(label, index, layerStateNames, layerStateList))
				{
					layer.state = (Project::Layer::State)index;
				}
				
				ImGui::NextColumn();
			}

			ImGui::EndChild();

			ImGui::End();
		}

		if (selectedEditAsset && selectedEditAsset->ImGuiHasHierarchy())
		{
			ImGui::Begin("Hierarchy###Scene");

			selectedEditAsset->ImGuiHierarchy();

			ImGui::End();
		}

		{
			ImGui::Begin("Properties");

			ImGui::Columns(2);

			if (selectedEditAsset)
			{
				selectedEditAsset->ImGuiProperties();
			}

			ImGui::Columns(1);

			ImGui::End();
		}

		{
			ImGui::Begin("Console");

			if (ImGui::BeginTabBar("##Logs", ImGuiTabBarFlags_None))
			{
				for (auto& log : logCategories)
				{
					if (ImGui::BeginTabItem(log.first.c_str()))
					{
						ImGui::SetNextItemWidth(-1);

						ImVec2 size = ImGui::GetContentRegionAvail();

						if (ImGui::ListBoxHeader("##listbox2", ImVec2(-1, -1)))
						{
							for (int n = 0; n < log.second->logs.size(); n++)
							{
								if (ImGui::Selectable(log.second->logs[n].c_str(), log.second->selItem == n))
								{
									log.second->selItem = n;
								}
							}

							ImGui::ListBoxFooter();
						}

						ImGui::SetScrollHereY(1.0f);

						ImGui::EndTabItem();
					}
				}

				ImGui::EndTabBar();
			}

			ImGui::End();
		}

		{
			ImGui::Begin("Assets browser");

			ImVec2 size = ImGui::GetContentRegionAvail();

			ImGui::BeginChild("AssetsList", ImVec2(size.x - 300, 0), true);

			AssetsFolder(&root.assets.rootFolder);

			ImGui::EndChild();

			AssetsTreePopup(true);

			ImGui::SameLine();

			ImGui::BeginChild("AssetProperty", ImVec2(300, 0), true);

			ImGui::Columns(2);

			if (selectedAsset)
			{
				selectedAsset->ImGuiMetaProperties();
			}

			ImGui::Columns(1);

			ImGui::EndChild();

			ImGui::End();
		}

		ShowCreateAssetDialog();

		ShowAbout();

		ShowProjectSettings();

		ShowEditorSettings();

		ShowViewport();

		if (TileSetWindow::instance)
		{
			TileSetWindow::instance->ImGui();
		}

		if (SpriteWindow::instance)
		{
			SpriteWindow::instance->ImGui();
		}

		return true;
	}

	bool Editor::Update()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (projectToLoad)
		{
			LoadProject(projectToLoad);
		}

		if (project.projectFullName.c_str()[0])
		{
			if (!ShowEditor())
			{
				return false;
			}
		}
		else
		{
			ShowSelectProject();
		}

		ImGuiContext* context = ImGui::GetCurrentContext();

		if (context->DragDropActive)
		{
			ImGuiContext* context = ImGui::GetCurrentContext();

			ImVec2 p = ImGui::GetIO().MousePos;
			ImGui::GetForegroundDrawList(context->MouseLastHoveredViewport)->AddRectFilled(ImVec2(p.x + 12, p.y + 10), ImVec2(p.x + 25, p.y + 20), ImGui::GetColorU32(ImGuiCol_DragDropTarget));
		}

		ImGui::Render();
		d3dDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		d3dDeviceContext->ClearRenderTargetView(mainRenderTargetView, (float*)&clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		swapChain->Present(1, 0);

		return true;
	}

	void Editor::UpdateOrin()
	{
		MaximizeEditorWindow();

		root.CountDeltaTime();

		root.render.DebugPrintText(5.0f, ScreenCorner::RightTop, COLOR_WHITE, "%i", root.GetFPS());

		if (!projectRunning && freeCamera.mode2D)
		{
			Math::Vector3 step = (ownGrid) ? gridStep : gizmo.alignGrid;

			float minStep = 16.0f;

			if (freeCamera.mode2D && !ownGrid)
			{
				minStep = 16.0f / freeCamera.zoom2D;
			}

			while (step.x < minStep)
			{
				step *= 2.0f;
			}

			Math::Vector3 pos = ownGrid ? gridOrigin : (gizmo.transform ? gizmo.transform->GetGlobal().Pos() : 0.0f);

			if (freeCamera.mode2D)
			{
				pos.x = freeCamera.pos2D.x;
				pos.y = freeCamera.pos2D.y;
			}

			if (!ownGrid)
			{
				pos.x = step.x * (int)(pos.x / step.x);
				pos.y = step.y * (int)(pos.y / step.y);
			}
			else
			{
				pos.x = step.x * (int)((pos.x - gridOrigin.x) / step.x) + gridOrigin.x;
				pos.y = step.y * (int)((pos.y - gridOrigin.y) / step.y) + gridOrigin.y;
			}

			pos *= Sprite::ToUnits(1.0f);
			step *= Sprite::ToUnits(1.0f);

			Color color = COLOR_LIGHT_GRAY_A(0.5f);

			int numCellsY = 30;
			int numCellsX = 45;

			if (freeCamera.mode2D)
			{
				numCellsX = (int)(Sprite::GetPixelsHeight() * 0.5f * Sprite::ToUnits(1.0f) / root.render.GetDevice()->GetAspect() / freeCamera.zoom2D / step.y + 2);
				numCellsY = (int)(Sprite::GetPixelsHeight() * 0.5f * Sprite::ToUnits(1.0f) / freeCamera.zoom2D / step.y + 2);
			}
			
			for (int i = -numCellsY; i <= numCellsY; i++)
			{
				root.render.DebugLine(Math::Vector3(-numCellsX * step.x + pos.x, i * step.y + pos.y, pos.z), color, Math::Vector3(numCellsX * step.x + pos.x, i * step.y + pos.y, pos.z), color, false);
			}

			for (int i = -numCellsX; i <= numCellsX; i++)
			{
				root.render.DebugLine(Math::Vector3(i * step.x + pos.x, -numCellsY * step.y + pos.y, pos.z), color, Math::Vector3(i * step.x + pos.x, numCellsY * step.y + pos.y, pos.z), color, false);
			}
		}

		if (!projectRunning && (!selectedEditAsset || !selectedEditAsset->IsEditable()))
		{
			float width = Sprite::GetPixelsHeight() / root.render.GetDevice()->GetHeight() * root.render.GetDevice()->GetWidth();

			Math::Vector3 rect[] = { Math::Vector3(0, 0, 0), Math::Vector3(width, 0, 0),
									Math::Vector3(width, Sprite::GetPixelsHeight(), 0), Math::Vector3(0, Sprite::GetPixelsHeight(), 0) } ;

			for (int i = 0; i < 4; i++)
			{
				Math::Vector3 p1 = Sprite::ToUnits(rect[i]);
				Math::Vector3 p2 = Sprite::ToUnits(rect[(i + 1) % 4]);

				root.render.DebugLine(p1, COLOR_CYAN, p2, COLOR_CYAN, false);
			}
		}

		float dt = root.GetDeltaTime();

		if (projectRunning)
		{
			viewportFocused = project.runningInFullscreen ? true : viewportFocused = (GetFocus() == hwnd && ImGui::IsWindowFocused());
			root.controls.SetFocused(viewportFocused);
		}

		if (viewportFocused && !projectRunning)
		{
			if (project.runningInFullscreen)
			{
				editorDrawer.DrawWindowBorder();
			}

			if (root.controls.DebugHotKeyPressed("KEY_LCONTROL", "KEY_LSHIFT", "KEY_Z"))
			{
				RedoAction();
			}
			else
			if (root.controls.DebugHotKeyPressed("KEY_LCONTROL", "KEY_Z"))
			{
				UndoAction();
			}
		}

		if (!projectRunning)
		{
			FreeCamera::UpdateMode mode = FreeCamera::UpdateMode::OnlyTransform;

			if (viewportFocused)
			{
				mode = (editMode == EditMode::DragFiled) ? FreeCamera::UpdateMode::Drag : FreeCamera::UpdateMode::Normal;

				if (InSelectMode() && selectedEditAsset && selectedEditAsset->BlockMouseButtons())
				{
					mode = FreeCamera::UpdateMode::OnlyTransform;
				}
			}
				
			freeCamera.Update(dt, mode);

			gizmo.Render();

			if (root.controls.DebugHotKeyPressed("KEY_LCONTROL", "KEY_S"))
			{
				Save();
			}
		}

		if (!projectRunning && selectedEditAsset)
		{
			selectedEditAsset->Update(dt);
		}

		root.Update();
	}

	void Editor::Render(float dt)
	{
		root.render.GetDevice()->Clear(true, projectRunning ? COLOR_BLACK : COLOR_GRAY, true, 1.0f);

		if (!projectRunning && !freeCamera.mode2D)
		{
			editorDrawer.DrawSkyBox();
		}

		root.render.ExecutePool(-10, dt);
		root.render.ExecutePool(0, dt);
		root.render.ExecutePool(1, dt);
		root.render.ExecutePool(2, dt);
		root.render.ExecutePool(3, dt);
		root.render.ExecutePool(4, dt);
		root.render.ExecutePool(5, dt);
		root.render.ExecutePool(6, dt);
		root.render.ExecutePool(7, dt);
		root.render.ExecutePool(8, dt);
		root.render.ExecutePool(9, dt);
		root.render.ExecutePool(10, dt);
		root.render.ExecutePool(11, dt);
		root.render.ExecutePool(12, dt);

		root.render.ExecutePool(100, dt);
		root.render.ExecutePool(150, dt);
		root.render.ExecutePool(199, dt);
		root.render.ExecutePool(1000, dt);
		root.render.GetDevice()->Present();
	}

	bool Editor::ProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		{
			return true;
		}

		if (msg == WM_DPICHANGED)
		{
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
			{
				const RECT* suggested_rect = (RECT*)lParam;
				SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}

		return false;
	}

	void Editor::OnResize(int width, int height)
	{
		if (d3dDevice != nullptr)
		{
			CleanupRenderTarget();
			swapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
	}

	void Editor::Release()
	{
		SpriteWindow::Release();
		TileSetWindow::Release();

		for (int i = 0; i < actions.size(); i++)
		{
			RELEASE(actions[i]);
		}

		actions.clear();

		for (auto& entry : projects)
		{
			entry.icon.ReleaseRef();
		}

		for (auto& asset : openedAssets)
		{
			asset.ReleaseRef();
		}

		StopProject();

		for (auto& item : logCategories)
		{
			delete item.second;
		}

		logCategories.clear();

		editorDrawer.Release();

		projects.clear();

		draggedTextureAsset.ReleaseRef();
		draggedAssetAnimGraph2D.ReleaseRef();
		draggedAssetTileSet.ReleaseRef();
		draggeSpriteLayerSet.ReleaseRef();

		project.Reset();

		root.render.DelTaskPool(renderTaskPool);
		root.Release();

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
	}
}

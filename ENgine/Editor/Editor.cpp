#include "Editor.h"

#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "EditorDrawer.h"

#include "SceneEntities\TestEntity.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifdef OAK_EDITOR
extern const char* OpenFileDialog(const char* extName, const char* ext, bool open);
#endif

namespace Oak
{
	Editor editor;

	bool Editor::Init(HWND setHwnd)
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

		root.render.AddExecutedLevelPool(1);

		renderTaskPool = root.render.AddTaskPool(_FL_);
		renderTaskPool->AddTask(1, this, (Object::Delegate) & Editor::Render);

		if (!root.render.GetDevice()->SetVideoMode(800, 600, &hwnd))
		{
			return false;
		}

		freeCamera.Init();

		editorDrawer.Init();

		SetupImGUI();

		return true;
	}

	void Editor::SetupImGUI()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiIO& guiIO = ImGui::GetIO();
		ImFont* font = io.Fonts->AddFontFromFileTTF("ENgine/OpenSans-Regular.ttf", 18);
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

	void Editor::ShowViewport()
	{
		ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImGuiIO& io = ImGui::GetIO();

		Oak::root.controls.OverrideMousePos((int)io.MousePos.x, (int)io.MousePos.y);

		UpdateOak();

		ImVec2 viewportPos = ImVec2(io.MousePos.x - ImGui::GetCursorScreenPos().x, io.MousePos.y - ImGui::GetCursorScreenPos().y);

		ImVec2 size = ImGui::GetContentRegionAvail();

		if (size.x != Oak::root.render.GetDevice()->GetWidth() || size.y != Oak::root.render.GetDevice()->GetHeight())
		{
			Oak::root.render.GetDevice()->SetVideoMode((int)size.x, (int)size.y, &hwnd);
		}

		ImGui::Image(Oak::root.render.GetDevice()->GetBackBuffer(), size);

		vireportHowered = ImGui::IsItemHovered();

		if (vireportHowered && ImGui::IsMouseDown(0))
		{
			gizmo.OnLeftMouseDown();
			viewportCaptured = true;
		}

		gizmo.OnMouseMove(Math::Vector2((float)viewportPos.x, (float)viewportPos.y));

		if (viewportCaptured && ImGui::IsMouseReleased(0))
		{
			gizmo.OnLeftMouseUp();
			viewportCaptured = false;
		}


		ImGui::End();
	}

	void Editor::ShowAbout()
	{
		if (!showAbout)
		{
			return;
		}

		ImGui::Begin("About", &showAbout, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize(ImVec2(300.0f, 200.0f));
		ImGui::Text("Oak is awesome!!!");

		if (ImGui::Button("Ok", ImVec2(40, 30)))
		{
			showAbout = false;
		}

		ImGui::End();
	}

	void Editor::SelectEntity(SceneEntity* entity)
	{
		if (entity != nullptr && selAsset.Get())
		{
			selAsset.ReleaseRef();
		}

		if (selectedEntity)
		{
			selectedEntity->SetEditMode(false);
			gizmo.Disable();
		}

		selectedEntity = entity;

		if (selectedEntity)
		{
			selectedEntity->SetEditMode(true);

			auto* transform = selectedEntity->GetTransform();

			if (transform)
			{
				gizmo.SetTransform(selectedEntity->GetTransform());

				if (!transform->unitsInvScale && gizmo.mode == TransformMode::Rectangle)
				{
					gizmo.mode = TransformMode::Move;
				}
				else
				if (transform->unitsInvScale)
				{
					gizmo.mode = TransformMode::Rectangle;
				}
			}
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

	void Editor::ProjectTreePopup(bool contextItem)
	{
		if (projectTreePopup)
		{
			return;
		}

		if ((contextItem && ImGui::BeginPopupContextItem("CreateScene")) ||
			(!contextItem && ImGui::BeginPopupContextWindow("CreateScene")))
		{
			projectTreePopup = true;

			if (ImGui::MenuItem("Add new scene"))
			{
				const char* fileName = OpenFileDialog("Scene file", "sca", false);

				if (fileName)
				{
					project.AddScene(fileName);
				}
			}

			if (ImGui::MenuItem("Add scene"))
			{
				const char* fileName = OpenFileDialog("Scene file", "sca", true);

				if (fileName)
				{
					project.AddScene(fileName);
				}
			}

			if (project.selectedScene && ImGui::MenuItem("Mark as start scne"))
			{
				project.SetStartScene(project.selectedScene->path.c_str());
			}

			if (project.selectedScene && ImGui::MenuItem("Delete"))
			{
				auto* scene = project.selectedScene;
				project.SelectScene(nullptr);
				project.DeleteScene(scene);
			}

			ImGui::EndPopup();
		}
	}

	void Editor::SceneTreePopup(bool contextItem)
	{
		if (sceneTreePopup)
		{
			return;
		}

		if ((contextItem && ImGui::BeginPopupContextItem("CreateEntity")) ||
			(!contextItem && ImGui::BeginPopupContextWindow("CreateEntity")))
		{
			sceneTreePopup = true;

			if (ImGui::BeginMenu("Create Entity"))
			{
				auto& decls = ClassFactorySceneEntity::Decls();

				for (auto& decl : decls)
				{
					if (ImGui::MenuItem(decl->GetShortName()))
					{
						auto* entity = project.selectedScene->scene->CreateEntity(decl->GetName());

						if (selectedEntity)
						{
							auto* parent = selectedEntity->GetParent();

							if (parent)
							{
								if (!entity->GetTransform())
								{
									RELEASE(entity)
								}
								else
								{
									entity->SetParent(parent, selectedEntity);
								}
							}
							else
							{
								project.selectedScene->scene->AddEntity(entity, selectedEntity);
							}
						}
						else
						{
							project.selectedScene->scene->AddEntity(entity);
						}

						if (entity)
						{
							auto* transform = entity->GetTransform();

							if (transform)
							{
								transform->local.Pos() = freeCamera.pos + Math::Vector3(cosf(freeCamera.angles.x), sinf(freeCamera.angles.y), sinf(freeCamera.angles.x)) * 5.0f;
							}

							SelectEntity(entity);
						}
					}
				}

				ImGui::EndMenu();
			}

			if (selectedEntity && ImGui::MenuItem("Duplicate"))
			{
				SceneEntity* copy = project.selectedScene->scene->CreateEntity(selectedEntity->className);

				auto* parent = selectedEntity->GetParent();

				if (parent)
				{
					copy->SetParent(parent, selectedEntity);
				}
				else
				{
					project.selectedScene->scene->AddEntity(copy, selectedEntity);
				}

				copy->Copy(selectedEntity);

				SelectEntity(copy);

				eastl::string name = selectedEntity->GetName();
				name += "_copy";

				copy->SetName(name.c_str());
			}

			if (selectedEntity && ImGui::MenuItem("Delete"))
			{
				SceneEntity* entity = selectedEntity;
				SelectEntity(nullptr);
				project.selectedScene->scene->DeleteEntity(entity, true);
			}

			ImGui::EndPopup();
		}
	}

	void Editor::EntitiesTreeView(const eastl::vector<SceneEntity*>& entities)
	{
		ImGuiContext* context = ImGui::GetCurrentContext();
		ImGuiWindow* window = context->CurrentWindow;

		if (entities.size() > 0)
		{
			for (int i = 0; i < entities.size(); i++)
			{
				SceneEntity* entity = entities[i];

				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

				if (entity->GetChilds().size() > 0)
				{
					nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
				}

				if (selectedEntity == entity)
				{
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				}

				bool open = ImGui::TreeNodeEx(entity, nodeFlags, entity->GetName()[0] == 0 ? "[Name not set]" : entity->GetName());

				if (ImGui::IsItemHovered() && (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
				{
					SelectEntity(entity);
				}

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					ImGui::SetDragDropPayload("_TREENODE", &entity, sizeof(SceneEntity*));
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginDragDropTarget())
				{
					auto rect = window->DC.LastItemRect;
					bool asChild = false;

					if (context->IO.MousePos.y > (rect.Min.y + rect.Max.y) * 0.5f)
					{
						rect.Min.x += 20;
						rect.Min.y = (rect.Min.y + rect.Max.y) * 0.5f;
						asChild = true;
					}

					SceneEntity* transformEntity = entity;

					if (!asChild)
					{
						transformEntity = transformEntity->GetParent();
					}

					ImGuiPayload& payload = context->DragDropPayload;
					bool dragFinished = false;

					if (payload.IsDataType("_TREENODE"))
					{
						uint64_t temp = *((uint64_t*)payload.Data);
						SceneEntity* dragged = (SceneEntity*)temp;					

						bool transformMatches = transformEntity ? (transformEntity->GetTransform() && !dragged->GetTransform()) : true;

						if (!dragged->ContainEntity(entity) && transformMatches)
						{
							if (ImGui::AcceptDragDropPayload("_TREENODE", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
							{
								if (dragged->GetParent())
								{
									dragged->SetParent(nullptr);
								}
								else
								{
									project.selectedScene->scene->DeleteEntity(dragged, false);
								}

								if (asChild)
								{
									dragged->SetParent(entity);
								}
								else
								{
									SceneEntity* parent = entity->GetParent();

									if (parent)
									{
										dragged->SetParent(parent, entity);
									}
									else
									{
										project.selectedScene->scene->AddEntity(dragged);
									}
								}

								dragFinished = true;
							}
						}
					}
					else
					if (payload.IsDataType("_ASSET_TEX"))
					{
						Assets::AssetRef* assetRef = reinterpret_cast<Assets::AssetRef**>(payload.Data)[0];
						auto textureRef = assetRef->GetAsset<AssetTexture>();

						if (ImGui::AcceptDragDropPayload("_ASSET_TEX", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
						{
							auto* assetEntity = project.selectedScene->scene->CreateEntity(textureRef->GetSceneEntityType());

							if (assetEntity)
							{
								textureRef->SetupCreatedSceneEntity(assetEntity);
								assetEntity->ApplyProperties();

								if (asChild)
								{
									assetEntity->SetParent(entity);
								}
								else
								{
									SceneEntity* parent = entity->GetParent();

									if (parent)
									{
										assetEntity->SetParent(parent, entity);
									}
									else
									{
										project.selectedScene->scene->AddEntity(assetEntity);
									}
								}

								SelectEntity(entity);

								dragFinished = true;
							}
						}
					}

					if (dragFinished)
					{
						ImGui::EndDragDropTarget();
					}
					else
					{
						window->DrawList->AddRect(rect.Min, rect.Max /*- ImVec2(10.0f, 10.0f)*/, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 0.0f, ~0, 2.0f);
					}
				}

				SceneTreePopup(true);

				if (open)
				{
					EntitiesTreeView(entity->GetChilds());
					ImGui::TreePop();
				}
			}
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

			if (item == root.assets.selFolder)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}

			bool open = ImGui::TreeNodeEx(&item, nodeFlags, item->name.c_str());

			if (ImGui::IsItemHovered() && (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
			{
				root.assets.selFolder = item;
				root.assets.selAsset = nullptr;
				selAsset.ReleaseRef();
			}

			if (open)
			{
				AssetsFolder(item);

				ImGui::TreePop();
			}
		}

		for (auto* item : folder->assets)
		{
			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

			if (item == root.assets.selAsset)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}

			bool open = ImGui::TreeNodeEx(item, nodeFlags, item->name.c_str());

			if (ImGui::IsItemHovered() && (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
			{
				root.assets.selFolder = nullptr;
				root.assets.selAsset = item;
				selAsset = item->GetAsset<AssetTexture>();

				SelectEntity(nullptr);
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				Assets::AssetRef* ptr = item;
				ImGui::SetDragDropPayload("_ASSET_TEX", &ptr, sizeof(Assets::AssetRef*));
				ImGui::EndDragDropSource();
			}

			if (open)
			{
				ImGui::TreePop();
			}
		}
	}

	bool Editor::Update()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
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
			ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.2f, nullptr, &dock_main_id);
			ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);

			ImGui::DockBuilderDockWindow("Toolbar", dock_top_id);
			ImGui::DockBuilderDockWindow("Project", dock_left_id);
			ImGui::DockBuilderDockWindow("Scene", dock_left_id);
			ImGui::DockBuilderDockWindow("Game", dock_main_id);
			ImGui::DockBuilderDockWindow("Properties", dock_right_id);
			ImGui::DockBuilderDockWindow("Assets", dock_bottom_id);
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
				if (ImGui::MenuItem("New"))
				{
					project.Reset();
				}

				if (ImGui::MenuItem("Load"))
				{
					const char* fileName = OpenFileDialog("Project file", "pra", true);

					if (fileName)
					{
						project.Load(fileName);
					}
				}

				bool saveProjectAs = false;;

				if (ImGui::MenuItem("Save"))
				{
					if (project.projectName.empty())
					{
						saveProjectAs = true;
					}
					else
					{
						project.Save();
					}
				}

				if (ImGui::MenuItem("Save as"))
				{
					saveProjectAs = true;
				}

				if (saveProjectAs)
				{
					const char* fileName = OpenFileDialog("Project file", "pra", false);

					if (fileName)
					{
						project.Save(fileName);
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
				{
					return false;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
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
		sceneTreePopup = false;

		{
			ImGui::Begin("Toolbar");

			PushButton("2D", freeCamera.mode_2d, [this]() {freeCamera.mode_2d = true; });
			PushButton("3D", !freeCamera.mode_2d, [this]() {freeCamera.mode_2d = false; });

			if (freeCamera.mode_2d)
			{
				ImGui::Text("Zoom");
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

			if (gizmo.IsEnabled())
			{
				auto* transform = selectedEntity ? selectedEntity->GetTransform() : nullptr;

				PushButton("Move", gizmo.mode == TransformMode::Move, [this]() {gizmo.mode = TransformMode::Move; });
				PushButton("Rotate", gizmo.mode == TransformMode::Rotate, [this, transform]() { if (!transform || !transform->unitsInvScale) gizmo.mode = TransformMode::Rotate; });
				PushButton("Scale", gizmo.mode == TransformMode::Scale, [this, transform]() { if (!transform || !transform->unitsInvScale)gizmo.mode = TransformMode::Scale; });
				PushButton("Rect", gizmo.mode == TransformMode::Rectangle, [this, transform]() { if (!transform || transform->unitsInvScale) gizmo.mode = TransformMode::Rectangle; });

				if (ImGui::Button(gizmo.useLocalSpace ? "Local" : "Global", ImVec2(50.0f, 25.0f)))
				{
					gizmo.useLocalSpace = !gizmo.useLocalSpace;
				}

				ImGui::SameLine();

				ImGui::Separator();
				ImGui::SameLine();

				if (ImGui::Button("To Object", ImVec2(75.0f, 25.0f)) && transform)
				{
					freeCamera.pos = transform->global.Pos() - Math::Vector3(cosf(freeCamera.angles.x), sinf(freeCamera.angles.y), sinf(freeCamera.angles.x)) * 5.0f;
				}

				ImGui::SameLine();

				if (ImGui::Button("To Camera", ImVec2(75.0f, 25.0f)) && transform)
				{
					transform->position = freeCamera.pos + Math::Vector3(cosf(freeCamera.angles.x), sinf(freeCamera.angles.y), sinf(freeCamera.angles.x)) * 5.0f;
				}

				ImGui::SameLine();

				ImGui::SameLine();

				ImGui::Separator();
				ImGui::SameLine();

				if (gizmo.mode == TransformMode::Rectangle)
				{
					PushButton("Snap", gizmo.useAlignRect, [this]() { gizmo.useAlignRect = !gizmo.useAlignRect; });

					ImGui::Text("SnapX");
					ImGui::SameLine();

					ImGui::SetNextItemWidth(100.0f);
					int value = (int)gizmo.alignRect.x;
					ImGui::InputInt("##SnapXID", &value);
					if (value < 2) value = 2;
					gizmo.alignRect.x = (float)value;
					ImGui::SameLine();

					ImGui::Text("SnapY");
					ImGui::SameLine();

					ImGui::SetNextItemWidth(100.0f);
					value = (int)gizmo.alignRect.y;
					ImGui::InputInt("##SnapYID", &value, ImGuiInputTextFlags_CharsDecimal);
					if (value < 2) value = 2;
					gizmo.alignRect.y = (float)value;
					ImGui::SameLine();
				}
			}

			ImGui::End();
		}

		{
			ImGui::Begin("Project");

			for (int i = 0; i < project.scenes.size(); i++)
			{
				auto* scene = project.scenes[i];

				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

				if (project.selectedScene == scene)
				{
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}

				ImGui::TreeNodeEx(scene, node_flags, (project.startScene == i) ? StringUtils::PrintTemp("%s (Start)", scene->name.c_str()) : scene->name.c_str());

				if (ImGui::IsItemClicked() || (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
				{
					project.SelectScene(scene);
				}

				ProjectTreePopup(true);
			}

			ProjectTreePopup(false);

			ImGui::End();
		}

		{
			ImGui::Begin("Scene");

			if (project.selectedScene)
			{
				EntitiesTreeView(project.selectedScene->scene->GetEntities());

				SceneTreePopup(false);
			}

			ImGui::End();
		}

		{
			ImGui::Begin("Properties");

			ImGui::Columns(2);

			if (selectedEntity)
			{
				selectedEntity->GetMetaData()->Prepare(selectedEntity);
				selectedEntity->GetMetaData()->ImGuiWidgets();
			}

			if (selAsset.Get())
			{
				selAsset->GetMetaData()->Prepare(selAsset.Get());
				selAsset->GetMetaData()->ImGuiWidgets();
				if (selAsset->GetMetaData()->IsValueWasChanged())
				{
					selAsset->Reload();
					selAsset->SaveMetaData();
				}
			}

			ImGui::Columns(1);

			ImGui::End();
		}

		{
			ImGui::Begin("Assets");

			AssetsFolder(&root.assets.rootFolder);

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

						ImGui::ListBox("##listbox2", &log.second->selItem, &log.second->logsPtr[0], (int)log.second->logs.size(), (int)(size.y / 23));

						ImGui::SetScrollHereY(1.0f);

						ImGui::EndTabItem();
					}
				}

				ImGui::EndTabBar();
			}

			ImGui::End();
		}

		ShowAbout();
		ShowViewport();

		ImGui::Render();
		d3dDeviceContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		d3dDeviceContext->ClearRenderTargetView(mainRenderTargetView, (float*)&clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		swapChain->Present(0, 0);

		return true;
	}

	void Editor::UpdateOak()
	{
		root.CountDeltaTime();

		root.render.DebugPrintText(5.0f, ScreenCorner::RightTop, COLOR_WHITE, "%i", root.GetFPS());

		if (gizmo.mode == TransformMode::Rectangle && gizmo.useAlignRect && selectedEntity)
		{
			Math::Vector2 step = gizmo.alignRect;

			float minStep = 16.0f;

			if (freeCamera.mode_2d)
			{
				minStep = 16.0f / freeCamera.zoom2D;
			}
			
			while (step.x < minStep)
			{
				step *= 2.0f;
			}

			Math::Vector3 pos = gizmo.transform->global.Pos();

			if (freeCamera.mode_2d)
			{
				pos.x = freeCamera.pos2D.x * (*gizmo.transform->unitsScale);
				pos.y = freeCamera.pos2D.y * (*gizmo.transform->unitsScale);
			}
			
			pos.x = step.x * ((int)(pos.x / step.x));
			pos.y = step.y * ((int)(pos.y / step.y));

			pos *= (*gizmo.transform->unitsInvScale);
			step *= (*gizmo.transform->unitsInvScale);

			Color color = COLOR_LIGHT_GRAY_A(0.5f);

			int numCellsY = 30;
			int numCellsX = 45;

			if (freeCamera.mode_2d)
			{
				numCellsX = Sprite::pixelsHeight * 0.5f * Sprite::pixelsPerUnitInvert / root.render.GetDevice()->GetAspect() / freeCamera.zoom2D / step.y + 2;
				numCellsY = Sprite::pixelsHeight * 0.5f * Sprite::pixelsPerUnitInvert / freeCamera.zoom2D / step.y + 2;
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

		float dt = root.GetDeltaTime();

		root.controls.SetFocused(GetActiveWindow() == hwnd && ImGui::IsWindowFocused());

		freeCamera.Update(dt);

		gizmo.Render();

		root.Update();

		if (project.selectedScene)
		{
			if (selectedEntity && selectedEntity->GetMetaData()->IsValueWasChanged())
			{
				selectedEntity->ApplyProperties();
			}

			if (selectedEntity && selectedEntity->HasOwnTasks())
			{
				project.EnableScene(project.selectedScene, false);
				selectedEntity->Tasks(true)->Execute(dt);
			}
			else
			{
				project.EnableScene(project.selectedScene, true);
				project.selectedScene->scene->Execute(dt);
			}
		}
	}

	void Editor::Render(float dt)
	{
		root.render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);
		root.render.ExecutePool(0, dt);
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
		for (auto& item : logCategories)
		{
			delete item.second;
		}

		logCategories.clear();

		editorDrawer.Release();

		project.Reset();

		root.render.DelTaskPool(renderTaskPool);
		root.Release();

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
	}
}

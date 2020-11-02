#include "Editor.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "SceneEntities\TestEntity.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Oak
{
	Scene scene;

	void AddEntity()
	{
		TestEntity * entity = (TestEntity*)scene.CreateEntity("TestEntity");

		int count = Math::RandRange(1.0f, 5.0f);

		for (int i = 0; i < count; i++)
		{
			TestItem item;
			item.boolProp = true;
			item.intProp = Math::RandRange(0.0f, 100.0f);
			item.floatProp = Math::RandRange(0.0f, 100.0f);

			entity->itemsProp.push_back(item);
		}

		entity->intProp = Math::RandRange(0.0f, 100.0f);
		entity->floatProp = Math::RandRange(0.0f, 100.0f);
		entity->floatProp2 = Math::RandRange(0.0f, 100.0f);
		entity->SetName(StringUtils::PrintTemp("TestEntity%i", scene.GetEntityCount()));
	}

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

		SetupImGUI();

		scene.Init();

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

		ImVec2 size = ImGui::GetWindowContentRegionMax();
		size.x = ImGui::GetWindowContentRegionWidth();
		size.y += 4;

		if (size.x != Oak::root.render.GetDevice()->GetWidth() || size.y != Oak::root.render.GetDevice()->GetHeight())
		{
			Oak::root.render.GetDevice()->SetVideoMode((int)size.x, (int)size.y, &hwnd);
		}

		ImGuiIO& io = ImGui::GetIO();

		Oak::root.controls.OverrideMousePos((int)io.MousePos.x, (int)io.MousePos.y);

		UpdateOak();

		ImGui::Image(Oak::root.render.GetDevice()->GetBackBuffer(), ImGui::GetContentRegionAvail());

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
		if (selectedEntity)
		{
			selectedEntity->SetEditMode(false);
		}

		selectedEntity = entity;

		if (selectedEntity)
		{
			selectedEntity->SetEditMode(true);
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
						AddEntity();
					}
				}

				ImGui::EndMenu();
			}

			if (selectedEntity && ImGui::MenuItem("Duplicate"))
			{
				SceneEntity* copy = scene.CreateEntity(selectedEntity->className);
				scene.GenerateUID(copy);
				copy->Copy(selectedEntity);

				eastl::string name = selectedEntity->GetName();
				name += "_copy";

				copy->SetName(name.c_str());
			}

			if (selectedEntity && ImGui::MenuItem("Delete"))
			{
				SceneEntity* entity = selectedEntity;
				SelectEntity(nullptr);
				scene.DeleteObject(entity, true);
			}

			ImGui::EndPopup();
		}
	}

	void Editor::Update()
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
			ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
			ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);

			ImGui::DockBuilderDockWindow("Scene", dock_left_id);
			ImGui::DockBuilderDockWindow("Game", dock_main_id);
			ImGui::DockBuilderDockWindow("Properties", dock_right_id);

			ImGui::DockBuilderFinish(dock_main_id);
		}
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags | ImGuiDockNodeFlags_NoCloseButton);


		if (ImGui::BeginMenuBar())
		{
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

		sceneTreePopup = false;

		{
			ImGui::Begin("Scene");

			if (scene.GetEntityCount() > 0)
			{
				for (int i = 0; i < scene.GetEntityCount(); i++)
				{
					SceneEntity* entity = scene.GetEntity(i);

					ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

					if (selectedEntity == entity)
					{
						node_flags |= ImGuiTreeNodeFlags_Selected;
					}

					ImGui::TreeNodeEx(entity, node_flags, entity->GetName());

					if (ImGui::IsItemClicked() || (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
					{
						selectedEntity = entity;
					}

					SceneTreePopup(true);

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_TREENODE", nullptr, 0);
						ImGui::Text("This is a drag and drop source");
						ImGui::EndDragDropSource();
					}
				}
			}

			SceneTreePopup(false);

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

			ImGui::Columns(1);

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
	}

	void Editor::UpdateOak()
	{
		root.CountDeltaTime();

		root.render.DebugPrintText(5.0f, ScreenCorner::RightTop, COLOR_WHITE, "%i", root.GetFPS());

		for (int i = 0; i <= 20; i++)
		{
			float pos = (float)i - 10.0f;

			root.render.DebugLine(Math::Vector3(pos, 0.0f, -10.0f), COLOR_WHITE, Math::Vector3(pos, 0.0f, 10.0f), COLOR_WHITE);
			root.render.DebugLine(Math::Vector3(-10.0f, 0.0f, pos), COLOR_WHITE, Math::Vector3(10.0f, 0.0f, pos), COLOR_WHITE);
		}

		float dt = root.GetDeltaTime();

		root.controls.SetFocused(GetActiveWindow() == hwnd && ImGui::IsWindowFocused());

		freeCamera.Update(dt);

		root.Update();
	}

	void Editor::Render(float dt)
	{
		root.render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);
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
		root.render.DelTaskPool(renderTaskPool);
		root.Release();

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
	}
}

#pragma once

#include "Root/Root.h"
#include "FreeCamera.h"
#include "Project.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>

/**
\ingroup gr_code_editor
*/

namespace Oak
{
	class Editor : Object
	{
		friend class Project;

		HWND hwnd;
		ID3D11Device* d3dDevice = nullptr;
		ID3D11DeviceContext* d3dDeviceContext = nullptr;
		IDXGISwapChain* swapChain = nullptr;
		ID3D11RenderTargetView* mainRenderTargetView = nullptr;

		TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;
		FreeCamera freeCamera;

		bool showAbout = false;
		SceneEntity* selectedEntity = nullptr;
		bool projectTreePopup = false;
		bool sceneTreePopup = false; 
		bool viewportCaptured = false;

	public:

		Project project;

		bool Init(HWND hwnd);
		bool Update();
		void Render(float dt);
		bool ProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void OnResize(int width, int height);
		void Release();

	private:

		void SetupImGUI();
		void ShowAbout();
		void ShowViewport();
		void ProjectTreePopup(bool contextItem);
		void SceneTreePopup(bool contextItem);

		void SelectEntity(SceneEntity* entity);

		bool CreateDeviceD3D();
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();

		void UpdateOak();
	};

	extern Editor editor;
}

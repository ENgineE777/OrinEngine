#pragma once

#include "Root/Root.h"
#include "FreeCamera.h"
#include "Project.h"
#include "Gizmo.h"

#include "imgui.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include "eastl/bonus/ring_buffer.h"

/**
\ingroup gr_code_editor
*/

namespace Oak
{
	class Editor : Object
	{
		friend class Project;
		friend class FreeCamera;
		friend class Root;

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
		bool vireportHowered = false;

		struct LogCategory
		{
			int selItem = -1;
			eastl::ring_buffer<eastl::string, eastl::vector<eastl::string>> logs;
			eastl::vector<const char*> logsPtr;

			LogCategory() : logs(128)
			{
				logsPtr.reserve(128);
			}
		};

		eastl::map<eastl::string, LogCategory*> logCategories;

		void CaptureLog(const char* name, const char* text);

	public:

		Project project;
		Gizmo gizmo;

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
		void EntitiesTreeView(const eastl::vector<SceneEntity*>& entities);

		template<typename Func>
		void PushButton(const char* label, bool pushed, Func callback)
		{
			float b = 1.0f;
			float c = 0.5f;
			int i = 4;
			bool needPopStyle = false;

			if (pushed)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, b, b));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, b, b));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, c, c));
				needPopStyle = true;
			}

			if (ImGui::Button(label, ImVec2(50.0f, 25.0f)))
			{
				callback();
			}

			if (needPopStyle)
			{
				ImGui::PopStyleColor(3);
				needPopStyle = false;
			}

			ImGui::SameLine();
		}

		void SelectEntity(SceneEntity* entity);

		bool CreateDeviceD3D();
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();

		void UpdateOak();
	};

	extern Editor editor;
}

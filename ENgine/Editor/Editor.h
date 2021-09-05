#pragma once

#include "IEditor.h"
#include "Root/Root.h"
#include "FreeCamera.h"
#include "Project.h"
#include "Gizmo.h"

#include "imgui.h"

#include <d3d11.h>
#include "eastl/bonus/ring_buffer.h"
#include "Root/Assets/Assets.h"

/**
\ingroup gr_code_editor
*/

namespace Oak
{
	class Editor : public IEditor
	{
		friend class Project;
		friend class FreeCamera;
		friend class EditorDrawer;
		friend class Root;

		struct ProjectEntry
		{
			eastl::string name;
			eastl::string path;
			TextureRef icon;
		};

		eastl::vector<ProjectEntry> projects;

		HWND hwnd;
		ID3D11Device* d3dDevice = nullptr;
		ID3D11DeviceContext* d3dDeviceContext = nullptr;
		IDXGISwapChain* swapChain = nullptr;
		ID3D11RenderTargetView* mainRenderTargetView = nullptr;

		TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;	

		bool showAbout = false;
		bool showProjectSettings = false;
		bool showEditorSettings = false;

		Assets::Folder* selectedFolder = nullptr;
		Assets::AssetHolder* selectedAssetHolder = nullptr;
		Asset* selectedAsset;

		AssetTextureRef draggedTextureAsset;
		AssetAnimGraph2DRef draggedAssetAnimGraph2D;

		bool projectTreePopup = false;
		bool sceneTreePopup = false; 
		bool viewportCaptured = false;
		bool vireportHowered = false;
		bool entityDeletedViaPopup = false;

		bool projectRunning = false;
		bool allowSceneDropTraget = true;

		bool needSetSizePrjSet = true; 
		bool needSetSizeEdSet = true;

		eastl::string themeList;
		eastl::vector<eastl::string> themes;
		int selectedTheme = 0;
		eastl::string selectedThemeName = "orange";

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
		void LoadPorjectIcon(ProjectEntry& entry);

	public:

		SceneEntity* selectedEntity = nullptr;
		Project project;
		FreeCamera freeCamera;
		Gizmo gizmo;
		bool viewportFocused = false;

		const char* OpenFileDialog(const char* extName, const char* ext, bool open);

		bool Init(HWND hwnd) override;
		bool Update() override;
		void Render(float dt);
		bool ProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
		void OnResize(int width, int height) override;

		void SelectEntity(SceneEntity* entity);

		void Release() override;

		void ApplySelecetedTheme();

		void CopyChilds(SceneEntity* entity, SceneEntity* copy);

	private:

		void SetupImGUI();
		bool ShowEditor();
		void ShowAbout();
		void ShowProjectSettings();
		void ShowEditorSettings();
		void ShowViewport();
		void ShowSelectProject();
		void ProjectTreePopup(bool contextItem);
		void SceneTreePopup(bool contextItem);
		void SceneDropTraget(SceneEntity* entity);
		void EntitiesTreeView(const eastl::vector<SceneEntity*>& entities);
		void AssetsFolder(Assets::Folder* folder);

		void StartProject();
		void StopProject();

		void SaveProjectsList();

		void LoadSettings();
		void SaveSettings();

		template<typename Func>
		void PushButton(const char* label, bool pushed, Func callback)
		{
			bool needPopStyle = false;

			if (pushed)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
				needPopStyle = true;
			}

			if (ImGui::Button(label, ImVec2(50.0f, 25.0f)))
			{
				callback();
			}

			if (needPopStyle)
			{
				ImGui::PopStyleColor(1);
				needPopStyle = false;
			}

			ImGui::SameLine();
		}

		bool CreateDeviceD3D();
		void CleanupDeviceD3D();
		void CreateRenderTarget();
		void CleanupRenderTarget();

		void UpdateOak();
	};

	#ifdef OAK_EXPORTING
	extern CLASS_DECLSPEC Editor editor;
	#endif
}

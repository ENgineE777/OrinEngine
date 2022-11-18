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
		AssetRef selectedAsset;

		int forceImGUISetViewportFocusCount = 0;
		eastl::vector<AssetRef> openedAssets;

		AssetTextureRef draggedTextureAsset;
		AssetAnimGraph2DRef draggedAssetAnimGraph2D;
		AssetTileSetRef draggedAssetTileSet;
		AssetSpritesLayerRef draggeSpriteLayerSet;

		enum class AssetDialog
		{
			Inactive,
			Folder,
			Scene,
			AnimGraph2D,
			TileSet,
			Prefab,
			SpritesLayer
		};

		enum class ViewportCature
		{
			None,
			LeftButton,
			RightButton,
			MiddleButton
		};

		bool projectTreePopup = false;
		bool assetsTreePopup = false;
		bool need2openAssetPopup = false;
		bool duplicateAsset = false;
		eastl::string createAssetName;
		eastl::string createPrefabType;
		eastl::vector<eastl::string> createPrefabTypesList;
		int createPrefabTypesIndex;
		eastl::string createPrefabTypesListFlat;
		AssetDialog assetDialog = AssetDialog::Inactive;
		ViewportCature viewportCaptured = ViewportCature::None;
		bool vireportHowered = false;

		bool projectRunning = false;

		const char* projectToLoad = nullptr;

		bool needSetSizePrjSet = true; 
		bool needSetSizeEdSet = true;

		bool selectMode = false;

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
		void LoadProjectIcon(ProjectEntry& entry);

		int selLayer = -1;
		eastl::string layerName;
		eastl::vector<eastl::string> layerStateNames;
		eastl::string layerStateList;

	public:

		Project project;
		FreeCamera freeCamera;
		Gizmo gizmo;
		bool viewportFocused = false;

		AssetRef selectedEditAsset;

		bool ownGrid = false;
		Math::Vector3 gridOrigin = 0.0f;
		Math::Vector2 gridStep = 0.0f;

		const char* OpenFileDialog(const char* extName, const char* ext, bool open);

		bool Init(HWND hwnd, const char* startProject) override;
		bool Update() override;
		void Render(float dt);
		bool ProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
		void OnResize(int width, int height) override;

		void SelectEditAsset(AssetRef asset);

		void Release() override;

		void ApplySelecetedTheme();

	private:

		void SetupImGUI();
		bool ShowEditor();
		void ShowCreateAssetDialog();
		void ShowAbout();
		void ShowProjectSettings();
		void ShowEditorSettings();
		void ShowViewport();
		void ShowSelectProject();
		void LoadProject(const char* path);

		void AssetsTreePopup(bool contextItem);
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

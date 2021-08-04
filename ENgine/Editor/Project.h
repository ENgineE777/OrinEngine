
#pragma once

#include "Support/Support.h"
#include "Root/Scenes/Scene.h"
#include "Root/Scenes/SceneEntity.h"

/**
\ingroup gr_code_editor
*/

namespace Oak
{
	class Project
	{
	public:

		struct SceneHolder
		{
			eastl::string path;
			eastl::string name;

			uint16_t uid = 0;
			Scene* scene = nullptr;

			uint32_t selectedEntity = -1;

			bool camera2DMode = false;

			Math::Vector2 camera3DAngles = Math::Vector2(0.0f, -0.5f);
			Math::Vector3 camera3DPos = Math::Vector3(0.0f, 6.0f, 0.0f);

			Math::Vector2 camera2DPos = 0.0;
			float camera2DZoom = 1.0f;

			Math::Vector2 gizmoAlign2D = 0.0f;

			void SetPath(const char* set_path)
			{
				path = set_path;

				char setName[256];
				StringUtils::GetFileName(path.c_str(), setName);
				StringUtils::RemoveExtension(setName);

				name = setName;
			}
		};

		SceneHolder* selectedScene = nullptr;

		eastl::string projectFullName;
		eastl::string projectName;
		char projectPath[1024];
		char applicationDir[512];
		eastl::string exportDir;

		eastl::string iconPath;
		TextureRef icon;

		eastl::string iconSmallPath;
		TextureRef iconSmall;

		int startScene = -1;

		Math::Vector2 alignRect = 32.0f;
		bool useAlignRect = false;

		eastl::vector<SceneHolder*> scenes;


		bool CanRun();
		void Load(const char* fileName);
		void LoadScene(SceneHolder* holder);

		void Save(const char* fileName);
		void Save();

		void FillSelectedObject(SceneHolder* holder);

		void SetStartScene(const char* path);
		void SelectScene(SceneHolder* holder);
		int  FindSceneIndex(const char* path);
		Scene* GetScene(const char* path);
		void AddScene(const char* path);
		void DeleteScene(SceneHolder* holder);

		void EnableScene(SceneHolder* holder, bool enable);

		void GenerateUID(SceneHolder* holder);
		void Reset();

		void SelectExportDir();
		void UpdateExeStrings(HANDLE res);
		void UpdateIconsSet(HANDLE resource, const char* path, bool smallIcon, int idOffest);
		void Export();
		void CpyFolder(const char* src, const char* dest = nullptr);
		void CpyFile(const char* src, const char* dest = nullptr);

		void SaveCameraPos(SceneHolder* holder);
	};
}

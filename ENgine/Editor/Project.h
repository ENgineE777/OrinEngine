
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

			void SetPath(const char* set_path)
			{
				path = set_path;

				char setName[256];
				StringUtils::GetFileName(path.c_str(), setName);
				StringUtils::RemoveExtension(setName);

				name = setName;
			}
		};

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

		void Save(const char* fileName);
		void Save();

		void SetStartScene(const char* path);
		bool IsStartScene(const char* path);
		int  FindSceneIndex(const char* path);
		SceneHolder* FindSceneHolder(const char* path);
		void AddScene(AssetScene* scene);
		void DeleteScene(SceneHolder* holder);

		void GenerateUID(SceneHolder* holder);
		void Reset();

		void SelectExportDir();
		void UpdateExeStrings(HANDLE res);
		void UpdateIconsSet(HANDLE resource, const char* path, bool smallIcon, int idOffest);
		void Export();
		void CpyFolder(const char* src, const char* dest = nullptr);
		void CpyFile(const char* src, const char* dest = nullptr);
	};
}


#pragma once

#include "Support/Support.h"
#include "Root/Scenes/Scene.h"
#include "Root/Scenes/SceneEntity.h"

/**
\ingroup gr_code_editor
*/

namespace Orin
{
	class Project
	{
	public:

		struct Layer
		{
			enum State
			{
				Normal,
				Unselectable,
				Invisible
			};

			State state = Normal;
			eastl::string name;
		};

		eastl::vector<Layer> layers;

		bool runningInFullscreen = false;

		eastl::string projectFullName;
		eastl::string projectName;
		char projectPath[1024];
		char applicationDir[512];
		eastl::string exportDir;

		eastl::string iconPath;
		TextureRef icon;

		eastl::string iconSmallPath;
		TextureRef iconSmall;

		eastl::string startScene;

		eastl::string p4URL;
		eastl::string p4Workspace;
		eastl::string p4User;

		Math::Vector2 alignRect = 32.0f;
		bool useAlignRect = false;
		bool hideCursor = false;

		bool CanRun();
		bool Load(const char* fileName);

		void Save(const char* fileName);
		void Save();

		void SetStartScene(const eastl::string& path);
		bool IsStartScene(const eastl::string& name);

		int FindLayer(const char* layer);

		void AddLayer(const char* name);
		void DeleteLayer(const char* name);

		bool LayerHiden(const char* name);
		bool LayerSelectable(const char* name);

		void Reset();

		void SelectExportDir();
		void UpdateExeStrings(HANDLE res);
		void UpdateIconsSet(HANDLE resource, const char* path, bool smallIcon, int idOffest);
		void Export();
		void CpyFolder(const char* src, const char* dest = nullptr);
		void CpyFile(const char* src, const char* dest = nullptr);
	};
}

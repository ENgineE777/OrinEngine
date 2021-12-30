#include "Editor.h"
#include "Support/Sprite.h"
#include "shlobj_core.h"

namespace Oak
{
	bool Project::CanRun()
	{
		if (startScene == -1)
		{
			MESSAGE_BOX("Can't start", "Please define a start scene");

			return false; 
		}

		return true;
	}

	void Project::Load(const char* fileName)
	{
		Reset();

		projectFullName = fileName;
		StringUtils::GetPath(projectFullName.c_str(), projectPath);
		StringUtils::FixSlashes(projectPath);

		root.SetRootPath(projectPath);
		root.assets.LoadAssets();

		root.scripts.CompileProjectCode(false);

		JsonReader reader;

		if (reader.ParseFile(projectFullName.c_str()))
		{
			reader.Read("start_scene", startScene);
			reader.Read("export_dir", exportDir);

			reader.Read("iconPath", iconPath);
			icon = root.render.LoadTexture(iconPath.c_str(), _FL_);

			reader.Read("iconSmallPath", iconSmallPath);
			iconSmall = root.render.LoadTexture(iconSmallPath.c_str(), _FL_);

			reader.Read("projectName", projectName);
			reader.Read("alignRect", editor.gizmo.alignRect);
			reader.Read("useAlignRect", editor.gizmo.useAlignRect);

			reader.Read("pixelsPerUnit", Sprite::pixelsPerUnit);
			Sprite::pixelsPerUnitInvert = 1.0f / Sprite::pixelsPerUnit;

			reader.Read("pixelsHeight", Sprite::pixelsHeight);

			bool useAlignRect = false;

			while (reader.EnterBlock("scenes"))
			{
				scenes.push_back(new SceneHolder());
				SceneHolder* scn = scenes.back();

				eastl::string str;
				reader.Read("path", str);
				scn->SetPath(str.c_str());

				reader.Read("selected_entity", scn->selectedEntity);
				reader.Read("camera2DMode", scn->camera2DMode);
				reader.Read("camera3DAngles", scn->camera3DAngles);
				reader.Read("camera3DPos", scn->camera3DPos);
				reader.Read("camera2DPos", scn->camera2DPos);
				reader.Read("camera2DZoom", scn->camera2DZoom);

				reader.LeaveBlock();
			}

			eastl::string edScene;
			reader.Read("selected_scene", edScene);

			int sceneIndex = FindSceneIndex(edScene.c_str());

			if (sceneIndex != -1)
			{
				SelectScene(scenes[sceneIndex]);
			}
		}
	}

	void Project::LoadScene(SceneHolder* holder)
	{
		if (holder->scene)
		{
			return;
		}

		holder->scene = new Scene();
		holder->scene->Init();

		char path[1024];
		StringUtils::Printf(path, 1024, "%s%s", projectPath, holder->path.c_str());

		StringUtils::Copy(holder->scene->projectScenePath, 512, holder->path.c_str());
		holder->scene->Load(path);
	}

	void Project::Save(const char* fileName)
	{
		bool needSetRoot = projectFullName[0] ? false : true;

		projectFullName = fileName;
		StringUtils::GetPath(projectFullName.c_str(), projectPath);

		Save();

		if (needSetRoot)
		{
			root.SetRootPath(projectPath);
			root.assets.LoadAssets();
		}
	}

	void Project::Save()
	{
		if (selectedScene)
		{
			FillSelectedObject(selectedScene);
			SaveCameraPos(selectedScene);
		}

		for (auto& holder : scenes)
		{
			if (!holder->scene)
			{
				continue;
			}

			char path[1024];
			StringUtils::Printf(path, 1024, "%s%s", projectPath, holder->path.c_str());

			holder->scene->Save(path);

			int len = (int)strlen(path);

			path[len - 2] = 'n';
			path[len - 1] = 't';
		}

		JsonWriter writer;
		writer.Start(projectFullName.c_str());

		writer.Write("export_dir", exportDir.c_str());
		writer.Write("iconPath", iconPath.c_str());
		writer.Write("iconSmallPath", iconSmallPath.c_str());

		if (projectName.empty())
		{
			char name[128];
			StringUtils::GetFileName(projectFullName.c_str(), name);
			StringUtils::RemoveExtension(name);

			projectName = name;
		}

		writer.Write("projectName", projectName.c_str());

		writer.Write("start_scene", startScene);
		writer.Write("scenes_count", (int)scenes.size());

		writer.Write("alignRect", editor.gizmo.alignRect);
		writer.Write("useAlignRect", editor.gizmo.useAlignRect);

		writer.Write("pixelsPerUnit", Sprite::pixelsPerUnit);
		writer.Write("pixelsHeight", Sprite::pixelsHeight);

		writer.StartArray("scenes");

		for (auto& scn : scenes)
		{
			writer.StartBlock(nullptr);

			writer.Write("path", scn->path.c_str());
			writer.Write("selected_entity", scn->selectedEntity);
			writer.Write("camera2DMode", scn->camera2DMode);
			writer.Write("camera3DAngles", scn->camera3DAngles);
			writer.Write("camera3DPos", scn->camera3DPos);
			writer.Write("camera2DPos", scn->camera2DPos);
			writer.Write("camera2DZoom", scn->camera2DZoom);

			writer.FinishBlock();
		}

		writer.FinishArray();

		if (selectedScene)
		{
			writer.Write("selected_scene", selectedScene->path.c_str());
		}
		else
		{
			writer.Write("selected_scene", "");
		}
	}

	void Project::FillSelectedObject(SceneHolder* holder)
	{
		if (editor.selectedEntity)
		{
			holder->selectedEntity = editor.selectedEntity->GetUID();
		}
		else
		{
			holder->selectedEntity = -1;
		}
	}

	void Project::SetStartScene(const char* path)
	{
		if (startScene != -1)
		{
			startScene = -1;
		}

		int index = FindSceneIndex(path);

		if (index != -1)
		{
			startScene = index;
		}
	}

	bool Project::IsStartScene(const char* path)
	{
		return startScene == FindSceneIndex(path);
	}

	void Project::SelectScene(SceneHolder* holder)
	{
		if (holder == selectedScene)
		{
			return;
		}

		if (selectedScene)
		{
			SaveCameraPos(selectedScene);

			FillSelectedObject(selectedScene);
			editor.SelectEntity(nullptr);

			EnableScene(selectedScene, false);
		}

		selectedScene = holder;

		if (selectedScene)
		{
			if (!selectedScene->scene)
			{
				LoadScene(selectedScene);
			}

			editor.freeCamera.mode2D = selectedScene->camera2DMode;

			editor.freeCamera.angles = selectedScene->camera3DAngles;
			editor.freeCamera.pos = selectedScene->camera3DPos;

			editor.freeCamera.pos2D = selectedScene->camera2DPos;
			editor.freeCamera.zoom2D = selectedScene->camera2DZoom;

			EnableScene(selectedScene, true);

			if (selectedScene->selectedEntity != -1)
			{
				editor.SelectEntity(selectedScene->scene->FindEntity(selectedScene->selectedEntity));
			}
		}
	}

	Project::SceneHolder* Project::FindSceneHolder(const char* path)
	{
		for (int i = 0; i < scenes.size(); i++)
		{
			if (StringUtils::IsEqual(scenes[i]->path.c_str(), path))
			{
				return scenes[i];
			}
		}

		return nullptr;
	}

	int Project::FindSceneIndex(const char* path)
	{
		char name[256];
		StringUtils::GetFileName(path, name);
		StringUtils::RemoveExtension(name);

		for (int i = 0; i<scenes.size(); i++)
		{
			if (StringUtils::IsEqual(scenes[i]->name.c_str(), name))
			{
				return i;
			}
		}

		return -1;
	}

	Scene* Project::GetScene(const char* path)
	{
		int index = FindSceneIndex(path);

		if (index != -1)
		{
			if (!scenes[index]->scene)
			{
				LoadScene(scenes[index]);
				scenes[index]->scene->EnableTasks(false);
			}

			return scenes[index]->scene;
		}

		return nullptr;
	}

	void Project::AddScene(const char* path)
	{
		char cropped_path[1024];
		StringUtils::GetCropPath(projectPath, path, cropped_path, 1024);

		if (FindSceneIndex(cropped_path) != -1)
		{
			return;
		}

		SceneHolder* holder = new SceneHolder();
		holder->SetPath(cropped_path);

		LoadScene(holder);

		if (holder->uid == 0)
		{
			GenerateUID(holder);
		}
		else
		{
			for (auto& scn : scenes)
			{
				if (scn == holder)
				{
					continue;
				}

				if (scn->uid == holder->uid)
				{
					holder->scene->Clear();
					delete holder->scene;
					delete holder;

					return;
				}
			}
		}

		holder->uid = holder->scene->uid;

		scenes.push_back(holder);

		char name[256];
		StringUtils::GetFileName(cropped_path, name);
		StringUtils::RemoveExtension(name);

		holder->SetPath(cropped_path);

		SelectScene(holder);
	}

	void Project::DeleteScene(SceneHolder* holder)
	{
		int index = FindSceneIndex(holder->path.c_str());

		if (index != -1)
		{
			if (startScene == index)
			{
				startScene = -1;
			}

			if (selectedScene == scenes[index])
			{
				SelectScene(nullptr);
			}

			delete scenes[index]->scene;
			delete scenes[index];
			scenes.erase(scenes.begin() + index);
		}
	}

	void Project::EnableScene(SceneHolder* holder, bool enable)
	{
		holder->scene->EnableTasks(enable);
	}

	void Project::GenerateUID(SceneHolder* holder)
	{
		uint16_t uid = 0;

		while (uid == 0)
		{
			float koef = Math::Rand() * 0.99f;
			uid = (uint16_t)(koef * 4096) << 4;

			for (auto& scn : scenes)
			{
				if (scn == holder)
				{
					continue;
				}

				if (scn->uid == uid)
				{
					uid = 0;
					break;
				}
			}

			if (uid != 0)
			{
				holder->uid = uid;
				holder->scene->uid = uid;
			}
		}
	}

	void Project::Reset()
	{
		icon.ReleaseRef();
		iconSmall.ReleaseRef();

		Sprite::pixelsPerUnit = 50.0f;
		Sprite::pixelsPerUnitInvert = 1.0f / Sprite::pixelsPerUnit;
		Sprite::pixelsHeight = 1080.0f;

		editor.SelectEntity(nullptr);
		selectedScene = nullptr;

		for (auto& scn : scenes)
		{
			RELEASE(scn->scene)
			delete scn;
		}

		scenes.clear();

		root.assets.Clear();

		projectFullName.clear();
		exportDir.clear();
		startScene = -1;
	}

	void Project::SelectExportDir()
	{
		BROWSEINFOA bi = { 0 };

		bi.lpszTitle = "Browse for Folder";

		LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

		if (pidl != NULL)
		{
			CHAR tszPath[MAX_PATH] = "\0";

			if (SHGetPathFromIDListA(pidl, tszPath) == TRUE)
			{
				exportDir = tszPath;
			}

			CoTaskMemFree(pidl);
		}
	}

	void Project::CpyFolder(const char* src, const char* dest)
	{
		root.files.CpyFolder((eastl::string(applicationDir) + src).c_str(), (exportDir + (dest != nullptr ? dest : src)).c_str());
	}

	void Project::CpyFile(const char* src, const char* dest)
	{
		root.files.CpyFile((eastl::string(applicationDir) + src).c_str(), (exportDir + (dest != nullptr ? dest : src)).c_str());
	}

	void Project::UpdateExeStrings(HANDLE resource)
	{
		eastl::wstring strings[16];

		StringUtils::Utf8toUtf16(strings[7], projectName.c_str());
		StringUtils::Utf8toUtf16(strings[13], "Oak");

		char buffer[256];
		int index = 0;

		for (auto& str : strings)
		{
			uint16_t sz = (uint16_t)str.size();
			memcpy(&buffer[index], &sz, 2);
			index += 2;

			for (int j = 0; j < str.size(); j++)
			{
				uint16_t chr = str[j];
				memcpy(&buffer[index], &chr, 2);
				index += 2;
			}
		}

		if (!UpdateResourceA(resource, MAKEINTRESOURCEA(6), MAKEINTRESOURCEA(7), 1033, (LPVOID)buffer, index))
		{
			auto err = GetLastError();
			root.Log("Export", "Failed to update string in output exe with error %u", err);
		}
	}

	void Project::UpdateIconsSet(HANDLE resource, const char* path, bool smallIcon, int idOffest)
	{
		int sizes[] = { 16, 24, 32, 48, 64, 72, 96, 128, 256 };

		char icoPath[512];
		StringUtils::Printf(icoPath, 512, "%s/%s", root.GetRootPath(), path);
		StringUtils::RemoveExtension(icoPath);

		for (int i = 0; i < sizeof(sizes) / sizeof(int); i++)
		{
			if (system(StringUtils::PrintTemp("%s/ENgine/Tools/nconvert/nconvert.exe -resize %i %i -overwrite -out ico \"%s%s\"", applicationDir, sizes[i], sizes[i], root.GetRootPath(), path)) == 0)
			{
				FileInMemory ico;

				if (ico.Load(StringUtils::PrintTemp("%s.ico", icoPath)))
				{
					uint8_t* ptr = ico.GetPtr();
					ptr += 22;

					if (!UpdateResourceA(resource, MAKEINTRESOURCEA(3), MAKEINTRESOURCEA(i + 1 + idOffest), 1033, (LPVOID)ptr, ico.GetSize() - 22))
					{
						auto err = GetLastError();
						root.Log("Export", "Failed to update icon set in output exe with error %u", err);
					}
				}
			}

			if (smallIcon && i == 4)
			{
				break;
			}
		}

		DeleteFileA(StringUtils::PrintTemp("%s.ico", icoPath));
	}

	void Project::Export()
	{
		if (projectFullName.empty())
		{
			return;
		}

		if (exportDir.empty())
		{
			SelectExportDir();

			if (exportDir.empty())
			{
				return;
			}
		}

		Save();

		GetCurrentDirectoryA(512, applicationDir);

		root.files.DeleteFolder(exportDir.c_str());

		root.files.CpyFolder(projectPath, (exportDir + "/project").c_str());

		root.files.DeleteFolder((exportDir + "/project/_Code").c_str());

		{
			char project_file_name[512];
			StringUtils::GetFileName(projectFullName.c_str(), project_file_name);

			rename((exportDir + "/project/" + project_file_name).c_str(), (exportDir + "/project/project.pra").c_str());
		}

		CpyFolder("/ENgine/Controls");
		CpyFolder("/ENgine/Shaders/PC");

		CpyFolder("/ENgine/ExportBinaries", "");

		CpyFile("/ENgine/helvetica");
		CpyFile("/ENgine/low2hi.dat");
		CpyFile("/ENgine/OpenSans-Regular.ttf");

		auto resource = BeginUpdateResourceA((exportDir + "/Oak.exe").c_str(), FALSE);
		if (resource != NULL)
		{
			UpdateExeStrings(resource);

			if (icon.Get())
			{
				UpdateIconsSet(resource, iconPath.c_str(), false, 0);

				UpdateIconsSet(resource, iconSmall.Get() ? iconSmallPath.c_str() : iconPath.c_str(), true, 9);
			}

			EndUpdateResourceA(resource, FALSE);
		}

		rename((exportDir + "/Oak.exe").c_str(), (exportDir + "/" + projectName + ".exe").c_str());

		MESSAGE_BOX("Export finished", (eastl::string("Resources of project were exported to folder:\n") + exportDir).c_str());
	}

	void Project::SaveCameraPos(SceneHolder* holder)
	{
		holder->camera2DMode = editor.freeCamera.mode2D;

		holder->camera3DAngles = editor.freeCamera.angles;
		holder->camera3DPos = editor.freeCamera.pos;

		holder->camera2DPos = editor.freeCamera.pos2D;
		holder->camera2DZoom = editor.freeCamera.zoom2D;
	}
}
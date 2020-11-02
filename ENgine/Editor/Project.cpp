#include "Editor.h"

namespace Oak
{
	bool Project::CanRun()
	{
		return (startScene != -1);
	}

	void Project::Load(const char* fileName)
	{
		Reset();

		projectName = fileName;
		StringUtils::GetPath(projectName.c_str(), projectPath);

		JsonReader reader;

		if (reader.Parse(projectName.c_str()))
		{
			reader.Read("start_scene", startScene);
			reader.Read("export_dir", exportDir);

			while (reader.EnterBlock("scenes"))
			{
				scenes.push_back(new SceneHolder());
				SceneHolder* scn = scenes.back();

				eastl::string str;
				reader.Read("path", str);
				scn->SetPath(str.c_str());

				reader.Read("selected_entity", scn->selectedEntity);

				while (reader.EnterBlock("include"))
				{
					reader.Read("path", str);

					scn->includedPathes.push_back(str);

					reader.LeaveBlock();
				}

				reader.LeaveBlock();
			}

			for (auto& scn : scenes)
			{
				for (auto& incl : scn->includedPathes)
				{
					for (auto& sub_scn : scenes)
					{
						if (StringUtils::IsEqual(sub_scn->path.c_str(), incl.c_str()))
						{
							scn->included.push_back(sub_scn);

							break;
						}
					}
				}
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

		for (auto& incl : holder->included)
		{
			LoadScene(incl);
		}

		holder->scene = new Scene();
		holder->scene->Init();

		for (auto& incl : holder->included)
		{
			holder->scene->includedScenes.push_back(incl->scene);
		}

		char path[1024];
		StringUtils::Printf(path, 1024, "%s%s", projectPath, holder->path.c_str());

		StringUtils::Copy(holder->scene->projectScenePath, 512, holder->path.c_str());
		holder->scene->Load(path);

		int len = (int)strlen(path);

		path[len - 2] = 'n';
		path[len - 1] = 't';
	}

	void Project::Save(const char* fileName)
	{
		projectName = fileName;
		StringUtils::GetPath(projectName.c_str(), projectPath);

		Save();
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
		writer.Start(projectName.c_str());

		writer.Write("export_dir", exportDir.c_str());
		writer.Write("start_scene", startScene);
		writer.Write("scenes_count", (int)scenes.size());

		writer.StartArray("scenes");

		for (auto& scn : scenes)
		{
			writer.StartBlock(nullptr);

			writer.Write("path", scn->path.c_str());

			writer.Write("selected_object", scn->selectedEntity);

			writer.StartArray("include");

			for (auto& incl : scn->included) 
			{
				writer.StartBlock(nullptr);

				writer.Write("path", incl->path.c_str());

				writer.FinishBlock();
			}

			writer.FinishArray();

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

			EnableScene(selectedScene, true);

			if (selectedScene->selectedEntity != -1)
			{
				editor.SelectEntity(selectedScene->scene->FindByUID(selectedScene->selectedEntity));
			}
		}
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

	bool Project::HaveDepenecy(const char* str, Project::SceneHolder* holder)
	{
		if (StringUtils::IsEqual(holder->path.c_str(), str))
		{
			return true;
		}

		for (auto& incl : holder->included)
		{
			bool res = HaveDepenecy(str, incl);

			if (res)
			{
				return true;
			}
		}

		return false;
	}

	void Project::EnableScene(SceneHolder* holder, bool enable)
	{
		for (auto& incl : holder->included)
		{
			EnableScene(incl, enable);
		}

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
		editor.SelectEntity(nullptr);
		selectedScene = nullptr;

		for (auto& scn : scenes)
		{
			if (scn->scene)
			{
				scn->scene->Clear();
				delete scn->scene;
				delete scn;
			}
		}

		scenes.clear();

		projectName.clear();
		exportDir.clear();
		startScene = -1;
	}

	void Project::Export()
	{
	}

	void Project::SaveCameraPos(SceneHolder* holder)
	{
	}
}
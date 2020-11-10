
#include "Root/Scenes/SceneEntity.h"
#include "Root/Root.h"

namespace Oak
{
	void SceneManager::Init()
	{
	}

	void SceneManager::LoadProject(const char* projectName)
	{
		StringUtils::GetPath(projectName, projectPath);

		JsonReader reader;
		if (reader.Parse(projectName))
		{
			int startScene = -1;
			if (!reader.Read("start_scene", startScene))
			{
				return;
			}

			int count = 0;
			reader.Read("scenesCount", count);
			scenes.resize(count);

			int index = 0;
			char name[256];

			while (reader.EnterBlock("scenes"))
			{
				SceneHolder& scn = scenes[index];

				reader.Read("path", scn.path);

				StringUtils::GetFileName(scn.path.c_str(), name);
				StringUtils::RemoveExtension(name);

				scenesSearch[name] = &scenes[index];

				while (reader.EnterBlock("include"))
				{
					reader.Read("path", name, 256);

					char inclName[256];

					StringUtils::GetFileName(name, inclName);
					StringUtils::RemoveExtension(inclName);

					reader.LeaveBlock();
				}

				reader.LeaveBlock();

				index++;
			}

			LoadScene(&scenes[startScene]);
		}
	}

	void SceneManager::LoadScene(const char* name)
	{
		if (scenesSearch.find(name) == scenesSearch.end())
		{
			return;
		}

		scenesToLoad.push_back(scenesSearch[name]);
	}

	void SceneManager::LoadScene(SceneHolder* holder)
	{
		if (failureOnScenePlay)
		{
			return;
		}

		holder->refCounter++;

		if (holder->scene)
		{
			return;
		}

		holder->scene = new Scene();
		holder->scene->Init();

		char path[1024];
		StringUtils::Printf(path, 1024, "%s%s", projectPath, holder->path.c_str());

		holder->scene->Load(path);

		if (!holder->scene->Play())
		{
			failureOnScenePlay = true;
		}
	}

	Scene* SceneManager::GetScene(const char* name)
	{
		if (scenesSearch.find(name) == scenesSearch.end())
		{
			return nullptr;
		}

		return scenesSearch[name]->scene;
	}

	void SceneManager::Execute(float dt)
	{
		for (auto* holder : scenesToLoad)
		{
			LoadScene(holder);
		}

		scenesToLoad.clear();

		for (auto* holder : scenesToDelete)
		{
			UnloadScene(holder);
		}

		scenesToDelete.clear();

		for (int i = 0; i < scenes.size(); i++)
		{
			auto& scn = scenes[i];

			if (scn.scene)
			{
				if (scn.refCounter != 0)
				{
					scn.scene->Execute(dt);
				}
			}
		}
	}

	void SceneManager::SetScenesGroupsState(const char* groupName, int state)
	{
		for (auto& scn : scenes)
		{
			if (scn.scene)
			{
				eastl::vector<Scene::Group*> outGroup;
				scn.scene->GetGroup(outGroup, groupName);

				for (auto group : outGroup)
				{
					for (auto entity : group->entities)
					{
						entity->SetState((SceneEntity::State)state);
					}
				}
			}
		}
	}

	void SceneManager::UnloadScene(const char* name)
	{
		if (scenesSearch.find(name) == scenesSearch.end())
		{
			return;
		}

		SceneHolder* holder = scenesSearch[name];

		if (holder->refCounter > 0)
		{
			holder->refCounter--;

			if (holder->refCounter == 0)
			{
				scenesToDelete.push_back(holder);
			}
		}
	}

	void SceneManager::UnloadScene(SceneHolder* holder)
	{
		if (holder->refCounter == 0)
		{
			RELEASE(holder->scene)
		}
	}

	void SceneManager::UnloadAll()
	{
		failureOnScenePlay = false;
		failureOnScenePlayMessage.clear();

		for (auto& scn : scenes)
		{
			if (scn.scene)
			{
				RELEASE(scn.scene)
			}
		}

		scenes.clear();
		scenesSearch.clear();
	}
}
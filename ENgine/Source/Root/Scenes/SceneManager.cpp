
#include "Root/Scenes/SceneEntity.h"
#include "Root/Root.h"

namespace Orin
{
	void SceneManager::Init()
	{
		ClassFactorySceneEntity::Sort();
		ClassFactorySceneEntity::SortGrouped();

		groupTaskPool = root.taskExecutor.CreateGroupTaskPool(_FL_);
	}

	void SceneManager::LoadProject(const char* projectName)
	{
		JsonReader reader;
		if (reader.ParseFile(projectName))
		{
			eastl::string startScene;
			if (!reader.Read("start_scene", startScene))
			{
				return;
			}

			float pixelsPerUnit = 50.0f;
			reader.Read("pixelsPerUnit", pixelsPerUnit);

			float pixelsHeight = 1080.0f;
			reader.Read("pixelsHeight", pixelsHeight);

			Sprite::SetData(pixelsHeight, pixelsPerUnit);

			StringUtils::LowerCase(startScene);

			LoadScene(&scenes[startScene]);
		}
	}

	void SceneManager::RegisterScene(const eastl::string& path, const eastl::string& name)
	{
		eastl::string nameStr = name;
		StringUtils::LowerCase(nameStr);

		SceneHolder& sceneHolder = scenes[nameStr];
		sceneHolder.path = path;

		StringUtils::LowerCase(sceneHolder.path);
	}

	uint16_t SceneManager::GenerateUID()
	{
		uint16_t uid = 0;

		while (uid == 0)
		{
			float koef = Math::Rand() * 0.99f;
			uid = (uint16_t)(koef * 4096) << 4;

			/*for (auto& scn : scenes)
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
			}*/

			if (uid != 0)
			{
				break;
			}
		}

		return uid;
	}

	void SceneManager::LoadScene(const char* name)
	{
		eastl::string nameStr = name;
		StringUtils::LowerCase(nameStr);

		if (scenes.find(nameStr) == scenes.end())
		{
			return;
		}

		scenesToLoad.push_back(&scenes[name]);
	}

	void SceneManager::LoadScene(SceneHolder* holder)
	{
		if (holder->scene)
		{
			return;
		}

		holder->scene = new Scene();
		holder->scene->Init();

		holder->scene->Load(holder->path.c_str());

		holder->scene->Play();

		orderToUnload.push_back(holder);
	}

	Scene* SceneManager::GetScene(const char* name)
	{
		if (scenes.find(name) == scenes.end())
		{
			return nullptr;
		}

		return scenes[name].scene;
	}

	TaskExecutor::SingleTaskPool* SceneManager::AddTaskPool(const char* file, int line)
	{
		return groupTaskPool->AddTaskPool(file, line);
	}

	void SceneManager::DelTaskPool(TaskExecutor::SingleTaskPool* pool)
	{
		groupTaskPool->DelTaskPool(pool);
	}

	void SceneManager::Execute(float dt)
	{
		for (auto* holder : scenesToDelete)
		{
			UnloadScene(holder);
		}

		scenesToDelete.clear();

		for (auto* holder : scenesToLoad)
		{
			LoadScene(holder);
		}

		scenesToLoad.clear();

		groupTaskPool->Execute(dt);
	}

	void SceneManager::SetScenesGroupsVisibilty(const char* groupName, bool set)
	{
		for (auto& scn : scenes)
		{
			if (scn.second.scene)
			{
				eastl::vector<Scene::Group*> outGroup;
				scn.second.scene->GetGroup(outGroup, groupName);

				for (auto group : outGroup)
				{
					for (auto entity : group->entities)
					{
						entity->SetVisiblity(set);
					}
				}
			}
		}
	}

	void SceneManager::UnloadScene(const char* name)
	{
		if (scenes.find(name) == scenes.end())
		{
			return;
		}

		SceneHolder& holder = scenes[name];

		holder.scene->EnableTasks(false);
		scenesToDelete.push_back(&holder);
	}

	void SceneManager::UnloadScene(SceneHolder* holder)
	{
		if (holder->scene)
		{
			auto* scene = holder->scene;

			auto iter = eastl::find(orderToUnload.begin(), orderToUnload.end(), holder);

			if (iter != orderToUnload.end())
			{
				orderToUnload.erase(iter);
			}

			RELEASE(holder->scene)
			root.sounds.DeleteSceneSounds(scene);
		}
	}

	void SceneManager::UnloadAll()
	{
		for (int i = (int)orderToUnload.size() - 1; i >= 0; i--)
		{
			if (orderToUnload[i]->scene)
			{
				RELEASE(orderToUnload[i]->scene)
			}
		}

		orderToUnload.clear();
	}

	void SceneManager::Release()
	{
		UnloadAll();

		delete groupTaskPool;
	}
}
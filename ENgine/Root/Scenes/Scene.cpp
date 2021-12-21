
#include "Root/Scenes/SceneEntity.h"
#include "Root/Root.h"

namespace Oak
{
	void Scene::Init()
	{
		taskPool = root.taskExecutor.CreateSingleTaskPool(_FL_);
		renderTaskPool = root.render.AddTaskPool(_FL_);
	}

	SceneEntity* Scene::CreateEntity(const char* name, bool setNameAndUDID)
	{
		ClassFactorySceneEntity* decl = ClassFactorySceneEntity::Find(name);

		if (!decl)
		{
			return nullptr;
		}

		SceneEntity* entity = decl->Create(_FL_);

		if (entity)
		{
			entity->scriptClassName = decl->GetShortName();

			entity->scene = this;
			entity->className = decl->GetName();
			entity->Init();

			entity->GetMetaData()->Prepare(entity);
			entity->GetMetaData()->SetDefValues();

			if (setNameAndUDID)
			{
				entity->SetName(decl->GetShortName());
				GenerateUID(entity);
			}
		}

		return entity;
	}

	void Scene::AddEntity(SceneEntity* entity, SceneEntity* entityBefore)
	{
		if (entityBefore)
		{
			for (int i = 0; i < entities.size(); i++)
			{
				if (entities[i] == entityBefore)
				{
					entities.insert(entities.begin() + i + 1, entity);
					return;
				}
			}
		}
		else
		{
			entities.push_back(entity);
		}
	}

	SceneEntity* Scene::FindEntity(uint32_t uid)
	{
		for (auto entity : entities)
		{
			if (entity && entity->GetUID() == uid)
			{
				return entity;
			}

			SceneEntity* child = entity->FindChild(uid);

			if (child)
			{
				return child;
			}
		}

		return nullptr;
	}

	void Scene::DeleteEntity(SceneEntity* obj, bool releaseObj)
	{
		for (int i = 0; i < entities.size(); i++)
		{
			if (entities[i] == obj)
			{
				entities.erase(entities.begin() + i);
				if (releaseObj)
				{
					obj->Release();
				}

				break;
			}
		}
	}

	const char* Scene::GetPath()
	{
		return scenePath;
	}

	const char* Scene::GetName()
	{
		return sceneName;
	}

	void Scene::Clear()
	{
		for (auto& entity : entities)
		{
			RELEASE(entity);
		}

		entities.clear();
	}

	void Scene::LoadEntities(JsonReader& reader, const char* name, eastl::vector<SceneEntity*>& entities)
	{
		while (reader.EnterBlock(name))
		{
			char type[512];
			reader.Read("type", type, 512);

			SceneEntity* entity = nullptr;

			entity = CreateEntity(type, false);

			if (entity)
			{
				entities.push_back(entity);

				reader.Read("uid", entity->uid);

				entity->Load(reader);

				LoadEntities(reader, "childs", entity->childs);
			}

			reader.LeaveBlock();
		}
	}

	void Scene::Load(const char* name)
	{
		JsonReader reader;
	
		StringUtils::GetPath(name, scenePath);

		if (reader.ParseFile(name))
		{
			StringUtils::GetFileName(name, sceneName);
			StringUtils::RemoveExtension(sceneName);

			reader.Read("uid", uid);
			LoadEntities(reader, "entities", entities);

			for (auto entity : entities)
			{
				entity->PostLoad();
				entity->UpdateVisibility();
			}
		}
	}

	void Scene::SaveEntities(JsonWriter& writer, const char* name, eastl::vector<SceneEntity*>& entities)
	{
		writer.StartArray(name);

		for (auto& entity : entities)
		{
			writer.StartBlock(nullptr);

			writer.Write("type", entity->className);
			writer.Write("uid", entity->GetUID());

			entity->Save(writer);

			if (entity->childs.size() > 0)
			{
				SaveEntities(writer, "childs", entity->childs);
			}

			writer.FinishBlock();
		}

		writer.FinishArray();
	}

	void Scene::Save(const char* name)
	{
		JsonWriter writer;

		if (writer.Start(name))
		{
			writer.Write("uid", uid);

			SaveEntities(writer, "entities", entities);
		}
	}

	void Scene::Execute(float dt)
	{
		taskPool->Execute(dt);
	}

	void Scene::Play()
	{
		if (playing)
		{
			return;
		}

		playing = true;

		for (auto entity : entities)
		{
			entity->Play();

			if (entity->groupName.c_str())
			{
				AddToGroup(entity, entity->groupName.c_str());
			}
		}

		for (auto& item : postPlayList)
		{
			(item.entity->*item.call)();
		}

		postPlayList.clear();
	}

	void Scene::AddPostPlay(int level, Object* entity, Object::DelegateSimple call)
	{
		postPlayList.push_back(PostPlayDelegate());

		PostPlayDelegate& delegate = postPlayList.back();

		delegate.level = level;
		delegate.entity = entity;
		delegate.call = call;

		int index = (int)postPlayList.size() - 2;

		while (index >= 0 && postPlayList[index].level > level)
		{
			PostPlayDelegate tmp = postPlayList[index + 1];
			postPlayList[index + 1] = postPlayList[index];
			postPlayList[index] = tmp;

			index--;
		}
	}

	#ifdef OAK_EDITOR
	const eastl::vector<SceneEntity*>& Scene::GetEntities()
	{
		return entities;
	}

	void Scene::Export()
	{
		for (auto* entity : entities)
		{
			entity->Export();
		}
	}
	#endif

	bool Scene::IsPlaying()
	{
		return playing;
	}

	void Scene::EnableTasks(bool enable)
	{
		taskPool->SetActive(enable);
		renderTaskPool->SetActive(enable);
	}

	void Scene::GenerateUID(SceneEntity* obj)
	{
		union UID32
		{
			uint32_t id;
			struct
			{
				uint16_t id1;
				uint16_t id2;
			};
		};

		UID32 objUid;
		objUid.id = 0;

		while (!objUid.id)
		{
			float koef = Math::Rand() * 0.99f;
			objUid.id = (uint32_t)(koef * 1048576);
			objUid.id1 |= uid;
			objUid.id = FindEntity(objUid.id) ? 0 : objUid.id;
		}

		obj->SetUID(objUid.id);
	}

	SceneEntity* Scene::FindInGroup(const char* groupName, const char* name)
	{
		eastl::vector<Group*> outGroups;

		GetGroup(outGroups, groupName);

		for (auto group : outGroups)
		{
			for (auto entity : group->entities)
			{
				if (StringUtils::IsEqual(entity->GetName(), name))
				{
					return entity;
				}
			}
		}

		return nullptr;
	}

	void Scene::GetGroup(eastl::vector<Group*>& outGroups, const char* name)
	{
		if (groups.find(name) != groups.end())
		{
			outGroups.push_back(&groups[name]);
		}
	}

	void Scene::AddToGroup(SceneEntity* entity, const char* name)
	{
		Group& group = groups[name];

		if (group.name.c_str()[0] == 0)
		{
			group.name = name;
		}

		group.entities.push_back(entity);
	}

	void Scene::DelFromGroup(Group& group, SceneEntity* entity, Scene* newScene)
	{
		for (int i = 0; i < group.entities.size(); i++)
		{
			if (group.entities[i] == entity)
			{
				if (newScene)
				{
					newScene->AddToGroup(entity, group.name.c_str());
				}
				group.entities.erase(group.entities.begin() + i);
				break;
			}
		}
	}

	void Scene::DelFromGroup(SceneEntity* obj, const char* name)
	{
		DelFromGroup(groups[name], obj);
	}

	void Scene::DelFromAllGroups(SceneEntity* obj, Scene* newScene)
	{
		for (auto& group : groups)
		{
			DelFromGroup(group.second, obj, newScene);
		}
	}

	void Scene::Release()
	{
		Clear();

		delete taskPool;
		root.render.DelTaskPool(renderTaskPool);

		delete this;
	}
}

#include "Root/Scenes/SceneEntity.h"
#include "Root/Root.h"

namespace Oak
{
	void Scene::Init()
	{
		taskPool = root.taskExecutor.CreateSingleTaskPool(_FL_);
		renderTaskPool = root.render.AddTaskPool(_FL_);
	}

	SceneEntity* Scene::CreateEntity(const char* name)
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

			AddEntity(entity);
		}

		return entity;
	}

	void Scene::AddEntity(SceneEntity* entity)
	{
		entities.push_back(entity);
	}

	SceneEntity* Scene::FindByUID(uint32_t uid)
	{
		SceneEntity* res = nullptr;

		for (auto obj : entities)
		{
			if (obj && obj->GetUID() == uid)
			{
				res = obj;
				break;
			}
		}

		return res;
	}

	void Scene::DeleteObject(SceneEntity* obj, bool releaseObj)
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

	void Scene::Load(const char* name)
	{
		JsonReader reader;
	
		StringUtils::GetPath(name, scenePath);

		if (reader.Parse(name))
		{
			StringUtils::GetFileName(name, sceneName);
			StringUtils::RemoveExtension(sceneName);

			reader.Read("uid", uid);
			while (reader.EnterBlock("entities"))
			{
				char type[512];
				reader.Read("type", type, 512);

				SceneEntity* obj = nullptr;

				obj = CreateEntity(type);

				if (obj)
				{
					reader.Read("uid", obj->uid);

					if (obj->uid == 0)
					{
						GenerateUID(obj);
					}

					auto* transform = obj->GetTransform();

					if (transform)
					{
						transform->Load(reader, "transform");
					}

					obj->Load(reader);
				}

				reader.LeaveBlock();
			}

			for (auto entity : entities)
			{
				entity->GetMetaData()->Prepare(entity);
				entity->GetMetaData()->PostLoad();

				entity->ApplyProperties();
			}
		}
	}

	void Scene::Save(const char* name)
	{
		JsonWriter writer;

		if (writer.Start(name))
		{
			writer.Write("uid", uid);

			writer.StartArray("entities");

			for (auto& entity : entities)
			{
				writer.StartBlock(nullptr);

				writer.Write("type", entity->className);
				writer.Write("uid", entity->GetUID());

				auto* transform = entity->GetTransform();

				if (transform)
				{
					transform->Save(writer, "transform");
				}

				entity->Save(writer);

				writer.FinishBlock();
			}

			writer.FinishArray();

		}
	}

	void Scene::Execute(float dt)
	{
		taskPool->Execute(dt);
	}

	bool Scene::Play()
	{
		if (playing)
		{
			return true;
		}

		playing = true;

		for (auto entity : entities)
		{
			if (!entity->Play())
			{
				return false;
			}

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

		return true;
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
	SceneEntity* Scene::GetEntity(int index)
	{
		return entities[index];
	}

	int Scene::GetEntityCount()
	{
		return (int)entities.size();
	}

	void Scene::Export()
	{
		for (auto* entity : entities)
		{
			entity->Export();
		}
	}
	#endif

	bool Scene::Playing()
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
			objUid.id = FindByUID(objUid.id) ? 0 : objUid.id;
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
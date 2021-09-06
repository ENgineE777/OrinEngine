
#include "Root/Scenes/SceneEntity.h"
#include "Root/Root.h"

namespace Oak
{
	void SceneEntity::ApplyProperties()
	{

	}

	#ifdef OAK_EDITOR
	void SceneEntity::SetScene(Scene* setScene)
	{
		if (scene == setScene)
		{
			return;
		}

		scene->DeleteEntity(this, false);

		scene->taskPool->DelAllTasks(this, setScene->taskPool);

		scene->renderTaskPool->DelAllTasks(this, setScene->renderTaskPool);

		scene->DelFromAllGroups(this, setScene);

		scene = setScene;
		scene->AddEntity(this);
	}

	void SceneEntity::SetEditMode(bool ed)
	{
		edited = ed;
	}

	bool SceneEntity::IsEditMode()
	{
		return edited;
	}

	void SceneEntity::Copy(SceneEntity* source)
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->Copy(source, source->GetMetaData()->properties);

		ApplyProperties();
	}
	#endif

	const char* SceneEntity::GetName()
	{
		return name.c_str();
	}

	void SceneEntity::SetName(const char* setName)
	{
		name = setName;
	}

	void SceneEntity::SetUID(uint32_t setUid)
	{
		uid = setUid;
	}

	uint32_t SceneEntity::GetUID()
	{
		return uid;
	}

	void SceneEntity::SetVisiblity(bool set)
	{
		visible = set;

		UpdateVisibility();
	}

	void SceneEntity::UpdateVisibility()
	{
		currentVisible = visible;

		if (parent)
		{
			currentVisible &= parent->currentVisible;
		}

		OnVisiblityChange(currentVisible);

		for (auto& child : childs)
		{
			child->UpdateVisibility();
		}
	}

	bool SceneEntity::IsVisible()
	{
	#ifdef OAK_EDITOR
		//if (!scene->Playing() && project.LayerHiden(layer_name.c_str()))
		{
			//return State::Invisible;
		}
	#endif

		return currentVisible;
	}

	void SceneEntity::OnVisiblityChange(bool set)
	{

	}

	Transform& SceneEntity::GetTransform()
	{
		return transform;
	}

	void SceneEntity::Load(JsonReader& reader)
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->Load(reader);
	}

	void SceneEntity::PostLoad()
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->PostLoad(scene);

		ApplyProperties();

		for (auto child : childs)
		{
			child->parent = this;

			child->GetTransform().parent = &transform.global;

			child->PostLoad();
		}
	}

	void SceneEntity::Save(JsonWriter& writer)
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->Save(writer);
	}

	TaskExecutor::SingleTaskPool* SceneEntity::Tasks(bool render)
	{
		return render ? scene->renderTaskPool : scene->taskPool;
	}

	void SceneEntity::Play()
	{
		for (auto* child : childs)
		{
			child->Play();
		}
	}

	#ifdef OAK_EDITOR
	bool SceneEntity::ContainEntity(SceneEntity* entity)
	{
		for (auto* child : childs)
		{
			if (child == entity || child->ContainEntity(entity))
			{
				return true;
			}
		}

		return false;
	}

	void SceneEntity::Export()
	{

	}

	bool SceneEntity::CheckSelection(Math::Vector2 ms, Math::Vector3 start, Math::Vector3 dir)
	{
		return false;
	}
	#endif

	void SceneEntity::Release()
	{
		for (auto* child : childs)
		{
			RELEASE(child)
		}

		if (scene)
		{
			scene->taskPool->DelAllTasks(this);
		}

		if (scene)
		{
			scene->renderTaskPool->DelAllTasks(this);
		}

		if (scene) scene->DelFromAllGroups(this);

		delete this;
	}

	void SceneEntity::SetParent(SceneEntity* setParent, SceneEntity* entityBefore)
	{
		auto& transform = GetTransform();

		if (parent)
		{
			for (int i = 0; i < parent->childs.size(); i++)
			{
				if (parent->childs[i] == this)
				{
					parent->childs.erase(parent->childs.begin() + i);
					break;
				}
			}

			transform.parent = nullptr;
			transform.local = transform.global;
		}

		parent = setParent;

		if (parent)
		{
			if (entityBefore)
			{
				for (int i = 0; i < parent->childs.size(); i++)
				{
					if (parent->childs[i] == entityBefore)
					{
						parent->childs.insert(parent->childs.begin() + i + 1, this);
						return;
					}
				}
			}
			else
			{
				parent->childs.push_back(this);
			}

			auto& transform = GetTransform();

			transform.parent = &parent->GetTransform().global;
			Math::Matrix inverse = parent->GetTransform().global;
			inverse.Inverse();

			transform.local = transform.global * inverse;
		}
	}

	SceneEntity* SceneEntity::GetParent()
	{
		return parent;
	}

	const eastl::vector<SceneEntity*>& SceneEntity::GetChilds()
	{
		return childs;
	}

	SceneEntity* SceneEntity::GetChild(uint32_t uid)
	{
		for (auto entity : childs)
		{
			if (entity && entity->GetUID() == uid)
			{
				return entity;
			}

			SceneEntity* child = entity->GetChild(uid);

			if (child)
			{
				return child;
			}
		}

		return nullptr;
	}

	void SceneEntity::BindClassToScript()
	{
	}

	bool SceneEntity::InjectIntoScript(const char* typeName, int name, void* property, const char* prefix)
	{
		if (!StringUtils::IsEqual(typeName, scriptClassName))
		{
			return false;
		}

		//*(asPWORD*)(property) = (asPWORD)this;

		return true;
	}
}
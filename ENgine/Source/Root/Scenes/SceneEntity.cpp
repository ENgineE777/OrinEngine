
#include "Root/Scenes/SceneEntity.h"
#include "Root/Root.h"

#ifdef OAK_EDITOR
#include "Editor/Editor.h"
#endif

namespace Oak
{
	void SceneEntity::FillLayersList(eastl::vector<eastl::string>& names)
	{
		for (auto& layer : editor.project.layers)
		{
			names.push_back(layer.name);
		}		
	}

	void SceneEntity::OpenSourcePrefab(void* owner)
	{
		SceneEntity* entity = (SceneEntity*)owner;

		if (entity->prefabRef.Get())
		{
			editor.SelectEditAsset(AssetRef(AssetRef((Asset*)entity->prefabRef.Get(), _FL_)));
		}
	}

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

		editor.gizmo.SetTransform(this, &transform);
	}

	bool SceneEntity::IsEditMode()
	{
		return edited;
	}

	void SceneEntity::Copy(SceneEntity* source)
	{
		if (source->prefabRef)
		{
			prefabRef = source->prefabRef;
			prefabRef->RegisterIntstance(this);
		}

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
		if (!GetScene()->IsPlaying() &&
			editor.project.LayerHiden(prefabInstance ? GetPrefabRoot()->layerName.c_str() : layerName.c_str()))
		{			
			return false;
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

			child->GetTransform().parent = &transform;
			transform.childs.push_back(&child->transform);

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

	bool SceneEntity::ImGuiProperties()
	{
		GetMetaData()->Prepare(this);

		if (prefabInstance)
		{
			GetMetaData()->ImGuiWidgets(nullptr);
		}
		else
		{
			static eastl::vector<eastl::string> ignorePrefabInstanceProprties;

			if (ignorePrefabInstanceProprties.empty())
			{
				ignorePrefabInstanceProprties.push_back(eastl::string("Edit Prefab"));
			}

			GetMetaData()->ImGuiWidgets(&ignorePrefabInstanceProprties, true);
		}

		if (GetMetaData()->IsValueWasChanged())
		{
			ApplyProperties();
			UpdateVisibility();

			return true;
		}

		return false;
	}

	bool SceneEntity::CheckSelection(Math::Vector2 ms, Math::Vector3 start, Math::Vector3 dir)
	{
		if (transform.objectType == ObjectType::Object2D)
		{
			auto mat = transform.GetGlobal();

			if (transform.rotation.z > 90.0f)
			{
				int k = 0;
				k++;
			}

			auto leftCorner = -mat.MulNormal(Sprite::ToUnits(transform.size * Math::Vector3(transform.offset.x, 1.0f - transform.offset.y, 0.5f)));
			auto rightCorner = mat.MulNormal(Sprite::ToUnits(transform.size * Math::Vector3(1.0f - transform.offset.x, transform.offset.y, 0.5f)));

			Math::Vector3 vmin = FLT_MAX;
			vmin.Min(leftCorner);
			vmin.Min(rightCorner);

			Math::Vector3 vmax = FLT_MIN;
			vmax.Max(leftCorner);
			vmax.Max(rightCorner);

			return Math::IntersectBBoxRay(mat.Pos() + vmin, mat.Pos() + vmax, start, dir);
		}
		
		return Math::IntersectBBoxRay(transform.GetGlobal().Pos() - transform.size * 0.5f, transform.GetGlobal().Pos() + transform.size * 0.5f, start, dir);;
	}

	void SceneEntity::OnMouseMove(Math::Vector2 ms)
	{

	}

	void SceneEntity::OnLeftMouseDown()
	{

	}

	void SceneEntity::OnLeftMouseUp()
	{

	}

	bool SceneEntity::OnRightMouseDown()
	{
		return false;
	}

	void SceneEntity::OnRightMouseUp()
	{

	}

	void SceneEntity::OnMiddleMouseDown()
	{

	}

	void SceneEntity::OnMiddleMouseUp()
	{

	}
	#endif

	void SceneEntity::Release()
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->PreapareToRelease();

		for (int k = 0; k < referenced.size(); k++)
		{
			referenced[k]->SetEntity(nullptr);
			k--;
		}

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

#ifdef OAK_EDITOR
		if (prefabRef.Get())
		{
			prefabRef->UnregisterIntstance(this);
		}
#endif

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
					parent->transform.childs.erase(parent->transform.childs.begin() + i);
					break;
				}
			}

			transform.SetLocal(transform.GetGlobal());
			transform.parent = nullptr;
		}

		parent = setParent;

		if (parent)
		{
			Math::Matrix invParent = parent->transform.GetGlobal();
			invParent.Inverse();

			transform.SetLocal(transform.GetGlobal() * invParent);

			transform.parent = setParent ? &setParent->transform : nullptr;

			if (entityBefore)
			{
				for (int i = 0; i < parent->childs.size(); i++)
				{
					if (parent->childs[i] == entityBefore)
					{
						parent->childs.insert(parent->childs.begin() + i + 1, this);
						parent->transform.childs.insert(parent->transform.childs.begin() + i + 1, &transform);

						return;
					}
				}
			}

			parent->childs.push_back(this);
			parent->transform.childs.push_back(&transform);
		}
	}

	void SceneEntity::SetSelfAsChild(SceneEntity* setParent, int index)
	{
		parent = setParent;

		if (parent)
		{
			parent->childs[index]->parent = nullptr;
			parent->childs[index]->transform.parent = nullptr;

			parent->childs[index] = this;
			parent->GetTransform().childs[index] = &transform;
		}
	}

	SceneEntity* SceneEntity::GetParent()
	{
		return parent;
	}

	SceneEntity* SceneEntity::GetPrefabRoot()
	{
		if (prefabInstance && parent && parent->prefabInstance)
		{
			return parent->GetPrefabRoot();
		}

		return this;
	}


	eastl::vector<SceneEntity*>& SceneEntity::GetChilds()
	{
		return childs;
	}

	SceneEntity* SceneEntity::FindChild(uint32_t uid)
	{
		for (auto entity : childs)
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
}
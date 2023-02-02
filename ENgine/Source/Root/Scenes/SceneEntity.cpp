
#include "Root/Scenes/SceneEntity.h"
#include "Root/Root.h"

#ifdef ORIN_EDITOR
#include "Editor/Editor.h"
#endif

namespace Orin
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

	#ifdef ORIN_EDITOR
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
	#ifdef ORIN_EDITOR
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
		GetMetaData()->checkEnabledState = prefabInstance;
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
		GetMetaData()->checkEnabledState = prefabInstance;
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

	#ifdef ORIN_EDITOR
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
		GetMetaData()->checkEnabledState = prefabInstance;

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
		Math::Vector3 vMin = FLT_MAX;
		Math::Vector3 vMax = FLT_MIN;

		GetBBox(vMin, vMax);
		
		return Math::IntersectBBoxRay(vMin, vMax, start, dir);

		/*if (transform.objectType == ObjectType::Object2D)
		{
			auto mat = transform.GetGlobal();

			Math::Vector3 corners[] = { mat.MulNormal(Sprite::ToUnits(transform.size * Math::Vector3(-transform.offset.x, transform.offset.y - 1.0f, 0.5f))),
										mat.MulNormal(Sprite::ToUnits(transform.size * Math::Vector3(1.0f - transform.offset.x, transform.offset.y - 1.0f, 0.5f))),
										mat.MulNormal(Sprite::ToUnits(transform.size * Math::Vector3(-transform.offset.x, transform.offset.y, 0.5f))),
										mat.MulNormal(Sprite::ToUnits(transform.size * Math::Vector3(1.0f - transform.offset.x, transform.offset.y, 0.5f))) };

			Math::Vector3 vmin = FLT_MAX;
			Math::Vector3 vmax = FLT_MIN;

			for (auto corner : corners)
			{
				vmin.Min(corner);
				vmax.Max(corner);
			}

			return Math::IntersectBBoxRay(mat.Pos() + vmin, mat.Pos() + vmax, start, dir);
		}

		return Math::IntersectBBoxRay(transform.GetGlobal().Pos() - transform.size * 0.5f, transform.GetGlobal().Pos() + transform.size * 0.5f, start, dir);*/
	}

	void SceneEntity::GetBBox(Math::Vector3& vMin, Math::Vector3& vMax)
	{
		auto mat = transform.GetGlobal();

		if (transform.objectType == ObjectType::Object2D)
		{
			Math::Vector3 corners[] = { mat.MulNormal(Sprite::ToUnits(transform.size * Math::Vector3(-transform.offset.x, transform.offset.y - 1.0f, 0.5f))),
										mat.MulNormal(Sprite::ToUnits(transform.size * Math::Vector3(1.0f - transform.offset.x, transform.offset.y - 1.0f, 0.5f))),
										mat.MulNormal(Sprite::ToUnits(transform.size * Math::Vector3(-transform.offset.x, transform.offset.y, 0.5f))),
										mat.MulNormal(Sprite::ToUnits(transform.size * Math::Vector3(1.0f - transform.offset.x, transform.offset.y, 0.5f))) };

			vMin = FLT_MAX;
			vMax = FLT_MIN;

			for (auto corner : corners)
			{
				vMin.Min(corner);
				vMax.Max(corner);
			}
		}
		else
		{
			vMin = -transform.size * 0.5f;
			vMax =  transform.size * 0.5f;
		}

		vMin = transform.GetGlobal().Pos() + vMin;
		vMax = transform.GetGlobal().Pos() + vMax;
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

#ifdef ORIN_EDITOR
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
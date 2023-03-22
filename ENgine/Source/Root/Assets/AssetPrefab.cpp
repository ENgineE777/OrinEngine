#include "Root/Root.h"
#include "Root/Scenes/SceneEntity.h"
#include "eastl/algorithm.h"

namespace Orin
{
	CLASSREG(Asset, AssetPrefab, "AssetPrefab")

	META_DATA_DESC(AssetPrefab)
	META_DATA_DESC_END()

	AssetPrefab::AssetPrefab() : AssetScene()
	{
		isPrefab = true;
	}

	void AssetPrefab::SetRootEntityType(const char* type)
	{
		auto* scene = GetScene();
		scene->AddEntity(scene->CreateEntity(type, false));
	}

	SceneEntity* AssetPrefab::CreateInstance(Scene* sceneOwner, bool callPostLoad)
	{
		auto& entities = GetScene()->GetEntities();
		SceneEntity* instance = nullptr;

		inCreatingInstance = true;

		if (entities.size() > 0)
		{
			auto* src = entities[0];

			instance = sceneOwner->CreateEntity(src->className, true);

			instance->Copy(src);

			instance->prefabRef = AssetPrefabRef(this, _FL_);

			instance->SetName(GetName().c_str());

			instanceMapping[src->GetUID()] = instance;

			CopyChilds(src, instance, sceneOwner);

			if (callPostLoad)
			{
				instance->PostLoad();
			}

			instance->UpdateVisibility();			

			FixReferences(instance);

			instanceMapping.clear();

#ifdef ORIN_EDITOR
			RegisterIntstance(instance);
#endif
		}

		inCreatingInstance = false;

		return instance;
	}

	void AssetPrefab::FixReferences(SceneEntity* entity)
	{
		entity->GetMetaData()->Prepare(entity);

		for (auto& prop : entity->GetMetaData()->properties)
		{
			if (prop.type == MetaData::Type::SceneEntity)
			{
				SceneEntityRefBase* ref = reinterpret_cast<SceneEntityRefBase*>(prop.value);

				if (instanceMapping.count(ref->uid) > 0)
				{
					ref->SetEntity(instanceMapping[ref->uid]);
				}
			}
		}

		for (auto child : entity->GetChilds())
		{
			FixReferences(child);
		}
	}

#ifdef ORIN_EDITOR
	void AssetPrefab::RegisterIntstance(SceneEntity* entity)
	{
		instances.push_back(entity);
	}

	void AssetPrefab::UnregisterIntstance(SceneEntity* entity)
	{
		auto iterator = eastl::find(instances.begin(), instances.end(), entity);

		if (iterator != instances.end())
		{
			instances.erase(iterator);
		}
	}

	void AssetPrefab::Save()
	{
		AssetScene::Save();

		int count = (int)instances.size();

		if (count > 0)
		{
			auto* root = GetScene()->GetEntities()[0];
			auto& rootChilds = root->GetChilds();

			for (int i = 0; i < count; i++)
			{
				auto* instance = instances[i];
				auto* scene = instance->GetScene();

				auto& childs = instance->GetChilds();

				for (int j = 0; j < childs.size(); j++)
				{
					if (childs[j]->prefabInstance)
					{
						auto* child = childs[j];
						child->SetParent(nullptr);
						RELEASE(child)
						j--;
					}
				}

				eastl::vector<SceneEntity*> tmp = childs;

				for (int j = 0; j < tmp.size(); j++)
				{
					tmp[j]->SetParent(nullptr, nullptr);
				}

				for (int j = 0; j < root->childs.size(); j++)
				{
					auto* childSrc = root->childs[j];
					auto* childCopy = scene->CreateEntity(childSrc->className, true);

					childCopy->SetParent(instance, nullptr);
					childCopy->Copy(childSrc);
					childCopy->PostLoad();

					CopyChilds(childSrc, childCopy, scene);
					childCopy->UpdateVisibility();
				}

				for (int j = 0; j < tmp.size(); j++)
				{
					tmp[j]->SetParent(instance, nullptr);
				}
			}
		}
	}
#endif
};

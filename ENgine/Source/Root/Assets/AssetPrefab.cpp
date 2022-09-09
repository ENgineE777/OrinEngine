#include "Root/Root.h"
#include "Root/Scenes/SceneEntity.h"
#include "eastl/algorithm.h"

namespace Oak
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
		scene->AddEntity(scene->CreateEntity(type));
	}

	SceneEntity* AssetPrefab::CreateInstance(Scene* sceneOwner)
	{
		auto& entities = GetScene()->GetEntities();
		SceneEntity* instance = nullptr;

		if (entities.size() > 0)
		{
			auto* src = entities[0];

			instance = sceneOwner->CreateEntity(src->className);

			instance->Copy(src);
			instance->PostLoad();

			instance->prefabRef = AssetPrefabRef(this, _FL_);
			instance->prefabInstance = true;

			instance->SetName(GetName().c_str());

			CopyChilds(src, instance, sceneOwner);
			instance->UpdateVisibility();

#ifdef OAK_EDITOR
			instances.push_back(instance);
#endif
		}

		return instance;
	}

#ifdef OAK_EDITOR
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

				bool visible = instance->visible;
				eastl::string name = instance->GetName();
				auto& transform = instance->GetTransform();

				auto position = transform.position;
				auto scale = transform.scale;
				auto rotation = transform.rotation;

				instance->Copy(root);

				instance->SetVisiblity(visible);
				instance->SetName(name.c_str());
				transform.position = position;
				transform.scale = scale;
				transform.rotation = rotation;

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
					auto* childCopy = scene->CreateEntity(childSrc->className);

					childCopy->SetParent(instance, nullptr);
					childCopy->prefabInstance = true;
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

#include "Root/Root.h"
#include "Root/Scenes/SceneEntity.h"

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
		}

		return instance;
	}
};
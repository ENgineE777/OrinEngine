#include "Root/Root.h"

namespace Oak
{
	CLASSREG(Asset, AssetPrefab, "AssetPrefab")

	META_DATA_DESC(AssetPrefab)
	META_DATA_DESC_END()

	void AssetPrefab::EnableTasks(bool set)
	{
		GetScene()->EnableTasks(set);
	}

	Scene* AssetPrefab::GetScene()
	{
		if (!scene)
		{
			scene = new Scene();
			scene->Init();

			scene->Load(path.c_str());

			scene->EnableTasks(false);
		}

		return scene;
	}

	void AssetPrefab::Reload()
	{
	}

	void AssetPrefab::LoadData(JsonReader& loader)
	{
	}

	#ifdef OAK_EDITOR
	void AssetPrefab::SaveData(JsonWriter& saver)
	{
	}

	const char* AssetPrefab::GetSceneEntityType()
	{
		return nullptr;
	}
	#endif

	void AssetPrefab::Release()
	{
		DELETE_PTR(scene)
	}
};
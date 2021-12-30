#include "Root/Root.h"

namespace Oak
{
	CLASSREG(Asset, AssetScene, "AssetScene")

	META_DATA_DESC(AssetScene)
	META_DATA_DESC_END()

	void AssetScene::Reload()
	{
	}

	void AssetScene::LoadData(JsonReader& loader)
	{
	}

	#ifdef OAK_EDITOR
	void AssetScene::SaveData(JsonWriter& saver)
	{
	}

	const char* AssetScene::GetSceneEntityType()
	{
		return nullptr;
	}
	#endif
};
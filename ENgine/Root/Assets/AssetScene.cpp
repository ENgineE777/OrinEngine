#include "Root/Root.h"
#include "SceneEntities/2D/SpriteEntity.h"


#ifdef OAK_EDITOR
#include "imgui.h"
#endif

namespace Oak
{
#ifdef OAK_EDITOR
	extern void ShowSpriteWindow(AssetTexture* texture);
#endif

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
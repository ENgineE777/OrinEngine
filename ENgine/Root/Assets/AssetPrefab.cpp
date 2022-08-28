#include "Root/Root.h"

namespace Oak
{
	CLASSREG(Asset, AssetPrefab, "AssetPrefab")

	META_DATA_DESC(AssetPrefab)
	META_DATA_DESC_END()

	AssetPrefab::AssetPrefab() : AssetScene()
	{
		isSelecteEditScenePrefab = true;
	}
};
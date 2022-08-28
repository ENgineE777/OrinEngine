#pragma once

#include "Asset.h"
#include "AssetScene.h"
#include "root/Render/Render.h"

namespace Oak
{
	class Scene;

	class CLASS_DECLSPEC AssetPrefab : public AssetScene
	{
		Scene* scene = nullptr;

	public:

		META_DATA_DECL_BASE(AssetPrefab)

			AssetPrefab();
	};
}
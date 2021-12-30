#pragma once

#include "Asset.h"
#include "root/Render/Render.h"

namespace Oak
{
	class Scene;

	class CLASS_DECLSPEC AssetPrefab : public Asset
	{
		Scene* scene = nullptr;

	public:

		META_DATA_DECL_BASE(AssetPrefab)

		Scene* GetScene();

		void EnableTasks(bool set) override;

		void Reload() override;

		void LoadData(JsonReader& loader) override;

		#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		#endif

		void Release() override;
	};
}
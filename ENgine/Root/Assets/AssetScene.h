#pragma once

#include "Asset.h"
#include "root/Render/Render.h"

namespace Oak
{
	class CLASS_DECLSPEC AssetScene : public Asset
	{
	public:

		META_DATA_DECL_BASE(AssetScene)

		void Reload() override;

		void LoadData(JsonReader& loader) override;

		#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		#endif
	};
}
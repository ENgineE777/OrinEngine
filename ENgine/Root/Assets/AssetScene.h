#pragma once

#include "Asset.h"
#include "root/Render/Render.h"

namespace Oak
{
	class CLASS_DECLSPEC AssetScene : public Asset
	{
		Scene* scene = nullptr;

	public:

		uint32_t selectedEntityID = -1;

		bool camera2DMode = false;

		Math::Vector2 camera3DAngles = Math::Vector2(0.0f, -0.5f);
		Math::Vector3 camera3DPos = Math::Vector3(0.0f, 6.0f, 0.0f);

		Math::Vector2 camera2DPos = 0.0;
		float camera2DZoom = 1.0f;

		Math::Vector2 gizmoAlign2D = 0.0f;

		META_DATA_DECL_BASE(AssetPrefab)

		Scene* GetScene();

		bool IsEditable() override;
		void EnableEditing(bool enable) override;

		void Reload() override;

		void LoadMetaData(JsonReader& loader) override;

#ifdef OAK_EDITOR
		void SaveMetaData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		void GrabEditorData();
#endif

		void Release() override;
	};
}
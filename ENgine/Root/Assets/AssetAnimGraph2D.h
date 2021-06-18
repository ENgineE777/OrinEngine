#pragma once

#include "Asset.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"

namespace Oak
{
	class AssetAnimGraph2D : public Asset
	{
		friend class PointerRef<AssetAnimGraph2D>;
		friend class AssetAnimGraph2DRef;

		static void StartEditAnimation(void* owner);

	public:

		AssetTextureRef texture;

		float k = 1.0f;
		META_DATA_DECL_BASE(AssetAnimGraph2D)

		void Reload() override;

		void LoadData(JsonReader& loader) override;

		#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		#endif
	};

	class AssetAnimGraph2DRef : public PointerRef<AssetAnimGraph2D>
	{
	public:

		AssetAnimGraph2DRef() : PointerRef() {};
		AssetAnimGraph2DRef(Asset* setPtr, const char* file, int line) : PointerRef(reinterpret_cast<AssetAnimGraph2D*>(setPtr), _FL_) {};
		AssetAnimGraph2DRef(AssetAnimGraph2D* setPtr, const char* file, int line) : PointerRef(setPtr, _FL_) {};
		
		void Draw(Transform* trans, Color clr, float dt);

		void SetupCreatedSceneEntity(SceneEntity* entity);

		Math::Vector2 GetSize();
	};
}
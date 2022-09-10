#pragma once

#include "Asset.h"
#include "AssetScene.h"
#include "root/Render/Render.h"

namespace Oak
{
	class CLASS_DECLSPEC AssetPrefab : public AssetScene
	{
		friend class PointerRef<AssetPrefab>;

#ifdef OAK_EDITOR
		eastl::vector<SceneEntity*> instances;
#endif
	public:

		META_DATA_DECL_BASE(AssetPrefab)

		AssetPrefab();

		void SetRootEntityType(const char* type);

		SceneEntity* CreateInstance(Scene* sceneOwner);

#ifdef OAK_EDITOR
		void RegisterIntstance(SceneEntity* entity);
		void UnregisterIntstance(SceneEntity* entity);
		void Save() override;
#endif
	};

	class CLASS_DECLSPEC AssetPrefabRef : public PointerRef<AssetPrefab>
	{
	public:

		AssetPrefabRef() : PointerRef() {};
		AssetPrefabRef(Asset* setPtr, const char* file, int line) : PointerRef(dynamic_cast<AssetPrefab*>(setPtr), _FL_) {};
		AssetPrefabRef(AssetPrefab* setPtr, const char* file, int line) : PointerRef(setPtr, _FL_) {};

		AssetPrefabRef& operator=(const AssetPrefabRef& ref)
		{
			Copy(ref);

			return *this;
		}
	};
}
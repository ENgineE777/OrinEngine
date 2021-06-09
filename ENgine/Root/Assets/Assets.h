#pragma once

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include "Support/ThreadExecutor.h"
#include <atomic>
#include "AssetTexture.h"

namespace Oak
{
	class Assets : public ThreadCaller
	{
		friend class Asset;

	public:

		struct AssetHolder
		{
			Asset* asset = nullptr;
			eastl::string name;
			eastl::string ext;
			eastl::string fullName;

			const char* GetAssetType()
			{
				return assetCreation[ext].c_str();
			}

			Asset* GetAsset()
			{
				if (asset == nullptr)
				{
					ClassFactoryAsset* decl = ClassFactoryAsset::Find(GetAssetType());

					asset = decl->Create(_FL_);
					asset->SetPath(fullName.c_str());
					asset->Reload();
				}

				return asset;
			}

			template<class T>
			T GetAssetRef()
			{
				return T(GetAsset(), _FL_);
			}
		};

		struct Folder
		{
			eastl::string name;
			eastl::string fullName;
			eastl::vector<Folder*> folders;
			eastl::vector<AssetHolder*> assets;

			void Clear()
			{
				for (auto* folder : folders)
				{
					folder->Clear();
					delete folder;
				}

				folders.clear();

				for (auto* asset : assets)
				{
					delete asset;
				}

				assets.clear();
			}
		};

	protected:

		static eastl::map<eastl::string, eastl::string> assetCreation;

		eastl::map<eastl::string, AssetHolder*> assetsMap;

		#ifdef OAK_EDITOR
		std::atomic<bool> scanning;
		std::atomic<bool> needRescan;

		ThreadExecutor executor;
		#endif

	public:

		Folder rootFolder;

		void Init();

		void LoadAssets();

		template<class T>
		T GetAssetRef(eastl::string& path)
		{
			if (assetsMap.count(path) > 0)
			{
				return assetsMap[path]->GetAssetRef<T>();
			}

			return T();
		};

		#ifdef OAK_EDITOR
		void LoadAssets(const char* path, Folder* folder, bool update);
		void ObserveRoot();
		#endif

		void Update();

		void Clear();
		void Release();
	};
}
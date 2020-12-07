#pragma once

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include "Support/ThreadExecutor.h"
#include <atomic>
#include "Asset.h"

namespace Oak
{
	class Assets : public ThreadCaller
	{
	public:

		struct AssetRef
		{
			Asset* asset = nullptr;
			eastl::string name;
			eastl::string ext;
			eastl::string fullName;
		};

		struct Folder
		{
			eastl::string name;
			eastl::string fullName;
			eastl::vector<Folder> folders;
			eastl::vector<AssetRef> assets;
		};

		eastl::string rootPath;
		Folder rootFolder;

		#ifdef OAK_EDITOR
		std::atomic<bool> scanning;
		std::atomic<bool> needRescan;

		ThreadExecutor executor;
		Folder* selFolder = nullptr;
		AssetRef* selAsset = nullptr;
		#endif

		void Init();

		void LoadAssets(const char* path, Folder& folder);

		void LoadAssets(const char* path);

		#ifdef OAK_EDITOR
		void ObserveRoot();
		#endif

		void Update();

		void Clear();
		void Release();
	};
}
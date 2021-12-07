#include "Root/Root.h"

namespace Oak
{
	eastl::map<eastl::string, eastl::string> Assets::assetCreation = { {"jpg", "AssetTexture"}, {"bmp", "AssetTexture"}, {"png", "AssetTexture"}, {"tga", "AssetTexture"},
	                                                                   {"psd", "AssetTexture"}, {"ang", "AssetAnimGraph2D"}, {"sca", "AssetScene"}, {"tls", "AssetTileSet"} };

	void Assets::Init()
	{

	}

	void Assets::LoadAssets()
	{
		const char* rootPath = root.GetRootPath();

		if (rootPath[0] == 0)
		{
			return;
		}

		#ifdef OAK_EDITOR
		LoadAssets(rootPath, &rootFolder, false);

		scanning = true;

		executor.Execute(this, (ThreadCaller::Delegate)&Assets::ObserveRoot);
		#endif
	}

	#ifdef OAK_EDITOR
	void Assets::LoadAssets(const char* path, Folder* folder, bool update)
	{
		WIN32_FIND_DATAA ffd;
		HANDLE hFile;
		CHAR serarchDir[256];
		CHAR serarchParams[256];

		BOOL fFile = TRUE;

		StringUtils::Copy(serarchDir, 256, path);

		StringUtils::Copy(serarchParams, 256, path);
		StringUtils::Cat(serarchParams, 256, "\\*.*");

		hFile = FindFirstFileA(serarchParams, &ffd);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			while (fFile)
			{
				if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					char extension[64];
					StringUtils::GetExtension(ffd.cFileName, extension, 64);

					if (assetCreation.count(extension) > 0)
					{
						char fileName[512];
						StringUtils::Printf(fileName, 512, "%s%s", path, ffd.cFileName);

						char relativeName[512];
						StringUtils::GetCropPath(root.GetRootPath(), fileName, relativeName, 512);

						bool found = false;

						if (update)
						{
							for (auto* item : folder->assets)
							{
								if (StringUtils::IsEqual(item->fullName.c_str(), relativeName))
								{
									if (item->asset && item->asset->WasChanged())
									{
										item->asset->Reload();
									}

									found = true;
									break;
								}
							}
						}

						if (!found)
						{
							folder->assets.push_back(NEW AssetHolder());

							AssetHolder* ref = folder->assets.back();
							ref->name = ffd.cFileName;
							ref->ext = extension;
							ref->fullName = relativeName;

							assetsMap[relativeName] = ref;
						}
					}
				}
				else
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
				{
					char subPath[512];
					StringUtils::Printf(subPath, 512, "%s%s/", path, ffd.cFileName);

					char relativePath[512];
					StringUtils::GetCropPath(root.GetRootPath(), subPath, relativePath, 512);

					bool found = false;

					if (update)
					{
						for (auto* item : folder->folders)
						{
							if (StringUtils::IsEqual(item->fullName.c_str(), relativePath))
							{
								LoadAssets(subPath, item, update);

								found = true;
								break;
							}
						}
					}

					if (!found && !StringUtils::IsEqual(ffd.cFileName, "_Code"))
					{
						folder->folders.push_back(NEW Folder());
						Folder* subFolder = folder->folders.back();
						subFolder->name = ffd.cFileName;
						subFolder->fullName = relativePath;

						LoadAssets(subPath, subFolder, update);
					}
				}

				fFile = FindNextFileA(hFile, &ffd);
			}

			FindClose(hFile);
		}
	}

	void Assets::ObserveRoot()
	{
		DWORD waitStatus;
		HANDLE changeHandles;

		changeHandles = FindFirstChangeNotificationA(root.GetRootPath(), true, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);

		/*if (changeHandles == INVALID_HANDLE_VALUE || changeHandles == NULL)
		{
			printf("\n ERROR: FindFirstChangeNotification function failed.\n");
		}*/

		while (scanning.load(std::memory_order_acquire))
		{
			waitStatus = WaitForMultipleObjects(1, &changeHandles, false, 0);

			if (waitStatus == WAIT_OBJECT_0)
			{
				ThreadExecutor::Sleep(500);

				needRescan.store(true, std::memory_order_release);

				if (FindNextChangeNotification(changeHandles) == FALSE)
				{
					//printf("\n ERROR: FindNextChangeNotification function failed.\n");
				}
			}
		}
	}
	#endif

	void Assets::Update()
	{
		#ifdef OAK_EDITOR
		if (needRescan.load(std::memory_order_acquire))
		{
			LoadAssets(root.GetRootPath(), &rootFolder, true);

			needRescan.store(false, std::memory_order_release);
		}
		#endif
	}

	void Assets::Clear()
	{
		#ifdef OAK_EDITOR
		scanning.store(false, std::memory_order_release);
		executor.Terminate();
		#endif

		assetsMap.clear();
		rootFolder.Clear();
	}

	void Assets::Release()
	{
		Clear();
	}
}
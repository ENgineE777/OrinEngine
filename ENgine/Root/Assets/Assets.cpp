#include "Root/Root.h"

namespace Oak
{
	void Assets::Init()
	{

	}

	void Assets::LoadAssets(const char* path, Folder& folder)
	{
#ifdef OAK_EDITOR
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

					char fileName[512];
					StringUtils::Printf(fileName, 512, "%s//%s", path, ffd.cFileName);

					char relativeName[512];
					StringUtils::GetCropPath(rootPath.c_str(), fileName, relativeName, 512);

					folder.assets.push_back();
					folder.assets.back().name = ffd.cFileName;
					folder.assets.back().ext = extension;
					folder.assets.back().fullName = relativeName;
				}
				else
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
				{
					char subPath[512];
					StringUtils::Printf(subPath, 512, "%s//%s", path, ffd.cFileName);

					char relativePath[512];
					StringUtils::GetCropPath(rootPath.c_str(), subPath, relativePath, 512);

					folder.folders.push_back();
					folder.folders.back().name = ffd.cFileName;
					folder.folders.back().fullName = relativePath;

					LoadAssets(subPath, folder.folders.back());
				}

				fFile = FindNextFileA(hFile, &ffd);
			}

			FindClose(hFile);
		}
#endif
	}

	void Assets::LoadAssets(const char* path)
	{
		rootPath = path;
		LoadAssets(path, rootFolder);

		scanning = true;

		#ifdef OAK_EDITOR
		executor.Execute(this, (ThreadCaller::Delegate)&Assets::ObserveRoot);
		#endif
	}

	#ifdef OAK_EDITOR
	void Assets::ObserveRoot()
	{
		DWORD waitStatus;
		HANDLE changeHandles;

		changeHandles = FindFirstChangeNotificationA(rootPath.c_str(), true, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);

		/*if (changeHandles == INVALID_HANDLE_VALUE || changeHandles == NULL)
		{
			printf("\n ERROR: FindFirstChangeNotification function failed.\n");
		}*/

		while (scanning.load(std::memory_order_acquire))
		{
			waitStatus = WaitForMultipleObjects(1, &changeHandles, false, 0);

			if (waitStatus == WAIT_OBJECT_0)
			{
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
			selFolder = nullptr;
			selAsset = nullptr;

			rootFolder.assets.clear();
			rootFolder.folders.clear();

			LoadAssets(rootPath.c_str(), rootFolder);

			needRescan.store(false, std::memory_order_release);
		}
		#endif
	}

	void Assets::Clear()
	{
		#ifdef OAK_EDITOR
		scanning.store(false, std::memory_order_release);
		executor.Terminate();

		selFolder = nullptr;
		selAsset = nullptr;
		#endif

		rootFolder.assets.clear();
		rootFolder.folders.clear();

		rootPath.clear();
	}

	void Assets::Release()
	{
		Clear();
	}
}

#ifdef PLATFORM_WIN
#include "Windows.h"
#include <sys/stat.h>
#endif

#include "Root/Root.h"
#include "Support/StringUtils.h"

namespace Oak
{
	bool Files::Init()
	{
		return true;
	}

	FILE* Files::FileOpen(const char* path, const char* mode)
	{
		FILE* file = nullptr;
		#ifdef PLATFORM_WIN
		fopen_s(&file, path, mode);
		#endif

		if (!file)
		{
			root.Log("Files", "File not found %s", path);
		}

		return file;
	}

	#ifdef PLATFORM_WIN
	bool Files::IsFileExist(const char*  name)
	{
		struct stat buffer;

		return (stat(name, &buffer) == 0);
	}

	void Files::CreateFolder(const char* path)
	{
		char ApplicationDir[512];
		GetCurrentDirectoryA(512, ApplicationDir);

		int index = 0;
		char PathTo[512];

		if (path[1] != ':')
		{
			PathTo[0] = '/';
			PathTo[1] = 0;
		}
		else
		{
			PathTo[0] = 0;
		}

		StringUtils::Copy(PathTo, 512, path);

		while (index < (int)strlen(path))
		{
			while (index < (int)strlen(PathTo) && (PathTo[index] != '/'&&PathTo[index] != '\\'))
			{
				index++;
			}

			if (index < (int)strlen(PathTo))
			{
				char FolderTo[512];

				StringUtils::Copy(FolderTo, 512, PathTo);

				FolderTo[index] = 0;

				char FullPath[512];

				if (path[1] != ':')
				{
					StringUtils::Copy(FullPath, 512, ApplicationDir);
				}
				else
				{
					FullPath[0] = 0;
				}

				StringUtils::Copy(FullPath, 512, FolderTo);

				CreateDirectoryA(FullPath, nullptr);
				index++;
			}
		}
	}

	void Files::DeleteFolder(const char* path)
	{
		WIN32_FIND_DATAA ffd;
		HANDLE hFile;
		CHAR searchDir[256];
		CHAR searchParams[256];

		BOOL fFile = TRUE;

		StringUtils::Copy(searchDir, 256, path);

		StringUtils::Copy(searchParams, 256, path);
		StringUtils::Cat(searchParams, 256, "\\*.*");

		hFile = FindFirstFileA(searchParams, &ffd);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			while (fFile)
			{
				if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					char FileName[512];

					StringUtils::Copy(FileName, 512, path);
					StringUtils::Cat(FileName, 512, "//");
					StringUtils::Copy(FileName, 512, ffd.cFileName);

					DeleteFileA(FileName);
				}
				else
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
				{
					char FileName[512];

					StringUtils::Copy(FileName, 512, path);
					StringUtils::Cat(FileName, 512, "//");
					StringUtils::Cat(FileName, 512, ffd.cFileName);

					DeleteFolder(FileName);

					RemoveDirectoryA(FileName);
				}

				fFile = FindNextFileA(hFile, &ffd);
			}

			FindClose(hFile);
		}

		RemoveDirectoryA(path);
	}

	void Files::CpyFolder(const char* path, const char* dest_path)
	{
		char cur_dir[521];
		StringUtils::Printf(cur_dir, 512, "%s/*.*", path);

		WIN32_FIND_DATAA data;
		HANDLE h = FindFirstFileA(cur_dir, &data);

		if (h != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!StringUtils::IsEqual(data.cFileName, ".") && !StringUtils::IsEqual(data.cFileName, ".."))
				{
					char sub_dir[521];
					StringUtils::Printf(sub_dir, 512, "%s/%s", path, data.cFileName);

					if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						char sub_path[521];
						StringUtils::Printf(sub_path, 512, "%s/%s", path, data.cFileName);

						char sub_dest_path[521];
						StringUtils::Printf(sub_dest_path, 512, "%s/%s", dest_path, data.cFileName);

						CpyFolder(sub_path, sub_dest_path);
					}
					else
					{
						char out_file[521];
						StringUtils::Printf(out_file, 512, "%s/%s", dest_path, data.cFileName);

						this->CpyFile(sub_dir, out_file);
					}
				}
			}

			while (FindNextFileA(h, &data));
		}

		FindClose(h);
	}

	bool Files::CpyFile(const char* srcPath, const char* destPath)
	{
		char ApplicationDir[512];
		GetCurrentDirectoryA(512, ApplicationDir);

		char strFrom[512];

		if (srcPath[1] == ':')
		{
			StringUtils::Copy(strFrom, 512, srcPath);
		}
		else
		{
			StringUtils::Copy(strFrom, 512, ApplicationDir);
			StringUtils::Cat(strFrom, 512, "/");
			StringUtils::Cat(strFrom, 512, srcPath);
		}

		char strTo[512];

		if (destPath[1] == ':')
		{
			StringUtils::Copy(strTo, 512, destPath);
		}
		else
		{
			StringUtils::Copy(strTo, 512, ApplicationDir);
			StringUtils::Cat(strTo, 512, "/");
			StringUtils::Cat(strTo, 512, destPath);
		}

		CreateFolder(destPath);

		char buf[8192];
		size_t size;

		FILE* source = nullptr;
		fopen_s(&source, strFrom, "rb");

		if (!source)
		{
			return false;
		}

		FILE* dest = nullptr;
		fopen_s(&dest, strTo, "wb");

		if (!dest)
		{
			fclose(source);
			return false;
		}

		while (size = fread(buf, 1, BUFSIZ, source))
		{
			fwrite(buf, 1, size, dest);
		}

		fclose(source);
		fclose(dest);

		return true;
	}
	#endif
}

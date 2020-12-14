
#include "Root/Root.h"

namespace Oak
{
	void Asset::SetPath(const char* setPath)
	{
		path = setPath;

#ifdef OAK_EDITOR
		char strName[512];
		StringUtils::GetFileName(path.c_str(), strName);
		name = strName;

		fullPath = root.GetRootPath() + path;

		if (!fullPath.empty())
		{
			stat(fullPath.c_str(), &fileInfo);
		}
#endif
	}

	const eastl::string& Asset::GetPath()
	{
		return path;
	}

	#ifdef OAK_EDITOR
	const eastl::string& Asset::GetName()
	{
		return name;
	}

	bool Asset::WasChanged()
	{
		struct stat info;

		if (!stat(fullPath.c_str(), &info))
		{
			if (fileInfo.st_mtime != info.st_mtime ||
				fileInfo.st_atime != info.st_atime ||
				fileInfo.st_ctime != info.st_ctime)
			{
				fileInfo = info;
				return true;
			}
		}

		return false;
	}
	#endif

	void Asset::Reload()
	{

	};
}

#include "Root/Root.h"

namespace Oak
{
	void Asset::SetPath(const char* setPath)
	{
		path = setPath;

#ifdef OAK_EDITOR
		fullPath = root.GetRootPath() + path;

		if (!fullPath.empty())
		{
			stat(fullPath.c_str(), &fileInfo);
		}
#endif
	}

	#ifdef OAK_EDITOR
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
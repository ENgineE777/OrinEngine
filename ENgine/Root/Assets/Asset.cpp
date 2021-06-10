
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

		JsonReader reader;

		StringUtils::Copy(strName, 512, path.c_str());
		StringUtils::Cat(strName, 512, ".meta");

		GetMetaData()->Prepare(this);

		if (reader.Parse(strName))
		{
			GetMetaData()->Load(reader);
			LoadData(reader);
		}
		else
		{
			SaveMetaData();
		}
#endif
	}

	const eastl::string& Asset::GetPath()
	{
		return path;
	}

	void Asset::LoadData(JsonReader& loader)
	{

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

	void Asset::SaveMetaData()
	{
		char strName[512];

		StringUtils::Copy(strName, 512, path.c_str());
		StringUtils::Cat(strName, 512, ".meta");

		JsonWriter writer;

		writer.Start(strName);
		GetMetaData()->Prepare(this);
		GetMetaData()->Save(writer);

		SaveData(writer);
	}

	void Asset::SaveData(JsonWriter& saver)
	{

	}
	#endif

	void Asset::Reload()
	{

	}

	void Asset::Release()
	{
		if (root.assets.assetsMap.count(path) > 0)
		{
			root.assets.assetsMap[path]->asset = nullptr;
		}

		delete this;
	}
}
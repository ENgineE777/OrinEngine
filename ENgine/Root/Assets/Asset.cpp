
#include "Root/Root.h"

namespace Oak
{
	void Asset::Init()
	{

	}

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

		if (reader.ParseFile(strName))
		{
			GetMetaData()->Load(reader);
			LoadMetaData(reader);
		}
		else
		{
			Save();
		}
#endif
	}

	const eastl::string& Asset::GetPath()
	{
		return path;
	}

	void Asset::LoadMetaData(JsonReader& loader)
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

	void Asset::Save()
	{
		char strName[512];

		StringUtils::Copy(strName, 512, path.c_str());
		StringUtils::Cat(strName, 512, ".meta");

		JsonWriter writer;

		writer.Start(strName);
		GetMetaData()->Prepare(this);
		GetMetaData()->Save(writer);

		SaveMetaData(writer);
	}

	void Asset::SaveMetaData(JsonWriter& saver)
	{

	}

	bool Asset::HasOwnTasks()
	{
		return taskPool || renderTaskPool;
	}

	void Asset::EnableTasks(bool enable)
	{
		if (taskPool) taskPool->SetActive(enable);
		if (renderTaskPool) renderTaskPool->SetActive(enable);
	}

	TaskExecutor::SingleTaskPool* Asset::Tasks()
	{
		if (!taskPool)
		{
			taskPool = root.taskExecutor.CreateSingleTaskPool(_FL_);
			taskPool->SetActive(false);
		}

		return taskPool;
	}

	TaskExecutor::SingleTaskPool* Asset::RenderTasks()
	{
		if (!renderTaskPool)
		{
			renderTaskPool = root.render.AddTaskPool(_FL_);
			renderTaskPool->SetActive(false);
		}

		return renderTaskPool;
	}

	void Asset::ImGuiProperties()
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->ImGuiWidgets();

		if (GetMetaData()->IsValueWasChanged())
		{
			Reload();
			Save();
		}
	}

	void Asset::ImGui(bool viewportFocused)
	{

	}

	void Asset::OnMouseMove(Math::Vector2 ms)
	{

	}

	void Asset::OnLeftMouseDown()
	{

	}

	void Asset::OnLeftMouseUp()
	{

	}

	void Asset::OnRightMouseDown()
	{

	}

	void Asset::OnRightMouseUp()
	{

	}

	void Asset::OnMiddleMouseDown()
	{

	}

	void Asset::OnMiddleMouseUp()
	{

	}
	#endif

	void Asset::Reload()
	{

	}

	void Asset::Release()
	{
#ifdef OAK_EDITOR
		if (taskPool)
		{
			delete taskPool;
		}

		if (renderTaskPool)
		{
			root.render.DelTaskPool(renderTaskPool);
		}
#endif

		if (root.assets.assetsMap.count(path) > 0)
		{
			root.assets.assetsMap[path]->asset = nullptr;
		}

		delete this;
	}
}
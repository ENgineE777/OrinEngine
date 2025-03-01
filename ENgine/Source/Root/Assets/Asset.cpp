
#include "Root/Root.h"

namespace Orin
{
	void Asset::Init()
	{

	}

	void Asset::SetPath(const char* setPath)
	{
		path = setPath;

#ifdef ORIN_EDITOR
		char strName[512];
		StringUtils::GetFileName(path.c_str(), strName);
		name = strName;

		fullPath = root.GetPath(Root::Path::Assets) + path;

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

	#ifdef ORIN_EDITOR
	const eastl::string& Asset::GetName()
	{
		return name;
	}

	bool Asset::SourceFileWasChanged()
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

	bool Asset::ContainsUnsavedChanges()
	{
		return containsUnsavedChanges;
	}

	void Asset::Save()
	{
		containsUnsavedChanges = false;

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

	bool Asset::IsEditable()
	{
		return taskPool || renderTaskPool;
	}

	void Asset::EnableEditing(bool enable)
	{
		if (taskPool) taskPool->SetActive(enable);
		if (renderTaskPool) renderTaskPool->SetActive(enable);
	}

	void Asset::Update(float dt)
	{
		if (Tasks())
		{
			Tasks()->Execute(dt);
		}
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

	void Asset::ImGuiMetaProperties()
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->ImGuiWidgets();

		if (GetMetaData()->IsValueWasChanged())
		{
			Reload();
			Save();
		}
	}

	bool Asset::ImGuiHasHierarchy()
	{
		return false;
	}

	void Asset::ImGuiHierarchy()
	{
	}

	void Asset::ImGuiProperties()
	{

	}

	void Asset::ImGuiViewport(bool viewportFocused)
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

	bool Asset::BlockMouseButtons()
	{
		return false;
	}
	#endif

	void Asset::Reload()
	{

	}

	void Asset::Release()
	{
#ifdef ORIN_EDITOR
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
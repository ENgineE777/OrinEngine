#pragma once

#include "Support/StringUtils.h"
#include "Support/ClassFactory.h"
#include "Support/PointerRef.h"
#include "Support/MetaData.h"
#include "Root/TaskExecutor/TaskExecutor.h"

#ifdef ORIN_EDITOR
#include <sys/stat.h>
#endif

namespace Orin
{
	class SceneEntity;

	class CLASS_DECLSPEC Asset : public Object
	{
		friend class PointerRef<Asset>;
	protected:

		#ifdef ORIN_EDITOR
		struct stat  fileInfo;
		eastl::string fullPath;
		eastl::string name;
		#endif

		eastl::string path;
		int refCounter = 0;

#ifdef ORIN_EDITOR
		TaskExecutor::SingleTaskPool* taskPool = nullptr;
		TaskExecutor::SingleTaskPool* renderTaskPool = nullptr;
		bool containsUnsavedChanges = false;
#endif

	public:

		virtual ~Asset() = default;

		virtual void Init();
		void SetPath(const char* setPath);
		const eastl::string& GetPath();

		virtual void LoadMetaData(JsonReader& loader);

		#ifdef ORIN_EDITOR
		const eastl::string& GetName();
		bool SourceFileWasChanged();
		bool ContainsUnsavedChanges();
		virtual void Save();
		virtual void SaveMetaData(JsonWriter& saver);
		virtual const char* GetSceneEntityType() = 0;
		virtual bool IsEditable();
		virtual void EnableEditing(bool enable);
		virtual void Update(float dt);
		TaskExecutor::SingleTaskPool* Tasks();
		TaskExecutor::SingleTaskPool* RenderTasks();
		virtual void ImGuiMetaProperties();
		virtual bool ImGuiHasHierarchy();
		virtual void ImGuiHierarchy();
		virtual void ImGuiProperties();
		virtual void ImGuiViewport(bool viewportFocused);
		virtual void OnMouseMove(Math::Vector2 ms);
		virtual	void OnLeftMouseDown();
		virtual void OnLeftMouseUp();
		virtual	void OnRightMouseDown();
		virtual void OnRightMouseUp();
		virtual	void OnMiddleMouseDown();
		virtual void OnMiddleMouseUp();
		#endif

		virtual MetaData* GetMetaData() = 0;

		virtual void Reload();

		virtual void Release();
	};

	CLASSFACTORYDEF(Asset)
	CLASSFACTORYDEF_END()

	class CLASS_DECLSPEC AssetRef : public PointerRef<Asset>
	{
	public:

		AssetRef() : PointerRef() {};
		AssetRef(Asset* setPtr, const char* file, int line) : PointerRef(setPtr, _FL_) {};

		AssetRef& operator=(const AssetRef& ref)
		{
			Copy(ref);

			return *this;
		}
	};
}
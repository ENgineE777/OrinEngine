#pragma once

#include "Support/StringUtils.h"
#include "Support/ClassFactory.h"
#include "Support/PointerRef.h"
#include "Support/MetaData.h"

#ifdef OAK_EDITOR
#include <sys/stat.h>
#endif

namespace Oak
{
	class SceneEntity;

	class Asset
	{
	protected:

		#ifdef OAK_EDITOR
		struct stat  fileInfo;
		eastl::string fullPath;
		eastl::string name;
		#endif

		eastl::string path;
		int refCounter = 0;

	public:

		virtual ~Asset() = default;

		void SetPath(const char* setPath);
		const eastl::string& GetPath();

		virtual void LoadData(JsonReader& loader);

		#ifdef OAK_EDITOR
		const eastl::string& GetName();
		bool WasChanged();
		void SaveMetaData();
		virtual void SaveData(JsonWriter& saver);
		virtual const char* GetSceneEntityType() = 0;
		#endif

		virtual MetaData* GetMetaData() = 0;

		virtual void Reload();

		virtual void Release();
	};

	CLASSFACTORYDEF(Asset)
	CLASSFACTORYDEF_END()
}
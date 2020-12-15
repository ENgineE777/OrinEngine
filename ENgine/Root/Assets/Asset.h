#pragma once

#include "Support/StringUtils.h"
#include "Support/ClassFactory.h"
#include "Support/PointerRef.h"

#ifdef OAK_EDITOR
#include <sys/stat.h>
#endif

namespace Oak
{
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

		#ifdef OAK_EDITOR
		const eastl::string& GetName();
		bool WasChanged();
		#endif

		virtual void Reload();

		virtual void Release();
	};

	CLASSFACTORYDEF(Asset)
	CLASSFACTORYDEF_END()
}
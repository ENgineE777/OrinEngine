#pragma once

#include "Support/StringUtils.h"
#include "Support/ClassFactory.h"

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

	public:

		void SetPath(const char* setPath);
		const eastl::string& GetPath();

		#ifdef OAK_EDITOR
		const eastl::string& GetName();
		bool WasChanged();
		#endif

		virtual void Reload();

		virtual void Release() = 0;
	};

	CLASSFACTORYDEF(Asset)
	CLASSFACTORYDEF_END()
}
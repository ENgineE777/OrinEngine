#pragma once

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include "Support/ThreadExecutor.h"
#include <atomic>
#include "root/Render/Render.h"

namespace Oak
{
	class Asset
	{
		friend class Assets;

	protected:

		eastl::string path;

	public:

		virtual void Reload() = 0;
		virtual void Release() = 0;
	};

	CLASSFACTORYDEF(Asset)
	CLASSFACTORYDEF_END()
}
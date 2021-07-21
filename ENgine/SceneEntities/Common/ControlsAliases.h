
#pragma once

#include "Root/Scenes/SceneEntity.h"

namespace Oak
{
	class ControlsAliases : public SceneEntity
	{
	public:
		META_DATA_DECL(ControlsAliases)

		/**
		\brief Filename of a file with aliases.
		*/

		eastl::string aliases_name;

	#ifndef DOXYGEN_SKIP

		virtual ~ControlsAliases() = default;

		void Init() override;

		bool Play() override;
	#endif
	};
}


#pragma once

#include "SceneEntities/3D/ScriptEntity3D.h"

namespace Oak
{
	class CLASS_DECLSPEC ScriptEntity2D : public ScriptEntity3D
	{
	public:

	#ifndef DOXYGEN_SKIP

		virtual ~ScriptEntity2D() = default;

		void Init() override;
	#endif
	};
}
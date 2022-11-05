
#pragma once

#include "Root/Scenes/SceneEntity.h"

namespace Oak
{
	class CLASS_DECLSPEC ScriptEntity3D : public SceneEntity
	{
		bool taskRegisterd = false;

	public:

	#ifndef DOXYGEN_SKIP

		virtual ~ScriptEntity3D() = default;

		void OnVisiblityChange(bool set) override;

		void Init() override;

		void Play() override;

		virtual void Update(float dt);
	#endif
	};
}
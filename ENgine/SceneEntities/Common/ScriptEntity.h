
#pragma once

#include "Root/Scenes/SceneEntity.h"

namespace Oak
{
	class CLASS_DECLSPEC ScriptEntity : public SceneEntity
	{
	public:
		META_DATA_DECL(ScriptEntity)

	#ifndef DOXYGEN_SKIP

		virtual ~ScriptEntity() = default;

		void SetVisible(bool visible) override;

		void Init() override;

		bool Play() override;

		virtual void Update(float dt);
	#endif
	};
}
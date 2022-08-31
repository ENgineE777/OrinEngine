
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Oak
{
	class PointLight2D : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(PointLight2D)

		Color color;
		ProgramRef spriteLight;

	#ifndef DOXYGEN_SKIP

		PointLight2D();
		virtual ~PointLight2D() = default;

		void Init() override;
		void Draw(float dt);
	#endif
	};
}
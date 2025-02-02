
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"
#include "root/Assets/AssetAnimGraph2D.h"

namespace Orin
{
	class CLASS_DECLSPEC AnimGraph2D : public SceneEntity
	{
	public:

		static bool pause;

		Color color;
		Color emmisive;
		float emmisiveIntencity = 1.0f;
		AssetAnimGraph2DRef anim;

		int lightGroup = 1;
		bool useRimLight = false;

		int drawLevel = 0;
		bool noZ = false;

		META_DATA_DECL_BASE(AnimGraph2D)

	#ifndef DOXYGEN_SKIP

		AnimGraph2D() = default;
		virtual ~AnimGraph2D() = default;

		void Init() override;
		void ApplyProperties() override;
		void Draw(float dt);
		void DrawOccluder(float dt);
		void DrawMask(float dt);
	#endif
	};
}

#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Orin
{
	class PointLight2DProgram : public RenderTechnique
	{
	public:
		const char* GetVsName() override;
		const char* GetPsName() override;

		void ApplyStates() override;
	};

	class PointLight2D : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(PointLight2D)

		Color color;
		float falloff = 0.5f;
		float intesity = 1.0f;
		float lineWidth = 0.0f;
		float viewAngle = 181.0f;
		bool castShadow = false;
		RenderTechniqueRef spriteLight;

	#ifndef DOXYGEN_SKIP

		PointLight2D();
		virtual ~PointLight2D() = default;

		void Init() override;
		void Draw(float dt);

		void GetBBox(Math::Vector3& vMin, Math::Vector3& vMax) override;
	#endif
	};
}

#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Orin
{
	class CLASS_DECLSPEC DefferedLight : public SceneEntity
	{
		friend class SpriteEntity;
		friend class TileMap;
		friend class AnimGraph2D;

		TextureRef albedoRT;
		TextureRef materialRT;
		TextureRef normalRT;
		TextureRef selfilumRT;

		VertexDeclRef vdecl;
		DataBufferRef buffer;

		TextureRef tempRT;

		Color ambientColor;
		float directionalDir = 0.0f;
		Color directionalColor;

		float metallic = 0.25f;

		static bool hackStateEnabled;
		static RenderTechniqueRef gbufferTech;
		RenderTechniqueRef blurRTech;

		RenderTechniqueRef defferdLightTech;

		float ComputeGaussian(float n);
		void BlurTexture(TextureRef src, TextureRef dest, float blurStrength);

	public:

		META_DATA_DECL_BASE(DefferedLight)

	#ifndef DOXYGEN_SKIP

		DefferedLight() = default;
		virtual ~DefferedLight() = default;

		void Init() override;

		void SetRT(float dt);
		void Draw(float dt);
		void Release() override;
	#endif
	};
}

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
		TextureRef occluderRT;

		TextureRef shadowRT;

		VertexDeclRef vdecl;
		DataBufferRef buffer;

		TextureRef tempRT;

		int numBlurSelfIlum = 4;
		float streangthBlurSelfIlum = 0.75f;
		float powerSelfIlum = 4.0f;

		struct GlobalLight
		{
			META_DATA_DECL_BASE(GlobalLight)

			Color ambientColor;
			float ambientIntensity = 1.0f;			
		};

		eastl::vector<GlobalLight> globalLights;

		float metallic = 0.25f;

		constexpr static int MAX_LIGHTS = 32;

		static bool hackStateEnabled;
		static RenderTechniqueRef gbufferTech;
		RenderTechniqueRef blurRTech;

		eastl::vector<class DirectionLight2D*> dirLights;
		eastl::vector<class PointLight2D*> lights;
		RenderTechniqueRef shadowCastTech;
		RenderTechniqueRef shadowRenderTech;
		RenderTechniqueRef defferdLightTech;

		float ComputeGaussian(float n);
		void BlurTexture(TextureRef src, TextureRef dest, float blurStrength);
		void GatherLights();
		void GenerateShadows();
		void BlurSelfIlum();
		void RenderLights();

		float timer = 0.0f;		

	public:

		bool useFilter = false;

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
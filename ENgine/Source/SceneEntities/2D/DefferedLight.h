
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
		eastl::vector<TextureRef> selfilumRTs;

		TextureRef occluderRT;

		TextureRef shadowRT;

		VertexDeclRef vdecl;
		DataBufferRef buffer;

		float contrast = 0.0f;
		float brightness = 0.0f;
		float saturate = 1.0f;

		struct GlobalLight
		{
			META_DATA_DECL_BASE(GlobalLight)

			Color ambientColor;
			float ambientIntensity = 1.0f;
		};

		eastl::vector<GlobalLight> globalLights;

		float metallic = 0.25f;

		constexpr static int MAX_LIGHTS = 64;

		static bool hackStateEnabled;
		static float lightGroupDivider;
		static RenderTechniqueRef gbufferTech;
		RenderTechniqueRef blurRTech;
		RenderTechniqueRef blurDownRTech;
		RenderTechniqueRef blurUpRTech;

		eastl::vector<class DirectionLight2D*> dirLights;
		eastl::vector<class PointLight2D*> lights;
		RenderTechniqueRef shadowCastTech;
		RenderTechniqueRef shadowRenderTech;
		RenderTechniqueRef defferdLightTech;

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
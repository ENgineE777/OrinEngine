
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Orin
{
	class CLASS_DECLSPEC DefferedLight : public SceneEntity
	{
		struct Material
		{
			TextureRef albedo;
			TextureRef material;
			TextureRef normal;
		};

		Material ground;
		Material box;
		
		TextureRef albedoRT;
		TextureRef materialRT;
		TextureRef normalRT;

		TextureRef sceneDepth;

		RenderTechniqueRef gbufferTech;
		RenderTechniqueRef defferdLightTech;

	public:

		META_DATA_DECL_BASE(DefferedLight)

	#ifndef DOXYGEN_SKIP

		DefferedLight() = default;
		virtual ~DefferedLight() = default;

		void Init() override;

		void SetRT(float dt);
		void Draw(float dt);

	#endif
	};
}
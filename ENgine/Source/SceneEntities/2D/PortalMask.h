
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Root/Assets/AssetSpritesLayer.h"

namespace Orin
{
	class PortalMask : public SceneEntity
	{
		TextureRef maskRT;

	public:
		
		RenderTechniqueRef quadMaskedPrg;
		RenderTechniqueRef quadMaskedDefferedPrg;

		META_DATA_DECL(SpritesLayer)
		
	#ifndef DOXYGEN_SKIP

		virtual ~PortalMask() = default;

		void Init() override;
		void ApplyProperties() override;
		void Draw(float dt);
	#endif
	};
}


#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Oak
{
	class CLASS_DECLSPEC Lights2DRenderer : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(Lights2DRenderer)

		RenderTechniqueRef spriteLight;
		RenderTechniqueRef lightMap2DProgram;
		VertexDeclRef vdecl;
		TextureRef lightMapRT;
		DataBufferRef buffer;

		Color ambientColor;

		struct Instance
		{
			bool active = false;
			Math::Vector3 position;
			float radius = 10.0f;
			Color color;
		};

		eastl::vector<Instance> instances;

	#ifndef DOXYGEN_SKIP

		Lights2DRenderer() = default;
		virtual ~Lights2DRenderer() = default;

		void Init() override;
		void Draw(float dt);

		int AddInstance(Math::Vector3 position,	float radius, Color color);
		void UpdateInstance(int id, Math::Vector3 position, float radius, Color color);
		void ReleaseInstance(int id);
	#endif
	};
}
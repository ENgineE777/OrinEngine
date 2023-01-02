
#include "Lights2DRenderer.h"
#include "Root/Root.h"
#include "PointLight2D.h"

namespace Orin
{
	class LightMap2DProgram : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "drawLightMap2D_vs.shd"; };
		virtual const char* GetPsName() { return "drawLightMap2D_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetBlendFunc(BlendArg::ArgDestColor, BlendArg::ArgZero);
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	ENTITYREG(SceneEntity, Lights2DRenderer, "2D/Lights", "Lights2DRenderer")

	META_DATA_DESC(Lights2DRenderer)
		BASE_SCENE_ENTITY_PROP(Lights2DRenderer)
		COLOR_PROP(Lights2DRenderer, ambientColor, COLOR_WHITE, "Visual", "Ambient color")
	META_DATA_DESC_END()

	void Lights2DRenderer::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY;

		Tasks(true)->AddTask(100, this, (Object::Delegate) & Lights2DRenderer::Draw);

		spriteLight = root.render.GetRenderTechnique<PointLight2DProgram>(_FL_);
		lightMap2DProgram = root.render.GetRenderTechnique<LightMap2DProgram>(_FL_);

		lightMapRT = root.render.GetDevice()->CreateTexture(512, 512, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
		
		VertexDecl::ElemDesc desc[] = { { ElementType::Float2, ElementSemantic::Position, 0 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(1, desc, _FL_);

		buffer = root.render.GetDevice()->CreateBuffer(4, sizeof(Math::Vector2), _FL_);

		Math::Vector2* v = (Math::Vector2*)buffer->Lock();

		v[0] = Math::Vector2(-1, 1);
		v[1] = Math::Vector2(1, 1);
		v[2] = Math::Vector2(-1, -1);
		v[3] = Math::Vector2(1, -1);

		buffer->Unlock();
	}

	void Lights2DRenderer::Draw(float dt)
	{
		root.render.GetDevice()->SetRenderTarget(0, lightMapRT);
		root.render.GetDevice()->SetDepth(nullptr);

		root.render.GetDevice()->Clear(true, Color(ambientColor.r, ambientColor.g, ambientColor.b, 1.0f), false, 1.0f);

		root.render.ExecutePool(555, dt);

		for (auto& inst : instances)
		{
			if (!inst.active)
			{
				continue;
			}

			Math::Matrix mat;
			mat.Pos() = inst.position;

			Sprite::Draw(nullptr, inst.color, mat, Math::Vector2(-inst.radius, inst.radius), inst.radius * 2.0f, 0.0f, 1.0f, spriteLight);
		}

		root.render.GetDevice()->RestoreRenderTarget();

		root.render.GetDevice()->SetVertexDecl(vdecl);
		root.render.GetDevice()->SetVertexBuffer(0, buffer);

		root.render.GetDevice()->SetRenderTechnique(lightMap2DProgram);

		lightMap2DProgram->SetTexture(ShaderType::Pixel, "colorMap", lightMapRT);

		root.render.GetDevice()->Draw(PrimitiveTopology::TriangleStrip, 0, 2);

		//root.render.DebugSprite(lightMapRT, 5.0f, 100.0f);
	}

	int Lights2DRenderer::AddInstance(Math::Vector3 position, float radius, Color color)
	{
		int id = -1;

		for (int i = 0; i < (int)instances.size(); i++)
		{
			if (!instances[i].active)
			{
				id = i;
				break;
			}
		}

		if (id == -1)
		{
			instances.push_back(Instance());

			id = (int)instances.size() - 1;
		}

		auto& inst = instances[id];

		inst.active = true;
		inst.position = position;
		inst.radius = radius;
		inst.color = color;

		return id;
	}

	void Lights2DRenderer::UpdateInstance(int id, Math::Vector3 position, float radius, Color color)
	{
		if (id >= 0 && id < (int)instances.size())
		{
			auto& inst = instances[id];

			inst.position = position;
			inst.radius = radius;
			inst.color = color;
		}
	}

	void Lights2DRenderer::ReleaseInstance(int id)
	{
		if (id >=0 && id < (int)instances.size())
		{
			instances[id].active = false;
		}
	}
}
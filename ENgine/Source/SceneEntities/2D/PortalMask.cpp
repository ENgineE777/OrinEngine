
#include "PortalMask.h"
#include "Root/Root.h"

namespace Orin
{
	class QuadRenderMaskedTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "sprite_masked_vs.shd"; };
		virtual const char* GetPsName() { return "sprite_masked_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	class QuadRenderMaskedLightenTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "sprite_masked_vs.shd"; };
		virtual const char* GetPsName() { return "sprite_masked_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetBlendFunc(BlendArg::ArgSrcAlpha, BlendArg::ArgOne);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	class QuadRenderMaskedDefferedTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "sprite_masked_vs.shd"; };
		virtual const char* GetPsName() { return "gbuffer_masked_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	ENTITYREG(SceneEntity, PortalMask, "2D/Sprites", "PortalMask")

	META_DATA_DESC(PortalMask)
		BASE_SCENE_ENTITY_PROP(PortalMask)
	META_DATA_DESC_END()

	void PortalMask::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RectMoveXY;

		quadMaskedPrg = root.render.GetRenderTechnique<QuadRenderMaskedTechnique>(_FL_);
		quadMaskedLightenPrg = root.render.GetRenderTechnique<QuadRenderMaskedLightenTechnique>(_FL_);
		quadMaskedDefferedPrg = root.render.GetRenderTechnique<QuadRenderMaskedDefferedTechnique>(_FL_);
	}

	void PortalMask::ApplyProperties()
	{
#ifdef ORIN_EDITOR
		Tasks(true)->DelAllTasks(this);
#endif

		Tasks(true)->AddTask( -15, this, (Object::Delegate)&PortalMask::Draw);
	}


	void PortalMask::Draw(float dt)
	{
		int screenWidth = root.render.GetDevice()->GetWidth();
		int screenHeight = root.render.GetDevice()->GetHeight();;

		if (!maskRT || maskRT->GetWidth() != screenWidth || maskRT->GetHeight() != screenHeight)
		{
			maskRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			maskRT->SetAdress(TextureAddress::Clamp);
		}

		root.render.GetDevice()->SetRenderTarget(0, maskRT);

		root.render.GetDevice()->Clear(true, COLOR_BLACK_A(1.0f), true, 1.0f);
		
		root.render.ExecutePool(550, 0.0f);

		root.render.GetDevice()->RestoreRenderTarget();

		quadMaskedPrg->SetTexture(ShaderType::Pixel, "maskMap", maskRT);
		quadMaskedLightenPrg->SetTexture(ShaderType::Pixel, "maskMap", maskRT);
		quadMaskedDefferedPrg->SetTexture(ShaderType::Pixel, "maskMap", maskRT);
	}
}

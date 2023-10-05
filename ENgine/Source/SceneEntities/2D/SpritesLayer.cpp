
#include "SpritesLayer.h"
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

	ENTITYREG(SceneEntity, SpritesLayer, "2D/Sprites", "SpritesLayer")

	META_DATA_DESC(SpritesLayer)
		BASE_SCENE_ENTITY_PROP(SpritesLayer)
		FLOAT_PROP(SpritesLayer, paralax.x, 0.0f, "Geometry", "paralax X", "X-axis paralax")
		FLOAT_PROP(SpritesLayer, paralax.y, 0.0f, "Geometry", "paralax Y", "Y-axis paralax")
		INT_PROP(SpritesLayer, drawLevel, 0, "Geometry", "draw_level", "Draw priority")
		BOOL_PROP(SpritesLayer, usePortlas, false, "Geometry", "usePortlas", "Draw priority")
		ASSET_SPRITES_LAYER_PROP(SpritesLayer, spritesAsset, "Visual", "SpritesLayer")
	META_DATA_DESC_END()

	void SpritesLayer::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RectMoveXY;

		quadMaskedPrg = root.render.GetRenderTechnique<QuadRenderMaskedTechnique>(_FL_);
	}

	void SpritesLayer::ApplyProperties()
	{
#ifdef ORIN_EDITOR
		Tasks(true)->DelAllTasks(this);
#endif

		Tasks(true)->AddTask(0 + drawLevel, this, (Object::Delegate)&SpritesLayer::Draw);
	}


	void SpritesLayer::Draw(float dt)
	{
		Math::Vector2 size = spritesAsset.GetSize();
		transform.size = size;

		if (usePortlas && drawLevel == 7)
		{
			int screenWidth = root.render.GetDevice()->GetWidth();
			int screenHeight = root.render.GetDevice()->GetHeight();;

			if (!maskRT || maskRT->GetWidth() != screenWidth || maskRT->GetHeight() != screenHeight)
			{
				maskRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
				maskRT->SetAdress(TextureAddress::Clamp);
			}

			root.render.GetDevice()->SetRenderTarget(0, maskRT);
			root.render.GetDevice()->Clear(true, COLOR_BLACK_A(1.0f), false, 1.0f);

			root.render.ExecutePool(550, 0.0f);

			root.render.GetDevice()->RestoreRenderTarget();

			root.render.DebugSprite(maskRT, 10.0f, 100.0f);

			quadMaskedPrg->SetTexture(ShaderType::Pixel, "maskMap", maskRT);
		}
		

		if (IsVisible())
		{
			auto* asset = spritesAsset.Get();

			Math::Vector3 camPos = 0.0f;
			int from = 0;
			int to = 0;

			Math::Matrix view;
			root.render.GetTransform(TransformStage::View, view);
			view.Inverse();

			camPos = Sprite::ToPixels(view.Pos());

			float scale = root.render.GetDevice()->GetHeight() / Sprite::GetPixelsHeight();
			to = (int)(((root.render.GetDevice()->GetWidth() / scale) / size.x + 1) * 0.5f);
			from = -to;

			float paralaxedX = transform.position.x + (camPos.x - transform.position.x) * (1.0f - paralax.x);

			int offset = (int)((camPos.x - paralaxedX) / size.x);

			for (int x = from - 1; x <= to + 1; x++)
			{
				Math::Vector3 pos = transform.position;

				if (GetScene()->IsPlaying())
				{
					pos.x = paralaxedX + (x + offset) * size.x;
					pos.y = transform.position.y + (camPos.y - transform.position.y) * (1.0f - paralax.y);
				}

				spritesAsset.prg = usePortlas ? quadMaskedPrg : Sprite::quadPrg;
				spritesAsset.Draw(pos, COLOR_WHITE, dt);
			}
		}
	}
}

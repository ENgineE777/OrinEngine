
#include "SpritesLayer.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, SpritesLayer, "SpritesLayer")

	META_DATA_DESC(SpritesLayer)
		BASE_SCENE_ENTITY_PROP(SpritesLayer)
		FLOAT_PROP(SpritesLayer, paralax.x, 0.0f, "Geometry", "paralax X", "X-axis paralax")
		FLOAT_PROP(SpritesLayer, paralax.y, 0.0f, "Geometry", "paralax Y", "Y-axis paralax")
		INT_PROP(SpritesLayer, drawLevel, 0, "Geometry", "draw_level", "Draw priority")
		ASSET_SPRITES_LAYER_PROP(SpritesLayer, spritesAsset, "Visual", "SpritesLayer")
	META_DATA_DESC_END()

	void SpritesLayer::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RectMoveXY;

		Tasks(true)->AddTask(0 + drawLevel, this, (Object::Delegate)&SpritesLayer::Draw);
	}

	void SpritesLayer::Draw(float dt)
	{
		Math::Vector2 size = spritesAsset.GetSize();
		transform.size = size;

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

				spritesAsset.Draw(pos, COLOR_WHITE, dt);
			}
		}
	}
}

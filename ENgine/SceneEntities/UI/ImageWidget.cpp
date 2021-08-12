
#include "ImageWidget.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, ImageWidget, "ImageWidget")

	META_DATA_DESC(ImageWidget)
		BASE_SCENE_ENTITY_PROP(ImageWidget)
		ASSET_TEXTURE_PROP(ImageWidget, texture, "Visual", "Texture")
	META_DATA_DESC_END()

	ImageWidget::ImageWidget() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void ImageWidget::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = SpriteTransformFlags;

		Tasks(true)->AddTask(0, this, (Object::Delegate) & ImageWidget::Draw);
	}

	void ImageWidget::ApplyProperties()
	{
		Math::Vector2 size = texture.GetSize();
		transform.size = Math::Vector3(size.x, size.y, 0.0f);
	}

	void ImageWidget::Draw(float dt)
	{
		transform.BuildMatrices();

		transform.global.Pos().y = Sprite::pixelsHeight - transform.global.Pos().y;

		if (scene->IsPlaying())
		{
			Math::Matrix view;
			root.render.GetTransform(TransformStage::View, view);

			view.Inverse();

			view.Pos() *= Sprite::pixelsPerUnit;

			float k = Sprite::pixelsHeight / root.render.GetDevice()->GetHeight();

			transform.global.Pos().x += view.Pos().x - root.render.GetDevice()->GetWidth() *  0.5f * k;
			transform.global.Pos().y += view.Pos().y - root.render.GetDevice()->GetHeight() * 0.5f * k;
		}

		texture.Draw(&transform, COLOR_WHITE, dt);
	}
}
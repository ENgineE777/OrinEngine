
#include "ImageWidget.h"
#include "Root/Root.h"

namespace Oak
{
	ENTITYREG(SceneEntity, ImageWidget, "UI", "ImageWidget")

	META_DATA_DESC(ImageWidget)
		BASE_SCENE_ENTITY_PROP(ImageWidget)
		ASSET_TEXTURE_PROP(ImageWidget, texture, "Visual", "Texture")

		ENUM_PROP(ImageWidget, horzAlign, 0, "Prop", "Horz align", "Horizontal aligment of a widget")
			ENUM_ELEM("Left", 0)
			ENUM_ELEM("Center", 1)
			ENUM_ELEM("Right", 2)
		ENUM_END
		ENUM_PROP(ImageWidget, vertAlign, 0, "Prop", "Vert align", "Vertical aligment of a widget")
			ENUM_ELEM("Top", 3)
			ENUM_ELEM("Center", 1)
			ENUM_ELEM("Bottom", 4)
		ENUM_END
		ENUM_PROP(ImageWidget, horzSize, 0, "Prop", "Horz size", "Type of width of a widget")
			ENUM_ELEM("Fixed", 0)
			ENUM_ELEM("Fill parent", 1)
			ENUM_ELEM("Wrap content", 2)
		ENUM_END
		ENUM_PROP(ImageWidget, vertSize, 0, "Prop", "Vert size", "Type of height of a widget")
			ENUM_ELEM("Fixed", 0)
			ENUM_ELEM("Fill parent", 1)
			ENUM_ELEM("Wrap content", 2)
		ENUM_END
		FLOAT_PROP(ImageWidget, leftPadding.x, 0.0f, "Prop", "left padding", "Left padding of a widget")
		FLOAT_PROP(ImageWidget, leftPadding.y, 0.0f, "Prop", "top padding", "Top padding of a widget")
		FLOAT_PROP(ImageWidget, rightPadding.x, 0.0f, "Prop", "right padding", "Right padding of a widget")
		FLOAT_PROP(ImageWidget, rightPadding.y, 0.0f, "Prop", "bottom padding", "Bottom padding of a widget")
		COLOR_PROP(ImageWidget, color, COLOR_WHITE, "Prop", "color")

	META_DATA_DESC_END()

	void ImageWidget::ApplyProperties()
	{
		if (texture.Get())
		{
			Math::Vector2 size = texture.GetSize();
			transform.size = Math::Vector3(size.x, size.y, 0.0f);
		}
	}

	void ImageWidget::Draw(float dt)
	{
		texture.prg = Sprite::quadPrgNoZ;
		texture.Draw(&transform, curColor, dt);
	}
}

#include "LabelWidget.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, LabelWidget, "LabelWidget")

	META_DATA_DESC(LabelWidget)
		BASE_SCENE_ENTITY_PROP(LabelWidget)
		INT_PROP(LabelWidget, fontHeight, 20, "Property", "Font Height", "Font Height")
		FILENAME_PROP(LabelWidget, fontName, "", "Property", "Font Name")
		STRING_PROP(LabelWidget, text, "Text", "Property", "Text")

		ENUM_PROP(LabelWidget, horzAlign, 0, "Prop", "Horz align", "Horizontal aligment of a widget")
			ENUM_ELEM("Left", 0)
			ENUM_ELEM("Center", 1)
			ENUM_ELEM("Right", 2)
		ENUM_END
		ENUM_PROP(LabelWidget, vertAlign, 0, "Prop", "Vert align", "Vertical aligment of a widget")
			ENUM_ELEM("Top", 3)
			ENUM_ELEM("Center", 1)
			ENUM_ELEM("Bottom", 4)
		ENUM_END
		ENUM_PROP(LabelWidget, horzSize, 0, "Prop", "Horz size", "Type of width of a widget")
			ENUM_ELEM("Fixed", 0)
			ENUM_ELEM("Fill parent", 1)
			ENUM_ELEM("Wrap content", 2)
		ENUM_END
		ENUM_PROP(LabelWidget, vertSize, 0, "Prop", "Vert size", "Type of height of a widget")
			ENUM_ELEM("Fixed", 0)
			ENUM_ELEM("Fill parent", 1)
			ENUM_ELEM("Wrap content", 2)
		ENUM_END
		FLOAT_PROP(LabelWidget, leftPadding.x, 0.0f, "Prop", "left padding", "Left padding of a widget")
		FLOAT_PROP(LabelWidget, leftPadding.y, 0.0f, "Prop", "top padding", "Top padding of a widget")
		FLOAT_PROP(LabelWidget, rightPadding.x, 0.0f, "Prop", "right padding", "Right padding of a widget")
		FLOAT_PROP(LabelWidget, rightPadding.y, 0.0f, "Prop", "bottom padding", "Bottom padding of a widget")
		COLOR_PROP(LabelWidget, color, COLOR_WHITE, "Prop", "color")

	META_DATA_DESC_END()

	void LabelWidget::ApplyProperties()
	{
		float k = root.render.GetDevice()->GetHeight() / Sprite::pixelsHeight;
		font = root.fonts.LoadFont(fontName.c_str(), false, false, (int)(k * (float)fontHeight), _FL_);
	}

	void LabelWidget::Draw(float dt)
	{
		Math::Vector3 pos3d = Math::Vector3(transform.GetGlobal().Pos().x * Sprite::pixelsPerUnitInvert, transform.GetGlobal().Pos().y * Sprite::pixelsPerUnitInvert, 0.0f);

		pos3d = root.render.TransformToScreen(pos3d, 2);

		Math::Matrix mat;
		mat.Pos().x = pos3d.x;
		mat.Pos().y = pos3d.y;

		font.Print(mat, 1.0f, curColor, text.c_str());
	}
}

#include "LabelWidget.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, LabelWidget, "UI", "LabelWidget")

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
		screenHeight = -1;
	}

	void LabelWidget::Draw(float dt)
	{
		int height = root.render.GetDevice()->GetHeight();
		float k = (float)height / Sprite::GetPixelsHeight();

		if (height != screenHeight)
		{
			int calculatedFontHeight = (int)(k * (float)fontHeight);
			font = root.fonts.LoadFont(fontName.c_str(), false, false, calculatedFontHeight, _FL_);

			screenHeight = height;
		}
		
		const char* txt = text.c_str();

		if (txt[0] == '$' && txt[1] != 0)
		{
			eastl::string key = &txt[1];
			txt = root.localization.GetLocalized(key);
		}

		if (horzSize == Size::wrapContext || vertSize == Size::wrapContext)
		{						
			float height = font.GetLineBreak(line_breaks, txt, 10000);

			float width = 1.0f;

			if (line_breaks.size() > 0)
			{

				for (auto& entry : line_breaks)
				{
					if (entry.width > width)
					{
						width = entry.width;
					}
				}
			}

			if (horzSize == Size::wrapContext)
			{
				transform.size.x = width / k;
			}

			if (vertSize == Size::wrapContext)
			{
				transform.size.y = height / k;
			}
		}

		auto offset = Sprite::ToUnits(transform.offset * transform.size);
		offset.y = -offset.y;

		auto pos3d = root.render.TransformToScreen(transform.GetGlobal().Pos() - offset, 2);

		Math::Matrix mat;
		mat.Pos().x = pos3d.x;
		mat.Pos().y = pos3d.y;

		font.Print(mat, 1.0f, curColor, txt);
	}
}

#include "ContainerWidget.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, ContainerWidget, "ContainerWidget")

	META_DATA_DESC(ContainerWidget)
		BASE_SCENE_ENTITY_PROP(ContainerWidget)

		ENUM_PROP(ContainerWidget, horzAlign, 0, "Prop", "Horz align", "Horizontal aligment of a widget")
			ENUM_ELEM("Left", 0)
			ENUM_ELEM("Center", 1)
			ENUM_ELEM("Right", 2)
		ENUM_END
		ENUM_PROP(ContainerWidget, vertAlign, 0, "Prop", "Vert align", "Vertical aligment of a widget")
			ENUM_ELEM("Top", 3)
			ENUM_ELEM("Center", 1)
			ENUM_ELEM("Bottom", 4)
		ENUM_END
		ENUM_PROP(ContainerWidget, horzSize, 0, "Prop", "Horz size", "Type of width of a widget")
			ENUM_ELEM("Fixed", 0)
			ENUM_ELEM("Fill parent", 1)
			ENUM_ELEM("Wrap content", 2)
		ENUM_END
		ENUM_PROP(ContainerWidget, vertSize, 0, "Prop", "Vert size", "Type of height of a widget")
			ENUM_ELEM("Fixed", 0)
			ENUM_ELEM("Fill parent", 1)
			ENUM_ELEM("Wrap content", 2)
		ENUM_END
		FLOAT_PROP(ContainerWidget, leftPadding.x, 0.0f, "Prop", "left padding", "Left padding of a widget")
		FLOAT_PROP(ContainerWidget, leftPadding.y, 0.0f, "Prop", "top padding", "Top padding of a widget")
		FLOAT_PROP(ContainerWidget, rightPadding.x, 0.0f, "Prop", "right padding", "Right padding of a widget")
		FLOAT_PROP(ContainerWidget, rightPadding.y, 0.0f, "Prop", "bottom padding", "Bottom padding of a widget")
		COLOR_PROP(ContainerWidget, color, COLOR_WHITE, "Prop", "color")

	META_DATA_DESC_END()

	void ContainerWidget::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = SpriteTransformFlags;

		Tasks(true)->AddTask(199, this, (Object::Delegate)&ContainerWidget::FullDraw);
	}

	void ContainerWidget::CalcState()
	{
		Math::Vector3 parentSize;

		ContainerWidget* parentWidget = dynamic_cast<ContainerWidget*>(parent);

		if (parentWidget)
		{
			parentSize = parentWidget->GetTransform().size;
			curColor = parentWidget->color * color;
		}
		else
		{
			parentSize.x = root.render.GetDevice()->GetWidth() * Sprite::pixelsHeight / root.render.GetDevice()->GetHeight();
			parentSize.y = Sprite::pixelsHeight;

			curColor = color;
		}

		if (horzSize == Size::fillParent)
		{
			transform.position.x = 0;
			transform.size.x = parentSize.x - rightPadding.x - leftPadding.x;
		}

		if (vertSize == Size::fillParent)
		{
			transform.position.y = 0;
			transform.size.y = parentSize.y - rightPadding.y - leftPadding.y;
		}

		transform.BuildMatrices();

		if (parentWidget)
		{
			Math::Vector3 offset = parentWidget->GetTransform().size * parentWidget->GetTransform().offset;
			transform.global.Pos().x -= offset.x;
			transform.global.Pos().y -= offset.y;
		}

		if (horzAlign == Align::alignLeft)
		{
			transform.global.Pos().x += leftPadding.x;
		}
		else
		if (horzAlign == Align::alignCenter)
		{
			transform.global.Pos().x += leftPadding.x + (parentSize.x - rightPadding.x - leftPadding.x) * 0.5f;
		}
		else
		if (horzAlign == Align::alignRight)
		{
			transform.global.Pos().x += -rightPadding.x + parentSize.x;
		}

		if (vertAlign == Align::alignTop)
		{
			transform.global.Pos().y += leftPadding.y;
		}
		else
		if (vertAlign == Align::alignCenter)
		{
			transform.global.Pos().y += leftPadding.y + (parentSize.y - rightPadding.x - leftPadding.x) * 0.5f;
		}
		else
		if (vertAlign == Align::alignBottom)
		{
			transform.global.Pos().y += (-rightPadding.y) + parentSize.y;
		}

		//transform.global.Pos().y = Sprite::pixelsHeight - transform.global.Pos().y;

		if (scene->IsPlaying() && !parent)
		{
			Math::Matrix view;
			root.render.GetTransform(TransformStage::View, view);

			view.Inverse();

			view.Pos() *= Sprite::pixelsPerUnit;

			float k = Sprite::pixelsHeight / root.render.GetDevice()->GetHeight();

			transform.global.Pos().x += view.Pos().x - root.render.GetDevice()->GetWidth() * 0.5f * k;
			transform.global.Pos().y += view.Pos().y - root.render.GetDevice()->GetHeight() * 0.5f * k;
		}

		//trans.axis.x = (horzAlign == align_right) ? -1.0f : 1.0f;
		//trans.axis.y = (vertAlign == align_bottom) ? -1.0f : 1.0f;
	}

	void ContainerWidget::FullDraw(float dt)
	{
		if (parent)
		{
			return;
		}

		DrawSelfWithChilds(dt);
	}

	void ContainerWidget::DrawSelfWithChilds(float dt)
	{
		if (!visible)
		{
			return;
		}

		CalcState();

		Draw(dt);

		for (auto* child : childs)
		{
			ContainerWidget* childWidget = dynamic_cast<ContainerWidget*>(child);

			if (childWidget)
			{
				childWidget->DrawSelfWithChilds(dt);
			}
		}
	}

	void ContainerWidget::Draw(float dt)
	{

	}
}
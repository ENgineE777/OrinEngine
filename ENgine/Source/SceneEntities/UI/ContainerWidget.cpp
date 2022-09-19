
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
			transform.position = Math::Vector3(0.0f, transform.position.y, transform.position.z);
			transform.size.x = parentSize.x - rightPadding.x - leftPadding.x;
		}

		if (vertSize == Size::fillParent)
		{
			transform.position = Math::Vector3(transform.position.y, 0.0f, transform.position.z);
			transform.size.y = parentSize.y - rightPadding.y - leftPadding.y;
		}

		transform.axis.x = (horzAlign == Align::alignRight) ? -1.0f : 1.0f;
		transform.axis.y = (vertAlign == Align::alignBottom) ? 1.0f : -1.0f;

		Math::Matrix global = transform.GetGlobal();

		if (parentWidget)
		{
			Math::Vector3 offset = parentWidget->GetTransform().size * parentWidget->GetTransform().offset;
			global.Pos().x += -offset.x;
			global.Pos().y += offset.y;
		}

		if (horzAlign == Align::alignLeft)
		{
			global.Pos().x += leftPadding.x;
		}
		else
		if (horzAlign == Align::alignCenter)
		{
			global.Pos().x += leftPadding.x + (parentSize.x - rightPadding.x - leftPadding.x) * 0.5f + transform.size.x * (transform.offset.x - 0.5f);
		}
		else
		if (horzAlign == Align::alignRight)
		{
			global.Pos().x += -rightPadding.x - transform.size.x + parentSize.x;
		}

		if (vertAlign == Align::alignTop)
		{
			global.Pos().y -= leftPadding.y;
		}
		else
		if (vertAlign == Align::alignCenter)
		{
			global.Pos().y -= leftPadding.y + (parentSize.y - rightPadding.y - leftPadding.y) * 0.5f + transform.size.y * (transform.offset.y - 0.5f);
		}
		else
		if (vertAlign == Align::alignBottom)
		{
			global.Pos().y -= -rightPadding.y - transform.size.y + parentSize.y;
		}

		transform.SetGlobal(global, false);
	}

	void ContainerWidget::FullDraw(float dt)
	{
		if (parent)
		{
			return;
		}

		Math::Vector2 pos = Math::Vector2(Sprite::pixelsHeight / root.render.GetDevice()->GetAspect(), -Sprite::pixelsHeight) * 0.5f * Sprite::pixelsPerUnitInvert;

		float dist = (Sprite::pixelsHeight * 0.5f * Sprite::pixelsPerUnitInvert) / (tanf(22.5f * Math::Radian));
		Math::Matrix view;
		view.BuildView(Math::Vector3(pos.x, pos.y, -dist), Math::Vector3(pos.x, pos.y, -dist + 1.0f), Math::Vector3(0, 1, 0));

		root.render.SetTransform(TransformStage::View, view);

		Math::Matrix proj;
		proj.BuildProjection(45.0f * Math::Radian, (float)root.render.GetDevice()->GetHeight() / (float)root.render.GetDevice()->GetWidth(), 1.0f, 1000.0f);
		root.render.SetTransform(TransformStage::Projection, proj);

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
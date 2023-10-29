
#include "ContainerWidget.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, ContainerWidget, "UI", "ContainerWidget")

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
		transform.transformFlag = SpriteTransformFlags;
		transform.objectType = ObjectType::ObjectUI;

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
			parentSize.x = root.render.GetDevice()->GetWidth() * Sprite::GetPixelsHeight() / root.render.GetDevice()->GetHeight();
			parentSize.y = Sprite::GetPixelsHeight();

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

		if ((horzSize == Size::wrapContext || vertSize == Size::wrapContext) && childs.size() > 0)
		{
			float width = 1.0f;
			float height = 1.0f;

			for (auto* child : childs)
			{
				ContainerWidget* childWidget = dynamic_cast<ContainerWidget*>(child);

				if (childWidget)
				{
					auto& trans = childWidget->GetTransform();

					auto size = (1.0f - trans.offset) * trans.size;
					size.y = -size.y;

					auto pos = trans.position + size;

					if (pos.x > width)
					{
						width = pos.x;
					}

					if (-pos.y > height)
					{
						height = -pos.y;
					}
				}
			}

			if (horzSize == Size::wrapContext)
			{
				transform.size.x = width;
			}
				
			if (vertSize == Size::wrapContext)
			{
				transform.size.y = height;
			}
		}

		transform.axis.x = (horzAlign == Align::alignRight) ? -1.0f : 1.0f;
		transform.axis.y = (vertAlign == Align::alignBottom) ? 1.0f : -1.0f;

		//HAck to force recalc transform
		transform.position = transform.position;

		Math::Matrix global = transform.GetGlobal();
		auto globalPos = Sprite::ToPixels(global.Pos());

		if (parentWidget)
		{
			Math::Vector3 offset = parentWidget->GetTransform().size * parentWidget->GetTransform().offset;
			globalPos.x += -offset.x;
			globalPos.y += offset.y;
		}

		if (horzAlign == Align::alignLeft)
		{
			globalPos.x += leftPadding.x;
		}
		else
		if (horzAlign == Align::alignCenter)
		{
			globalPos.x += leftPadding.x + (parentSize.x - rightPadding.x - leftPadding.x) * 0.5f + transform.size.x * (transform.offset.x - 0.5f) * transform.scale.x;
		}
		else
		if (horzAlign == Align::alignRight)
		{
			globalPos.x += -rightPadding.x - transform.size.x * transform.scale.x + parentSize.x;
		}

		if (vertAlign == Align::alignTop)
		{
			globalPos.y -= leftPadding.y;
		}
		else
		if (vertAlign == Align::alignCenter)
		{
			globalPos.y -= leftPadding.y + (parentSize.y - rightPadding.y - leftPadding.y) * 0.5f + transform.size.y * (transform.offset.y - 0.5f);
		}
		else
		if (vertAlign == Align::alignBottom)
		{
			globalPos.y -= -rightPadding.y - transform.size.y * transform.scale.y + parentSize.y;
		}

		global.Pos() = Sprite::ToUnits(globalPos);

		transform.SetGlobal(global, false);
	}

	void ContainerWidget::FullDraw(float dt)
	{
		if (parent)
		{
			return;
		}

		Math::Matrix prevView;
		root.render.GetTransform(TransformStage::View, prevView);

		Math::Matrix prevProj;
		root.render.GetTransform(TransformStage::Projection, prevProj);

		Math::Vector2 pos = Sprite::ToUnits(Math::Vector2(Sprite::Sprite::GetPixelsHeight() / root.render.GetDevice()->GetAspect(), -Sprite::GetPixelsHeight()) * 0.5f);

		float dist = Sprite::GetPixelsHeight() * 0.5f * Sprite::ToUnits(1.0f) / (tanf(22.5f * Math::Radian));
		Math::Matrix view;
		view.BuildView(Math::Vector3(pos.x, pos.y, -dist), Math::Vector3(pos.x, pos.y, -dist + 1.0f), Math::Vector3(0, 1, 0));

		root.render.SetTransform(TransformStage::View, view);

		Math::Matrix proj;
		proj.BuildProjection(45.0f * Math::Radian, (float)root.render.GetDevice()->GetHeight() / (float)root.render.GetDevice()->GetWidth(), 1.0f, 1000.0f);
		root.render.SetTransform(TransformStage::Projection, proj);

		DrawSelfWithChilds(dt);

		root.render.SetTransform(TransformStage::View, prevView);
		root.render.SetTransform(TransformStage::Projection, prevProj);
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
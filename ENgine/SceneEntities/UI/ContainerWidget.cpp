
#include "ContainerWidget.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, ContainerWidget, "ContainerWidget")

	META_DATA_DESC(ContainerWidget)
		BASE_SCENE_ENTITY_PROP(ContainerWidget)
	META_DATA_DESC_END()

		ContainerWidget::ContainerWidget() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void ContainerWidget::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = SpriteTransformFlags;
	}
}
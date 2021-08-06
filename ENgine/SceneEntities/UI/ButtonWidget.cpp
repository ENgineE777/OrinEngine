
#include "ButtonWidget.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, ButtonWidget, "ButtonWidget")

	META_DATA_DESC(ButtonWidget)
		BASE_SCENE_ENTITY_PROP(ButtonWidget)
	META_DATA_DESC_END()

		ButtonWidget::ButtonWidget() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void ButtonWidget::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = SpriteTransformFlags;
	}
}
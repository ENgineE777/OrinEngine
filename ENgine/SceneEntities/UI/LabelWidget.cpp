
#include "LabelWidget.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, LabelWidget, "LabelWidget")

	META_DATA_DESC(LabelWidget)
		BASE_SCENE_ENTITY_PROP(LabelWidget)
	META_DATA_DESC_END()

	LabelWidget::LabelWidget() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void LabelWidget::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = SpriteTransformFlags;
	}
}
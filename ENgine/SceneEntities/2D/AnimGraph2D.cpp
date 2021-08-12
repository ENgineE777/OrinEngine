
#include "AnimGraph2D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, AnimGraph2D, "AnimGraph2D")

	META_DATA_DESC(AnimGraph2D)
		BASE_SCENE_ENTITY_PROP(AnimGraph2D)
		ASSET_ANIM_GRAPH_2D_PROP(AnimGraph2D, anim, "Visual", "Anim")
	META_DATA_DESC_END()

	AnimGraph2D::AnimGraph2D() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void AnimGraph2D::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY | RectMoveXY | RectAnchorn;

		Tasks(true)->AddTask(0, this, (Object::Delegate)&AnimGraph2D::Draw);
	}

	void AnimGraph2D::Draw(float dt)
	{
		if (IsVisible() && anim.Get())
		{
			transform.BuildMatrices();

			anim.Draw(&transform, COLOR_WHITE, dt);
		}
	}
}
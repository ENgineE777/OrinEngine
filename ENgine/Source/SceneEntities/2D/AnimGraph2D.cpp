
#include "AnimGraph2D.h"
#include "Root/Root.h"

namespace Oak
{
	ENTITYREG(SceneEntity, AnimGraph2D, "2D/Sprites","AnimGraph2D")

	META_DATA_DESC(AnimGraph2D)
		BASE_SCENE_ENTITY_PROP(AnimGraph2D)
		ASSET_ANIM_GRAPH_2D_PROP(AnimGraph2D, anim, "Visual", "Anim")
		COLOR_PROP(AnimGraph2D, color, COLOR_WHITE, "Visual", "Color")
	META_DATA_DESC_END()

	AnimGraph2D::AnimGraph2D() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void AnimGraph2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY | RectMoveXY | RectAnchorn;

		Tasks(true)->AddTask(0, this, (Object::Delegate)&AnimGraph2D::Draw);
	}

	void AnimGraph2D::Draw(float dt)
	{
		if (IsVisible() && anim.Get())
		{
			anim.Draw(&transform, color, dt);
		}
	}
}
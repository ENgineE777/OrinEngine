
#include "Node2D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, Node2D, "Node2D")

	META_DATA_DESC(Node2D)
		BASE_SCENE_ENTITY_PROP(Node2D)
	META_DATA_DESC_END()

	Node2D::Node2D() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void Node2D::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY;

		Tasks(false)->AddTask(0, this, (Object::Delegate)&Node2D::Update);
	}

	void Node2D::Update(float dt)
	{
		transform.BuildMatrices();
	}
}
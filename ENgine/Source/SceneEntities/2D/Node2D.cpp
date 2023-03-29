
#include "Node2D.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, Node2D, "2D", "Node2D")

	META_DATA_DESC(Node2D)
		BASE_SCENE_ENTITY_PROP(Node2D)
	META_DATA_DESC_END()

	void Node2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.size = 64.0f;
		transform.transformFlag = prefabInstance ? (MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY | TransformFlag::RectMoveXY) : SpriteTransformFlags;
	}
}
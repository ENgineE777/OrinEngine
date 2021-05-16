
#include "TestEntity2D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, TestEntity2D, "TestEntity2D")

	META_DATA_DESC(TestEntity2D)
		BASE_SCENE_ENTITY_PROP(TestEntity2D)
		FLOAT_PROP(TestEntity2D, transform.pos.x, 0.0f, "Prop", "x", "X coordinate of a camera position")
		FLOAT_PROP(TestEntity2D, transform.pos.y, 0.0f, "Prop", "y", "Y coordinate of a camera position")
		FLOAT_PROP(TestEntity2D, transform.size.x, 100.0f, "Prop", "width", "Width of a sprite")
		FLOAT_PROP(TestEntity2D, transform.size.y, 100.0f, "Prop", "height", "Height of a sprite")
		COLOR_PROP(TestEntity2D, color, COLOR_WHITE, "Prop", "color")
		ASSET_TEXTURE_PROP(TestEntity2D, texture, "Prop", "Texture")
	META_DATA_DESC_END()

	void TestEntity2D::Init()
	{
		data.frames.push_back(Sprite::Frame());
		RenderTasks(false)->AddTask(0, this, (Object::Delegate) & TestEntity2D::Draw);
	}

	void TestEntity2D::ApplyProperties()
	{
		data.texture = texture ? texture->GetTexture() : root.render.GetWhiteTexture();
	}

	Transform* TestEntity2D::GetTransform()
	{
		return &transform;
	}

	void TestEntity2D::Draw(float dt)
	{
		transform.BuildMatrices();
		Sprite::Draw(&transform, color, &data, &frameState, false, false);
	}
}

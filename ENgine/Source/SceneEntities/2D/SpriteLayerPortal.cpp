
#include "SpriteLayerPortal.h"
#include "DefferedLight.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, SpriteLayerPortal, "2D/Sprites", "SpriteLayerPortal")

	META_DATA_DESC(SpriteLayerPortal)
		BASE_SCENE_ENTITY_PROP(SpriteLayerPortal)		
	META_DATA_DESC_END()

	SpriteLayerPortal::SpriteLayerPortal() : SceneEntity()
	{
	}

	void SpriteLayerPortal::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = (TransformFlag)(TransformFlag::SpriteTransformFlags);
		transform.size = 100.0f;
	}

	void SpriteLayerPortal::ApplyProperties()
	{
#ifdef ORIN_EDITOR
		Tasks(true)->DelAllTasks(this);
#endif

		Tasks(true)->AddTask(550, this, (Object::Delegate)&SpriteLayerPortal::Draw);
		Tasks(true)->AddTask(7, this, (Object::Delegate)&SpriteLayerPortal::EditorDraw);
	}

	void SpriteLayerPortal::Draw(float dt)
	{
		if (IsVisible())
		{
			AssetTextureRef texture;
			
			texture.prg = Sprite::quadPrgNoDepthWrite;
			texture.Draw(&transform, COLOR_WHITE, dt);
		}
	}

	void SpriteLayerPortal::EditorDraw(float dt)
	{
		if (IsVisible() && !GetScene()->IsPlaying())
		{
			AssetTextureRef texture;

			texture.prg = Sprite::quadPrgNoDepthWrite;
			texture.Draw(&transform, COLOR_MAGNETA_A(0.25f), dt);
		}
	}
}
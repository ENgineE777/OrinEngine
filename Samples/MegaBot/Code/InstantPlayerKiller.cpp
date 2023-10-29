
#include "InstantPlayerKiller.h"
#include "Character.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, InstantPlayerKiller, "MegaBot", "InstantPlayerKiller")

	META_DATA_DESC(InstantPlayerKiller)
		BASE_SCENE_ENTITY_PROP(InstantPlayerKiller)
		INT_PROP(InstantPlayerKiller, physGroup, 1, "Physics", "Physical group", "Physical group")
	META_DATA_DESC_END()

	void InstantPlayerKiller::Init()
	{
		PhysTriger2D::Init();

		visibleDuringPlay = false;
		color = COLOR_YELLOW;

		if (prefabInstance)
		{
			transform.transformFlag = MoveXYZ | RectMoveXY;
		}
	}

	void InstantPlayerKiller::OnContactStart(int index, SceneEntity* entity, int contactIndex)
	{
		Character::player->SetActiveScreen(true);
	}
}
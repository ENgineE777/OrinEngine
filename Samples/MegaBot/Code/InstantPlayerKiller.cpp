
#include "InstantPlayerKiller.h"
#include "Character.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, InstantPlayerKiller, "MegaBot", "InstantPlayerKiller")

	META_DATA_DESC(InstantPlayerKiller)
		BASE_SCENE_ENTITY_PROP(InstantPlayerKiller)
		COLOR_PROP(InstantPlayerKiller, color, COLOR_YELLOW, "Geometry", "color")
		INT_PROP(InstantPlayerKiller, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(InstantPlayerKiller, visibleDuringPlay, true, "Physics", "visibleDuringPlay", "Show collision during play")
	META_DATA_DESC_END()

	void InstantPlayerKiller::Init()
	{
		PhysTriger2D::Init();

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
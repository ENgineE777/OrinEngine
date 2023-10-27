
#include "SpawnPoint.h"
#include "Root/Root.h"
#include "Character.h"

namespace Orin
{
	ENTITYREG(SceneEntity, SpawnPoint, "MegaBot", "SpawnPoint")

	META_DATA_DESC(SpawnPoint)
		BASE_SCENE_ENTITY_PROP(SpawnPoint)
		COLOR_PROP(SpawnPoint, color, COLOR_YELLOW, "Geometry", "color")
		INT_PROP(SpawnPoint, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(SpawnPoint, visibleDuringPlay, true, "Physics", "visibleDuringPlay", "Show collision during play")
		STRING_PROP(SpawnPoint, nextLevel, "", "Data", "next level")
	META_DATA_DESC_END()

	void SpawnPoint::OnContactStart(int index, SceneEntity* entity, int contactIndex)
	{
		Character::player->SetSpawnPoint(transform.position);
	}
}
#include "ControlsAliases.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, ControlsAliases, "ControlsAliases")

	META_DATA_DESC(ControlsAliases)
		BASE_SCENE_ENTITY_PROP(ControlsAliases)
		FILENAME_PROP(ControlsAliases, aliases_name, "", "Property", "Aliases")
	META_DATA_DESC_END()

	void ControlsAliases::Init()
	{
	}

	void ControlsAliases::ApplyProperties()
	{
		root.controls.LoadAliases(aliases_name.c_str());
	}
}
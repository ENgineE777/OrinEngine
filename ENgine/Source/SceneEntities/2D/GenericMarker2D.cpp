
#include "GenericMarker2D.h"

#ifdef ORIN_EDITOR
#include "Editor/Editor.h"
#endif

namespace Orin
{
	ENTITYREG(SceneEntity, GenericMarker2D, "2D", "GenericMarker2D")

	META_DATA_DESC(GenericMarker2D)
		BASE_SCENE_ENTITY_PROP(GenericMarker2D)
		STRING_PROP(GenericMarker2D, sceneGroup, "", "Prop", "scene_group")
		BOOL_PROP(GenericMarker2D, fullShade, true, "Prop", "full_shade", "use full shade")
		BOOL_PROP(GenericMarker2D, isPath, false, "Prop", "is path", "is path")
		ARRAY_PROP_INST_CALLGIZMO(GenericMarker2D, instances, Instance, "Prop", "inst", GenericMarker, selInst, SetGizmo)
	META_DATA_DESC_END()

	GenericMarker2D::GenericMarker2D()
	{
		is2D = true;
	}
}
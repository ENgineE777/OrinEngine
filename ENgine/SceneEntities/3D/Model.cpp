
#include "Model.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, ModelEntity, "Mesh3D")

	META_DATA_DESC(ModelEntity)
		BASE_SCENE_ENTITY_PROP(ModelEntity)
		FILENAME_PROP(ModelEntity, meshPath, "", "Visual", "Mesh")
	META_DATA_DESC_END()

	ModelEntity::ModelEntity() : SceneEntity()
	{
		//inst_class_name = "SpriteInst";
	}

	void ModelEntity::Init()
	{
		Tasks(false)->AddTask(0, this, (Object::Delegate)& ModelEntity::Update);
	}

	void ModelEntity::ApplyProperties()
	{
		RELEASE(mesh)
		mesh = root.meshes.LoadMesh(meshPath.c_str(), Tasks(true));
	}

	void ModelEntity::Update(float dt)
	{
		if (mesh)
		{
			transform.BuildMatrices();
			mesh->transform = transform.global;
		}
	}
}
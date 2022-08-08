#include "Root/Root.h"


#ifdef OAK_EDITOR
#include "Editor/Editor.h"
#endif

namespace Oak
{
	CLASSREG(Asset, AssetScene, "AssetScene")

	META_DATA_DESC(AssetScene)
	META_DATA_DESC_END()

	Scene* AssetScene::GetScene()
	{
		if (!scene)
		{
			scene = new Scene();
			scene->Init();

			scene->Load(path.c_str());

			scene->EnableTasks(false);
		}

		return scene;
	}

	void AssetScene::Reload()
	{
	}

	void AssetScene::LoadMetaData(JsonReader& reader)
	{
		reader.Read("selected_entity", selectedEntityID);
		reader.Read("camera2DMode", camera2DMode);
		reader.Read("camera3DAngles", camera3DAngles);
		reader.Read("camera3DPos", camera3DPos);
		reader.Read("camera2DPos", camera2DPos);
		reader.Read("camera2DZoom", camera2DZoom);
	}

#ifdef OAK_EDITOR
	void AssetScene::GrabEditorData()
	{
		camera2DMode = editor.freeCamera.mode2D;

		camera3DAngles = editor.freeCamera.angles;
		camera3DPos = editor.freeCamera.pos;

		camera2DPos = editor.freeCamera.pos2D;
		camera2DZoom = editor.freeCamera.zoom2D;

		selectedEntityID = editor.selectedEntity ? editor.selectedEntity->GetUID() : -1;
	}

	void AssetScene::SaveMetaData(JsonWriter& writer)
	{
		if (GetScene()->taskPool->IsActive())
		{
			GrabEditorData();
		}

		writer.Write("selected_entity", selectedEntityID);
		writer.Write("camera2DMode", camera2DMode);
		writer.Write("camera3DAngles", camera3DAngles);
		writer.Write("camera3DPos", camera3DPos);
		writer.Write("camera2DPos", camera2DPos);
		writer.Write("camera2DZoom", camera2DZoom);

		GetScene()->Save(GetScene()->projectScenePath);
	}

	bool AssetScene::IsEditable()
	{
		return true;
	}

	void AssetScene::EnableEditing(bool enable)
	{
		GetScene()->EnableTasks(enable);
	
		if (!enable)
		{
			GrabEditorData();

			editor.SelectEntity(nullptr);
		}
		else
		{
			editor.freeCamera.mode2D = camera2DMode;

			editor.freeCamera.angles = camera3DAngles;
			editor.freeCamera.pos = camera3DPos;

			editor.freeCamera.pos2D = camera2DPos;
			editor.freeCamera.zoom2D = camera2DZoom;

			if (selectedEntityID != -1)
			{
				editor.SelectEntity(scene->FindEntity(selectedEntityID));
			}
		}
	}

	const char* AssetScene::GetSceneEntityType()
	{
		return nullptr;
	}
#endif

	void AssetScene::Release()
	{
		DELETE_PTR(scene)
	}
};
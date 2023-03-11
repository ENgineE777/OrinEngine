#pragma once

#include "Asset.h"
#include "root/Render/Render.h"

namespace Orin
{
	class CLASS_DECLSPEC AssetScene : public Asset
	{
	protected:

		Scene* scene = nullptr;
		eastl::map<uint32_t, SceneEntity*> instanceMapping;

	public:

		uint32_t selectedEntityID = -1;

		bool camera2DMode = false;

		Math::Vector2 camera3DAngles = Math::Vector2(0.0f, -0.5f);
		Math::Vector3 camera3DPos = Math::Vector3(0.0f, 6.0f, 0.0f);

		Math::Vector2 camera2DPos = 0.0;
		float camera2DZoom = 1.0f;

		Math::Vector2 gizmoAlign2D = 0.0f;
		uint16_t uid = 0;

		META_DATA_DECL_BASE(AssetPrefab)

		Scene* GetScene();

		void Reload() override;

		void LoadMetaData(JsonReader& loader) override;

#ifdef ORIN_EDITOR
		bool IsEditable() override;
		void EnableEditing(bool enable) override;

		void SaveMetaData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		void GrabEditorData();

		bool IsPrefab();
		void Copy(AssetScene* srcScene);
		void Duplicate(SceneEntity* soucre);

		void CheckSelection(SceneEntity* entity, eastl::vector<SceneEntity*>& selection, Math::Vector2 ms, Math::Vector3 start, Math::Vector3 dir);
		void CheckSelection(Math::Vector2 ms, Math::Vector3 start, Math::Vector3 dir);

		void SelectEntity(SceneEntity* entity, bool resetMultiSelect);

		void ImGuiProperties() override;
		bool ImGuiHasHierarchy() override;
		void ImGuiHierarchy() override;
		void ImGuiViewport(bool viewportFocused) override;

		void OnMouseMove(Math::Vector2 ms) override;
		void OnLeftMouseDown() override;
		void OnLeftMouseUp() override;
		void OnRightMouseDown() override;
		void OnRightMouseUp() override;
		void OnMiddleMouseDown() override;
		void OnMiddleMouseUp() override;
#endif

		void Release() override;

	protected:
		bool sceneTreePopup = false;
		bool entityDeletedViaPopup = false;
		bool allowSceneDropTraget = true;
		bool isPrefab = false;
		bool inCreatingInstance = false;
		bool blockPopupInViewport = true;
		SceneEntity* selectedEntity = nullptr;
		int underSelectionIndex;
		Math::Vector3 prevMultislectPos;
		Transform multislectTransform;
		eastl::vector<SceneEntity*> underSelection;

		eastl::vector<SceneEntity*> selectedEntities;

		void CreateEntityPopupEntry(const eastl::vector<class ClassFactorySceneEntity*>& decls, int& curIndex, int curDepth, bool onlyToCreate);
		void EntitiesTreeView(const eastl::vector<SceneEntity*>& entities);
		void SceneDropTraget(SceneEntity* entity);
		void SceneTreePopup(bool contextItem);
		void CopyChilds(SceneEntity* entity, SceneEntity* copy, Scene* sceneOwner);
		void DeleteSelectedEntity();
	};
}
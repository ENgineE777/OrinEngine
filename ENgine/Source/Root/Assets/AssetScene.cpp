#include "Root/Root.h"


#ifdef ORIN_EDITOR
#include "Editor/Editor.h"
#include "imgui_internal.h"
#endif

namespace Orin
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
		reader.Read("uid", uid);
		reader.Read("selected_entity", selectedEntityID);
		reader.Read("camera2DMode", camera2DMode);
		reader.Read("camera3DAngles", camera3DAngles);
		reader.Read("camera3DPos", camera3DPos);
		reader.Read("camera2DPos", camera2DPos);
		reader.Read("camera2DZoom", camera2DZoom);
	}

#ifdef ORIN_EDITOR
	void AssetScene::GrabEditorData()
	{
		camera2DMode = editor.freeCamera.mode2D;

		camera3DAngles = editor.freeCamera.angles;
		camera3DPos = editor.freeCamera.pos;

		camera2DPos = editor.freeCamera.pos2D;
		camera2DZoom = editor.freeCamera.zoom2D;

		selectedEntityID = selectedEntity ? selectedEntity->GetUID() : -1;
	}

	bool AssetScene::IsPrefab()
	{
		return isPrefab;
	}

	void AssetScene::SaveMetaData(JsonWriter& writer)
	{
		if (GetScene()->taskPool->IsActive())
		{
			GrabEditorData();
		}

		writer.Write("uid", uid);
		writer.Write("selected_entity", selectedEntityID);
		writer.Write("camera2DMode", camera2DMode);
		writer.Write("camera3DAngles", camera3DAngles);
		writer.Write("camera3DPos", camera3DPos);
		writer.Write("camera2DPos", camera2DPos);
		writer.Write("camera2DZoom", camera2DZoom);

		GetScene()->Save(GetPath().c_str());
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

			SelectEntity(nullptr, true);
		}
		else
		{
			editor.freeCamera.mode2D = camera2DMode;

			editor.freeCamera.angles = camera3DAngles;
			editor.freeCamera.pos = camera3DPos;

			editor.freeCamera.pos2D = camera2DPos;
			editor.freeCamera.zoom2D = camera2DZoom;

			if (auto* entity = GetScene()->FindEntity(selectedEntityID))
			{
				SelectEntity(entity, true);
			}
		}
	}

	const char* AssetScene::GetSceneEntityType()
	{
		return nullptr;
	}

	void AssetScene::ImGuiProperties()
	{
		if (selectedEntity)
		{
			containsUnsavedChanges |= selectedEntity->ImGuiProperties();
		}
	}

	bool AssetScene::ImGuiHasHierarchy()
	{
		return true;
	}

	void AssetScene::SceneDropTraget(SceneEntity* entity)
	{
		ImGuiContext* context = ImGui::GetCurrentContext();
		ImGuiWindow* window = context->CurrentWindow;

		if (allowSceneDropTraget && ImGui::BeginDragDropTarget())
		{
			allowSceneDropTraget = false;

			auto rect = window->DC.LastItemRect;
			bool asChild = false;

			if (entity && context->IO.MousePos.y > (rect.Min.y + rect.Max.y) * 0.5f)
			{
				rect.Min.x += 20;
				rect.Min.y = (rect.Min.y + rect.Max.y) * 0.5f;
				asChild = true;
			}

			SceneEntity* transformEntity = entity;

			if (!asChild && entity)
			{
				transformEntity = transformEntity->GetParent();
			}

			ImGuiPayload& payload = context->DragDropPayload;
			bool dragFinished = false;

			SceneEntity* assetEntity = nullptr;

			if (payload.IsDataType("_TREENODE"))
			{
				uint64_t temp = *((uint64_t*)payload.Data);
				SceneEntity* dragged = (SceneEntity*)temp;

				if (!dragged->ContainEntity(entity))
				{
					if (ImGui::AcceptDragDropPayload("_TREENODE", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
					{
						bool allowDrag = true;

						SceneEntity* parent = asChild ? entity : (entity ? entity->GetParent() : nullptr);

						if (isPrefab && !parent)
						{
							allowDrag = false;
						}

						if (allowDrag)
						{
							if (dragged->GetParent())
							{
								dragged->SetParent(nullptr);
							}
							else
							{
								GetScene()->DeleteEntity(dragged, false);
							}

							if (parent)
							{
								dragged->SetParent(parent, asChild ? nullptr : entity);
							}
							else
							{
								GetScene()->AddEntity(dragged);
							}

							dragged->UpdateVisibility();

							containsUnsavedChanges = true;
						}

						dragFinished = true;
					}
				}
			}
			else
			if (payload.IsDataType("_ASSET_TEX"))
			{
				auto& assetRef = *reinterpret_cast<AssetTextureRef**>(payload.Data)[0];

				if (ImGui::AcceptDragDropPayload("_ASSET_TEX", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
				{
					assetEntity = GetScene()->CreateEntity(assetRef->GetSceneEntityType(), false);

					if (assetEntity)
					{
						assetRef.SetupCreatedSceneEntity(assetEntity);
						assetEntity->ApplyProperties();
					}

					dragFinished = true;
				}
			}
			else
			if (payload.IsDataType("_ASSET_ANIM_GRAPH_2D"))
			{
				auto& assetRef = *reinterpret_cast<AssetAnimGraph2DRef**>(payload.Data)[0];

				if (ImGui::AcceptDragDropPayload("_ASSET_ANIM_GRAPH_2D", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
				{
					assetEntity = GetScene()->CreateEntity(assetRef->GetSceneEntityType(), false);

					if (assetEntity)
					{
						assetRef.SetupCreatedSceneEntity(assetEntity);
						assetEntity->ApplyProperties();
					}

					dragFinished = true;
				}
			}
			else
			if (payload.IsDataType("_ASSET_TILE_SET"))
			{
				auto& assetRef = *reinterpret_cast<AssetTileSetRef**>(payload.Data)[0];

				if (ImGui::AcceptDragDropPayload("_ASSET_TILE_SET", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
				{
					assetEntity = GetScene()->CreateEntity(assetRef->GetSceneEntityType(), false);

					if (assetEntity)
					{
						assetRef.SetupCreatedSceneEntity(assetEntity);
						assetEntity->ApplyProperties();
					}

					dragFinished = true;
				}
			}
			else
			if (payload.IsDataType("_ASSET_SPRITES_LAYER"))
			{
				auto& assetRef = *reinterpret_cast<AssetSpritesLayerRef**>(payload.Data)[0];

				if (ImGui::AcceptDragDropPayload("_ASSET_SPRITES_LAYER", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
				{
					assetEntity = GetScene()->CreateEntity(assetRef->GetSceneEntityType(), false);

					if (assetEntity)
					{
						assetRef.SetupCreatedSceneEntity(assetEntity);
						assetEntity->ApplyProperties();
					}

					dragFinished = true;
				}
			}
			else
			if (payload.IsDataType("_ASSET_PREFAB"))
			{
				Assets::AssetHolder* holder = reinterpret_cast<Assets::AssetHolder**>(payload.Data)[0];

				if (ImGui::AcceptDragDropPayload("_ASSET_PREFAB", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
				{
					AssetPrefab* prefab = holder->GetAsset<AssetPrefab>();
					assetEntity = prefab->CreateInstance(scene);

					containsUnsavedChanges = true;
			
					dragFinished = true;
				}
			}

			if (assetEntity)
			{
				bool allowAdd = true;

				SceneEntity* parent = asChild ? entity : (entity ? entity->GetParent() : nullptr);

				if (allowAdd)
				{
					containsUnsavedChanges = true;

					if (parent)
					{
						assetEntity->SetParent(parent, asChild ? nullptr : entity);
					}
					else
					{
						GetScene()->AddEntity(assetEntity);
					}

					assetEntity->UpdateVisibility();
					SelectEntity(assetEntity, true);

					auto& transform = assetEntity->GetTransform();

					if (transform.objectType == ObjectType::Object2D)
					{
						transform.position = editor.freeCamera.pos2D;
					}
					else
					{
						transform.position = editor.freeCamera.pos + Math::Vector3(cosf(editor.freeCamera.angles.x), sinf(editor.freeCamera.angles.y), sinf(editor.freeCamera.angles.x)) * 5.0f;
					}
				}
				else
				{
					RELEASE(assetEntity)
				}
			}

			if (dragFinished)
			{
				ImGui::EndDragDropTarget();
			}
			else
			if (entity)
			{
				window->DrawList->AddRect(rect.Min, rect.Max /*- ImVec2(10.0f, 10.0f)*/, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 0.0f, ~0, 2.0f);
			}
		}
	}

	void AssetScene::CopyChilds(SceneEntity* entity, SceneEntity* copy, Scene* sceneOwner)
	{
		auto& childs = entity->GetChilds();

		for (auto* child : childs)
		{
			if (!child->prefabRef && child->prefabInstance)
			{
				continue;
			}

			SceneEntity* childCopy = nullptr;

			if (child->prefabInstance)
			{
				childCopy = child->prefabRef->CreateInstance(sceneOwner);
			}
			else
			{
				childCopy = sceneOwner->CreateEntity(child->className, inCreatingInstance ? true : child->prefabInstance);
			}

			childCopy->SetParent(copy, nullptr);
			childCopy->Copy(child);
			childCopy->PostLoad();

			CopyChilds(child, childCopy, sceneOwner);
		}
	}

	void AssetScene::DeleteSelectedEntity()
	{
		if (!selectedEntity)
		{
			return;
		}

		SceneEntity* entity = selectedEntity;
		SelectEntity(nullptr, true);

		editor.DeleteActionsFromHistory(entity);

		if (entity->GetParent())
		{
			entity->SetParent(nullptr);
			entity->Release();
		}
		else
		{
			GetScene()->DeleteEntity(entity, true);
		}

		containsUnsavedChanges = true;
	}

	void AssetScene::CreateEntityPopupEntry(const eastl::vector<ClassFactorySceneEntity*>& decls, int& curIndex, int curDepth, bool onlyToCreate)
	{
		while (curIndex < decls.size())
		{
			if (decls[curIndex]->groups.size() == curDepth)
			{
				if (ImGui::MenuItem(decls[curIndex]->GetShortName()))
				{
					auto* entity = GetScene()->CreateEntity(decls[curIndex]->GetName(), false);

					if (selectedEntity)
					{
						auto* parent = onlyToCreate ? selectedEntity : selectedEntity->GetParent();

						if (parent)
						{
							entity->SetParent(parent, selectedEntity);
						}
						else
						{
							if (!isPrefab)
							{
								GetScene()->AddEntity(entity, selectedEntity);
							}
							else
							{
								RELEASE(entity)
							}
						}
					}
					else
					{
						GetScene()->AddEntity(entity);
					}

					if (entity)
					{
						auto& transform = entity->GetTransform();

						if (transform.objectType == ObjectType::Object2D)
						{
							transform.position = editor.freeCamera.pos2D;
						}
						else
						{
							transform.position = editor.freeCamera.pos + Math::Vector3(cosf(editor.freeCamera.angles.x), sinf(editor.freeCamera.angles.y), sinf(editor.freeCamera.angles.x)) * 5.0f;
						}

						entity->UpdateVisibility();
						SelectEntity(entity, true);

						containsUnsavedChanges = true;
					}
				}
			}
			else
			if (ImGui::BeginMenu(decls[curIndex]->groups[curDepth].c_str()))
			{
				CreateEntityPopupEntry(decls, curIndex, curDepth + 1, onlyToCreate);

				ImGui::EndMenu();
			}

			if (curDepth > 0 && curIndex + 1 < decls.size())
			{
				if (decls[curIndex + 1]->groups.size() < curDepth)
				{
					return;
				}

				if (!StringUtils::IsEqual(decls[curIndex]->groups[curDepth - 1].c_str(), decls[curIndex + 1]->groups[curDepth - 1].c_str()))
				{
					return;
				}
			}

			curIndex++;

			if (curIndex == decls.size())
			{
				break;
			}
		}
	}

	void AssetScene::Duplicate(SceneEntity* soucre)
	{
		SceneEntity* copy = nullptr;

		if (soucre->prefabInstance)
		{
			copy = soucre->prefabRef->CreateInstance(soucre->GetScene());
		}
		else
		{
			copy = GetScene()->CreateEntity(soucre->className, soucre->prefabInstance);
		}

		auto* parent = selectedEntity ? selectedEntity->GetParent() : nullptr;

		if (parent)
		{
			copy->SetParent(parent, selectedEntity);
		}
		else
		{
			GetScene()->AddEntity(copy, selectedEntity);
		}

		copy->Copy(soucre);
		copy->PostLoad();

		CopyChilds(soucre, copy, scene);

		copy->UpdateVisibility();

		SelectEntity(copy, true);

		char temp[1024];
		StringUtils::Copy(temp, 1024, selectedEntity->GetName());
		StringUtils::InreaseNumberInName(temp, 1024);

		copy->SetName(temp);

		containsUnsavedChanges = true;
	}

	void AssetScene::SceneTreePopup(bool contextItem)
	{
		if (isPrefab && (selectedEntity == nullptr))
		{
			return;
		}

		bool onlyToCreate = isPrefab && !selectedEntity->GetParent();

		entityDeletedViaPopup = false;

		if (sceneTreePopup)
		{
			return;
		}

		if ((contextItem && ImGui::BeginPopupContextItem("CreateEntity")) ||
			(!contextItem && ImGui::BeginPopupContextWindow("CreateEntity")))
		{
			sceneTreePopup = true;

			if (ImGui::BeginMenu("Create Entity"))
			{
				auto& decls = ClassFactorySceneEntity::GropedDecls();
				int index = 0;
				int curDepth = 0;

				CreateEntityPopupEntry(decls, index, curDepth, onlyToCreate);
				
				ImGui::EndMenu();
			}

			if (selectedEntity && !onlyToCreate && ImGui::MenuItem("Copy"))
			{
				editor.bufferedSceneEntity.SetEntity(selectedEntity);
			}

			if (editor.bufferedSceneEntity && !onlyToCreate && ImGui::MenuItem("Paste"))
			{
				Duplicate(editor.bufferedSceneEntity.GetSceneEntity());
			}

			if (selectedEntity && !onlyToCreate && ImGui::MenuItem("Duplicate"))
			{
				Duplicate(selectedEntity);
			}

			if (!onlyToCreate && ImGui::MenuItem("Delete"))
			{
				if (selectedEntity)
				{
					DeleteSelectedEntity();

					entityDeletedViaPopup = true;
				}
			}

			ImGui::EndPopup();
		}
	}

	void AssetScene::EntitiesTreeView(const eastl::vector<SceneEntity*>& entities)
	{
		ImGuiContext* context = ImGui::GetCurrentContext();
		ImGuiWindow* window = context->CurrentWindow;

		if (entities.size() > 0)
		{
			for (int i = 0; i < entities.size(); i++)
			{
				SceneEntity* entity = entities[i];

				if (entity->prefabInstance && entity->parent && entity->parent->prefabInstance)
				{
					continue;
				}

				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

				if (entity->GetChilds().size() > 0)
				{
					bool onlyPrefabChild = true;

					for (auto* child : entity->GetChilds())
					{
						if (!child->prefabInstance)
						{
							onlyPrefabChild = false;
							break;
						}
					}
					
					if (!onlyPrefabChild || !entity->prefabInstance)
					{
						nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
					}
				}

				if (selectedEntity == entity)
				{
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				}

				bool open = ImGui::TreeNodeEx(entity, nodeFlags, entity->GetName()[0] == 0 ? "[Name not set]" : entity->GetName());

				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right))
					{
						SelectEntity(entity, true);
					}

					ImGui::BeginTooltip();
					ImGui::Text(entity->className);
					ImGui::EndTooltip();
				}

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					ImGui::SetDragDropPayload("_TREENODE", &entity, sizeof(SceneEntity*));
					ImGui::EndDragDropSource();
				}

				SceneDropTraget(entity);

				SceneTreePopup(true);

				if (open)
				{
					if (!entityDeletedViaPopup)
					{
						EntitiesTreeView(entity->GetChilds());
					}

					ImGui::TreePop();
				}
			}
		}
	}

	void AssetScene::ImGuiHierarchy()
	{
		sceneTreePopup = false;

		allowSceneDropTraget = true;

		ImGui::BeginChild("SceneRoot");

		EntitiesTreeView(GetScene()->GetEntities());

		ImGui::EndChild();

		SceneDropTraget(nullptr);

		SceneTreePopup(true);
	}

	void AssetScene::ImGuiViewport(bool viewportFocused)
	{
		if (viewportFocused)
		{
			if (root.GetControls()->DebugKeyPressed("KEY_DELETE"))
			{
				DeleteSelectedEntity();
			}

			if (selectedEntity && root.GetControls()->DebugHotKeyPressed("KEY_LCONTROL", "KEY_D"))
			{
				Duplicate(selectedEntity);
			}

			if (selectedEntity && root.GetControls()->DebugHotKeyPressed("KEY_LCONTROL", "KEY_C"))
			{
				editor.bufferedSceneEntity.SetEntity(selectedEntity);
			}

			if (editor.bufferedSceneEntity.GetSceneEntity() && root.GetControls()->DebugHotKeyPressed("KEY_LCONTROL", "KEY_V"))
			{
				Duplicate(editor.bufferedSceneEntity.GetSceneEntity());
			}
		}

		if (selectedEntities.size() > 1)
		{
			auto delta = Sprite::ToUnits(multislectTransform.position - prevMultislectPos);
			prevMultislectPos = multislectTransform.position;

			for (auto* entity : selectedEntities)
			{
				auto math = entity->GetTransform().GetGlobal();

				math.Pos() += delta;

				entity->GetTransform().SetGlobal(math);

				Math::Vector3 vMinEntity = FLT_MAX;
				Math::Vector3 vMaxEntity = FLT_MIN;

				entity->GetBBox(vMinEntity, vMaxEntity);

				vMinEntity = Sprite::ToPixels(vMinEntity);
				vMaxEntity = Sprite::ToPixels(vMaxEntity);

				Sprite::DebugRect({ vMinEntity.x, vMinEntity.y }, { vMaxEntity.x, vMaxEntity.y }, COLOR_YELLOW);
			}
		}

		if (!blockPopupInViewport)
		{
			SceneTreePopup(true);
		}
	}

	void AssetScene::Copy(AssetScene* srcScene)
	{
		camera2DMode = srcScene->camera2DMode;
		camera3DAngles = srcScene->camera3DAngles;
		camera3DPos = srcScene->camera3DPos;
		camera2DPos = srcScene->camera2DPos;
		camera2DZoom = srcScene->camera2DZoom;

		auto& srcEntities = srcScene->GetScene()->GetEntities();
		auto* scene = GetScene();

		for (auto* entity : srcEntities)
		{
			SceneEntity* copy = nullptr;

			if (entity->prefabInstance)
			{
				copy = entity->prefabRef->CreateInstance(scene);
			}
			else
			{
				copy = scene->CreateEntity(entity->className, entity->prefabInstance);
			}

			scene->AddEntity(copy, nullptr);

			copy->Copy(entity);
			copy->PostLoad();

			CopyChilds(entity, copy, scene);

			copy->UpdateVisibility();
		}
	}

	void AssetScene::CheckSelection(SceneEntity* entity, eastl::vector<SceneEntity*>& selection, Math::Vector2 ms, Math::Vector3 start, Math::Vector3 dir)
	{
		if (!entity->IsVisible() || !editor.project.LayerSelectable(entity->layerName.c_str()))
		{
			return;
		}

		if (entity->CheckSelection(ms, start, dir))
		{
			auto* entityRoot = entity->GetPrefabRoot();

			if (editor.project.LayerSelectable(entityRoot->layerName.c_str()))
			{
				auto iterator = eastl::find(selection.begin(), selection.end(), entityRoot);

				if (iterator == selection.end())
				{
					selection.push_back(entityRoot);
				}
			}
		}

		auto& childs = entity->GetChilds();

		for (auto* child : childs)
		{
			CheckSelection(child, selection, ms, start, dir);
		}
	}

	void AssetScene::CheckSelection(Math::Vector2 ms, Math::Vector3 start, Math::Vector3 dir)
	{
		eastl::vector<SceneEntity*> tmpSelection;

		for (auto* entity : GetScene()->GetEntities())
		{
			CheckSelection(entity, tmpSelection, ms, start, dir);
		}

		bool sameSelection = true;

		if (tmpSelection.size() != underSelection.size())
		{
			sameSelection = false;
		}
		else
		{
			for (int index = 0; index < underSelection.size(); index++)
			{
				if (underSelection[index] != tmpSelection[index])
				{
					sameSelection = false;
					break;
				}
			}
		}

		if (sameSelection)
		{
			underSelectionIndex++;

			if (underSelectionIndex >= underSelection.size())
			{
				underSelectionIndex = 0;
			}
		}
		else
		{
			underSelectionIndex = 0;
			underSelection = tmpSelection;
		}

		if (underSelection.size() > 0)
		{
			SelectEntity(underSelection[underSelectionIndex], false);
		}
	}

	void AssetScene::SelectEntity(SceneEntity* entity, bool resetMultiSelect)
	{
		auto& gizmo = editor.gizmo;

		if (selectedEntity)
		{
			selectedEntity->SetEditMode(false);
			gizmo.Disable();
		}

		selectedEntity = entity;

		if (resetMultiSelect)
		{
			selectedEntities.clear();
		}

		if (selectedEntity)
		{
			selectedEntity->SetEditMode(true);

			auto& transform = selectedEntity->GetTransform();

			if (transform.transformFlag & TransformFlag::RectFull)
			{
				gizmo.mode = TransformMode::Rectangle;
			}
			else
			if ((gizmo.mode == TransformMode::Rotate && !(transform.transformFlag & TransformFlag::RotateXYZ)) ||
				(gizmo.mode == TransformMode::Scale && !(transform.transformFlag & TransformFlag::ScaleXYZ)) ||
				(gizmo.mode == TransformMode::Rectangle && !(transform.transformFlag & TransformFlag::RectFull)))
			{
				gizmo.mode = TransformMode::Move;
			}
		}
	}

	void AssetScene::OnMouseMove(Math::Vector2 ms)
	{
		if (selectedEntity)
		{
			selectedEntity->OnMouseMove(ms);
		}
	}

	void AssetScene::OnLeftMouseDown()
	{
		float mx = root.controls.DebugKeyValue("MS_X", false, false);
		float my = root.controls.DebugKeyValue("MS_Y", false, false);

		Math::Vector2 screenPos = Math::Vector2((float)mx / (float)root.render.GetDevice()->GetWidth(), (float)my / (float)root.render.GetDevice()->GetHeight());

		Math::Vector3 v;
		v.x = (2.0f * screenPos.x - 1) / editor.freeCamera.proj._11;
		v.y = -(2.0f * screenPos.y - 1) / editor.freeCamera.proj._22;
		v.z = 1.0f;

		Math::Matrix invView = editor.freeCamera.view;
		invView.Inverse();

		Math::Vector3 dir;
		dir.x = v.x * invView._11 + v.y * invView._21 + v.z * invView._31;
		dir.y = v.x * invView._12 + v.y * invView._22 + v.z * invView._32;
		dir.z = v.x * invView._13 + v.y * invView._23 + v.z * invView._33;
		dir.Normalize();

		if (root.controls.DebugKeyPressed("KEY_LCONTROL", AliasAction::Pressed))
		{
			CheckSelection({ (float)mx, (float)my }, invView.Pos(), dir);

			if (root.controls.DebugKeyPressed("KEY_LSHIFT", AliasAction::Pressed))
			{
				if (selectedEntity)
				{
					auto iter = eastl::find_if(selectedEntities.begin(), selectedEntities.end(), [this](const SceneEntity* entity) { return entity == selectedEntity; });

					if (iter == selectedEntities.end())
					{
						selectedEntities.push_back(selectedEntity);
					}
					else
					if (selectedEntities.size() > 1)
					{
						selectedEntities.erase(iter);
					}

					if (selectedEntities.size() > 1)
					{
						SelectEntity(nullptr, false);

						Math::Vector3 vMin = FLT_MAX;
						Math::Vector3 vMax = FLT_MIN;

						for (auto* entity : selectedEntities)
						{
							Math::Vector3 vMinEntity = FLT_MAX;
							Math::Vector3 vMaxEntity = FLT_MIN;

							entity->GetBBox(vMinEntity, vMaxEntity);
							vMin.Min(vMinEntity);
							vMax.Max(vMaxEntity);
						}

						prevMultislectPos = Sprite::ToPixels((vMin + vMax) * 0.5f);

						multislectTransform.objectType = ObjectType::Object2D;
						multislectTransform.size = Sprite::ToPixels(vMax - vMin);						
						multislectTransform.position = prevMultislectPos;

						editor.gizmo.SetTransform(this, &multislectTransform);
					}
					else
					{
						SelectEntity(selectedEntities[0], false);
					}
				}
			}
			else
			{
				selectedEntities.clear();
				editor.DeleteActionsFromHistory(this);
				selectedEntities.push_back(selectedEntity);
			}
		}
		else
		{
			if (selectedEntity)
			{
				selectedEntity->OnLeftMouseDown();
			}
		}
	}

	void AssetScene::OnLeftMouseUp()
	{
		if (selectedEntity)
		{
			selectedEntity->OnLeftMouseUp();
		}
	}

	void AssetScene::OnRightMouseDown()
	{
		if (selectedEntity)
		{
			blockPopupInViewport = selectedEntity->OnRightMouseDown();
		}
		else
		{
			blockPopupInViewport = false;
		}
	}

	void AssetScene::OnRightMouseUp()
	{
		if (selectedEntity)
		{
			selectedEntity->OnRightMouseUp();
		}
	}

	void AssetScene::OnMiddleMouseDown()
	{
		if (selectedEntity)
		{
			selectedEntity->OnMiddleMouseDown();
		}
	}

	void AssetScene::OnMiddleMouseUp()
	{
		if (selectedEntity)
		{
			selectedEntity->OnMiddleMouseUp();
		}
	}
#endif

	void AssetScene::Release()
	{
		RELEASE(scene)

		Asset::Release();
	}
};
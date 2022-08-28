#include "Root/Root.h"


#ifdef OAK_EDITOR
#include "Editor/Editor.h"
#include "imgui_internal.h"
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

		selectedEntityID = selectedEntity ? selectedEntity->GetUID() : -1;
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

			SelectEntity(nullptr);
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
				SelectEntity(scene->FindEntity(selectedEntityID));
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
			selectedEntity->GetMetaData()->Prepare(selectedEntity);
			selectedEntity->GetMetaData()->ImGuiWidgets();

			if (selectedEntity->GetMetaData()->IsValueWasChanged())
			{
				selectedEntity->ApplyProperties();
				selectedEntity->UpdateVisibility();
			}
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

						if (parent && parent->GetTransform().unitsInvScale != dragged->GetTransform().unitsInvScale)
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
								scene->DeleteEntity(dragged, false);
							}

							if (parent)
							{
								dragged->SetParent(parent, asChild ? nullptr : entity);
							}
							else
							{
								scene->AddEntity(dragged);
							}

							dragged->UpdateVisibility();
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
					assetEntity = scene->CreateEntity(assetRef->GetSceneEntityType());

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
					assetEntity = scene->CreateEntity(assetRef->GetSceneEntityType());

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
					assetEntity = scene->CreateEntity(assetRef->GetSceneEntityType());

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
					assetEntity = scene->CreateEntity(assetRef->GetSceneEntityType());

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
					Scene* scene = prefab->GetScene();
			
					auto& entities = scene->GetEntities();

					if (entities.size() > 0)
					{
						auto* src = entities[0];

						assetEntity = scene->CreateEntity(src->className);

						assetEntity->Copy(src);
						assetEntity->PostLoad();

						Scene* scene = prefab->GetScene();
						assetEntity->SetName(scene->GetName());

						CopyChilds(src, assetEntity);
						assetEntity->UpdateVisibility();
					}

					dragFinished = true;
				}
			}

			if (assetEntity)
			{
				bool allowAdd = true;

				SceneEntity* parent = asChild ? entity : (entity ? entity->GetParent() : nullptr);

				if (parent && parent->GetTransform().unitsInvScale != assetEntity->GetTransform().unitsInvScale)
				{
					allowAdd = false;
				}

				if (allowAdd)
				{
					if (parent)
					{
						assetEntity->SetParent(parent, asChild ? nullptr : entity);
					}
					else
					{
						scene->AddEntity(assetEntity);
					}

					assetEntity->UpdateVisibility();
					SelectEntity(assetEntity);

					auto& transform = assetEntity->GetTransform();

					if (transform.unitsScale)
					{
						transform.position = editor.freeCamera.pos2D;
					}
					else
					{
						transform.local.Pos() = editor.freeCamera.pos + Math::Vector3(cosf(editor.freeCamera.angles.x), sinf(editor.freeCamera.angles.y), sinf(editor.freeCamera.angles.x)) * 5.0f;
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

	void AssetScene::CopyChilds(SceneEntity* entity, SceneEntity* copy)
	{
		auto& childs = entity->GetChilds();

		for (auto* child : childs)
		{
			SceneEntity* childCopy = scene->CreateEntity(child->className);

			childCopy->SetParent(copy, nullptr);

			childCopy->Copy(child);
			childCopy->PostLoad();

			CopyChilds(child, childCopy);
		}
	}

	void AssetScene::SceneTreePopup(bool contextItem)
	{
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
				auto& decls = ClassFactorySceneEntity::Decls();

				for (auto& decl : decls)
				{
					if (ImGui::MenuItem(decl->GetShortName()))
					{
						auto* entity = scene->CreateEntity(decl->GetName());

						if (selectedEntity)
						{
							auto* parent = selectedEntity->GetParent();

							if (parent)
							{
								if (entity->GetTransform().unitsInvScale == parent->GetTransform().unitsInvScale)
								{
									entity->SetParent(parent, selectedEntity);
								}
								else
								{
									RELEASE(entity)
								}
							}
							else
							{
								scene->AddEntity(entity, selectedEntity);							}
						}
						else
						{
							scene->AddEntity(entity);
						}

						if (entity)
						{
							auto& transform = entity->GetTransform();

							if (transform.unitsScale)
							{
								transform.position = editor.freeCamera.pos2D;
							}
							else
							{
								transform.local.Pos() = editor.freeCamera.pos + Math::Vector3(cosf(editor.freeCamera.angles.x), sinf(editor.freeCamera.angles.y), sinf(editor.freeCamera.angles.x)) * 5.0f;
							}

							entity->UpdateVisibility();
							SelectEntity(entity);
						}
					}
				}

				ImGui::EndMenu();
			}

			if (selectedEntity && (!isSelecteEditScenePrefab || (isSelecteEditScenePrefab && selectedEntity->GetParent() != nullptr)) && ImGui::MenuItem("Duplicate"))
			{
				SceneEntity* copy = scene->CreateEntity(selectedEntity->className);

				auto* parent = selectedEntity->GetParent();

				if (parent)
				{
					copy->SetParent(parent, selectedEntity);
				}
				else
				{
					scene->AddEntity(copy, selectedEntity);
				}

				copy->Copy(selectedEntity);
				copy->PostLoad();

				CopyChilds(selectedEntity, copy);
				copy->UpdateVisibility();

				SelectEntity(copy);

				eastl::string name = selectedEntity->GetName();
				name += "_copy";

				copy->SetName(name.c_str());
			}

			if (selectedEntity && ImGui::MenuItem("Delete"))
			{
				SceneEntity* entity = selectedEntity;
				SelectEntity(nullptr);

				if (entity->GetParent())
				{
					entity->SetParent(nullptr);
					entity->Release();
				}
				else
				{
					scene->DeleteEntity(entity, true);
				}

				entityDeletedViaPopup = true;
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

				ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

				if (entity->GetChilds().size() > 0)
				{
					nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
				}

				if (selectedEntity == entity)
				{
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				}

				bool open = ImGui::TreeNodeEx(entity, nodeFlags, entity->GetName()[0] == 0 ? "[Name not set]" : entity->GetName());

				if (ImGui::IsItemHovered() && (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
				{
					SelectEntity(entity);
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

		EntitiesTreeView(scene->GetEntities());

		ImGui::EndChild();

		SceneDropTraget(nullptr);

		SceneTreePopup(true);
	}

	void AssetScene::SelectEntity(SceneEntity* entity)
	{
		auto& gizmo = editor.gizmo;

		if (selectedEntity)
		{
			selectedEntity->SetEditMode(false);
			gizmo.Disable();
		}

		selectedEntity = entity;

		if (selectedEntity)
		{
			selectedEntity->SetEditMode(true);

			auto& transform = selectedEntity->GetTransform();

			gizmo.SetTransform(&selectedEntity->GetTransform());

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
		if (selectedEntity)
		{
			selectedEntity->OnLeftMouseDown();
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
			selectedEntity->OnRightMouseDown();
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
		DELETE_PTR(scene)
	}
};
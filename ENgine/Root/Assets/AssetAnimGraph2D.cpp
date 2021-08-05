#include "Root/Root.h"
#include "SceneEntities/2D/AnimGraph2D.h"

#ifdef OAK_EDITOR
#include "Editor/EditorDrawer.h"
#include "imgui.h"
#endif

namespace Oak
{

	META_DATA_DESC(AssetAnimGraph2D::Link)
		STRING_PROP(AssetAnimGraph2D::Link, name, "", "Link", "Name")
		BOOL_PROP(AssetAnimGraph2D::Link, defLink, "", "Link", "DefaultLink", "If link is default")
	META_DATA_DESC_END()

	META_DATA_DESC(AssetAnimGraph2D::Event)
		INT_PROP(AssetAnimGraph2D::Event, frameNumber, 0, "Event", "frame Number", "Time on timeline when event should be fired")
		STRING_PROP(AssetAnimGraph2D::Event, name, "", "Event", "Name")
		STRING_PROP(AssetAnimGraph2D::Event, param, "", "Event", "param")
	META_DATA_DESC_END()

	META_DATA_DESC(AssetAnimGraph2D::Node)
		STRING_PROP(AssetAnimGraph2D::Node, name, "", "Node", "Name")
		ASSET_TEXTURE_PROP(AssetAnimGraph2D::Node, texture, "Node", "Texture")
		BOOL_PROP(AssetAnimGraph2D::Node, looped, true, "Node", "Looped", "If animation is looped")
		BOOL_PROP(AssetAnimGraph2D::Node, reversed, false, "Node", "Reversed", "If node should be fliped horizontally")
		ARRAY_PROP(AssetAnimGraph2D::Node, events, Event, "Node", "event")
	META_DATA_DESC_END()

	CLASSREG(Asset, AssetAnimGraph2D, "AssetAnimGraph2D")

	META_DATA_DESC(AssetAnimGraph2D)
	META_DATA_DESC_END()

	void AssetAnimGraph2D::Init()
	{
#ifdef OAK_EDITOR
		RenderTasks()->AddTask(0, this, (Object::Delegate)& AssetAnimGraph2D::Draw);
#endif
	}

	void AssetAnimGraph2D::Reload()
	{
	}

	void AssetAnimGraph2D::LoadData(JsonReader& loader)
	{
		loader.Read("camPos", camPos);
		loader.Read("camZoom", camZoom);
		loader.Read("def_node", defNode);

		int count = 0;
		loader.Read("Count", count);
		nodes.resize(count);

		for (int i = 0; i < count; i++)
		{
			if (!loader.EnterBlock("Node")) break;

			Node& node = nodes[i];
			loader.Read("type", (int&)node.type);
			loader.Read("looped", node.looped);
			loader.Read("reverse", node.reversed);
			loader.Read("pos", node.pos);
			loader.Read("name", node.name);
			loader.Read("object_uid", node.object_uid);
			loader.Read("def_link", node.defLink);

			node.texture.LoadData(loader, "texture");

			int link_count = 0;
			loader.Read("Count", link_count);
			node.links.resize(link_count);

			for (int j = 0; j < link_count; j++)
			{
				if (!loader.EnterBlock("Link")) break;

				Link& link = node.links[j];

				loader.Read("index", link.index);
				loader.Read("name", link.name);

				loader.LeaveBlock();
			}

			int event_count = 0;
			loader.Read("CountEvent", event_count);
			node.events.resize(event_count);

			for (int j = 0; j < event_count; j++)
			{
				if (!loader.EnterBlock("Event")) break;

				Event& event = node.events[j];

				loader.Read("frameNumber", event.frameNumber);
				loader.Read("name", event.name);
				loader.Read("param", event.param);

				loader.LeaveBlock();
			}

			if (node.defLink != -1)
			{
				node.links[node.defLink].defLink = true;
			}

			loader.LeaveBlock();
		}
	}

	#ifdef OAK_EDITOR
	void AssetAnimGraph2D::SaveData(JsonWriter& saver)
	{
		saver.Write("camPos", camPos);
		saver.Write("camZoom", camZoom);
		saver.Write("def_node", defNode);

		int count = (int)nodes.size();
		saver.Write("Count", count);

		saver.StartArray("Node");

		for (int i = 0; i < count; i++)
		{
			saver.StartBlock(nullptr);

			Node& node = nodes[i];
			saver.Write("type", (int&)node.type);
			saver.Write("looped", node.looped);
			saver.Write("reverse", node.reversed);
			saver.Write("pos", node.pos);
			saver.Write("name", node.name.c_str());
			saver.Write("object_uid", node.object_uid);
			saver.Write("def_link", node.defLink);
			node.texture.SaveData(saver, "texture");

			int link_count = (int)node.links.size();
			saver.Write("Count", link_count);

			saver.StartArray("Link");

			for (int j = 0; j < link_count; j++)
			{
				saver.StartBlock(nullptr);

				Link& link = node.links[j];

				saver.Write("index", link.index);
				saver.Write("name", link.name.c_str());

				saver.FinishBlock();
			}

			saver.FinishArray();

			int event_count = (int)node.events.size();
			saver.Write("CountEvent", event_count);

			saver.StartArray("Event");

			for (int j = 0; j < event_count; j++)
			{
				saver.StartBlock(nullptr);

				Event& event = node.events[j];

				saver.Write("frameNumber", event.frameNumber);
				saver.Write("name", event.name.c_str());
				saver.Write("param", event.param.c_str());

				saver.FinishBlock();
			}

			saver.FinishArray();

			saver.FinishBlock();
		}

		saver.FinishArray();
	}

	const char* AssetAnimGraph2D::GetSceneEntityType()
	{
		return "AnimGraph2D";
	}

	void AssetAnimGraph2D::Draw(float dt)
	{
		editorDrawer.SetCameraMatrices(camPos, root.render.GetDevice()->GetHeight() / camZoom, root.render.GetDevice()->GetAspect());

		editorDrawer.DrawCheckerBoard(camPos, Math::Vector2((float)root.render.GetDevice()->GetWidth(), (float)root.render.GetDevice()->GetHeight()), camZoom);

		if (drag == DragMode:: AddLink)
		{
			editorDrawer.DrawLine(nodes[selNode].pos + Math::Vector2(nodeSize.x, -nodeSize.y) * 0.5f, mousePos, COLOR_WHITE);
		}

		if (selLink != -1 && selNode != -1)
		{
			Node& node = nodes[selNode];

			if (node.links[selLink].defLink)
			{
				if (node.defLink != -1 && node.defLink != selLink)
				{
					node.links[node.defLink].defLink = false;
				}

				node.defLink = selLink;
			}
			else
			{
				node.defLink = -1;
			}
		}

		int index = 0;
		for (auto& node : nodes)
		{
			int link_index = 0;
			for (auto& link : node.links)
			{
				if (index == selNode && selLink != -1)
				{
					if (node.links[selLink].defLink)
					{
						if (link_index != selLink)
						{
							link.defLink = false;
						}
						else
						{
							node.defLink = selLink;
						}
					}
					else
					if (link.defLink)
					{
						node.defLink = link_index;
					}
				}

				Math::Vector2 p1 = node.pos + Math::Vector2(nodeSize.x, -nodeSize.y) * 0.5f;
				Math::Vector2 p2 = nodes[link.index].pos + Math::Vector2(nodeSize.x, -nodeSize.y) * 0.5f;

				Math::Vector2 dir = (p2 - p1);
				float len = dir.Normalize();

				link.arrowPos = p1 + dir * len * 0.6f;
				link.angle = atan2f(dir.y, dir.x);

				if (selNode != -1 && link.index == selNode)
				{
					bool should_skip = false;

					for (auto& sub_link : nodes[selNode].links)
					{
						if (sub_link.index == index)
						{
							should_skip = true;
							break;
						}
					}

					if (should_skip)
					{
						continue;
					}
				}

				Color color = COLOR_WHITE;

				if (index == selNode)
				{
					if (selLink == -1)
					{
						color = COLOR_YELLOW;
					}
					else
					if (link_index == selLink)
					{
						color = COLOR_GREEN;
					}
				}

				editorDrawer.DrawLine(p1, p2, color);

				link_index++;
			}

			index++;
		}

		index = 0;
		for (auto& node : nodes)
		{
			int link_index = 0;
			for (auto& link : node.links)
			{
				Color color = COLOR_WHITE;

				if (index == selNode)
				{
					if (selLink == -1)
					{
						color = COLOR_CYAN;
					}
					else
					if (link_index == selLink)
					{
						color = COLOR_CYAN;
					}
				}

				editorDrawer.DrawSprite(editorDrawer.arrowTex, link.arrowPos, linkSize, Math::Vector2(-linkSize.x, linkSize.y) * 0.5f, link.angle, color);

				link_index++;
			}

			index++;
		}

		index = 0;

		for (auto& node : nodes)
		{
			Color color = COLOR_CYAN;

			if (index == defNode)
			{
				color = COLOR_MAGNETA;
			}

			if ((selNode == index && selLink == -1) || targetNode == index)
			{
				color = COLOR_GREEN;
			}

			/*if (node.type == AnimNode)
			{
				color.b = 1.0f;

				if (index == def_node)
				{
					color.g = 1.0f;
				}
			}

			if (node.type == LogicNode)
			{
				color.g = 1.0f;
			}

			if (node.type == GroupNode)
			{
				color.r = 1.0f;
				color.b = 1.0f;
			}*/

			editorDrawer.DrawSprite(editorDrawer.nodeTex, node.pos, nodeSize, 0.0f, 0.0f, color);

			editorDrawer.PrintText(node.pos + Math::Vector2(5.0f, -3.0f), camZoom, COLOR_WHITE, node.name.c_str());

			editorDrawer.PrintText(node.pos + Math::Vector2(5.0f, -25.0f), camZoom, COLOR_WHITE, node.texture.GetName());

			if (index == defNode)
			{
				editorDrawer.PrintText(node.pos + Math::Vector2(5.0f, -55.0f), camZoom, COLOR_WHITE, "Start node");
			}

			index++;
		}
	}

	int AssetAnimGraph2D::GetNodeIndex(Math::Vector2& ms)
	{
		int selected = -1;

		int index = 0;
		for (auto& node : nodes)
		{
			if (node.pos.x < ms.x && ms.x < node.pos.x + nodeSize.x &&
				node.pos.y - nodeSize.y < ms.y && ms.y < node.pos.y)
			{
				selected = index;
			}

			index++;
		}

		return selected;
	}

	void AssetAnimGraph2D::CreateNode(NodeType type)
	{
		if (defNode == -1 && type == NodeType::AnimNode)
		{
			defNode = (int)nodes.size();
		}

		Node node;
		node.type = type;
		node.pos = mousePos;
		node.name = "Node";

		nodes.push_back(node);
	}

	void AssetAnimGraph2D::Delete()
	{
		if (selNode == -1)
		{
			return;
		}

		if (selLink != -1)
		{
			nodes[selNode].links.erase(nodes[selNode].links.begin() + selLink);
			selLink = -1;
			selNode = -1;

			return;
		}

		if (nodes[selNode].type == NodeType::AnimNode)
		{
			defNode = -1;
		}

		for (auto& node : nodes)
		{
			for (int i = 0; i < node.links.size(); i++)
			{
				if (node.links[i].index == selNode)
				{
					node.links.erase(node.links.begin() + i);
					i--;
				}
				else
					if (node.links[i].index > selNode)
					{
						node.links[i].index--;
					}
			}
		}

		nodes.erase(nodes.begin() + selNode);
		selNode = -1;
	}

	void AssetAnimGraph2D::MakeNodeAsDefault()
	{
		if (selNode != -1 && nodes[selNode].type == NodeType::AnimNode)
		{
			defNode = selNode;
		}
	}

	void AssetAnimGraph2D::ImGuiProperties()
	{
		GetMetaData()->Prepare(this);
		GetMetaData()->ImGuiWidgets();

		bool changed = GetMetaData()->IsValueWasChanged();

		if (selNode != -1 && selLink != -1)
		{
			Link* link = &nodes[selNode].links[selLink];
			link->GetMetaData()->Prepare(link);
			link->GetMetaData()->ImGuiWidgets();
			changed |= link->GetMetaData()->IsValueWasChanged();
		}
		else
		if (selNode != -1)
		{
			Node* node = &nodes[selNode];
			node->GetMetaData()->Prepare(node);
			node->GetMetaData()->ImGuiWidgets();
			changed |= node->GetMetaData()->IsValueWasChanged();
		}

		if (changed)
		{
			SaveMetaData();
		}
	}

	void AssetAnimGraph2D::ImGui(bool viewportFocused)
	{
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ASSET_TEX", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

			if (payload)
			{
				int node = GetNodeIndex(mousePos);

				if (node != -1)
				{
					nodes[node].texture = *(reinterpret_cast<AssetTextureRef**>(payload->Data)[0]);

					SaveMetaData();
				}
			}
		}

		if (viewportFocused)
		{
			ImGuiIO& io = ImGui::GetIO();
			camZoom = Math::Clamp(camZoom + io.MouseWheel * 0.5f, 0.4f, 3.0f);
		}

		if (ImGui::BeginPopupContextItem("AnimGraph2FContext"))
		{
			bool changed = false;

			if (ImGui::MenuItem("Create Anim Node"))
			{
				CreateNode(NodeType::AnimNode);
				changed = true;
			}

			if (ImGui::MenuItem("Create Logic Node"))
			{
				CreateNode(NodeType::LogicNode);
				changed = true;
			}

			if (ImGui::MenuItem("Creat Group Node"))
			{
				CreateNode(NodeType::GroupNode);
				changed = true;
			}

			if (ImGui::MenuItem("Mark as Default"))
			{
				MakeNodeAsDefault();
				changed = true;
			}

			if (ImGui::MenuItem("Delete"))
			{
				Delete();
				changed = true;
			}

			if (ImGui::MenuItem("Center camera"))
			{
				camPos = 0.0f;
				camZoom = 1.0f;

				if (nodes.size() > 0)
				{
					Math::Vector2 leftCorner = FLT_MAX;
					Math::Vector2 rightCorner = -FLT_MAX;

					for (auto& node : nodes)
					{
						leftCorner.Min(node.pos);
						rightCorner.Max(node.pos + Math::Vector2(nodeSize.x, -nodeSize.y));
					}

					camPos = (rightCorner - leftCorner) * 0.5f;
				}
			}

			if (changed)
			{
				SaveMetaData();
			}

			ImGui::EndPopup();
		}
	}

	void AssetAnimGraph2D::OnMouseMove(Math::Vector2 ms)
	{
		mousePos = camPos + Math::Vector2(prevMs.x - root.render.GetDevice()->GetWidth() * 0.5f, -prevMs.y + root.render.GetDevice()->GetHeight() * 0.5f) / camZoom;

		Math::Vector2 delta(prevMs.x - ms.x, ms.y - prevMs.y);
		prevMs = ms;

		delta /= camZoom;

		if (drag == DragMode::None)
		{
			return;
		}

		if (drag == DragMode::Field)
		{
			camPos += delta;
		}

		if (drag == DragMode::MoveNode)
		{
			nodes[selNode].pos -= delta;
		}

		if (drag == DragMode::AddLink)
		{
			targetNode = GetNodeIndex(mousePos);

			if (targetNode != -1 && targetNode != selNode)
			{
				Node& node = nodes[targetNode];

				if (node.pos.x > mousePos.x || mousePos.x > node.pos.x + nodeSize.x ||
					node.pos.y - nodeSize.y > mousePos.y || mousePos.y > node.pos.y)
				{
					targetNode = -1;
				}
			}

			if ((targetNode == selNode) || (targetNode != -1 && nodes[targetNode].type == NodeType::GroupNode))
			{
				targetNode = -1;
			}
		}
	}

	void AssetAnimGraph2D::OnLeftMouseDown()
	{
		drag = DragMode::Field;

		selNode = GetNodeIndex(mousePos);
		selLink = -1;

		if (selNode == -1)
		{
			int index = 0;
			for (auto& node : nodes)
			{
				if (node.links.size() > 0)
				{
					int link_index = 0;

					for (auto& link : node.links)
					{
						if (link.arrowPos.x - linkSize.x < mousePos.x && mousePos.x < link.arrowPos.x + linkSize.x &&
							link.arrowPos.y - linkSize.y < mousePos.y && mousePos.y < link.arrowPos.y + linkSize.y)
						{
							selNode = index;
							selLink = link_index;
						}

						link_index++;
					}

				}

				index++;
			}
		}

		if (selNode != -1 && selLink == -1)
		{
			drag = DragMode::MoveNode;
		}

		if (selNode != -1 && root.controls.DebugKeyPressed("KEY_LCONTROL", AliasAction::Pressed))
		{
			drag = DragMode::AddLink;
		}
	}

	void AssetAnimGraph2D::OnLeftMouseUp()
	{
		if (targetNode != -1)
		{
			bool allow_add = true;

			for (auto& link : nodes[selNode].links)
			{
				if (link.index == targetNode)
				{
					allow_add = false;
					break;
				}
			}

			if (allow_add)
			{
				Link link;
				link.index = targetNode;

				nodes[selNode].links.push_back(link);

				selLink = (int)nodes[selNode].links.size() - 1;

				SaveMetaData();
			}

			targetNode = -1;
		}

		if (drag == DragMode::MoveNode)
		{
			SaveMetaData();
		}

		drag = DragMode::None;
	}
	#endif

	void AssetAnimGraph2DRef::SetOnFrameChangeCallback(eastl::function<void(int, eastl::string&, eastl::string&)> setOnFrameChange)
	{
		onFrameChange = setOnFrameChange;
	}

	void AssetAnimGraph2DRef::Reset()
	{
		if (!Get())
		{
			curNode = nullptr;
			return;
		}

		GotoNode(Get()->defNode);
	}

	bool AssetAnimGraph2DRef::ActivateLink(const char* linkName)
	{
		if (!curNode)
		{
			return false;
		}

		for (auto& link : curNode->links)
		{
			if (StringUtils::IsEqual(link.name.c_str(), linkName))
			{
				GotoNode(link.index);
				return true;
			}
		}

		return false;
	}

	void AssetAnimGraph2DRef::GotoNode(int index)
	{
		if (!Get())
		{
			return;
		}

		curNode = &Get()->nodes[index];

		auto fireEvents = [this](int frame)
		{
			for (auto& event : curNode->events)
			{
				if (frame == event.frameNumber)
				{
					onFrameChange(frame, event.name, event.param);
				}
			}
		};

		curTexture = curNode->texture;
		curTexture.ResetAnim(curNode->looped, curNode->reversed, fireEvents);
	}

	bool AssetAnimGraph2DRef::GotoNode(const char* nodeName)
	{
		if (!Get())
		{
			return false;
		}

		int index = 0;

		for (auto& node : Get()->nodes)
		{
			if (StringUtils::IsEqual(node.name.c_str(), nodeName))
			{
				GotoNode(index);
				return true;
			}

			index++;
		}

		return false;
	}

	void AssetAnimGraph2DRef::Draw(Transform* trans, Color clr, float dt)
	{
		if (!Get() || !curNode)
		{
			return;
		}

		if (curTexture.Get())
		{
			auto size = curTexture.GetSize();
			trans->size.x = size.x;
			trans->size.y = size.y;

			curTexture.Draw(trans, clr, dt);
		}
		else
		{
			trans->size.x = 50.0f;
			trans->size.y = 50.0f;
		}

		if (!curNode->looped && curTexture.IsAnimFinished())
		{
			if (curNode->defLink != -1)
			{
				GotoNode(curNode->links[curNode->defLink].index);
			}
			else
			if (curNode->links.size() > 0)
			{
				GotoNode(curNode->links[0].index);
			}
		}
	}

	void AssetAnimGraph2DRef::SetupCreatedSceneEntity(SceneEntity* entity)
	{
		AnimGraph2D* sprite = reinterpret_cast<AnimGraph2D*>(entity);

		if (sprite)
		{
			sprite->anim = *this;
			sprite->anim.Reset();

			char str[256];
			StringUtils::Copy(str, 256, Get()->name.c_str());
			StringUtils::RemoveExtension(str);

			entity->SetName(str);
		}
	}

	Math::Vector2 AssetAnimGraph2DRef::GetSize()
	{
		return 0.0f;
	}

	void AssetAnimGraph2DRef::LoadData(JsonReader& loader, const char* name)
	{
		if (loader.EnterBlock(name))
		{
			eastl::string path;
			if (loader.Read("path", path))
			{
				*this = Oak::root.assets.GetAssetRef<AssetAnimGraph2DRef>(path);
				Reset();
			}

			loader.LeaveBlock();
		}
	}

#ifdef OAK_EDITOR
	void AssetAnimGraph2DRef::SaveData(JsonWriter& saver, const char* name)
	{
		if (Get())
		{
			saver.StartBlock(name);
			saver.Write("Path", Get()->GetPath().c_str());
			saver.FinishBlock();
		}
	}
#endif
};
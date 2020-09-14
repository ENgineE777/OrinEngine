
#include "MetaData.h"
#include "imgui.h"

#ifdef OAK_EDITOR
extern const char* OpenFileDialog(const char* extName, const char* ext, bool open);
#endif

namespace Oak
{
	void MetaData::Prepare(void* set_owner, void* set_root)
	{
		if (!inited)
		{
			Init();
			inited = true;
		}

		owner = set_owner;
		root = set_root ? set_root : owner;

		#ifdef OAK_EDITOR
		StringUtils::Printf(guiID, 64, "%p", owner);
		#endif

		for (int i = 0; i < properties.size(); i++)
		{
			Property& prop = properties[i];
			prop.value = (uint8_t*)owner + prop.offset;

			#ifdef OAK_EDITOR
			if (prop.type == Type::Callback)
			{
				prop.value = (uint8_t*)prop.callback;
			}
			#endif

			if (prop.adapter)
			{
				prop.adapter->value = prop.value;
			}
		}
	}

	void MetaData::SetDefValues()
	{
		for (int i = 0; i < properties.size(); i++)
		{
			Property& prop = properties[i];

			if (prop.type == Type::Boolean)
			{
				memcpy(prop.value, &prop.defvalue.boolean, sizeof(bool));
			}
			else
			if (prop.type == Type::Integer)
			{
				memcpy(prop.value, &prop.defvalue.integer, sizeof(int));
			}
			else
			if (prop.type == Type::Float)
			{
				memcpy(prop.value, &prop.defvalue.flt, sizeof(float));
			}
			else
			if (prop.type == Type::String || prop.type == Type::FileName)
			{
				*((eastl::string*)prop.value) = defStrings[prop.defvalue.string];
			}
			else
			if (prop.type == Type::Color)
			{
				memcpy(prop.value, prop.defvalue.color, sizeof(float) * 4);
			}
			else
			if (prop.type == Type::Enum)
			{
				MetaDataEnum& enm = enums[prop.defvalue.enumIndex];

				int value = enm.values[enm.defIndex];
				memcpy(prop.value, &value, sizeof(int));
			}
			else
			if (prop.type == Type::EnumString)
			{
				*((eastl::string*)prop.value) = "";
			}
		}
	}

	void MetaData::Load(JSONReader& reader)
	{
		for (int i = 0; i < properties.size(); i++)
		{
			Property& prop = properties[i];

			if (prop.type == Type::Boolean)
			{
				bool val;
				if (reader.Read(prop.name.c_str(), val))
				{
					memcpy(prop.value, &val, sizeof(bool));
				}
			}
			else
			if (prop.type == Type::Integer || prop.type == Type::Enum)
			{
				int val;
				if (reader.Read(prop.name.c_str(), val))
				{
					memcpy(prop.value, &val, sizeof(int));
				}
			}
			else
			if (prop.type == Type::Float)
			{
				float val;
				if (reader.Read(prop.name.c_str(), val))
				{
					memcpy(prop.value, &val, sizeof(float));
				}
			}
			else
			if (prop.type == Type::String || prop.type == Type::EnumString || prop.type == Type::FileName)
			{
				reader.Read(prop.name.c_str(), *((eastl::string*)prop.value));
			}
			else
			if (prop.type == Type::Color)
			{
				reader.Read(prop.name.c_str(), *((Oak::Color*)prop.value));
			}
			else
			if (prop.type == Type::Array)
			{
				if (reader.EnterBlock(prop.name.c_str()))
				{
					int count = 0;
					if (reader.Read("count", count))
					{
						prop.adapter->Resize(count);
				
						for (int i = 0; i < count; i++)
						{
							reader.EnterBlock("Elem");

							prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
							prop.adapter->GetMetaData()->Load(reader);

							reader.LeaveBlock();
						}
					}

					reader.LeaveBlock();
				}
			}
		}
	}

	void MetaData::PostLoad()
	{
		for (auto& prop : properties)
		{
			if (prop.type == Type::Array)
			{
				for (int i = 0; i < prop.adapter->GetSize(); i++)
				{
					prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
					prop.adapter->GetMetaData()->PostLoad();
				}
			}
		}
	}

	void MetaData::Save(JsonWriter& writer)
	{
		for (int i = 0; i < properties.size(); i++)
		{
			Property& prop = properties[i];

			if (prop.type == Type::Boolean)
			{
				writer.Write(prop.name.c_str(), *((bool*)prop.value));
			}
			else
			if (prop.type == Type::Integer || prop.type == Type::Enum)
			{
				writer.Write(prop.name.c_str(), *((int*)prop.value));
			}
			else
			if (prop.type == Type::Float)
			{
				writer.Write(prop.name.c_str(), *((float*)prop.value));
			}
			else
			if (prop.type == Type::String || prop.type == Type::EnumString || prop.type == Type::FileName)
			{
				writer.Write(prop.name.c_str(), ((eastl::string*)prop.value)->c_str());
			}
			else
			if (prop.type == Type::Color)
			{
				writer.Write(prop.name.c_str(), *((Oak::Color*)prop.value));
			}
			else
			if (prop.type == Type::Array)
			{
				writer.StartBlock(prop.name.c_str());

				int count = prop.adapter->GetSize();
				writer.Write("count", count);
			
				writer.StartArray("Elem");

				for (int i = 0; i < count; i++)
				{
					writer.StartBlock(nullptr);

					prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
					prop.adapter->GetMetaData()->Save(writer);

					writer.FinishBlock();
				}

				writer.FinishArray();

				writer.FinishBlock();
			}
		}
	}

	void MetaData::Copy(void* source)
	{
		for (auto& prop : properties)
		{
			uint8_t* src = (uint8_t*)source + prop.offset;

			if (prop.type == Type::Boolean)
			{
				memcpy(prop.value, src, sizeof(bool));
			}
			else
			if (prop.type == Type::Integer || prop.type == Type::Enum)
			{
				memcpy(prop.value, src, sizeof(int));
			}
			else
			if (prop.type == Type::Float)
			{
				memcpy(prop.value, src, sizeof(float));
			}
			else
			if (prop.type == Type::String || prop.type == Type::EnumString || prop.type == Type::FileName)
			{
				*((eastl::string*)prop.value) = *((eastl::string*)src);
			}
			else
			if (prop.type == Type::Color)
			{
				memcpy(prop.value, src, sizeof(float) * 4);
			}
			else
			if (prop.type == Type::Array)
			{
				prop.adapter->value = src;
				int count = prop.adapter->GetSize();
			
				prop.adapter->value = prop.value;
				prop.adapter->Resize(count);

				for (int i = 0; i < count; i++)
				{
					prop.adapter->value = src;
					uint8_t* src_item = prop.adapter->GetItem(i);

					prop.adapter->value = prop.value;

					prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
					prop.adapter->GetMetaData()->Copy(src_item);
				}
			}
		}
	}

	#ifdef OAK_EDITOR
	void MetaData::ConstructCategoriesData()
	{
		for (int i = 0; i < properties.size(); i++)
		{
			int index = -1;

			for (int j = 0; j < categoriesData.size(); j++)
			{
				if (StringUtils::IsEqual(categoriesData[j].name.c_str(), properties[i].catName.c_str()))
				{
					index = j;
					break;
				}
			}

			if (index == -1)
			{
				CategoryData data;
				data.name = properties[i].catName;
				data.indices.push_back(i);

				categoriesData.push_back(data);
			}
			else
			{
				categoriesData[index].indices.push_back(i);
			}
		}
	}

	void MetaData::ImGuiWidgets()
	{
		if (categoriesData.size() == 0)
		{
			ConstructCategoriesData();
		}

		char propGuiID[256];

		for (int j = 0; j < categoriesData.size(); j++)
		{
			StringUtils::Printf(propGuiID, 256, "%s###%sCat%i", categoriesData[j].name.c_str(), guiID, j);

			if (root == owner)
			{
				ImGui::Columns(1);
			}

			bool is_open = ImGui::CollapsingHeader(propGuiID);
			
			if (root == owner)
			{
				ImGui::Columns(2);
			}
			else
			{
				ImGui::NextColumn();

				if (ImGui::CollapsingHeader(propGuiID))
				{

				}

				ImGui::NextColumn();
			}

			if (is_open)
			{
				for (int i = 0; i < categoriesData[j].indices.size(); i++)
				{
					auto& prop = properties[categoriesData[j].indices[i]];

					if (prop.type != Type::FileName && prop.type != Type::Callback && prop.type != Type::Array)
					{
						StringUtils::Printf(propGuiID, 256, "###%s%i", guiID, i);
					}

					if (prop.type == Type::Array)
					{
						StringUtils::Printf(propGuiID, 256, "%s###%s%i", prop.propName.c_str(), guiID, i);
						bool items_open = ImGui::TreeNode(propGuiID);

						StringUtils::Printf(propGuiID, 256, "Add###%sAdd%i", guiID, i);

						ImGui::NextColumn();

						if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
						{
							prop.adapter->PushBack();
							prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem((int)prop.adapter->GetSize() - 1));
							prop.adapter->GetMetaData()->SetDefValues();
						}

						ImGui::NextColumn();

						if (items_open)
						{
							int count = prop.adapter->GetSize();
							int index2delete = -1;

							for (int i = 0; i < count; i++)
							{
								bool itemOpen = ImGui::TreeNode(StringUtils::PrintTemp("Item %i###%s%i", i, guiID, i));

								ImGui::NextColumn();

								StringUtils::Printf(propGuiID, 256, "Del###%sDel%i", guiID, i);

								if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
								{
									index2delete = i;
								}

								ImGui::NextColumn();

								if (itemOpen)
								{
									prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
									prop.adapter->GetMetaData()->ImGuiWidgets();

									ImGui::TreePop();
								}
							}

							if (index2delete != -1)
							{
								prop.adapter->Delete(index2delete);
							}

							ImGui::TreePop();
						}
					}
					else
					{
						ImGui::Text(prop.propName.c_str());
						ImGui::NextColumn();

						ImGui::SetNextItemWidth(-1);

						if (prop.type == Type::Boolean)
						{
							ImGui::Checkbox(propGuiID, (bool*)prop.value);
						}
						else
						if (prop.type == Type::Integer)
						{
							ImGui::InputInt(propGuiID, (int*)prop.value);
						}
						else
						if (prop.type == Type::Float)
						{
							ImGui::InputFloat(propGuiID, (float*)prop.value);
						}
						else
						if (prop.type == Type::String)
						{
							eastl::string* str = (eastl::string*)prop.value;

							struct Funcs
							{
								static int ResizeCallback(ImGuiInputTextCallbackData* data)
								{
									if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
									{
										eastl::string* str = (eastl::string*)data->UserData;
										str->resize(data->BufSize + 1);
										data->Buf = str->begin();
									}
									return 0;
								}
							};

							ImGui::InputText(propGuiID, str->begin(), (size_t)str->size() + 1, ImGuiInputTextFlags_CallbackResize, Funcs::ResizeCallback, (void*)str);
						}
						else
						if (prop.type == Type::FileName)
						{
							eastl::string* str = (eastl::string*)prop.value;

							StringUtils::Printf(propGuiID, 256, "%s###%s%i", str->c_str()[0] ? str->c_str() : "File not set", guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x - 30.0f, 0.0f)))
							{
								const char* fileName = OpenFileDialog("Any file", nullptr, true);

								if (fileName)
								{
									str->assign(fileName);
								}
							}
							if (str->c_str()[0] && (ImGui::IsItemActive() || ImGui::IsItemHovered()))
							{
								ImGui::SetTooltip(str->c_str());
							}

							ImGui::SameLine();

							StringUtils::Printf(propGuiID, 256, "Del###%sDel%i", guiID);

							if (ImGui::Button(propGuiID, ImVec2(30.0f, 0.0f)))
							{
								str->assign("");
							}
						}
						else
						if (prop.type == Type::Color)
						{
							ImGui::ColorEdit4(propGuiID, (float*)prop.value);
						}
						else
						if (prop.type == Type::Enum)
						{
							int value = *((int*)prop.value);
							int index = 0;

							MetaDataEnum& enumData = enums[prop.defvalue.enumIndex];

							for (int i = 0; i < enumData.values.size(); i++)
							{
								if (enumData.values[i] == value)
								{
									index = i;
									break;
								}
							}

							if (enumData.enumList.empty())
							{
								int count = 0;

								for (int i = 0; i < enumData.names.size(); i++)
								{
									count += (int)enumData.names[i].size() + 1;
								}

								enumData.enumList.resize(count + 1);

								int index = 0;

								for (int i = 0; i < enumData.names.size(); i++)
								{
									int sz = (int)enumData.names[i].size() + 1;
									memcpy(&enumData.enumList[index], enumData.names[i].c_str(), sz);
									index += sz;
								}
							}

							ImGui::Combo(propGuiID, &index, enumData.enumList.c_str());
							*((int*)prop.value) = enumData.values[index];
						}
						else
						if (prop.type == Type::EnumString)
						{
							//FIX ME!!!
						}
						else
						if (prop.type == Type::Callback)
						{
							StringUtils::Printf(propGuiID, 256, "Action###%s%i", guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
							{
								prop.callback(owner);
							}
						}

						ImGui::NextColumn();
					}
				}
			}
		}
	}

	bool MetaData::IsValueWasChanged()
	{
		bool res = false;

		/*for (auto& prop : properties)
		{
			for (auto& widget : prop.widgets)
			{
				if (!widget.second->panel->IsVisible())
				{
					continue;
				}

				res |= widget.second->changed;
				widget.second->changed = false;

				if (prop.type == Type::Array)
				{
					res |= prop.adapter->GetMetaData()->IsValueWasChanged();
					continue;
				}
			}
		}*/

		return res;
	}
	#endif
}
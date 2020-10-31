
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
				if (reader.Read(prop.propName.c_str(), val))
				{
					memcpy(prop.value, &val, sizeof(bool));
				}
			}
			else
			if (prop.type == Type::Integer || prop.type == Type::Enum)
			{
				int val;
				if (reader.Read(prop.propName.c_str(), val))
				{
					memcpy(prop.value, &val, sizeof(int));
				}
			}
			else
			if (prop.type == Type::Float)
			{
				float val;
				if (reader.Read(prop.propName.c_str(), val))
				{
					memcpy(prop.value, &val, sizeof(float));
				}
			}
			else
			if (prop.type == Type::String || prop.type == Type::EnumString || prop.type == Type::FileName)
			{
				reader.Read(prop.propName.c_str(), *((eastl::string*)prop.value));
			}
			else
			if (prop.type == Type::Color)
			{
				reader.Read(prop.propName.c_str(), *((Oak::Color*)prop.value));
			}
			else
			if (prop.type == Type::Array)
			{
				if (reader.EnterBlock(prop.propName.c_str()))
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
				writer.Write(prop.propName.c_str(), *((bool*)prop.value));
			}
			else
			if (prop.type == Type::Integer || prop.type == Type::Enum)
			{
				writer.Write(prop.propName.c_str(), *((int*)prop.value));
			}
			else
			if (prop.type == Type::Float)
			{
				writer.Write(prop.propName.c_str(), *((float*)prop.value));
			}
			else
			if (prop.type == Type::String || prop.type == Type::EnumString || prop.type == Type::FileName)
			{
				writer.Write(prop.propName.c_str(), ((eastl::string*)prop.value)->c_str());
			}
			else
			if (prop.type == Type::Color)
			{
				writer.Write(prop.propName.c_str(), *((Oak::Color*)prop.value));
			}
			else
			if (prop.type == Type::Array)
			{
				writer.StartBlock(prop.propName.c_str());

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
	void MetaData::ImGuiWidgets()
	{
		for (auto& prop : properties)
		{
			if (prop.guiID.empty())
			{
				prop.guiID = "###" + prop.propName;
			}

			ImGui::Text(prop.propName.c_str());
			ImGui::NextColumn();

			if (prop.type == Type::Boolean)
			{
				ImGui::Checkbox(prop.guiID.c_str(), (bool*)prop.value);
			}
			else
			if (prop.type == Type::Integer)
			{
				ImGui::InputInt(prop.guiID.c_str(), (int*)prop.value);
			}
			else
			if (prop.type == Type::Float)
			{
				ImGui::InputFloat(prop.guiID.c_str(), (float*)prop.value);
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

				ImGui::InputText(prop.guiID.c_str(), str->begin(), (size_t)str->size() + 1, ImGuiInputTextFlags_CallbackResize, Funcs::ResizeCallback, (void*)str);
			}
			else
			if (prop.type == Type::FileName)
			{
				eastl::string* str = (eastl::string*)prop.value;

				prop.guiID = str->c_str() + eastl::string("###") + prop.propName;

				if (ImGui::Button(prop.guiID.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 30.0f, 0.0f)))
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

				prop.guiID = "Del" + eastl::string("###") + prop.propName + "Del";

				if (ImGui::Button(prop.guiID.c_str(), ImVec2(30.0f, 0.0f)))
				{
					str->assign("");
				}
			}
			else
			if (prop.type == Type::Color)
			{
				ImGui::ColorEdit4(prop.guiID.c_str(), (float*)prop.value);
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
						count += enumData.names[i].size() + 1;
					}

					enumData.enumList.resize(count + 1);

					int index = 0;

					for (int i = 0; i < enumData.names.size(); i++)
					{
						int sz = enumData.names[i].size() + 1;
						memcpy(&enumData.enumList[index], enumData.names[i].c_str(), sz);
						index += sz;
					}
				}

				ImGui::Combo(prop.guiID.c_str(), &index, enumData.enumList.c_str());
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
				prop.guiID = eastl::string("Action###") + prop.propName;

				if (ImGui::Button(prop.guiID.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
				{
					prop.callback(owner);
				}
			}
			else
			if (prop.type == Type::Array)
			{
				//FIX ME!!!
			}

			ImGui::NextColumn();
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
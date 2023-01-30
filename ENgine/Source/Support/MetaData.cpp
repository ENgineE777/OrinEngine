
#include "MetaData.h"
#include "imgui.h"
#include "Root/Root.h"
#include "Transform.h"
#include "Root/Scenes/SceneEntity.h"
#include "ImGuiHelper.h"

#ifdef ORIN_EDITOR
#include "Editor/Editor.h"
#include "Editor/TileSetWindow.h"
#endif

namespace Orin
{
#ifdef ORIN_EDITOR
	MetaData::MetaDataPropertyAction::MetaDataPropertyAction() : IEditorAction(nullptr)
	{

	}

	MetaData::MetaDataPropertyAction::MetaDataPropertyAction(MetaDataPropertyAction& action) : IEditorAction(action.owner)
	{
		propertyOwner = action.propertyOwner;

		prop = action.prop;

		memcpy(storage, action.storage, sizeof(storage));
		memcpy(savedStorage, action.savedStorage, sizeof(savedStorage));
	}

	void MetaData::MetaDataPropertyAction::SetOwners(void* setOwner, void* setPropertyOwner, Property& setProp)
	{
		owner = setOwner;
		propertyOwner = setPropertyOwner;
		prop = setProp;

		InitStorage((uint8_t*)&storage);
		InitStorage((uint8_t*)&savedStorage);
	}

	void MetaData::MetaDataPropertyAction::InitStorage(uint8_t* dest)
	{
		if (prop.type == Type::Boolean)
		{
			bool* boolValue = (bool*)dest;
			*dest = false;
		}
		else
		if (prop.type == Type::Integer || prop.type == Type::Enum)
		{
			int* intValue = (int*)dest;
			*intValue = 0;
		}
		else
		if (prop.type == Type::Float)
		{
			float* floatValue = (float*)dest;
			*floatValue = 0.0f;
		}
		else
		if (prop.type == Type::String || prop.type == Type::EnumString || prop.type == Type::FileName)
		{
			eastl::string* stringValue = (eastl::string*)dest;
			memset(stringValue, 0, sizeof(eastl::string));
			stringValue->eastl::string::basic_string();
		}
		else
		if (prop.type == Type::Color)
		{
			Color* colorValue = (Color*)dest;
			*colorValue = COLOR_WHITE;
		}
		else
		if (prop.type == Type::Vector2)
		{
			Math::Vector2* vectorValue = (Math::Vector2*)dest;
			*vectorValue = Math::Vector2();
		}
		else
		if (prop.type == Type::Vector3)
		{
			Math::Vector3* vectorValue = (Math::Vector3*)dest;
			*vectorValue = Math::Vector3();
		}
		else
		if (prop.type == Type::Transform)
		{
			Transform* transformValue = (Transform*)dest;
			memset(transformValue, 0, sizeof(Transform));
			transformValue->Transform::Transform();
		}
		/*else
		if (prop.type == Type::AssetTexture)
		{
			AssetTextureRef* refValue = (AssetTextureRef*)dest;
			memset(refValue, 0, sizeof(AssetTextureRef));
			refValue->AssetTextureRef::AssetTextureRef();
		}
		else
		if (prop.type == Type::AssetAnimGraph2D)
		{
			AssetAnimGraph2DRef* refValue = (AssetAnimGraph2DRef*)dest;
			memset(refValue, 0, sizeof(AssetAnimGraph2DRef));
			refValue->AssetAnimGraph2DRef::AssetAnimGraph2DRef();
		}
		else
		if (prop.type == Type::AssetTileSet)
		{
			AssetTileSetRef* refValue = (AssetTileSetRef*)dest;
			memset(refValue, 0, sizeof(AssetTileSetRef));
			refValue->AssetTileSetRef::AssetTileSetRef();
		}
		else
		if (prop.type == Type::AssetSpritesLayer)
		{
			AssetSpritesLayerRef* refValue = (AssetSpritesLayerRef*)dest;
			memset(refValue, 0, sizeof(AssetSpritesLayerRef));
			refValue->AssetSpritesLayerRef::AssetSpritesLayerRef();
		}
		else
		if (prop.type == Type::SceneEntity)
		{
			SceneEntityRefBase* refValue = (SceneEntityRefBase*)dest;
			*refValue = SceneEntityRefBase();
		}*/
	}

	void MetaData::MetaDataPropertyAction::CopyPropertyData(uint8_t* src, uint8_t* dest)
	{
		if (prop.type == Type::Boolean)
		{
			memcpy(dest, src, sizeof(bool));
		}
		else
		if (prop.type == Type::Integer || prop.type == Type::Enum)
		{
			memcpy(dest, src, sizeof(int));
		}
		else
		if (prop.type == Type::Float)
		{
			memcpy(dest, src, sizeof(float));
		}
		else
		if (prop.type == Type::String || prop.type == Type::EnumString || prop.type == Type::FileName)
		{
			*((eastl::string*)dest) = *((eastl::string*)src);
		}
		else
		if (prop.type == Type::Color)
		{
			memcpy(dest, src, sizeof(float) * 4);
		}
		else
		if (prop.type == Type::Vector2)
		{
			memcpy(dest, src, sizeof(float) * 2);
		}
		else
		if (prop.type == Type::Vector3)
		{
			memcpy(dest, src, sizeof(float) * 3);
		}
		else
		if (prop.type == Type::Transform)
		{
			Transform* transformSrc = (Transform*)src;
			Transform* transformDest = (Transform*)dest;

			transformDest->position = transformSrc->position;
			transformDest->rotation = transformSrc->rotation;
			transformDest->scale = transformSrc->scale;
			transformDest->size = transformSrc->size;
			transformDest->offset = transformSrc->offset;
		}
		/*else
		if (prop.type == Type::AssetTexture)
		{
			AssetTextureRef* refSrc = reinterpret_cast<AssetTextureRef*>(src);
			AssetTextureRef* ref = reinterpret_cast<AssetTextureRef*>(dest);

			*ref = *refSrc;
		}
		else
		if (prop.type == Type::AssetAnimGraph2D)
		{
			AssetAnimGraph2DRef* refSrc = reinterpret_cast<AssetAnimGraph2DRef*>(src);
			AssetAnimGraph2DRef* ref = reinterpret_cast<AssetAnimGraph2DRef*>(dest);

			*ref = *refSrc;
			ref->Reset();
		}
		else
		if (prop.type == Type::AssetTileSet)
		{
			AssetTileSetRef* refSrc = reinterpret_cast<AssetTileSetRef*>(src);
			AssetTileSetRef* ref = reinterpret_cast<AssetTileSetRef*>(dest);

			*ref = *refSrc;
		}
		else
		if (prop.type == Type::AssetSpritesLayer)
		{
			AssetSpritesLayerRef* refSrc = reinterpret_cast<AssetSpritesLayerRef*>(src);
			AssetSpritesLayerRef* ref = reinterpret_cast<AssetSpritesLayerRef*>(dest);

			*ref = *refSrc;
		}
		else
		if (prop.type == Type::SceneEntity)
		{
			SceneEntityRefBase* ref = reinterpret_cast<SceneEntityRefBase*>(dest);
			SceneEntityRefBase* ref2 = reinterpret_cast<SceneEntityRefBase*>(src);

			ref->uid = ref2->uid;
		}*/
	}
	
	void MetaData::MetaDataPropertyAction::SaveAsSavedProperty()
	{
		CopyPropertyData((uint8_t*)propertyOwner + prop.offset, (uint8_t*)&savedStorage);
	}

	void MetaData::MetaDataPropertyAction::SaveAsProperty()
	{
		CopyPropertyData((uint8_t*)propertyOwner + prop.offset, (uint8_t*)&storage);
	}

	void MetaData::MetaDataPropertyAction::Apply()
	{
		CopyPropertyData((uint8_t*)&storage, (uint8_t*)propertyOwner + prop.offset);
	}

	void MetaData::MetaDataPropertyAction::Undo()
	{
		CopyPropertyData((uint8_t*)&savedStorage, (uint8_t*)propertyOwner + prop.offset);
	}
#endif

	void MetaData::Prepare(void* set_owner, void* set_root)
	{
		if (!inited)
		{
			Init();
			inited = true;
		}

		checkEnabledState = false;

		owner = set_owner;
		root = set_root ? set_root : owner;

		#ifdef ORIN_EDITOR
		StringUtils::Printf(guiID, 64, "%p", owner);
		#endif

		for (int i = 0; i < properties.size(); i++)
		{
			Property& prop = properties[i];
			prop.value = (uint8_t*)owner + prop.offset;

			#ifdef ORIN_EDITOR
			if (prop.type == Type::Callback)
			{
				prop.value = (uint8_t*)prop.callback;
			}
			else
			if (prop.type == Type::EnumString)
			{
				auto& entry = enums[prop.defvalue.enumIndex];
				entry.enumList.clear();
				entry.names.clear();

				prop.enum_callback(entry.names);
			}
			#endif

			if (prop.adapter)
			{
				#ifdef ORIN_EDITOR
				if (prop.adapter->selItemOffset != -1)
				{
					prop.adapter->selItem = (int32_t*)((uint8_t*)root + prop.adapter->selItemOffset);
				}
				#endif

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
			if (prop.type == Type::Vector2)
			{
				memcpy(prop.value, prop.defvalue.vector2, sizeof(float) * 2);
			}
			else
			if (prop.type == Type::Vector3)
			{
				memcpy(prop.value, prop.defvalue.vector3, sizeof(float) * 3);
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
			else
			if (prop.type == Type::AssetTexture)
			{
				AssetTextureRef* ref = reinterpret_cast<AssetTextureRef*>(prop.value);
				ref->ReleaseRef();
			}
			else
			if (prop.type == Type::AssetAnimGraph2D)
			{
				AssetAnimGraph2DRef* ref = reinterpret_cast<AssetAnimGraph2DRef*>(prop.value);
				ref->ReleaseRef();
			}
			else
			if (prop.type == Type::AssetTileSet)
			{
				AssetTileSetRef* ref = reinterpret_cast<AssetTileSetRef*>(prop.value);
				ref->ReleaseRef();
			}
			else
			if (prop.type == Type::AssetSpritesLayer)
			{
				AssetSpritesLayerRef* ref = reinterpret_cast<AssetSpritesLayerRef*>(prop.value);
				ref->ReleaseRef();
			}
			else
			if (prop.type == Type::Transform)
			{
				Transform* transform = (Transform*)prop.value;
				transform->position = 0.0f;
				transform->rotation = 0.0f;
				transform->scale = 1.0f;
				transform->offset = 0.5f;
			}
		}
	}

	void MetaData::Load(JsonReader& reader, eastl::vector<eastl::string>* allowedProprties)
	{
		for (int i = 0; i < properties.size(); i++)
		{
			Property& prop = properties[i];

			if (checkEnabledState && !prop.enabledState)
			{
				continue;
			}

			if (allowedProprties && allowedProprties->end() != find(allowedProprties->begin(), allowedProprties->end(), prop.name))
			{
				continue;
			}

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
				reader.Read(prop.name.c_str(), *((Color*)prop.value));
			}
			else
			if (prop.type == Type::Vector2)
			{
				reader.Read(prop.name.c_str(), *((Math::Vector2*)prop.value));
			}
			else
			if (prop.type == Type::Vector3)
			{
				reader.Read(prop.name.c_str(), *((Math::Vector3*)prop.value));
			}
			else
			if (prop.type == Type::AssetTexture)
			{
				AssetTextureRef* ref = reinterpret_cast<AssetTextureRef*>(prop.value);
				ref->LoadData(reader, prop.name.c_str());
			}
			else
			if (prop.type == Type::AssetAnimGraph2D)
			{
				AssetAnimGraph2DRef* ref = reinterpret_cast<AssetAnimGraph2DRef*>(prop.value);
				ref->LoadData(reader, prop.name.c_str());
			}
			else
			if (prop.type == Type::AssetTileSet)
			{
				AssetTileSetRef* ref = reinterpret_cast<AssetTileSetRef*>(prop.value);
				ref->LoadData(reader, prop.name.c_str());
			}
			else
			if (prop.type == Type::AssetSpritesLayer)
			{
				AssetSpritesLayerRef* ref = reinterpret_cast<AssetSpritesLayerRef*>(prop.value);
				ref->LoadData(reader, prop.name.c_str());
			}
			else
			if (prop.type == Type::Transform)
			{
				Transform* transform = (Transform*)prop.value;
				transform->Load(reader, prop.propName.c_str());
			}
			else
			if (prop.type == Type::SceneEntity)
			{
				if (reader.EnterBlock(prop.propName.c_str()))
				{
					SceneEntityRefBase* ref = (SceneEntityRefBase*)prop.value;

					reader.Read("uid", ref->uid);
					
					reader.LeaveBlock();
				}
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

	void MetaData::PostLoad(Scene* scene)
	{
		for (auto& prop : properties)
		{
			if (prop.type == Type::Array)
			{
				for (int i = 0; i < prop.adapter->GetSize(); i++)
				{
					prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
					prop.adapter->GetMetaData()->PostLoad(scene);
				}
			}
			else
			if (prop.type == Type::SceneEntity)
			{
				SceneEntityRefBase* ref = (SceneEntityRefBase*)prop.value;
				ref->SetEntity(scene->FindEntity(ref->uid));
			}
		}
	}

	void MetaData::Save(JsonWriter& writer, eastl::vector<eastl::string>* allowedProprties)
	{
		for (int i = 0; i < properties.size(); i++)
		{
			Property& prop = properties[i];

			if (checkEnabledState && !prop.enabledState)
			{
				continue;
			}

			if (allowedProprties && allowedProprties->end() != find(allowedProprties->begin(), allowedProprties->end(), prop.name))
			{
				continue;
			}

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
				writer.Write(prop.name.c_str(), *((Color*)prop.value));
			}
			else
			if (prop.type == Type::Vector2)
			{
				writer.Write(prop.name.c_str(), *((Math::Vector2*)prop.value));
			}
			else
			if (prop.type == Type::Vector3)
			{
				writer.Write(prop.name.c_str(), *((Math::Vector3*)prop.value));
			}
			else
			if (prop.type == Type::AssetTexture)
			{
				AssetTextureRef* ref = reinterpret_cast<AssetTextureRef*>(prop.value);
				ref->SaveData(writer, prop.name.c_str());
			}
			else
			if (prop.type == Type::AssetAnimGraph2D)
			{
				AssetAnimGraph2DRef* ref = reinterpret_cast<AssetAnimGraph2DRef*>(prop.value);
				ref->SaveData(writer, prop.name.c_str());
			}
			else
			if (prop.type == Type::AssetTileSet)
			{
				AssetTileSetRef* ref = reinterpret_cast<AssetTileSetRef*>(prop.value);
				ref->SaveData(writer, prop.name.c_str());
			}
			else
			if (prop.type == Type::AssetSpritesLayer)
			{
				AssetSpritesLayerRef* ref = reinterpret_cast<AssetSpritesLayerRef*>(prop.value);
				ref->SaveData(writer, prop.name.c_str());
			}
			else
			if (prop.type == Type::Transform)
			{
				Transform* transform = (Transform*)prop.value;
				transform->Save(writer, prop.propName.c_str());
			}
			else
			if (prop.type == Type::SceneEntity)
			{
				SceneEntityRefBase* ref = (SceneEntityRefBase*)prop.value;

				writer.StartBlock(prop.propName.c_str());

				writer.Write("uid", ref->uid);

				writer.FinishBlock();
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

	void MetaData::PreapareToRelease()
	{
		for (auto& prop : properties)
		{
			if (prop.type == Type::Array)
			{
				for (int i = 0; i < prop.adapter->GetSize(); i++)
				{
					prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
					prop.adapter->GetMetaData()->PreapareToRelease();
				}
			}
			else
			if (prop.type == Type::SceneEntity)
			{
				SceneEntityRefBase* ref = (SceneEntityRefBase*)prop.value;
				ref->SetEntity(nullptr);
			}
		}
	}

	void MetaData::Copy(void* source, eastl::vector<Property>& sourceProperties)
	{
		for (auto& prop : properties)
		{
			uint8_t* src = nullptr;

			for (auto& sourceProp : sourceProperties)
			{
				if (StringUtils::IsEqual(prop.name.c_str(), sourceProp.name.c_str()))
				{
					src = (uint8_t*)source + sourceProp.offset;
					break;
				}
			}

			if (src == nullptr)
			{
				continue;
			}

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
			if (prop.type == Type::Vector2)
			{
				memcpy(prop.value, src, sizeof(float) * 2);
			}
			else
			if (prop.type == Type::Vector3)
			{
				memcpy(prop.value, src, sizeof(float) * 3);
			}
			else
			if (prop.type == Type::AssetTexture)
			{
				AssetTextureRef* refSrc = reinterpret_cast<AssetTextureRef*>(src);
				AssetTextureRef* ref = reinterpret_cast<AssetTextureRef*>(prop.value);

				*ref = *refSrc;
			}
			else
			if (prop.type == Type::AssetAnimGraph2D)
			{
				AssetAnimGraph2DRef* refSrc = reinterpret_cast<AssetAnimGraph2DRef*>(src);
				AssetAnimGraph2DRef* ref = reinterpret_cast<AssetAnimGraph2DRef*>(prop.value);

				*ref = *refSrc;
				ref->Reset();
			}
			else
			if (prop.type == Type::AssetTileSet)
			{
				AssetTileSetRef* refSrc = reinterpret_cast<AssetTileSetRef*>(src);
				AssetTileSetRef* ref = reinterpret_cast<AssetTileSetRef*>(prop.value);

				*ref = *refSrc;
			}
			else
			if (prop.type == Type::AssetSpritesLayer)
			{
				AssetSpritesLayerRef* refSrc = reinterpret_cast<AssetSpritesLayerRef*>(src);
				AssetSpritesLayerRef* ref = reinterpret_cast<AssetSpritesLayerRef*>(prop.value);

				*ref = *refSrc;
			}
			else
			if (prop.type == Type::Transform)
			{
				Transform* transformSrc = (Transform*)src;
				Transform* transformDest = (Transform*)prop.value;

				transformDest->position = transformSrc->position;
				transformDest->rotation = transformSrc->rotation;
				transformDest->scale = transformSrc->scale;
				transformDest->size = transformSrc->size;
				transformDest->offset = transformSrc->offset;
			}
			else
			if (prop.type == Type::SceneEntity)
			{
				SceneEntityRefBase* ref = reinterpret_cast<SceneEntityRefBase*>(prop.value);
				SceneEntityRefBase* ref2 = reinterpret_cast<SceneEntityRefBase*>(src);

				ref->uid = ref2->uid;
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
					uint8_t* srcItem = prop.adapter->GetItem(i);
					auto& srcProperties = prop.adapter->GetMetaData()->properties;

					prop.adapter->value = prop.value;

					prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem(i), root);
					prop.adapter->GetMetaData()->Copy(srcItem, srcProperties);
				}
			}
		}
	}

	#ifdef ORIN_EDITOR
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

	bool MetaData::ImGuiVector(float* x, float* y, float* z, float* w, const char* name, const char* propID)
	{
		bool changed = false;

		ImGui::Text(name);
		ImGui::NextColumn();

		float* values[] = { x, y, z, w };
		const char* prefix[] = { "x", "y", "z", "w" };

		float width = 0.0f;

		for (int i = 0; i < 4; i++)
		{
			if (values[i])
			{
				width += 1.0f;
			}
		}

		width = ImGui::GetContentRegionAvail().x / width;

		bool firstEntry = true;

		for (int i = 0; i < 4; i++)
		{
			if (values[i])
			{
				if (!firstEntry)
				{
					ImGui::SameLine();
				}

				ImGui::SetNextItemWidth(width);

				char propGuiID[256];
				StringUtils::Printf(propGuiID, 256, "%s%s%s", propID, name, prefix[i]);

				if (ImGui::InputFloat(propGuiID, values[i]))
				{
					changed = true;
				}

				firstEntry = false;
			}
		}

		ImGui::NextColumn();

		return changed;
	}

	void MetaData::ImGuiWidgets(eastl::vector<eastl::string>* whitelistedProprties, bool useAsBlacklisted)
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

			bool is_open = ImGui::CollapsingHeader(propGuiID, ImGuiTreeNodeFlags_DefaultOpen);
			
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

					editorAction.SetOwners(nullptr, owner, prop);
					editorAction.SaveAsSavedProperty();

					if (checkEnabledState && !prop.enabledState)
					{
						continue;
					}

					if (whitelistedProprties)
					{
						bool found = whitelistedProprties->end() != eastl::find(whitelistedProprties->begin(), whitelistedProprties->end(), prop.propName);

						if ((!useAsBlacklisted && !found) || (useAsBlacklisted && found))
						{
							continue;
						}
					}

					if (prop.type != Type::FileName && prop.type != Type::Callback && prop.type != Type::Array)
					{
						StringUtils::Printf(propGuiID, 256, "###%s%s%i", categoriesData[j].name.c_str(), guiID, i);
					}

					if (prop.type == Type::Callback)
					{
						ImGui::Columns(1);

						StringUtils::Printf(propGuiID, 256, "%s%s%i", prop.propName.c_str(), propGuiID, i);

						if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
						{
							prop.callback(owner);
						}

						ImGui::NextColumn();

						ImGui::Columns(2);
					}
					else
					if (prop.type == Type::Array)
					{
						StringUtils::Printf(propGuiID, 256, "%s###%s%i", prop.propName.c_str(), guiID, i);
						bool items_open = ImGui::TreeNode(propGuiID);

						StringUtils::Printf(propGuiID, 256, "Add###%s%s%iAdd", categoriesData[j].name.c_str(), guiID, i);

						ImGui::NextColumn();

						int selItem = -1;

						if (prop.adapter->selItem != nullptr)
						{
							selItem = *prop.adapter->selItem;
						}

						if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
						{
							prop.adapter->PushBack();
							prop.adapter->GetMetaData()->Prepare(prop.adapter->GetItem((int)prop.adapter->GetSize() - 1), root);
							prop.adapter->GetMetaData()->SetDefValues();
							prop.changed = true;

							if (prop.adapter->selItem != nullptr)
							{
								selItem = prop.adapter->GetSize() - 1;
							}

							if (prop.adapter->itemAddedCallback)
							{
								(((Object*)owner)->*prop.adapter->itemAddedCallback)();
							}
						}

						ImGui::NextColumn();

						if (items_open)
						{
							if (prop.adapter->selItem != nullptr)
							{
								ImGui::Text("Selection");
								ImGui::NextColumn();

								float width = ImGui::GetContentRegionAvail().x * 0.3f;

								if (ImGui::Button("Prev", ImVec2(width, 0.0f)))
								{
									if (selItem > 0)
									{
										selItem--;
									}
								}

								ImGui::SameLine();

								if (ImGui::Button("DeSel", ImVec2(width, 0.0f)))
								{
									selItem = -1;
								}

								ImGui::SameLine();

								if (ImGui::Button("Next", ImVec2(width, 0.0f)))
								{
									if (selItem < prop.adapter->GetSize() - 1)
									{
										selItem++;
									}
								}

								ImGui::NextColumn();
							}

							int count = prop.adapter->GetSize();
							int index2delete = -1;

							for (int i = 0; i < count; i++)
							{
								bool itemOpen = ImGui::TreeNode(StringUtils::PrintTemp("Item %i###%s%i", i, guiID, i));

								ImGui::NextColumn();

								StringUtils::Printf(propGuiID, 256, "Del###%s%s%iDel", categoriesData[j].name.c_str(), guiID, i);

								float width = (prop.adapter->selItem != nullptr) ? ImGui::GetContentRegionAvail().x * 0.5f : ImGui::GetContentRegionAvail().x;

								if (prop.adapter->selItem != nullptr)
								{
									if (selItem == i)
									{
										if (ImGui::Button(StringUtils::PrintTemp("Deselect###%s%s%iDesel", categoriesData[j].name.c_str(), guiID, i), ImVec2(width, 0.0f)))
										{
											selItem = -1;
										}
									}
									else
									{
										if(ImGui::Button(StringUtils::PrintTemp("Select###%s%s%iSel", categoriesData[j].name.c_str(), guiID, i), ImVec2(width, 0.0f)))
										{
											selItem = i;
										}
									}

									ImGui::SameLine();
								}

								if (ImGui::Button(propGuiID, ImVec2(width, 0.0f)))
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
								prop.changed = true;
							}

							ImGui::TreePop();
						}

						if (prop.adapter->selItem != nullptr && selItem != *prop.adapter->selItem)
						{
							*prop.adapter->selItem = selItem;

							if (prop.adapter->gizmoCallback)
							{
								(((Object*)owner)->*prop.adapter->gizmoCallback)();
							}
						}
					}
					else
					if (prop.type == Type::Transform)
					{
						Transform* transform = (Transform*)prop.value;

						if (transform->transformFlag & TransformFlag::MoveXYZ)
						{
							Math::Vector3 position = transform->position;

							if (ImGuiVector(transform->transformFlag & TransformFlag::MoveX ? &position.x : nullptr,
											transform->transformFlag & TransformFlag::MoveY ? &position.y : nullptr,
											transform->transformFlag & TransformFlag::MoveZ ? &position.z : nullptr,
											nullptr, "Position", propGuiID))
							{
								transform->position = position;
								prop.changed = true;
							}
						}

						if (transform->transformFlag & TransformFlag::RotateXYZ)
						{
							Math::Vector3 rotation = transform->rotation;

							if (ImGuiVector(transform->transformFlag & TransformFlag::RotateX ? &rotation.x : nullptr,
											transform->transformFlag & TransformFlag::RotateY ? &rotation.y : nullptr,
											transform->transformFlag & TransformFlag::RotateZ ? &rotation.z : nullptr,
											nullptr, "Rotation", propGuiID))
							{
								transform->rotation = rotation;
								prop.changed = true;
							}
						}

						if (transform->transformFlag & TransformFlag::ScaleXYZ)
						{
							Math::Vector3 scale = transform->scale;

							if (ImGuiVector(transform->transformFlag & TransformFlag::ScaleX ? &scale.x : nullptr,
											transform->transformFlag & TransformFlag::ScaleY ? &scale.y : nullptr,
											transform->transformFlag & TransformFlag::ScaleZ ? &scale.z : nullptr,
											nullptr, "Scale", propGuiID))
							{
								transform->scale = scale;
								prop.changed = true;
							}
						}

						if (transform->transformFlag & TransformFlag::SizeXYZ)
						{
							if (ImGuiVector(transform->transformFlag & TransformFlag::SizeX ? &transform->size.x : nullptr,
											transform->transformFlag & TransformFlag::SizeY ? &transform->size.y : nullptr,
											transform->transformFlag & TransformFlag::SizeZ ? &transform->size.z : nullptr, nullptr, "Size", propGuiID))
							{
								prop.changed = true;
							}
						}

						if (transform->transformFlag & TransformFlag::RectSizeXY)
						{
							if (ImGuiVector(transform->transformFlag & TransformFlag::RectSizeX ? &transform->size.x : nullptr,
											transform->transformFlag & TransformFlag::RectSizeY ? &transform->size.y : nullptr,
											nullptr, nullptr, "Size", propGuiID))
							{
								prop.changed = true;
							}
						}

						if (transform->transformFlag & TransformFlag::RectAnchorn)
						{
							if (ImGuiVector(transform->transformFlag & TransformFlag::RectAnchorn ? &transform->offset.x : nullptr,
											transform->transformFlag & TransformFlag::RectAnchorn ? &transform->offset.y : nullptr,
											nullptr, nullptr, "Offset", propGuiID))
							{
								prop.changed = true;
							}
						}
					}
					else
					{
						ImGui::Text(prop.propName.c_str());
						ImGui::NextColumn();

						ImGui::SetNextItemWidth(-1);

						if (prop.type == Type::Boolean)
						{
							if (ImGui::Checkbox(propGuiID, (bool*)prop.value))
							{
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::Integer)
						{
							if (ImGui::InputInt(propGuiID, (int*)prop.value))
							{
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::Float)
						{
							if (ImGui::InputFloat(propGuiID, (float*)prop.value))
							{
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::String)
						{
							eastl::string& str = *((eastl::string*)prop.value);

							if (ImGuiHelper::InputString(propGuiID, str))
							{
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::FileName)
						{
							eastl::string* str = (eastl::string*)prop.value;

							StringUtils::Printf(propGuiID, 256, "%s###%s%s%i", str->c_str()[0] ? str->c_str() : "File not set", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x - 30.0f, 0.0f)))
							{
								const char* fileName = editor.OpenFileDialog("Any file", nullptr, true);

								if (fileName)
								{
									char relativeName[512];
									StringUtils::GetCropPath(Orin::root.GetPath(Root::Path::Assets), fileName, relativeName, 512);

									str->assign(relativeName);
									prop.changed = true;
								}
							}

							if (str->c_str()[0] && (ImGui::IsItemActive() || ImGui::IsItemHovered()))
							{
								ImGui::SetTooltip(str->c_str());
							}

							ImGui::SameLine();

							StringUtils::Printf(propGuiID, 256, "Del###%s%s%iDel", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(30.0f, 0.0f)))
							{
								str->assign("");
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::Color)
						{
							if (ImGui::ColorEdit4(propGuiID, (float*)prop.value))
							{
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::Vector2)
						{
							if (ImGui::InputFloat2(propGuiID, (float*)prop.value))
							{
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::Vector3)
						{
							if (ImGui::InputFloat3(propGuiID, (float*)prop.value))
							{
								prop.changed = true;
							}
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

							if (ImGuiHelper::InputCombobox(propGuiID, index, enumData.names, enumData.enumList))
							{
								*((int*)prop.value) = enumData.values[index];
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::EnumString)
						{
							int index = 0;

							MetaDataEnum& enumData = enums[prop.defvalue.enumIndex];
							eastl::string& str = *((eastl::string*)prop.value);

							for (int i = 0; i < enumData.names.size(); i++)
							{
								if (enumData.names[i] == str)
								{
									index = i;
									break;
								}
							}

							if (ImGuiHelper::InputCombobox(propGuiID, index, enumData.names, enumData.enumList))
							{
								str = enumData.names[index];
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::AssetTexture)
						{
							AssetTextureRef* ref = reinterpret_cast<AssetTextureRef*>(prop.value);

							if (ref->Get())
							{
								ImGui::BeginGroup();

								ref->ImGuiImage(120.0f);

								if (ref->Get() && (ImGui::IsItemActive() || ImGui::IsItemHovered()))
								{
									ImGui::SetTooltip(ref->Get()->GetPath().c_str());
								}

								StringUtils::Printf(propGuiID, 256, "Delete###%s%s%iDel", categoriesData[j].name.c_str(), guiID, i);

								if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
								{
									*ref = AssetTextureRef();
									prop.changed = true;
								}

								ImGui::EndGroup();
							}
							else
							{
								StringUtils::Printf(propGuiID, 256, "%s###%s%s%i", "None", categoriesData[j].name.c_str(), guiID, i);

								if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
								{
								}
							}

							if (ImGui::BeginDragDropTarget())
							{
								const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ASSET_TEX", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

								if (payload)
								{
									AssetTextureRef* assetRef = reinterpret_cast<AssetTextureRef**>(payload->Data)[0];

									*ref = *assetRef;
									prop.changed = true;
								}
							}
						}
						else
						if (prop.type == Type::AssetAnimGraph2D)
						{
							AssetAnimGraph2DRef* ref = reinterpret_cast<AssetAnimGraph2DRef*>(prop.value);

							StringUtils::Printf(propGuiID, 256, "%s###%s%s%i", ref->Get() ? ref->Get()->GetName().c_str() : "None", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x - 30.0f, 0.0f)))
							{
							}

							if (ref->Get() && (ImGui::IsItemActive() || ImGui::IsItemHovered()))
							{
								ImGui::SetTooltip(ref->Get()->GetPath().c_str());
							}

							if (ImGui::BeginDragDropTarget())
							{
								const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ASSET_ANIM_GRAPH_2D", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

								if (payload)
								{
									AssetAnimGraph2DRef* assetRef = reinterpret_cast<AssetAnimGraph2DRef**>(payload->Data)[0];

									*ref = *assetRef;
									ref->Reset();

									prop.changed = true;
								}
							}

							ImGui::SameLine();

							StringUtils::Printf(propGuiID, 256, "Del###%s%s%iDel", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(30.0f, 0.0f)))
							{
								*ref = AssetAnimGraph2DRef();
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::AssetTileSet)
						{
							AssetTileSetRef* ref = reinterpret_cast<AssetTileSetRef*>(prop.value);

							StringUtils::Printf(propGuiID, 256, "%s###%s%s%i", ref->Get() ? ref->Get()->GetName().c_str() : "None", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x - 70.0f, 0.0f)))
							{
							}

							if (ref->Get() && (ImGui::IsItemActive() || ImGui::IsItemHovered()))
							{
								ImGui::SetTooltip(ref->Get()->GetPath().c_str());
							}

							if (ImGui::BeginDragDropTarget())
							{
								const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ASSET_TILE_SET", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

								if (payload)
								{
									AssetTileSetRef* assetRef = reinterpret_cast<AssetTileSetRef**>(payload->Data)[0];

									*ref = *assetRef;
									prop.changed = true;
								}
							}

							ImGui::SameLine();

							StringUtils::Printf(propGuiID, 256, "Ed###%s%s%iEd", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(30.0f, 0.0f)))
							{
								TileSetWindow::StartEdit(ref->Get());
							}

							ImGui::SameLine();

							StringUtils::Printf(propGuiID, 256, "Del###%s%s%iDel", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(30.0f, 0.0f)))
							{
								*ref = AssetTileSetRef();
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::AssetSpritesLayer)
						{
							AssetSpritesLayerRef* ref = reinterpret_cast<AssetSpritesLayerRef*>(prop.value);

							StringUtils::Printf(propGuiID, 256, "%s###%s%s%i", ref->Get() ? ref->Get()->GetName().c_str() : "None", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x - 70.0f, 0.0f)))
							{
							}

							if (ref->Get() && (ImGui::IsItemActive() || ImGui::IsItemHovered()))
							{
								ImGui::SetTooltip(ref->Get()->GetPath().c_str());
							}

							if (ImGui::BeginDragDropTarget())
							{
								const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ASSET_SPRITES_LAYER", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

								if (payload)
								{
									AssetSpritesLayerRef* assetRef = reinterpret_cast<AssetSpritesLayerRef**>(payload->Data)[0];

									*ref = *assetRef;
									prop.changed = true;
								}
							}

							ImGui::SameLine();

							StringUtils::Printf(propGuiID, 256, "Del###%s%s%iDel", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(30.0f, 0.0f)))
							{
								*ref = AssetSpritesLayerRef();
								prop.changed = true;
							}
						}
						else
						if (prop.type == Type::SceneEntity)
						{
							SceneEntityRefBase* ref = reinterpret_cast<SceneEntityRefBase*>(prop.value);

							StringUtils::Printf(propGuiID, 256, "%s###%s%s%i", ref->GetSceneEntity() ? ref->GetSceneEntity()->GetName() : "None", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(ImGui::GetContentRegionAvail().x - 30.0f, 0.0f)))
							{
							}

							if (ImGui::BeginDragDropTarget())
							{
								const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

								if (payload)
								{
									uint64_t temp = *((uint64_t*)payload->Data);
									SceneEntity* entity = (SceneEntity*)temp;

									ref->SetEntity(entity);
									ref->uid = entity->GetUID();

									prop.changed = true;
								}
							}

							ImGui::SameLine();

							StringUtils::Printf(propGuiID, 256, "Del###%s%s%iDel", categoriesData[j].name.c_str(), guiID, i);

							if (ImGui::Button(propGuiID, ImVec2(30.0f, 0.0f)))
							{
								ref->SetEntity(nullptr);
								ref->uid = 0;

								prop.changed = true;
							}
						}

						ImGui::NextColumn();
					}

					/*if (prop.changed)
					{
						editorAction.SaveAsProperty();

						editor.AddAction(new MetaDataPropertyAction(editorAction));
					}*/
				}
			}
		}
	}

	bool MetaData::IsValueWasChanged()
	{
		bool res = false;

		for (auto& prop : properties)
		{
			if (prop.type == Type::Array)
			{
				res |= prop.adapter->GetMetaData()->IsValueWasChanged();
			}
			else
			{
				res |= prop.changed;
				prop.changed = false;
			}
		}

		return res;
	}
	#endif
}
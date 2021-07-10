
#pragma once

#include "Support/Support.h"
#include "Support/Delegate.h"
#include "Root/Files/JSONReader.h"
#include "Root/Files/JSONWriter.h"

/**
\ingroup gr_code_common
*/

namespace Oak
{
	class Scene;

	struct MetaDataEnum
	{
		int defIndex;
		eastl::string enumList;
		eastl::vector<eastl::string> names;
		eastl::vector<int> values;
	};

	/**
	\brief MetaData

	Class for storing class meta data which used in serialization/desirialization and editiong property

	*/

	class MetaData
	{

	public:

	#ifndef DOXYGEN_SKIP
		enum class Type
		{
			Boolean,
			Integer,
			Float,
			String,
			FileName,
			Color,
			Enum,
			EnumString,
			Callback,
			Transform,
			AssetTexture,
			AssetAnimGraph2D,
			SceneEntity,
			Array
		};

		union DefValue
		{
			bool  boolean;
			int   integer;
			float flt;
			int   string;
			float color[4];
			int   enumIndex;
		};

		eastl::vector<MetaDataEnum> enums;
		eastl::vector<eastl::string> defStrings;

		#ifdef OAK_EDITOR
		typedef void(*Callback)(void* owner);
		typedef void(*EnumStringCallback)(void* owner, void** strings);
		#endif

		struct ArrayAdapter
		{
			uint8_t* value = nullptr;

			#ifdef OAK_EDITOR
			int64_t sel_item_offset = -1;
			int32_t* sel_item = nullptr;
			Object::DelegateSimple gizmoCallback = nullptr;
			#endif

			virtual void Resize(int length) {};
			virtual int GetSize() { return 0; };
			virtual void PushBack() {};
			virtual void Delete(int index) {};
			virtual uint8_t* GetItem(int index) { return nullptr; };
			virtual MetaData* GetMetaData() { return nullptr; };
		};

		template <typename StructType>
		struct ArrayAdapterImpl : public ArrayAdapter
		{
			inline eastl::vector<StructType>* Vec()
			{
				return (eastl::vector<StructType>*)value;
			};
			void Resize(int length) override
			{
				Vec()->resize(length);
			};
			virtual int GetSize()
			{
				return (int)Vec()->size();
			};
			void PushBack() override
			{
				Vec()->push_back(StructType());
			};
			void Delete(int index) override
			{
				Vec()->erase(Vec()->begin() + index);
			};
			uint8_t* GetItem(int index) override
			{
				return (uint8_t*)&Vec()->at(index);
			};
			MetaData* GetMetaData() override
			{
				return &StructType::meta_data;
			}
		};

		struct Property
		{
			int64_t offset;
			Type type;
			DefValue defvalue;
			uint8_t* value;
			eastl::string name;

			#ifdef OAK_EDITOR
			eastl::string catName;
			eastl::string propName;
			eastl::string brief;
			Callback callback;
			EnumStringCallback enum_callback;
			bool changed = false;
			#endif

			ArrayAdapter*  adapter = nullptr;
		};

		bool inited = false;
		void* owner = nullptr;
		void* root = nullptr;

		#ifdef OAK_EDITOR
		char guiID[64];

		struct CategoryData
		{
			eastl::string name;
			eastl::vector<int> indices; 
		};

		eastl::vector<CategoryData> categoriesData;
		#endif
		eastl::vector<Property> properties;

		MetaData() = default;
		virtual void Init() = 0;

	#endif

		void Prepare(void* owner, void* root = nullptr);
		void SetDefValues();
		void Load(JsonReader& reader);
		void PostLoad(Scene* scene);
		void Save(JsonWriter& writer);

		void Copy(void* source, eastl::vector<Property>& sourceProperties);

		#ifndef DOXYGEN_SKIP
		#ifdef OAK_EDITOR
		void ImGuiWidgets();
		bool ImGuiVector(float* x, float* y, float* z, float* w, const char* name, const char* propID);
		void ConstructCategoriesData();
		bool IsValueWasChanged();
		#endif
		#endif
	};

	#define META_DATA_DECL(className)\
	class MetaDataImpl : public MetaData\
	{\
	public:\
		virtual void Init();\
	};\
	static MetaDataImpl meta_data;\
	MetaData* GetMetaData() override;

	#define META_DATA_DECL_BASE(className)\
	class MetaDataImpl : public MetaData\
	{\
	public:\
		virtual void Init();\
	};\
	static MetaDataImpl meta_data;\
	virtual MetaData* GetMetaData();

	#define META_DATA_DESC(className)\
	className::MetaDataImpl className::meta_data;\
	MetaData* className::GetMetaData() { return &className::meta_data; };\
	void className::MetaDataImpl::Init()\
	{

	#define META_DATA_DESC_END()\
	}

	#define memberOFFSET(s,m) ((size_t)(&reinterpret_cast<s*>(100000)->m)-100000)

	#define BASE_PROP(className, classMember, defValue, strCatName, strPropName, tp, defValueName, doxy_brief)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = tp;\
		prop.defvalue.defValueName = defValue;\
		prop.name = #classMember;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		prop.brief = doxy_brief;\
		properties.push_back(prop);\
	}

	#define BOOL_PROP(className, classMember, defValue, strCatName, strPropName, doxy_brief)\
	BASE_PROP(className, classMember, defValue, strCatName, strPropName, Type::Boolean, boolean, doxy_brief)

	#define INT_PROP(className, classMember, defValue, strCatName, strPropName, doxy_brief)\
	BASE_PROP(className, classMember, defValue, strCatName, strPropName, Type::Integer, integer, doxy_brief)

	#define FLOAT_PROP(className, classMember, defValue, strCatName, strPropName, doxy_brief)\
	BASE_PROP(className, classMember, defValue, strCatName, strPropName, Type::Float, flt, doxy_brief)

	#define BASE_STRING_PROP(className, classMember, defValue, strCatName, strPropName, tp)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = tp;\
		defStrings.push_back(defValue);\
		prop.defvalue.string = (int)defStrings.size() - 1;\
		prop.name = #classMember;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		properties.push_back(prop);\
	}


	#define STRING_PROP(className, classMember, defValue, strCatName, strPropName)\
	BASE_STRING_PROP(className, classMember, defValue, strCatName, strPropName, Type::String)

	#define FILENAME_PROP(className, classMember, defValue, strCatName, strPropName)\
	BASE_STRING_PROP(className, classMember, defValue, strCatName, strPropName, Type::FileName)

	#define COLOR_PROP(className, classMember, defValue, strCatName, strPropName)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = Type::Color;\
		Oak::Color tmp = Oak::defValue;\
		memcpy(prop.defvalue.color, &tmp.r, sizeof(float) * 4);\
		prop.name = #classMember;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		properties.push_back(prop);\
	}

	#define ENUM_PROP(className, classMember, defValue, strCatName, strPropName, doxy_brief)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.name = #classMember;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		prop.type = Type::Enum;\
		prop.brief = doxy_brief;\
		MetaDataEnum enm;\
		enm.defIndex = (int)defValue;

	#define ENUM_ELEM(name, value)\
		enm.names.push_back(name);\
		enm.values.push_back((int)value);

	#define ENUM_END\
		enums.push_back(enm);\
		prop.defvalue.enumIndex = (int)enums.size() - 1;\
		properties.push_back(prop);\
	}

	#ifdef OAK_EDITOR
	#define STRING_ENUM_PROP(className, classMember, set_callback, strCatName, strPropName)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = Type::EnumString;\
		prop.name = #classMember;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		prop.enum_callback = set_callback;\
		properties.push_back(prop);\
	}
	#else
	#define STRING_ENUM_PROP(className, classMember, strCatName, strPropName)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = Type::EnumString;\
		prop.name = #classMember;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		properties.push_back(prop);\
	}
	#endif

	#ifdef OAK_EDITOR
	#define CALLBACK_PROP(className, set_callback, strCatName, strPropName)\
	{\
		Property prop;\
		prop.type = Type::Callback;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		prop.callback = set_callback;\
		properties.push_back(prop);\
	}
	#endif

	#define ARRAY_PROP_INST(className, classMember, structType, strCatName, strPropName, selItemClassName, selItemClassMember)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = Type::Array;\
		prop.name = #classMember;\
		prop.propName = strPropName; \
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		prop.adapter = new ArrayAdapterImpl<structType>;\
		prop.adapter->sel_item_offset = memberOFFSET(selItemClassName, selItemClassMember);\
		properties.push_back(prop);\
	}

	#define ARRAY_PROP_INST_CALLGIZMO(className, classMember, structType, strCatName, strPropName, selItemClassName, selItemClassMember, setGizmoCallback)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = Type::Array;\
		prop.name = #classMember;\
		prop.propName = strPropName; \
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		prop.adapter = new ArrayAdapterImpl<structType>;\
		prop.adapter->sel_item_offset = memberOFFSET(selItemClassName, selItemClassMember);\
		prop.adapter->gizmoCallback = (Object::DelegateSimple)&className::setGizmoCallback;\
		properties.push_back(prop);\
	}

	#define ARRAY_PROP(className, classMember, structType, strCatName, strPropName)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = Type::Array;\
		prop.name = #classMember;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		prop.adapter = new(nullptr, 0) ArrayAdapterImpl<structType>;\
		properties.push_back(prop);\
	}

	#define ASSET_TEXTURE_PROP(className, classMember, strCatName, strPropName)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = Type::AssetTexture;\
		prop.name = #classMember;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		properties.push_back(prop);\
	}

	#define ASSET_ANIM_GRAPH_2D_PROP(className, classMember, strCatName, strPropName)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = Type::AssetAnimGraph2D;\
		prop.name = #classMember;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		properties.push_back(prop);\
	}

	#define TRANSFORM_PROP(className, classMember, strPropName)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = Type::Transform;\
		prop.name = #classMember;\
		prop.catName = strPropName;\
		prop.propName = strPropName;\
		properties.push_back(prop);\
	}

	#define SCENEOBJECT_PROP(className, classMember, strCatName, strPropName)\
	{\
		Property prop;\
		prop.offset = memberOFFSET(className, classMember);\
		prop.type = Type::SceneEntity;\
		prop.name = #classMember;\
		prop.catName = strCatName;\
		prop.propName = strPropName;\
		properties.push_back(prop);\
	}
}
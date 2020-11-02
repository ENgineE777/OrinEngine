
#include "TestEntity.h"

#include "Root/Root.h"

namespace Oak
{
	META_DATA_DESC(TestItem)
		BOOL_PROP(TestItem, boolProp, false, "Values", "Bool value", "sdfdsf")
		INT_PROP(TestItem, intProp, 80, "Values", "Int value", "sdfdsf")
		FLOAT_PROP(TestItem, floatProp, 1.0f, "Values", "Float value", "sdfdsf")
	META_DATA_DESC_END()

	void CallbackTest(void* owner)
	{
		TestEntity* jkl = (TestEntity*)owner;
	}

	CLASSREG(SceneEntity, TestEntity, "TestEntity")

	META_DATA_DESC(TestEntity)
		BASE_SCENE_ENTITY_PROP(TestEntity)
		BOOL_PROP(TestEntity, boolProp, false, "Category1", "Bool value", "sdfdsf")
		INT_PROP(TestEntity, intProp, 80, "Category1", "Int value", "sdfdsf")
		FLOAT_PROP(TestEntity, floatProp, 1.0f, "Category1", "Float value", "sdfdsf")
		FLOAT_PROP(TestEntity, floatProp2, 5.0f, "Category2", "Another float value", "sdfdsf")
		STRING_PROP(TestEntity, strProp, "ABC", "Category2", "String value")
		FILENAME_PROP(TestEntity, filenameProp, "", "Category2", "File Name")
		COLOR_PROP(TestEntity, colorProp, COLOR_GREEN, "Category3", "Color value")
		ENUM_PROP(TestEntity, enumProp, TestEntity::Shape::Box, "Category3", "Enum value", "brief")
			ENUM_ELEM("Box", 0)
			ENUM_ELEM("Sphere", 1)
			ENUM_ELEM("Cylinder", 2)
		ENUM_END
		CALLBACK_PROP(TestEntity, CallbackTest, "Values", "Callaback")
		ARRAY_PROP(TestEntity, itemsProp, TestItem, "Values", "Items array")
	META_DATA_DESC_END()
}

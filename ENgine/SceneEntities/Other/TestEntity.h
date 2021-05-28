
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"

namespace Oak
{
	class TestItem
	{
	public:

		bool boolProp = false;
		int intProp = 50;
		float floatProp = 1.0f;

		META_DATA_DECL_BASE(TestItem)
	};

	class TestEntity : public SceneEntity
	{
	public:

		enum Shape
		{
			Box,
			Sphere,
			Cylinder
		};

		bool boolProp = false;
		int intProp = 50;
		float floatProp = 1.0f;
		float floatProp2 = 1.0f;
		eastl::string strProp;
		eastl::string filenameProp;
		Color colorProp;
		Shape enumProp;
		eastl::vector<TestItem> itemsProp;

		META_DATA_DECL_BASE(TestEntity)

		void Init() override {};
	};
}

#pragma once

#include "PhysBox3D.h"

namespace Oak
{
	class CLASS_DECLSPEC PhysTriger3D : public PhysBox3D
	{
	protected:

	public:

		virtual ~PhysTriger3D() = default;

		META_DATA_DECL(PhysTriger3D)

		void Init() override;
		void ApplyProperties() override;

		virtual void OnContactStart(int index, SceneEntity* entity, int contactIndex);
		virtual void OnContactEnd(int index, SceneEntity* entity, int  contactIndex);
	};
}

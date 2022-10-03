
#pragma once

#include "SceneEntities/3D/PhysTriger3D.h"

namespace Oak
{
	class CLASS_DECLSPEC PhysTriger2D : public PhysTriger3D
	{
	protected:

	public:

		virtual ~PhysTriger2D() = default;

		META_DATA_DECL(PhysTriger3D)

		void Init() override;
		void Play() override;
		void Draw(float dt) override;
	};
}

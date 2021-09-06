
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Root/Meshes/Meshes.h"
#include "root/Physics/PhysController.h"
#include "root/Physics/PhysScene.h"

namespace Oak
{
	class CLASS_DECLSPEC KinematicCapsule2D : public SceneEntity
	{
		PhysScene::BodyUserData bodyData;
		uint32_t physGroup;

		Math::Vector3 upVector;
		bool affectOnParent = false;

	public:

		META_DATA_DECL_BASE(KinematicCapsule2D)

	#ifndef DOXYGEN_SKIP

		PhysController* controller = nullptr;

		KinematicCapsule2D() = default;
		virtual ~KinematicCapsule2D() = default;

		void Init() override;
		void OnVisiblityChange(bool set) override;
		void Play() override;
		void Move(Math::Vector2 dir, uint32_t ignoreGroup = 0);
		void EditorDraw(float dt);
	#endif
	};
}
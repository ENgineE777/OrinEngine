
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Root/Meshes/Meshes.h"
#include "root/Physics/PhysController.h"
#include "root/Physics/PhysScene.h"

namespace Orin
{
	class CLASS_DECLSPEC KinematicCapsule2D : public SceneEntity
	{
		PhysScene::BodyUserData bodyData;
		uint32_t physGroup;

		bool affectOnParent = false;
		bool needUpdate = false;

		float height = 16.0f;
		float radius = 16.0f;
		float slopeLimit = cosf(Math::Radian * 20.0f);
		float stepOffset = 0.5f;

		Math::Vector3 GetTopOffset() const;
		Math::Vector3 GetBottomOffset() const;

	public:

		META_DATA_DECL_BASE(KinematicCapsule2D)

	#ifndef DOXYGEN_SKIP

		PhysController* controller = nullptr;

		KinematicCapsule2D() = default;
		virtual ~KinematicCapsule2D() = default;

		void Init() override;
		void ApplyProperties() override;
		void OnVisiblityChange(bool set) override;
		void Play() override;
		Math::Vector3 GetPosition();
		void SetPosition(Math::Vector3 pos);
		void Move(Math::Vector2 dir, uint32_t group = 0);
		void UpdateParent(float dt);
		void EditorDraw(float dt);
		void DebugDraw();

		void Release() override;
	#endif
	};
}

#pragma once

#define WIN32_LEAN_AND_MEAN

#include "SceneEntities/3D/terrain.h"
#include "SceneEntities/Physics/3D/PhysBox3D.h"

#include "Root/Physics/Physics.h"

#include "Root/Render/Render.h"
#include "Root/Scenes/SceneEntity.h"
#include "tank.h"
#include "Root/Meshes/Meshes.h"

namespace Orin
{
	class TankClient : public SceneEntity
	{
	public:

		META_DATA_DECL(TankClient)

		Math::Vector2 angles;
		Math::Matrix view;
		Math::Matrix proj;

		int alias_forward;
		int alias_strafe;
		int alias_rotate_active;
		int alias_rotate_x;
		int alias_rotate_y;
		int alias_fire;

		eastl::string base_model_name;
		eastl::string tower_model_name;

		struct Instance
		{
			bool is_contralable;
			int id;
			Tank::ClientState clientState;
			Tank::ServerState serverState;
			Mesh::Instance* base_drawer;
			Mesh::Instance* tower_drawer;
		};

		eastl::vector<Instance> instances;

		virtual void Init();

		void AddIsntance(int id, bool  is_contralable);

		void Play() override;
		void Stop();
		void Update(float dt);

		void SendClientState(float dt);
	};
}
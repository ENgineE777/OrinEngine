
#pragma once

#include "SceneEntities/3D/terrain.h"
#include "SceneEntities/Physics/3D/PhysBox3D.h"

#include "Root/Physics/Physics.h"

#include "Root/Render/Render.h"
#include "Root/Scenes/SceneEntity.h"

#include "Root/Network/Network.h"

namespace Orin
{
	class Tank : public SceneEntity, public NetworkDelegate
	{
	public:

		enum PACKETID
		{
			CLIENTID = 0,
			ADDINSTANCE,
			CLIENTSTATE,
			SERVERSTATE
		};

		META_DATA_DECL(Tank)

		struct Projectile
		{
			static float maxTimeLife;
			static float speed;
			static float splashTime;
			static float splashMaxRadius;

			Math::Vector3 pos;
			Math::Vector3 claster_pos[5];
			Math::Vector3 dir;
			int stage;
			float lifetime;
			int state;
			bool special;
		};

		eastl::vector<Projectile> projectiles;
 
		struct ServerState
		{
			int hp = 100;
			int ammo = 50;
			bool is_ai = false;
			int special = 0;
			float angle = 0;
			Math::Vector3 pos = 0.0f;
			float move_speed = 0.0f;
			float strafe_speed = 0.0f;
			float shoot_cooldown = 0.0f;
			float tower_angel = 0.0f;
			float timeStamp = 0.0f;
			PhysController* controller = nullptr;
		};

		struct ClientState
		{
			int up = 0;
			int rotate = 0;
			bool fired = false;
			bool special_fired = false;
			float needed_tower_angel = 0.0f;
			float timeStamp = 0.0f;
			Math::Vector3 gun_pos;
			Math::Vector3 gun_dir;
		};

		class TankClient* client;

		bool is_server = false;

		bool bonuses_places = false;

		struct Bonus
		{
			int type = 0;
			Math::Vector3 pos;
			float cooldown = -1.0f;
		};

		eastl::vector<Bonus> bonuses;

		int clientID;
		NetworkServer netServer;
		NetworkClient netClient;

		float time = 0.0f;

		void Init();

		void AddInstance(int id, Math::Vector3 pos, bool is_bot);

		void Play() override;
		void Update(float dt);
		void AddSplash(Math::Vector3& pos, float radius, float force);

		void SendServerState(float dt);
		void SendClientState(float dt);

		void OnDataRecieved(int id, void* data, int size) override;
		void OnClientConnected(int id) override;
	};
}
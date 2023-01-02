
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"
#include "root/Assets/AssetAnimGraph2D.h"

namespace Orin
{
	class SimpleCharacter2D : public SceneEntity
	{
	public:
		META_DATA_DECL_BASE(SimpleCharacter2D)

		SimpleCharacter2D* target = nullptr;

		bool allow_move = false;
		float speed = 120.0f;
		float floor_width = 200.0f;
		float floor_height = 100.0f;
		bool is_enemy = true;
		bool flipped = false;
		int dir_horz = 0;
		int dir_vert = 0;
		float time_2_kick = 0.5f;
		float cur_time_2_kuck = 0;
		float cur_time_to_kick = -1.0f;
		float death_fly = -1.0f;
		float vanish_time = -1.0f;
		float arraive = -1.0f;
		float resp_time = -1.0f;
		int max_hp = 100;
		int cur_hp = 100;
		int next_kick = -1;

		bool moveBack = false;

		Math::Vector3 init_pos;

		AssetAnimGraph2DRef animGraph;

		virtual ~SimpleCharacter2D() = default;

		void Init() override;
		void ApplyProperties() override;
		void Update(float dt);
		void Draw(float dt);

		void Play() override;

		SimpleCharacter2D* FindTarget();
		void ControlPlayer(float dt);
		void ControlEnemy(float dt);
		void MakeHit(int damage);
		void Respawn();
		void Reset();

		void OnFrameChangeCallback(int frame, eastl::string& name, eastl::string& param);
	};
}


#include "SimpleCharacter2D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, SimpleCharacter2D, "SimpleCharacter2D")

	META_DATA_DESC(SimpleCharacter2D)
		BASE_SCENE_ENTITY_PROP(SimpleCharacter2D)
		ASSET_TEXTURE_PROP(SimpleCharacter2D, texture, "Visual", "Texture")
		FLOAT_PROP(SimpleCharacter2D, speed, 120.0f, "Properties", "Speed", "Speed of a charater")
		BOOL_PROP(SimpleCharacter2D, is_enemy, true, "Properties", "IsEnemy", "Definig if charcter is a enemy")
		INT_PROP(SimpleCharacter2D, max_hp, 100, "Properties", "HP", "Max Health of a character")
		FLOAT_PROP(SimpleCharacter2D, floor_width, 500.0f, "Properties", "FloorWidth", "Width of a level floor")
		FLOAT_PROP(SimpleCharacter2D, floor_height, 200.0f, "Properties", "FloorHeight", "Higth of a level floor")
	META_DATA_DESC_END()

	Sprite::FrameState SimpleCharacter2D::frameState;

	void SimpleCharacter2D::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = SpriteTransformFlags;

		sprite.frames.push_back(Sprite::Frame());

		Tasks(false)->AddTask(10, this, (Object::Delegate)&SimpleCharacter2D::Update);
		RenderTasks(false)->AddTask(0, this, (Object::Delegate)&SimpleCharacter2D::Draw);

		GetScene()->AddToGroup(this, "SimpleCharacter2D");
	}

	void SimpleCharacter2D::ApplyProperties()
	{
		sprite.texture = texture ? texture->GetTexture() : root.render.GetWhiteTexture();
		transform.size = Math::Vector3((float)sprite.texture->GetWidth(), (float)sprite.texture->GetHeight(), 0.0f);
	}

	void SimpleCharacter2D::Update(float dt)
	{
		if (!IsVisible())
		{
			return;
		}

		if (GetScene()->Playing())
		{
			if (arraive > 0.0f)
			{
				arraive -= dt;

				if (arraive < 0.0f)
				{
					arraive = -1.0f;
					//graph_instance.ActivateLink("Idle");
				}
				else
				{
					return;
				}
			}

			if (!target)
			{
				target = FindTarget();
			}

			if (!is_enemy)
			{
				ControlPlayer(dt);
			}
			else
			{
				ControlEnemy(dt);
			}

			if (cur_time_to_kick > 0.0f)
			{
				cur_time_to_kick -= dt;

				if (cur_time_to_kick < 0.0f)
				{
					cur_time_to_kick = -1.0f;

					Math::Vector2 kick_pos = Math::Vector2(transform.position.x, transform.position.x);
					kick_pos.x += flipped ? -120.0f : 120.0f;

					MakeHit(kick_pos, 25);

					if (target && target->cur_hp == 0)
					{
						target = nullptr;
					}
				}
			}

			if (target && allow_move)
			{
				flipped = transform.position.x > target->transform.position.x;
			}

			if (dir_horz == 0 && dir_vert == 0)
			{
				if (allow_move)
				{
					//graph_instance.ActivateLink("Idle");
					allow_move = false;
				}
			}
			else
			{
				transform.position.x = Math::Clamp(transform.position.x + dt * speed * dir_horz, -floor_width, floor_width);
				transform.position.y = Math::Clamp(transform.position.y + dt * speed * 0.75f * dir_vert, 0.0f, floor_height);
			}

			if (resp_time > 0.0f)
			{
				resp_time -= dt;

				if (resp_time < 0.0f)
				{
					resp_time = -1.0f;

					Respawn();
				}
			}

			if (vanish_time > 0.0f)
			{
				vanish_time -= dt;

				if (vanish_time < 0.0f)
				{
					vanish_time = -1.0f;

					resp_time = 3.0f;
				}
			}

			if (death_fly > 0.0f)
			{
				death_fly -= dt;

				transform.position.x += dt * (flipped ? 380.0f : -380.0f);

				if (death_fly < 0.0f)
				{
					death_fly = -1.0f;
					vanish_time = 2.0f;
				}
			}
		}
	}

	void SimpleCharacter2D::Draw(float dt)
	{
		if (!IsVisible())
		{
			return;
		}

		if (resp_time > 0.0f || (vanish_time > 0.0f && ((int)(vanish_time / 0.1f) % 2 == 0)))
		{
			return;
		}

		transform.position.z = (transform.position.y / floor_height) * 0.9f;
		transform.scale.x = (flipped ? -1.0f : 1.0f) * fabsf(transform.scale.x);
		
		transform.BuildMatrices();

		if (arraive > 0.0f)
		{
			//trans.mat_global.Pos().y -= 1024.0f * arraive;
		}

		//if (GetState() == SceneEntity::State::Active)
		{
			//graph_instance.Update(GetName(), 0, Script(), nullptr, dt);
		}

		Sprite::UpdateFrame(&sprite, &frameState, dt);

		Sprite::Draw(&transform, COLOR_WHITE, &sprite, &frameState, true, false);
	}

	SimpleCharacter2D* SimpleCharacter2D::FindTarget()
	{
		eastl::vector<Scene::Group*> out_group;
		GetScene()->GetGroup(out_group, "SimpleCharacter2D");

		for (auto group : out_group)
		{
			for (auto& object : group->entities)
			{
				SimpleCharacter2D* chraracter = (SimpleCharacter2D*)object;

				if (chraracter->cur_hp <= 0)
				{
					continue;
				}

				if (chraracter->is_enemy == !is_enemy)
				{
					return chraracter;
				}
			}
		}

		return nullptr;
	}

	void SimpleCharacter2D::ControlPlayer(float dt)
	{
		dir_horz = 0;

		if (root.controls.DebugKeyPressed("KEY_A", AliasAction::Pressed))
		{
			if (!allow_move)
			{
				allow_move = true;
				//allow_move = graph_instance.ActivateLink(target ? "WalkBack" : "Walk");
			}

			if (allow_move)
			{
				if (!target)
				{
					flipped = true;
				}

				dir_horz = -1;
			}
		}
		else
		if (root.controls.DebugKeyPressed("KEY_D", AliasAction::Pressed))
		{
			if (!allow_move)
			{
				allow_move = true;
				//allow_move = graph_instance.ActivateLink("Walk");
			}

			if (allow_move)
			{
				if (!target)
				{
					flipped = false;
				}

				dir_horz = 1;
			}
		}

		dir_vert = 0;

		if (root.controls.DebugKeyPressed("KEY_W", AliasAction::Pressed))
		{
			if (!allow_move)
			{
				allow_move = true;
				//allow_move = graph_instance.ActivateLink("WalkBack");
			}

			if (allow_move)
			{
				dir_vert = 1;
			}
		}
		else
		if (root.controls.DebugKeyPressed("KEY_S", AliasAction::Pressed))
		{
			if (!allow_move)
			{
				allow_move = true;
				//allow_move = graph_instance.ActivateLink("Walk");
			}

			if (allow_move)
			{
				dir_vert = -1;
			}
		}

		if (root.controls.DebugKeyPressed("KEY_E"))
		{
			/*if (graph_instance.ActivateLink("LegKick1"))
			{
				cur_time_to_kick = 0.45f;
				allow_move = false;
			}*/
		}

		if (root.controls.DebugKeyPressed("KEY_R"))
		{
			/*if (graph_instance.ActivateLink("LegKick2"))
			{
				cur_time_to_kick = 0.55f;
				allow_move = false;
			}*/
		}
	}

	void SimpleCharacter2D::ControlEnemy(float dt)
	{
		if (target)
		{
			//if (next_kick == -1)
			{
				//next_kick
			}

			if (fabs(transform.position.x - target->transform.position.x) > 200.0f)
			{
				if (!allow_move)
				{
					allow_move = true;
					//allow_move = graph_instance.ActivateLink("Walk");
				}

				if (allow_move)
				{
					dir_horz = (transform.position.x - target->transform.position.x) > 0.0f ? -1 : 1;
				}
			}
			else
			{
				dir_horz = 0;
			}

			if (fabsf(transform.position.x - target->transform.position.x) < 500.0f && fabsf(transform.position.y - target->transform.position.y) > 5.0f)
			{
				if (!allow_move)
				{
					allow_move = true;
					//allow_move = graph_instance.ActivateLink("Walk");
				}

				if (allow_move)
				{
					dir_vert = (transform.position.y - target->transform.position.y) > 0.0f ? -1 : 1;
				}
			}
			else
			{
				dir_vert = 0;
			}

			if (fabsf(transform.position.x - target->transform.position.x) < 200.0f && fabsf(transform.position.y - target->transform.position.y) < 5.0f)
			{
				cur_time_2_kuck += dt;
				if (cur_time_2_kuck > time_2_kick)
				{
					cur_time_2_kuck = 0.0f;

					bool sec_kick = Math::Rand() > 0.5;
					/*if (graph_instance.ActivateLink(sec_kick ? "LegKick1" : "LegKick2"))
					{
						cur_time_to_kick = sec_kick ? 0.55f : 0.85f;
						flipped = trans.pos.x > target->trans.pos.x;
						allow_move = false;
					}*/
				}
			}
			else
			{
				cur_time_2_kuck = 0.0f;
			}
		}
	}

	void SimpleCharacter2D::MakeHit(Math::Vector2 pos, int damage)
	{
		eastl::vector<Scene::Group*> out_group;
		GetScene()->GetGroup(out_group, "SimpleCharacter2D");

		for (auto group : out_group)
		{
			for (auto& object : group->entities)
			{
				SimpleCharacter2D* chraracter = (SimpleCharacter2D*)object;

				if (chraracter->is_enemy == !is_enemy && chraracter->cur_hp > 0)
				{
					if ((chraracter->transform.position.x - 85.0f) < pos.x && pos.x < (chraracter->transform.position.x + 85.0f) && fabsf(chraracter->transform.position.y - pos.y) < 15.0f)
					{
						/*if (chraracter->graph_instance.ActivateLink("Hit"))
						{
							chraracter->cur_hp -= damage;

							chraracter->cur_time_to_kick = -1.0f;
							chraracter->allow_move = false;

							if (chraracter->cur_hp <= 0)
							{
								chraracter->cur_hp = 0;
								chraracter->target = nullptr;
								chraracter->graph_instance.GotoNode("Death");
								chraracter->death_fly = 0.75f;
							}
						}*/
					}
				}
			}
		}
	}

	void SimpleCharacter2D::Respawn()
	{
		cur_hp = max_hp;

		arraive = 1.0f;

		//graph_instance.GotoNode("Resp");
	}

	void SimpleCharacter2D::Reset()
	{
		transform.position = init_pos;

		cur_hp = max_hp;

		allow_move = false;
		flipped = false;
		dir_horz = 0;
		dir_vert = 0;
		time_2_kick = 0.5f;
		cur_time_2_kuck = 0;
		cur_time_to_kick = -1.0f;
		death_fly = -1.0f;
		vanish_time = -1.0f;
		arraive = -1.0f;
		resp_time = -1.0f;
		next_kick = -1;

		//graph_instance.Reset();

		if (is_enemy)
		{
			Respawn();
		}
	}

	bool SimpleCharacter2D::Play()
	{
		init_pos = transform.position;

		Reset();

		return true;
	}
}
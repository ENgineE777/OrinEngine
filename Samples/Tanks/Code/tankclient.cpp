#include "tankclient.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, TankClient, "Sample", "TankClient")

	META_DATA_DESC(TankClient)
		BASE_SCENE_ENTITY_PROP(TankClient)
		FILENAME_PROP(TankClient, base_model_name, "", "Prop", "BaseModel")
		FILENAME_PROP(TankClient, tower_model_name, "", "Prop", "TowerModel")
	META_DATA_DESC_END()

	void TankClient::Init()
	{
		GetScene()->AddToGroup(this, "TankClient");

		angles.y = -Math::HalfPI;

		Tasks(false)->AddTask(0, this, (Object::Delegate)&TankClient::Update);
	}

	void TankClient::Play()
	{
		alias_forward = GetRoot()->GetControls()->GetAlias("Tank.MOVE_FORWARD");
		alias_strafe = GetRoot()->GetControls()->GetAlias("Tank.MOVE_STRAFE");
		alias_rotate_active = GetRoot()->GetControls()->GetAlias("Tank.ROTATE_ACTIVE");
		alias_rotate_x = GetRoot()->GetControls()->GetAlias("Tank.ROTATE_X");
		alias_rotate_y = GetRoot()->GetControls()->GetAlias("Tank.ROTATE_Y");
		alias_fire = GetRoot()->GetControls()->GetAlias("Tank.FIRE");
	}

	void TankClient::Stop()
	{
	}

	void TankClient::SendClientState(float dt)
	{

	}

	void TankClient::AddIsntance(int id, bool is_contralable)
	{
		Instance inst;
	
		inst.id = id;
		inst.is_contralable = is_contralable;

		inst.base_drawer = GetRoot()->GetMeshes()->LoadMesh(base_model_name.c_str(), Tasks(true));
		inst.tower_drawer = GetRoot()->GetMeshes()->LoadMesh(tower_model_name.c_str(), Tasks(true));

		instances.push_back(inst);
	}

	void TankClient::Update(float dt)
	{
		if (!GetScene()->IsPlaying())
		{
			return;
		}

		for (auto& inst : instances)
		{
			Math::Matrix mat;
			mat.RotateY(inst.serverState.angle);
			mat.Pos() = inst.serverState.pos;

			Math::Vector3 target_pt = 0.0f;
			PhysScene::RaycastDesc rcdesc;

			if (inst.is_contralable)
			{
				/*if (core.controls.GetAliasState(alias_rotate_active, Controls::Active))
				{
					angles.x -= core.controls.GetAliasValue(alias_rotate_x, true);

					if (angles.y > HALF_PI)
					{
						angles.y = HALF_PI;
					}

					if (angles.y < -HALF_PI)
					{
						angles.y = -HALF_PI;
					}
				}*/

				view.BuildView(mat.Pos() + Math::Vector3(0, 4.5f, 0.0f) - Math::Vector3(cosf(angles.x), sinf(angles.y), sinf(angles.x)) * 55, mat.Pos() + Math::Vector3(0,4.5f,0.0f), Math::Vector3(0, 1, 0));
				proj.BuildProjection(45.0f * Math::Radian, (float)GetRoot()->GetRender()->GetDevice()->GetHeight() / (float)GetRoot()->GetRender()->GetDevice()->GetWidth(), 1.0f, 1000.0f);

				GetRoot()->GetRender()->SetTransform(TransformStage::View, view);
				GetRoot()->GetRender()->SetTransform(TransformStage::Projection, proj);

				inst.clientState.needed_tower_angel = inst.serverState.angle;

				Math::Vector2 screepos = Math::Vector2((float)GetRoot()->GetControls()->GetAliasValue(alias_rotate_x, false) / (float)GetRoot()->GetRender()->GetDevice()->GetWidth(),
														(float)GetRoot()->GetControls()->GetAliasValue(alias_rotate_y, false) / (float)GetRoot()->GetRender()->GetDevice()->GetHeight());

				Math::Vector3 v;
				v.x = (2.0f * screepos.x - 1) / proj._11;
				v.y = -(2.0f * screepos.y - 1) / proj._22;
				v.z = 1.0f;

				Math::Matrix inv_view = view;
				inv_view.Inverse();

				Math::Vector3 camPos = inv_view.Pos();

				Math::Vector3 dir;
				dir.x = v.x * inv_view._11 + v.y * inv_view._21 + v.z * inv_view._31;
				dir.y = v.x * inv_view._12 + v.y * inv_view._22 + v.z * inv_view._32;
				dir.z = v.x * inv_view._13 + v.y * inv_view._23 + v.z * inv_view._33;
				dir.Normalize();

				rcdesc.origin = camPos;
				rcdesc.dir = dir;
				rcdesc.length = 500;
				rcdesc.group = 1;

				if (GetRoot()->GetPhysScene()->RayCast(rcdesc))
				{
					target_pt = rcdesc.hitPos;
					GetRoot()->GetRender()->DebugSphere(target_pt, COLOR_RED, 0.5f);

					dir = target_pt - mat.Pos();
					dir.y = 0.0f;
					dir.Normalize();

					inst.clientState.needed_tower_angel = atan2(dir.x, dir.z);

					//if (dir.Dot(Math::Vector3(0, 0, 1)) > 0.0f)
					{
//						inst.clientState.needed_tower_angel = Math::PI * 2 - inst.clientState.needed_tower_angel;
					}
				}

				inst.clientState.up = (int)GetRoot()->GetControls()->GetAliasValue(alias_forward, false);
				inst.clientState.rotate = -(int)GetRoot()->GetControls()->GetAliasValue(alias_strafe, false);

				inst.clientState.fired = GetRoot()->GetControls()->GetAliasState(alias_fire);
				inst.clientState.special_fired = GetRoot()->GetControls()->GetAliasState(alias_rotate_active);
			}

			float under = 1.0f;

			rcdesc.dir = Math::Vector3(0, -1, 0);
			rcdesc.length = under + 2.0f;

			Math::Vector3 org = mat.Pos();
			org.y += under;

			Math::Vector3 scr_pos = GetRoot()->GetRender()->TransformToScreen(mat.Pos(), 2);
			Math::Vector2 bar_size(60.0f, 5.0f);

			GetRoot()->GetRender()->DebugSprite(nullptr, Math::Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 60.0f), bar_size, COLOR_RED);
			GetRoot()->GetRender()->DebugSprite(nullptr, Math::Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 60.0f), Math::Vector2(bar_size.x * inst.serverState.hp * 0.01f, bar_size.y), COLOR_GREEN);

			GetRoot()->GetRender()->DebugSprite(nullptr, Math::Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 50.0f), bar_size, COLOR_RED);
			GetRoot()->GetRender()->DebugSprite(nullptr, Math::Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 50.0f), Math::Vector2(bar_size.x * inst.serverState.ammo * 0.01f, bar_size.y), COLOR_CYAN);

			GetRoot()->GetRender()->DebugSprite(nullptr, Math::Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 40.0f), bar_size, COLOR_RED);
			GetRoot()->GetRender()->DebugSprite(nullptr, Math::Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 40.0f), Math::Vector2(bar_size.x * inst.serverState.special * 0.01f, bar_size.y), COLOR_YELLOW);

			GetRoot()->GetRender()->DebugSprite(nullptr, Math::Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 30.0f), bar_size, COLOR_RED);
			GetRoot()->GetRender()->DebugSprite(nullptr, Math::Vector2(scr_pos.x - bar_size.x * 0.5f, scr_pos.y - 30.0f), Math::Vector2(bar_size.x * (1.0f - inst.serverState.shoot_cooldown / 1.5f), bar_size.y), COLOR_BLUE);

			Math::Vector3 p1 = org + mat.Vz() * 1.75f;
			rcdesc.origin = p1;
			rcdesc.group = 1;

			if (GetRoot()->GetPhysScene()->RayCast(rcdesc))
			{
				p1 = rcdesc.hitPos;
			}
			else
			{
				p1.y -= under;
			}

			Math::Vector3 p2 = org - mat.Vz() * 1.75f - mat.Vx();
			rcdesc.origin = p2;

			if (GetRoot()->GetPhysScene()->RayCast(rcdesc))
			{
				p2 = rcdesc.hitPos;
			}
			else
			{
				p2.y -= under;
			}

			Math::Vector3 p3 = org - mat.Vz() * 1.75f + mat.Vx();
			rcdesc.origin = p3;

			if (GetRoot()->GetPhysScene()->RayCast(rcdesc))
			{
				p3 = rcdesc.hitPos;
			}
			else
			{
				p3.y -= under;
			}

			mat.Vx() = p3 - p2;
			mat.Vx().Normalize();

			mat.Vz() = p1 - (p3 + p2)*0.5f;
			mat.Vz().Normalize();

			mat.Vy() = mat.Vz().Cross(mat.Vx());

			GetRoot()->GetRender()->DebugSphere(p1, COLOR_RED, 0.5f);
			GetRoot()->GetRender()->DebugSphere(p2, COLOR_RED, 0.5f);
			GetRoot()->GetRender()->DebugSphere(p3, COLOR_RED, 0.5f);

			Math::Matrix mdl = mat;
			inst.base_drawer->transform = mdl;

			Math::Matrix local;
			inst.base_drawer->GetLocatorTransform("loc_tower", local);

			Math::Matrix rot;
			rot.RotateY(-inst.serverState.tower_angel - inst.serverState.angle);
			mdl = rot * local;
			inst.tower_drawer->transform = mdl;

			Math::Vector3 tower = mdl.Pos();

			inst.tower_drawer->GetLocatorTransform("loc_left", mdl);

			if (inst.is_contralable)
			{
				inst.clientState.gun_pos = mdl.Pos();

				Math::Vector3 dr = target_pt - tower;
				float hgt = dr.y;
				dr.y = 0;
				float len = dr.Length();

				dr = mdl.Vz();
				dr.y = 0;
				dr.Normalize();
				dr *= len;
				dr.y = hgt;

				Math::Vector3 trg = tower + dr;

				inst.clientState.gun_dir = trg - mdl.Pos();
				inst.clientState.gun_dir.Normalize();

				rcdesc.origin = inst.clientState.gun_pos;
				rcdesc.dir = inst.clientState.gun_dir;
				rcdesc.length = 500;

				if (GetRoot()->GetPhysScene()->RayCast(rcdesc))
				{
					trg = rcdesc.hitPos;
				}

				GetRoot()->GetRender()->DebugSphere(trg, COLOR_CYAN, 0.5f);
			}
		}
	}
}
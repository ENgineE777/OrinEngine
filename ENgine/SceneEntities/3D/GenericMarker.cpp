
#include "GenericMarker.h"

#ifdef OAK_EDITOR
#include "Editor/Editor.h"
#endif

namespace Oak
{
	META_DATA_DESC(GenericMarker::Instance)
		FLOAT_PROP(GenericMarker::Instance, radius, 1.0f, "Prop", "radius", "radius of a marker")
		COLOR_PROP(GenericMarker::Instance, color, COLOR_WHITE, "Prop", "color")
	META_DATA_DESC_END()

	CLASSREG(SceneEntity, GenericMarker, "GenericMarker")

	META_DATA_DESC(GenericMarker)
		BASE_SCENE_ENTITY_PROP(GenericMarker)
		STRING_PROP(GenericMarker, scene_group, "", "Prop", "scene_group")
		BOOL_PROP(GenericMarker, full_shade, true, "Prop", "full_shade", "full_shade")
		BOOL_PROP(GenericMarker, is_path, false, "Prop", "is_path", "is_path")
		//ARRAY_PROP_INST_CALLGIZMO(GenericMarker, instances, Instance, "Prop", "inst", GenericMarker, sel_inst, SetGizmo)
	META_DATA_DESC_END()

	void GenericMarker::Init()
	{
		Tasks(false)->AddTask(100, this, (Object::Delegate)&GenericMarker::Draw);
	}

	void GenericMarker::ApplyProperties()
	{
		GetScene()->DelFromAllGroups(this);

		if (scene_group.c_str()[0] != 0)
		{
			GetScene()->AddToGroup(this, scene_group.c_str());
		}
	}

	/*void GenericMarker::Load(JSONReader& reader)
	{
		SceneObject::Load(reader);

		for (auto& inst : instances)
		{
			reader.EnterBlock("inst_trans");

			reader.Read("trans", &inst.transform);

			reader.LeaveBlock();
		}
	}

	void GenericMarker::Save(JSONWriter& writer)
	{
		SceneObject::Save(writer);

		writer.StartArray("inst_trans");

		for (auto& inst : instances)
		{
			writer.StartBlock(nullptr);

			writer.Write("trans", &inst.transform);

			writer.FinishBlock();
		}

		writer.FinishArray();
	}*/

	void GenericMarker::Draw(float dt)
	{
		if (GetScene()->Playing() || !IsVisible())
		{
			return;
		}

	#ifdef OAK_EDITOR
		if (edited)
		{
			if (root.controls.DebugKeyPressed("KEY_I") && sel_inst != -1)
			{
				instances.erase(sel_inst + instances.begin());
				sel_inst = -1;
				SetGizmo();
			}

			bool add_center = root.controls.DebugKeyPressed("KEY_P");
			bool add_copy = root.controls.DebugKeyPressed("KEY_O");

			if (add_center || (add_copy && sel_inst != -1))
			{
				Instance inst;

				if (add_copy)
				{
					inst.color = instances[sel_inst].color;
					inst.transform = instances[sel_inst].transform;
					inst.transform.Pos().x += 1.0f;
				}
				else
				{
					//inst.transform.Pos() += editor.freecamera.pos + Vector3(cosf(editor.freecamera.angles.x), sinf(editor.freecamera.angles.y), sinf(editor.freecamera.angles.x)) * 5.0f;
				}

				instances.push_back(inst);

				sel_inst = (int)instances.size() - 1;

				//SetGizmo();
			}
		}
	#endif

		int index = 0;

		for (auto& inst : instances)
		{
			root.render.DebugSphere(inst.transform.Pos(), inst.color, inst.radius, full_shade);

			if (index != 0 && is_path)
			{
				auto prev_inst = instances[index - 1];
				root.render.DebugLine(inst.transform.Pos(), inst.color, prev_inst.transform.Pos(), prev_inst.color);
			}

			index++;
		}
	}

	#ifdef OAK_EDITOR
	/*bool GenericMarker::CheckSelection(Vector2 ms, Vector3 start, Vector3 dir)
	{
		sel_inst = -1;

		for (int i = 0; i < instances.size(); i++)
		{
			auto instance = instances[i];

			if (Math::IntersectSphereRay(instance.transform.Pos(), instance.radius, start, dir))
			{
				sel_inst = i;

				if (IsEditMode())
				{
					SetGizmo();
				}

				return true;
			}
		}

		return false;
	}*/

	void GenericMarker::SetEditMode(bool ed)
	{
		SceneEntity::SetEditMode(ed);

		if (!ed)
		{
			sel_inst = -1;
		}

		SetGizmo();
	}

	void GenericMarker::SetGizmo()
	{
		if (sel_inst != -1)
		{
			//Gizmo::inst->SetTrans3D(&instances[sel_inst].transform);
		}
		else
		{
			//Gizmo::inst->Disable();
		}
	}
#endif
}
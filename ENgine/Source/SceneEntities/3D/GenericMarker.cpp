
#include "GenericMarker.h"

#ifdef OAK_EDITOR
#include "Editor/Editor.h"
#endif

namespace Oak
{
	META_DATA_DESC(GenericMarker::Instance)
		FLOAT_PROP(GenericMarker::Instance, radius, 1.0f, "Prop", "radius", "radius of a marker")
		COLOR_PROP(GenericMarker::Instance, color, COLOR_WHITE, "Prop", "color")
		TRANSFORM_PROP(GenericMarker::Instance, transform, "Transform")
	META_DATA_DESC_END()

	CLASSREG(SceneEntity, GenericMarker, "GenericMarker")

	META_DATA_DESC(GenericMarker)
		BASE_SCENE_ENTITY_PROP(GenericMarker)
		STRING_PROP(GenericMarker, sceneGroup, "", "Prop", "scene_group")
		BOOL_PROP(GenericMarker, fullShade, true, "Prop", "full_shade", "use full shade")
		BOOL_PROP(GenericMarker, isPath, false, "Prop", "is path", "is path")
		ARRAY_PROP_INST_CALLGIZMO(GenericMarker, instances, Instance, "Prop", "inst", GenericMarker, selInst, SetGizmo)
	META_DATA_DESC_END()

	Math::Vector3 GenericMarker::Instance::GetPosition()
	{
		if (transform.parent)
		{
			return transform.parent->GetGlobal().MulVertex(transform.position);
		}

		return transform.position;
	}

	void GenericMarker::Init()
	{
		if (is2D)
		{
			transform.unitsScale = &Sprite::pixelsPerUnit;
			transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		}

		transform.transformFlag = MoveXYZ | RotateXYZ;

		Tasks(false)->AddTask(100, this, (Object::Delegate)&GenericMarker::Draw);
	}

	void GenericMarker::ApplyProperties()
	{
		GetScene()->DelFromAllGroups(this);

		if (sceneGroup.c_str()[0] != 0)
		{
			GetScene()->AddToGroup(this, sceneGroup.c_str());
		}
	}

	void GenericMarker::Draw(float dt)
	{
		if (GetScene()->IsPlaying() || !IsVisible())
		{
			return;
		}

		UpdateTransforms();

	#ifdef OAK_EDITOR
		if (edited)
		{
			if (root.controls.DebugKeyPressed("KEY_I") && selInst != -1)
			{
				instances.erase(selInst + instances.begin());
				selInst = -1;
				SetGizmo();
			}

			bool add_center = root.controls.DebugKeyPressed("KEY_P");
			bool add_copy = root.controls.DebugKeyPressed("KEY_O");

			if (add_center || (add_copy && selInst != -1))
			{
				Instance inst;

				if (is2D)
				{
					inst.transform.unitsScale = &Sprite::pixelsPerUnit;
					inst.transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
				}

				if (add_copy)
				{
					inst.color = instances[selInst].color;
					inst.transform = instances[selInst].transform;
					inst.transform.position = Math::Vector3(inst.transform.position.x + 1.0f, inst.transform.position.y, inst.transform.position.z);
				}
				else
				{
					if (is2D)
					{
						inst.transform.position = editor.freeCamera.pos2D;
					}
					else
					{
						inst.transform.position = editor.freeCamera.pos + Math::Vector3(cosf(editor.freeCamera.angles.x), sinf(editor.freeCamera.angles.y), sinf(editor.freeCamera.angles.x)) * 5.0f;
					}

					Math::Matrix invMat = transform.GetGlobal();
					invMat.Inverse();

					inst.transform.position = invMat.MulVertex(inst.transform.position);
				}

				instances.push_back(inst);

				selInst = (int)instances.size() - 1;

				SetGizmo();
			}
		}
	#endif

		int index = 0;

		for (auto& inst : instances)
		{
			inst.transform.parent = &transform;

			if (is2D)
			{
				inst.transform.unitsScale = &Sprite::pixelsPerUnit;
				inst.transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
			}

			Math::Vector3 pos = inst.GetPosition();
			
			if (is2D)
			{
				pos *= Sprite::pixelsPerUnitInvert;
			}

			root.render.DebugSphere(pos, inst.color, inst.radius, fullShade);

			if (index != 0 && isPath)
			{
				auto prevInst = instances[index - 1];
				Math::Vector3 prevPos = prevInst.GetPosition();

				if (is2D)
				{
					prevPos *= Sprite::pixelsPerUnitInvert;
				}

				root.render.DebugLine(pos, inst.color, prevPos, prevInst.color);
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

	void GenericMarker::Play()
	{
		SceneEntity::Play();

		UpdateTransforms();
	}

	void GenericMarker::UpdateTransforms()
	{
		for (auto& inst : instances)
		{
			inst.transform.parent = &transform;

			if (is2D)
			{
				inst.transform.unitsScale = &Sprite::pixelsPerUnit;
				inst.transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
			}
		}
	}

	void GenericMarker::SetEditMode(bool ed)
	{
		SceneEntity::SetEditMode(ed);

		if (!ed)
		{
			SetGizmo();
		}
	}

	void GenericMarker::SetGizmo()
	{
		if (selInst != -1)
		{
			instances[selInst].transform.transformFlag = MoveXYZ;
			editor.gizmo.SetTransform(&instances[selInst].transform);
		}
		else
		{
			editor.gizmo.SetTransform(&transform);
		}
	}
#endif
}
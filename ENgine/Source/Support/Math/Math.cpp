#include <stdlib.h>
#include "Root/Root.h"
#include "Support/Support.h"

namespace Orin::Math
{
	float Rand()
	{
		return ((float)rand() / RAND_MAX);
	}

	float RandRange(float a, float b)
	{
		return (a + (b - a) * (float)rand() / RAND_MAX);
	}

	float Clamp(float value, float a, float b)
	{
		return fminf(fmaxf(value, a), b);
	}

	float Sign(float value)
	{
		return value > 0.000001f ? 1.0f : -1.0f;
	}

	float AdvanceAngle(float angle, float target_angle, float delta_angle)
	{
		float diff = GetAnglesDifference(angle, target_angle);

		if (fabs(diff) < fabs(delta_angle))
		{
			angle = target_angle;
		}
		else
		{
			angle += delta_angle * Sign(diff);
		}

		if (angle > PI)
		{
			angle -= TwoPI;
		}
		else
		if (angle < -PI)
		{
			angle += TwoPI;
		}

		return angle;
	}

	float GetAnglesDifference(float angle, float target_angle)
	{
		while (angle > PI)
		{
			angle = angle - TwoPI;
		}

		while (angle < -PI)
		{
			angle = angle + TwoPI;
		}

		while (target_angle > PI)
		{
			target_angle = target_angle - TwoPI;
		}

		while (target_angle < -PI)
		{
			target_angle = target_angle + TwoPI;
		}

		if (target_angle - angle > PI)
		{
			target_angle -= TwoPI;
		}
		else
		if (angle - target_angle > PI)
		{
			target_angle += TwoPI;
		}

		return target_angle - angle;
	}

	bool IsSameAngles(float angle, float target_angle)
	{
		return fabs(GetAnglesDifference(angle, target_angle)) < 0.001f;
	}

	bool IsPointInPolygon(Vector2 pt, eastl::vector<Vector2>& polygon, bool debugDraw)
	{
		if (debugDraw)
		{
			for (int i = 0; i < polygon.size(); i++)
			{
				int nextIndex = (i + 1) % polygon.size();
				Sprite::DebugLine(polygon[i], polygon[nextIndex], COLOR_GREEN);
			}
		}

		eastl::vector<Vector2> temp;

		int count = (int)polygon.size();
		temp.resize(count);

		for (int i = 0; i < count; i++)
		{
			temp[i] = polygon[i] - pt;
		}

		bool orientation = temp[0].Cross(temp[1]) > 0;

		for (int i = 1; i < count; i++)
		{
			if ((temp[i].Cross(temp[(i + 1) % count]) > 0) != orientation)
			{
				return false;
			}
		}

		return true;
	}

	bool IsPointInLineSegment(Vector3 pt, Vector3 p1, Vector3 p2)
	{
		return AreApproximatelyEqual((p1 - p2).Length(), (p1 - pt).Length() + (p2 - pt).Length());
	}

	bool IsPointInTriangle(Vector2 pt, Vector2 p1, Vector2 p2, Vector2 p3, bool debugDraw)
	{
		eastl::vector<Vector2> polygon;
		polygon.push_back(p1);
		polygon.push_back(p2);
		polygon.push_back(p3);

		return IsPointInPolygon(pt, polygon, debugDraw);
	}

	bool IsPointInRectangle(Vector2 pt, Vector2 center, Vector2 offset, Vector2 size, float angle, bool debugDraw)
	{
		eastl::vector<Vector2> polygon = { { 0.0f, size.y * 0.5f },{ size.x, size.y * 0.5f }, { size.x, -size.y * 0.5f }, { 0.0f, -size.y * 0.5f } };

		float cs = cosf(angle);
		float sn = sinf(angle);

		Vector2 local_offset = offset * size * 0.5f;

		for (int i = 0; i < 4; i++)
		{
			polygon[i] = polygon[i] - local_offset;
			polygon[i] = Vector2{ polygon[i].x * cs - polygon[i].y * sn, polygon[i].x * sn + polygon[i].y * cs } + center;
		}

		return IsPointInPolygon(pt, polygon, debugDraw);
	}

	bool IsPointInSector(Vector2 pt, Vector2 center, float orientation, float distance, float angle, bool debugDraw)
	{
		float side_angle = orientation - angle * 0.5f;
		Vector2 p2(cosf(side_angle) * distance + center.x, sinf(side_angle) * distance + center.y);

		side_angle = orientation + angle * 0.5f;
		Vector2 p3(cosf(side_angle) * distance + center.x, sinf(side_angle) * distance + center.y);

		return IsPointInTriangle(pt, center, p2, p3, debugDraw);
	}

	bool IntersectSphereRay(Vector3 pos, float radius, Vector3 start, Vector3 dir)
	{
		Vector3 L = pos - start;
		float tca = L.Dot(dir);
		float d2 = L.Dot(L) - tca * tca;

		float radius2 = radius * radius;

		if (d2 > radius2)
		{
			return false;
		}

		float thc = sqrtf(radius2 - d2);
		float t0 = tca - thc;
		float t1 = tca + thc;

		if (t0 > t1)
		{
			float tmp = t0;
			t0 = t1;
			t1 = tmp;
		}

		if (t0 < 0)
		{
			t0 = t1;

			if (t0 < 0)
			{
				return false;
			}
		}

		return true;
	}

	bool IntersectBBoxRay(Vector3 vmin, Vector3 vmax, Vector3 start, Vector3 dir)
	{
		Vector3 invdir = 1 / dir;

		int sign[3];
		sign[0] = (invdir.x < 0);
		sign[1] = (invdir.y < 0);
		sign[2] = (invdir.z < 0);

		Vector3 bounds[2];
		bounds[0] = vmin;
		bounds[1] = vmax;

		float tmin, tmax, tymin, tymax, tzmin, tzmax;

		tmin = (bounds[sign[0]].x - start.x) * invdir.x;
		tmax = (bounds[1 - sign[0]].x - start.x) * invdir.x;
		tymin = (bounds[sign[1]].y - start.y) * invdir.y;
		tymax = (bounds[1 - sign[1]].y - start.y) * invdir.y;

		if ((tmin > tymax) || (tymin > tmax))
		{
			return false;
		}

		if (tymin > tmin)
		{
			tmin = tymin;
		}

		if (tymax < tmax)
		{
			tmax = tymax;
		}

		tzmin = (bounds[sign[2]].z - start.z) * invdir.z;
		tzmax = (bounds[1 - sign[2]].z - start.z) * invdir.z;

		if ((tmin > tzmax) || (tzmin > tmax))
		{
			return false;
		}

		if (tzmin > tmin)
		{
			tmin = tzmin;
		}

		if (tzmax < tmax)
		{
			tmax = tzmax;
		}

		return true;
	}

	bool IntersectTrianglrRay(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 orig, Vector3 dir, float distance)
	{
		Vector3 e1, e2, pvec, qvec, tvec;

		e1 = v2 - v1;
		e2 = v3 - v1;

		pvec = dir.Cross(e2);

		float det = pvec.Dot(e1);

		if (det < Epsilon && det > -Epsilon)
		{
			return false;
		}

		float invDet = 1.0f / det;
		tvec = orig - v1;

		float u = invDet * tvec.Dot(pvec);
		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		qvec = tvec.Cross(e1);

		float v = invDet * qvec.Dot(dir);
		if (v < 0.0f || u + v > 1.0f)
		{
			return false;
		}

		return true;
	}

	bool IntersectPlaneRay(Vector3 planeP, Vector3 planeN, Vector3 rayP, Vector3 rayD, Vector3& intersection)
	{
		float d = planeP.Dot(-planeN);
		float det = (rayD.z * planeN.z + rayD.y * planeN.y + rayD.x * planeN.x);

		if (fabsf(det) < Epsilon)
		{
			intersection = 0.0f;
			return false;
		}

		float t = -(d + rayP.z * planeN.z + rayP.y * planeN.y + rayP.x * planeN.x) / det;
		intersection = rayP + t * rayD;

		return true;
	}

	void GetMouseRay(Math::Vector2 ms, Math::Vector3& mouseOrigin, Math::Vector3& mouseDirection)
	{
		Math::Vector2 msConv = ms;
		msConv.x /= (float)root.render.GetDevice()->GetWidth();
		msConv.y /= (float)root.render.GetDevice()->GetHeight();

		Math::Matrix view;
		root.render.GetTransform(TransformStage::View, view);

		Math::Matrix view_proj;
		root.render.GetTransform(TransformStage::Projection, view_proj);

		Math::Vector3 v;
		v.x = (2.0f * msConv.x - 1) / view_proj._11;
		v.y = -(2.0f * msConv.y - 1) / view_proj._22;
		v.z = 1.0f;

		Math::Matrix inv_view = view;
		inv_view.Inverse();
		mouseOrigin = inv_view.Pos();

		mouseDirection.x = v.x * inv_view._11 + v.y * inv_view._21 + v.z * inv_view._31;
		mouseDirection.y = v.x * inv_view._12 + v.y * inv_view._22 + v.z * inv_view._32;
		mouseDirection.z = v.x * inv_view._13 + v.y * inv_view._23 + v.z * inv_view._33;
		mouseDirection.Normalize();
	}
}
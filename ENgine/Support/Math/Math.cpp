#include <stdlib.h>
#include "Root/Root.h"
#include "Support/Support.h"

namespace Oak::Math
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

	bool IsPointInPolygon(Math::Vector2 pt, eastl::vector<Math::Vector2>& polygon)
	{
		eastl::vector<Math::Vector2> temp;

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

	bool IsPointInRectangle(Math::Vector2 pt, Math::Vector2 center, Math::Vector2 offset, Math::Vector2 size, float angle, bool debugDraw)
	{
		eastl::vector<Math::Vector2> polygon = { { 0.0f, size.y * 0.5f },{ size.x, size.y * 0.5f }, { size.x, -size.y * 0.5f }, { 0.0f, -size.y * 0.5f } };

		float cs = cosf(angle);
		float sn = sinf(angle);

		Vector2 local_offset = offset * size * 0.5f;

		for (int i = 0; i < 4; i++)
		{
			polygon[i] = polygon[i] - local_offset;
			polygon[i] = Vector2{ polygon[i].x * cs - polygon[i].y * sn, polygon[i].x * sn + polygon[i].y * cs } + center;
		}

		if (debugDraw)
		{
			for (int i = 0; i < polygon.size(); i++)
			{
				int nextIndex = (i + 1) % polygon.size();
				root.render.DebugLine(Math::Vector3(polygon[i].x, polygon[i].y, 0.0f) * Sprite::pixelsPerUnitInvert, COLOR_GREEN,
										Math::Vector3(polygon[nextIndex].x, polygon[nextIndex].y, 0.0f) * Sprite::pixelsPerUnitInvert, COLOR_GREEN, false);
			}
		}

		return Math::IsPointInPolygon(pt, polygon);
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

	bool IntersectPlaneRay(Math::Vector3 planeP, Math::Vector3 planeN, Math::Vector3 rayP, Math::Vector3 rayD, Math::Vector3& intersection)
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

	bool IsInsideTriangle(Math::Vector2 s, Math::Vector2 a, Math::Vector2 b, Math::Vector2 c)
	{
		float as_x = s.x - a.x;
		float as_y = s.y - a.y;

		bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0;

		if ((c.x - a.x) * as_y - (c.y - a.y) * as_x > 0 == s_ab) return false;
		if ((c.x - b.x) * (s.y - b.y) - (c.y - b.y) * (s.x - b.x) > 0 != s_ab) return false;

		return true;
	}
}
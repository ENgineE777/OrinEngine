#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

/**
\ingroup gr_code_common_math
*/

namespace Oak::Math
{
	constexpr static float Radian = 0.01745329f;
	constexpr static float PI = 3.14159265f;
	constexpr static float TwoPI = 6.28318530f;
	constexpr static float HalfPI = 1.57079632f;
	constexpr static float Epsilon = 0.000001f;

	CLASS_DECLSPEC float Rand();
	CLASS_DECLSPEC float RandRange(float a, float b);
	CLASS_DECLSPEC float Clamp(float value, float a, float b);
	CLASS_DECLSPEC float Sign(float value);
	CLASS_DECLSPEC float AdvanceAngle(float angle, float target_angle, float delta_angle);
	CLASS_DECLSPEC float GetAnglesDifference(float angle, float target_angle);
	CLASS_DECLSPEC bool IsSameAngles(float angle, float target_angle);
	CLASS_DECLSPEC bool IsPointInTriangle(Vector2 pt, Vector2 p1, Vector2 p2, Vector2 p3, bool debugDraw = false);
	CLASS_DECLSPEC bool IsPointInSector(Vector2 pt, Vector2 center, float orientation, float distance, float angle, bool debugDraw = false);
	CLASS_DECLSPEC bool IsPointInPolygon(Vector2 pt, eastl::vector<Vector2>& polygon, bool debugDraw = false);
	CLASS_DECLSPEC bool IsPointInRectangle(Vector2 pt, Vector2 center, Vector2 offset, Vector2 size, float angle, bool debugDraw = false);
	CLASS_DECLSPEC bool IntersectSphereRay(Vector3 pos, float radius, Vector3 start, Vector3 dir);
	CLASS_DECLSPEC bool IntersectBBoxRay(Vector3 vmin, Vector3 vmax, Vector3 start, Vector3 dir);
	CLASS_DECLSPEC bool IntersectTrianglrRay(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 orig, Vector3 dir, float distance);
	CLASS_DECLSPEC bool IntersectPlaneRay(Vector3 planeP, Vector3 planeN, Vector3 rayP, Vector3 rayD, Vector3& intersection);
}
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

	float Rand();
	float RandRange(float a, float b);
	float Clamp(float value, float a, float b);
	float Sign(float value);
	float AdvanceAngle(float angle, float target_angle, float delta_angle);
	float GetAnglesDifference(float angle, float target_angle);
	bool IsSameAngles(float angle, float target_angle);
	bool IntersectSphereRay(Vector3 pos, float radius, Vector3 start, Vector3 dir);
	bool IntersectBBoxRay(Vector3 vmin, Vector3 vmax, Vector3 start, Vector3 dir);
	bool IntersectTrianglrRay(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 orig, Vector3 dir, float distance);
	bool IntersectPlaneRay(Math::Vector3 planeP, Math::Vector3 planeN, Math::Vector3 rayP, Math::Vector3 rayD, Math::Vector3& intersection);
	bool IsInsideTriangle(Math::Vector2 s, Math::Vector2 a, Math::Vector2 b, Math::Vector2 c);
}
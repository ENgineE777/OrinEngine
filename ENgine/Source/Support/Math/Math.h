#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include <eastl/vector.h>

/**
\ingroup gr_code_common_math
*/

namespace Orin::Math
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
	CLASS_DECLSPEC bool IsRectsOverlap(Vector2 rect1_left, Vector2 rect1_right, Vector2 rect2_left, Vector2 rect2_right);
	CLASS_DECLSPEC bool IsSameAngles(float angle, float target_angle);
	CLASS_DECLSPEC bool IsPointInLineSegment(Vector3 pt, Vector3 p1, Vector3 p2);
	CLASS_DECLSPEC bool IsPointInTriangle(Vector2 pt, Vector2 p1, Vector2 p2, Vector2 p3, bool debugDraw = false);	
	CLASS_DECLSPEC bool IsPointInSector(Vector2 pt, Vector2 center, float orientation, float distance, float angle, bool debugDraw = false);
	CLASS_DECLSPEC bool IsPointInPolygon(Vector2 pt, eastl::vector<Vector2>& polygon, bool debugDraw = false);
	CLASS_DECLSPEC bool IsPointInRectangle(Vector2 pt, Vector2 center, Vector2 offset, Vector2 size, float angle, bool debugDraw = false);
	CLASS_DECLSPEC bool IntersectSegmentSegemnt(Vector2 a1, Vector2 a2, Vector2 b1, Vector2 b2);
	CLASS_DECLSPEC bool IntersectSegmentTriangle(Vector2 a1, Vector2 a2, Vector2 b1, Vector2 b2, Vector2 b3);
	CLASS_DECLSPEC bool IntersectTriangleTriangle(Vector2 a1, Vector2 a2, Vector2 a3, Vector2 b1, Vector2 b2, Vector2 b3);
	CLASS_DECLSPEC bool IntersectSphereRay(Vector3 pos, float radius, Vector3 start, Vector3 dir);
	CLASS_DECLSPEC bool IntersectBBoxRay(Vector3 vmin, Vector3 vmax, Vector3 start, Vector3 dir);	
	CLASS_DECLSPEC bool IntersectTrianglrRay(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 orig, Vector3 dir, float distance);
	CLASS_DECLSPEC bool IntersectPlaneRay(Vector3 planeP, Vector3 planeN, Vector3 rayP, Vector3 rayD, Vector3& intersection);
	CLASS_DECLSPEC void GetMouseRay(Math::Vector2 ms, Math::Vector3& mouseOrigin, Math::Vector3& mouseDirection);

	//https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
	CLASS_DECLSPEC inline bool IsRelativeEqual(float a, float b, float max_diff = 1e-5f, float max_rel_diff = FLT_EPSILON)
	{
		const float diff = fabsf(a - b);
		if (diff < max_diff)
			return true;
		a = fabsf(a);
		b = fabsf(b);
		return diff < (a < b ? b : a)* max_rel_diff;
	}

	CLASS_DECLSPEC inline bool IsEqual(float a, float b, float eps = 1e-5f)
	{
		return IsRelativeEqual(a, b, eps, FLT_EPSILON);
	}

	CLASS_DECLSPEC inline bool AreApproximatelyEqual(float a, float b, float epsilon = 8.f * FLT_EPSILON)
	{
		return IsRelativeEqual(a, b, epsilon, epsilon);
	}

	CLASS_DECLSPEC inline bool IsNonZero(float a, float eps = 1e-5f)
	{
		return fabsf(a) >= eps;
	}

	CLASS_DECLSPEC inline bool ValueInRange(float value, float min, float max)
	{
		return (value >= min) && (value <= max);
	}

	CLASS_DECLSPEC inline float SafeAtan2(float y, float x)
	{
		return (x != 0.f || y != 0.f) ? ::atan2f(y, x) : 0.f;
	}

	CLASS_DECLSPEC inline float Atan2(float y, float x)
	{
		return ::atan2f(y, x);
	}

	template<typename T>
    CLASS_DECLSPEC inline T Approach(T from, T to, float dt, float viscosity)
    {
        if (viscosity < 1e-5f)
            return to;
        return from + (1.f - ::expf(-dt / viscosity)) * (to - from);
    }

	CLASS_DECLSPEC inline float EaseInQuad(float k)
	{
		return k * k;
	}

	CLASS_DECLSPEC inline float EaseInCubic(float k)
	{
		return k * k * k;
	}

	CLASS_DECLSPEC inline float EaseInQuart(float k)
	{
		return k * k * k * k;
	}

	CLASS_DECLSPEC inline float EaseOutQuad(float k)
	{
		return 1 - (1 - k) * (1 - k);
	}

	CLASS_DECLSPEC inline float EaseOutCubic(float k)
	{
		return 1 - (1 - k) * (1 - k) * (1 - k);
	}

	CLASS_DECLSPEC inline float EaseOutQuart(float k)
	{
		return 1 - (1 - k) * (1 - k) * (1 - k) * (1 - k);
	}

	CLASS_DECLSPEC inline float EaseInBack(float k)
	{
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.f;

		return c3 * k * k * k - c1 * k * k;
	}

	CLASS_DECLSPEC inline float EaseOutBack(float t)
	{
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.f;
		const float k = (t - 1.f);

		return 1.f + c3 * k * k * k + c1 * k * k;
	}
}
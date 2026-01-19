#include "Engine_Defines.h"
#include "Engine_Math.h"

ENGINE_DLL _float Math::Lerp(_float x, _float y, _float t)
{
	return x + (y - x) * t;
}

ENGINE_DLL _vector2 Math::Lerp(_vector2 x, _vector2 y, _float t)
{
	return x + (y - x) * t;
}

ENGINE_DLL _float Math::WrapDeg(_float deg)
{
	while (deg > 180.f)  deg -= 360.f;
	while (deg < -180.f) deg += 360.f;
	return deg;
}

ENGINE_DLL _float Math::QuatDot(const Quaternion& a, const Quaternion& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

ENGINE_DLL Quaternion Math::QuatNegate(const Quaternion& q)
{
	return Quaternion(-q.x, -q.y, -q.z, -q.w);
}

ENGINE_DLL _uint Math::LcgStep(_uint& s)
{
	s = s * 1664525u + 1013904223u;
	return s;
}
ENGINE_DLL _float Math::Rand01(_uint& s)
{
	const _uint x = LcgStep(s) & 0x00FFFFFFu;
	return (float)x / (float)0x01000000u;
}

ENGINE_DLL _float Math::SmoothStep01(float t)
{
	t = clamp(t, 0.f, 1.f);
	return t * t * (3.f - 2.f * t);
}

ENGINE_DLL _float Math::Clamp01(_float t)
{
	if (t < 0.f) return 0.f;
	if (t > 1.f) return 1.f;
	return t;
}

ENGINE_DLL Vector3 Math::SeedPhase(_uint& seed)
{
	seed = seed * 1664525u + 1013904223u;
	const _float a = (seed & 1023u) * (6.28318530718f / 1023.f);

	seed = seed * 1664525u + 1013904223u;
	const _float b = (seed & 1023u) * (6.28318530718f / 1023.f);

	seed = seed * 1664525u + 1013904223u;
	const _float c = (seed & 1023u) * (6.28318530718f / 1023.f);

	return {a, b, c};
}

ENGINE_DLL _float Math::ApplyEase(EaseType type, _float t)
{
	t = clamp(t, 0.f, 1.f);

	switch (type)
	{
	case EaseType::None:         return t;
	case EaseType::Linear:       return t;

	case EaseType::InOutSine:    return EaseInOutSine(t);
	case EaseType::OutCubic:     return EaseOutCubic(t);
	case EaseType::InOutCubic:   return EaseInOutCubic(t);
	case EaseType::OutSine:      return EaseOutSine(t);
	case EaseType::InOutQuad:    return EaseInOutQuad(t);

	case EaseType::InSine:       return EaseInSine(t);
	case EaseType::InCubic:      return EaseInCubic(t);
	case EaseType::InQuad:       return EaseInQuad(t);
	case EaseType::InCirc:       return EaseInCirc(t);

	case EaseType::InOutCirc:    return EaseInOutCirc(t);
	case EaseType::OutCirc:      return EaseOutCirc(t);
	case EaseType::OutQuad:      return EaseOutQuad(t);

	case EaseType::InQuart:      return EaseInQuart(t);
	case EaseType::InQuint:      return EaseInQuint(t);
	case EaseType::InOutQuart:   return EaseInOutQuart(t);
	case EaseType::OutQuart:     return EaseOutQuart(t);
	case EaseType::InOutQuint:   return EaseInOutQuint(t);
	case EaseType::OutQuint:     return EaseOutQuint(t);

	case EaseType::InOutExpo:    return EaseInOutExpo(t);
	case EaseType::OutExpo:      return EaseOutExpo(t);
	case EaseType::InExpo:       return EaseInExpo(t);

	case EaseType::OutBack:      return EaseOutBack(t);
	case EaseType::InOutBack:    return EaseInOutBack(t);
	case EaseType::InBack:       return EaseInBack(t);

	case EaseType::OutElastic:   return EaseOutElastic(t);
	case EaseType::InOutElastic: return EaseInOutElastic(t);
	case EaseType::InElastic:    return EaseInElastic(t);

	case EaseType::OutBounce:    return EaseOutBounce(t);
	case EaseType::InOutBounce:  return EaseInOutBounce(t);
	case EaseType::InBounce:     return EaseInBounce(t);

	default:                     return t;
	}
}

ENGINE_DLL _float Math::EaseInSine(_float t)
{
	return 1.f - cosf((t * XM_PI) * 0.5f);
}

ENGINE_DLL _float Math::EaseOutSine(_float t)
{
	return sinf((t * XM_PI) * 0.5f);
}

ENGINE_DLL _float Math::EaseInOutSine(_float t)
{
	return 0.5f * (1.f - cosf(XM_PI * t));
}

ENGINE_DLL _float Math::EaseInQuad(_float t)
{
	return t * t;
}

ENGINE_DLL _float Math::EaseOutQuad(_float t)
{
	return 1.f - (1.f - t) * (1.f - t);
}

ENGINE_DLL _float Math::EaseInOutQuad(_float t)
{
	return (t < 0.5f) ? (2.f * t * t) : (1.f - powf(-2.f * t + 2.f, 2.f) * 0.5f);
}

ENGINE_DLL _float Math::EaseInCubic(_float t)
{
	return t * t * t;
}

ENGINE_DLL _float Math::EaseOutCubic(_float t)
{
	const float u = 1.f - t;
	return 1.f - u * u * u;
}

ENGINE_DLL _float Math::EaseInOutCubic(_float t)
{
	return (t < 0.5f) ? (4.f * t * t * t) : (1.f - powf(-2.f * t + 2.f, 3.f) * 0.5f);
}

ENGINE_DLL _float Math::EaseInQuart(_float t)
{
	return t * t * t * t;
}

ENGINE_DLL _float Math::EaseOutQuart(_float t)
{
	return 1.f - powf(1.f - t, 4.f);
}

ENGINE_DLL _float Math::EaseInOutQuart(_float t)
{
	return (t < 0.5f) ? (8.f * powf(t, 4.f)) : (1.f - powf(-2.f * t + 2.f, 4.f) * 0.5f);
}

ENGINE_DLL _float Math::EaseInQuint(_float t)
{
	return powf(t, 5.f);
}

ENGINE_DLL _float Math::EaseOutQuint(_float t)
{
	return 1.f - powf(1.f - t, 5.f);
}

ENGINE_DLL _float Math::EaseInOutQuint(_float t)
{
	return (t < 0.5f) ? (16.f * powf(t, 5.f)) : (1.f - powf(-2.f * t + 2.f, 5.f) * 0.5f);
}

ENGINE_DLL _float Math::EaseInCirc(_float t)
{
	return 1.f - sqrtf(1.f - t * t);
}

ENGINE_DLL _float Math::EaseOutCirc(_float t)
{
	const float u = t - 1.f;
	return sqrtf(1.f - u * u);
}

ENGINE_DLL _float Math::EaseInOutCirc(_float t)
{
	if (t < 0.5f)
		return (1.f - sqrtf(1.f - powf(2.f * t, 2.f))) * 0.5f;

	return (sqrtf(1.f - powf(-2.f * t + 2.f, 2.f)) + 1.f) * 0.5f;
}

ENGINE_DLL _float Math::EaseInExpo(_float t)
{
	if (t <= 0.f) return 0.f;
	return powf(2.f, 10.f * t - 10.f);
}

ENGINE_DLL _float Math::EaseOutExpo(_float t)
{
	if (t >= 1.f) return 1.f;
	return 1.f - powf(2.f, -10.f * t);
}

ENGINE_DLL _float Math::EaseInOutExpo(_float t)
{
	if (t <= 0.f) return 0.f;
	if (t >= 1.f) return 1.f;
	if (t < 0.5f) return powf(2.f, 20.f * t - 10.f) * 0.5f;
	return (2.f - powf(2.f, -20.f * t + 10.f)) * 0.5f;
}

ENGINE_DLL _float Math::EaseInBack(_float t)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.f;
	return c3 * t * t * t - c1 * t * t;
}

ENGINE_DLL _float Math::EaseOutBack(_float t)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.f;
	const float u = t - 1.f;
	return 1.f + c3 * (u * u * u) + c1 * (u * u);
}

ENGINE_DLL _float Math::EaseInOutBack(_float t)
{
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	if (t < 0.5f)
	{
		const float u = 2.f * t;
		return (u * u * ((c2 + 1.f) * u - c2)) * 0.5f;
	}
	else
	{
		const float u = 2.f * t - 2.f;
		return (u * u * ((c2 + 1.f) * u + c2) + 2.f) * 0.5f;
	}
}

ENGINE_DLL _float Math::EaseInElastic(_float t)
{
	if (t <= 0.f) return 0.f;
	if (t >= 1.f) return 1.f;
	const float c4 = (2.f * XM_PI) / 3.f;
	return -powf(2.f, 10.f * t - 10.f) * sinf((t * 10.f - 10.75f) * c4);
}

ENGINE_DLL _float Math::EaseOutElastic(_float t)
{
	if (t <= 0.f) return 0.f;
	if (t >= 1.f) return 1.f;
	const float c4 = (2.f * XM_PI) / 3.f;
	return powf(2.f, -10.f * t) * sinf((t * 10.f - 0.75f) * c4) + 1.f;
}

ENGINE_DLL _float Math::EaseInOutElastic(_float t)
{
	if (t <= 0.f) return 0.f;
	if (t >= 1.f) return 1.f;

	const float c5 = (2.f * XM_PI) / 4.5f;

	if (t < 0.5f)
		return -0.5f * powf(2.f, 20.f * t - 10.f) * sinf((20.f * t - 11.125f) * c5);

	return 0.5f * powf(2.f, -20.f * t + 10.f) * sinf((20.f * t - 11.125f) * c5) + 1.f;
}

static inline float OutBounceLocal(float t)
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (t < 1.f / d1)
		return n1 * t * t;

	if (t < 2.f / d1)
	{
		t -= 1.5f / d1;
		return n1 * t * t + 0.75f;
	}

	if (t < 2.5f / d1)
	{
		t -= 2.25f / d1;
		return n1 * t * t + 0.9375f;
	}

	t -= 2.625f / d1;
	return n1 * t * t + 0.984375f;
}

ENGINE_DLL _float Math::EaseOutBounce(_float t)
{
	return OutBounceLocal(t);
}

ENGINE_DLL _float Math::EaseInBounce(_float t)
{
	return 1.f - OutBounceLocal(1.f - t);
}

ENGINE_DLL _float Math::EaseInOutBounce(_float t)
{
	if (t < 0.5f)
		return (1.f - OutBounceLocal(1.f - 2.f * t)) * 0.5f;

	return (1.f + OutBounceLocal(2.f * t - 1.f)) * 0.5f;
}

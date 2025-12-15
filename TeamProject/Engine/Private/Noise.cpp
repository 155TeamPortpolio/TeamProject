#include "Engine_Defines.h"
#include "Noise.h"
#include "Helper_Func.h"

CNoise::CNoise()
	:IParticleModule()
{
}

void CNoise::SetParams(PARTICLE_MODULE_DESC* pDesc)
{
	NOISE_DESC* pNoiseDesc = static_cast<NOISE_DESC*>(pDesc);

	m_vStrength = pNoiseDesc->vStrength;
	m_vFrequency = pNoiseDesc->vFrequency;
	m_vScrollSpeed = pNoiseDesc->vScrollSpeed;
}

void CNoise::Update(CParticleSystem::PARTICLE& particle, _float dt)
{
	m_fElapsedTime += dt;

	_vector3 vSample = particle.vPosition * m_vFrequency * particle.fNoiseFrequency + m_fElapsedTime * m_vScrollSpeed;
	_vector3 noise{};
	noise.x = MakeNoise(vSample + _vector3(17.1f, 0.f, 0.f));
	noise.y = MakeNoise(vSample + _vector3(0.f, 31.7f, 0.f));
	noise.z = MakeNoise(vSample + _vector3(0.f, 0.f, 47.3f));

	noise *= m_vStrength;

	particle.vVelocity = particle.vVelocity + noise * dt;
}

CNoise* CNoise::Create()
{
	CNoise* instance = new CNoise();

	return instance;
}

void CNoise::Free()
{
	__super::Free();
}

_float CNoise::MakeNoise(_float3 p)
{
	_int x0{}, y0{}, z0{};
	_int x1{}, y1{}, z1{};

	x0 = Floor(p.x);
	y0 = Floor(p.y);
	z0 = Floor(p.z);

	x1 = x0 + 1;
	y1 = y0 + 1;
	z1 = z0 + 1;

	_float fx = p.x - static_cast<_float>(x0);
	_float fy = p.y - static_cast<_float>(y0);
	_float fz = p.z - static_cast<_float>(z0);

	_float u = Fade(fx);
	_float v = Fade(fy);
	_float w = Fade(fz);

	_float c000 = NormalizeHash(x0, y0, z0);
	_float c100 = NormalizeHash(x1, y0, z0);
	_float c010 = NormalizeHash(x0, y1, z0);
	_float c110 = NormalizeHash(x1, y1, z0);
	
	_float c001 = NormalizeHash(x0, y0, z1);
	_float c101 = NormalizeHash(x1, y0, z1);
	_float c011 = NormalizeHash(x0, y1, z1);
	_float c111 = NormalizeHash(x1, y1, z1);
	
	_float x00 = Math::Lerp(c000, c100, u);
	_float x10 = Math::Lerp(c010, c110, u);
	_float x01 = Math::Lerp(c001, c101, u);
	_float x11 = Math::Lerp(c011, c111, u);
	
	_float y0v = Math::Lerp(x00, x10, v);
	_float y1v = Math::Lerp(x01, x11, v);

	return Math::Lerp(y0v, y1v, w); // [-1,1]
}

_int CNoise::Floor(_float x)
{
	_int p = static_cast<_int>(x);

	return (x < static_cast<_float>(p)) ? p - 1 : p;
}

_float CNoise::Fade(_float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

_uint CNoise::Hash(_uint x)
{
	x ^= x >> 16;
	x *= 0x7feb352dU;
	x ^= x >> 15;
	x *= 0x846ca68bU;
	x ^= x >> 16;

	return x;
}

_float CNoise::NormalizeHash(_int x, _int y, _int z)
{
	_uint iSeed = 0;

	iSeed ^= Hash((uint32_t)x * 73856093U);
	iSeed ^= Hash((uint32_t)y * 19349663U);
	iSeed ^= Hash((uint32_t)z * 83492791U);

	iSeed &= 0x7fffffffU;
	_float p = static_cast<_float>(iSeed) / 2147483647.0f;
	return p * 2.0f - 1.0f;
}

#pragma once

enum class MODULE_MASK :_uint
{
	NONE = 0,
	LIFE_TIME_VELOCITY = 1u << 0,
	LIFE_TIME_SIZE = 1u << 1,
	LIFE_TIME_COLOR = 1u << 2,
	SIZE_BY_SPEED = 1u << 3,
	TEXTURE_SHEET_ANIMATION = 1u << 4,
	NOISE = 1u << 5
};

inline MODULE_MASK operator | (MODULE_MASK a, MODULE_MASK b)
{
	return static_cast<MODULE_MASK>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline MODULE_MASK operator & (MODULE_MASK a, MODULE_MASK b)
{
	return static_cast<MODULE_MASK>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline MODULE_MASK operator ~ (MODULE_MASK a)
{
	return static_cast<MODULE_MASK>(~static_cast<unsigned int>(a));
}

inline MODULE_MASK operator ^ (MODULE_MASK a, MODULE_MASK b)
{
	return static_cast<MODULE_MASK>(static_cast<unsigned int>(a) ^ static_cast<unsigned int>(b));
}

inline _bool HasFlag(MODULE_MASK mask, MODULE_MASK flag)
{
	return (static_cast<unsigned int>(mask) & static_cast<unsigned int>(flag)) == static_cast<unsigned int>(flag);
}

typedef struct tagLifeTimeVelocity
{
	_float fDampScale{};
}LIFE_TIME_VELOCITY;

typedef struct tagLifeTimeSize
{
	_float2 vStartScale{};
	_float2 vEndScale{};
}LIFE_TIME_SIZE;

typedef struct tagLifeTimeColor
{
	_float4 vStartColor{};
	_float4 vEndColor{};
}LIFE_TIME_COLOR;

typedef struct tagSizeBySpeed
{

}SIZE_BY_SPEED;

typedef struct tagTextureSheetAnimation
{
	_bool isRandomFrameIndex{};
	_bool isParticleAnimated{};
	_uint iCol{ 1 };
	_uint iRow{ 1 };
	_uint iMaxFrameIndex{};
}TEXTURE_SHEET_ANIMATION;

typedef struct tagNoise
{
	_float fElapsedTime{};
	_float3 vStrength{};
	_float3 vFrequency{};
	_float3 vScrollSpeed{};
}NOISE;

typedef struct tagParticleUpdateParams
{
	LIFE_TIME_VELOCITY lifeTimeVelocity{};
	LIFE_TIME_SIZE lifeTimeSize{};
	LIFE_TIME_COLOR lifeTimeColor{};
	TEXTURE_SHEET_ANIMATION textureSheetAnimation{};
	NOISE noise{};
}PARTICLE_UPDATE_PARAMS;
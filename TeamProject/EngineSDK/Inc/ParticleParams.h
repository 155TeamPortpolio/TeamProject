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
};
inline MODULE_MASK operator & (MODULE_MASK a, MODULE_MASK b)
{
	return static_cast<MODULE_MASK>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline _bool HasFlag(MODULE_MASK mask, MODULE_MASK flag)
{
	return (static_cast<unsigned int>(mask) & static_cast<unsigned int>(flag)) == static_cast<unsigned int>(flag);
}

typedef struct tagParticleUpdateParams
{

}PARTICLE_UPDATE_PARAMS;
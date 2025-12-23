#pragma once

typedef struct tagTextureSlotModule
{
	enum class SAMPLER_MODE : _uint { WRAP, CLAMP, END };
	enum class MAIN_USAGE : _uint { AS_COLOR, AS_CHANNEL, AS_GRAYSCALE, END };
	enum class CHANNEL_USAGE : _uint { NONE, SHAPE_MASK, EMISSION, DISTORTION, END };

	SAMPLER_MODE eSamplerMode = SAMPLER_MODE::WRAP;
	MAIN_USAGE eMainUsage = MAIN_USAGE::AS_COLOR;
	CHANNEL_USAGE eRed = CHANNEL_USAGE::NONE;
	CHANNEL_USAGE eGreen = CHANNEL_USAGE::NONE;
	CHANNEL_USAGE eBlue = CHANNEL_USAGE::NONE;
	CHANNEL_USAGE eAlpha = CHANNEL_USAGE::NONE;

	_uint iSamplerModeParam{};
	_uint iMainUsageParam{};
	_uint4 vChannelUsageParam{};

}TEXTURE_SLOT_MODULE;

typedef struct tagColorModule
{
	EaseType eEaseType = EaseType::None;
	_float4 vStartColor{ 1.f,1.f,1.f,1.f };
	_float4 vEndColor{ 1.f,1.f,1.f,1.f };
	_float4 vCurrColor{ 1.f,1.f,1.f,1.f };
}COLOR_MODULE;

typedef struct tagScaleModule
{
	EaseType eEaseType = EaseType::None;
	_float3 vStartScale{ 1.f,1.f,1.f };
	_float3 vEndScale{ 1.f,1.f,1.f };
	_float3 vCurrScale{ 1.f,1.f,1.f };
}SCALE_MODULE;

typedef struct tagUVAnimationModule
{
	EaseType eEaseType = EaseType::None;
	_float2 vStartUVOffset{};
	_float2 vEndUVOffset{};
	_float2 vCurrUVOffset{};
}UV_ANIMATION_MODULE;

typedef struct tagSpriteAnimationModule
{
	_uint iCol{ 1 };
	_uint iRow{ 1 };
	_uint iMaxFrameIndex{ 1 };
	_uint iCurrFrameIndex{};
}SPRITE_ANIMATION_MODULE;

typedef struct tagDissolveModule
{
	EaseType eEaseType = EaseType::None;
	_float fStartProgress{};
	_float fEndProgress{};
	_float fProgress{};
}DISSOLVE_MODULE;

typedef struct tagBloomModule
{
	_float fIntensity{};
}BLOOM_MODULE;

typedef struct tagNoiseModule
{

}NOISE_MODULE;

typedef struct tagDistortionModule
{

}DISTORTION_MODULE;
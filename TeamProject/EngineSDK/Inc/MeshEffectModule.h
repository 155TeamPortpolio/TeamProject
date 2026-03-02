#pragma once

typedef struct tagTextureSlotModule
{
	enum class SAMPLER_MODE : _uint { WRAP, CLAMP, END };
	enum class MAIN_USAGE : _uint { AS_COLOR, AS_CHANNEL, END };
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
	_uint iRGBMask{};
	_uint iColorMode{};

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
	_float fEnableDissolve{};
	_float fDissolveSoftness{};
	_float fStartProgress{};
	_float fEndProgress{};
	_float fProgress{};
}DISSOLVE_MODULE;

typedef struct tagBloomModule
{
	_float fThreshold{};
	_float fSoftness{};
	_float fIntensity{};
}BLOOM_MODULE;

typedef struct tagNoiseModule
{
	_float fEnableNoise{};
	_float fNoiseStrength{};
	_float fNoiseTilling{};
	_float2 vNoiseUVSpeed{};
}NOISE_MODULE;

typedef struct tagMaskModule
{
	_float fEnableMaskA{};
	_float fEnableMaskB{};
	_float fMaskTilling{};
}MASK_MODULE;

typedef struct tagDistortionModule
{
	_bool useDiffuseAlpha = true;
	_bool useDistortionMask = false;
	_float fEnableDistortion{};
	_float fDistortionStrength{};
	_float fDistortionTilling{};
	_float2 vDistortionUVSpeed{};
}DISTORTION_MODULE;

typedef struct tagGradientModule
{
	enum class GRADIENT_MODE :_uint { GRAY_SCALE, UV_X, UV_Y, LIFE_TIME, END };

	_float fEnableGradient{};
	GRADIENT_MODE eGradientMode = GRADIENT_MODE::GRAY_SCALE;

}GRADIENT_MODULE;
#ifndef __SHADER_DEFINE_HLSL__
#define __SHADER_DEFINE_HLSL__

#pragma pack_matrix(row_major)
#pragma pack_constant_buffers

#include "Shader_State.hlsl"

cbuffer FrameBuffer : register(b0)
{
    matrix matView;
    matrix matProjection;
    matrix matViewInverse;
    matrix matProjectionInverse;
    matrix matOrthograph;
    float4 vCamPosition;
    float3 CameraForward;
    float zFar;
};

cbuffer LightBuffer : register(b1)
{
    vector vLightPosition;
    vector vLightDirection;
    vector vLightDiffuse;
    vector vLightAmbient;
    vector vLightSpecular;
    float   fLightRange;
    float3 lightPadding;
};

cbuffer TransformPerDraw : register(b2)
{
    uint TransformIndex;
};

cbuffer SkinningPerDraw : register(b4)
{
    uint SkinningOffset;
};

cbuffer MaterialBuffer : register(b6)
{
    float4 vMtrlDiffuse =1.f;
    float4 vMtrlAmbient = 1.f;
    float4 vMtrlSpecular;
    float fSpecularPow;
};

cbuffer ShadowBuffer : register(b8)
{
    matrix matShadowView;
    matrix matShadowProjection;
    matrix matShadowViewInverse;
    matrix matShadowProjectionInverse;
    float4 vShadowPosition;
    float zShadowFar;
    float3 ShadowPadding;
};

struct BoneMatrix{matrix BoneMat;};
struct TransfomMatrix{matrix Transform;};

// 式式式式式式式式式式式式式 Base Material 式式式式式式式式式式式式式
Texture2D DiffuseTexture : register(t1);
Texture2D NormalTexture : register(t2);
Texture2D OpacityTexture : register(t3);
Texture2D EmmisionTexture : register(t4);
Texture2D SpecularTexture : register(t5);

// 式式式式式式式式式式式式式 Orientation / Gray 式式式式式式式式式式式式式
Texture2D AlbedoGrayTexture : register(t6);
Texture2D NormalOryTexture : register(t7);
Texture2D AlbedoOryTexture : register(t8);
Texture2D EmmisionOryTexture : register(t9);

// 式式式式式式式式式式式式式 Wave / Index / Scale 式式式式式式式式式式式式式
Texture2D IndexMap : register(t10);
Texture2D ScaleX : register(t11);
Texture2D ScaleY : register(t12);
Texture2D ScaleXY : register(t13);

// 式式式式式式式式式式式式式 gradation / Mix 式式式式式式式式式式式式式
Texture2D MixtureTexture : register(t14);
Texture2D GradationTexture : register(t15);
Texture2D GradationEdgeTexture : register(t16);

// 式式式式式式式式式式式式式 Global / Mask 式式式式式式式式式式式式式
Texture2D g_PaletteTexture : register(t17);
Texture2D g_PaletteEdgeTexture : register(t18);
Texture2D g_MaskTexture : register(t19);

// 式式式式式式式式式式式式式  SRV 式式式式式式式式式式式式式
StructuredBuffer<BoneMatrix> g_BoneMatrices : register(t0);
StructuredBuffer<TransfomMatrix> ObjectBufferArray : register(t28);

// 式式式式式式式式式式式式式  Sprite 式式式式式式式式式式式式式
Texture2D SpriteTexture : register(t27);
#endif // __SHADER_DEFINE_HLSL__
#ifndef __SHADER_DEFINE_HLSL__
#define __SHADER_DEFINE_HLSL__

#pragma pack_matrix(row_major)
#pragma pack_constant_buffers

#include "Shader_State.hlsl"
#include "Shader_Function.hlsl"

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

cbuffer TransformPerDraw : register(b2)
{
    uint TransformIndex;
    vector vLookVector;
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
    matrix matStaticLightViewProj[4];
    matrix matSkinnedLightViewProj[4];
    float4 vCascadeSplits;
    float4 vShadowPosition;
    int iCurrentCascade;
    float zShadowFar;
    
    float2 ShadowPadding;
};

cbuffer LightBuffer : register(b9)
{
    vector vLightDir;
    vector vLightPos;
    vector vLightDiffuse;
    vector vLightAmbient;
    vector vLightSpecular;
    float fLightRange;
    float fLightIntensity;
    int iLightSize;
    float LightPadding;
}

struct BoneMatrix{matrix BoneMat;};
struct TransfomMatrix{matrix Transform;};

// 式式式式式式式式式式式式式 Base Material 式式式式式式式式式式式式式
Texture2D DiffuseTexture : register(t0);
Texture2D SpecularTexture : register(t1);
Texture2D AmbientTexture : register(t2);
Texture2D EmissiveTexture : register(t3);
Texture2D HeightTexture : register(t4);
Texture2D NormalTexture : register(t5);
Texture2D ShinessTexture : register(t6);
Texture2D OpacityTexture : register(t7);
Texture2D DisplacementTexture : register(t8);
Texture2D LightTexture : register(t9);
Texture2D ReflectionTexture : register(t10);
Texture2D BaseColorTexture : register(t11);
Texture2D NormalCameraTexture : register(t12);
Texture2D EmmisionTexture : register(t13);
Texture2D MetalnessTexture : register(t14);
Texture2D DiffuseRoughnessTexture : register(t15);
Texture2D AmbientOcclusionTexture : register(t16);
Texture2D NoiseTexture : register(t17);
Texture2D DissolveTexture : register(t18);
Texture2D AlphaMaskTexture : register(t19);
Texture2D DistortionTexture : register(t20);
Texture2D DitheringTexture : register(t21);
Texture2D GradientTexture : register(t22);

// 式式式式式式式式式式式式式  SRV 式式式式式式式式式式式式式
StructuredBuffer<BoneMatrix> g_BoneMatrices : register(t30);
StructuredBuffer<TransfomMatrix> ObjectBufferArray : register(t31);

// 式式式式式式式式式式式式式  Sprite 式式式式式式式式式式式式式
Texture2D SpriteTexture : register(t29);
Texture2D MaskTexture   : register(t28);
#endif // __SHADER_DEFINE_HLSL__
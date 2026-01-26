#ifndef __SHADER_DEFERRED_DEFINE_HLSL__
#define __SHADER_DEFERRED_DEFINE_HLSL__

#pragma pack_matrix(row_major)
#pragma pack_constant_buffers

#include "Shader_State.hlsl"
#include "Shader_Function.hlsl"
#include "Shader_PBRFunction.hlsl"

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

cbuffer SSAOBuffer : register(b10)
{
    float fRadius;
    float fBias;
    float fScreenWidth;
    float fScreenHeight;
};

cbuffer SSAOKernel : register(b11)
{
    float4 SSAOKernel[64];
};

struct BoneMatrix{matrix BoneMat;};
struct TransfomMatrix{matrix Transform;};

// ───────────── Forward ─────────────
//===Common===
Texture2D DiffuseTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MetalicTexture : register(t2);
Texture2D ShadowTexture : register(t3);
Texture2D DepthTexture : register(t4);

//===SkinnedMesh===
Texture2D RimLightTexture : register(t5);
Texture2D RimLightFinalTexture : register(t6);
Texture2D AmbientTexture : register(t7);
Texture2D FaceDirTexture : register(t8);

//===StaticMesh===
Texture2D EmissiveTexture : register(t10);

Texture2D SSAONoiseTexture : register(t11);
Texture2D SSAOTexture : register(t12);
Texture2D SSAOFinalTexture : register(t13);

//===LightAcc===
Texture2D LightTexture : register(t15);
Texture2D LightInfoTexture : register(t16);

//─────────────Effect─────────────
Texture2D EffectTexture : register(t20);
Texture2D EffectAccTexture : register(t21);
Texture2D EffectBloomAccTextutre : register(t22);
Texture2D RevealageTexture : register(t23);

//─────────────UI─────────────
Texture2D UI2DTexture : register(t30);
Texture2D UI3DTexture : register(t31);

//─────────────Post─────────────
//==Distortion==
Texture2D DistortionCombinedTexture : register(t32);
Texture2D VanishTexture : register(t33);
Texture2D VanishNoiseTexture : register(t34);

Texture2D MotionDepthTexture : register(t36);
Texture2D MotionBlurTexture : register(t37);
Texture2D MotionHeightTexture : register(t38);
Texture2D MotionNoiseTexture : register(t39);

//===Bloom===
Texture2D EffectBrightTexture : register(t40);
Texture2D EffectBloomInfo : register(t41);

Texture2D MeshBrightTexture : register(t42);
Texture2D MeshBloomInfo : register(t43);

Texture2D EffectBlurXTexture : register(t44);
Texture2D MeshBlurXTexture : register(t45);

Texture2D EffectBloomFinalTexture : register(t46);
Texture2D MeshBloomFinalTexture : register(t47);
//===========
//====FOG====
Texture2D StaticDepthTexture : register(t48);
Texture2D SkinnedDepthTexture : register(t49);

//====HDR=====
Texture2D HDRBrightTexture : register(t50);
Texture2D HDRBlurXTexture : register(t51);
Texture2D HDRBloomFinalTexture : register(t52);
//===RADIAL===
Texture2D RadialBloomTexture : register(t55);
Texture2D FogTexture : register(t56);
//============
Texture2D SkinnedCombinedTexture : register(t60);
Texture2D StaticCombinedTexture : register(t61);
Texture2D UICombinedTexture : register(t62);
Texture2D EffectCombinedTexture : register(t63);

Texture2D FinalTexture : register(t65);

// ─────────────  ETC ─────────────
Texture2DArray StaticShadowMapArray : register(t70);
Texture2DArray SkinnedShadowMapArray : register(t71);
Texture2D RampTexture : register(t72);

// ─────────────  SRV ─────────────
StructuredBuffer<BoneMatrix> g_BoneMatrices : register(t75);
StructuredBuffer<TransfomMatrix> ObjectBufferArray : register(t76);

// ─────────────  Sprite ─────────────
Texture2D SpriteTexture : register(t80);


// ─────────────  Shadow ─────────────
SamplerComparisonState ShadowSampler : register(s10);


#endif // __SHADER_DEFINE_HLSL__
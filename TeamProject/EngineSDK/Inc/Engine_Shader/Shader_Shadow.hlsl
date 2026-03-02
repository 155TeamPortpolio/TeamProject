#ifndef __SHADER_SHADOW_HLSL__
#define __SHADER_SHADOW_HLSL__

#include "Shader_Deferred_Define.hlsl"

static const float SHADOW_MAP_SIZE = 8192.0f;
static const float INV_SHADOW_MAP_SIZE = 1.0f / 8192.0f;

int GetCascadeIndex(float fViewDepth)
{
    int cascadeIndex = 2;
    cascadeIndex = (fViewDepth < vCascadeSplits.y) ? 1 : cascadeIndex;
    cascadeIndex = (fViewDepth < vCascadeSplits.x) ? 0 : cascadeIndex;
    return cascadeIndex;
}

struct BiasResult
{
    float depthBias;
    float3 normalOffset;
    bool SkipShadow;
};

BiasResult GetShadowBiasWithNormalOffset(float3 normal, float3 lightDir, int cascadeIndex)
{
    BiasResult result;
    
    float NdotL = dot(normal, lightDir);
    
    if (NdotL <= 0.1f)
    {
        result.SkipShadow = true;
        result.depthBias = 0.0f;
        result.normalOffset = float3(0, 0, 0);
        return result;
    }
    
    result.SkipShadow = false;
    
    float cascadeScale = (float) (1 << cascadeIndex); 

    float slopeFactor = saturate(1.0f - NdotL) / max(NdotL, 0.01f);
    float baseBias = 0.0002f * cascadeScale;
    result.depthBias = clamp(baseBias * (1.0f + slopeFactor), 0.0005f, 0.005f);
    
    float normalOffsetScale = 0.05f * cascadeScale;
    result.normalOffset = normal * (normalOffsetScale * (1.0f - NdotL));
    
    return result;
}

static const float2 PoissonDisk16[16] =
{
    float2(-0.613392, 0.617481),
    float2(0.170019, -0.040254),
    float2(0.645680, 0.493210),
    float2(-0.817194, -0.271096),
    float2(0.977050, -0.108615),
    float2(-0.667531, 0.326090),
    float2(0.078707, -0.715323),
    float2(0.724479, -0.580798),
    float2(-0.467574, -0.405438),
    float2(0.354411, -0.887570),
    float2(-0.084078, 0.898312),
    float2(0.034211, 0.979980),
    float2(-0.780145, 0.486251),
    float2(0.612476, 0.705252),
    float2(-0.578845, -0.768792),
    float2(0.391522, 0.849605)
};


float StaticFindBlockerDistance(float3 shadowCoord, int cascadeIndex, float bias)
{
    float blockerSum = 0.0f;
    int blockerCount = 0;
    float searchRadius = 5.0f * INV_SHADOW_MAP_SIZE;
    
    [unroll]
    for (int i = 0; i < 8; i++)
    {
        float2 offset = PoissonDisk16[i * 2] * searchRadius;
        float depth = StaticShadowMapArray.SampleLevel(
            DefaultSampler,
            float3(shadowCoord.xy + offset, cascadeIndex),
            0
        ).r;
        
        if (depth < shadowCoord.z - bias)
        {
            blockerSum += depth;
            blockerCount++;
        }
    }
    
    return (blockerCount == 0) ? -1.0f : (blockerSum / (float) blockerCount);
}

float SkinnedFindBlockerDistance(float3 shadowCoord, int cascadeIndex, float bias)
{
    float blockerSum = 0.0f;
    int blockerCount = 0;
    float searchRadius = 5.0f * INV_SHADOW_MAP_SIZE;
    
    [unroll]
    for (int i = 0; i < 8; i++)
    {
        float2 offset = PoissonDisk16[i * 2] * searchRadius;
        float depth = SkinnedShadowMapArray.SampleLevel(
            DefaultSampler,
            float3(shadowCoord.xy + offset, cascadeIndex),
            0
        ).r;
        
        if (depth < shadowCoord.z - bias)
        {
            blockerSum += depth;
            blockerCount++;
        }
    }
    
    return (blockerCount == 0) ? -1.0f : (blockerSum / (float) blockerCount);
}


float StaticSampleShadowMapPCSS(float3 shadowCoord, int cascadeIndex, float bias)
{
    float avgBlockerDepth = StaticFindBlockerDistance(shadowCoord, cascadeIndex, bias);
    
    if (avgBlockerDepth < 0.0f)
        return 1.0f;
    
    float penumbraWidth = (shadowCoord.z - avgBlockerDepth) / avgBlockerDepth;
    penumbraWidth = clamp(penumbraWidth * 20.0f, 1.0f, 10.0f);
    
    float shadow = 0.0f;
    float2 texelSize = penumbraWidth * INV_SHADOW_MAP_SIZE;
    
    [unroll]
    for (int i = 0; i < 16; i++)
    {
        float2 offset = PoissonDisk16[i] * texelSize;
        shadow += StaticShadowMapArray.SampleCmpLevelZero(
            ShadowSampler,
            float3(shadowCoord.xy + offset, cascadeIndex),
            shadowCoord.z - bias
        );
    }
    
    return shadow * 0.0625f; 
}

float SkinnedSampleShadowMapPCSS(float3 shadowCoord, int cascadeIndex, float bias)
{
    float avgBlockerDepth = SkinnedFindBlockerDistance(shadowCoord, cascadeIndex, bias);
    
    if (avgBlockerDepth < 0.0f)
        return 1.0f;
    
    float penumbraWidth = (shadowCoord.z - avgBlockerDepth) / avgBlockerDepth;
    penumbraWidth = clamp(penumbraWidth * 20.0f, 1.0f, 10.0f);
    
    float shadow = 0.0f;
    float2 texelSize = penumbraWidth * INV_SHADOW_MAP_SIZE;
    
    [unroll]
    for (int i = 0; i < 16; i++)
    {
        float2 offset = PoissonDisk16[i] * texelSize;
        shadow += SkinnedShadowMapArray.SampleCmpLevelZero(
            ShadowSampler,
            float3(shadowCoord.xy + offset, cascadeIndex),
            shadowCoord.z - bias
        );
    }
    
    return shadow * 0.0625f; // 1/16
}

float3 ProjectToShadowSpace(float4 worldPos, float4x4 lightViewProj)
{
    float4 sc = mul(worldPos, lightViewProj);
    sc.xyz /= sc.w;
    sc.x = sc.x * 0.5f + 0.5f;
    sc.y = sc.y * -0.5f + 0.5f;
    return sc.xyz;
}

bool IsInShadowBounds(float3 sc)
{
    return all(sc >= 0.0f) && all(sc <= 1.0f);
}

float CalculateShadowWithNormalOffset(float4 vWorldPos, float fViewDepth,
                                       float3 worldNormal, float3 lightDir, float2 screenPos)
{
    int cascadeIndex = GetCascadeIndex(fViewDepth);
    
    BiasResult biasResult = GetShadowBiasWithNormalOffset(worldNormal, lightDir, cascadeIndex);
    if (biasResult.SkipShadow)
        return 0.0f;

    float4 offsetWorldPos = vWorldPos;
    offsetWorldPos.xyz += biasResult.normalOffset;
    
    float3 staticSC = ProjectToShadowSpace(offsetWorldPos, matStaticLightViewProj[cascadeIndex]);
    float3 skinnedSC = ProjectToShadowSpace(offsetWorldPos, matSkinnedLightViewProj[cascadeIndex]);

    float fStatic = 1.0f;
    float fSkinned = 1.0f;
    
    if (IsInShadowBounds(staticSC))
        fStatic = StaticSampleShadowMapPCSS(staticSC, cascadeIndex, biasResult.depthBias);
    
    if (IsInShadowBounds(skinnedSC))
        fSkinned = SkinnedSampleShadowMapPCSS(skinnedSC, cascadeIndex, biasResult.depthBias);
    
    return fStatic * fSkinned;
}

float CalculateShadow(float4 vWorldPos, float fViewDepth,
                     float3 worldNormal, float3 lightDir, float2 screenPos)
{
    float shadowRaw = CalculateShadowWithNormalOffset(vWorldPos, fViewDepth,
                                                     worldNormal, lightDir, screenPos);
    
    float NdotL = dot(worldNormal, lightDir);
    float toon = (NdotL * 0.5f + 0.5f) < 0.8f ? 0.8f : 1.0f;
    float shadowToon = shadowRaw > 0.5f ? 1.0f : 0.3f;
    
    return toon * shadowToon;
}

#endif

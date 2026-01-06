#ifndef __SHADER_SHADOW_HLSL__
#define __SHADER_SHADOW_HLSL__

#include "Shader_Deferred_Define.hlsl"

int GetCascadeIndex(float fViewDepth)
{
    int cascadeIndex = 3;
    if (fViewDepth < vCascadeSplits.x)
        cascadeIndex = 0;
    else if (fViewDepth < vCascadeSplits.y)
        cascadeIndex = 1;
    else if (fViewDepth < vCascadeSplits.z)
        cascadeIndex = 2;
    
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
    result.SkipShadow = false;
    
    float NdotL = saturate(dot(normal, lightDir));
    if (NdotL <= 0.1f)
    {
        result.SkipShadow = true;
        result.depthBias = 0.0f;
        result.normalOffset = float3(0, 0, 0);
        return result;
    }
    
    float baseBias = 0.0002f * pow(2.0f, cascadeIndex);
    float slopeFactor = sqrt(1.0f - NdotL * NdotL) / max(NdotL, 0.001f);
    result.depthBias = baseBias * (1.0f + slopeFactor);
    result.depthBias = clamp(result.depthBias, 0.0005f, 0.005f);
    
    float normalOffsetScale = 0.05f * pow(2.0f, cascadeIndex);
    result.normalOffset = normal * normalOffsetScale * (1.0f - NdotL);
    
    return result;
}

static const float2 PoissonDisk64[64] =
{
    float2(-0.613392, 0.617481),
    float2(0.170019, -0.040254),
    float2(-0.299417, 0.791925),
    float2(0.645680, 0.493210),
    float2(-0.651784, 0.717887),
    float2(0.421003, 0.027070),
    float2(-0.817194, -0.271096),
    float2(-0.705374, -0.668203),
    float2(0.977050, -0.108615),
    float2(0.063326, 0.142369),
    float2(0.203528, 0.214331),
    float2(-0.667531, 0.326090),
    float2(-0.098422, -0.295755),
    float2(-0.885922, 0.215369),
    float2(0.566637, 0.605213),
    float2(0.039766, -0.396100),
    float2(0.751946, 0.453352),
    float2(0.078707, -0.715323),
    float2(-0.075838, -0.529344),
    float2(0.724479, -0.580798),
    float2(0.222999, -0.215125),
    float2(-0.467574, -0.405438),
    float2(-0.248268, -0.814753),
    float2(0.354411, -0.887570),
    float2(0.175817, 0.382366),
    float2(0.487472, -0.063082),
    float2(-0.084078, 0.898312),
    float2(0.488876, -0.783441),
    float2(0.470016, 0.217933),
    float2(-0.696890, -0.549791),
    float2(-0.149693, 0.605762),
    float2(0.034211, 0.979980),
    float2(0.503098, -0.308878),
    float2(-0.016205, -0.872921),
    float2(0.385784, -0.393902),
    float2(-0.146886, -0.859249),
    float2(0.643361, 0.164098),
    float2(0.634388, -0.049471),
    float2(-0.688894, 0.007843),
    float2(0.464034, -0.188818),
    float2(-0.440840, 0.137486),
    float2(0.364483, 0.511704),
    float2(0.034028, 0.325968),
    float2(0.099094, -0.308023),
    float2(0.693960, -0.366253),
    float2(0.678884, -0.204688),
    float2(0.001801, 0.780328),
    float2(0.145177, -0.898984),
    float2(0.062655, -0.611866),
    float2(0.315226, -0.604297),
    float2(-0.780145, 0.486251),
    float2(-0.371868, 0.882138),
    float2(0.200476, 0.494430),
    float2(-0.494552, -0.711051),
    float2(0.612476, 0.705252),
    float2(-0.578845, -0.768792),
    float2(-0.772454, -0.090976),
    float2(0.504440, 0.372295),
    float2(0.155736, 0.065157),
    float2(0.391522, 0.849605),
    float2(-0.620106, -0.328104),
    float2(0.789239, -0.419965),
    float2(-0.545396, 0.538133),
    float2(-0.178564, -0.596057)
};


float StaticSampleShadowMapAdaptivePCF(float3 shadowCoord, int cascadeIndex, float bias)
{
    float shadow = 0.0f;
    
    float baseFilterSize = 6.0f;
    float filterSize = baseFilterSize * (cascadeIndex + 1);
    float2 texelSize = filterSize / 8192.0f;
    
    [unroll]
    for (int i = 0; i < 64; i++)
    {
        float2 offset = PoissonDisk64[i] * texelSize;
        shadow += StaticShadowMapArray.SampleCmpLevelZero(
            ShadowSampler,
            float3(shadowCoord.xy + offset, cascadeIndex),
            shadowCoord.z - bias
        );
    }
    
    return shadow / 64.0f;
}

float SkinnedSampleShadowMapAdaptivePCF(float3 shadowCoord, int cascadeIndex, float bias)
{
    float shadow = 0.0f;
    
    float baseFilterSize = 6.0f;
    float filterSize = baseFilterSize * (cascadeIndex + 1);
    float2 texelSize = filterSize / 8192.0f;
    
    [unroll]
    for (int i = 0; i < 64; i++)
    {
        float2 offset = PoissonDisk64[i] * texelSize;
        shadow += SkinnedShadowMapArray.SampleCmpLevelZero(
            ShadowSampler,
            float3(shadowCoord.xy + offset, cascadeIndex),
            shadowCoord.z - bias
        );
    }
    
    return shadow / 64.0f;
}

float StaticSampleShadowMapRotatedGrid(float3 shadowCoord, int cascadeIndex, float bias, float2 screenPos)
{
    float shadow = 0.0f;
    
    float angle = frac(sin(dot(screenPos, float2(12.9898, 78.233))) * 43758.5453) * 6.28318530718;
    float s = sin(angle);
    float c = cos(angle);
    float2x2 rotationMatrix = float2x2(c, -s, s, c);
    
    float filterSize = 2.5f * (cascadeIndex + 1);
    float2 texelSize = filterSize / 8192.0f;
    
    [unroll]
    for (int x = -3; x <= 3; x++)
    {
        [unroll]
        for (int y = -3; y <= 3; y++)
        {
            float2 offset = mul(float2(x, y), rotationMatrix) * texelSize;
            shadow += StaticShadowMapArray.SampleCmpLevelZero(
                ShadowSampler,
                float3(shadowCoord.xy + offset, cascadeIndex),
                shadowCoord.z - bias
            );
        }
    }
    
    return shadow / 49.0f;
}

float SkinnedSampleShadowMapRotatedGrid(float3 shadowCoord, int cascadeIndex, float bias, float2 screenPos)
{
    float shadow = 0.0f;
    
    float angle = frac(sin(dot(screenPos, float2(12.9898, 78.233))) * 43758.5453) * 6.28318530718;
    float s = sin(angle);
    float c = cos(angle);
    float2x2 rotationMatrix = float2x2(c, -s, s, c);
    
    float filterSize = 2.5f * (cascadeIndex + 1);
    float2 texelSize = filterSize / 8192.0f;
    
    [unroll]
    for (int x = -3; x <= 3; x++)
    {
        [unroll]
        for (int y = -3; y <= 3; y++)
        {
            float2 offset = mul(float2(x, y), rotationMatrix) * texelSize;
            shadow += SkinnedShadowMapArray.SampleCmpLevelZero(
                ShadowSampler,
                float3(shadowCoord.xy + offset, cascadeIndex),
                shadowCoord.z - bias
            );
        }
    }
    
    return shadow / 49.0f;
}

float StaticFindBlockerDistance(float3 shadowCoord, int cascadeIndex, float bias)
{
    float blockerSum = 0.0f;
    int blockerCount = 0;
    
    float searchRadius = 5.0f / 8192.0f;
    
    for (int i = 0; i < 16; i++)
    {
        float2 offset = PoissonDisk64[i * 4] * searchRadius;
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
    
    if (blockerCount == 0)
        return -1.0f;
    
    return blockerSum / float(blockerCount);
}

float SkinnedFindBlockerDistance(float3 shadowCoord, int cascadeIndex, float bias)
{
    float blockerSum = 0.0f;
    int blockerCount = 0;
    
    float searchRadius = 5.0f / 8192.0f;
    
    for (int i = 0; i < 16; i++)
    {
        float2 offset = PoissonDisk64[i * 4] * searchRadius;
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
    
    if (blockerCount == 0)
        return -1.0f;
    
    return blockerSum / float(blockerCount);
}
// PCSS Main
float StaticSampleShadowMapPCSS(float3 shadowCoord, int cascadeIndex, float bias)
{
    float avgBlockerDepth = StaticFindBlockerDistance(shadowCoord, cascadeIndex, bias);
    
    if (avgBlockerDepth < 0.0f)
        return 1.0f;
    
    float penumbraWidth = (shadowCoord.z - avgBlockerDepth) / avgBlockerDepth;
    penumbraWidth = clamp(penumbraWidth * 20.0f, 1.0f, 10.0f);
    
    float shadow = 0.0f;
    float2 texelSize = penumbraWidth / 8192.0f;
    
    for (int i = 0; i < 64; i++)
    {
        float2 offset = PoissonDisk64[i] * texelSize;
        shadow += StaticShadowMapArray.SampleCmpLevelZero(
            ShadowSampler,
            float3(shadowCoord.xy + offset, cascadeIndex),
            shadowCoord.z - bias
        );
    }
    
    return shadow / 64.0f;
}

float SkinnedSampleShadowMapPCSS(float3 shadowCoord, int cascadeIndex, float bias)
{
    float avgBlockerDepth = SkinnedFindBlockerDistance(shadowCoord, cascadeIndex, bias);
    
    if (avgBlockerDepth < 0.0f)
        return 1.0f;
    
    float penumbraWidth = (shadowCoord.z - avgBlockerDepth) / avgBlockerDepth;
    penumbraWidth = clamp(penumbraWidth * 20.0f, 1.0f, 10.0f);
    
    float shadow = 0.0f;
    float2 texelSize = penumbraWidth / 8192.0f;
    
    for (int i = 0; i < 64; i++)
    {
        float2 offset = PoissonDisk64[i] * texelSize;
        shadow += SkinnedShadowMapArray.SampleCmpLevelZero(
            ShadowSampler,
            float3(shadowCoord.xy + offset, cascadeIndex),
            shadowCoord.z - bias
        );
    }
    
    return shadow / 64.0f;
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
    
    float4 staticshadowCoord = mul(offsetWorldPos, matStaticLightViewProj[cascadeIndex]);
    staticshadowCoord.xyz /= staticshadowCoord.w;
    staticshadowCoord.x = staticshadowCoord.x * 0.5f + 0.5f;
    staticshadowCoord.y = staticshadowCoord.y * -0.5f + 0.5f;
    
    float4 skinnedshadowCoord = mul(offsetWorldPos, matSkinnedLightViewProj[cascadeIndex]);
    skinnedshadowCoord.xyz /= skinnedshadowCoord.w;
    skinnedshadowCoord.x = skinnedshadowCoord.x * 0.5f + 0.5f;
    skinnedshadowCoord.y = skinnedshadowCoord.y * -0.5f + 0.5f;

    if (staticshadowCoord.x < 0.0f || staticshadowCoord.x > 1.0f ||
        staticshadowCoord.y < 0.0f || staticshadowCoord.y > 1.0f ||
        staticshadowCoord.z < 0.0f || staticshadowCoord.z > 1.0f ||
        skinnedshadowCoord.x < 0.0f || skinnedshadowCoord.x > 1.0f ||
        skinnedshadowCoord.y < 0.0f || skinnedshadowCoord.y > 1.0f ||
        skinnedshadowCoord.z < 0.0f || skinnedshadowCoord.z > 1.0f)
    {
        return 1.0f;
    }
    
    
    // 64 Poisson 
    //float shadow = SampleShadowMapAdaptivePCF(shadowCoord.xyz, cascadeIndex, biasResult.depthBias);
    
    // Rotated Grid 
    //float shadow = SampleShadowMapRotatedGrid(shadowCoord.xyz, cascadeIndex, biasResult.depthBias, screenPos);
    
    // PCSS 
    float fStatic = StaticSampleShadowMapPCSS(staticshadowCoord.xyz, cascadeIndex, biasResult.depthBias);
    float fSkinned = SkinnedSampleShadowMapPCSS(skinnedshadowCoord.xyz, cascadeIndex, biasResult.depthBias);
    
    float finalShadow = fStatic * fSkinned;
    
    return finalShadow;
}

float GetCascadeBlendFactor(float fViewDepth, int cascadeIndex)
{
    float blendDistance = 5.0f; // 블렌딩 거리 증가
    
    if (cascadeIndex == 0)
    {
        float distToNext = vCascadeSplits.x - fViewDepth;
        return saturate(distToNext / blendDistance);
    }
    else if (cascadeIndex == 1)
    {
        float distToNext = vCascadeSplits.y - fViewDepth;
        return saturate(distToNext / blendDistance);
    }
    else if (cascadeIndex == 2)
    {
        float distToNext = vCascadeSplits.z - fViewDepth;
        return saturate(distToNext / blendDistance);
    }
    
    return 1.0f;
}

float CalculateShadow(float4 vWorldPos, float fViewDepth,
                     float3 worldNormal, float3 lightDir, float2 screenPos)
{
    int cascadeIndex = GetCascadeIndex(fViewDepth);
    
    float shadow = CalculateShadowWithNormalOffset(vWorldPos, fViewDepth,
                                                     worldNormal, lightDir, screenPos);
    return shadow;
    
    // Cascade Blending
    float blendFactor = GetCascadeBlendFactor(fViewDepth, cascadeIndex);
    
    if (blendFactor < 1.0f && cascadeIndex < 3)
    {
        float nextShadow = CalculateShadowWithNormalOffset(vWorldPos, fViewDepth,
                                                            worldNormal, lightDir, screenPos);
        shadow = lerp(nextShadow, shadow, blendFactor);
    }
    
    return shadow;
}
#endif
#include "Shader_Deferred_Define.hlsl"

matrix g_WorldMatrix;

// ============================================================
// Cascade Index 계산
// ============================================================
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

// ============================================================
// Shadow Bias 계산 (Normal Offset 방식)
// ============================================================
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
    if (NdotL <= 0.01f)
    {
        result.SkipShadow = true;
        result.depthBias = 0.0f;
        result.normalOffset = float3(0, 0, 0);
        return result;
    }
    
    // Depth Bias
    float baseBias = 0.001f * pow(2.0f, cascadeIndex); // 0.0005 → 0.001
    float slopeFactor = sqrt(1.0f - NdotL * NdotL) / max(NdotL, 0.001f);
    result.depthBias = baseBias * (1.0f + slopeFactor);
    result.depthBias = clamp(result.depthBias, 0.0005f, 0.005f); // 최대값 증가
    
    // Normal Offset
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


float SampleShadowMapAdaptivePCF(float3 shadowCoord, int cascadeIndex, float bias)
{
    float shadow = 0.0f;
    
    float baseFilterSize = 6.0f; 
    float filterSize = baseFilterSize * (cascadeIndex + 1);
    float2 texelSize = filterSize / 8192.0f;
    
    [unroll]
    for (int i = 0; i < 64; i++)
    {
        float2 offset = PoissonDisk64[i] * texelSize;
        shadow += ShadowMapArray.SampleCmpLevelZero(
            ShadowSampler,
            float3(shadowCoord.xy + offset, cascadeIndex),
            shadowCoord.z - bias
        );
    }
    
    return shadow / 64.0f;
}

float SampleShadowMapRotatedGrid(float3 shadowCoord, int cascadeIndex, float bias, float2 screenPos)
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
            shadow += ShadowMapArray.SampleCmpLevelZero(
                ShadowSampler,
                float3(shadowCoord.xy + offset, cascadeIndex),
                shadowCoord.z - bias
            );
        }
    }
    
    return shadow / 49.0f;
}


float FindBlockerDistance(float3 shadowCoord, int cascadeIndex, float bias)
{
    float blockerSum = 0.0f;
    int blockerCount = 0;
    
    float searchRadius = 5.0f / 8192.0f;
    
    // 16개 샘플로 Blocker 찾기
    for (int i = 0; i < 16; i++)
    {
        float2 offset = PoissonDisk64[i * 4] * searchRadius; // 64개 중 16개만 사용
        float depth = ShadowMapArray.SampleLevel(
            DefaultSampler, // Comparison 아닌 일반 샘플러 필요
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
        return -1.0f; // No blocker
    
    return blockerSum / float(blockerCount);
}

// PCSS Main
float SampleShadowMapPCSS(float3 shadowCoord, int cascadeIndex, float bias)
{
    // 1. Blocker Search
    float avgBlockerDepth = FindBlockerDistance(shadowCoord, cascadeIndex, bias);
    
    if (avgBlockerDepth < 0.0f)
        return 1.0f; // No shadow
    
    // 2. Penumbra Size 계산
    float penumbraWidth = (shadowCoord.z - avgBlockerDepth) / avgBlockerDepth;
    penumbraWidth = clamp(penumbraWidth * 20.0f, 1.0f, 10.0f); // 크기 제한
    
    // 3. PCF with adaptive filter size
    float shadow = 0.0f;
    float2 texelSize = penumbraWidth / 8192.0f;
    
    for (int i = 0; i < 64; i++)
    {
        float2 offset = PoissonDisk64[i] * texelSize;
        shadow += ShadowMapArray.SampleCmpLevelZero(
            ShadowSampler,
            float3(shadowCoord.xy + offset, cascadeIndex),
            shadowCoord.z - bias
        );
    }
    
    return shadow / 64.0f;
}


float CalculateShadowWithNormalOffset(float4 vLightSpacePos[4], float4 vWorldPos, float fViewDepth,
                                       float3 worldNormal, float3 lightDir, float2 screenPos)
{
    int cascadeIndex = GetCascadeIndex(fViewDepth);
    
    BiasResult biasResult = GetShadowBiasWithNormalOffset(worldNormal, lightDir, cascadeIndex);
    if (biasResult.SkipShadow)
        return 0.0f; 

    float4 offsetWorldPos = vWorldPos;
    offsetWorldPos.xyz += biasResult.normalOffset;
    
    float4 shadowCoord = mul(offsetWorldPos, matLightViewProj[cascadeIndex]);
    shadowCoord.xyz /= shadowCoord.w;
    shadowCoord.x = shadowCoord.x * 0.5f + 0.5f;
    shadowCoord.y = shadowCoord.y * -0.5f + 0.5f;

    if (shadowCoord.x < 0.0f || shadowCoord.x > 1.0f ||
        shadowCoord.y < 0.0f || shadowCoord.y > 1.0f ||
        shadowCoord.z < 0.0f || shadowCoord.z > 1.0f)
    {
        return 1.0f;
    }
    
    
    // 옵션 1: 64 Poisson 
    //float shadow = SampleShadowMapAdaptivePCF(shadowCoord.xyz, cascadeIndex, biasResult.depthBias);
    
    // 옵션 2: Rotated Grid 
    //float shadow = SampleShadowMapRotatedGrid(shadowCoord.xyz, cascadeIndex, biasResult.depthBias, screenPos);
    
    // 옵션 3: PCSS 
    float shadow = SampleShadowMapPCSS(shadowCoord.xyz, cascadeIndex, biasResult.depthBias);
    
    return shadow;
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

float CalculateShadow(float4 vLightSpacePos[4], float4 vWorldPos, float fViewDepth,
                     float3 worldNormal, float3 lightDir, float2 screenPos)
{
    int cascadeIndex = GetCascadeIndex(fViewDepth);
    
    float shadow = CalculateShadowWithNormalOffset(vLightSpacePos, vWorldPos, fViewDepth,
                                                     worldNormal, lightDir, screenPos);
    return shadow;
    
    // Cascade Blending
    float blendFactor = GetCascadeBlendFactor(fViewDepth, cascadeIndex);
    
    if (blendFactor < 1.0f && cascadeIndex < 3)
    {
        float nextShadow = CalculateShadowWithNormalOffset(vLightSpacePos, vWorldPos, fViewDepth,
                                                            worldNormal, lightDir, screenPos);
        shadow = lerp(nextShadow, shadow, blendFactor);
    }
    
    return shadow;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    matrix matWVP = mul(g_WorldMatrix, matOrthograph);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_RESULT
{
    vector vResult : SV_TARGET0;
};

PS_OUT_RESULT PS_SSAO(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vDepthDesc = DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    float fViewZ = vDepthDesc.y * zFar;
    
    float3 N = mul(float4(worldNormal, 0.f), matView).xyz;
    N = normalize(N);
    
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);

    float3 fragPos = mul(vWorldPos, matView).xyz;
    
    float2 noiseScale = float2(fScreenWidth / 8.0, fScreenHeight / 8.0);
    float3 randomVec = SSAONoiseTexture.Sample(DefaultSampler, In.vTexcoord * noiseScale).xyz;
    randomVec = randomVec * 2.0 - 1.0;
    
    float3 T = normalize(randomVec - N * dot(randomVec, N));
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    
    float occlusion = 0.0;
    
    for (int i = 0; i < 64; ++i)
    {
        float3 samplePos = mul(SSAOKernel[i].xyz, TBN);
        samplePos = fragPos + samplePos * fRadius;
        
        float4 offset = float4(samplePos, 1.0);
        offset = mul(offset, matProjection);
        offset.xyz /= offset.w;
        
        offset.xy = offset.xy * 0.5 + 0.5;
        offset.y = 1.0 - offset.y;
        
        vector vSampleDepthDesc = DepthTexture.Sample(DefaultSampler, offset.xy);
        float fSampleViewZ = vSampleDepthDesc.y * zFar;
        
        vector vSampleWorldPos;
        vSampleWorldPos.x = offset.x * 2.f - 1.f;
        vSampleWorldPos.y = offset.y * -2.f + 1.f;
        vSampleWorldPos.z = vSampleDepthDesc.x;
        vSampleWorldPos.w = 1.f;
        
        vSampleWorldPos = vSampleWorldPos * fSampleViewZ;
        vSampleWorldPos = mul(vSampleWorldPos, matProjectionInverse);
        vSampleWorldPos = mul(vSampleWorldPos, matViewInverse);
        
        float3 sampleViewPos = mul(vSampleWorldPos, matView).xyz;
        float sampleDepth = sampleViewPos.z;
        
        float rangeCheck = smoothstep(0.0, 1.0, fRadius / abs(fragPos.z - sampleDepth));
        
        occlusion += (sampleDepth <= samplePos.z + fBias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / 64.0);
    Out.vResult = occlusion;
    
    return Out;
}

PS_OUT_RESULT PS_SSAO_BLUR(PS_IN In)
{
    PS_OUT_RESULT Out;
    float2 texelSize = 1.0 / float2(fScreenWidth, fScreenHeight);

    const float weights[5] = { 0.06136, 0.24477, 0.38774, 0.24477, 0.06136 };
    
    float result = 0.0;
    
    for (int x = -2; x <= 2; ++x)
    {
        for (int y = -2; y <= 2; ++y)
        {
            float2 offset = float2(float(x), float(y)) * texelSize;
            float weight = weights[x + 2] * weights[y + 2];
            result += SSAOTexture.Sample(DefaultSampler, In.vTexcoord + offset).r * weight;
        }
    }
    
    Out.vResult = result;
    
    return Out;
}

static const float weights[9] =
{
    0.2270270270,
    0.1945945946,
    0.1216216216,
    0.0540540541,
    0.0162162162,
    0.0081081081,
    0.0040540541,
    0.0020270270,
    0.0010135135
};

PS_OUT_RESULT PS_BLOOM_BLURX(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector BloomInfo = MeshBloomInfo.Sample(DefaultSampler, In.vTexcoord);
    float Strength = BloomInfo.g;

    float4 bright = MeshBrightTexture.Sample(DefaultSampler, In.vTexcoord);

    float3 result = bright.rgb * weights[0];
    float texelSize = Strength / fScreenWidth;
    
    float4 brightSample;
    for (int i = 1; i < 9; ++i)
    {
        brightSample = MeshBrightTexture.Sample(DefaultSampler, In.vTexcoord + float2(texelSize * i, 0));
        result += brightSample.rgb * weights[i];
            
        brightSample = MeshBrightTexture.Sample(DefaultSampler, In.vTexcoord - float2(texelSize * i, 0));
        result += brightSample.rgb * weights[i];
    }
        
    Out.vResult = float4(result, 1.f);
   
    return Out;
}

PS_OUT_RESULT PS_BLOOM_BLURY(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector BloomInfo = MeshBloomInfo.Sample(DefaultSampler, In.vTexcoord);
    float Strength = BloomInfo.g;

    float4 BlurX = MeshBlurXTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 result = BlurX.rgb * weights[0];
    float texelSize = Strength / fScreenHeight;
        
    for (int i = 1; i < 9; ++i)
    {
        result += MeshBlurXTexture.Sample(DefaultSampler,
                In.vTexcoord + float2(0, texelSize * i)).rgb * weights[i];
        result += MeshBlurXTexture.Sample(DefaultSampler,
                In.vTexcoord - float2(0, texelSize * i)).rgb * weights[i];
    }
    
    Out.vResult = float4(result, 1.f);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vLight : SV_TARGET0;
    vector fLightInfo : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalicDesc = MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    
    float roughness = vMetalicDesc.r;
    float metalic = vMetalicDesc.g;
    float specular = vMetalicDesc.b;
    float3 lightDir = normalize(vLightDir.xyz * -1);
    float fViewZ = vDepthDesc.y * zFar;
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float4 vLightSpacePos[4];
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        vLightSpacePos[i] = mul(vWorldPos, matLightViewProj[i]);
    }
    
    float shadow = CalculateShadow(vLightSpacePos, vWorldPos, fViewZ, worldNormal, lightDir, In.vTexcoord);
    
    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);
        
    float NdotL = dot(worldNormal, lightDir) * 0.5f + 0.5f;
        
    float3 halfVec = normalize(viewDir + lightDir);
    float specBase = saturate(dot(worldNormal, halfVec));
    float specularPower = lerp(50.0f, 5.0f, roughness);
    specular = pow(specBase, specularPower) * specular;
    
    float3 PBR = CalculateDirectionalLight(vDiffuse.rgb, worldNormal, metalic, roughness, viewDir, lightDir, vLightDiffuse.rgb, fLightIntensity, shadow);
        
    Out.vLight = float4(PBR * vNormalDesc.a, 1.f);
    //Out.vLight = float4(shadow, shadow, shadow, 1.f);

    Out.fLightInfo = float4(NdotL, specular, shadow, 1.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalicDesc = MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    
    float roughness = vMetalicDesc.r;
    float metalic = vMetalicDesc.g;
 
    float fViewZ = vDepthDesc.y * zFar;
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float3 lightDir = normalize(vLightPos.xyz - vWorldPos.xyz);
    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);
    
    float NdotL = dot(worldNormal, lightDir) * 0.5f + 0.5f;
    
    float3 PBR = CalculatePointLight
    (vDiffuse.rgb, worldNormal, metalic, roughness, vWorldPos.xyz, viewDir, lightDir, vLightDiffuse.rgb,
    fLightIntensity, vLightPos.xyz, fLightRange, 1.0f);
    
    Out.vLight = float4(PBR * vNormalDesc.a, vDiffuse.a);
    Out.fLightInfo = float4(NdotL, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_RESULT PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vLight = LightTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vBloom = MeshBloomFinalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vLightInfo = LightInfoTexture.Sample(DefaultSampler, In.vTexcoord);
    float ssao = SSAOFinalTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    float NdotL = vLightInfo.r;
    float2 vRampCoord = float2(1 - NdotL, 0.5f);
    vector vRampSample = RampTexture.Sample(DefaultSampler, vRampCoord);
    float vRamp = lerp(0.1f, 1.0f, vRampSample.g);
    
    float shadowValue = vLightInfo.b;
    shadowValue = saturate(shadowValue * 0.7f + 0.3f);
    
    float3 vAmbient = vLightAmbient.rgb * vDiffuse.rgb * ssao * shadowValue;
    vAmbient = max(vAmbient, vDiffuse.rgb * vLightAmbient.rgb * 0.05);
    float4 vResult = float4(vLight.rgb * vRamp + vAmbient, vDiffuse.a);
    
    float3 specularColor = vLightSpecular.rgb * vLightInfo.g;
    vResult.rgb += specularColor + vBloom.rgb;
    
    Out.vResult = vResult;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SSAO();
    }

    pass SSAO_BLUR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SSAO_BLUR();
    }

    pass BLOOM_BLURX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOM_BLURX();
    }

    pass BLOOM_BLURY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOM_BLURY();
    }

    pass DIRECTIONAL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass POINT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass COMBINED
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }
}

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
};

BiasResult GetShadowBiasWithNormalOffset(float3 normal, float3 lightDir, int cascadeIndex)
{
    BiasResult result;
    
    float NdotL = saturate(dot(normal, -lightDir));
    
    // Depth Bias
    float baseBias = 0.0002f * pow(2.0f, cascadeIndex);
    float slopeFactor = sqrt(1.0f - NdotL * NdotL) / max(NdotL, 0.5f);
    result.depthBias = baseBias * (1.0f + slopeFactor * 0.5f);
    result.depthBias = clamp(result.depthBias, 0.00005f, 0.002f);
    
    // Normal Offset
    float normalOffsetScale = 0.2f * pow(2.0f, cascadeIndex);
    //float normalOffsetScale = 0.5f * pow(2.0f, cascadeIndex);
    result.normalOffset = normal * normalOffsetScale;
    
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
    
    // Normal Offset 적용
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
    float shadow = SampleShadowMapAdaptivePCF(shadowCoord.xyz, cascadeIndex, biasResult.depthBias);
    
    // 옵션 2: Rotated Grid 
    //float shadow = SampleShadowMapRotatedGrid(shadowCoord.xyz, cascadeIndex, biasResult.depthBias, screenPos);
    
    // 옵션 3: PCSS 
    //float shadow = SampleShadowMapPCSS(shadowCoord.xyz, cascadeIndex, biasResult.depthBias);
    
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
    
    matrix matWV, matWVP;
    matWVP = mul(g_WorldMatrix, matOrthograph);
    
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

PS_OUT_RESULT PS_OUTLINERIMLIGHT(PS_IN In)
{
    PS_OUT_RESULT Out;

    float4 vRimInfo = RimLightTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vRimInfo.a <= 0.f)
    {
        Out.vResult = float4(0.f, 0.f, 0.f, 0.f);
        return Out;
    }

    float4 vNormalDesc = NormalTexture.Sample(PointClampSampler, In.vTexcoord);
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    float4 vDepthDesc = DepthTexture.Sample(PointClampSampler, In.vTexcoord);

    float fViewZ = vDepthDesc.y * zFar;

    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);
    float NdotV = saturate(dot(worldNormal, viewDir));
    
    float rimPower = 1.f - NdotV;
    float fresnelPower = lerp(2.0, 8.0, vRimInfo.a);
    rimPower = pow(rimPower, fresnelPower);

    float fRim = smoothstep(0.45, 0.55, rimPower);

    float3 vRimColor = vRimInfo.rgb * fRim;
    Out.vResult = float4(vRimColor, 1.f);

    return Out;
}

PS_OUT_RESULT PS_RIMLIGHT(PS_IN In)
{
    PS_OUT_RESULT Out;

    float4 vRimInfo = RimLightTexture.Sample(PointSampler, In.vTexcoord);

    if (vRimInfo.a <= 0.f)
    {
        Out.vResult = float4(0.f, 0.f, 0.f, 0.f);
        return Out;
    }

    float4 vNormal = NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vDepthDesc = DepthTexture.Sample(PointSampler, In.vTexcoord);

    float fViewZ = vDepthDesc.y * zFar;

    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);
    vNormal = float4(vNormal.xyz * 2.f - 1.f, 0.f);

    float fRim = 1.f - saturate(dot(normalize(vNormal.xyz), viewDir));
    fRim = pow(fRim, vRimInfo.a);

    float3 vRimColor = vRimInfo.rgb * fRim;

    Out.vResult = float4(vRimColor, 1.f);

    return Out;
}

//GaussianBlur
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
PS_OUT_RESULT PS_BRIGHT(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float emissive = AmbientTexture.Sample(DefaultSampler, In.vTexcoord).b;
    float4 vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
  
    float3 vResult = vDiffuse.rgb * emissive;
    
    Out.vResult = float4(vResult, 1.f);
    return Out;
}

PS_OUT_RESULT PS_BLOOM_BLURX(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 bright = MeshBrightTexture.Sample(DefaultSampler, In.vTexcoord);

    float3 result = bright.rgb * weights[0];
    float texelSize = 0.5f / fScreenWidth;
    
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

    float4 BlurX = MeshBlurXTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 result = BlurX.rgb * weights[0];
    float texelSize = 0.5f / fScreenHeight;
        
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
    float2 fLightInfo : SV_TARGET1;
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
    
    float Skin = vMetalicDesc.a;
    
    float3 lightDir = normalize(vLightDir.xyz * -1);
    float fViewZ = vDepthDesc.a;
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
    //float shadow = CalculateShadow(vLightSpacePos, fViewZ, vWorldPos.xyz, worldNormal, lightDir);
    
    if (Skin < 0.7f)
    {
        float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);

        float NdotL = dot(worldNormal, lightDir) * 0.5f + 0.5f;

        float3 halfVec = normalize(viewDir + lightDir);
        float specBase = saturate(dot(worldNormal, halfVec));
        float specularPower = lerp(50.0f, 5.0f, roughness);
        specular = pow(specBase, specularPower) * specular;
        
        float3 PBR = CalculateDirectionalLight(vDiffuse.rgb, worldNormal, metalic, roughness, viewDir, lightDir, vLightDiffuse.rgb, fLightIntensity, shadow);
    
        Out.vLight = float4(PBR * vNormalDesc.a, 1.f);      
        Out.fLightInfo = float2(NdotL, specular);
        return Out;
    }
    else
    {
        vector LightDesc = vMetalicDesc;
        float3 vLookVector = normalize(FaceDirTexture.Sample(DefaultSampler, In.vTexcoord).xyz * 2.f - 1.f);
        
        float3 headRight = normalize(cross(float3(0, 1, 0), vLookVector));

        float RdotL = dot(headRight, lightDir);
        float FdotL = dot(vLookVector, lightDir);
        
        float faceShadow = LightDesc.r;
        float specularMask = LightDesc.g;
    
        faceShadow *= saturate(-FdotL);
        float brightness = lerp(0.15f, 0.45f, faceShadow);

        Out.vLight = float4(vDiffuse.rgb * vLightDiffuse.rgb * brightness * vNormalDesc.a, 1.f);
        Out.fLightInfo = float2(brightness, 0);
    }
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalicDesc = MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if(vMetalicDesc.a > 0.7f)
    {
        Out.vLight = float4(0.f, 0.f, 0.f, 1.f);
        Out.fLightInfo = float2(0.f, 0.f);
        return Out;
    }
    
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
    
    Out.vLight = float4(PBR * vNormalDesc.a, 1.f);
    Out.fLightInfo = float2(NdotL, 0.f);
    
    return Out;
}

PS_OUT_RESULT PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vLight = LightTexture.Sample(DefaultSampler, In.vTexcoord);
    float2 fLightInfo = LightInfoTexture.Sample(DefaultSampler, In.vTexcoord).rg;
    vector vAmbient = AmbientTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vRimLight = RimLightFinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float fOutLine = NormalTexture.Sample(DefaultSampler, In.vTexcoord).a;
    vector vMetalic = MetalicTexture.Sample(DefaultSampler, In.vTexcoord).a;
    vector vBloom = MeshBloomFinalTexture.Sample(DefaultSampler, In.vTexcoord);

    float NdotL = fLightInfo.r;
    float2 vRampCoord = float2(1 - NdotL, 0.5f);
    vector vRampSample = RampTexture.Sample(DefaultSampler, vRampCoord);
    float vRamp = lerp(0.1f, 1.0f, vRampSample.g);
    
    float3 ambient = vLightAmbient.rgb * vDiffuse.rgb * vAmbient.g * fOutLine;
    ambient = max(ambient, vDiffuse.rgb * vLightAmbient.rgb * 0.05);
    
    if (vMetalic.a < 0.7) Out.vResult = float4(vLight.rgb * vRamp + ambient, 1.f);
    else Out.vResult = float4(vLight.rgb, 1.f);
    
    float rimIntensity = max(vRamp, 0.5f);
    Out.vResult.rgb += vRimLight.rgb * rimIntensity;
    
    float3 specularColor = vLightSpecular.rgb * fLightInfo.g;
    Out.vResult.rgb += specularColor + vBloom.rgb;
    return Out;
}


technique11 DefaultTechnique
{
    pass OUTLINERIMLIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINERIMLIGHT();
    }

    pass RIMLIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RIMLIGHT();
    }

    pass BRIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRIGHT();
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
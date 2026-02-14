#include "Shader_Define.hlsl"

static const float g_TsunamiHeight = 38.0f;

static const float g_PeakTime = 0.42f;
static const float g_RiseWidth = 0.35f;
static const float g_FallWidth = 0.12f;

static const float g_WallDepthStart = 0.30f;
static const float g_WallDepthEnd = 0.70f;
static const float g_CurlForward = 35.0f;
static const float g_CurlDrop = 30.0f;
static const float g_CrashForward = 50.0f;
static const float g_CrashDrop = 55.0f;

static const float g_TopNoiseScale = 6.0f;
static const float g_SurfaceNoise = 2.0f;
static const float g_NoiseFreqX = 0.04f;
static const float g_NoiseFreqZ = 0.08f;
static const float g_NoiseSpeed = 1.5f;

float g_Time;
float g_CycleTime;
float g_Roughness;
float g_FoamAmount;
float2 g_NoiseOffset;
float3 g_WaterTint;
float g_TintStrength;

float SafeRoughness()
{
    return (g_Roughness > 0.001) ? g_Roughness : 1.0;
}
float SafeFoamAmount()
{
    return (g_FoamAmount > 0.001) ? g_FoamAmount : 1.0;
}
float SafeTintStrength()
{
    return (g_TintStrength > 0.001) ? g_TintStrength : 0.0;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vNormal : NORMAL;
};

struct VS_OUT
{
    float3 vLocalPos : POSITION;
    float3 vWorldPos : TEXCOORD1;
    float2 vTexcoord : TEXCOORD0;
    float3 vNormal : NORMAL;
};

VS_OUT VS_TESS(VS_IN In)
{
    VS_OUT Out;
    Out.vLocalPos = In.vPosition;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), ObjectBufferArray[TransformIndex].Transform).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = In.vNormal;
    return Out;
}

struct PatchConstant
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

PatchConstant PatchConstantFunc(InputPatch<VS_OUT, 3> patch)
{
    PatchConstant pt;
    pt.edges[0] = 64.0;
    pt.edges[1] = 64.0;
    pt.edges[2] = 64.0;
    pt.inside = 64.0;
    return pt;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunc")]
VS_OUT HS_TESS(InputPatch<VS_OUT, 3> patch, uint id : SV_OutputControlPointID)
{
    return patch[id];
}

float hash(float2 p)
{
    return frac(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x))));
}

float valueNoise(float2 x)
{
    float2 i = floor(x);
    float2 f = frac(x);
    float a = hash(i);
    float b = hash(i + float2(1.0, 0.0));
    float c = hash(i + float2(0.0, 1.0));
    float d = hash(i + float2(1.0, 1.0));
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(float2 p, int octaves)
{
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * valueNoise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

struct DS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vWorldPos : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
    float3 vNormal : NORMAL;
    float fFoamMask : TEXCOORD3;
    float fHeightPct : TEXCOORD4;
    float fPhase : TEXCOORD5;
};

[domain("tri")]
DS_OUT DS_TESS(PatchConstant pc, float3 bary : SV_DomainLocation, const OutputPatch<VS_OUT, 3> patch)
{
    DS_OUT Out = (DS_OUT) 0;

    float3 worldPos = patch[0].vWorldPos * bary.x + patch[1].vWorldPos * bary.y + patch[2].vWorldPos * bary.z;
    float2 texcoord = patch[0].vTexcoord * bary.x + patch[1].vTexcoord * bary.y + patch[2].vTexcoord * bary.z;

    float t = frac(g_Time / g_CycleTime);
    float depth = texcoord.y;
    float roughness = SafeRoughness();

    float edgeMask = smoothstep(0.0, 0.03, texcoord.x) * smoothstep(1.0, 0.97, texcoord.x)
                   * smoothstep(0.0, 0.03, texcoord.y) * smoothstep(1.0, 0.97, texcoord.y);

    float wallBase = smoothstep(g_WallDepthStart, g_WallDepthEnd, depth);
    float wallBack = 1.0 - smoothstep(g_WallDepthEnd, 1.0, depth) * 0.3;
    float wallProfile = wallBase * wallBack;

    float xCenter = abs(texcoord.x - 0.5) * 2.0;
    float xProfile = 1.0 - pow(xCenter, 2.5) * 0.55;
    xProfile = max(xProfile, 0.15);

    float risePart = exp(-pow((t - g_PeakTime) / g_RiseWidth, 2.0));
    float fallPart = exp(-pow((t - g_PeakTime) / g_FallWidth, 2.0));
    float blendFactor = smoothstep(g_PeakTime - 0.03, g_PeakTime + 0.03, t);
    float envelope = lerp(risePart, fallPart, blendFactor);
    envelope *= smoothstep(0.0, 0.08, t);
    envelope *= smoothstep(1.0, 0.93, t);

    float height = g_TsunamiHeight * envelope * wallProfile * xProfile;
    float topRegion = smoothstep(0.5, 0.9, wallProfile);

    float2 noiseCoord = float2(worldPos.x * g_NoiseFreqX, worldPos.z * g_NoiseFreqZ + g_Time * g_NoiseSpeed)
                      + g_NoiseOffset;
    float topNoise = fbm(noiseCoord, 4) * 2.0 - 1.0;
    height += topNoise * g_TopNoiseScale * roughness * topRegion * envelope * xProfile;

    float2 surfNoiseCoord = float2(worldPos.x * 0.15, worldPos.z * 0.15 + g_Time * 2.0)
                          + g_NoiseOffset * 1.7;
    float surfNoise = (valueNoise(surfNoiseCoord) - 0.5) * 2.0;
    height += surfNoise * g_SurfaceNoise * roughness * wallProfile * envelope * xProfile;

    float2 bumpCoord = float2(worldPos.x * 0.02, g_Time * 0.3)
                     + g_NoiseOffset * 0.5;
    float largeBump = pow(valueNoise(bumpCoord), 2.0) * 7.5;
    height += largeBump * roughness * topRegion * envelope * xProfile;

    float spatialRatio = saturate(wallProfile * xProfile / 0.8);
    float topFocus = saturate((spatialRatio - 0.1) / 0.9);
    float curlMask = pow(topFocus, 1.5);

    float curlStart = g_PeakTime * 0.9;
    float curlEnd = g_PeakTime + 0.35;
    float overPeak = saturate((t - curlStart) / (curlEnd - curlStart));
    float curlCrash = overPeak * overPeak * (3.0 - 2.0 * overPeak);

    float maxCurlAngle = 3.14159 * 1.1;
    float curlAngle = curlCrash * curlMask * maxCurlAngle;

    float localPeakHeight = g_TsunamiHeight * envelope * wallProfile * xProfile;
    float pivotHeight = localPeakHeight * 0.55;
    float armLength = max(height - pivotHeight, 0.0);

    float preCurlLean = envelope * (1.0 - curlCrash) * curlMask * g_CurlForward * 0.4;
    worldPos.z -= preCurlLean * edgeMask;

    float sinA = sin(curlAngle);
    float cosA = cos(curlAngle);

    if (armLength > 0.01 && curlAngle > 0.01)
    {
        float localY = armLength;
        float rotatedY = localY * cosA;
        float rotatedZ = localY * sinA;
        height = pivotHeight + rotatedY;
        worldPos.z -= rotatedZ * edgeMask;
    }
    else
    {
        float leanAmount = curlCrash * saturate(wallProfile - 0.2) * 15.0;
        worldPos.z -= leanAmount * edgeMask * xProfile;
    }

    height = max(height, 0.0);
    float finalHeight = height * edgeMask;
    worldPos.y += finalHeight;
    float heightForShading = finalHeight;

    float eps = 0.8;
    float2 ncXp = float2((worldPos.x + eps) * g_NoiseFreqX, worldPos.z * g_NoiseFreqZ + g_Time * g_NoiseSpeed)
                + g_NoiseOffset;
    float2 ncXm = float2((worldPos.x - eps) * g_NoiseFreqX, worldPos.z * g_NoiseFreqZ + g_Time * g_NoiseSpeed)
                + g_NoiseOffset;
    float hXp = fbm(ncXp, 4) * g_TopNoiseScale * roughness * topRegion * envelope;
    float hXm = fbm(ncXm, 4) * g_TopNoiseScale * roughness * topRegion * envelope;
    float dhdx = (hXp - hXm) / (2.0 * eps);

    float depthPlus = saturate(depth + 0.01);
    float depthMinus = saturate(depth - 0.01);
    float wallPlus = smoothstep(g_WallDepthStart, g_WallDepthEnd, depthPlus)
                   * (1.0 - smoothstep(g_WallDepthEnd, 1.0, depthPlus) * 0.3);
    float wallMinus = smoothstep(g_WallDepthStart, g_WallDepthEnd, depthMinus)
                    * (1.0 - smoothstep(g_WallDepthEnd, 1.0, depthMinus) * 0.3);
    float dhdz_wall = (wallPlus - wallMinus) * g_TsunamiHeight * envelope * xProfile / (2.0 * 0.01 * 100.0);

    float3 normal = normalize(float3(-dhdx, 1.0, -dhdz_wall));
    float curlNormalStrength = saturate(curlCrash * curlMask);
    float3 curlNormal = normalize(float3(0, -sinA, -cosA));
    normal = normalize(lerp(normal, curlNormal, curlNormalStrength * 0.6));

    Out.vWorldPos = float4(worldPos, 1.0);
    Out.vPosition = mul(float4(worldPos, 1.0), matView);
    Out.vPosition = mul(Out.vPosition, matProjection);
    Out.vTexcoord = texcoord;
    Out.vNormal = normal;
    Out.fFoamMask = saturate(heightForShading / (g_TsunamiHeight * 0.55)) * smoothstep(0.85, 0.99, topRegion);
    Out.fHeightPct = saturate(heightForShading / g_TsunamiHeight);
    Out.fPhase = t;

    return Out;
}

float3 UnpackNormalRG(float4 packed)
{
    float3 n;
    n.xy = packed.rg * 2.0 - 1.0;
    n.z = sqrt(max(1.0 - dot(n.xy, n.xy), 0.0));
    return n;
}

float3 BlendNormalUDN(float3 base, float3 detail)
{
    return normalize(float3(base.xy + detail.xy, base.z));
}

float4 PS_TESS(DS_OUT In) : SV_TARGET
{
    // ── 팔레트 (톤매핑 *1.2 ACES 고려) ──
    static const float3 DeepColor = float3(0.010, 0.002, 0.008);
    static const float3 MidColor = float3(0.05, 0.005, 0.030);
    static const float3 ShallowColor = float3(0.13, 0.012, 0.070);
    static const float3 BrightColor = float3(0.25, 0.030, 0.140);
    static const float3 HotColor = float3(0.38, 0.060, 0.220);

    static const float3 SunDir = normalize(float3(0.4, 0.75, -0.5));
    static const float3 SunColor = float3(0.50, 0.25, 0.35);

    float heightPct = In.fHeightPct;
    float foamMask = In.fFoamMask;
    float phase = In.fPhase;
    float3 worldPos = In.vWorldPos.xyz;
    float time = g_Time;

    // ── 노말맵 3레이어 (RG 언팩) ──
    float2 uvN1 = worldPos.xz * 0.025 + float2(time * 0.3, time * 0.15);
    float2 uvN2 = worldPos.xz * 0.050 + float2(-time * 0.2, time * 0.35);
    float2 uvN3 = worldPos.xz * 0.120 + float2(time * 0.15, -time * 0.25);

    float4 rawN1 = NormalTexture.Sample(LinearSampler, uvN1);
    float4 rawN2 = MetalnessTexture.Sample(LinearSampler, uvN2);
    float4 rawN3 = AmbientTexture.Sample(LinearSampler, uvN3);

    float3 detailN1 = UnpackNormalRG(rawN1);
    float3 detailN2 = UnpackNormalRG(rawN2);
    float3 detailN3 = UnpackNormalRG(rawN3);

    float3 blendedDetail = BlendNormalUDN(detailN1, detailN2 * 0.7);
    blendedDetail = BlendNormalUDN(blendedDetail, detailN3 * 0.4);

    float3 meshN = normalize(In.vNormal);
    float detailStrength = lerp(0.4, 1.0, heightPct);
    float3 N = normalize(float3(
        meshN.x + blendedDetail.x * detailStrength,
        meshN.y,
        meshN.z + blendedDetail.y * detailStrength
    ));

    float3 V = normalize(vCamPosition.xyz - worldPos);
    float3 H = normalize(SunDir + V);
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, SunDir));
    float NdotH = saturate(dot(N, H));

    // ── 기본 그라데이션 ──
    float3 waterColor = lerp(DeepColor, MidColor, smoothstep(0.0, 0.20, heightPct));
    waterColor = lerp(waterColor, ShallowColor, smoothstep(0.15, 0.45, heightPct));
    waterColor = lerp(waterColor, BrightColor, smoothstep(0.50, 0.80, heightPct));


    float2 uvCaustic1 = worldPos.xz * 0.018 + float2(time * 0.15, time * 0.08);
    float2 uvCaustic2 = worldPos.xz * 0.030 + float2(-time * 0.10, time * 0.18);

    float4 causticA = DiffuseTexture.Sample(LinearSampler, uvCaustic1);
    float4 causticB = DiffuseTexture.Sample(LinearSampler, uvCaustic2);

    // G채널 블렌드 → 에너지 줄기 (셀 경계선)
    float veinLine = saturate(causticA.g * 0.6 + causticB.g * 0.5);
    veinLine = smoothstep(0.25, 0.70, veinLine); // 부드럽게 threshold
    float veinMask = smoothstep(0.08, 0.50, heightPct); // 밑바닥엔 약하게
    float veinIntensity = veinLine * veinMask;

    // R채널 블렌드 → 어두운 영역 강화 (깊이감)
    float darkBlob = saturate(causticA.r * 0.5 + causticB.r * 0.5);
    darkBlob = 1.0 - smoothstep(0.3, 0.7, darkBlob) * 0.3;
    waterColor *= darkBlob;

    // 에너지 줄기 색 적용
    waterColor = lerp(waterColor, BrightColor, veinIntensity * 0.5);
    waterColor = lerp(waterColor, HotColor, pow(veinIntensity, 2.0) * 0.35);

    // B채널 → 미세 엣지 반짝
    float edgeSparkle = saturate(causticA.b * 0.5 + causticB.b * 0.5);
    edgeSparkle = pow(smoothstep(0.5, 0.85, edgeSparkle), 2.0);

    // ── SSS ──
    float sssMask = pow(saturate((heightPct - 0.25) / 0.75), 1.3);
    float sssBackLight = pow(saturate(dot(V, -SunDir + N * 0.3)), 3.0) * 0.4;
    float sssThin = smoothstep(0.55, 0.88, heightPct) * 0.35;
    float sssTotal = saturate(sssBackLight + sssThin) * sssMask;
    waterColor = lerp(waterColor, HotColor * 0.7, sssTotal * 0.4);

    // ── 라이팅 ──
    float wrapDiffuse = saturate(NdotL * 0.5 + 0.5);
    float3 ambient = lerp(float3(0.005, 0.001, 0.003), float3(0.015, 0.003, 0.010), heightPct);
    float3 diffuse = waterColor * wrapDiffuse * SunColor * 0.5 + ambient;

    // 스페큘러
    float specSharp = pow(NdotH, 200.0) * NdotL * 0.5;
    float specWide = pow(NdotH, 24.0) * NdotL * 0.12;
    float3 specular = (specSharp + specWide) * SunColor;

    // 프레넬 + 반사
    float fresnel = pow(1.0 - NdotV, 3.0);
    fresnel = lerp(0.02, 0.30, fresnel);
    float3 reflDir = reflect(-V, N);
    float3 envReflect = ShallowColor * 0.15;
    envReflect += pow(saturate(dot(reflDir, SunDir)), 32.0) * SunColor * 0.12;

    float3 lit = lerp(diffuse, envReflect, fresnel * 0.25) + specular;

    // ── 에너지 줄기의 엣지에 미세 글로우 ──
    lit += HotColor * edgeSparkle * veinMask * 0.12;

    // ── 능선 글로우 ──
    float ridgeGlow = smoothstep(0.72, 0.93, heightPct);
    lit += BrightColor * ridgeGlow * 0.06;

    float2 uvFoam1 = worldPos.xz * 0.05 + float2(time * 0.5, time * 0.15);
    float2 uvFoam2 = worldPos.xz * 0.08 + float2(-time * 0.3, time * 0.7);
    float foam1 = LightTexture.Sample(LinearSampler, uvFoam1).r;
    float foam2 = LightTexture.Sample(LinearSampler, uvFoam2).r;

    // 스플래터 패턴 → 밝은 에너지 파편
    float splatter = saturate(foam1 * 0.5 + foam2 * 0.4);
    float splatterBright = pow(smoothstep(0.45, 0.80, splatter), 2.0);
    float splatterMask = smoothstep(0.10, 0.45, heightPct);
    lit += HotColor * splatterBright * splatterMask * 0.15;

    // 포말 패턴 → 부드러운 에너지 안개 (큰 영역)
    float foamSoft = smoothstep(0.2, 0.6, splatter);
    float foamIntensity = foamMask * foamSoft * 0.25;
    lit = lerp(lit, BrightColor * 0.6, foamIntensity);

    // ── 스프레이 ──
    float crashPhase = saturate((phase - g_PeakTime) / 0.25);
    float sprayMask = pow(saturate(foamMask * 1.5), 2.0) * crashPhase;
    float sprayNoise = fbm(worldPos.xz * 0.2 + time * 3.0, 2);
    float spray = sprayMask * sprayNoise;
    lit += HotColor * spray * 0.06;

    // ── 높이 대비 ──
    lit *= lerp(0.25, 1.0, pow(heightPct, 0.7));

    // ── 최소 발광 ──
    lit += DeepColor * 1.2 * smoothstep(0.0, 0.12, heightPct);

    float alpha = smoothstep(0.0, 0.05, heightPct);
    alpha = lerp(alpha, 1.0, foamIntensity * 0.2);
    alpha = lerp(alpha, 1.0, veinIntensity * heightPct * 0.15);

    return float4(lit, saturate(alpha));
}
technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_TESS();
        HullShader = compile hs_5_0 HS_TESS();
        DomainShader = compile ds_5_0 DS_TESS();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TESS();
    }
}
#include "Shader_Define.hlsl"

// ================================================================================================
// 젠레스 존 제로 - 모독자 해일 셰이더
// ================================================================================================

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

// ── 구조체 ──────────────────────────────────────────────────────
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

// ================================================================================================
// Vertex Shader
// ================================================================================================
VS_OUT VS_TESS(VS_IN In)
{
    VS_OUT Out;
    Out.vLocalPos = In.vPosition;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), ObjectBufferArray[TransformIndex].Transform).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = In.vNormal;
    return Out;
}

// ================================================================================================
// Hull Shader
// ================================================================================================
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

// ================================================================================================
// 노이즈
// ================================================================================================
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

// ================================================================================================
// Domain Shader
// ================================================================================================
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

    float2 noiseCoord = float2(worldPos.x * g_NoiseFreqX, worldPos.z * g_NoiseFreqZ + g_Time * g_NoiseSpeed);
    float topNoise = fbm(noiseCoord, 4) * 2.0 - 1.0;
    height += topNoise * g_TopNoiseScale * topRegion * envelope * xProfile;

    float2 surfNoiseCoord = float2(worldPos.x * 0.15, worldPos.z * 0.15 + g_Time * 2.0);
    float surfNoise = (valueNoise(surfNoiseCoord) - 0.5) * 2.0;
    height += surfNoise * g_SurfaceNoise * wallProfile * envelope * xProfile;

    float2 bumpCoord = float2(worldPos.x * 0.02, g_Time * 0.3);
    float largeBump = pow(valueNoise(bumpCoord), 2.0) * 7.5;
    height += largeBump * topRegion * envelope * xProfile;

    // 컬
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

    // 법선
    float eps = 0.8;
    float2 ncXp = float2((worldPos.x + eps) * g_NoiseFreqX, worldPos.z * g_NoiseFreqZ + g_Time * g_NoiseSpeed);
    float2 ncXm = float2((worldPos.x - eps) * g_NoiseFreqX, worldPos.z * g_NoiseFreqZ + g_Time * g_NoiseSpeed);
    float hXp = fbm(ncXp, 4) * g_TopNoiseScale * topRegion * envelope;
    float hXm = fbm(ncXm, 4) * g_TopNoiseScale * topRegion * envelope;
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
    Out.fFoamMask = saturate(heightForShading / (g_TsunamiHeight * 0.85)) * smoothstep(0.6, 0.95, topRegion);
    Out.fHeightPct = saturate(heightForShading / g_TsunamiHeight);
    Out.fPhase = t;

    return Out;
}

float4 PS_TESS(DS_OUT In) : SV_TARGET
{
    float3 viewDir = normalize(vCamPosition.xyz - In.vWorldPos.xyz);
    float h = In.fHeightPct;
    float foam = In.fFoamMask;

    // ── 메쉬 UV 텍스처 ──────────────────────────────────────────
    float2 meshUV = In.vTexcoord;
    float2 worldOffset = In.vWorldPos.xz * 0.001;

    float2 uv1 = meshUV * 0.5 + worldOffset + float2(g_Time * 0.02, g_Time * 0.05);
    float2 uv2 = meshUV * 0.8 + worldOffset * 1.3 - float2(g_Time * 0.03, g_Time * 0.06);
    float2 uv3 = meshUV * 1.5 + worldOffset * 0.7 + float2(g_Time * 0.04, -g_Time * 0.03);
    float2 uv4 = meshUV * 0.25 + worldOffset * 0.5 + float2(g_Time * 0.01, g_Time * 0.02);

    float tex1 = DiffuseTexture.Sample(LinearSampler, uv1).r;
    float tex2 = DiffuseTexture.Sample(LinearSampler, uv2).r;
    float tex3 = LightTexture.Sample(LinearSampler, uv3).r;
    float tex4 = DiffuseTexture.Sample(LinearSampler, uv4).r;
    float tex5 = NormalTexture.Sample(LinearSampler, uv1 * 1.5 + 0.3).r;
    float tex6 = MetalnessTexture.Sample(LinearSampler, uv2 * 0.7 + 0.5).r;

    // ── 노멀 마스크 (보조 역할만) ───────────────────────────────
    float flatness = saturate(In.vNormal.y);
    float curlMask = 1.0 - flatness;
    curlMask = smoothstep(0.3, 0.8, curlMask);



    // 흐름 패턴 A: 큰 스케일의 밝고 어두운 영역
    float flowA = tex1 * 0.5 + tex4 * 0.5;

    // 흐름 패턴 B: 중간 스케일
    float flowB = tex2 * 0.6 + tex5 * 0.4;

    // 흐름 패턴 C: 작은 디테일
    float flowC = tex3 * 0.5 + tex6 * 0.5;

    // 메인 흐름: A와 B를 곱해서 겹치는 부분만 밝게
    float mainFlow = flowA * flowB;
    // 0~1 → 리맵: 어두운 쪽은 날리고 밝은 쪽을 키움
    mainFlow = smoothstep(0.15, 0.55, mainFlow);

    // 디테일 흐름: 작은 줄기
    float detailFlow = flowA * flowC;
    detailFlow = smoothstep(0.2, 0.5, detailFlow);

    // 넓은 명암 (전체적인 밝고 어두운 영역)
    float broadShade = smoothstep(0.25, 0.65, flowA);

    // ── 최종 밝기 마스크 (텍스처 기반 + 노멀 보조) ────────────────
    // 텍스처가 80%, 노멀이 20%
    float brightMask = mainFlow * 0.6 + detailFlow * 0.2 + curlMask * 0.2;
    brightMask = saturate(brightMask);

    // 베이스: 검정이 아닌 어두운 마룬/자주 (레퍼런스의 기본 톤)
    // 톤매핑 후 아주 어두운 마젠타가 됨
    float3 colorDark = float3(0.008, 0.001, 0.006); // 가장 어두운 골
    float3 colorMaroon = float3(0.025, 0.003, 0.018); // 어두운 마룬
    float3 colorMid = float3(0.08, 0.008, 0.055); // 중간 자주

    // 넓은 명암으로 베이스 색 결정
    float3 baseColor = lerp(colorDark, colorMaroon, broadShade);

    // 흐름 패턴으로 중간톤 영역 추가
    baseColor = lerp(baseColor, colorMid, mainFlow * 0.5);

    // 디테일 줄기로 약간 더 밝은 라인
    baseColor += float3(0.03, 0.003, 0.02) * detailFlow * 0.4;

    // ── 밝은 영역 (HDR 발광) ────────────────────────────────────
    // 레퍼런스의 밝은 핑크 영역: 텍스처 흐름이 강한 곳
    float3 glowColor = float3(0.8, 0.05, 0.55); // 밝은 마젠타
    float3 glowHot = float3(2.0, 0.15, 1.4); // 핫 핑크 (가장 밝은 곳)

    // 밝기 마스크가 높은 곳에 발광
    float glowIntensity = smoothstep(0.3, 0.8, brightMask);
    float3 glow = lerp(glowColor, glowHot, glowIntensity);
    baseColor += glow * glowIntensity;

    // 컬 영역에서 추가 부스트 (노멀이 기울어진 곳 + 텍스처 밝은 곳)
    float curlBoost = curlMask * mainFlow;
    baseColor += float3(0.5, 0.03, 0.35) * curlBoost;

    // ── 프레넬 ──────────────────────────────────────────────────
    float NdotV = saturate(dot(In.vNormal, viewDir));
    float fresnel = pow(1.0 - NdotV, 3.0);
    baseColor += float3(0.1, 0.005, 0.07) * fresnel * brightMask * 0.3;

    // ── 높이 페이드 ─────────────────────────────────────────────
    baseColor *= smoothstep(0.0, 0.08, h);

    return float4(baseColor, 1.0);
}
// ================================================================================================
// Technique
// ================================================================================================
technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_TESS();
        HullShader = compile hs_5_0 HS_TESS();
        DomainShader = compile ds_5_0 DS_TESS();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TESS();
    }
}

#include "Shader_Define.hlsl"

// ================================================================================================
// 젠레스 존 제로 - 모독자 해일 셰이더
// 
// 동작 순서 (한 사이클):
//   Phase 0: 대기 (바닥 평평)
//   Phase 1: 뒤쪽에서 벽처럼 솟아오름 (울퉁불퉁한 상단)
//   Phase 2: 최고점 도달, 꼭대기가 앞으로 말리기 시작
//   Phase 3: 앞으로 쏟아지며 덮침
//   Phase 4: 착지 후 잔파
//
// 메시: XZ 평면 (Y = up), texcoord.y = 0이 카메라 쪽, 1이 뒤쪽
// ================================================================================================

// ── 조절 가능한 파라미터 ──────────────────────────────────────────
static const float g_TsunamiHeight = 38.0f; // 해일 최대 높이 (살짝 더 낮춤)
static const float g_CycleTime = 7.0f; // 한 사이클 총 시간 (초)

// 애니메이션 곡선 제어
static const float g_PeakTime = 0.42f; // 최고점 시점 (0~1 사이클 내)
static const float g_RiseWidth = 0.35f; // 올라가는 폭 (넓을수록 천천히)
static const float g_FallWidth = 0.12f; // 내려가는 폭 (더 빠르게 덮침)

// 벽 형상 제어
static const float g_WallDepthStart = 0.30f; // 이 depth 이후부터 벽이 솟음 (0=카메라쪽, 1=뒤쪽)
static const float g_WallDepthEnd = 0.70f; // 이 depth에서 벽이 최고 높이
static const float g_CurlForward = 45.0f; // 말릴 때 앞으로 밀리는 거리 (강화)
static const float g_CurlDrop = 40.0f; // 말릴 때 아래로 떨어지는 거리 (강화)
static const float g_CrashForward = 65.0f; // 덮칠 때 앞으로 밀리는 거리 (강화)
static const float g_CrashDrop = 70.0f; // 덮칠 때 아래로 떨어지는 거리 (강화)

// 노이즈/울퉁불퉁 제어
static const float g_TopNoiseScale = 6.0f; // 꼭대기 불규칙 높이
static const float g_SurfaceNoise = 2.0f; // 표면 울퉁불퉁함
static const float g_NoiseFreqX = 0.04f; // X방향 노이즈 주파수
static const float g_NoiseFreqZ = 0.08f; // Z방향 노이즈 주파수
static const float g_NoiseSpeed = 1.5f; // 노이즈 흐름 속도

float g_Time;

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
// Vertex Shader: 테셀레이션 전 데이터 패스스루
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
// Hull Shader: 테셀레이션 레벨 (최대 64)
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
// 유틸리티: 노이즈 함수들
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

// FBM (Fractal Brownian Motion) - 여러 옥타브의 노이즈를 겹쳐서 자연스러운 형태 생성
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
// Domain Shader: [핵심] 해일 형상 생성
// ================================================================================================
struct DS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vWorldPos : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
    float3 vNormal : NORMAL;
    float fFoamMask : TEXCOORD3;
    float fHeightPct : TEXCOORD4; // 현재 높이 비율 (셰이딩용)
    float fPhase : TEXCOORD5; // 현재 페이즈 (PS에서 활용)
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
    float topRegion = smoothstep(0.6, 1.0, wallProfile);
    
    float xCenter = abs(texcoord.x - 0.5) * 2.0;
    float xProfile = 1.0 - pow(xCenter, 2.5) * 0.55;
    xProfile = max(xProfile, 0.15);

    float peakTime = g_PeakTime;
    float riseWidth = g_RiseWidth;
    float fallWidth = g_FallWidth;
    
    float risePart = exp(-pow((t - peakTime) / riseWidth, 2.0));
    float fallPart = exp(-pow((t - peakTime) / fallWidth, 2.0));
    float blendWidth = 0.03;
    float blendFactor = smoothstep(peakTime - blendWidth, peakTime + blendWidth, t);
    float envelope = lerp(risePart, fallPart, blendFactor);
    envelope *= smoothstep(0.0, 0.08, t);
    envelope *= smoothstep(1.0, 0.93, t);

    float height = g_TsunamiHeight * envelope * wallProfile * xProfile;
    
    float2 noiseCoord = float2(worldPos.x * g_NoiseFreqX, worldPos.z * g_NoiseFreqZ + g_Time * g_NoiseSpeed);
    float topNoise = fbm(noiseCoord, 4) * 2.0 - 1.0;
    height += topNoise * g_TopNoiseScale * topRegion * envelope * xProfile;

    float2 surfNoiseCoord = float2(worldPos.x * 0.15, worldPos.z * 0.15 + g_Time * 2.0);
    float surfNoise = (valueNoise(surfNoiseCoord) - 0.5) * 2.0;
    height += surfNoise * g_SurfaceNoise * wallProfile * envelope * xProfile;

    float2 bumpCoord = float2(worldPos.x * 0.02, g_Time * 0.3);
    float largeBump = pow(valueNoise(bumpCoord), 2.0) * 7.5;
    height += largeBump * topRegion * envelope * xProfile;

    float spatialRatio = saturate(wallProfile * xProfile / 0.8);
    float topFocus = saturate((spatialRatio - 0.25) / 0.75);
    float curlMask = pow(topFocus, 3.5);

    float overPeak = saturate((t - peakTime * 0.82) / (1.0 - peakTime * 0.82));
    float curlCrash = overPeak * overPeak * overPeak * (10.0 - 15.0 * overPeak + 6.0 * overPeak * overPeak);
    curlCrash = min(curlCrash, 0.7);

    float forwardTemporal = max(envelope, curlCrash);
    float riseLean = forwardTemporal * curlMask * g_CurlForward * 0.5;
    worldPos.z -= riseLean * edgeMask * xProfile;
    worldPos.z -= curlCrash * curlMask * g_CrashForward * 1.0 * edgeMask * xProfile;

    float heightBeforeDrop = height;
    float curlDropAmount = curlCrash * (g_CurlDrop + g_CrashDrop) * 0.5;
    float riseCurlDrop = forwardTemporal * g_CurlDrop * 0.2;
    float totalDrop = max(curlDropAmount, riseCurlDrop);
    height -= min(totalDrop * curlMask * xProfile, height * 0.6);

    height = max(height, 0.0);
    float finalHeight = height * edgeMask;
    worldPos.y += finalHeight;

    float eps = 0.5;
    
    float2 ncX = float2((worldPos.x + eps) * g_NoiseFreqX, worldPos.z * g_NoiseFreqZ + g_Time * g_NoiseSpeed);
    float hX = fbm(ncX, 4) * g_TopNoiseScale * topRegion * envelope;
    float dhdx = (hX - topNoise * g_TopNoiseScale * topRegion * envelope) / eps;
    
    float dhdz = wallProfile > 0.01 ? (finalHeight / (depth * 100.0 + 1.0)) : 0.0;
    
    float3 normal = normalize(float3(-dhdx, 1.0, -dhdz));
    float curlNormalBlend = (riseLean + curlCrash) * curlMask;
    normal = normalize(lerp(normal, float3(0, 0, -1), saturate(curlNormalBlend) * 0.5));

    // 출력
    Out.vWorldPos = float4(worldPos, 1.0);
    Out.vPosition = mul(float4(worldPos, 1.0), matView);
    Out.vPosition = mul(Out.vPosition, matProjection);
    Out.vTexcoord = texcoord;
    Out.vNormal = normal;
    Out.fFoamMask = saturate((heightBeforeDrop * edgeMask) / (g_TsunamiHeight * 0.5)) * topRegion;
    Out.fHeightPct = saturate(finalHeight / g_TsunamiHeight);
    Out.fPhase = t;

    return Out;
}

// ================================================================================================
// Pixel Shader: 깊은 물 + 거품 + 프레넬 셰이딩
// ================================================================================================
float4 PS_TESS(DS_OUT In) : SV_TARGET
{
    float3 viewDir = normalize(vCamPosition.xyz - In.vWorldPos.xyz);
    float3 lightDir = normalize(float3(0.3, 1.0, -0.3));

    // ── UV 애니메이션 ──
    float2 flowUV = In.vTexcoord * 8.0f + float2(g_Time * 0.08, g_Time * 0.05);
    float2 flowUV2 = In.vTexcoord * 12.0f - float2(g_Time * 0.06, g_Time * 0.03);

    // ── 텍스처 샘플링 ──
    float3 surfColor = DiffuseTexture.Sample(LinearSampler, flowUV).rgb;

    // 노멀맵 블렌딩 (3장 겹쳐서 거친 수면)
    float3 n1 = NormalTexture.Sample(LinearSampler, flowUV * 1.5).rgb * 2.0 - 1.0;
    float3 n2 = MetalnessTexture.Sample(LinearSampler, flowUV2 * 0.8).rgb * 2.0 - 1.0;
    float3 n3 = AmbientTexture.Sample(LinearSampler, flowUV * 2.5 + g_Time * 0.12).rgb * 2.0 - 1.0;
    float3 combinedNormal = normalize(In.vNormal + (n1 + n2 + n3) * 0.25);

    // 거품 텍스처
    float foamTex = LightTexture.Sample(LinearSampler, flowUV * 0.4).r;
    float foamTex2 = LightTexture.Sample(LinearSampler, flowUV2 * 0.3).r;
    float combinedFoam = max(foamTex, foamTex2);
    float finalFoamMask = smoothstep(0.3, 0.6, combinedFoam * In.fFoamMask);

    // ── 색상 ──
    // 깊이에 따른 그라데이션: 바닥 = 아주 어둡고, 위 = 투명한 물색
    float3 deepColor = float3(0.00, 0.01, 0.03); // 심해 (거의 검정)
    float3 midColor = float3(0.02, 0.10, 0.18); // 중간층
    float3 shallowColor = float3(0.05, 0.25, 0.35); // 얕은 물 / 상단

    float heightBlend = In.fHeightPct;
    float3 waterColor = lerp(deepColor, midColor, smoothstep(0.0, 0.4, heightBlend));
    waterColor = lerp(waterColor, shallowColor, smoothstep(0.4, 0.8, heightBlend));
    
    // 텍스처와 블렌딩
    float3 baseColor = waterColor * (0.6 + surfColor * 0.4);

    // ── 거품 (흰색 물보라) ──
    float3 foamColor = float3(0.85, 0.90, 0.95);
    float3 finalColor = lerp(baseColor, foamColor, finalFoamMask);

    // ── 프레넬 (가장자리 빛남) ──
    float fresnel = pow(1.0 - saturate(dot(combinedNormal, viewDir)), 4.0);
    finalColor += fresnel * float3(0.15, 0.30, 0.40) * heightBlend;

    // ── 스펙큘러 하이라이트 ──
    float3 reflDir = reflect(-lightDir, combinedNormal);
    float spec = pow(saturate(dot(reflDir, viewDir)), 80.0);
    finalColor += spec * 0.5 * (1.0 - finalFoamMask) * heightBlend;

    // ── 덮침 중에는 약간 밝아지기 (충격파 느낌) ──
    // 피크 직후 ~ 내려갈 때 잠깐 밝아짐
    float crashBright = smoothstep(0.4, 0.55, In.fPhase) * smoothstep(0.75, 0.55, In.fPhase);
    finalColor += crashBright * float3(0.1, 0.15, 0.2) * In.fHeightPct;

    // ── 벽 안쪽 어두운 그림자 (두께감) ──
    float innerShadow = 1.0 - smoothstep(0.0, 0.3, heightBlend);
    finalColor *= (0.5 + innerShadow * 0.5);
    // 위 줄은 오히려 어둡게 만드니 반대로:
    // 높이가 낮은 부분(바닥)은 어둡게, 높은 부분(벽 상단)은 밝게
    finalColor *= (0.4 + 0.6 * heightBlend);

    return float4(finalColor, 1.0);
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

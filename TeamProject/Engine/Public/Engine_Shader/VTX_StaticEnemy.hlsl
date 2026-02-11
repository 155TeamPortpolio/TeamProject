#include "Shader_Define.hlsl"

float fUseVanish;
float3 vEmissiveColor;
float fEmissiveStrength;
float3 vRimLightColor;
vector vOutLineColor;

float fOutLineThickness;
float fRimLightPower;
float fTime;
float fDissolveProgress;

float fDissolveScrollSpeed;
float fDissolveNoiseStrength;
float fDissolveEdgeWidth;
float2 vDissolveTiling;
float3 pad;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float viewZ : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    matrix matWV, matWVP;
    
    float3 worldPos = mul(float4(In.vPosition, 1.f), ObjectBufferArray[TransformIndex].Transform).xyz;
    float4 viewPos = mul(float4(worldPos, 1.f), matView);
    float4 projPos = mul(viewPos, matProjection);

    Out.vPosition = projPos;
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = mul(vector(In.vNormal, 0.f), ObjectBufferArray[TransformIndex].Transform);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), ObjectBufferArray[TransformIndex].Transform)).xyz;
    Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
    Out.viewZ = viewPos.z;
    return Out;
}
VS_OUT VS_WALL(VS_IN In)
{
    VS_OUT Out;

    // ==========================
    // World / TBN
    // ==========================
    float3 worldPos = mul(float4(In.vPosition, 1.f), ObjectBufferArray[TransformIndex].Transform).xyz;

    float3 normalW = normalize(mul(float4(In.vNormal, 0.f), ObjectBufferArray[TransformIndex].Transform).xyz);
    float3 tangentW = normalize(mul(float4(In.vTangent, 0.f), ObjectBufferArray[TransformIndex].Transform).xyz);
    float3 binormW = normalize(cross(normalW, tangentW));

    float2 uvBase = In.vTexcoord;

    // ==========================
    // 1) 빨려들기 타이밍(suck)
    // ==========================
    const float cycleSpeed = 0.45f; // 1/초
    float phase = frac(fTime * cycleSpeed); // 0..1

    float suck = saturate((phase - 0.5f) * 2.0f); // 0..1 (뒤 절반에서 빨려듦)
    suck = suck * suck; // 가속 느낌

    // ==========================
    // 2) "끝이 먼저" 사라지게 (uv.y가 뿌리=0, 끝=1 가정)
    // ==========================
    float along = saturate(uvBase.y);

    // 끝에서부터 내려오는 범위 (0.15~0.40 추천)
    const float tipStart = 0.25f;

    // along이 1에 가까울수록 먼저 켜짐
    float tipMask = saturate((along - (1.0f - tipStart)) / tipStart);
    tipMask = tipMask * tipMask;

    // 끝쪽에만 먼저 강하게 적용되는 suck
    float suckTip = suck * tipMask;

    // ==========================
    // 3) 덩쿨 마스크(시간 스크롤 + 월드 기반 가닥 위상)
    // ==========================
    float2 uvNoise = uvBase * vDissolveTiling;
    uvNoise.y -= fTime * fDissolveScrollSpeed;
    uvNoise.x += fTime * (0.25f * fDissolveScrollSpeed);

    float vineShape = DissolveTexture.SampleLevel(LinearSampler, uvNoise, 0).r;
    vineShape = saturate((vineShape - 0.25f) * 1.6f);

    // 월드 XZ 기반 가닥 분리 위상
    const float strandSpacing = 0.8f; // 작을수록 가닥 많아짐
    float2 strandCoord = floor(worldPos.xz / strandSpacing);
    float hash = frac(sin(dot(strandCoord, float2(127.1f, 311.7f))) * 43758.5453f);
    float strandPhase = hash * 6.2831853f;

    // ==========================
    // 4) 휘감기(스크류) + 수축 + 아래로 빨려들기
    // ==========================
    const float twistTurns = 4.0f; // 길이 전체 회전 수(2~6)
    const float twistSpeed = 5.0f; // 시간 회전 속도(3~10)
    float angle = (along * 6.2831853f * twistTurns) + (fTime * twistSpeed) + strandPhase;

    float s, c;
    sincos(angle, s, c);

    // 끝쪽에서 먼저 감기고(=radius), 먼저 얇아지고(=thin), 먼저 내려감
    const float radiusBase = 0.10f; // 감김 반경(0.03~0.20)
    float radius = radiusBase * vineShape * suckTip;

    const float thinAmount = 0.06f; // 수축 강도(0.02~0.12)
    float thin = thinAmount * vineShape * suckTip;

    // 단면(노말/바이노멀)에서 원 운동 -> "줄기들이 휘감기며 내려오는" 착시
    worldPos += (normalW * c + binormW * s) * radius;

    // 가늘어짐(폭/노말 방향 수축)
    worldPos -= binormW * thin;
    worldPos -= normalW * (thin * 0.6f);

    // 아래로 빨려들기(끝쪽 먼저)
    const float pullDown = 2.5f; // 0.8~4.0
    worldPos.y -= suckTip * pullDown;

    // ==========================
    // Project
    // ==========================
    float4 viewPos = mul(float4(worldPos, 1.f), matView);
    float4 projPos = mul(viewPos, matProjection);

    Out.vPosition = projPos;
    Out.vProjPos = projPos;
    Out.vTexcoord = uvBase;

    Out.vNormal = float4(normalW, 0.f);
    Out.vTangent = tangentW;
    Out.vBinormal = binormW;

    Out.viewZ = viewPos.z;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float viewZ : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vMetalic : SV_TARGET3;
    vector vEmissive : SV_TARGET4;
    float2 fEmissiveInfo : SV_TARGET5;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.2)
    {
        discard;
    }
    Out.vDiffuse = vMtrlDiffuse;
    
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalic = MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vAmbient = AmbientTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fNoise = NoiseTexture.Sample(LinearSampler, In.vTexcoord * vDissolveTiling.x).r;
    if (fNoise < fDissolveProgress)
        discard;
  
    vAmbient.r = 0.f;
    
    float3 vEmissive = lerp(vMtrlDiffuse.rgb * 0.05f, vEmissiveColor, fEmissiveStrength);
    
    float3 vNormal;
    vNormal.xy = vNormalDesc.xy * 2.f - 1.f;
    vNormal.z = 1.f;
    float3 T = normalize(In.vTangent);
    float3 B = normalize(In.vBinormal * -1);
    float3 N = normalize(In.vNormal.xyz);

    float3x3 WorldMatrix = float3x3(T, B, N);
        
    vNormal = mul(vNormal, WorldMatrix);
    vMetalic.a = 0.6f;
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, vNormalDesc.z);
   
    if (vAmbient.g < 0.2)
        vAmbient.g = 1.f;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
   // Out.vAmbient = vAmbient;
    Out.vMetalic = vMetalic;
   // Out.vRimLight = float4(vRimLightColor, fRimLightPower);
    Out.vEmissive = float4(vEmissive, 1.f);
   // Out.vPostInfo = float4(fUseVanish, 0.f, 0.f, 0.f);
    return Out;
}


PS_OUT PS_WALL(PS_IN In)
{
    PS_OUT Out;

    vector vMtrlDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.2)
        discard;

    Out.vDiffuse = vMtrlDiffuse;

    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalic = MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vAmbient = AmbientTexture.Sample(DefaultSampler, In.vTexcoord);

    float2 uvBase = In.vTexcoord;

   //// progress 완만하게(프레임 점프 체감 감소)
   //float progress = saturate(fDissolveProgress);
   //progress = progress * progress * (3.0f - 2.0f * progress);
   //
   //float2 uvNoise = uvBase ;
   //uvNoise.x += fTime * 5.2f * fDissolveScrollSpeed;
   //uvNoise.y += fTime * 2.0f * fDissolveScrollSpeed;
   //float vineShape = DissolveTexture.Sample(LinearSampler, uvNoise).r;
   //
   //clip(vineShape - 0.01f);

    // 이하 기존 유지
    vAmbient.r = 0.f;

    float3 vEmissive = lerp(vMtrlDiffuse.rgb * 0.05f, vEmissiveColor, fEmissiveStrength);

    float3 vNormal;
    vNormal.xy = vNormalDesc.xy * 2.f - 1.f;
    vNormal.z = 1.f;

    float3 tangent = normalize(In.vTangent);
    float3 binormal = normalize(In.vBinormal * -1);
    float3 normalBase = normalize(In.vNormal.xyz);

    float3x3 tbn = float3x3(tangent, binormal, normalBase);
    vNormal = mul(vNormal, tbn);

    vMetalic.a = 0.6f;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, vNormalDesc.z);

    if (vAmbient.g < 0.2)
        vAmbient.g = 1.f;

    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
    Out.vMetalic = vMetalic;
    Out.vEmissive = float4(vEmissive, 1.f);

    return Out;
}
struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    //float4 vProjPos : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    float3 worldPos = mul(float4(In.vPosition, 1.f),
                         ObjectBufferArray[TransformIndex].Transform).xyz;
    
    float4 lightSpacePos = mul(float4(worldPos, 1.f), matStaticLightViewProj[iCurrentCascade]);
    Out.vPosition = lightSpacePos;
    
    return Out;
}

struct PS_IN_SHDOW
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT_SHADOW
{
   //vector vShadow : SV_TARGET0;
};

void PS_MAIN_SHADOW(PS_IN_SHDOW In)
{
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }  
    pass Wall
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_WALL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WALL();
    }  
    pass Shadow
    {
        SetRasterizerState(RS_Shadow);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
}


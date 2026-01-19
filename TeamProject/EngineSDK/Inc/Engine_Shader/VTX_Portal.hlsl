#include "Shader_Define.hlsl"

float g_Time;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    matrix matWV, matWVP;

    float3 objectWorld = mul(float4(0, 0, 0, 1), ObjectBufferArray[TransformIndex].Transform).xyz;
    
    float2 worldScale;
    worldScale.x = length(ObjectBufferArray[TransformIndex].Transform._11_12_13);
    worldScale.y = length(ObjectBufferArray[TransformIndex].Transform._21_22_23);

    float3 camRight = normalize(matViewInverse._11_12_13); // X축
    float3 camUp = normalize(matViewInverse._21_22_23); // Y축
    float3 camForward = normalize(matViewInverse._31_32_33); // Z축

    // 정점 위치(로컬 quad 좌표) 적용
    float3 worldPos = objectWorld + camRight * In.vPosition.x * worldScale.x + camUp * In.vPosition.y * worldScale.y;

    float4 vPos = mul(float4(worldPos, 1.f), matView);
    Out.vPosition = mul(vPos, matProjection);

    // 텍스처 좌표
    Out.vTexcoord = In.vTexcoord;

    // Normal은 카메라를 향하도록 고정 (조명 넣는다면 기존 메시 normals 불필요)
    Out.vNormal = float4(normalize(-camForward), 0.f);

    Out.vProjPos = Out.vPosition;
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float2 center = float2(0.5f, 0.5f);
    float2 toCenter = In.vTexcoord - center;
    float2 direction = normalize(toCenter);
    
    // === 시간에 따라 노이즈 UV 이동 ===
    
    float2 noiseUV1 = In.vTexcoord * 2.0f + float2(g_Time * 0.05f, g_Time * 0.03f);
    float noise1 = NoiseTexture.Sample(LinearSampler, noiseUV1).r;
    
    float2 noiseUV2 = In.vTexcoord * 3.5f - float2(g_Time * 0.08f, g_Time * 0.05f);
    float noise2 = NoiseTexture.Sample(LinearSampler, noiseUV2).r;
    
    float2 noiseUV3 = In.vTexcoord * 6.0f + float2(g_Time * 0.12f, -g_Time * 0.1f);
    float noise3 = NoiseTexture.Sample(LinearSampler, noiseUV3).r;
    
    noise1 = pow(noise1, 0.5f);
    noise2 = pow(noise2, 0.7f);
    
    float combinedNoise = noise1 * 0.6f + noise2 * 0.3f + noise3 * 0.1f;
    
    float distortStrength = 0.25f;
    float2 distortedUV = In.vTexcoord + direction * (combinedNoise - 0.5f) * distortStrength;
    
    // 회전도 시간에 따라
    float angle = (combinedNoise - 0.5f) * 0.3f + g_Time * 0.2f;
    float cosA = cos(angle);
    float sinA = sin(angle);
    float2 rotated = distortedUV - center;
    distortedUV = float2(
        rotated.x * cosA - rotated.y * sinA,
        rotated.x * sinA + rotated.y * cosA
    ) + center;
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(DefaultSampler, distortedUV);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = float4(vMtrlDiffuse.rgb, 1.f);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}


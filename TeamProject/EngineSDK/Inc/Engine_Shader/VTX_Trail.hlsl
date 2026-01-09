#include "Shader_Define.hlsl"

float2 UVOffset;
uint ColorMode;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vColor : TEXCOORD2;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vColor : TEXCOORD2;
    float4 vViewPosition : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float3 worldPos = In.vPosition;
    float4 viewPos = mul(float4(worldPos, 1.f), matView);
    float4 projPos = mul(viewPos, matProjection);
    
    Out.vPosition = projPos;
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    Out.vColor = In.vColor;
    Out.vViewPosition = viewPos;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vColor : TEXCOORD2;
    float4 vViewPosition : TEXCOORD3;
};

struct PS_OUT
{
    float4 vDiffuseAcc : SV_Target0;
    float4 vBloomAcc : SV_Target1;
    float4 vBloomInfo : SV_Target2;
    float4 vRevealage : SV_Target3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = In.vTexcoord + UVOffset;
    
    float4 vDiffuse = DiffuseTexture.Sample(LinearSampler, vTexcoord);
    float4 vResult = ApplyColorMode(ColorMode, vDiffuse, In.vColor);
    float3 vColor = vResult.rgb;
    float fAlpha = vResult.a * (1.f - In.vLifeTime.x / In.vLifeTime.y) * max(vDiffuse.b, max(vDiffuse.r, vDiffuse.g));
    
     /* 깊이 기반 가중치 생성 */
    float fLinearZ = In.vViewPosition.z;
    float fDepthBias = 1.f / (1.f + fLinearZ * fLinearZ);
    float fWeight = clamp((fAlpha * 4.f + 0.01f) * fDepthBias, 0.01f, 1.f);
    float4 vPremulColor = float4(vColor * fAlpha, fAlpha);
    
    Out.vDiffuseAcc = vPremulColor * fWeight;
    Out.vBloomAcc = SoftExtractBright(vPremulColor, 0.8f, 0.5f, 1.f) * fWeight;
    Out.vBloomInfo = float4(0.f, 1.f, 0.f, 0.f);
    Out.vRevealage = float4(fAlpha, fAlpha, fAlpha, fAlpha);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_ReadOnly, 0);
        SetBlendState(BS_OITAccmulation, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }  
}


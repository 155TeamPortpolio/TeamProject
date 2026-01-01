#include "Shader_Define.hlsl"

matrix g_WorldMatrix;

Texture2D g_DiffuseEffectAccTexture;
Texture2D g_BloomEffectAccTexture;
Texture2D g_RevealageTexture;

BlendState BS_OITComposite
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = One;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
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
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_COMPOSITE
{
    float4 vDiffuseEffect : SV_Target0;
    float4 vBloomEffect : SV_Target1;
};

PS_OUT_COMPOSITE PS_MAIN_COMPOSITE(PS_IN In)    //여기서 가중치 합성 후 원래 타겟에 다시 쓰기
{
    PS_OUT_COMPOSITE Out;
    
    float4 vDiffuseEffectDesc = g_DiffuseEffectAccTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vBloomEffectDesc = g_BloomEffectAccTexture.Sample(LinearSampler, In.vTexcoord);
    float fRevealage = g_RevealageTexture.Sample(LinearSampler, In.vTexcoord).r;
    
    float fElipson = 0.00001f;
    
    /* Diffuse */
    Out.vDiffuseEffect.rgb = vDiffuseEffectDesc.rgb;
    Out.vDiffuseEffect.a = saturate(1.f - fRevealage);
    
    /* Bloom */
    Out.vBloomEffect.rgb = vBloomEffectDesc.rgb;
    Out.vBloomEffect.a = saturate(1.f - fRevealage);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Composite
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OITComposite, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMPOSITE();
    }
}

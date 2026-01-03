#include "Shader_Deferred_Define.hlsl"

matrix g_WorldMatrix;

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
    
    float4 vDiffuseEffectDesc = EffectAccTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vBloomEffectDesc = EffectBloomAccTextutre.Sample(LinearSampler, In.vTexcoord);
    float fRevealage = RevealageTexture.Sample(LinearSampler, In.vTexcoord).r;
    
    float fElipson = 0.00001f;
    
    /* Diffuse */
    Out.vDiffuseEffect.rgb = vDiffuseEffectDesc.rgb;
    Out.vDiffuseEffect.a = saturate(1.f - fRevealage);
    
    /* Bloom */
    Out.vBloomEffect.rgb = vBloomEffectDesc.rgb;
    Out.vBloomEffect.a = saturate(1.f - fRevealage);
    
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

struct PS_OUT_RESULT
{
    float4 vResult : SV_Target0;
};

PS_OUT_RESULT PS_BLOOM_BLURX(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vBloomInfo = EffectBloomInfo.Sample(DefaultSampler, In.vTexcoord);
    
    float bloomType = vBloomInfo.r;
    float bloomStrength = vBloomInfo.g;

    float4 bright = EffectBrightTexture.Sample(DefaultSampler, In.vTexcoord);

    if (bloomType < 0.5f) // Gaussian
    {
        float3 result = bright.rgb * weights[0];
        float texelSize = bloomStrength / fScreenWidth;
        
        for (int i = 1; i < 9; ++i)
        {
            float4 brightSample = EffectBrightTexture.Sample(DefaultSampler,
                In.vTexcoord + float2(texelSize * i, 0));
            result += brightSample.rgb * weights[i];
            
            brightSample = EffectBrightTexture.Sample(DefaultSampler,
                In.vTexcoord - float2(texelSize * i, 0));
            result += brightSample.rgb* weights[i];
        }
        
        Out.vResult = float4(result, 1.f);
    }
    else
    {
        Out.vResult = bright;
    }
    
    
    return Out;
}

PS_OUT_RESULT PS_BLOOM_BLURY(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vBloomInfo = EffectBloomInfo.Sample(DefaultSampler, In.vTexcoord);
    
    float4 blurX = EffectBlurXTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float bloomType = vBloomInfo.r;
    float bloomStrength = vBloomInfo.g;
    float2 RadialCenter = vBloomInfo.ba;

    if (bloomType < 0.5f) // Gaussian
    {
        float3 result = blurX.rgb * weights[0];
        float texelSize = bloomStrength / fScreenHeight;
        
        for (int i = 1; i < 9; ++i)
        {
            result += EffectBlurXTexture.Sample(DefaultSampler,
                In.vTexcoord + float2(0, texelSize * i)).rgb * weights[i];
            result += EffectBlurXTexture.Sample(DefaultSampler,
                In.vTexcoord - float2(0, texelSize * i)).rgb * weights[i];
        }
        
        Out.vResult = float4(result, 1.f);
    }
    else // Radial Blur
    {
        float2 dir = In.vTexcoord - RadialCenter;
        float dist = length(dir);
        dir = normalize(dir);
        
        float3 result = float3(0, 0, 0);
        float samples = 15.0f;
        float strength = 0.1f;
        
        for (float i = 0; i < samples; i++)
        {
            float offset = (i / samples) * strength * dist;
            result += EffectBlurXTexture.Sample(DefaultSampler,
                In.vTexcoord - dir * offset).rgb;
        }
        
        Out.vResult = float4(result / samples, blurX.a);
    }
    
    return Out;
}

PS_OUT_RESULT PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vBloom = EffectBloomFinalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vResult = vDiffuse + vBloom;
    vResult.a = vDiffuse.a;
    
    Out.vResult = vResult;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass COMPOSITE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OITComposite, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMPOSITE();
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

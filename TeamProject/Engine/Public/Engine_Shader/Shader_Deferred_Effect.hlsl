#include "Shader_Deferred_Define.hlsl"

matrix g_WorldMatrix;
float BloomScreenWidth;
float BloomScreenHeight;

float RimStrength = 0.3; // 0~ 몇
float RimEdgeScale = 20; // 120~300 추천
float RimEdgePower = 2.5; // 1.5~2.5
float RimSampleOffset = 2; // 1~3 (픽셀 단위)
float RimBloomMul = 1; // 1~3

BlendState BS_OITComposite
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = One;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;

    SrcBlendAlpha = One;
    DestBlendAlpha = Inv_Src_Alpha;
    BlendOpAlpha = Add;
};

float3 BoostBrightColor(float3 c, float boost, float start)
{
    float lum = dot(c, float3(0.2126, 0.7152, 0.0722));
    float t = saturate((lum - start) / (1.0 - start)); // start=0.3~0.6
    return c * (1.0 + boost * t); // boost=0.3~1.0
}

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
    
    float fElipson = 1e-8;
    float fDiffuseAlpha = vDiffuseEffectDesc.a;
    float fBloomAlpha = vBloomEffectDesc.a;
    float fOutAlpha = saturate(1.f - fRevealage);
    
    /* Diffuse */
    float3 vDiffuseColor = (fDiffuseAlpha > fElipson) ? (vDiffuseEffectDesc.rgb / fDiffuseAlpha) : 0.f;
    vDiffuseColor = BoostBrightColor(vDiffuseColor, 0.5f, 1.f);
    Out.vDiffuseEffect = float4(vDiffuseColor * fOutAlpha, fOutAlpha);
    
    /* Bloom */
    float3 vBloomColor = (fBloomAlpha > fElipson) ? (vBloomEffectDesc.rgb / fBloomAlpha) : 0.f;
    Out.vBloomEffect = float4(vBloomColor * fOutAlpha, fOutAlpha);
    
    /* RimLight */
    float eps = 1e-6;

// inside mask
    float mask = saturate(1.0 - fRevealage);

    float2 texel = float2(1.0 / BloomScreenWidth, 1.0 / BloomScreenHeight);

// ---- band thickness in pixels (outline band itself) ----
    float BandPx = 1.0; // 1~4 추천

    float2 o1 = float2(texel.x, 0) * BandPx;
    float2 o2 = float2(0, texel.y) * BandPx;

// erosion (shrink)
    float m0 = mask;
    float mL = saturate(1.0 - RevealageTexture.Sample(LinearSampler, In.vTexcoord - o1).r);
    float mR = saturate(1.0 - RevealageTexture.Sample(LinearSampler, In.vTexcoord + o1).r);
    float mD = saturate(1.0 - RevealageTexture.Sample(LinearSampler, In.vTexcoord - o2).r);
    float mU = saturate(1.0 - RevealageTexture.Sample(LinearSampler, In.vTexcoord + o2).r);

// diagonals (추가!)
    float mDL = saturate(1.0 - RevealageTexture.Sample(LinearSampler, In.vTexcoord - o1 - o2).r);
    float mDR = saturate(1.0 - RevealageTexture.Sample(LinearSampler, In.vTexcoord + o1 - o2).r);
    float mUL = saturate(1.0 - RevealageTexture.Sample(LinearSampler, In.vTexcoord - o1 + o2).r);
    float mUR = saturate(1.0 - RevealageTexture.Sample(LinearSampler, In.vTexcoord + o1 + o2).r);

// 8-neighborhood erosion
    float eroded = min(m0, min(min(mL, mR), min(mD, mU)));
    eroded = min(eroded, min(min(mDL, mDR), min(mUL, mUR)));
    
// band = only boundary ring (interior becomes 0)
    float band = saturate(mask - eroded);

// tiny AA  (band 기준으로 하는게 더 안정적이라 이쪽 추천)
    float bw = max(fwidth(band), eps);
    band = smoothstep(0.0, bw * 2.0, band);

// ===== glow color =====
    float3 glowColor = RimLightAccTexture.Sample(LinearSampler, In.vTexcoord).rgb;

// add strength
    float GlowAdd = 2.0; // LDR: 0.2~3 / HDR: 1~10
    float3 glow = glowColor * band * RimStrength * GlowAdd;

// ----------------------------------------------------
// A) Straight alpha 파이프라인이면 (일반적인 Out.vDiffuse)
// ----------------------------------------------------
    Out.vDiffuseEffect.rgb += glow;
    return Out;
}

//GaussianBlur
static const float weights[13] =
{
    0.0999083581,
    0.0968345011,
    0.0881688166,
    0.0754147853,
    0.0605974824,
    0.0457413795,
    0.0324354950,
    0.0216066977,
    0.0135211259,
    0.0079486599,
    0.0043896669,
    0.0022773292,
    0.0011098816
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
        float texelSize = bloomStrength / BloomScreenWidth;
        
        for (int i = 1; i < 13; ++i)
        {
            float4 brightSample = EffectBrightTexture.Sample(LinearSampler,
                In.vTexcoord + float2(texelSize * i, 0));
            result += brightSample.rgb * weights[i];
            
            brightSample = EffectBrightTexture.Sample(LinearSampler,
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
        float texelSize = bloomStrength / BloomScreenHeight;
        
        for (int i = 1; i < 13; ++i)
        {
            result += EffectBlurXTexture.Sample(LinearSampler,
                In.vTexcoord + float2(0, texelSize * i)).rgb * weights[i];
            result += EffectBlurXTexture.Sample(LinearSampler,
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
  
    Out.vResult.rgb = vDiffuse.rgb + vBloom.rgb;
    Out.vResult.a = vDiffuse.a;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass COMPOSITE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
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

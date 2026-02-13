#include "Shader_Deferred_Define.hlsl"

matrix g_WorldMatrix;

float   HDRIntensity;

float   RadialIntensity;
float2  RadialCenter;

float   FogDensity;
float4  FogColor;

float   GlitchIntensity;

float   g_Time;
float3  AddictiveColor;

float ScreenWidth;
float ScreenHeight;

float   GuassianIntensity;
float   SaturationIntensity;

bool    bSaturateStaticUse;
bool    bSaturateSkinnedUse;
bool    bSaturateEffectUse;

bool    bSkinned = false;

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

struct PS_OUT_BACKBUFFER
{
    vector vBackBuffer : SV_TARGET0;
};

struct PS_OUT_LIGHT
{
    vector vLight : SV_TARGET0;
    float2 fLightInfo : SV_TARGET1;
};

struct PS_OUT_RESULT
{
    vector vResult : SV_TARGET0;
};

PS_OUT_RESULT PS_FOG(PS_IN In)
{
    PS_OUT_RESULT Out;

    float4 vStaticDepthDesc = StaticDepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vSkinnedDepthDesc = SkinnedDepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vScene = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vDepthDesc = (vStaticDepthDesc.x > 0.0001f) ? vStaticDepthDesc : vSkinnedDepthDesc;
    float fViewZ = vDepthDesc.y * zFar;
    
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float3 vViewDir = vWorldPos.xyz - vCamPosition.xyz;
    float fDistance = length(vViewDir);
    
    float fFogFactor = exp(-FogDensity * fDistance);
    
    fFogFactor = saturate(fFogFactor);
    fFogFactor = lerp(0.3f, 1.0f, fFogFactor);
    
    float4 vFoggedColor = lerp(FogColor, vScene, fFogFactor);
 
    Out.vResult = vFoggedColor;
    
    return Out;
}

PS_OUT_RESULT PS_HDR_BRIGHTPASS(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 scene = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 effect = EffectCombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 sceneBright = SoftExtractBright(scene);
    float4 effectBright = SoftExtractBright(effect, 0.5f, 0.5f, 2.f);
    
    float4 bright = effectBright + sceneBright;
    
    Out.vResult = bright;

    return Out;
}

PS_OUT_RESULT PS_HDR_BLURH(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float2 texelSize = HDRIntensity / float2(ScreenWidth, ScreenHeight);
    float3 result = 0;
    float4 OriginTexture = HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord);
    float weights[3] = { 0.398942, 0.241971, 0.053991 };
    
    result = HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord).rgb * weights[0];
    
    for (int i = 1; i < 3; ++i)
    {
        float2 offset = float2(texelSize.x * i, 0);
        result += HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord + offset).rgb * weights[i];
        result += HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord - offset).rgb * weights[i];
    }
    
    Out.vResult = float4(result, OriginTexture.a);
    return Out;
}

PS_OUT_RESULT PS_HDR_BLURV(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float2 texelSize = HDRIntensity / float2(ScreenWidth, ScreenHeight);
    float3 result = 0;

    float weights[3] = { 0.398942, 0.241971, 0.053991 };
    float4 OriginTexture = HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord);
    
    result = HDRBlurXTexture.Sample(DefaultSampler, In.vTexcoord).rgb * weights[0];
    
    for (int i = 1; i < 3; ++i)
    {
        float2 offset = float2(0, texelSize.y * i);
        result += HDRBlurXTexture.Sample(DefaultSampler, In.vTexcoord + offset).rgb * weights[i];
        result += HDRBlurXTexture.Sample(DefaultSampler, In.vTexcoord - offset).rgb * weights[i];
    }
    
    Out.vResult = float4(result, OriginTexture.a);
    
    return Out;
}

PS_OUT_RESULT PS_GUASSIAN_BLURH(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float2 texelSize = GuassianIntensity / float2(ScreenWidth, ScreenHeight);
    float3 result = 0;

    float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
    
    result = FinalTexture.Sample(DefaultSampler, In.vTexcoord).rgb * weights[0];
    
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = float2(texelSize.x * i, 0);
        result += FinalTexture.Sample(DefaultSampler, In.vTexcoord + offset).rgb * weights[i];
        result += FinalTexture.Sample(DefaultSampler, In.vTexcoord - offset).rgb * weights[i];
    }
    
    Out.vResult = float4(result, 1.f);
    return Out;
}

PS_OUT_RESULT PS_GUASSIAN_BLURV(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float2 texelSize = GuassianIntensity / float2(ScreenWidth, ScreenHeight);
    float3 result = 0;

    float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
    
    result = GuassianBlurXTexture.Sample(DefaultSampler, In.vTexcoord).rgb * weights[0];
    
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = float2(0, texelSize.y * i);
        result += GuassianBlurXTexture.Sample(DefaultSampler, In.vTexcoord + offset).rgb * weights[i];
        result += GuassianBlurXTexture.Sample(DefaultSampler, In.vTexcoord - offset).rgb * weights[i];
    }
    
    Out.vResult = float4(result, 1.f);
    
    return Out;
}

PS_OUT_RESULT PS_RADIAL_BLUR(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 final = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float2 dir = In.vTexcoord - RadialCenter;
    float dist = length(dir);
    dir = normalize(dir);
        
    float3 result = float3(0, 0, 0);
    float samples = 15.0f;
        
    for (float i = 0; i < samples; i++)
    {
        float offset = (i / samples) * RadialIntensity * dist;
        result += FinalTexture.Sample(DefaultSampler, In.vTexcoord - dir * offset).rgb;
    }
        
    Out.vResult = float4(result / samples, final.a);

    return Out;
}


PS_OUT_RESULT PS_ADDICTIVECOLOR(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 scene = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if(bSkinned)
    {
        float skinnedAlpha = 1 - SkinnedCombinedTexture.Sample(DefaultSampler, In.vTexcoord).a;
        float3 tinted = scene.rgb * AddictiveColor;
    
        Out.vResult = float4(lerp(scene.rgb, tinted, skinnedAlpha), scene.a);
    }
    else
    {
        Out.vResult = float4(scene.rgb * AddictiveColor, scene.a);
    }
    
    return Out;
}

PS_OUT_RESULT PS_GLITCH(PS_IN In)
{
    PS_OUT_RESULT Out;
    float2 uv = In.vTexcoord;
    
    float randomOffset = GlitchNoiseTexture.Sample(LinearSampler, float2(g_Time * 0.3, 0.5)).r * 10.0;
    
    float noise1 = GlitchNoiseTexture.Sample(LinearSampler, float2(uv.y * 30.0 + randomOffset, 0.5)).r;
    float noise2 = GlitchNoiseTexture.Sample(LinearSampler, float2(uv.y * 80.0 + g_Time * 2.0, 0.7)).r;

    float combinedNoise = noise1 * 0.7 + noise2 * 0.3;
    
    float shift = (combinedNoise - 0.5) * GlitchIntensity * 0.02;
    
    float2 glitchUV = float2(uv.x + shift, uv.y);
    Out.vResult = FinalTexture.Sample(DefaultSampler, glitchUV);
    
    return Out;
}

PS_OUT_RESULT PS_SATURATION(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 scene = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float staticMask = bSaturateStaticUse ? StaticCombinedTexture.Sample(DefaultSampler, In.vTexcoord).a : 0.0;
    float skinnedMask = bSaturateSkinnedUse ? SkinnedCombinedTexture.Sample(DefaultSampler, In.vTexcoord).a : 0.0;
    float effectMask = bSaturateEffectUse ? EffectCombinedTexture.Sample(DefaultSampler, In.vTexcoord).a : 0.0;
    
    float combinedMask = saturate(staticMask + skinnedMask + effectMask);
    combinedMask = step(0.01, combinedMask);
    
    float gray = dot(scene.rgb, float3(0.2126, 0.7152, 0.0722));
    float saturation = 1.0 - SaturationIntensity;
    float3 desaturated = lerp(float3(gray, gray, gray), scene.rgb, saturation);

    float3 result = lerp(scene.rgb, desaturated, combinedMask);
    
    Out.vResult = float4(result, scene.a);
    return Out;
}

PS_OUT_RESULT PS_DISTORTION(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 distortionDesc = DistortionCombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 effect = EffectCombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    float2 distortion = distortionDesc.rg;
    float weight = max(distortionDesc.a, 1e-6);
    distortion /= weight;
    float2 distortedUV = saturate(In.vTexcoord + distortion);
   
    float4 scene = FinalTexture.Sample(DefaultSampler, distortedUV);
    float4 hdrBloom = HDRBloomFinalTexture.Sample(DefaultSampler, distortedUV);
    scene.rgb = scene.rgb + hdrBloom.rgb * 0.3;
    scene.rgb = effect.rgb + scene.rgb * (1.f - effect.a);
    scene.rgb += effect.rgb;
    float alpha = max(scene.a, effect.a);
    
    Out.vResult = float4(scene.rgb, alpha);
    
    
    return Out;
}


PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vSkinned = SkinnedCombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vStatic = StaticCombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vUI = UICombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vVanish = VanishNoiseTexture.Sample(DefaultSampler, In.vTexcoord);

    float4 vStaticDepth = StaticDepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vSkinnedDepth = SkinnedDepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vBlend = BlendTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vNonLight = NonLightTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 result;
    float resultAlpha;
    
    bool hasStatic = vStaticDepth.x > 0.0001f;
    bool hasSkinned = vSkinnedDepth.x > 0.0001f;
    
    if (hasStatic && hasSkinned)
    {
        if (vStaticDepth.x < vSkinnedDepth.x) 
        {
            result = vStatic.rgb;
            resultAlpha = vStatic.a;
        }
        else 
        {
            result = lerp(vStatic.rgb, vSkinned.rgb, vSkinned.a);
            resultAlpha = max(vSkinned.a, vStatic.a);
        }
    }
    else if (hasSkinned)
    {
        result = vSkinned.rgb;
        resultAlpha = vSkinned.a;
    }
    else if (hasStatic)
    {
        result = vStatic.rgb;
        resultAlpha = vStatic.a;
    }
    else
    {
        result = float3(0, 0, 0);
        resultAlpha = 0;
    }
    
    result.rgb += vVanish.rgb;
    
    result.rgb = lerp(result.rgb, vUI.rgb, vUI.a);
    resultAlpha = max(vUI.a, resultAlpha);
    
    result.rgb = lerp(result.rgb, vBlend.rgb, vBlend.a);
    resultAlpha = max(vBlend.a, resultAlpha);
    
    result.rgb = lerp(result.rgb, vNonLight.rgb, vNonLight.a);
    resultAlpha = max(vNonLight.a, resultAlpha);
    
    Out.vBackBuffer = float4(result.rgb, resultAlpha);
    
    return Out;
}

float4 PS_MAIN_FINAL(PS_IN In) : SV_Target
{
    float4 scene = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 ui = UI2DTexture.Sample(DefaultSampler, In.vTexcoord);

    //float3 hdrColor = effect.rgb + scene.rgb * (1.f - effect.a);
    //float alpha = max(scene.a, effect.a);
    float3 hdrColor = scene.rgb;
    
    float3 mapped = ZZZStyleTonemap(hdrColor);
    
    float3 finalColor = ui.rgb + mapped * (1.f - ui.a);

    return float4(finalColor, scene.a);
}


technique11 DefaultTechnique
{
    pass HDR_BRIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HDR_BRIGHTPASS();
    }

    pass HDR_BLURH
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HDR_BLURH();
    }

    pass HDR_BLURV
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HDR_BLURV();
    }

    pass GUASSIAN_BLURH
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GUASSIAN_BLURH();
    }

    pass GUASSIAN_BLURV
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GUASSIAN_BLURV();
    }

    pass RADIAL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RADIAL_BLUR();
    }

    pass FOG
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FOG();
    }

    pass SATURATION
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SATURATION();
    }

    pass ADDICTIVECOLOR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ADDICTIVECOLOR();
    }

    pass DISTORTION
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }

    pass GLITCH
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GLITCH();
    }

    pass COMBINED
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }

    pass FINAL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }
}
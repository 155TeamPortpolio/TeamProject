#include "Shader_Deferred_Define.hlsl"

matrix g_WorldMatrix;

float   g_FogDensity;
float4  g_FogColor;
bool    g_FogUse;
float   g_Time;

float   g_RadialEaseT;
float3  g_AddictiveColor;
float   g_AddictiveStrength = 3.f;
bool   g_UseAddictiveColor = false;
float2  g_RadialCenter;
bool    g_RadialUse = false;

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
    
    float effectAlpha = 1 - EffectCombinedTexture.Sample(DefaultSampler, In.vTexcoord).a;
    
    if (g_FogUse == false)
    {
        Out.vResult = vScene;
        return Out;
    }
    
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
    
    float fFogFactor = exp(-g_FogDensity * fDistance);
    
    fFogFactor = saturate(fFogFactor);
    fFogFactor = lerp(0.3f, 1.0f, fFogFactor);
    
    float4 vFoggedColor = lerp(g_FogColor, vScene, fFogFactor);
 
    Out.vResult = lerp(vScene, vFoggedColor, effectAlpha);
    
    return Out;
}

PS_OUT_RESULT PS_HDR_BRIGHTPASS(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 scene = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 bright = SoftExtractBright(scene);
    
    Out.vResult = bright;

    return Out;
}

PS_OUT_RESULT PS_HDR_BLURH(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float2 texelSize = 1.0 / float2(fScreenWidth, fScreenHeight);
    float3 result = 0;
    
    float weights[3] = { 0.398942, 0.241971, 0.053991 };
    
    result = HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord).rgb * weights[0];
    
    for (int i = 1; i < 3; ++i)
    {
        float2 offset = float2(texelSize.x * i, 0);
        result += HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord + offset).rgb * weights[i];
        result += HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord - offset).rgb * weights[i];
    }
    
    Out.vResult = float4(result, 1.0);
    return Out;
}

PS_OUT_RESULT PS_HDR_BLURV(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float2 texelSize = 1.0 / float2(fScreenWidth, fScreenHeight);
    float3 result = 0;

    float weights[3] = { 0.398942, 0.241971, 0.053991 };

    result = HDRBlurXTexture.Sample(DefaultSampler, In.vTexcoord).rgb * weights[0];
    
    for (int i = 1; i < 3; ++i)
    {
        float2 offset = float2(0, texelSize.y * i);
        result += HDRBlurXTexture.Sample(DefaultSampler, In.vTexcoord + offset).rgb * weights[i];
        result += HDRBlurXTexture.Sample(DefaultSampler, In.vTexcoord - offset).rgb * weights[i];
    }
    
    Out.vResult = float4(result, 1.0);
    
    return Out;
}

PS_OUT_RESULT PS_RADIAL_BLUR(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 final = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (g_RadialUse == false)
    {
        Out.vResult = final;
        return Out;
    }
    
    float2 dir = In.vTexcoord - g_RadialCenter;
    float dist = length(dir);
    dir = normalize(dir);
        
    float3 result = float3(0, 0, 0);
    float samples = 15.0f;
    float strength = g_RadialEaseT * 0.15;
        
    for (float i = 0; i < samples; i++)
    {
        float offset = (i / samples) * strength * dist;
        result += FinalTexture.Sample(DefaultSampler, In.vTexcoord - dir * offset).rgb;
    }
        
    Out.vResult = float4(result / samples, final.a);

    return Out;
}

PS_OUT_RESULT PS_DISTORTION_ADD(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    // ============================================
    // �׽�Ʈ�� �Ķ���� (���⼭ ����)
    // ============================================
    float2 center = float2(0.5, 0.5); // ȭ�� �߾�
    float radius = 0.3; // ȿ�� �ݰ� (�׽�Ʈ: 0.2 ~ 0.5)
    float power = 2.0; // � ���� (�׽�Ʈ: 1.0 ~ 5.0)
    float strength = 0.8; // �ְ� ���� (�׽�Ʈ: -1.0 ~ 1.0)
    
    // ============================================
    // Spherical Distortion ���
    // ============================================
    float2 offset = In.vTexcoord - center;
    
    // Aspect Ratio ����
    offset.x *= 1.777;
    
    float distance = length(offset);
    
    // �ݰ� ��
    if (distance > radius || distance < 0.0001)
    {
        Out.vResult = float4(0.5, 0.5, 0.5, 1.0);
        return Out;
    }
    
    // ����ȭ�� �Ÿ�
    float normalizedDist = distance / radius;
    
    // �߽ɿ��� ���ϰ�
    float distortStrength = 1.0 - normalizedDist;
    distortStrength = pow(distortStrength, power);
    
    // ���� ���
    float2 direction = normalize(offset);
    float2 distortion = direction * distortStrength * strength;
    
    // Aspect ������
    distortion.x /= 1.777;
    
    // ����
    Out.vResult = float4(distortion + 0.5, 0.5, 1.0);
    
    return Out;
}


PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vSkinned = SkinnedCombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vStatic = StaticCombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vEffect = EffectCombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vUI = UICombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 result = vSkinned.rgb;
    
    result.rgb = lerp(result.rgb, vStatic.rgb, vStatic.a);
    result.rgb = lerp(result.rgb, vUI.rgb, vUI.a);
    float3 finalColor = vEffect.rgb + result * (1.f - vEffect.a);
    float alpha = max(vEffect.a, max(vUI.a, max(vSkinned.a, vStatic.a)));
    Out.vBackBuffer = float4(finalColor, alpha);
    
    return Out;
}

float4 PS_MAIN_FINAL(PS_IN In) : SV_Target
{
    float4 scene = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 radialBloom = RadialBloomTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 hdrBloom = HDRBloomFinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 ui = UI2DTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 hdrColor = scene.rgb;
    
    if (g_UseAddictiveColor)
    {
        float skinnedAlpha = 1 - SkinnedCombinedTexture.Sample(DefaultSampler, In.vTexcoord).a;
        float3 tinted = scene.rgb + scene.rgb * g_AddictiveColor * g_AddictiveStrength;
        hdrColor = lerp(scene.rgb, tinted, skinnedAlpha);
    }
    
    hdrColor += hdrBloom.rgb * 0.3;
    
    float3 mapped = ACESFilm(hdrColor) + radialBloom.rgb;
    
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

    pass RADIAL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RADIAL_BLUR();
    }

    pass DISTORTION_ADD
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION_ADD();
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

    pass COMBINED
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
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
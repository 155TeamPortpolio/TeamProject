#include "Shader_Define.hlsl"

matrix g_WorldMatrix;

Texture2D g_NormalTexture;
Texture2D g_DiffuseTexture;
Texture2D g_LightTexture;
Texture2D g_SpecularTexture;
Texture2D g_EmmisiveTexture;
Texture2D g_DepthTexture;
Texture2D g_ShadowTexture;
Texture2D g_MetalicTexture;
Texture2D g_AmbientTexture;
Texture2D g_RampTexture;
Texture2D g_SSAONoiseTexture;
Texture2D g_SSAOTexture;
Texture2D g_SSAOBlurTexture;
Texture2D g_BrightTexture;
Texture2D g_BloomInfo;
Texture2D g_BlurXTexture;
Texture2D g_BloomFinal;
Texture2D g_3DUITexture;
Texture2D g_DistortionTexture;
Texture2D g_DistortionNoiseTexture;
Texture2D g_DistortionAdd_Texture;
Texture2D g_DistortionFinal;
Texture2D g_EffectDiffuseTexture;
Texture2D g_HDRBlurXTexture;
Texture2D g_HDRBlurYTexture;

Texture2D g_FinalTexture;
Texture2D g_UITexture;
Texture2D g_PostProcessTexture;

vector g_vLightDir;
vector g_vLightPos;
float  g_fLightRange;
float  g_fLightIntensity;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;

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
};

struct PS_OUT_RESULT
{
    vector vResult : SV_TARGET0;
};

PS_OUT_RESULT PS_SSAO(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    float fViewZ = vDepthDesc.y * zFar;
    
    float3 N = mul(float4(worldNormal, 0.f), matView).xyz;
    N = normalize(N);
    
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);

    float3 fragPos = mul(vWorldPos, matView).xyz;
    
    float2 noiseScale = float2(fScreenWidth / 4.0, fScreenHeight / 4.0);
    float3 randomVec = g_SSAONoiseTexture.Sample(DefaultSampler, In.vTexcoord * noiseScale).xyz;
    randomVec = randomVec * 2.0 - 1.0; 
    
    float3 T = normalize(randomVec - N * dot(randomVec, N));
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    
    float occlusion = 0.0;
    
    for (int i = 0; i < 64; ++i)
    {
        float3 samplePos = mul(SSAOKernel[i].xyz, TBN);
        samplePos = fragPos + samplePos * fRadius;
        
        float4 offset = float4(samplePos, 1.0);
        offset = mul(offset, matProjection);
        offset.xyz /= offset.w;
        
        offset.xy = offset.xy * 0.5 + 0.5;
        offset.y = 1.0 - offset.y;
        
        vector vSampleDepthDesc = g_DepthTexture.Sample(DefaultSampler, offset.xy);
        float fSampleViewZ = vSampleDepthDesc.y * zFar;
        
        vector vSampleWorldPos;
        vSampleWorldPos.x = offset.x * 2.f - 1.f;
        vSampleWorldPos.y = offset.y * -2.f + 1.f;
        vSampleWorldPos.z = vSampleDepthDesc.x;
        vSampleWorldPos.w = 1.f;
        
        vSampleWorldPos = vSampleWorldPos * fSampleViewZ;
        vSampleWorldPos = mul(vSampleWorldPos, matProjectionInverse);
        vSampleWorldPos = mul(vSampleWorldPos, matViewInverse);
        
        float3 sampleViewPos = mul(vSampleWorldPos, matView).xyz;
        float sampleDepth = sampleViewPos.z;
        
        float rangeCheck = smoothstep(0.0, 1.0, fRadius / abs(fragPos.z - sampleDepth));
        
        occlusion += (sampleDepth <= samplePos.z + fBias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / 64.0);
    Out.vResult = occlusion;
    
    return Out;
}

PS_OUT_RESULT PS_SSAO_BLUR(PS_IN In)
{
    PS_OUT_RESULT Out;
    float2 texelSize = 1.0 / float2(fScreenWidth, fScreenHeight);

    const float weights[5] = { 0.06136, 0.24477, 0.38774, 0.24477, 0.06136 };
    
    float result = 0.0;
    
    for (int x = -2; x <= 2; ++x)
    {
        for (int y = -2; y <= 2; ++y)
        {
            float2 offset = float2(float(x), float(y)) * texelSize;
            float weight = weights[x + 2] * weights[y + 2];
            result += g_SSAOTexture.Sample(DefaultSampler, In.vTexcoord + offset).r * weight;
        }
    }
    
    Out.vResult = result;
    
    return Out;
}

//GaussianBlur
static const float weights[9] =
{
    0.13298076,
    0.12579441,
    0.10648267,
    0.08065691,
    0.05467002,
    0.03315905,
    0.01799699,
    0.00874063,
    0.00379866
};

PS_OUT_RESULT PS_BLOOM_BLURX(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vBloomInfo = g_BloomInfo.Sample(DefaultSampler, In.vTexcoord);
    
    float bloomType = vBloomInfo.r;
    float bloomStrength = vBloomInfo.g;

    float4 bright = g_BrightTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (bloomType == 0.f) // Gaussian
    {
        float3 result = bright.rgb * weights[0];
        float texelSize = bloomStrength / fScreenWidth;
        
        for (int i = 1; i < 9; ++i)
        {
            result += g_BrightTexture.Sample(DefaultSampler,
                In.vTexcoord + float2(texelSize * i, 0)).rgb * weights[i];
            result += g_BrightTexture.Sample(DefaultSampler,
                In.vTexcoord - float2(texelSize * i, 0)).rgb * weights[i];
        }
        
        Out.vResult = float4(result, bright.a);
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
    
    vector vBloomInfo = g_BloomInfo.Sample(DefaultSampler, In.vTexcoord);
    
    float4 blurX = g_BlurXTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float bloomType = vBloomInfo.r;
    float bloomStrength = vBloomInfo.g;
    float2 RadialCenter = vBloomInfo.ba;
    
    if (bloomType < 0.5f) // Gaussian
    {
        float3 result = blurX.rgb * weights[0];
        float texelSize = bloomStrength / fScreenHeight;
        
        for (int i = 1; i < 9; ++i)
        {
            result += g_BlurXTexture.Sample(DefaultSampler,
                In.vTexcoord + float2(0, texelSize * i)).rgb * weights[i];
            result += g_BlurXTexture.Sample(DefaultSampler,
                In.vTexcoord - float2(0, texelSize * i)).rgb * weights[i];
        }
        
        Out.vResult = float4(result, blurX.a);
    }
    else // Radial Blur
    {
        float2 dir = In.vTexcoord - RadialCenter;
        float dist = length(dir);
        dir = normalize(dir);
        
        float3 result = float3(0, 0, 0);
        float samples = 15.0f;          //test�ϰ� �ʹٸ� �̰� ���� ����
        float strength = 0.1f;          //test�ϰ� �ʹٸ� �̰� ���� ����
        
        for (float i = 0; i < samples; i++)
        {
            float offset = (i / samples) * strength * dist;
            result += g_BlurXTexture.Sample(DefaultSampler,
                In.vTexcoord - dir * offset).rgb;
        }
        
        Out.vResult = float4(result / samples, blurX.a);
    }
    
    return Out;
}

PS_OUT_RESULT PS_HDR_BRIGHTPASS(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 scene = g_FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 bright = ExtractBright(scene);
    
    Out.vResult = bright; // 밝은 부분만 추출
    return Out;
}
static const float hdrweights[5] =
{
    0.227027, // 중앙
    0.1945946, // ±1
    0.1216216, // ±2
    0.054054, // ±3
    0.016216 // ±4
};
PS_OUT_RESULT PS_HDR_BLURX(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector bright = g_BrightTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 result = bright.rgb * hdrweights[0];
    float texelSize = 1.f / fScreenWidth;
        
    for (int i = 1; i < 5; ++i)
    {
        result += g_BrightTexture.Sample(DefaultSampler,
                In.vTexcoord + float2(texelSize * i, 0)).rgb * hdrweights[i];
        result += g_BrightTexture.Sample(DefaultSampler,
                In.vTexcoord - float2(texelSize * i, 0)).rgb * hdrweights[i];
    }
        
    Out.vResult = float4(result, bright.a);

    return Out;
}

PS_OUT_RESULT PS_HDR_BLURY(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector HDRBlurX = g_HDRBlurXTexture.Sample(DefaultSampler, In.vTexcoord);

    float3 result = HDRBlurX.rgb * hdrweights[0];
    float texelSize = 1.f / fScreenHeight;
        
    for (int i = 1; i < 5; ++i)
    {
        result += g_HDRBlurXTexture.Sample(DefaultSampler,
                In.vTexcoord + float2(0, texelSize * i)).rgb * hdrweights[i];
        result += g_HDRBlurXTexture.Sample(DefaultSampler,
                In.vTexcoord - float2(0, texelSize * i)).rgb * hdrweights[i];
    }
        
    Out.vResult = float4(result, HDRBlurX.a);
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

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float roughness = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).r;
    float metalic = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).g;
    
    vector vRamp = g_RampTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fViewZ = vDepthDesc.y * zFar;
    
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float3 lightDir = normalize(g_vLightDir.xyz * -1);
    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);

    float NdotL = dot(worldNormal, lightDir) * -0.5f + 0.5f;
    
    float2 vRampCoord = float2(NdotL, 0.5f);
    float3 vRampColor;
 
    vRampColor = saturate(g_RampTexture.Sample(DefaultSampler, vRampCoord).g - 0.5);
    
    float3 PBR = CalculateDirectionalLight(vDiffuse.rgb, worldNormal, metalic, roughness, viewDir, lightDir, g_vLightDiffuse.rgb, g_fLightIntensity, 1.f);
    
    float RampRatio = 0.7f; 
    Out.vLight = float4(lerp(PBR, PBR * vRampColor, RampRatio), 1.f);

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float roughness = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).r;
    float metalic = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).g;
 
    
    float fViewZ = vDepthDesc.y * zFar;
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float3 lightDir = normalize(g_vLightPos.xyz - vWorldPos.xyz);
    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);
    
    float NdotL = dot(worldNormal, lightDir) * -0.5f + 0.5f;
    
    float2 vRampCoord = float2(NdotL, 0.5f);
    float3 vRampColor;
 
    vRampColor = saturate(g_RampTexture.Sample(DefaultSampler, vRampCoord).g - 0.5);
    
    float3 PBR = CalculatePointLight
    (vDiffuse.rgb, worldNormal, metalic, roughness, vWorldPos.xyz, viewDir, lightDir, g_vLightDiffuse.rgb,
    g_fLightIntensity, g_vLightPos.xyz, g_fLightRange, 1.0f);
    
    float RampRatio = 0.7f;
    Out.vLight = float4(lerp(PBR, PBR * vRampColor, RampRatio), 1.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out; 
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vLight = g_LightTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vUI3D = g_3DUITexture.Sample(DefaultSampler, In.vTexcoord);
    vector vEffect = g_EffectDiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float ssao = g_SSAOBlurTexture.Sample(DefaultSampler, In.vTexcoord).r;
    float ao = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).b;
    vector vAmbient = g_AmbientTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 ambient = vDiffuse.rgb * vAmbient.g *ssao;
    //ambient = max(ambient, vDiffuse.rgb * 0.15f); 

    Out.vBackBuffer = float4(vLight.rgb + ambient, 1.f);
 
    if (vUI3D.a > 0.f) Out.vBackBuffer.rgb = vUI3D.rgb;
    if (vEffect.a > 0.f) Out.vBackBuffer.rgb = lerp(Out.vBackBuffer.rgb, vEffect.rgb, vEffect.a);
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * zFar;
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    vWorldPos = mul(vWorldPos, matShadowView);
    vWorldPos = mul(vWorldPos, matShadowProjection);
    
    float2 vTexcoord;
    
    vTexcoord.x = vWorldPos.x / vWorldPos.w * 0.5f + 0.5f;
    vTexcoord.y = vWorldPos.y / vWorldPos.w * -0.5f + 0.5f;
    
    float4 vLightDepthDesc = g_ShadowTexture.Sample(DefaultSampler, vTexcoord);
    
    //if (vWorldPos.w - 0.01f > vLightDepthDesc.y * zShadowFar)
    //{
    //    Out.vBackBuffer *= 0.3f;
    //}
    return Out;
}

float4 PS_MAIN_FINAL(PS_IN In) : SV_Target
{ 
    float4 scene = g_FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 hdrBloom = g_HDRBlurYTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 effectbloom = g_BloomFinal.Sample(DefaultSampler, In.vTexcoord);
    float4 ui = g_UITexture.Sample(DefaultSampler, In.vTexcoord);
    //float4 distortion = g_DistortionFinal.Sample(DefaultSampler, In.vTexcoord);
    float3 hdrColor = scene.rgb;
    hdrColor += hdrBloom.rgb * 0.3;
    if (effectbloom.a > 0.f) hdrColor += effectbloom.rgb * effectbloom.a;
    
    float3 mapped = ACESFilm(hdrColor);
    float3 finalColor = lerp(mapped, ui.rgb, ui.a);
    return float4(finalColor, 1.f);
}

technique11 DefaultTechnique
{
    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SSAO();
    }

    pass SSAO_BLUR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SSAO_BLUR();
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

    pass HDR_BRIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HDR_BRIGHTPASS();
    }

    pass HDR_BLURX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HDR_BLURX();
    }

    pass HDR_BLURY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HDR_BLURY();
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

    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }

    pass Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }
}
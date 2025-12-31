#include "Shader_Deferred_Define.hlsl"

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
Texture2D g_DistortionTexture;
Texture2D g_DistortionNoiseTexture;
Texture2D g_DistortionAdd_Texture;
Texture2D g_DistortionFinal;

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

struct PS_OUT_RESULT
{
    vector vResult : SV_TARGET0;
};

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
    else // Radial은 X패스에서 그대로 통과
    {
        Out.vResult = bright;
    }
    Out.vResult=1.f;
    
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
        float samples = 15.0f;          //test하고 싶다면 이거 수정 ㄱㄱ
        float strength = 0.1f;          //test하고 싶다면 이거 수정 ㄱㄱ
        
        for (float i = 0; i < samples; i++)
        {
            float offset = (i / samples) * strength * dist;
            result += g_BlurXTexture.Sample(DefaultSampler,
                In.vTexcoord - dir * offset).rgb;
        }
        
        Out.vResult = float4(result / samples, blurX.a);
    }
    Out.vResult=1.f;
    
    return Out;
}

PS_OUT_RESULT PS_DISTORTION_ADD(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    // ============================================
    // 테스트용 파라미터 (여기서 조절)
    // ============================================
    float2 center = float2(0.5, 0.5); // 화면 중앙
    float radius = 0.3; // 효과 반경 (테스트: 0.2 ~ 0.5)
    float power = 2.0; // 곡선 강도 (테스트: 1.0 ~ 5.0)
    float strength = 0.8; // 왜곡 세기 (테스트: -1.0 ~ 1.0)
    
    // ============================================
    // Spherical Distortion 계산
    // ============================================
    float2 offset = In.vTexcoord - center;
    
    // Aspect Ratio 보정
    offset.x *= 1.777;
    
    float distance = length(offset);
    
    // 반경 밖
    if (distance > radius || distance < 0.0001)
    {
        Out.vResult = float4(0.5, 0.5, 0.5, 1.0);
        return Out;
    }
    
    // 정규화된 거리
    float normalizedDist = distance / radius;
    
    // 중심에서 강하게
    float distortStrength = 1.0 - normalizedDist;
    distortStrength = pow(distortStrength, power);
    
    // 방향 계산
    float2 direction = normalize(offset);
    float2 distortion = direction * distortStrength * strength;
    
    // Aspect 역보정
    distortion.x /= 1.777;
    
    // 저장
    Out.vResult = float4(distortion + 0.5, 0.5, 1.0);
    Out.vResult = 1.f;

    return Out;
}

float4 PS_MAIN_FINAL(PS_IN In) : SV_Target
{ //
    float4 scene = g_FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 bloom = g_BloomFinal.Sample(DefaultSampler, In.vTexcoord);
  
    float4 ui = g_UITexture.Sample(DefaultSampler, In.vTexcoord);
    float3 mapped = scene.rgb + bloom.rgb;
  
    return 1.f;
    return float4((1 - ui.a) * mapped.xyz + (ui.a * ui.rgb), 1.f);
}



technique11 DefaultTechnique
{
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

    pass DISTORTION_ADD
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION_ADD();
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
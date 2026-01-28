#include "Shader_Define.hlsl"

float4x4 g_worldMatrix;

uint RGBMask;
uint ColorMode;
float Progress;
float ScreenWidth;
float ScreenHeight;

/* Texture */
Texture2D GradientTexture;
#define NONE  0
#define SHAPE_MASK 1
#define EMISSION  2
#define DISTORTION 3

uint SamplerMode;
uint MainUsage;
uint4 ChannelUsage;
float GetChannelValue(float4 vTexSample, uint iTargetUsage)
{
    if (ChannelUsage.x == iTargetUsage)
        return vTexSample.x;
    
    if (ChannelUsage.y == iTargetUsage)
        return vTexSample.y;
    
    if (ChannelUsage.z == iTargetUsage)
        return vTexSample.z;
    
    if (ChannelUsage.a == iTargetUsage)
        return vTexSample.a;
    
    return 0.f;
}

float4 ApplySamplerMode(uint samplerMode,float2 texCoord ,Texture2D sampleTexture)
{
    float4 vResult;

    if(0 == samplerMode)
    {
        vResult = sampleTexture.Sample(LinearSampler, texCoord);
    }
    else if(1 == samplerMode)
    {
        vResult = sampleTexture.Sample(LinearClampSampler, texCoord);
    }
    else
    {
        vResult = float4(1.f, 1.f, 1.f, 1.f);
    }
    
    return vResult;
    
}

float3 ApplyGradientMode(uint gradientMode, float grayMask, float2 texcoord)
{
    float3 vResult;
    float2 vTexcoord;
    
    if(0 == gradientMode) /* Gray Scale */
    {
        grayMask = saturate(grayMask);
        vTexcoord = float2(grayMask, 0.5f);
        
        vResult = GradientTexture.Sample(LinearClampSampler, vTexcoord).rgb;
    }
    else if(1 == gradientMode) /* UV x*/
    {
        vTexcoord = float2(texcoord.x, 0.5f);
        
        vResult = GradientTexture.Sample(LinearClampSampler, vTexcoord).rgb;
    }
    else if(2 == gradientMode) /* UV y*/
    {
        vTexcoord = float2(texcoord.y, 0.5f);
        
        vResult = GradientTexture.Sample(LinearClampSampler, vTexcoord).rgb;
    }
    else /* Life Time */
    {
        float t = saturate(Progress);
        vTexcoord = float2(t, 0.5f);
        
        vResult = GradientTexture.Sample(LinearClampSampler, vTexcoord).rgb;
    }
    
    return vResult;
}

/*Color*/
float4 vBaseColor;

/*UV Animation Params*/
float2 UVOffset;

/*Sprite Animation Params*/
uint Col;
uint Row;
uint FrameIndex;

/* Bloom Params */
float3 BloomColor;
float BloomThreshold;
float BloomSoftness;
float BloomIntensity;

/*Dissolve Params*/
float EnableDissolve;
float DissolveProgress;
float DissolveSoftness;

/*Noise Params*/
float EnableNoise;
float NoiseStrength;
float NoiseTiling;
float2 NoiseUVSpeed;
float ElapsedTime;

/*Mask Params*/
Texture2D AlphaMaskTextureA;
Texture2D AlphaMaskTextureB;
float EnableMaskA;
float EnableMaskB;
float MaskTilling;

/*Distortion Params*/
float EnableDistortion;
float DistortionStrength;
float DistortionTilling;
float DistortionUVSpeed;

/*Gradient Params*/
float EnableGradient;
uint GradientMode; //0 : GrayScale, 1 : UV, 2 : LIFE_TIME

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
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float4 vViewPosition : TEXCOORD2;
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
    Out.vViewPosition = viewPos;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float4 vViewPosition : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuseAcc : SV_Target0;
    float4 vBloomAcc : SV_Target1;
    float4 vBloomInfo : SV_Target2;
    float4 vRevealage : SV_Target3;
    float4 vDistortionAcc : SV_Target4;
};

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
    PS_OUT Out;
    
    float2 Texcoord = CalculateFrameIndex(Col, Row, FrameIndex, In.vTexcoord);
    Texcoord += UVOffset;
    
    /* Noise */
    float2 vNoiseTexcoord = In.vTexcoord * NoiseTiling + ElapsedTime * NoiseUVSpeed;
    float2 vNoise = ApplySamplerMode(SamplerMode, vNoiseTexcoord, NoiseTexture).rg * 2.f - 1.f;
    Texcoord += vNoise * NoiseStrength * EnableNoise;
    
    /* Diffuse */
    vector vDiffuse = vector(1.f, 1.f, 1.f, 1.f);
    vDiffuse = ApplySamplerMode(SamplerMode, Texcoord, DiffuseTexture);   
 
    /* Dissolve */
    float fDissolveMask = 1.f;
    float fDissolveSoftness = max(DissolveSoftness, 1e-5);
    
    fDissolveMask = ApplySamplerMode(SamplerMode, In.vTexcoord, DissolveTexture).r;
    float fDissolveAlpha = smoothstep(DissolveProgress - fDissolveSoftness, DissolveProgress + fDissolveSoftness, fDissolveMask);
    fDissolveAlpha = lerp(1.f, fDissolveAlpha, EnableDissolve);
    
    /* Mask */
    float fMaskA = ApplySamplerMode(SamplerMode, In.vTexcoord * MaskTilling, AlphaMaskTextureA).r;
    fMaskA = lerp(1.f, fMaskA, EnableMaskA);
    
    float fMaskB = ApplySamplerMode(SamplerMode, In.vTexcoord * MaskTilling, AlphaMaskTextureB).r;
    fMaskB = lerp(1.f, fMaskB, EnableMaskB);
    
    float fMask = fMaskA * fMaskB;
    
    /* Distortion */
    float2 vDistortionTexcoord = In.vTexcoord * DistortionTilling + ElapsedTime * DistortionUVSpeed;
    float2 vDistortion = ApplySamplerMode(SamplerMode, vDistortionTexcoord, DistortionTexture).rg;
    vDistortion = vDistortion * 2.f - 1.f;
    vDistortion = vDistortion * DistortionStrength * float2(1.f / ScreenWidth, 1.f / ScreenHeight) * EnableDistortion;
    
    float4 vResult = float4(1.f, 1.f, 1.f, 1.f);
    
    if (MainUsage == 0)  //as color mode
    {
        vResult = ApplyColorMode(ColorMode, vDiffuse, vBaseColor);
        
        if(1 == RGBMask)
        {
            float fColorMask = max(vDiffuse.r, max(vDiffuse.g, vDiffuse.b));
            vResult.a *= fColorMask;
        }
    }
    else if (MainUsage == 1) //as channel
    {
        float fShapeMask = GetChannelValue(vDiffuse, SHAPE_MASK);
        float fEmission = GetChannelValue(vDiffuse, EMISSION);
        float fDistortion = GetChannelValue(vDiffuse, DISTORTION);
        
        if (fDistortion > 0.f)
        {
            float2 vDistortionOffset = (fDistortion - 0.5f) * 2.f;
            Texcoord += vDistortionOffset;
            
            vDiffuse = DiffuseTexture.Sample(LinearClampSampler, Texcoord);
            fShapeMask = GetChannelValue(vDiffuse, SHAPE_MASK);
            fEmission = GetChannelValue(vDiffuse, EMISSION);
        }
        
        vResult = vBaseColor + fEmission * vBaseColor;
        vResult.a = fShapeMask;
    }
    
    float3 vColor = vResult.rgb;
    float fAlpha = vResult.a * fDissolveAlpha * fMask;
    
    /* Gradient */
    if (EnableGradient > 0.5f)
    {
        float3 vGradientColor = ApplyGradientMode(GradientMode, fAlpha, In.vTexcoord);
        vColor = ApplyColorMode(ColorMode, float4(vColor, 1.f), float4(vGradientColor, 1.f)).rgb;
    }
    
    /* 깊이 기반 가중치 생성 */
    float fLinearZ = In.vViewPosition.z;
    float fDepthBias = 1.f / (1.f + fLinearZ * fLinearZ);
    float fWeight = clamp((fAlpha * 4.f + 0.01f) * fDepthBias, 0.01f, 1.f);
    float4 vPremulColor = float4(vColor * fAlpha, fAlpha);
    
    Out.vDiffuseAcc = vPremulColor * fWeight;
    Out.vBloomAcc = SoftExtractBright(vPremulColor, BloomThreshold, BloomSoftness, BloomIntensity);
    Out.vBloomAcc.a = fAlpha;
    Out.vBloomInfo = float4(0.f, 1.5f, 0.f, 0.f);
    Out.vRevealage = float4(fAlpha, fAlpha, fAlpha, fAlpha);
    Out.vDistortionAcc = float4(vDistortion * fAlpha, 0.f, fAlpha);
    
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
        PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
    }
}


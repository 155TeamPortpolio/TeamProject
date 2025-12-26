#include "Shader_Define.hlsl"

float4x4 g_worldMatrix;
BlendState BS_OITAccmulation
{
    /* Diffuse Effect */
    BlendEnable[0] = true;
    SrcBlend[0] = One;
    DestBlend[0] = One;
    BlendOp[0] = Add;
    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = One;
    BlendOpAlpha[0] = Add;

    /* Bloom Effect */
    BlendEnable[1] = true;
    SrcBlend[1] = One;
    DestBlend[1] = One;
    BlendOp[1] = Add;
    SrcBlendAlpha[1] = One;
    DestBlendAlpha[1] = One;
    BlendOpAlpha[1] = Add;

    /* Bloom Info */
    BlendEnable[2] = true;
    SrcBlend[2] = One;
    DestBlend[2] = One;
    BlendOp[2] = Add;
    SrcBlendAlpha[2] = One;
    DestBlendAlpha[2] = One;
    BlendOpAlpha[2] = Add;

    /* Revealage */
    BlendEnable[3] = true;
    SrcBlend[3] = Zero;
    DestBlend[3] = Inv_Src_Alpha;
    BlendOp[3] = Add;
    SrcBlendAlpha[3] = Zero;
    DestBlendAlpha[3] = Inv_Src_Alpha;
    BlendOpAlpha[3] = Add;
};


float Progress;

/* Texture */
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

/*Color*/
float4 vBaseColor;

/*UV Animation Params*/
float2 UVOffset;

/*Sprite Animation Params*/
uint Col;
uint Row;
uint FrameIndex;

/* Bloom Params */
float BloomThreshold;
float BloomSoftness;
float BloomIntensity;

/*Dissolve Params*/
float DissolveProgress;

/*Distortion Params*/

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
};

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
    PS_OUT Out;
    
    float2 Texcoord = CalculateFrameIndex(Col, Row, FrameIndex, In.vTexcoord);
    Texcoord += UVOffset;
    
    vector vTexSample = vector(1.f, 1.f, 1.f, 1.f);
    float fDissolveMask = 1.f;

    if(SamplerMode == 0)
    {
       vTexSample = DiffuseTexture.Sample(LinearSampler, Texcoord);
       //fDissolveMask = DissolveTexture.Sample(LinearSampler, Texcoord).r;
    }
    else if(SamplerMode == 1)
    {
       vTexSample = DiffuseTexture.Sample(LinearClampSampler, Texcoord);
       //fDissolveMask = DissolveTexture.Sample(LinearClampSampler, Texcoord).r;
    }
    
    //if (fDissolveMask < DissolveProgress)
    //    discard;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    if (MainUsage == 0)  //as color
    {
        color = vTexSample;
    }
    else if (MainUsage == 1) //as channel
    {
        float fShapeMask = GetChannelValue(vTexSample, SHAPE_MASK);
        float fEmission = GetChannelValue(vTexSample, EMISSION);
        float fDistortion = GetChannelValue(vTexSample, DISTORTION);
        
        if (fDistortion > 0.f)
        {
            float2 vDistortionOffset = (fDistortion - 0.5f) * 2.f;
            Texcoord += vDistortionOffset;
            
            vTexSample = DiffuseTexture.Sample(LinearClampSampler, Texcoord);
            fShapeMask = GetChannelValue(vTexSample, SHAPE_MASK);
            fEmission = GetChannelValue(vTexSample, EMISSION);
        }
        
        color = vBaseColor + fEmission * vBaseColor;
        color.a = fShapeMask;
    }
    else if (MainUsage == 2) //as grayscale
    {
        float fValue = vTexSample.r;
        color = vBaseColor * fValue;
    }
    
    /* 깊이 기반 가중치 생성 */
    float fLinearZ = In.vViewPosition.z;
    float fWeight = clamp(0.03 / (1e-5 + pow(fLinearZ, 4.f)), 0.01f, 3e3);
    fWeight = max(fWeight, 1.f);
    
    float3 vColor = color.rgb;
    float fAlpha = color.a;
    
    if (fAlpha < 0.01f)
        discard;
    
    Out.vDiffuseAcc = float4(vColor, fAlpha);
    Out.vDiffuseAcc.a = fAlpha;
    Out.vBloomAcc = SoftExtractBright(float4(vColor, fAlpha), BloomThreshold, BloomSoftness, BloomIntensity);
    Out.vBloomAcc.a = fAlpha;
    Out.vBloomInfo = float4(0.f, 1.5f, 0.f, 0.f);
    Out.vRevealage = float4(fAlpha, fAlpha, fAlpha, fAlpha);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_ReadOnly, 0);
        SetBlendState(BS_OITAccmulation, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
    }
}


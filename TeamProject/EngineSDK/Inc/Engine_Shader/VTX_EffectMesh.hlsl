#include "Shader_Define.hlsl"

float4x4 g_worldMatrix;

/* Texture */
#define NONE = 0;
#define SHAPE_MASK = 1;
#define EMISSION = 2;
#define DISTORTION = 3;

float4 ChannelUsage;

/*Color*/
float4 vBaseColor;

/*UV Animation Params*/
float2 UVOffset;

/*Sprite Animation Params*/
uint Col;
uint Row;
uint FrameIndex;

/* Bloom Params */
float BloomIntensity;

/*Dissolve Params*/
float DissolveProgress;

/*Distortion Params*/

/*Func*/
float GetChannelValue(float4 vTexSample, uint iTargetUsage)
{
    if(ChannelUsage.x == iTargetUsage)
        return vTexSample.x;
    
    if(ChannelUsage.y == iTargetUsage)
        return vTexSample.y;
    
    if(ChannelUsage.z == iTargetUsage)
        return vTexSample.z;
    
    if(ChannelUsage.a == iTargetUsage)
        return vTexSample.a;
    
    return 0.f;
}

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
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vBloom : SV_TARGET1;
    vector BloomInfo : SV_TARGET2;
};

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    //if (vMtrlDiffuse.a < 0.1f)
    //    discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
}

PS_OUT PS_MAIN_UVANIMATION(PS_IN In)
{
    PS_OUT Out;
    
    float2 Texcoord = In.vTexcoord + UVOffset;
        
    vector vMtrlDiffuse = DiffuseTexture.Sample(LinearClampSampler, Texcoord);
    float fDissolveMask = DissolveTexture.Sample(LinearSampler, Texcoord).r;
    
    float fBrightIntensity = 2.f;
    float fBase = vMtrlDiffuse.g;
    float fBright = vMtrlDiffuse.r;
    float fMask = vMtrlDiffuse.b;
    float fRGBMask = max(vMtrlDiffuse.r, max(vMtrlDiffuse.g, vMtrlDiffuse.b));
    
    if (fDissolveMask < DissolveProgress)
        discard;
    
    //if (vMtrlDiffuse.a < 0.01f)
    //    discard;
    
    Out.vDiffuse = vBaseColor * (fBase + fBright * fBrightIntensity);
    Out.vDiffuse.a = vBaseColor.a * fRGBMask;
    Out.BloomInfo = float4(0.f, 1.5f, 0.f, 0.f);
    Out.vBloom = Out.vDiffuse * BloomIntensity;
    Out.vBloom.a = Out.vDiffuse.a;
    
    return Out;
}

PS_OUT PS_MAIN_SPRITEANIMATION(PS_IN In)
{
    PS_OUT Out;
    
    float2 FrameSize = float2(1.f / Col, 1.f / Row);
    int iFrameX = FrameIndex % Col;
    int iFrameY = FrameIndex / Col;
    float2 FrameMin = float2(iFrameX, iFrameY) * FrameSize;
    float2 TexCoord = FrameMin + In.vTexcoord * FrameSize;
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(LinearSampler, TexCoord);
    if(vMtrlDiffuse.a <0.01f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse.a *= vBaseColor.a;
    Out.vBloom = float4(0.f, 0.f, 0.f, 0.f);
    Out.BloomInfo = float4(0.f, 1.f, 0.f, 0.f);
    
    return Out;
}

struct PS_OUT_BRIGHT
{
    vector vBloom : SV_TARGET0;
    vector BloomInfo : SV_TARGET1;
};

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
    }

    pass UVAnimation
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UVANIMATION();
    }

    pass SpriteAnimation
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPRITEANIMATION();
    }
}


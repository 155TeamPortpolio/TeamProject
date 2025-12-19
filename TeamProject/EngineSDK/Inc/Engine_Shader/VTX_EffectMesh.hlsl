#include "Shader_Define.hlsl"

float4x4 g_worldMatrix;

/*Default Params*/
float4 vBaseColor;
float4 vBrightColor;
float4 vEmissiveColor;
float Alpha;
float Threshold;

/*Sprite Animation Params*/
uint Col;
uint Row;
uint FrameIndex;

/*UV Animation Params*/
float2 UVOffset;

/*Noise Params*/

/*Dissolve Params*/
float DissolveThreshold;

/*Distortion Params*/

/* Bloom Params */
float BloomIntensity;

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

VS_OUT VS_MAIN_BRIGHT(VS_IN In)
{
    VS_OUT Out;
    
    matrix matWV, matWVP;
    
    float3 worldPos = mul(float4(In.vPosition, 1.f), g_worldMatrix).xyz;
    float4 viewPos = mul(float4(worldPos, 1.f), matView);
    float4 projPos = mul(viewPos, matProjection);

    Out.vPosition = projPos;
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_worldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_worldMatrix)).xyz;
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
    
    if (fDissolveMask < DissolveThreshold)
        discard;
    
    Out.vDiffuse = vBaseColor * (fBase + fBright * fBrightIntensity);
    Out.vDiffuse.a = Alpha * fRGBMask;
    
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
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse.a *= Alpha;
    
    return Out;
}

struct PS_OUT_BRIGHT
{
    vector vBloom : SV_TARGET0;
    vector BloomInfo : SV_TARGET1;
};

PS_OUT_BRIGHT PS_BRIGHT(PS_IN In)
{
    PS_OUT_BRIGHT Out;  
    
    float2 Texcoord = In.vTexcoord + UVOffset;
        
    vector vMtrlDiffuse = DiffuseTexture.Sample(LinearClampSampler, Texcoord);
    float fDissolveMask = DissolveTexture.Sample(LinearSampler, Texcoord).r;
    
    float fBrightIntensity = 2.f;
    float fBase = vMtrlDiffuse.g;
    float fBright = vMtrlDiffuse.r;
    float fMask = vMtrlDiffuse.b;
    float fRGBMask = max(vMtrlDiffuse.r, max(vMtrlDiffuse.g, vMtrlDiffuse.b));
    
    if (fDissolveMask < DissolveThreshold)
        discard;
    
    vector vBloomColor = vBaseColor * (fBase + fBright * fBrightIntensity);
    vBloomColor.a = Alpha * fRGBMask;
    
    
    Out.BloomInfo = float4(0.f, 1.5f, 0.f,0.f);
    Out.vBloom = vBloomColor * BloomIntensity;
    
    return Out;
}

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

    pass Bright
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_BRIGHT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRIGHT();
    }
}


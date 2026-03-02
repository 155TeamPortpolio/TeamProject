#include "Shader_Define.hlsl"

// ==========================
// Params
// ==========================
float fUseVanish;
float3 vEmissiveColor;
float fEmissiveStrength;
float3 vRimLightColor = float3(0.378, 0.029, 0.070);
vector vOutLineColor;

float fOutLineThickness;
float fRimLightPower = 4.f;

float fTime;
float fDissolveProgress;

float fDissolveScrollSpeed;
float fDissolveNoiseStrength; // (optional, not used heavily below)
float fDissolveEdgeWidth;
float2 vDissolveTiling;
float3 pad;

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
    float viewZ : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float3 worldPos = mul(float4(In.vPosition, 1.f), ObjectBufferArray[TransformIndex].Transform).xyz;
    float4 viewPos = mul(float4(worldPos, 1.f), matView);
    float4 projPos = mul(viewPos, matProjection);

    Out.vPosition = projPos;
    Out.vTexcoord = In.vTexcoord;

    Out.vNormal = mul(vector(In.vNormal, 0.f), ObjectBufferArray[TransformIndex].Transform);
    Out.vProjPos = Out.vPosition;

    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), ObjectBufferArray[TransformIndex].Transform)).xyz;
    Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
    Out.viewZ = viewPos.z;

    return Out;
}
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float viewZ : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vMetalic : SV_TARGET3;
    vector vEmissive : SV_TARGET4;
    float2 fEmissiveInfo : SV_TARGET5;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.2)
    {
        discard;
    }
    Out.vDiffuse = vMtrlDiffuse;
    
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalic = MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vAmbient = AmbientTexture.Sample(DefaultSampler, In.vTexcoord);
    vAmbient.r = 0.f;
    
    float3 vEmissive = lerp(vMtrlDiffuse.rgb * 0.05f, vEmissiveColor, fEmissiveStrength);
    
    float3 vNormal;
    vNormal.xy = vNormalDesc.xy * 2.f - 1.f;
    vNormal.z = 1.f;
    float3 T = normalize(In.vTangent);
    float3 B = normalize(In.vBinormal * -1);
    float3 N = normalize(In.vNormal.xyz);

    float3x3 WorldMatrix = float3x3(T, B, N);
        
    vNormal = mul(vNormal, WorldMatrix);
    vMetalic.a = 0.6f;
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, vNormalDesc.z);
   
    if (vAmbient.g < 0.2)
        vAmbient.g = 1.f;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
    Out.vMetalic = vMetalic;
    Out.vEmissive = float4(vEmissive, 1.f);
    return Out;
}

PS_OUT PS_WALL(PS_IN In)
{
    PS_OUT Out;
    float2 BaseUV = In.vTexcoord;
    BaseUV.y -= fTime;
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, BaseUV);
    float vDissolve = DissolveTexture.Sample(DefaultSampler, BaseUV).r;
    
    if (vDissolve < 0.01f)
        discard;
    
    Out.vDiffuse = vDiffuse;
    
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalic = MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vAmbient = AmbientTexture.Sample(DefaultSampler, In.vTexcoord);
    vAmbient.r = 0.f;

    float3 vEmissive = lerp(vMtrlDiffuse.rgb * 0.05f, vEmissiveColor, fEmissiveStrength);

    float3 vNormalTS;
    vNormalTS.xy = vNormalDesc.xy * 2.f - 1.f;
    vNormalTS.z = 1.f;

    float3 tangent = normalize(In.vTangent);
    float3 binormal = normalize(In.vBinormal * -1);
    float3 normalW = normalize(In.vNormal.xyz);

    float3x3 tbn = float3x3(tangent, binormal, normalW);
    float3 vNormalW = mul(vNormalTS, tbn);

    vMetalic.a = 0.6f;

    Out.vNormal = vector(vNormalW.xyz * 0.5f + 0.5f, vNormalDesc.z);
    if (vAmbient.g < 0.2f)
        vAmbient.g = 1.f;

    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
    Out.vMetalic = vMetalic;
    //Out.vEmissive = float4(vEmissive, 1.f);
    //Out.fEmissiveInfo = float2(0.f, 0.f);
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    float3 worldPos = mul(float4(In.vPosition, 1.f),
                         ObjectBufferArray[TransformIndex].Transform).xyz;
    float4 lightSpacePos = mul(float4(worldPos, 1.f), matStaticLightViewProj[iCurrentCascade]);
    Out.vPosition = lightSpacePos;
    
    return Out;
}

struct PS_IN_SHDOW
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT_SHADOW
{
};

void PS_MAIN_SHADOW(PS_IN_SHDOW In)
{
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Wall
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WALL();
    }

    pass Shadow
    {
        SetRasterizerState(RS_Shadow);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
}
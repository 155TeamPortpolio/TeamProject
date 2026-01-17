#include "Shader_Define.hlsl"

float3 vRimLightColor;
float fRimLightPower;

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
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.2)
    {
        discard;
    }
  
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalic = MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vDiffuse = vMtrlDiffuse;
    
    if (vNormalDesc.a > 0.2f)
    {
        float3 vNormal;
        vNormal.x = vNormalDesc.y * 2.f - 1.f;
        vNormal.y = vNormalDesc.z * 2.f - 1.f;
        vNormal.z = 1.f;
        float3 T = normalize(In.vTangent);
        float3 B = normalize(In.vBinormal * -1);
        float3 N = normalize(In.vNormal.xyz);

        float3x3 WorldMatrix = float3x3(T, B, N);
        
        vNormal = mul(vNormal, WorldMatrix);
    
        Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 1.f);
    }
    else
    {
        float3 vNormal = normalize(In.vNormal);
        Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    }
    
    float linearDepth = saturate(In.viewZ / zFar);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, linearDepth, 1.f);
    Out.vMetalic = float4(vMetalic.rgb, 0.3f);
    Out.vEmissive = float4(vMtrlDiffuse.rgb * vMetalic.b * 1.f, vMetalic.a);
    Out.fEmissiveInfo = float2(0.f, 2.f);
    return Out;
}

PS_OUT PS_DEBUG(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.2)
    {
        vMtrlDiffuse = float4(In.vTexcoord, 1, 1);
    }
  
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal;
    vNormal.x = vNormalDesc.y * 2.f - 1.f;
    vNormal.y = vNormalDesc.z * 2.f - 1.f;
    vNormal.z = 1.f;
    
    float3 T = normalize(In.vTangent);
    float3 B = normalize(In.vBinormal * -1);
    float3 N = normalize(In.vNormal.xyz);

    float3x3 WorldMatrix = float3x3(T, B, N);

    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
    vector vMetalic = MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vMetalic = vMetalic;
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    //float4 vProjPos : TEXCOORD0;
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
   // float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
   //vector vShadow : SV_TARGET0;
};

void PS_MAIN_SHADOW(PS_IN_SHDOW In)
{
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }  
    pass Blend
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_ReadOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DEBUG();
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


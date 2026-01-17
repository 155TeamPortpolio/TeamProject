#include "Shader_Define.hlsl"

float3 color     = float3(1.f, 1.f, 1.f);
float  alpha     = 1.f;
float  alphaTest = -1.f;

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
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    float4 tex = DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    clip(tex.a - 0.05f);

    float4 outCol = float4(color, tex.a * alpha);

    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vDiffuse = outCol;

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

        Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    }
    else
    {
        float3 vNormal = normalize(In.vNormal.xyz);
        Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    }

    float linearDepth = saturate(In.viewZ / zFar);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, linearDepth, 1.f);

    return Out;
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}

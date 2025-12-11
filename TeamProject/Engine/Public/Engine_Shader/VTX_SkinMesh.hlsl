#include "Shader_Define.hlsl"

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
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
    
    matWV = mul(ObjectBufferArray[TransformIndex].Transform, matView);
    matWVP = mul(matWV, matProjection);
    
    float fWeightW = 1.0 - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    float4x4 BoneMatrix =
        g_BoneMatrices[SkinningOffset + In.vBlendIndex.x].BoneMat * In.vBlendWeight.x +
        g_BoneMatrices[SkinningOffset + In.vBlendIndex.y].BoneMat * In.vBlendWeight.y +
        g_BoneMatrices[SkinningOffset + In.vBlendIndex.z].BoneMat * In.vBlendWeight.z +
        g_BoneMatrices[SkinningOffset + In.vBlendIndex.w].BoneMat * fWeightW;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    
    float3 worldPos = mul(vPosition, ObjectBufferArray[TransformIndex].Transform).xyz;
    float4 viewPos = mul(float4(worldPos, 1.f), matView);
    float4 projPos = mul(viewPos, matProjection);

    Out.vPosition = projPos;
    
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = mul(vNormal, ObjectBufferArray[TransformIndex].Transform);
    Out.vProjPos = Out.vPosition;

    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), BoneMatrix)).xyz;
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
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vEmission : SV_TARGET3;
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
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
 
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vNormal = vector(vNormal.xyz*0.5f + 0.5f, 1.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
    Out.vEmission = EmmisionTexture.Sample(DefaultSampler, In.vTexcoord);

    return Out;
}


struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    float fWeightW = 1.0 - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    float4x4 BoneMatrix =
        g_BoneMatrices[SkinningOffset + In.vBlendIndex.x].BoneMat * In.vBlendWeight.x +
        g_BoneMatrices[SkinningOffset + In.vBlendIndex.y].BoneMat * In.vBlendWeight.y +
        g_BoneMatrices[SkinningOffset + In.vBlendIndex.z].BoneMat * In.vBlendWeight.z +
        g_BoneMatrices[SkinningOffset + In.vBlendIndex.w].BoneMat * fWeightW;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    
    float3 worldPos = mul(vPosition, ObjectBufferArray[TransformIndex].Transform).xyz;
    float4 viewPos = mul(float4(worldPos, 1.f), matShadowView);
    float4 projPos = mul(viewPos, matShadowProjection);
    
    Out.vPosition = projPos;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN_SHDOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
    vector vShadow : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHDOW In)
{
    PS_OUT_SHADOW Out;
 
    Out.vShadow = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zShadowFar, 0.f, 0.f);
  
    return Out;
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

    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
}


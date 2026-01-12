#include "Shader_Define.hlsl"

float3 vRimLightColor;
float fRimLightPower;
vector vOutLineColor;
float fOutLineThickness;
float fDissolveProgress;
float4x4 g_OutLineBoneMatrices[512];
matrix g_worldMatrix;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
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
    vector vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);
    vector vBinormal = mul(float4(In.vBinormal, 0.f), BoneMatrix);
    
    float3 worldPos = mul(vPosition, ObjectBufferArray[TransformIndex].Transform).xyz;
    float4 viewPos = mul(float4(worldPos, 1.f), matView);
    float4 projPos = mul(viewPos, matProjection);
        
    matrix matrixWV = mul(ObjectBufferArray[TransformIndex].Transform, matView);
    matrix matrixWVP = mul(matrixWV, matProjection);
    Out.vPosition = projPos;
    
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vNormal, ObjectBufferArray[TransformIndex].Transform));
    Out.vProjPos = Out.vPosition;

    Out.vTangent = normalize(mul(vTangent, ObjectBufferArray[TransformIndex].Transform));
    Out.vBinormal = normalize(mul(vBinormal, ObjectBufferArray[TransformIndex].Transform));
    Out.viewZ = viewPos.z;
    return Out;
}


VS_OUT VS_OUTLINE(VS_IN In)
{
    VS_OUT Out;
    
    float fWeightW = 1.0 - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    float4x4 BoneMatrix =
        g_OutLineBoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_OutLineBoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_OutLineBoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_OutLineBoneMatrices[In.vBlendIndex.w] * fWeightW;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    vector vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);
    vector vBinormal = mul(float4(In.vBinormal, 0.f), BoneMatrix);
    
    float3 worldPos = mul(vPosition, g_worldMatrix).xyz;
    vector worldNormal = normalize(mul(g_worldMatrix, vNormal));
    
    worldPos += worldNormal.xyz * fOutLineThickness;
    
    float4 viewPos = mul(float4(worldPos, 1.f), matView);
    float4 projPos = mul(viewPos, matProjection);

    Out.vPosition = projPos;
    
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vNormal, g_worldMatrix));
    Out.vProjPos = Out.vPosition;

    Out.vTangent = normalize(mul(vTangent, g_worldMatrix));
    Out.vBinormal = normalize(mul(vBinormal, g_worldMatrix));

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
    vector vAmbient : SV_Target4;
    vector vRimLight : SV_Target5;
    vector vLook : SV_Target6;
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
    
    float fNoise = NoiseTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    if (fNoise < fDissolveProgress)
        discard;
    
    vAmbient.r = 0.f;
    if(vNormalDesc.a > 0.f)
    {
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
    }
    else
    {
        float3 vNormal = normalize(In.vNormal);
        Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);

        float3 headRight = normalize(cross(float3(0.f, 1.f, 0.f), vLookVector.xyz));

        vMetalic = LightTexture.Sample(DefaultSampler, In.vTexcoord);

        vMetalic.a = 0.8f;
        Out.vLook = float4(vLookVector.xyz * 0.5f + 0.5f, 1.f);
    }
    if (vAmbient.g < 0.2) vAmbient.g = 1.f;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
    Out.vAmbient = vAmbient;
    Out.vMetalic = vMetalic;
    Out.vRimLight = float4(vRimLightColor, fRimLightPower);
    return Out;
}

PS_OUT PS_OUTLINE(PS_IN In)
{
    PS_OUT Out;
    
    Out.vDiffuse = vOutLineColor;
    return Out;
}

PS_OUT PS_DEBUG(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.2)
    {
        vMtrlDiffuse = float4(In.vTexcoord, 1, 1);
    }
  
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vDiffuse = vMtrlDiffuse;
    float3 vNormal;
    vNormal.xy = vNormalDesc.xy * 2.f - 1.f;
    vNormal.z = 1.f;
    float3 T = normalize(In.vTangent);
    float3 B = normalize(In.vBinormal * -1);
    float3 N = normalize(In.vNormal.xyz);

    float3x3 WorldMatrix = float3x3(T, B, N);

    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, vNormalDesc.z);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
    Out.vMetalic = MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vAmbient = AmbientTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
   // float4 vProjPos : TEXCOORD0;
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
    float4 lightSpacePos = mul(float4(worldPos, 1.f), matSkinnedLightViewProj[iCurrentCascade]);
    
    Out.vPosition = lightSpacePos;
   // Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN_SHDOW
{
    float4 vPosition : SV_POSITION;
    //float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
   // vector vShadow : SV_TARGET0;
};

void PS_MAIN_SHADOW(PS_IN_SHDOW In)
{

}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_WriteStencil,1);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
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

    pass OutLine
    {
        SetRasterizerState(RS_CullFront);
        SetDepthStencilState(DSS_OutlineStencil, 1);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_OUTLINE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE();
    }
}


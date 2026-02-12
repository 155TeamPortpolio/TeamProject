#include "Shader_Define.hlsl"

matrix g_WorldMatrix;

float3 vRimLightColor;
float3 vEmissiveColor;
float fRimLightPower;

int FrameIndex;
int Col;
int Row;

float2 UVTiling;

float g_Time;

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
    float3 vWorldPos : TEXCOORD3;
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
    Out.vWorldPos = worldPos; 
    return Out;
}

VS_OUT VS_ORTHOMAIN(VS_IN In)
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
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float viewZ : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float3 vWorldPos : TEXCOORD3;
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
    Out.fEmissiveInfo = float2(0.f, 1.f);
    return Out;
}

PS_OUT PS_EMISSIVE(PS_IN In)
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
    Out.vEmissive = float4(vEmissiveColor.rgb * vMetalic.b * 1.f, vMetalic.a);
    Out.fEmissiveInfo = float2(0.f, 1.f);
    return Out;
}

PS_OUT PS_TILING(PS_IN In)
{
    PS_OUT Out;

    vector vMtrlDiffuse = DiffuseTexture.Sample(LinearSampler, In.vTexcoord * UVTiling);
    
    if (vMtrlDiffuse.a < 0.2)
    {
        discard;
    }
  
    vector vNormalDesc = NormalTexture.Sample(LinearSampler, In.vTexcoord * UVTiling);
    vector vMetalic = MetalnessTexture.Sample(LinearSampler, In.vTexcoord * UVTiling);
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
    Out.vEmissive = float4(vEmissiveColor.rgb * vMetalic.b * 1.f, vMetalic.a);
    Out.fEmissiveInfo = float2(0.f, 1.f);
    return Out;
}

PS_OUT PS_TV(PS_IN In)
{
    PS_OUT Out;
    
    float2 TVTexcoord = CalculateFrameIndex(Col, Row, FrameIndex, In.vTexcoord);
    vector vMtrlDiffuse = DiffuseTexture.Sample(DefaultSampler, TVTexcoord);
    if (vMtrlDiffuse.a < 0.2)
    {
        discard;
    }
    vector vMetalic = MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vDiffuse = float4(vMtrlDiffuse.rgb, 1.f);
    
    float3 vNormal = normalize(In.vNormal);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float linearDepth = saturate(In.viewZ / zFar);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, linearDepth, 1.f);
    Out.vMetalic = float4(vMetalic.rgb, 0.3f);
    //Out.vEmissive = float4(vMtrlDiffuse.rgb * vMetalic.b * 1.f, vMetalic.a);
    //Out.fEmissiveInfo = float2(0.f, 1.f);
    return Out;
}

PS_OUT PS_UI(PS_IN In)
{
    PS_OUT Out;
    
    Out.vDiffuse = DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT PS_WATER(PS_IN In)
{
    PS_OUT Out;
    
    float3 deepWater = float3(0.01, 0.01, 0.02);
    float3 shallowWater = float3(0.02, 0.025, 0.04);
    float3 foamColor = float3(0.04, 0.045, 0.05);
    
    float worldTiling = 0.02;
    float2 worldUV = In.vWorldPos.xz * worldTiling;
    
    float2 flowDir1 = float2(0.0, 1.0); 
    float2 flowDir2 = float2(0.866, -0.5); 
    float2 flowDir3 = float2(-0.866, -0.5); 
    
    float2 uv1 = worldUV + flowDir1 * g_Time * 0.02;
    uv1 += float2(sin(g_Time * 0.2), cos(g_Time * 0.15)) * 0.02;
    
    float2 uv2 = worldUV + flowDir2 * g_Time * 0.03;
    uv2 += float2(cos(g_Time * 0.3), sin(g_Time * 0.25)) * 0.015;
    
    float2 uv3 = worldUV * 2.5 + flowDir3 * g_Time * 0.025;
    float rotAngle = g_Time * 0.05;
    float cosA = cos(rotAngle);
    float sinA = sin(rotAngle);
    float2x2 rotationMat = float2x2(cosA, -sinA, sinA, cosA);
    uv3 = mul(uv3, rotationMat);
    
    float3 normal1 = NormalTexture.Sample(LinearSampler, uv1).rgb * 2.0 - 1.0;
    normal1.xy *= 2.0;
    float3 normal2 = MetalnessTexture.Sample(LinearSampler, uv2).rgb * 2.0 - 1.0;
    normal2.xy *= 1.8;
    float4 detailSample = EmissiveTexture.Sample(LinearSampler, uv3);
    float3 detailNormal = detailSample.rgb * 2.0 - 1.0;
    detailNormal.xy *= 3.5;
    
    float3 waterNormal = normalize(normal1 * 0.5 + normal2 * 0.4 + detailNormal * 0.3);
    
    float3 viewDir = normalize(vCamPosition.xyz - In.vWorldPos);
    float NdotV = saturate(dot(waterNormal, viewDir));
    float fresnel = pow(1.0 - NdotV, 4.0);
    
    float depth = saturate(In.viewZ / 20.0);
    float3 waterColor = lerp(shallowWater, deepWater, depth);
    
    float3 lightDir = normalize(float3(0.3, 1.0, 0.2));
    float NdotL = dot(waterNormal, lightDir) * 0.5 + 0.5;
    waterColor *= lerp(0.3, 1.8, NdotL);
    
    float normalVariation = length(normal1.xy - normal2.xy);
    
    float glitter = saturate(normalVariation - 1.7) * 5.0;
    glitter = pow(glitter, 2.0);
    
    float flicker = sin(detailSample.b * 40.0 + g_Time * 3.0) * 0.5 + 0.5;
    flicker = pow(flicker, 2.0);
    
    glitter *= (0.6 + fresnel * 0.4);
    
    float3 glitterColor = float3(0.9, 0.85, 0.7);
    waterColor += glitter * flicker * glitterColor * 0.25;
    
    float wave = detailSample.g;
    waterColor += wave * 0.05 * float3(0.03, 0.035, 0.04);
    
    float foam = saturate(detailSample.r * 3.0 - 2.0);
    foam *= (1.0 - depth) * 0.25;
    waterColor = lerp(waterColor, foamColor, foam);
    
    float3 reflectionColor = float3(0.2, 0.23, 0.28);
    waterColor = lerp(waterColor, reflectionColor, fresnel * 0.03);
    
    float alpha = 0.85 + depth * 0.15;
    Out.vDiffuse = float4(waterColor, alpha);
    
    Out.vNormal = float4(waterNormal * 0.5 + 0.5, 1.0);
    
    float linearDepth = saturate(In.viewZ / zFar);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, linearDepth, 1.f);
    
    float roughness = 0.05 + depth * 0.08;
    float metallic = 0.95;
    Out.vMetalic = float4(roughness, metallic, 0.0, 1.0);
    
    return Out;
}

PS_OUT PS_DEBUG(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
     vMtrlDiffuse = float4(In.vTexcoord, 1, 1);
  
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
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }  
    pass Water
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WATER();
    }
    pass Emissive
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EMISSIVE();
    }
    pass NoCull
    {
        SetRasterizerState(RS_NoCull);
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
    pass Tiling
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TILING();
    }
    pass TV
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TV();
    }
    pass UI
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_WriteOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_ORTHOMAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UI();
    }
    pass Debug
    {
        SetRasterizerState(RS_NoCull);
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


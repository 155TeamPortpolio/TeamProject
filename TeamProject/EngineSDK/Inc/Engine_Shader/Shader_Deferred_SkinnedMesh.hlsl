#include "Shader_Deferred_Define.hlsl"

matrix g_WorldMatrix;

int GetCascadeIndex(float fViewDepth)
{
    int cascadeIndex = 3;
    if (fViewDepth < vCascadeSplits.x)
        cascadeIndex = 0;
    else if (fViewDepth < vCascadeSplits.y)
        cascadeIndex = 1;
    else if (fViewDepth < vCascadeSplits.z)
        cascadeIndex = 2;
    
    return cascadeIndex;
}

float GetShadowBias(float3 normal, float3 lightDir, int cascadeIndex)
{
    float cosTheta = saturate(dot(normal, lightDir));
    float baseBias = 0.005f * (cascadeIndex + 1); // cascade별로 다른 bias
    return baseBias * tan(acos(cosTheta));
}

float SampleShadowMap(float3 shadowCoord, int cascadeIndex, float bias)
{
    float shadow = 0.0f;
    float2 texelSize = 1.0f / 2048.0f;
    
    [unroll]
    for (int x = -1; x <= 1; x++)
    {
        [unroll]
        for (int y = -1; y <= 1; y++)
        {
            float2 offset = float2(x, y) * texelSize;
            shadow += ShadowMapArray.SampleCmpLevelZero(
                ShadowSampler,
                float3(shadowCoord.xy + offset, cascadeIndex),
                shadowCoord.z - bias
            );
        }
    }
    
    return shadow / 9.0f;
}

float CalculateShadow(float4 vLightSpacePos[4], float fViewDepth, float3 worldNormal, float3 lightDir)
{
    int cascadeIndex = GetCascadeIndex(fViewDepth);
    
    float4 shadowCoord = vLightSpacePos[cascadeIndex];
    shadowCoord.xyz /= shadowCoord.w;
    
    shadowCoord.x = shadowCoord.x * 0.5f + 0.5f;
    shadowCoord.y = shadowCoord.y * -0.5f + 0.5f;
    
    if (shadowCoord.x < 0.0f || shadowCoord.x > 1.0f ||
        shadowCoord.y < 0.0f || shadowCoord.y > 1.0f ||
        shadowCoord.z < 0.0f || shadowCoord.z > 1.0f)
    {
        return 1.0f;
    }
    
    float bias = GetShadowBias(worldNormal, lightDir, cascadeIndex);
    return SampleShadowMap(shadowCoord.xyz, cascadeIndex, bias);
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
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
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_RESULT
{
    vector vResult : SV_TARGET0;
};

PS_OUT_RESULT PS_OUTLINERIMLIGHT(PS_IN In)
{
    PS_OUT_RESULT Out;

    float4 vRimInfo = RimLightTexture.Sample(DefaultSampler, In.vTexcoord);

    if (vRimInfo.a <= 0.f)
    {
        Out.vResult = float4(0.f, 0.f, 0.f, 0.f);
        return Out;
    }

    float4 vNormalDesc = NormalTexture.Sample(PointClampSampler, In.vTexcoord);
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    float4 vDepthDesc = DepthTexture.Sample(PointClampSampler, In.vTexcoord);

    float fViewZ = vDepthDesc.y * zFar;

    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);
    float NdotV = saturate(dot(worldNormal, viewDir));
    
    float rimPower = 1.f - NdotV;
    float fresnelPower = lerp(2.0, 8.0, vRimInfo.a);
    rimPower = pow(rimPower, fresnelPower);

    float fRim = smoothstep(0.45, 0.55, rimPower);

    float3 vRimColor = vRimInfo.rgb * fRim;
    Out.vResult = float4(vRimColor, 1.f);

    return Out;
}

PS_OUT_RESULT PS_RIMLIGHT(PS_IN In)
{
    PS_OUT_RESULT Out;

    float4 vRimInfo = RimLightTexture.Sample(PointSampler, In.vTexcoord);

    if (vRimInfo.a <= 0.f)
    {
        Out.vResult = float4(0.f, 0.f, 0.f, 0.f);
        return Out;
    }

    float4 vNormal = NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vDepthDesc = DepthTexture.Sample(PointSampler, In.vTexcoord);

    float fViewZ = vDepthDesc.y * zFar;

    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);
    vNormal = float4(vNormal.xyz * 2.f - 1.f, 0.f);

    float fRim = 1.f - saturate(dot(normalize(vNormal.xyz), viewDir));
    fRim = pow(fRim, vRimInfo.a);

    float3 vRimColor = vRimInfo.rgb * fRim;

    Out.vResult = float4(vRimColor, 1.f);

    return Out;
}

//GaussianBlur
static const float weights[9] =
{
    0.2270270270,
    0.1945945946,
    0.1216216216,
    0.0540540541,
    0.0162162162,
    0.0081081081,
    0.0040540541,
    0.0020270270,
    0.0010135135
};
PS_OUT_RESULT PS_BRIGHT(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float emissive = AmbientTexture.Sample(DefaultSampler, In.vTexcoord).b;
    float4 vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
  
    float3 vResult = vDiffuse.rgb * emissive;
    
    Out.vResult = float4(vResult, 1.f);
    return Out;
}

PS_OUT_RESULT PS_BLOOM_BLURX(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 bright = MeshBrightTexture.Sample(DefaultSampler, In.vTexcoord);

    float3 result = bright.rgb * weights[0];
    float texelSize = 0.5f / fScreenWidth;
    
    float4 brightSample;
    for (int i = 1; i < 9; ++i)
    {
        brightSample = MeshBrightTexture.Sample(DefaultSampler, In.vTexcoord + float2(texelSize * i, 0));
        result += brightSample.rgb * weights[i];
            
        brightSample = MeshBrightTexture.Sample(DefaultSampler, In.vTexcoord - float2(texelSize * i, 0));
        result += brightSample.rgb * weights[i];
    }
        
    Out.vResult = float4(result, 1.f);
   
    return Out;
}

PS_OUT_RESULT PS_BLOOM_BLURY(PS_IN In)
{
    PS_OUT_RESULT Out;

    float4 BlurX = MeshBlurXTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 result = BlurX.rgb * weights[0];
    float texelSize = 0.5f / fScreenHeight;
        
    for (int i = 1; i < 9; ++i)
    {
        result += MeshBlurXTexture.Sample(DefaultSampler,
                In.vTexcoord + float2(0, texelSize * i)).rgb * weights[i];
        result += MeshBlurXTexture.Sample(DefaultSampler,
                In.vTexcoord - float2(0, texelSize * i)).rgb * weights[i];
    }
    Out.vResult = float4(result, 1.f);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vLight : SV_TARGET0;
    float2 fLightInfo : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalicDesc = MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    
    float roughness = vMetalicDesc.r;
    float metalic = vMetalicDesc.g;
    float specular = vMetalicDesc.b;
    
    float Skin = vMetalicDesc.a;
    
    float3 lightDir = normalize(vLightDir.xyz * -1);
    float fViewZ = vDepthDesc.a;
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float4 vLightSpacePos[4];
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        vLightSpacePos[i] = mul(vWorldPos, matLightViewProj[i]);
    }
    
    float shadow = CalculateShadow(vLightSpacePos, fViewZ, worldNormal, lightDir);
    
    if (Skin < 0.7f)
    {
        float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);

        float NdotL = dot(worldNormal, lightDir) * 0.5f + 0.5f;

        float3 halfVec = normalize(viewDir + lightDir);
        float specBase = saturate(dot(worldNormal, halfVec));
        float specularPower = lerp(50.0f, 5.0f, roughness);
        specular = pow(specBase, specularPower) * specular;
        
        float3 PBR = CalculateDirectionalLight(vDiffuse.rgb, worldNormal, metalic, roughness, viewDir, lightDir, vLightDiffuse.rgb, fLightIntensity, shadow);
    
        Out.vLight = float4(PBR * vNormalDesc.a, 1.f);      
        Out.fLightInfo = float2(NdotL, specular);
        return Out;
    }
    else
    {
        vector LightDesc = vMetalicDesc;
        float3 vLookVector = normalize(FaceDirTexture.Sample(DefaultSampler, In.vTexcoord).xyz * 2.f - 1.f);
        
        float3 headRight = normalize(cross(float3(0, 1, 0), vLookVector));

        float RdotL = dot(headRight, lightDir);
        float FdotL = dot(vLookVector, lightDir);
        
        float faceShadow = LightDesc.r;
        float specularMask = LightDesc.g;
    
        faceShadow *= saturate(-FdotL);
        float brightness = lerp(0.15f, 0.45f, faceShadow);

        Out.vLight = float4(vDiffuse.rgb * vLightDiffuse.rgb * brightness * vNormalDesc.a, 1.f);
        Out.fLightInfo = float2(brightness, 0);
    }
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalicDesc = MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if(vMetalicDesc.a > 0.7f)
    {
        Out.vLight = float4(0.f, 0.f, 0.f, 1.f);
        Out.fLightInfo = float2(0.f, 0.f);
        return Out;
    }
    
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    
    float roughness = vMetalicDesc.r;
    float metalic = vMetalicDesc.g;
 
    float fViewZ = vDepthDesc.y * zFar;
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float3 lightDir = normalize(vLightPos.xyz - vWorldPos.xyz);
    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);
    
    float NdotL = dot(worldNormal, lightDir) * 0.5f + 0.5f;
    
    float3 PBR = CalculatePointLight
    (vDiffuse.rgb, worldNormal, metalic, roughness, vWorldPos.xyz, viewDir, lightDir, vLightDiffuse.rgb,
    fLightIntensity, vLightPos.xyz, fLightRange, 1.0f);
    
    Out.vLight = float4(PBR * vNormalDesc.a, 1.f);
    Out.fLightInfo = float2(NdotL, 0.f);
    
    return Out;
}

PS_OUT_RESULT PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vLight = LightTexture.Sample(DefaultSampler, In.vTexcoord);
    float2 fLightInfo = LightInfoTexture.Sample(DefaultSampler, In.vTexcoord).rg;
    vector vAmbient = AmbientTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vRimLight = RimLightFinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float fOutLine = NormalTexture.Sample(DefaultSampler, In.vTexcoord).a;
    vector vMetalic = MetalicTexture.Sample(DefaultSampler, In.vTexcoord).a;
    vector vBloom = MeshBloomFinalTexture.Sample(DefaultSampler, In.vTexcoord);

    float NdotL = fLightInfo.r;
    float2 vRampCoord = float2(1 - NdotL, 0.5f);
    vector vRampSample = RampTexture.Sample(DefaultSampler, vRampCoord);
    float vRamp = lerp(0.1f, 1.0f, vRampSample.g);
    
    float3 ambient = vLightAmbient.rgb * vDiffuse.rgb * vAmbient.g * fOutLine;
    ambient = max(ambient, vDiffuse.rgb * vLightAmbient.rgb * 0.05);
    
    if (vMetalic.a < 0.7) Out.vResult = float4(vLight.rgb * vRamp + ambient, 1.f);
    else Out.vResult = float4(vLight.rgb, 1.f);
    
    float rimIntensity = max(vRamp, 0.5f);
    Out.vResult.rgb += vRimLight.rgb * rimIntensity;
    
    float3 specularColor = vLightSpecular.rgb * fLightInfo.g;
    Out.vResult.rgb += specularColor + vBloom.rgb;
    return Out;
}


technique11 DefaultTechnique
{
    pass OUTLINERIMLIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINERIMLIGHT();
    }

    pass RIMLIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RIMLIGHT();
    }

    pass BRIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRIGHT();
    }

    pass BLOOM_BLURX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOM_BLURX();
    }

    pass BLOOM_BLURY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOM_BLURY();
    }

    pass DIRECTIONAL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass POINT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass COMBINED
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }
}
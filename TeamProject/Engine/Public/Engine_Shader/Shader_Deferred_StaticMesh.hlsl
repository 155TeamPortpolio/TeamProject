#include "Shader_Deferred_Define.hlsl"
#include "Shader_Shadow.hlsl"
matrix g_WorldMatrix;


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
    
    matrix matWVP = mul(g_WorldMatrix, matOrthograph);
    
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

PS_OUT_RESULT PS_SSAO(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vDepthDesc = DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
    float fViewZ = vDepthDesc.y * zFar;
    
    float3 N = mul(float4(worldNormal, 0.f), matView).xyz;
    N = normalize(N);
    
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);

    float3 fragPos = mul(vWorldPos, matView).xyz;
    
    float2 noiseScale = float2(fScreenWidth / 8.0, fScreenHeight / 8.0);
    float3 randomVec = SSAONoiseTexture.Sample(DefaultSampler, In.vTexcoord * noiseScale).xyz;
    randomVec = randomVec * 2.0 - 1.0;
    
    float3 T = normalize(randomVec - N * dot(randomVec, N));
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    
    float occlusion = 0.0;
    
    for (int i = 0; i < 64; ++i)
    {
        float3 samplePos = mul(SSAOKernel[i].xyz, TBN);
        samplePos = fragPos + samplePos * fRadius;
        
        float4 offset = float4(samplePos, 1.0);
        offset = mul(offset, matProjection);
        offset.xyz /= offset.w;
        
        offset.xy = offset.xy * 0.5 + 0.5;
        offset.y = 1.0 - offset.y;
        
        vector vSampleDepthDesc = DepthTexture.Sample(DefaultSampler, offset.xy);
        float fSampleViewZ = vSampleDepthDesc.y * zFar;
        
        vector vSampleWorldPos;
        vSampleWorldPos.x = offset.x * 2.f - 1.f;
        vSampleWorldPos.y = offset.y * -2.f + 1.f;
        vSampleWorldPos.z = vSampleDepthDesc.x;
        vSampleWorldPos.w = 1.f;
        
        vSampleWorldPos = vSampleWorldPos * fSampleViewZ;
        vSampleWorldPos = mul(vSampleWorldPos, matProjectionInverse);
        vSampleWorldPos = mul(vSampleWorldPos, matViewInverse);
        
        float3 sampleViewPos = mul(vSampleWorldPos, matView).xyz;
        float sampleDepth = sampleViewPos.z;
        
        float rangeCheck = smoothstep(0.0, 1.0, fRadius / abs(fragPos.z - sampleDepth));
        
        occlusion += (sampleDepth <= samplePos.z + fBias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / 64.0);
    Out.vResult = occlusion;
    
    return Out;
}

PS_OUT_RESULT PS_SSAO_BLUR(PS_IN In)
{
    PS_OUT_RESULT Out;
    float2 texelSize = 1.0 / float2(fScreenWidth, fScreenHeight);

    const float weights[5] = { 0.06136, 0.24477, 0.38774, 0.24477, 0.06136 };
    
    float result = 0.0;
    
    for (int x = -2; x <= 2; ++x)
    {
        for (int y = -2; y <= 2; ++y)
        {
            float2 offset = float2(float(x), float(y)) * texelSize;
            float weight = weights[x + 2] * weights[y + 2];
            result += SSAOTexture.Sample(DefaultSampler, In.vTexcoord + offset).r * weight;
        }
    }
    
    Out.vResult = result;
    
    return Out;
}

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

PS_OUT_RESULT PS_BLOOM_BLURX(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector BloomInfo = MeshBloomInfo.Sample(DefaultSampler, In.vTexcoord);
    float Strength = BloomInfo.g;

    float4 bright = MeshBrightTexture.Sample(DefaultSampler, In.vTexcoord);

    float3 result = bright.rgb * weights[0];
    float texelSize = Strength / fScreenWidth;
    
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
    
    vector BloomInfo = MeshBloomInfo.Sample(DefaultSampler, In.vTexcoord);
    float Strength = BloomInfo.g;

    float4 BlurX = MeshBlurXTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 result = BlurX.rgb * weights[0];
    float texelSize = Strength / fScreenHeight;
        
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
    vector vLightInfo : SV_TARGET1;
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
    float3 lightDir = normalize(vLightDir.xyz * -1);
    float fViewZ = vDepthDesc.y * zFar;
    
    vector vWorldPos;
    
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    

    float shadow = CalculateShadow(vWorldPos, fViewZ, worldNormal, lightDir, In.vTexcoord);
    
    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);
        
    float NdotL = saturate(dot(worldNormal, lightDir));

    float3 halfVec = normalize(viewDir + lightDir);
    float specBase = saturate(dot(worldNormal, halfVec));
    float specularPower = lerp(50.0f, 5.0f, roughness);
    specular = pow(specBase, specularPower) * specular;
    
    float3 PBR = CalculateDirectionalLight(vDiffuse.rgb, worldNormal, metalic, roughness, viewDir, lightDir, vLightDiffuse.rgb, fLightIntensity, 1.f);
        
    Out.vLight = float4(PBR * vNormalDesc.a, 1.f);
    Out.vLightInfo = float4(NdotL, specular, shadow, 1.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalicDesc = MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
    
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
    
    float NdotL = saturate(dot(worldNormal, lightDir));
    
    float3 PBR = CalculatePointLight
    (vDiffuse.rgb, worldNormal, metalic, roughness, vWorldPos.xyz, viewDir, lightDir, vLightDiffuse.rgb,
    fLightIntensity, vLightPos.xyz, fLightRange, 1.0f);
    
    Out.vLight = float4(PBR * vNormalDesc.a, vDiffuse.a);
    Out.vLightInfo = float4(NdotL, 0.f, 0.f, 0.f);
    
    return Out;
}
PS_OUT_LIGHT PS_MAIN_SPOTLIGHT(PS_IN In)
{
    PS_OUT_LIGHT Out;

    vector vNormalDesc = NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMetalicDesc = MetalicTexture.Sample(DefaultSampler, In.vTexcoord);

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

    float NdotL = saturate(dot(worldNormal, lightDir));

    float3 PBR = CalculateSpotLight(
        vDiffuse.rgb, worldNormal, metalic, roughness, vWorldPos.xyz,
        viewDir,
        vLightDiffuse.rgb, fLightIntensity,
        vLightPos.xyz, fLightRange,
        normalize(vLightDir.xyz), // 라이트 전방
        fInnerCos, fOuterCos,
        1.0f // shadowFactor (일단 1)
    );

    Out.vLight = float4(PBR * vNormalDesc.a, vDiffuse.a);
    Out.vLightInfo = float4(NdotL, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_RESULT PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    vector vDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vLight = LightTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vBloom = MeshBloomFinalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vLightInfo = LightInfoTexture.Sample(DefaultSampler, In.vTexcoord);
    float ssao = SSAOFinalTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    float NdotL = vLightInfo.r;
    
    float shadowValue = vLightInfo.b;
    shadowValue = saturate(shadowValue * 0.7f + 0.3f);
    
    float3 vAmbient = vLightAmbient.rgb * vDiffuse.rgb * ssao * shadowValue;
    vAmbient = max(vAmbient, vDiffuse.rgb * vLightAmbient.rgb * 0.05);
    float4 vResult = float4(vLight.rgb + vAmbient, vDiffuse.a);
    
    float3 specularColor = vLightSpecular.rgb * vLightInfo.g;
    vResult.rgb += specularColor + vBloom.rgb;
    
    Out.vResult = vResult;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SSAO();
    }

    pass SSAO_BLUR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SSAO_BLUR();
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
        SetBlendState(BS_Additive_MaxAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass POINT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive_MaxAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass SPOTLIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive_MaxAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOTLIGHT();
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

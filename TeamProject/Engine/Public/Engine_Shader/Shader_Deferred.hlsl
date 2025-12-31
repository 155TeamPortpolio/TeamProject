#include "Shader_Deferred_Define.hlsl"

matrix g_WorldMatrix;

float g_FogDensity;
float4 g_FogColor;
float g_Time;

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

struct PS_OUT_BACKBUFFER
{
    vector vBackBuffer : SV_TARGET0;
};

struct PS_OUT_LIGHT
{
    vector vLight : SV_TARGET0;
    float2 fLightInfo : SV_TARGET1;
};

struct PS_OUT_RESULT
{
    vector vResult : SV_TARGET0;
};

PS_OUT_RESULT PS_FOG(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 vDepthDesc = DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vScene = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fViewZ = vDepthDesc.y * zFar;
    
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float3 vViewDir = vWorldPos.xyz - vCamPosition.xyz;
    float fDistance = length(vViewDir);
    
    float fFogFactor = exp(-g_FogDensity * fDistance);
    
    fFogFactor = saturate(fFogFactor);
    fFogFactor = lerp(0.3f, 1.0f, fFogFactor);
    
    float4 vFoggedColor = lerp(g_FogColor, vScene, fFogFactor);
  
    if (vDepthDesc.x >= 0.9999f)
    {
        vFoggedColor = vScene;
    }
    
    Out.vResult = vFoggedColor;
    
    return Out;
}

PS_OUT_RESULT PS_HDR_BRIGHTPASS(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float4 scene = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 bright = SoftExtractBright(scene);
    
    Out.vResult = bright;

    return Out;
}

PS_OUT_RESULT PS_HDR_BLURH(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float2 texelSize = 1.0 / float2(fScreenWidth, fScreenHeight);
    float3 result = 0;
    
    float weights[3] = { 0.398942, 0.241971, 0.053991 };
    
    result = HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord).rgb * weights[0];
    
    for (int i = 1; i < 3; ++i)
    {
        float2 offset = float2(texelSize.x * i, 0);
        result += HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord + offset).rgb * weights[i];
        result += HDRBrightTexture.Sample(DefaultSampler, In.vTexcoord - offset).rgb * weights[i];
    }
    
    Out.vResult = float4(result, 1.0);
    return Out;
}

PS_OUT_RESULT PS_HDR_BLURV(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    float2 texelSize = 1.0 / float2(fScreenWidth, fScreenHeight);
    float3 result = 0;

    float weights[3] = { 0.398942, 0.241971, 0.053991 };

    result = HDRBlurXTexture.Sample(DefaultSampler, In.vTexcoord).rgb * weights[0];
    
    for (int i = 1; i < 3; ++i)
    {
        float2 offset = float2(0, texelSize.y * i);
        result += HDRBlurXTexture.Sample(DefaultSampler, In.vTexcoord + offset).rgb * weights[i];
        result += HDRBlurXTexture.Sample(DefaultSampler, In.vTexcoord - offset).rgb * weights[i];
    }
    
    Out.vResult = float4(result, 1.0);
    
    return Out;
}

PS_OUT_RESULT PS_DISTORTION_ADD(PS_IN In)
{
    PS_OUT_RESULT Out;
    
    // ============================================
    // �׽�Ʈ�� �Ķ���� (���⼭ ����)
    // ============================================
    float2 center = float2(0.5, 0.5); // ȭ�� �߾�
    float radius = 0.3; // ȿ�� �ݰ� (�׽�Ʈ: 0.2 ~ 0.5)
    float power = 2.0; // � ���� (�׽�Ʈ: 1.0 ~ 5.0)
    float strength = 0.8; // �ְ� ���� (�׽�Ʈ: -1.0 ~ 1.0)
    
    // ============================================
    // Spherical Distortion ���
    // ============================================
    float2 offset = In.vTexcoord - center;
    
    // Aspect Ratio ����
    offset.x *= 1.777;
    
    float distance = length(offset);
    
    // �ݰ� ��
    if (distance > radius || distance < 0.0001)
    {
        Out.vResult = float4(0.5, 0.5, 0.5, 1.0);
        return Out;
    }
    
    // ����ȭ�� �Ÿ�
    float normalizedDist = distance / radius;
    
    // �߽ɿ��� ���ϰ�
    float distortStrength = 1.0 - normalizedDist;
    distortStrength = pow(distortStrength, power);
    
    // ���� ���
    float2 direction = normalize(offset);
    float2 distortion = direction * distortStrength * strength;
    
    // Aspect ������
    distortion.x /= 1.777;
    
    // ����
    Out.vResult = float4(distortion + 0.5, 0.5, 1.0);
    
    return Out;
}

//PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
//{
//    PS_OUT_LIGHT Out;
    
//    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
//    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
//    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
//    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
   
//    float fSkin = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).a;
    
//    float fViewZ = vDepthDesc.y * zFar;
    
//    vector vWorldPos;
//    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
//    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
//    vWorldPos.z = vDepthDesc.x;
//    vWorldPos.w = 1.f;
    
//    vWorldPos = vWorldPos * fViewZ;
//    vWorldPos = mul(vWorldPos, matProjectionInverse);
//    vWorldPos = mul(vWorldPos, matViewInverse);
    
//    float3 lightDir = normalize(vLightDir.xyz * -1);
//    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);

//    float NdotL = dot(worldNormal, lightDir) * 0.5f + 0.5f;
    
//    if (fSkin < 0.7f)
//    {
//        float roughness = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).r;
//        float metalic = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).g;
//        float specular = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).b;

//        float3 halfVec = normalize(viewDir + lightDir);
//        float specBase = saturate(dot(worldNormal, halfVec));
//        float specularPower = lerp(50.0f, 5.0f, roughness);
//        specular = pow(specBase, specularPower) * specular;
        
//        float3 PBR = CalculateDirectionalLight(vDiffuse.rgb, worldNormal, metalic, roughness, viewDir, lightDir, vLightDiffuse.rgb, fLightIntensity, 1.f);
//        Out.vLight = float4(PBR * vNormalDesc.a, 1.f);
//        Out.fLightInfo = float2(NdotL, specular);
        
//    }
//    else
//    {
//        vector LightDesc = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord);
//        float3 vLookVector = normalize(g_LookTexture.Sample(DefaultSampler, In.vTexcoord).xyz * 2.f - 1.f);
        
//        float3 headRight = normalize(cross(float3(0, 1, 0), vLookVector));

//        float RdotL = dot(headRight, lightDir);
//        float FdotL = dot(vLookVector, lightDir);
        
//        float faceShadow = LightDesc.r;
//        float specularMask = LightDesc.g;
    
//        faceShadow *= saturate(-FdotL);
//        float brightness = lerp(0.15f, 0.45f, faceShadow);

//        Out.vLight = float4(vDiffuse.rgb * vLightDiffuse.rgb * brightness * vNormalDesc.a, 1.f);
//        Out.fLightInfo = float2(brightness, 0);
//    }
//    return Out;
//}

//PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
//{
//    PS_OUT_LIGHT Out;
    
//    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
//    float3 worldNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);
//    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
//    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
//    float roughness = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).r;
//    float metalic = g_MetalicTexture.Sample(DefaultSampler, In.vTexcoord).g;
 
//    float fViewZ = vDepthDesc.y * zFar;
    
//    vector vWorldPos;
    
//    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
//    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
//    vWorldPos.z = vDepthDesc.x;
//    vWorldPos.w = 1.f;
    
//    vWorldPos = vWorldPos * fViewZ;
//    vWorldPos = mul(vWorldPos, matProjectionInverse);
//    vWorldPos = mul(vWorldPos, matViewInverse);
    
//    float3 lightDir = normalize(vLightPos.xyz - vWorldPos.xyz);
//    float3 viewDir = normalize(vCamPosition.xyz - vWorldPos.xyz);
    
//    float NdotL = dot(worldNormal, lightDir) * 0.5f + 0.5f;
    
//    float3 PBR = CalculatePointLight
//    (vDiffuse.rgb, worldNormal, metalic, roughness, vWorldPos.xyz, viewDir, lightDir, vLightDiffuse.rgb,
//    fLightIntensity, vLightPos.xyz, fLightRange, 1.0f);
    
//    Out.vLight = float4(PBR * vNormalDesc.a, 1.f);
//    Out.fLightInfo = float2(NdotL, 0.f);
    
//    return Out;
//}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vSkinned = SkinnedCombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vStatic = StaticCombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vUI = UICombinedTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 result = vSkinned.rgb;
    
    if (vStatic.a > 0.01f) result = vStatic.rgb;
    
    if (vUI.a > 0.01f) result = vUI.rgb;
    
    Out.vBackBuffer = float4(result, 1.f);
    return Out;
}

float4 PS_MAIN_FINAL(PS_IN In) : SV_Target
{
    float4 scene = FinalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 hdrBloom = HDRBloomFinalTexture.Sample(DefaultSampler, In.vTexcoord);
   // float4 effectbloom = BloomFinal.Sample(DefaultSampler, In.vTexcoord);
    //float4 ui = UITexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 hdrColor = scene.rgb;
    hdrColor += hdrBloom.rgb * 0.3;
    //if (effectbloom.a > 0.f)
    //    hdrColor += effectbloom.rgb * effectbloom.a;
    
    float3 mapped = ACESFilm(hdrColor);
    //float3 finalColor = lerp(mapped, ui.rgb, ui.a);
    
    return float4(mapped, 1.f);
}

technique11 DefaultTechnique
{
    pass HDR_BRIGHT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HDR_BRIGHTPASS();
    }

    pass HDR_BLURH
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HDR_BLURH();
    }

    pass HDR_BLURV
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HDR_BLURV();
    }

    pass DISTORTION_ADD
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION_ADD();
    }

    pass FOG
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FOG();
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

    pass FINAL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }
}
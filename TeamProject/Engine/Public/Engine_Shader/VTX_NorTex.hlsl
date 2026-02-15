#include "Shader_Define.hlsl"

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    matrix matWV, matWVP;
    
    matWV = mul(ObjectBufferArray[TransformIndex].Transform, matView);
    matWVP = mul(matWV, matProjection);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = mul(vector(In.vNormal, 0.f), ObjectBufferArray[TransformIndex].Transform);
    Out.vProjPos = Out.vPosition;   
    return Out;
}

VS_OUT VS_BILLBOARD(VS_IN In)
{
    VS_OUT Out;
    matrix matWV, matWVP;

    float3 objectWorld = mul(float4(0, 0, 0, 1), ObjectBufferArray[TransformIndex].Transform).xyz;
    
    float2 worldScale;
    worldScale.x = length(ObjectBufferArray[TransformIndex].Transform._11_12_13);
    worldScale.y = length(ObjectBufferArray[TransformIndex].Transform._21_22_23);

    float3 camRight = normalize(matViewInverse._11_12_13); // X축
    float3 camUp = normalize(matViewInverse._21_22_23); // Y축
    float3 camForward = normalize(matViewInverse._31_32_33); // Z축

    // 정점 위치(로컬 quad 좌표) 적용
    float3 worldPos = objectWorld + camRight * In.vPosition.x * worldScale.x + camUp * In.vPosition.y * worldScale.y;

    float4 vPos = mul(float4(worldPos, 1.f), matView);
    Out.vPosition = mul(vPos, matProjection);

    // 텍스처 좌표
    Out.vTexcoord = In.vTexcoord;

    // Normal은 카메라를 향하도록 고정 (조명 넣는다면 기존 메시 normals 불필요)
    Out.vNormal = float4(normalize(-camForward), 0.f);

    Out.vProjPos = Out.vPosition;
    return Out;
}

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
VS_OUT VS_MAIN_CUSTOM(VS_IN In)
{
    // 월드, 뷰, 프로젝션 행렬을 따로 던져서 버텍스 셰이더 처리
    VS_OUT Out;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
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
    
    //이거는 래핑한다는뜻, 30을 곱해서 반복적인 타일을 만들어주는 것
    vector vMtrlDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
   
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
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

    float3 worldPos = mul(float4(In.vPosition, 1.f), ObjectBufferArray[TransformIndex].Transform).xyz;
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

PS_OUT PS_MAIN_UI(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.001f)
        discard;
       
    vector vMaskDesc = MaskTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vMaskDesc.y * zFar;

    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vMaskDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, matProjectionInverse);
    vWorldPos = mul(vWorldPos, matViewInverse);
    
    float2 vTexcoord;
    vTexcoord.x = vWorldPos.x / vWorldPos.w * 0.5f + 0.5f;
    vTexcoord.y = vWorldPos.y / vWorldPos.w * -0.5f + 0.5f;
    
    vMaskDesc = MaskTexture.Sample(DefaultSampler, vTexcoord);
    
    if (vWorldPos.w - 0.01f < vMaskDesc.y * zFar)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, 0.f, 1.f);
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
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Billboard
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_BILLBOARD();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteOnly, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_CUSTOM();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UI();
    }
}


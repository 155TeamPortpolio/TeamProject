#include "Shader_Define.hlsl"

vector g_Color = { 1.f, 1.f, 0.f, 1.f };
float4x4 g_WorldMatrix =
{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

struct VS_IN
{
	float3 vPosition    : POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexcoord    : TEXCOORD0;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    
    float4 iRight       : INSTANCE0;
    float4 iUp          : INSTANCE1;
    float4 iLook        : INSTANCE2;
    float4 iTranslation : INSTANCE3;
    float fBrightness   : INSTANCE4;
}; 

struct VS_OUT
{
	float4 vWorldPos    : SV_POSITION;
	float2 vTexcoord    : TEXCOORD0;
	float fBrightness   : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;
    
    row_major float4x4 instWorld = float4x4(In.iRight, In.iUp, In.iLook, In.iTranslation);
    
    float3 camRight = matViewInverse[0].xyz;
    float3 camUp = matViewInverse[1].xyz;

    float3 billboardPos =
        In.vPosition.x * camRight +
        In.vPosition.y * camUp +
        In.iTranslation.xyz;

    float4 worldPos = float4(billboardPos, 1.f);
    
    matrix VP = mul(matView, matProjection);
    matrix WVP = mul(g_WorldMatrix, VP);
    Out.vWorldPos = mul(worldPos, WVP);
	Out.vTexcoord = In.vTexcoord;
	Out.fBrightness = In.fBrightness;
   
	return Out;
}

struct PS_IN
{
	float4 vWorldPos    : SV_POSITION;
	float2 vTexcoord    : TEXCOORD0;
	float fBrightness   : TEXCOORD1;
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
    if (vMtrlDiffuse.a < 0.1f)
        discard;
        
    Out.vDiffuse = vMtrlDiffuse * g_Color;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}


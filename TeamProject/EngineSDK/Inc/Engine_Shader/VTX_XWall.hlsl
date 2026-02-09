#include "Shader_Define.hlsl"

vector g_Color;
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
	float3 vPosition    : POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexcoord    : TEXCOORD0;
	float3 vTangent		: TANGENT;
	float3 vBinormal	: BINORMAL;
	
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
	
	float3 localPos =
		In.vPosition.x * In.iRight.xyz +
		In.vPosition.y * In.iUp.xyz +
		In.iTranslation.xyz;

	// 부모 월드 적용
	float4 worldPos = mul(float4(localPos, 1.f), g_WorldMatrix);

	// 최종 클립 공간
	matrix VP = mul(g_ViewMatrix, g_ProjMatrix);
	Out.vWorldPos = mul(worldPos, VP);

	// UV 그대로
	Out.vTexcoord = In.vTexcoord;

	// 밝기 전달
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
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.2)
    {
        discard;
    }
    
    Out.vDiffuse = vMtrlDiffuse * g_Color;
  
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_NoCull);
        SetDepthStencilState(DSS_None,1);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}


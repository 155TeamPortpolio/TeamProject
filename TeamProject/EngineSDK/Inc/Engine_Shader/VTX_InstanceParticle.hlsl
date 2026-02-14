#include "Shader_Define.hlsl"

float4x4 g_WorldMatrix;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal	 : NORMAL;
	float2 vTexcoord : TEXCOORD0;
	float3 vTangent  : TANGENT;
	float3 vBinormal : BINORMAL;
    
	float4 iRight		: INSTANCE0;
	float4 iUp			: INSTANCE1;
	float4 iLook		: INSTANCE2;
	float4 iTranslation : INSTANCE3;
	float4 iColor		: INSTANCE4;
	float2 iLifetime	: INSTANCE5;
};

struct VS_OUT
{
	float4 vWorldPos : SV_POSITION;
	float4 vNormal	 : NORMAL;
	float3 vTangent  : TANGENT;
	float3 vBinormal : BINORMAL;
    
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos  : TEXCOORD2;
	float  viewZ	 : TEXCOORD3;
	float4 iColor	 : TEXCOORD4;
	float2 iLifetime : TEXCOORD5;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    row_major float4x4 instLocal = float4x4(In.iRight, In.iUp, In.iLook, In.iTranslation);

    float4x4 worldMat = mul(instLocal, g_WorldMatrix);

    float4 localPos = float4(In.vPosition, 1.f);
    float4 worldPos = mul(localPos, worldMat);

    matrix VP = mul(matView, matProjection);

    Out.vWorldPos = mul(worldPos, VP);
    Out.vProjPos = Out.vWorldPos;

    float3 worldNormal = mul(float4(In.vNormal, 0.f), worldMat).xyz;
    Out.vNormal = float4(normalize(worldNormal), 1);

    Out.vTexcoord = In.vTexcoord;

    float4 viewPos = mul(worldPos, matView);
    Out.viewZ = viewPos.z;

    Out.iColor = In.iColor;
    Out.iLifetime = In.iLifetime;

    return Out;
}

struct PS_IN
{
	float4 vWorldPos : SV_POSITION;
	float4 vNormal	 : NORMAL;
	float3 vTangent  : TANGENT;
	float3 vBinormal : BINORMAL;
    
	float2 vTexcoord : TEXCOORD0;
	float4 vProjPos  : TEXCOORD2;
	float  viewZ	 : TEXCOORD3;
	float4 iColor	 : TEXCOORD4;
	float2 iLifetime : TEXCOORD5;
};

struct PS_OUT
{
	vector vDiffuse : SV_TARGET0;
	vector vNormal  : SV_TARGET1;
	vector vDepth	: SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out;
    
	vector vMtrlDiffuse = DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
	if (vMtrlDiffuse.a < 0.2f)
		discard;
        
	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = normalize(In.vNormal);
    
	float linearDepth = saturate(In.viewZ / zFar);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, linearDepth, 1.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}


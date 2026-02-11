#include "Shader_Define.hlsl"

vector g_Color = { 0.8f, 0.35f, 0.1f, 1.f };
vector g_BrightColor = { 1.f, 0.0f, 0.0f, 1.f };
float4x4 g_WorldMatrix;

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
    float4 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    
    float2 vTexcoord    : TEXCOORD0;
    float fBrightness   : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
    float viewZ         : TEXCOORD3;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;
    
    row_major float4x4 instLocal= float4x4(In.iRight, In.iUp, In.iLook, In.iTranslation);
    
    float4 instanceOrigin = mul(float4(0, 0, 0, 1), instLocal);
    float4 worldOrigin = mul(instanceOrigin, g_WorldMatrix);
    
    float3 camRight = matViewInverse[0].xyz;
    float3 camUp = matViewInverse[1].xyz;

    float scale = 0.2f;
    
    float3 billboardPos =
    worldOrigin.xyz +
    camRight * (In.vPosition.x * scale) +
    camUp * (In.vPosition.y * scale);

    float4 worldPos = float4(billboardPos, 1.f);

    matrix VP = mul(matView, matProjection);
    
    Out.vWorldPos = mul(worldPos, VP);
    Out.vNormal = mul(vector(In.vNormal, 0.f), ObjectBufferArray[TransformIndex].Transform);
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), ObjectBufferArray[TransformIndex].Transform)).xyz;
    Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
    
    Out.vTexcoord = In.vTexcoord;
	Out.fBrightness = In.fBrightness;
    Out.vProjPos = Out.vWorldPos;
    
    float4 viewPos = mul(worldPos, matView);
    Out.viewZ = viewPos.z;
    
	return Out;
}

struct PS_IN
{
    float4 vWorldPos    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    
    float2 vTexcoord    : TEXCOORD0;
    float  fBrightness  : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
    float  viewZ        : TEXCOORD3;
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
    if (vMtrlDiffuse.a < 0.9f)
        discard;
        
    Out.vDiffuse = vMtrlDiffuse * g_Color;    
    Out.vNormal = normalize(In.vNormal);
    
    float linearDepth = saturate(In.viewZ / zFar);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / zFar, linearDepth, 1.f);
    Out.vEmissive = float4(g_BrightColor.rgb * In.fBrightness, 1.f);
    Out.fEmissiveInfo = float2(0.f, 1.f);
    
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


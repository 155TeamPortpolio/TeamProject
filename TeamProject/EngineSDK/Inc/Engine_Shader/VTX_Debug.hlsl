#include "Shader_Define.hlsl"

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_Position; 
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    matrix matWV,matWVP;
    matWV = mul(ObjectBufferArray[TransformIndex].Transform, matView);
    matWVP = mul(matWV, matProjection);
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    return Out;
}
struct PS_IN
{
    float4 vPosition : SV_Position; 
};

struct PS_OUT
{
    vector vColor : SV_TARGET0; 
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out ;
    Out.vColor = float4(1.f,0.f,0.f,0.f);
    return Out;
}



technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
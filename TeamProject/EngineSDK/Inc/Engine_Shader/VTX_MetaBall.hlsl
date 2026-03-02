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
    float4 vProjPosition : TEXCOORD1;
    float4 vWorldPosition : TEXCOORD2;
    float4 vRayOrigin : TEXCOORD3;
    float4 vRayDir : TEXCOORD4;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    matrix matrixWV = mul(ObjectBufferArray[TransformIndex].Transform, matView);
    matrix matrixWVP = mul(matrixWV, matProjection);
    
    Out.vWorldPosition = mul(float4(In.vPosition, 1.f), ObjectBufferArray[TransformIndex].Transform);
    Out.vPosition = mul(float4(In.vPosition, 1.f), matrixWVP);
    Out.vProjPosition = Out.vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vRayOrigin = vCamPosition;
    //Out.vRayDir = 
    
    return Out;
}


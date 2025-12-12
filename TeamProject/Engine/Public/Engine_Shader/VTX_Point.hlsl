#include "Shader_Define.hlsl"

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vWorldPos : POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), ObjectBufferArray[TransformIndex].Transform);
   
    return Out;
}

struct GS_IN
{
    float4 vWorldPos : POSITION;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> triStream)
{
    GS_OUT v[4];

    float3 worldPos = In[0].vWorldPos.xyz;
    float3 camPos = vCamPosition.xyz;
    float3 worldUp = float3(0.f, 1.f, 0.f);
    
    float3 look = normalize(camPos - worldPos);
    float3 right = normalize(cross(worldUp, look));
    float3 up = normalize(cross(look, right));
    
    float scaleX = length(ObjectBufferArray[TransformIndex].Transform[0]);
    float scaleY = length(ObjectBufferArray[TransformIndex].Transform[1]);

    float3 offsetRight = right * (scaleX * 0.5f);
    float3 offsetUp = up * (scaleY * 0.5f);

    // 정점 4개 위치 계산 (월드 기준)
    float3 p0 = worldPos + (offsetRight + offsetUp);
    float3 p1 = worldPos + (-offsetRight + offsetUp);
    float3 p2 = worldPos + (-offsetRight - offsetUp);
    float3 p3 = worldPos + (offsetRight - offsetUp);
    
    matrix matrixVP = mul(matView, matProjection);
    v[0].vPosition = mul(float4(p0, 1.f), matrixVP);
    v[0].vTexcoord = float2(0, 0);

    v[1].vPosition = mul(float4(p1, 1.f), matrixVP);
    v[1].vTexcoord = float2(1, 0);

    v[2].vPosition = mul(float4(p2, 1.f), matrixVP);
    v[2].vTexcoord = float2(1, 1);

    v[3].vPosition = mul(float4(p3, 1.f), matrixVP);
    v[3].vTexcoord = float2(0, 1);

    triStream.Append(v[0]);
    triStream.Append(v[1]);
    triStream.Append(v[2]);
    triStream.RestartStrip();

    triStream.Append(v[0]);
    triStream.Append(v[2]);
    triStream.Append(v[3]);
    triStream.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vDiffuse = DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
        
    if (vDiffuse.a < 0.1f)
        discard;
    Out.vColor = vDiffuse ;
    return Out;
}

technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }  
}


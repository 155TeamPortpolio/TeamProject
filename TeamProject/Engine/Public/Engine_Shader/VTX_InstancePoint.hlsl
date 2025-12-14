#include "Shader_Define.hlsl"

float4x4 g_WorldMatrix;

struct VS_IN
{
    float3 vPosition : POSITION;
    row_major matrix TransformMatrix : WORLD;
    float3 vVelocity : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
};

struct VS_OUT
{
    float4 vWorldPos : POSITION;
    float2 vSize : PSIZE;
    float3 vVelocity : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4 position = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    Out.vWorldPos = mul(position, g_WorldMatrix);
    Out.vSize = float2(length(In.TransformMatrix._11_12_13), length(In.TransformMatrix._21_22_23));
    Out.vLifeTime = In.vLifeTime;
    
    float t = In.vLifeTime.x / In.vLifeTime.y;
    Out.vVelocity = In.vVelocity;
    Out.vColor = In.vColor;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

struct GS_IN
{
    float4 vWorldPos : POSITION;
    float2 vSize : PSIZE;
    float3 vVelocity : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
};

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
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
    
    float scaleX = In[0].vSize.x;
    float scaleY = In[0].vSize.y;
    
    float3 offsetRight = right * (scaleX * 0.5f);
    float3 offsetUp = up * (scaleY * 0.5f);
    
    float3 p0 = worldPos + (offsetRight + offsetUp);
    float3 p1 = worldPos + (-offsetRight + offsetUp);
    float3 p2 = worldPos + (-offsetRight - offsetUp);
    float3 p3 = worldPos + (offsetRight - offsetUp);
    
    matrix matrixVP = mul(matView, matProjection);
    v[0].vPosition = mul(float4(p0, 1.f), matrixVP);
    v[0].vTexcoord = float2(0, 0);
    v[0].vColor = In[0].vColor;
    v[0].vLifeTime = In[0].vLifeTime;

    v[1].vPosition = mul(float4(p1, 1.f), matrixVP);
    v[1].vTexcoord = float2(1, 0);
    v[1].vColor = In[0].vColor;
    v[1].vLifeTime = In[0].vLifeTime;
    
    v[2].vPosition = mul(float4(p2, 1.f), matrixVP);
    v[2].vTexcoord = float2(1, 1);
    v[2].vColor = In[0].vColor;
    v[2].vLifeTime = In[0].vLifeTime;
    
    v[3].vPosition = mul(float4(p3, 1.f), matrixVP);
    v[3].vTexcoord = float2(0, 1);
    v[3].vColor = In[0].vColor;
    v[3].vLifeTime = In[0].vLifeTime;
    
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
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_Target0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float4 color = DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (color.a < 0.1f)
        discard;
    
    Out.vColor = color * In.vColor;
    
    return Out;
}

technique11 Default
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(1.f, 1.f, 1.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}


#include "Shader_Define.hlsl"
#include "CS_Particle.hlsli"

StructuredBuffer<InstanceData> InstanceDatas;

float4x4 g_WorldMatrix;

uint Col;
uint Row;

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vWorldPos : POSITION;
    float2 vSize : PSIZE;
    float3 vVelocity : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
    uint iFrameIndex : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In, uint InstanceID : SV_InstanceID)
{
    VS_OUT Out;
    
    InstanceData data = InstanceDatas[InstanceID];
    
    matrix TransformMatrix;
    TransformMatrix._11_12_13_14 = data.vRight;
    TransformMatrix._21_22_23_24 = data.vUp;
    TransformMatrix._31_32_33_34 = data.vLook;
    TransformMatrix._41_42_43_44 = data.vTranslate;
    
    float4 position = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    Out.vWorldPos = mul(position, g_WorldMatrix);
    Out.vSize = float2(length(data.vRight), length(data.vUp));
    Out.vLifeTime = data.vLife;
    
    float t = Out.vLifeTime.x / Out.vLifeTime.y;
    Out.vVelocity = data.vVelocity;
    Out.vColor = data.vColor;
    Out.vLifeTime = data.vLife;
    Out.iFrameIndex = data.iFrameIndex;
    
    return Out;
}

struct GS_IN
{
    float4 vWorldPos : POSITION;
    float2 vSize : PSIZE;
    float3 vVelocity : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
    uint iFrameIndex : TEXCOORD3;
};

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
    uint iFrameIndex : TEXCOORD3;
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
    
    float3 dir = normalize(In[0].vVelocity);
    float vx = dot(dir, right);
    float vy = dot(dir, up);
    
    float rotZ = atan2(vy, vx);
    float c = cos(rotZ);
    float s = sin(rotZ);
    
    float scaleX = In[0].vSize.x;
    float scaleY = In[0].vSize.y;
    
    float2 offset[4] =
    {
        float2(scaleX * 0.5f, scaleY * 0.5f),
        float2(-scaleX * 0.5f, scaleY * 0.5f),
        float2(-scaleX * 0.5f, -scaleY * 0.5f),
        float2(scaleX * 0.5f, -scaleY * 0.5f)
    };
    
    for (int i = 0; i < 4; ++i)
    {
        float x = offset[i].x;
        float y = offset[i].y;

        offset[i].x = x * c - y * s;
        offset[i].y = x * s + y * c;
    }
    
    float3 p0 = worldPos + (offset[0].x * right + offset[0].y * up);
    float3 p1 = worldPos + (offset[1].x * right + offset[1].y * up);
    float3 p2 = worldPos + (offset[2].x * right + offset[2].y * up);
    float3 p3 = worldPos + (offset[3].x * right + offset[3].y * up);
    
    matrix matrixVP = mul(matView, matProjection);
    v[0].vPosition = mul(float4(p0, 1.f), matrixVP);
    v[0].vTexcoord = float2(0, 0);
    v[0].vColor = In[0].vColor;
    v[0].vLifeTime = In[0].vLifeTime;
    v[0].iFrameIndex = In[0].iFrameIndex;

    v[1].vPosition = mul(float4(p1, 1.f), matrixVP);
    v[1].vTexcoord = float2(1, 0);
    v[1].vColor = In[0].vColor;
    v[1].vLifeTime = In[0].vLifeTime;
    v[1].iFrameIndex = In[0].iFrameIndex;
    
    v[2].vPosition = mul(float4(p2, 1.f), matrixVP);
    v[2].vTexcoord = float2(1, 1);
    v[2].vColor = In[0].vColor;
    v[2].vLifeTime = In[0].vLifeTime;
    v[2].iFrameIndex = In[0].iFrameIndex;
    
    v[3].vPosition = mul(float4(p3, 1.f), matrixVP);
    v[3].vTexcoord = float2(0, 1);
    v[3].vColor = In[0].vColor;
    v[3].vLifeTime = In[0].vLifeTime;
    v[3].iFrameIndex = In[0].iFrameIndex;
    
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
    uint iFrameIndex : TEXCOORD3;
};

struct PS_OUT
{
    float4 vColor : SV_Target0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float2 FrameSize = float2(1.f / Col, 1.f / Row);
    int iFrameX = In.iFrameIndex % Col;
    int iFrameY = In.iFrameIndex / Col;
    float2 FrameMin = float2(iFrameX, iFrameY) * FrameSize;
    float2 TexCoord = FrameMin + In.vTexcoord * FrameSize;
    
    float4 color = DiffuseTexture.Sample(LinearSampler, TexCoord);
    //if (color.a < 0.1f)
    //    discard;
    
    Out.vColor = lerp(In.vColor, color, color.a);//color;
    Out.vColor.a = color.a;
    //Out.vColor = float4(1.f / In.iFrameIndex, 1.f / Col, 1.f / Row, 1.f);
    
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


#include "Shader_Define.hlsl"

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vWorldPos : POSITION;
};

struct VS_OUT_SCREEN_EFFECT
{
    float4 vProjPos : POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), ObjectBufferArray[TransformIndex].Transform);
   
    return Out;
}

VS_OUT_SCREEN_EFFECT VS_MAIN_SCREEN_EFFECT(VS_IN In)
{
    VS_OUT_SCREEN_EFFECT Out;
    
    matrix matrixWV = mul(ObjectBufferArray[TransformIndex].Transform, matView);
    matrix matrixWVP = mul(matrixWV, matProjection);
    
    Out.vProjPos = mul(float4(In.vPosition, 1.f), matrixWVP);

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

struct GS_IN_SCREEN_EFFECT
{
    float4 vProjPos : POSITION;
};

struct GS_OUT_SCREEN_EFFECT
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

float ScreenWidth;
float ScreenHeight;
float Width;
float Height;
float3 Color;
float Alpha;

[maxvertexcount(24)]
void GS_MAIN_SCREEN_EFFECT(point GS_IN_SCREEN_EFFECT In[1], inout TriangleStream<GS_OUT_SCREEN_EFFECT> triStream)
{
    float projZ = In[0].vProjPos.z;
    float projW = In[0].vProjPos.w;
    float2 vAnchorNDC = In[0].vProjPos.xy / In[0].vProjPos.w;
    float LenNdcX = Width * (2.f / ScreenWidth);
    float LenNdcY = Width * (2.f / ScreenHeight);
    float ThickNdcX = Height * (2.f / ScreenWidth);
    float ThickNdcY = Height * (2.f / ScreenHeight);

    /* Right (가로, 앵커가 왼쪽) */
    {
        GS_OUT_SCREEN_EFFECT Out[4];
        float2 LT = float2(vAnchorNDC.x, vAnchorNDC.y + ThickNdcY * 0.5f);
        float2 RT = float2(vAnchorNDC.x + LenNdcX, vAnchorNDC.y + ThickNdcY * 0.5f);
        float2 RB = float2(vAnchorNDC.x + LenNdcX, vAnchorNDC.y - ThickNdcY * 0.5f);
        float2 LB = float2(vAnchorNDC.x, vAnchorNDC.y - ThickNdcY * 0.5f);

        Out[0].vPosition = float4(LT * projW, projZ, projW);
        Out[0].vTexcoord = float2(0.f, 0.f);
        
        Out[1].vPosition = float4(RT * projW, projZ, projW);
        Out[1].vTexcoord = float2(1.f, 0.f);
        
        Out[2].vPosition = float4(RB * projW, projZ, projW);
        Out[2].vTexcoord = float2(1.f, 1.f);
        
        Out[3].vPosition = float4(LB * projW, projZ, projW);
        Out[3].vTexcoord = float2(0.f, 1.f);
        
        triStream.Append(Out[0]);
        triStream.Append(Out[1]);
        triStream.Append(Out[2]);
        triStream.RestartStrip();
        
        triStream.Append(Out[0]);
        triStream.Append(Out[2]);
        triStream.Append(Out[3]);
        triStream.RestartStrip();
    }

    /* Bottom (세로, 앵커가 위쪽) */
    {
        GS_OUT_SCREEN_EFFECT Out[4];
        float2 LT = float2(vAnchorNDC.x - ThickNdcX * 0.5f, vAnchorNDC.y);
        float2 RT = float2(vAnchorNDC.x + ThickNdcX * 0.5f, vAnchorNDC.y);
        float2 RB = float2(vAnchorNDC.x + ThickNdcX * 0.5f, vAnchorNDC.y - LenNdcY);
        float2 LB = float2(vAnchorNDC.x - ThickNdcX * 0.5f, vAnchorNDC.y - LenNdcY);

        Out[0].vPosition = float4(LT * projW, projZ, projW);
        Out[0].vTexcoord = float2(1.f, 0.f);
        
        Out[1].vPosition = float4(RT * projW, projZ, projW);
        Out[1].vTexcoord = float2(1.f, 1.f);
        
        Out[2].vPosition = float4(RB * projW, projZ, projW);
        Out[2].vTexcoord = float2(0.f, 1.f);
        
        Out[3].vPosition = float4(LB * projW, projZ, projW);
        Out[3].vTexcoord = float2(0.f, 0.f);

        triStream.Append(Out[0]);
        triStream.Append(Out[1]);
        triStream.Append(Out[2]);
        triStream.RestartStrip();
        
        triStream.Append(Out[0]);
        triStream.Append(Out[2]);
        triStream.Append(Out[3]);
        triStream.RestartStrip();
    }

    /* Left (가로, 앵커가 오른쪽) */
    {
        GS_OUT_SCREEN_EFFECT Out[4];
        float2 LT = float2(vAnchorNDC.x - LenNdcX, vAnchorNDC.y + ThickNdcY * 0.5f);
        float2 RT = float2(vAnchorNDC.x, vAnchorNDC.y + ThickNdcY * 0.5f);
        float2 RB = float2(vAnchorNDC.x, vAnchorNDC.y - ThickNdcY * 0.5f);
        float2 LB = float2(vAnchorNDC.x - LenNdcX, vAnchorNDC.y - ThickNdcY * 0.5f);

        Out[0].vPosition = float4(LT * projW, projZ, projW);
        Out[0].vTexcoord = float2(0.f, 0.f);
    
        Out[1].vPosition = float4(RT * projW, projZ, projW);
        Out[1].vTexcoord = float2(1.f, 0.f);
        
        Out[2].vPosition = float4(RB * projW, projZ, projW);
        Out[2].vTexcoord = float2(1.f, 1.f);
        
        Out[3].vPosition = float4(LB * projW, projZ, projW);
        Out[3].vTexcoord = float2(0.f, 1.f);

        triStream.Append(Out[0]);
        triStream.Append(Out[1]);
        triStream.Append(Out[2]);
        triStream.RestartStrip();
        
        triStream.Append(Out[0]);
        triStream.Append(Out[2]);
        triStream.Append(Out[3]);
        triStream.RestartStrip();
    }

    /* Top (세로, 앵커가 아래쪽) */
    {
        GS_OUT_SCREEN_EFFECT Out[4];
        float2 LT = float2(vAnchorNDC.x - ThickNdcX * 0.5f, vAnchorNDC.y + LenNdcY);
        float2 RT = float2(vAnchorNDC.x + ThickNdcX * 0.5f, vAnchorNDC.y + LenNdcY);
        float2 RB = float2(vAnchorNDC.x + ThickNdcX * 0.5f, vAnchorNDC.y);
        float2 LB = float2(vAnchorNDC.x - ThickNdcX * 0.5f, vAnchorNDC.y);

        Out[0].vPosition = float4(LT * projW, projZ, projW);
        Out[0].vTexcoord = float2(1.f, 0.f);
        
        Out[1].vPosition = float4(RT * projW, projZ, projW);
        Out[1].vTexcoord = float2(1.f, 1.f);
        
        Out[2].vPosition = float4(RB * projW, projZ, projW);
        Out[2].vTexcoord = float2(0.f, 1.f);
        
        Out[3].vPosition = float4(LB * projW, projZ, projW);
        Out[3].vTexcoord = float2(0.f, 0.f);

        triStream.Append(Out[0]);
        triStream.Append(Out[1]);
        triStream.Append(Out[2]);
        triStream.RestartStrip();
        
        triStream.Append(Out[0]);
        triStream.Append(Out[2]);
        triStream.Append(Out[3]);
        triStream.RestartStrip();
    }
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

struct PS_OUT_SCREEN_EFFECT
{
    float4 vDiffuseAcc : SV_Target0;
    float4 vBloomAcc : SV_Target1;
    float4 vBloomInfo : SV_Target2;
    float4 vRevealage : SV_Target3;
};

uint Col;
uint Row;
uint FrameIndex;

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vDiffuse = DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
        
    if (vDiffuse.a < 0.1f)
        discard;
    Out.vColor = vDiffuse ;
    return Out;
}

PS_OUT PS_MAIN_SPRITEANIMATION(PS_IN In)
{
    PS_OUT Out;
    
    float2 FrameSize = float2(1.f / Col, 1.f / Row);
    int iFrameX = FrameIndex % Col;
    int iFrameY = FrameIndex / Col;
    float2 FrameMin = float2(iFrameX, iFrameY) * FrameSize;
    float2 TexCoord = FrameMin + In.vTexcoord * FrameSize;
    
    vector vDiffuse = DiffuseTexture.Sample(LinearSampler, TexCoord);
    if (vDiffuse.a < 0.1f)
        discard;
    
    Out.vColor = vDiffuse;
    
    return Out;
}

PS_OUT_SCREEN_EFFECT PS_MAIN_SCREENEFFECT(PS_IN In)
{
    PS_OUT_SCREEN_EFFECT Out;
    
    vector vDiffuse = DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vResult = ApplyColorMode(0, vDiffuse, float4(Color, Alpha));
    vector vPremulColor = vector(vResult.rgb * vResult.a, vResult.a);
    vector vBloomColor = float4(1.f, 0.2f, 0.f, 1.f);
    vector vBloomPremul = float4(vBloomColor.rgb * vResult.a, vResult.a);
    
    Out.vDiffuseAcc = vPremulColor;
    Out.vBloomAcc = ExtractBright(vBloomPremul, 0.3f, 0.2f, 20.f);
    Out.vBloomInfo = float4(0.f, 1.5f, 0.f, 0.f);
    Out.vRevealage = vResult.a;
    
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

    pass SpriteAnimation
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_SPRITEANIMATION();
    }

    pass ScreenEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OITAccmulation, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_SCREEN_EFFECT();
        GeometryShader = compile gs_5_0 GS_MAIN_SCREEN_EFFECT();
        PixelShader = compile ps_5_0 PS_MAIN_SCREENEFFECT();
    }
}


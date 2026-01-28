#include "Shader_Define.hlsl"

#define PI 3.14159265359

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

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

VS_OUT VS_MAIN_CUSTOM(VS_IN In)
{
    VS_OUT Out;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
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
    
    float3 right = normalize(ObjectBufferArray[TransformIndex].Transform[0].xyz);
    float3 up = normalize(ObjectBufferArray[TransformIndex].Transform[1].xyz);
    float scaleX = length(ObjectBufferArray[TransformIndex].Transform[0].xyz);
    float scaleY = length(ObjectBufferArray[TransformIndex].Transform[1].xyz);

    float3 offsetRight = right * ( scaleX*0.5f);
    float3 offsetUp = up * (scaleY * 0.5f);

    // 정점 4개 위치 계산 (월드 기준)
    float3 p0 = worldPos + (-offsetRight + offsetUp);
    float3 p1 = worldPos + (offsetRight + offsetUp);
    float3 p2 = worldPos + (offsetRight - offsetUp);
    float3 p3 = worldPos + (-offsetRight - offsetUp);

    // 직교 투영 사용
    v[0].vPosition = mul(float4(p0, 1.f), matOrthograph);
    v[0].vTexcoord = float2(0, 0);

    v[1].vPosition = mul(float4(p1, 1.f), matOrthograph);
    v[1].vTexcoord = float2(1, 0);

    v[2].vPosition = mul(float4(p2, 1.f), matOrthograph);
    v[2].vTexcoord = float2(1, 1);

    v[3].vPosition = mul(float4(p3, 1.f), matOrthograph);
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

[maxvertexcount(6)]
void GS_MAIN_CUSTOM(point GS_IN In[1], inout TriangleStream<GS_OUT> triStream)
{
    GS_OUT v[4];

    float3 worldPos = In[0].vWorldPos.xyz;
    
    float3 right = normalize(g_WorldMatrix[0].xyz);
    float3 up = normalize(g_WorldMatrix[1].xyz);
    float scaleX = length(g_WorldMatrix[0].xyz);
    float scaleY = length(g_WorldMatrix[1].xyz);

    float3 offsetRight = right * (scaleX * 0.5f);
    float3 offsetUp = up * (scaleY * 0.5f);

    // 정점 4개 위치 계산 (월드 기준)
    float3 p0 = worldPos + (-offsetRight + offsetUp);
    float3 p1 = worldPos + (offsetRight + offsetUp);
    float3 p2 = worldPos + (offsetRight - offsetUp);
    float3 p3 = worldPos + (-offsetRight - offsetUp);

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    // 직교 투영 사용
    v[0].vPosition = mul(float4(p0, 1.f), matVP);
    v[0].vTexcoord = float2(0, 0);

    v[1].vPosition = mul(float4(p1, 1.f), matVP);
    v[1].vTexcoord = float2(1, 0);

    v[2].vPosition = mul(float4(p2, 1.f), matVP);
    v[2].vTexcoord = float2(1, 1);

    v[3].vPosition = mul(float4(p3, 1.f), matVP);
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

float ShearK = 0.f;

[maxvertexcount(6)]
void GS_SHEAR(point GS_IN In[1], inout TriangleStream<GS_OUT> triStream)
{
    GS_OUT v[4];

    float3 worldPos = In[0].vWorldPos.xyz;

    float3 right = normalize(ObjectBufferArray[TransformIndex].Transform[0].xyz);
    float3 up = normalize(ObjectBufferArray[TransformIndex].Transform[1].xyz);
    float scaleX = length(ObjectBufferArray[TransformIndex].Transform[0].xyz);
    float scaleY = length(ObjectBufferArray[TransformIndex].Transform[1].xyz);

    float halfW = scaleX * 0.5f;
    float halfH = scaleY * 0.5f;

    float3 offsetRight = right * halfW;
    float3 offsetUp = up * halfH;

    float3 shiftTop = right * (ShearK * scaleY);
    float3 shiftBot = float3(0.f, 0.f, 0.f);

    float3 p0 = worldPos + (-offsetRight + offsetUp) + shiftTop;
    float3 p1 = worldPos + (offsetRight + offsetUp) + shiftTop;
    float3 p2 = worldPos + (offsetRight - offsetUp) + shiftBot;
    float3 p3 = worldPos + (-offsetRight - offsetUp) + shiftBot;

    v[0].vPosition = mul(float4(p0, 1.f), matOrthograph);
    v[0].vTexcoord = float2(0, 0);

    v[1].vPosition = mul(float4(p1, 1.f), matOrthograph);
    v[1].vTexcoord = float2(1, 0);

    v[2].vPosition = mul(float4(p2, 1.f), matOrthograph);
    v[2].vTexcoord = float2(1, 1);

    v[3].vPosition = mul(float4(p3, 1.f), matOrthograph);
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

uint Col = 1;
uint Row = 1;
uint FrameIndex;

float2 UVOffset;
 
float FillAmount;
float Direction;

float4 vColor;

float2 vFlip;

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
    
    float2 vTexcoord = { In.vTexcoord.x * (1.f - 2.f * vFlip.x) + vFlip.x, In.vTexcoord.y * (1.f - 2.f * vFlip.y) + vFlip.y };
    
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, vTexcoord);
    //clip(vDiffuse.a - 0.1f);
    
    float4 color = vDiffuse * vColor;
    Out.vColor.rgb = color.rgb * color.a;
    Out.vColor.a = color.a;
    
    return Out;
}

Texture2D ColorTexture;

uint ColorCol = 1;
uint ColorRow = 1;
uint ColorFrameIndex = 0;

PS_OUT PS_MAIN_SPRITEANIMATION_COLORATLAS(PS_IN In)
{
    PS_OUT Out;

    float2 uvDigit = CalculateFrameIndex(Col, Row, FrameIndex, In.vTexcoord);
    vector digit = SpriteTexture.Sample(LinearSampler, uvDigit);
    clip(digit.a - 0.1f);

    float2 uvColor = CalculateFrameIndex(ColorCol, ColorRow, ColorFrameIndex, In.vTexcoord);
    vector grad = ColorTexture.Sample(LinearSampler, uvColor);

    float4 color = (digit * grad) * vColor;
    Out.vColor.rgb = color.rgb * color.a;
    Out.vColor.a = color.a;
    return Out;
}

PS_OUT PS_MAIN_SPRITEANIMATION(PS_IN In)
{    
    PS_OUT Out;
    
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, CalculateFrameIndex(Col, Row, FrameIndex, In.vTexcoord));
    clip(vDiffuse.a - 0.1f);
    
    float4 color = vDiffuse * vColor;
    Out.vColor.rgb = color.rgb * color.a;
    Out.vColor.a = color.a;
    
    return Out;
}

PS_OUT PS_MAIN_UVANIMATION(PS_IN In)
{
    PS_OUT Out;
    
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, In.vTexcoord + UVOffset);
    clip(vDiffuse.a - 0.1f);
    
    float4 color = vDiffuse * vColor;
    Out.vColor.rgb = color.rgb * color.a;
    Out.vColor.a = color.a;
    
    return Out;
}

PS_OUT PS_MAIN_LINEARFILL(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = { In.vTexcoord.x * (1.f - 2.f * Direction) + Direction, In.vTexcoord.y };
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, In.vTexcoord);
    clip(vDiffuse.a - 0.1f);
    
    clip(FillAmount - vTexcoord.x);
    
    float4 color = vDiffuse * vColor;
    Out.vColor.rgb = color.rgb * color.a;
    Out.vColor.a = color.a;
    
    return Out;
}

PS_OUT PS_MAIN_RADIALFILL(PS_IN In)
{
    PS_OUT Out;
    
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, In.vTexcoord);
    clip(vDiffuse.a - 0.1f);
    
    float2 vTexcoord = In.vTexcoord - 0.5f;
    float  fAngle    = atan2(vTexcoord.y, vTexcoord.x);
    fAngle = fAngle / (PI * 2.f) + 0.5f; // 0 ~ 1로 정규화
    fAngle = (1.f - Direction) - frac(fAngle - 0.25f) * (Direction * -2.f + 1.f);
    
    clip(FillAmount - fAngle);
    
    float4 color = vDiffuse * vColor;
    Out.vColor.rgb = color.rgb * color.a;
    Out.vColor.a = color.a;
    
    return Out;
}
// ---------------------------------------------------------------------------------------
float MaskThreshold = 0.1f;

PS_OUT PS_STENCIL_WRITE_ALPHA(PS_IN In)
{
    PS_OUT Out;

    float2 vTexcoord = float2(
        In.vTexcoord.x * (1.f - 2.f * vFlip.x) + vFlip.x,
        In.vTexcoord.y * (1.f - 2.f * vFlip.y) + vFlip.y
    );

    vector vDiffuse = SpriteTexture.Sample(LinearSampler, vTexcoord);
    if (MaskThreshold > 0)
        clip(vDiffuse.a - MaskThreshold);           // fThreshold에 양수를 넣으면 그 절대값보다 작은 값을 자름
    else
        clip((MaskThreshold * -1.f)  - vDiffuse.a); // fThreshold에 음수를 넣으면 그 절대값보다 큰 값을 자름
    
    Out.vColor = 1;
    return Out;
}

float MaskPreviewAlpha = 0.5f;

PS_OUT PS_MAIN_MASKPREVIEW(PS_IN In)
{
    PS_OUT Out;

    float2 vTexcoord = float2(
        In.vTexcoord.x * (1.f - 2.f * vFlip.x) + vFlip.x,
        In.vTexcoord.y * (1.f - 2.f * vFlip.y) + vFlip.y
    );

    vector vDiffuse = SpriteTexture.Sample(LinearSampler, vTexcoord);
    clip(vDiffuse.a - 0.1f);

    Out.vColor = (vDiffuse * vColor) * MaskPreviewAlpha;
    return Out;
}

/* 9Slice 변수 */
float2 vSizePx;     // 사각형 크기 (픽셀)
float2 vTopLeftPx;  // 사각형의 왼쪽 상단 모서리 (픽셀)

float4 vBorderPx;   // left, right, top, bottom (픽셀)

float4 uvRangeX;    // L0, L1, R0, R1 (CPU에서 계산해서 넘김)
float4 uvRangeY;    // T0, T1, B0, B1 (CPU에서 계산해서 넘김)

float remap(float x, float inMin, float inMax, float outMin, float outMax)
{
    return outMin + (x - inMin) / max((inMax - inMin), 0.0001f) * (outMax - outMin);
}

float calcU(float x)
{
    if (x < vBorderPx.x)
        return remap(x, 0, vBorderPx.x, uvRangeX.x, uvRangeX.y);
    else if (x > vSizePx.x - vBorderPx.y)
        return remap(x, vSizePx.x - vBorderPx.y, vSizePx.x, uvRangeX.z, uvRangeX.w);
    else
        return remap(x, vBorderPx.x, vSizePx.x - vBorderPx.y, uvRangeX.y, uvRangeX.z);
}

float calcV(float y)
{
    if (y < vBorderPx.z)
        return remap(y, 0, vBorderPx.z, uvRangeY.x, uvRangeY.y);
    else if (y > vSizePx.y - vBorderPx.w)
        return remap(y, vSizePx.y - vBorderPx.w, vSizePx.y, uvRangeY.z, uvRangeY.w);
    else
        return remap(y, vBorderPx.z, vSizePx.y - vBorderPx.w, uvRangeY.y, uvRangeY.z);
}

PS_OUT PS_MAIN_NINESLICE(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = In.vTexcoord;
    
    vTexcoord.x = calcU(In.vPosition.x - vTopLeftPx.x);
    vTexcoord.y = calcV(In.vPosition.y - vTopLeftPx.y);
    
    vector vDiffuse = SpriteTexture.Sample(DefaultSampler, vTexcoord);
    clip(vDiffuse.a - 0.1f);
    
    float4 color = vDiffuse * vColor;
    Out.vColor.rgb = color.rgb * color.a;
    Out.vColor.a = color.a;
    
    return Out;
}

PS_OUT PS_MAIN_CUSTOM(PS_IN In)
{
    PS_OUT Out;
    
    float2 vTexcoord = { In.vTexcoord.x * (1.f - 2.f * vFlip.x) + vFlip.x, In.vTexcoord.y * (1.f - 2.f * vFlip.y) + vFlip.y };
    
    vector vDiffuse = SpriteTexture.Sample(LinearSampler, vTexcoord);
    clip(vDiffuse.a - 0.1f);
    
    float4 color = vDiffuse * vColor;
    Out.vColor.rgb = color.rgb * color.a;
    Out.vColor.a = color.a;
    
    return Out;
}

// -------------------------------------------------------------------------------------
technique11 DefaultTechnique
{
    pass Opaque
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN();
    }

    pass SpriteAnimation
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN_SPRITEANIMATION();
    }

    pass UVAnimation
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN_UVANIMATION();
    }

    pass LinearFill
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN_LINEARFILL();
    }

    pass RadialFill
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN_RADIALFILL();
    }

    pass UI_MaskPreview
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN_MASKPREVIEW();
    }

    pass UI_StencilWrite
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_UIWriteStencil, 1);
        SetBlendState(BS_ColorWriteOff, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_STENCIL_WRITE_ALPHA();
    }

    pass UI_StencilWritePreview
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_UIWriteStencil, 1);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN_MASKPREVIEW();
    }

    pass Opaque_StencilTest
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_UIStencilTest, 1);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN();
    }

    pass UVAnimation_StencilTest
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_UIStencilTest, 1);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN_UVANIMATION();
    }

    pass LinearFill_StencilTest
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_UIStencilTest, 1);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN_LINEARFILL();
    }

    pass RadialFill_StencilTest
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_UIStencilTest, 1);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN_RADIALFILL();
    }

    pass SpriteAnimation_StencilTest
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_UIStencilTest, 1);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader    = compile ps_5_0 PS_MAIN_SPRITEANIMATION();
    }

    pass NineSlice
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader  = compile vs_5_0 VS_MAIN();
        GeometryShader= compile gs_5_0 GS_MAIN();
        PixelShader   = compile ps_5_0 PS_MAIN_NINESLICE();
    }

    pass SpriteAnimation_ColorAtlas
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_SHEAR();
        PixelShader    = compile ps_5_0 PS_MAIN_SPRITEANIMATION_COLORATLAS();
    }
    
    pass OpaqueCustom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Premultiplied, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader   = compile vs_5_0 VS_MAIN_CUSTOM();
        GeometryShader = compile gs_5_0 GS_MAIN_CUSTOM();
        PixelShader    = compile ps_5_0 PS_MAIN_CUSTOM();
    }
}